/*
    element_gui.cpp - This file is part of Element
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

#if ELEMENT_USE_LIBJUCE
 #include <juce/modules/config.h>
 #include "element/modules/config.h"
#else
 #include "AppConfig.h"
#endif

#if JUCE_MODULE_AVAILABLE_element_engines
 #include "modules/juce_audio_processors/juce_audio_processors.h"
 #include "../element_engines/element_engines.h"
#endif
#include "../element_models/element_models.h"
#include "element_gui.h"

namespace Element {

#include "base/DecibelScaleComponent.cpp"
#include "base/DigitalMeter.cpp"
#include "base/LookAndFeel.cpp"
    
#if ELEMENT_DOCKING_WINDOWS
 #include "docking/Dock.cpp"
 #include "docking/DockArea.cpp"
 #include "docking/DockItem.cpp"
 #include "docking/DockLayout.cpp"
#endif
    
#include "embed/NativeComponent.cpp"
#include "screens/Screen.cpp"
#include "screens/ScreenDisplay.cpp"
#include "screens/ScreenManager.cpp"

#if JUCE_MODULE_AVAILABLE_element_engines
 #include "timeline/MidiEditorBody.cpp"
 #include "timeline/TimelineComponent.cpp"
 #include "timeline/TimelineClip.cpp"
#endif

}
