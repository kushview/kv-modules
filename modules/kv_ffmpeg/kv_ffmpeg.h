/*
  BEGIN_JUCE_MODULE_DECLARATION
 
  ID:            kv_ffmpeg
  vendor:        Kushview, LLC.
  version:       0.0.1
  name:          Provides FFmpeg library support for JUCE
  description:   Classes used for reading/writing media formats supported by ffmpeg
  dependencies:  juce_core, kv_video
  website:       http://kushview.net
  license:       GPL v3

  searchpaths:   local/include
  OSXLibs:       avcodec avformat avutil swscale
 
  END_JUCE_MODULE_DECLARATION
*/

#pragma once
#define KV_FFMPEG_H_INCLUDED

#include <juce_core/juce_core.h>
#include <kv_video/kv_video.h>

#if __clang__
/* Using ffmpeg from homebrew compiles with deprecated declares allowed.
   This prevents warnings from within ffmpeg depending on how it was compiled */
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#ifdef __cplusplus
 extern "C" {
#endif

#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"

#ifdef __cplusplus
}
#endif

#if __clang__
 #pragma clang diagnostic pop
#endif

namespace kv
{
using namespace juce;
using namespace Element;
void ffmpeg_init (const bool useNetwork = false);
void ffmpeg_deinit();

#include "FFmpegDecoder.h"
#include "FFmpegScaler.h"
}
