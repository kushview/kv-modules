
#include "kv_ffmpeg.h"

namespace kv {
static bool ffmpegUseNetwork = false;
void ffmpeg_init (const bool useNetwork)
{
    av_register_all();
    ffmpegUseNetwork = useNetwork;
    if (ffmpegUseNetwork)
        avformat_network_init();
}

void ffmpeg_deinit()
{
    if (ffmpegUseNetwork)
        avformat_network_deinit();
}
}

#if __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wunused-variable"
 #pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace kv {
 #include "FFmpegDecoder.cpp"
 // #include "ffmpeg_VideoReader.cpp"
 // #include "ffmpeg_VideoWriter.cpp"
 // #include "ffmpeg_VideoComponent.cpp"
}

#if __clang__
 #pragma clang diagnostic pop
#endif
