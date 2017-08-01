
#pragma once

/** Wrapper around ffmpeg's swscale */
class FFmpegVideoScaler
{
public:
    /** Creates a scaler object. It does nothing before you call setupScaler */
    FFmpegVideoScaler () : scalerContext (nullptr) { }

    ~FFmpegVideoScaler ()
    {
        if (scalerContext)
            sws_freeContext (scalerContext);
    }

    /** Frees the scaler and resets linesizes */
    void reset()
    {
        if (scalerContext)
        {
            sws_freeContext (scalerContext);
            scalerContext = nullptr;
        }
        
        for (int i = 0; i < 4; ++i)
            inLinesizes[i] = outLinesizes[i] = 0;
    }
    
    /** Setup a scaler to scale video frames and to convert pixel formats */
    void setupScaler (const int in_width, const int in_height,  const AVPixelFormat in_format,
                      const int out_width, const int out_height, const AVPixelFormat out_format)
    {
        reset();

        const AVPixFmtDescriptor* in_descriptor = av_pix_fmt_desc_get (in_format);
        if (! in_descriptor)
        {
            DBG ("[KV] ffmpeg: no description for input pixel format");
            return;
        }
        
        const int in_bitsPerPixel = av_get_padded_bits_per_pixel (in_descriptor);
        for (int i = 0; i < 4; ++i)
            inLinesizes[i] = i < in_descriptor->nb_components ? in_width * in_bitsPerPixel >> 3 : 0;

        const AVPixFmtDescriptor* out_descriptor = av_pix_fmt_desc_get (out_format);
        if (! out_descriptor)
        {
            DBG ("[KV] ffmpeg: no description for output pixel format");
            return;
        }
        const int out_bitsPerPixel = av_get_padded_bits_per_pixel (out_descriptor);
        for (int i = 0; i < 4; ++i)
            outLinesizes [i] = i < out_descriptor->nb_components ? out_width * out_bitsPerPixel >> 3 : 0;

        /* create scaling context */
        scalerContext = sws_getContext (in_width,  in_height, in_format,
                                        out_width, out_height, out_format,
                                        SWS_BILINEAR, nullptr, nullptr, nullptr);
        if (! scalerContext)
        {
            DBG ("[KV] ffmpeg: impossible to create scale context for the conversion");
        }
    }

    /** Takes an AVFrame from ffmpeg and converts it to a JUCE Image. Image is in a format
        matching to the platform */
    void convertFrameToImage (juce::Image& image, const AVFrame* frame)
    {
        if (nullptr == scalerContext)
            return;
        Image::BitmapData data (image, 0, 0, image.getWidth(), image.getHeight(),
                                Image::BitmapData::writeOnly);

        uint8_t* destination[4] = {data.data, nullptr, nullptr, nullptr};

        sws_scale (scalerContext, frame->data, frame->linesize,
                   0, frame->height, destination, outLinesizes);
    }

    /** Converts a JUCE Image into a ffmpeg AVFrame to be written into a video stream */
    void convertImageToFrame (AVFrame* frame, const juce::Image& image)
    {
        if (nullptr == scalerContext)
            return;

        juce::Image::BitmapData data (image, 0, 0, image.getWidth(), image.getHeight());
        const uint8_t* const source[4] = { data.data, nullptr, nullptr, nullptr };
        sws_scale (scalerContext, source, inLinesizes,
                   0, image.getHeight(), frame->data, frame->linesize);
    }

private:
    SwsContext* scalerContext;
    int inLinesizes[4];
    int outLinesizes[4];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFmpegVideoScaler)
};
