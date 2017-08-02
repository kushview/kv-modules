
#define LOG_FORMAT_INFO 0

class FFmpegFrameQueue
{
public:
    typedef std::pair<uint64_t, AVFrame*> Frame;
    typedef std::vector<Frame> FIFO;
    
    FFmpegFrameQueue (const int capacity)
        : fifo (capacity)
    {
        frames.resize (static_cast<size_t> (capacity),
                       std::make_pair (0, nullptr));
        for (int i = 0; i < capacity; ++i)
            frames[i].second = av_frame_alloc();

        reset();
    }
    
    ~FFmpegFrameQueue()
    {
        reset();
        
        for (auto& frame : frames)
            av_frame_free (&frame.second);
        
        frames.clear();
    }
    
    void reset()
    {
        fifo.reset();
        for (auto& f : frames)
            f.first = 0;
    }
    
    int canWrite() const                        { return fifo.getFreeSpace() > 0; }
    int canRead() const                         { return fifo.getNumReady() > 0; }
    int getNumReady() const                     { return fifo.getNumReady(); }
    int getNumFree() const                      { return fifo.getFreeSpace(); }
    int getTotalSize() const                    { return fifo.getTotalSize(); }
    
    /** Prints information about the buffer to the console */
    void dump()
    {
        DBG("num ready: " << getNumReady());
        DBG("num avail: " << getNumFree());
        DBG("-----");
    }
    
    /** Returns the current frame for reading */
    AVFrame* getReadFrame() const
    {
        int i1, b1, i2, b2;
        fifo.prepareToRead (1, i1, b1, i2, b2);
        return (i1 >= 0 && b1 > 0) ? frames[i1].second :
               (i2 >= 0 && b2 > 0) ? frames[i2].second : nullptr;
    }
    
    /** Returns the current frame for writing */
    AVFrame* getWriteFrame() const
    {
        int i1, b1, i2, b2;
        fifo.prepareToWrite (1, i1, b1, i2, b2);
        return (i1 >= 0 && b1 > 0) ? frames[i1].second :
               (i2 >= 0 && b2 > 0) ? frames[i2].second : nullptr;
    }
    
    /** Call this after a write frame is updated and read to be read */
    void finishedWrite() const
    {
        fifo.finishedWrite (1);
    }
    
    /** Call this after reading frame and it isn't needed anymore */
    void finishedRead() const
    {
        fifo.finishedRead (1);
    }
    
private:
    FIFO frames;
    mutable AbstractFifo fifo;
};

class FFmpegStreamQueue
{
public:
    FFmpegStreamQueue (const int audioSize = 4096,
                       const int videoSize = 16,
                       const int subtitleSize = 8)
        : audio (audioSize), video (videoSize),
          subtitle (subtitleSize)
    { }
    
    ~FFmpegStreamQueue()
    {
        audio.reset();
        video.reset();
        subtitle.reset();
    }

    FFmpegFrameQueue audio;
    FFmpegFrameQueue video;
    FFmpegFrameQueue subtitle;
};

struct FFmpegDecoder::Pimpl : public FFmpegDecoder::Sink
{
    Pimpl (FFmpegDecoder& d, FFmpegStreamQueue* q)
        : decoder           (d),
          format            (nullptr),
          audio             (nullptr),
          video             (nullptr),
          subtitle          (nullptr),
          audioStream       (-1),
          videoStream       (-1),
          subtitleStream    (-1)
    {
        audioFrame = videoFrame = nullptr;
        
        if (q == nullptr)
        {
            queue.setOwned (new FFmpegStreamQueue());
        }
        else
        {
            queue.setNonOwned (q);
        }
    }
    
    ~Pimpl()
    {
        if (videoFrame)
            av_frame_free (&videoFrame);
        if (audioFrame)
            av_frame_free (&audioFrame);
    }
    
    FFmpegDecoder::Sink& sink()
    {
        return decoder.sink != nullptr ? *decoder.sink : *this;
    }
    
    bool openFile (const File& file)
    {
        if (nullptr != format)
            close();
        
        // open input file, and allocate format context
        const int ret = avformat_open_input (&format, file.getFullPathName().toRawUTF8(),
                                             nullptr, nullptr);
        if (ret < 0)
        {
            DBG ("[KV] ffmpeg: opening file for decoding failed");
            close();
            return false;
        }
        
        // retrieve stream information
        if (avformat_find_stream_info (format, nullptr) < 0)
        {
            DBG ("[KV] ffmpeg: stream info not found for decoding");
            close();
            return false;
        }
        
        videoStream = openCodecContext (&video, AVMEDIA_TYPE_VIDEO, true);
        if (isPositiveAndBelow (videoStream, static_cast<int> (format->nb_streams)))
        {
            DBG("[KV] ffmpeg: video stream opened: " << videoStream);
            // noop
        }
        
        audioStream = openCodecContext (&audio, AVMEDIA_TYPE_AUDIO, true);
        if (isPositiveAndBelow (audioStream, static_cast<int> (format->nb_streams)))
        {
            DBG("[KV] ffmpeg: audio stream opened: " << audioStream);
            audio->request_sample_fmt = AV_SAMPLE_FMT_FLTP;
        }
        
       #if LOG_FORMAT_INFO
        av_dump_format (format, 0, file.getFullPathName().toRawUTF8(), 0);
       #endif
        
        return true;
    }
    
    void close()
    {
        if (audioStream >= 0)
        {
            audioStream = -1;
            avcodec_free_context (&audio);
            audio = nullptr;
        }
        
        if (videoStream >= 0)
        {
            videoStream = -1;
            avcodec_free_context (&video);
            video = nullptr;
        }
        
        if (subtitleStream >= 0)
        {
            subtitleStream = -1;
            avcodec_free_context (&subtitle);
            subtitle = nullptr;
        }
        
        if (format != nullptr)
            avformat_close_input (&format);
    }
    
    bool read (AVFrame* dst = nullptr)
    {
        if (nullptr == format)
            return false;
        
        AVPacket packet;
        packet.data = nullptr;
        packet.size = 0;
        av_init_packet (&packet);
        
        int error = av_read_frame (format, &packet);
        if (error >= 0)
        {
            if (packet.stream_index == audioStream)
            {
                decodeAudioPacket (&packet, queue->audio.getWriteFrame());
            }
            
            else if (packet.stream_index == videoStream)
            {
                if (decodeVideoPacket (&packet, queue->video.getWriteFrame()) > 0)
                    queue->video.finishedWrite();
            }
        }
        
        av_packet_unref (&packet);
        return error >= 0;
    }
    
    AVStream* getVideoStream() const
    {
        return (format && isPositiveAndBelow (videoStream, static_cast<int> (format->nb_streams)))
            ? format->streams[videoStream] : nullptr;
    }
    
private:
    friend class FFmpegDecoder;
    FFmpegDecoder& decoder;
    
    AVFormatContext* format;
    AVCodecContext* audio, *video, *subtitle;
    int audioStream, videoStream, subtitleStream;
    AVFrame* audioFrame, *videoFrame;
    OptionalScopedPointer<FFmpegStreamQueue> queue;
    
    /** Opens a context for reading. Returns the stream index */
    int openCodecContext (AVCodecContext** decoderContext, AVMediaType type, bool refCounted)
    {
        AVCodec *decoder = nullptr;
        AVDictionary *opts = nullptr;
        
        int id = av_find_best_stream (format, type, -1, -1, nullptr, 0);
        
        if (isPositiveAndBelow (id, static_cast<int> (format->nb_streams)))
        {
            AVStream* const stream = format->streams[id];
            
            // find decoder for the stream
            decoder = avcodec_find_decoder (stream->codecpar->codec_id);
            if (! decoder)
            {
                DBG ("Failed to find " + String (av_get_media_type_string (type)) + " codec");
                return -1;
            }
            
            // Allocate a codec context for the decoder
            *decoderContext = avcodec_alloc_context3 (decoder);
            if (! *decoderContext)
            {
                DBG ("Failed to allocate the " + String (av_get_media_type_string(type)) +
                     " codec context");
                return -1;
            }
            
            // Copy codec parameters from input stream to output codec context
            if (avcodec_parameters_to_context (*decoderContext, stream->codecpar) < 0)
            {
                DBG ("Failed to copy " + String (av_get_media_type_string(type)) +
                     " codec parameters to decoder context");
                return -1;
            }
            
            // Init the decoders, with or without reference counting
            av_dict_set (&opts, "refcounted_frames", refCounted ? "1" : "0", 0);
            if (avcodec_open2 (*decoderContext, decoder, &opts) < 0)
            {
                DBG ("Failed to open " + String (av_get_media_type_string(type)) + " codec");
                avcodec_free_context (decoderContext);
                return -1;
            }
            
            return id;
        }
        else
        {
            DBG ("Could not find " + String (av_get_media_type_string(type)) +
                 " stream in input file");
            return -1;
        }
    }
    
    /** Decodes an audio packet, returns the number of samples decoded */
    int decodeAudioPacket (AVPacket* packet, AVFrame* frame)
    {
        int gotFrame = 0;
        int decoded = packet->size;
        int result = 0;
        
        // decode audio frame
        do {
            // call decode until packet is empty, result is num bytes decoded
            result = avcodec_decode_audio4 (audio, frame, &gotFrame, packet);
            
            decoded = jmin (result, packet->size);
            packet->data += decoded;
            packet->size -= decoded;
            
            if (gotFrame > 0 && decoded > 0 && frame->extended_data != nullptr)
            {
                sink().audioFrameDecoded (format->streams[audioStream], frame);
                queue->audio.finishedWrite();
            }

        } while (gotFrame > 0 && packet->size > 0);
        
        return result;
    }
    
    /** Decodes a video packet, returns the result of avcodec_decode_video2 */
    int decodeVideoPacket (AVPacket* packet, AVFrame* frame)
    {
        if (video == nullptr || packet->size <= 0)
            return 0.0;
        
        int gotPicture = 0;
        int result = avcodec_decode_video2 (video, frame, &gotPicture, packet);
        
        if (result > 0)
        {
            // we only really got a frame if the timestamp is valid? -MRF
            if (frame->best_effort_timestamp >= 0)
                sink().videoFrameDecoded (format->streams[videoStream], frame);
            else
                result = 0;
        }
        
        if (result == 0)
        {
            // noop, no frame acquired
        }
        else
        {
            // AVError produced
        }
        
        return result;
    }
};

FFmpegDecoder::FFmpegDecoder (Sink* s, FFmpegStreamQueue* q)
    : sink(s)
{
    pimpl = new Pimpl (*this, q);
}

FFmpegDecoder::~FFmpegDecoder()
{
    pimpl = nullptr;
}

bool FFmpegDecoder::openFile (const File& file)     { return pimpl->openFile (file); }
void FFmpegDecoder::close()                         { pimpl->close(); }
void FFmpegDecoder::read()                          { pimpl->read(); }

int FFmpegDecoder::getWidth() const { return 1920; }
int FFmpegDecoder::getHeight() const { return 1080; }

AVPixelFormat FFmpegDecoder::getPixelFormat() const
{
    if (pimpl && pimpl->video)
        return pimpl->video->pix_fmt;
    return AV_PIX_FMT_NONE;
}

class FFmpegVideoSource::Pimpl : public FFmpegDecoder::Sink,
                                 public Thread
{
public:
    Pimpl()
        : FFmpegDecoder::Sink(),
          Thread ("ffmpeg video source"),
          audioOut (1, 1)
    {
        decoder = new FFmpegDecoder (this, &queue);
        frameIndex = 0;
        image = Image (Image::RGB, 640, 360, true);
        startThread();
    }
    
    ~Pimpl()
    {
        stopThread (100);
        decoder->setSink (nullptr);
        decoder->close();
        decoder = nullptr;
    }
    
    void run() override
    {
        while (! threadShouldExit())
        {
            sem.wait();
            
            if (decoder->getPixelFormat() == AV_PIX_FMT_NONE)
                continue;
            
            if (queue.video.getNumFree() > 4 || queue.audio.getNumFree() > 20)
                decoder->read();
        }
        
        DBG("[KV] ffmpeg: video source thread exited");
    }
    
    void tick()
    {
        if (queue.video.canRead())
        {
            auto* frame = queue.video.getReadFrame();
            scale.convertFrameToImage (image, frame);
            queue.video.finishedRead();
            av_frame_unref (frame);
        }

        if (queue.audio.canRead())
        {
            auto* frame = queue.audio.getReadFrame();
            
            const int channels = av_get_channel_layout_nb_channels (frame->channel_layout);
            const int numSamples = frame->nb_samples;
            AudioSampleBuffer audio ((float* const*) frame->extended_data,
                                     channels, 0, numSamples);
            
           #if DEBUG_LOG_AUDIO_PACKETS
            DBG("decoded audio: " << "channels: "   << audio.getNumChannels()
                                  << " samples: "   << audio.getNumSamples()
                                  << " pts: "       << frame->pts
                                  << " dts: "       << frame->pkt_dts
                                  << " best effort: " << frame->best_effort_timestamp
                                  << " level: "     << audio.getRMSLevel(0, 0, numSamples));
           #endif

            if (numSamples <= audioOut.getFreeSpace())
            {
                audioOut.addToFifo (audio);
            }
            else
            {
                // DBG("[KV] ffmpeg: audio buffer overflow in decoding");
            }
            
            queue.audio.finishedRead();
            av_frame_unref (frame);
        }
        
        sem.post();
    }
    
    void openFile (const File& file)
    {
        decoder->openFile (file);
        
        audioOut.setSize (2, 32 * 1024);
        
        DBG("setup input: " << decoder->getWidth() << "x" << decoder->getHeight());
        scale.setupScaler (decoder->getWidth(),
                           decoder->getHeight(),
                           decoder->getPixelFormat(),
                           640, 360, AV_PIX_FMT_BGR0);

        sem.post();
    }
    
    void close()
    {
        decoder->close();
        scale.reset();
        audioOut.setSize (1, 1);
    }
    
    void videoFrameDecoded (const AVStream* stream, AVFrame* frame) override
    {

    }
    
    void audioFrameDecoded (const AVStream* stream, AVFrame* frame) override
    {

    }
    
    void subtitleFrameDecoded (const AVStream* stream, AVFrame*) override { }
    
    Element::Semaphore sem;
    
    void stop()
    {
        if (isThreadRunning())
        {
            signalThreadShouldExit();
            sem.post();
            stopThread (100);
        }
    }
    
private:
    ScopedPointer<FFmpegDecoder> decoder;
    FFmpegStreamQueue queue;
    FFmpegVideoScaler scale;
    int frameIndex;
    Image image;
    AudioRingBuffer<float> audioOut;
    
    friend class FFmpegVideoSource;
};

FFmpegVideoSource::FFmpegVideoSource()
{
    pimpl = new Pimpl();
}

FFmpegVideoSource::~FFmpegVideoSource()
{
    pimpl->stop();
    pimpl = nullptr;
}

void FFmpegVideoSource::tick()
{
    pimpl->tick();
}

void FFmpegVideoSource::openFile (const File& file)
{
    pimpl->openFile (file);
}

Image FFmpegVideoSource::findImage (double pts)
{
    return pimpl->image;
}

void readSamples (AudioSampleBuffer& buffer) {
    
}
