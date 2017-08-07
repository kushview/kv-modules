/*
    This file is part of the Kushview Modules for JUCE
    Copyright (C) 2017  Kushview, LLC.  All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#define LOG_FORMAT_INFO 0
#define LOG_STREAM_INFO 0
#define DEBUG_LOG_AUDIO_PACKETS 0

struct FFmpegRational : public Rational {
    explicit FFmpegRational (const AVRational& avr)
        : Rational (avr.num, avr.den) { }
};

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
        
        videoStream = openCodecContext (&video, AVMEDIA_TYPE_VIDEO, false);
        if (isPositiveAndBelow (videoStream, static_cast<int> (format->nb_streams)))
        {
            const AVStream* const stream = getVideoStream();
            const double duration = static_cast<double> (stream->duration);
            const AVRational frameRate = av_stream_get_r_frame_rate (stream);
            const AVRational timeBase  = stream->time_base;
            
           #if LOG_STREAM_INFO
            DBG("[KV] ffmpeg: video stream opened: " << videoStream);
            DBG("[KV] ffmpeg:   dimensions: " << video->width << "x" << video->height);
            DBG("[KV] ffmpeg:   frame rate: " << frameRate.num << " / " << frameRate.den);
            DBG("[KV] ffmpeg:   time base: " << timeBase.num << " / " << timeBase.den);
            DBG("[KV] ffmpeg:   duration (seconds): " << duration * av_q2d (stream->time_base));
            DBG("[KV] ffmpeg:   num frames: " << stream->nb_frames);
            DBG("[KV] ffmpeg:   tick size: " << stream->duration / stream->nb_frames);
            DBG("[KV] ffmpeg:   avg rate: " << stream->avg_frame_rate.num << " / " << stream->avg_frame_rate.den);
            DBG("[KV] ffmpeg:   int tb: " << (int)av_q2intfloat(stream->time_base));
           #endif
        }
        
        audioStream = openCodecContext (&audio, AVMEDIA_TYPE_AUDIO, false);
        if (isPositiveAndBelow (audioStream, static_cast<int> (format->nb_streams)))
        {
            const AVStream* const stream = getAudioStream();
            const double duration = static_cast<double> (stream->duration);
            const AVRational rate = av_stream_get_r_frame_rate(stream);
            const AVRational codec_tb = stream->time_base;
            
           #if LOG_STREAM_INFO
            DBG("");
            DBG("[KV] ffmpeg: audio stream opened: " << audioStream);
            audio->request_sample_fmt = AV_SAMPLE_FMT_FLTP;
            DBG("[KV] ffmpeg:   samplerate: " << audio->sample_rate);
            DBG("[KV] ffmpeg:   real: " << rate.num << " / " << rate.den);
            DBG("[KV] ffmpeg:   timebase: " << codec_tb.num << " / " << codec_tb.den);
            DBG("[KV] ffmpeg:   duration: " << duration * av_q2d(stream->time_base));
            DBG("[KV] ffmpeg:   tick size: " << stream->duration / stream->nb_frames);
           #endif
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
    
    bool read()
    {
        if (nullptr == format)
            return false;
        
        AVPacket packet;
        packet.data = nullptr;
        packet.size = 0;
        av_init_packet (&packet);
        
        int error = av_read_frame (format, &packet);
        
        if (error == 0)
        {
            if (packet.stream_index == audioStream)
            {
                error = decodeAudioPacket (&packet);
            }
            
            else if (packet.stream_index == videoStream)
            {
                if (decodeVideoPacket (&packet, queue->video.getWriteFrame()) > 0)
                    queue->video.finishedWrite();
            }
        }
        
        av_packet_unref (&packet);
        return error == 0;
    }
    
    AVStream* getAudioStream() const
    {
        return (format && isPositiveAndBelow (videoStream, static_cast<int> (format->nb_streams)))
            ? format->streams[audioStream] : nullptr;
    }
    
    AVStream* getVideoStream() const
    {
        return (format && isPositiveAndBelow (videoStream, static_cast<int> (format->nb_streams)))
            ? format->streams[videoStream] : nullptr;
    }
    
    /** Returns the width of the decoded video stream. Zero if no video */
    int getWidth() const        { return (nullptr != video ? video->width : 0); }
    
    /** Returns the height of the decoded video stream. Zero if no video*/
    int getHeight() const       { return (nullptr != video ? video->height : 0); }
    
    /** Returns the sample rate of the audio stream. Zero if no audio */
    double getSampleRate() const
    {
        return (nullptr != audio) ? static_cast<double> (audio->sample_rate)
                                  : 0.0;
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
        
        if (isPositiveAndBelow (id, static_cast<int>(format->nb_streams)))
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
            DBG ("Could not find " + String (av_get_media_type_string (type)) +
                 " stream in input file");
            return -1;
        }
    }
    
    /** Decodes an audio packet
        Returns an AVError on failure or zero on success 
     */
    int decodeAudioPacket (AVPacket* packet)
    {
        jassert (packet && audio);
        auto* frame = queue->audio.getWriteFrame();
        int result = avcodec_send_packet (audio, packet);
        
        if (result == AVERROR(EAGAIN))
        {
            while (result != AVERROR(EAGAIN))
                result = avcodec_receive_frame (audio, frame);
            result = avcodec_send_packet (audio, packet);
        }
        else if (result != 0)
        {
            return result;
        }
        
        result = avcodec_receive_frame (audio, frame);
        while (result == AVERROR(EAGAIN))
            result = avcodec_receive_frame (audio, frame);
        
        if (0 == result)
            queue->audio.finishedWrite();
        
        return result;
    }

    /** Decodes a video packet, returns the result of avcodec_decode_video2 */
    int decodeVideoPacket (AVPacket* packet, AVFrame* frame)
    {
        if (video == nullptr || packet->size <= 0 || frame == nullptr)
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
int FFmpegDecoder::getWidth()   const { return pimpl->getWidth(); }
int FFmpegDecoder::getHeight()  const { return pimpl->getHeight(); }

Rational FFmpegDecoder::getRealFrameRate() const
{
    if (auto* s = pimpl->getVideoStream())
        return FFmpegRational (av_stream_get_r_frame_rate (s));
    return Rational();
}

AVPixelFormat FFmpegDecoder::getPixelFormat() const
{
    return (pimpl && pimpl->video) ? pimpl->video->pix_fmt
                                   : AV_PIX_FMT_NONE;
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
            
            while (queue.video.getNumReady() < 2)
                decoder->read();
        }
        
        DBG("[KV] ffmpeg: video source thread exited");
    }
    
    void videoTick (const double pts)
    {
        AVFrame* frame = nullptr;
        const AVRational tb = { 1, 6000 };
        bool gotFrame = false;
        
        while (queue.video.canRead())
        {
            frame = queue.video.getReadFrame();
            const double myPts = av_q2d(tb) * (double)frame->pts;
            
            if (myPts >= pts)
            {
                gotFrame = true;
                scale.convertFrameToImage (image, frame);
                // image.clear(Rectangle<int> (0,0,image.getWidth(),image.getHeight()));
                // DBG("[KV] ffmpeg: decoded video @ " << av_q2d(tb) * (double)frame->pts << " master: " << pts);
            }
            
            queue.video.finishedRead();
            av_frame_unref (frame);
            
            if (gotFrame)
                break;
        }
        
        #if 1
        while (queue.audio.canRead())
        {
            frame = queue.audio.getReadFrame();
            
            const int channels = av_get_channel_layout_nb_channels (frame->channel_layout);
            const int numSamples = frame->nb_samples;

            AudioSampleBuffer audio ((float**) frame->extended_data,
                                     channels, 0, numSamples);
            
           #if DEBUG_LOG_AUDIO_PACKETS
            const double seconds = static_cast<double>(frame->pts) / (double)frame->sample_rate;
            DBG("[KV] ffmpeg: decoded audio: " << "channels: "   << audio.getNumChannels()
                << " samples: "   << audio.getNumSamples()
                << " pts: "       << frame->pts
                << " sec: "       << seconds
                << " free: "      << audioOut.getFreeSpace()
                << " fmt: "       << av_get_sample_fmt_name ((AVSampleFormat) frame->format));
           #endif

            bool stopFlag = false;
            if (numSamples < audioOut.getFreeSpace())
            {
                audioOut.write (audio);
            }
            else
            {
                stopFlag = true;
            }
            
            queue.audio.finishedRead();
            av_frame_unref (frame);
            
            if (stopFlag)
                break;
        }
        #endif
        
        sem.post();
    }
    
    void openFile (const File& file)
    {
        decoder->openFile (file);
        
        audioOut.setSize (2, 192000);
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
    
    void videoFrameDecoded (const AVStream* stream, AVFrame* frame) override { }
    void audioFrameDecoded (const AVStream* stream, AVFrame* frame) override { }
    void subtitleFrameDecoded (const AVStream* stream, AVFrame*) override { }
    
    Semaphore sem;
    
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

void FFmpegVideoSource::videoTick (const double seconds)
{
    pimpl->videoTick (seconds);
}

void FFmpegVideoSource::openFile (const File& file)
{
    pimpl->openFile (file);
}

Image FFmpegVideoSource::findImage (double pts)
{
    return pimpl->image;
}

Rational FFmpegVideoSource::getRealFrameRate() const
{
    return pimpl->decoder->getRealFrameRate();
}

void FFmpegVideoSource::renderAudio (const AudioSourceChannelInfo& info)
{
    auto& queue (pimpl->queue);
    auto& audioOut (pimpl->audioOut);

    if (info.numSamples <= pimpl->audioOut.getNumReady())
    {
        pimpl->audioOut.readFromFifo (info);
    }
    else
    {
        int numSamples = audioOut.getNumReady();
        if (numSamples > 0)
        {
            audioOut.readFromFifo (info, numSamples);
            info.buffer->clear (numSamples, info.numSamples - numSamples);
        }
        else
        {
            info.clearActiveBufferRegion();
        }
    }
}
