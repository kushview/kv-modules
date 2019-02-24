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

    ID:            kv_video
    vendor:        Kushview
    version:       0.0.1
    name:          Video classes for JUCE
    description:   Provides classes to read media streams from video sources
    dependencies:  juce_audio_basics, juce_audio_formats, kv_core
    website:       http://kushview.net
    license:       GPL

    END_JUCE_MODULE_DECLARATION
*/

#pragma once
#define KV_VIDEO_H_INCLUDED

#include <kv_core/kv_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace kv {
 #include "sources/VideoSource.h"
}
