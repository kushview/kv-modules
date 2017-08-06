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
}

#if __clang__
 #pragma clang diagnostic pop
#endif
