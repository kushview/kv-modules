
#define LOG_FORMAT_INFO 0

struct FFmpegDecoder::Pimpl
{
    Pimpl (FFmpegDecoder& d)
        : decoder           (d),
          format            (nullptr),
          audio             (nullptr),
          video             (nullptr),
          subtitle          (nullptr),
          audioStream       (-1),
          videoStream       (-1),
          subtitleStream    (-1)
    { }
    
    ~Pimpl() { }
    
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
            close();
            return false;
        }
        
        // open the streams
        audioStream = openCodecContext (&audio, AVMEDIA_TYPE_AUDIO, true);
        if (isPositiveAndBelow (audioStream, static_cast<int> (format->nb_streams)))
        {
            audio->request_sample_fmt = AV_SAMPLE_FMT_FLTP;
            //audioContext->request_channel_layout = AV_CH_LAYOUT_STEREO_DOWNMIX;
        }
        
        videoStream = openCodecContext (&video, AVMEDIA_TYPE_VIDEO, true);
        if (isPositiveAndBelow (videoStream, static_cast<int> (format->nb_streams)))
        {
            // noop
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
    
    void read()
    {
        if (nullptr == format)
            return;
        
        int error = 0;

        // initialize packet, set data to NULL, let the demuxer fill it
        AVPacket packet;
        packet.data = nullptr;
        packet.size = 0;
        av_init_packet (&packet);
        error = av_read_frame (format, &packet);
        
        if (error >= 0)
        {
            if (packet.stream_index == audioStream)
                decodeAudioPacket (&packet);
            else if (packet.stream_index == videoStream)
                decodeVideoPacket (&packet);
        }
        
        av_packet_unref (&packet);
    }
    
private:
    FFmpegDecoder& decoder;
    AVFormatContext* format;
    AVCodecContext* audio, *video, *subtitle;
    int audioStream, videoStream, subtitleStream;
    AVFrame* audioFrame, *videoFrame;
    double currentPTS;
    
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
    int decodeAudioPacket (AVPacket* packet)
    {
        int got_frame = 0;
        int decoded = packet->size;
        int outputNumSamples = 0;
        
        // decode audio frame
        do {
            // call decode until packet is empty
            int ret = avcodec_decode_audio4 (audio, audioFrame, &got_frame, packet);
            if (ret < 0)
            {
                DBG ("Error decoding audio frame: (Code " + String (ret) + ")");
                break;
            }
            
            int64_t framePTS = av_frame_get_best_effort_timestamp (audioFrame);
            double  framePTSsecs = static_cast<double> (framePTS) / audio->sample_rate;
            
            /* Some audio decoders decode only part of the packet, and have to be
               called again with the remainder of the packet data.
               Sample: fate-suite/lossless-audio/luckynight-partial.shn
               Also, some decoders might over-read the packet. */
            decoded = jmin (ret, packet->size);
            
            packet->data += decoded;
            packet->size -= decoded;
            
            if (got_frame && decoded > 0 && audioFrame->extended_data != nullptr)
            {
                const int channels   = av_get_channel_layout_nb_channels (audioFrame->channel_layout);
                const int numSamples = audioFrame->nb_samples;
                
                int offset = (currentPTS - framePTSsecs) * audio->sample_rate;
                if (offset > 100)
                {
                    if (offset < numSamples)
                    {
                        outputNumSamples = numSamples - offset;
                        AudioBuffer<float> subset ((float* const*)audioFrame->extended_data,
                                                   channels, offset, outputNumSamples);
                        // audioFifo.addToFifo (subset);
                    }
                }
                else
                {
                    outputNumSamples = numSamples;
                    // audioFifo.addToFifo ((const float **)audioFrame->extended_data, numSamples);
                }
            }
            
        } while (got_frame && packet->size > 0);
        
        return outputNumSamples;
    }
    
    /** Decodes a video packet, returns the presentation time stamp in seconds */
    double decodeVideoPacket (AVPacket* packet)
    {
        double pts_sec = 0.0;
        if (video && packet->size > 0)
        {
            int got_picture = 0;
            if (avcodec_decode_video2 (video, videoFrame, &got_picture, packet) > 0)
            {
                AVRational timeBase = av_make_q (1, AV_TIME_BASE);
                if (isPositiveAndBelow (videoStream, static_cast<int> (format->nb_streams)))
                    timeBase = format->streams[videoStream]->time_base;
                
                pts_sec = av_q2d (timeBase) * videoFrame->best_effort_timestamp;
                if (pts_sec >= 0.0)
                {
                    DBG("[KV] ffmpeg: decoded video frame @ " << pts_sec);
                    // videoFrames[videoFifoWrite].first = pts_sec;
                    // videoFifoWrite = ++videoFifoWrite % videoFrames.size();
                }
            }
        }
        
        return pts_sec;
    }
};

FFmpegDecoder::FFmpegDecoder()
{
    pimpl = new Pimpl (*this);
}

FFmpegDecoder::~FFmpegDecoder()
{
    pimpl = nullptr;
}

bool FFmpegDecoder::openFile (const File& file)     { return pimpl->openFile (file); }
void FFmpegDecoder::close()                         { pimpl->close(); }

FFmpegVideoSource::FFmpegVideoSource() { }
FFmpegVideoSource::~FFmpegVideoSource() { }
void FFmpegVideoSource::tick()
{
    
}
