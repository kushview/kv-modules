/*
    kv_engines.h - This file is part of the Kushview Modules for JUCE
    Copyright (C) 2014-2017  Kushview, LLC.  All rights reserved.

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

    ID:               kv_engines
    vendor:           Kushview, LLC
    version:          0.0.1
    name:             Kushview Engines
    description:      Classes for creating multimedia processing engines
    website:          https://kushview.net
    license:          GPL v2

    dependencies:     juce_data_structures, juce_audio_processors, juce_audio_devices

    END_JUCE_MODULE_DECLARATION
 */

#ifndef ELEMENT_ENGINES_H_INCLUDED
#define ELEMENT_ENGINES_H_INCLUDED

#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <kv_models/kv_models.h>

/** Config: ELEMENT_USE_JACK
    Set this to enable Jack Audio Support (default is disabled)
 */
#ifndef ELEMENT_USE_JACK
 #define ELEMENT_USE_JACK 0
#endif

#if ELEMENT_USE_JACK
 #include <vector>
 #include <jack/jack.h>
#endif

namespace Element {

#include "common/Processor.h"
#include "common/MidiSequencePlayer.h"
#include "common/Shuttle.h"

#if ELEMENT_USE_JACK
 #include "jack/Jack.h"
#endif

}

#endif   // ELEMENT_MODELS_H_INCLUDED
