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

#ifndef ELEMENT_GUI_H_INCLUDED
#define ELEMENT_GUI_H_INCLUDED

#include "modules/juce_gui_basics/juce_gui_basics.h"
#include "../element_models/element_models.h"

namespace Element {

class Screen;
class ScreenManager;
class ScreenDisplay;

// look and feel
#include "base/Icons.h"
#include "base/LookAndFeel.h"

#include "screens/ScreenDisplay.h"
#include "screens/Screen.h"
#include "screens/ScreenManager.h"


#if JUCE_MODULE_AVAILABLE_element_engines
 // timelines
 #include "timeline/TrackHeights.h"
 #include "timeline/TimelineComponent.h"
 #include "timeline/TimelineClip.h"
 #include "timeline/NoteClipItem.h"
 #include "timeline/MidiEditorBody.h"

 // graph stuff
 #include "base/GraphEditorBase.h"

 // misc audio/video engine stuff
 #include "base/PluginWindow.h"
#endif

// misc widgets
#include "base/DecibelScaleComponent.h"
#include "base/DigitalMeter.h"
#include "base/IconButton.h"
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
