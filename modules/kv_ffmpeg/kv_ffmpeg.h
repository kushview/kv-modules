/*
    This file is part of the Kushview Modules for JUCE
    Copyright (c) 2017-2019  Kushview, LLC.  All rights reserved.

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

    BEGIN_JUCE_MODULE_DECLARATION

    ID:            kv_ffmpeg
    vendor:        Kushview
    version:       0.0.1
    name:          FFmpeg library support for JUCE
    description:   Classes used for reading/writing media streams supported by ffmpeg
    dependencies:  juce_core, kv_video
    website:       http://kushview.net
    license:       GPL

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

namespace kv {
using namespace juce;

void ffmpeg_init (const bool useNetwork = false);
void ffmpeg_deinit();

#include "filters/FFmpegScaler.h"
#include "io/FFmpegDecoder.h"
}
