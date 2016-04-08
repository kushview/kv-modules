/*
    element_module.h - This file is part of Element
    Copyright (C) 2014  Kushview, LLC.  All rights reserved.

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

/*
  BEGIN_JUCE_MODULE_DECLARATION

  ID:               element_engines
  vendor:           kushview
  version:          0.0.1
  name:             Element Engines
  description:      Element Engines
  website:          https://kushview.net
  license:          GPL

  dependencies:     juce_data_structures, juce_audio_processors, juce_audio_devices
  OSXFrameworks:
  iOSFrameworks:
  linuxLibs:
  mingwLibs:

  END_JUCE_MODULE_DECLARATION
 */

#ifndef ELEMENT_ENGINES_H_INCLUDED
#define ELEMENT_ENGINES_H_INCLUDED

#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <element_models/element_models.h>

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
