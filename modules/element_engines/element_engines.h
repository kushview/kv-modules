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

#ifndef ELEMENT_ENGINES_H_INCLUDED
#define ELEMENT_ENGINES_H_INCLUDED

#include "modules/juce_gui_extra/juce_gui_extra.h"
#include "modules/juce_audio_devices/juce_audio_devices.h"
#include "modules/juce_audio_processors/juce_audio_processors.h"
#include "../element_models/element_models.h"

namespace Element {
//#include "audio/PortType.h"
#include "audio/Processor.h"
    
//#include "audio/Buffer.h"
//#include "audio/AtomBuffer.h"
//#include "audio/BufferFactory.h"
#include "audio/ClipFactory.h"
#include "audio/ClipSource.h"
#include "audio/DataType.h"
#include "audio/Engine.h"
    
#include "audio/GraphProcessor.h"
#include "audio/GraphController.h"
#include "audio/GraphDocument.h"
#include "audio/GraphPlayer.h"
#include "audio/GraphPort.h"

#include "audio/MidiDataBase.h"
#include "audio/MidiSequencePlayer.h"

#include "audio/SequenceCursor.h"
#include "audio/Sequencer.h"
#include "audio/SequencerTrack.h"
#include "audio/Shuttle.h"
    
#include "session/AppController.h"
#include "session/DeviceManager.h"
#include "session/PluginManager.h"
#include "session/PluginWrapper.h"
#include "session/SessionBase.h"
    
}

#endif   // ELEMENT_MODELS_H_INCLUDED
