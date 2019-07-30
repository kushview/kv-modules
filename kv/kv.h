/*
    This file is part of the Kushview modules for JUCE
    Copyright (C) 2016-2017 Kushview, LLC.  All rights reserved.

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

#pragma once

#include <juce/juce.h>

#if JUCE_MODULE_AVAILABLE_kv_core
 #include <kv_core/kv_core.h>
#endif

#if JUCE_MODULE_AVAILABLE_kv_edd
 #include <kv_edd/kv_edd.h>
#endif

#if JUCE_MODULE_AVAILABLE_kv_engines
 #include <kv_engines/kv_engines.h>
#endif

#if JUCE_MODULE_AVAILABLE_kv_ffmpeg
 #include <kv_ffmpeg/kv_ffmpeg.h>
#endif

#if JUCE_MODULE_AVAILABLE_kv_gui
 #include <kv_gui/kv_gui.h>
#endif

#if JUCE_MODULE_AVAILABLE_kv_lv2
 #include <kv_lv2/kv_lv2.h>
#endif

#if JUCE_MODULE_AVAILABLE_kv_models
 #include <kv_models/kv_models.h>
#endif

#if JUCE_MODULE_AVAILABLE_kv_video
 #include <kv_video/kv_video.h>
#endif
