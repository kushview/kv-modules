/*
    element_gui.h - This file is part of Element
    Copyright (C) 2016 Kushview, LLC.  All rights reserved.

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

  ID:               element_gui
  vendor:           kushview
  version:          0.0.1
  name:             Element GUI
  description:      Element GUI
  website:          https://kushview.net
  license:          GPL

  dependencies:     element_base, juce_gui_basics
  OSXFrameworks:
  iOSFrameworks:
  linuxLibs:
  mingwLibs:

  END_JUCE_MODULE_DECLARATION
 */

#ifndef ELEMENT_GUI_H_INCLUDED
#define ELEMENT_GUI_H_INCLUDED

#include <juce_gui_basics/juce_gui_basics.h>
#include <element_models/element_models.h>

/** Config: ELEMENT_DOCKING_WINDOWS
    Experimental: Set this to enable Docking windows support. Docking windows
    support is a major work in progress. The final design and API is subject to
    change until deemed stable. (default is disabled)
 */
#ifndef ELEMENT_DOCKING_WINDOWS
 #define ELEMENT_DOCKING_WINDOWS 0
#endif

namespace Element {

class Screen;
class ScreenManager;
class ScreenDisplay;

// look and feel
#include "base/LookAndFeel.h"

// screens
#include "screens/ScreenDisplay.h"
#include "screens/Screen.h"
#include "screens/ScreenManager.h"

#if JUCE_MODULE_AVAILABLE_element_engines
 // timelines
 #include "timeline/TrackHeights.h"
 #include "timeline/TimelineComponent.h"
 #include "timeline/TimelineClip.h"
#endif

// misc widgets
#include "base/DecibelScaleComponent.h"
#include "base/DigitalMeter.h"
#include "base/SkinDial.h"

// docking
#if ELEMENT_DOCKING_WINDOWS
 #include "docking/Dock.h"
#endif

// embedding
#include "embed/NativeComponent.h"

}

#endif   // ELEMENT_MODELS_H_INCLUDED
