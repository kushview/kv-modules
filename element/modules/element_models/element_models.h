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

#ifndef ELEMENT_MODELS_H_INCLUDED
#define ELEMENT_MODELS_H_INCLUDED


#include "modules/juce_audio_basics/juce_audio_basics.h"
#include "modules/juce_data_structures/juce_data_structures.h"
#include "../element_base/element_base.h"

namespace Element {

#include "models/ObjectModel.h"
#include "models/AssetTree.h"
#include "models/AssetType.h"
#include "models/Asset.h"
    
#include "models/NodeModel.h"
#include "models/BlockModel.h"
#include "models/GraphModel.h"
#include "models/MediaModel.h"
#include "models/ClipModel.h"

#include "models/Note.h"
#include "models/NoteSequence.h"
#include "models/PortModel.h"
#include "models/SequenceModel.h"
#include "models/TempoMap.h"
#include "models/TrackModel.h"
    
}

#endif   // ELEMENT_MODELS_H_INCLUDED
