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

    BEGIN_JUCE_MODULE_DECLARATION

    ID:               kv_gui
    vendor:           Kushview, LLC
    version:          0.0.1
    name:             Kushview provided UI components
    description:      Layouts and common widgets
    website:          https://kushview.net
    license:          GPL v2

    dependencies:     kv_core, juce_gui_basics

    END_JUCE_MODULE_DECLARATION
 */

#pragma once
#define KV_GUI_H_INCLUDED

#include <juce_gui_basics/juce_gui_basics.h>
#include <kv_models/kv_models.h>

/** Config: KV_DOCKING_WINDOWS
    Experimental: Set this to enable Docking windows support. Docking windows
    support is a major work in progress. The final design and API is subject to
    change until deemed stable. (default is disabled)
 */
#ifndef KV_DOCKING_WINDOWS
 #define KV_DOCKING_WINDOWS 0
#endif

namespace kv {
    using namespace juce;
    class Screen;
    class ScreenManager;
    class ScreenDisplay;
}

// look and feel
#include "base/LookAndFeel_KV1.h"

namespace kv {

#include "base/PatchMatrixComponent.h"

// screens
#include "screens/ScreenDisplay.h"
#include "screens/Screen.h"
#include "screens/ScreenManager.h"
}

#if JUCE_MODULE_AVAILABLE_kv_engines
 // timelines
 #include "timeline/TrackHeights.h"
 #include "timeline/TimelineComponent.h"
 #include "timeline/TimelineClip.h"
#endif

#include "base/FileComboBox.h"

namespace kv {
// misc widgets
#include "base/DecibelScaleComponent.h"
#include "base/DigitalMeter.h"
#include "base/SkinDial.h"

// embedding
#include "embed/NativeComponent.h"

}

// docking
#if KV_DOCKING_WINDOWS
 #include "docking/DockPlacement.h"
 #include "docking/DockLayout.h"
 #include "docking/Dock.h"
 #include "docking/DockContainer.h"
 #include "docking/DockPanel.h"
 #include "docking/DockItemTabs.h"
 #include "docking/DockWindow.h"
#endif
