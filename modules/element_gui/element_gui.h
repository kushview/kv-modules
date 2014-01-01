/*
    element_module.h - This file is part of Element
    Copyright (C) 2013  Michael Fisher <mfisher31@gmail.com>

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

#ifndef ELEMENT_GUI_H_INCLUDED
#define ELEMENT_GUI_H_INCLUDED

#include "../element_models/element_models.h"
#include "../element_engines/element_engines.h"
#include "modules/juce_gui_basics/juce_gui_basics.h"
#include "modules/juce_audio_processors/juce_audio_processors.h"

namespace Element {

// look and feel
#include "base/Icons.h"
#include "base/Stock.h"
#include "base/LookAndFeel.h"
    
// timelines
#include "base/TrackHeights.h"
#include "base/TimelineBase.h"
#include "base/TimelineClip.h"
#include "base/NoteClipItem.h"
#include "base/MidiEditorBody.h"

// graph stuff
#include "base/GraphEditorBase.h"

// misc widgets
#include "base/DecibelScaleComponent.h"
#include "base/HighQualityMeter.h"
#include "base/IconButton.h"
#include "base/PluginWindow.h"
#include "base/SkinDial.h"
#include "base/TreeviewBase.h"

// assets
#include "assets/AssetTreeView.h"

// docking
#include "docking/Dock.h"

// embedding
#include "embed/NativeComponent.h"
    
}

#endif   // ELEMENT_MODELS_H_INCLUDED
