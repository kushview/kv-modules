/*
    element_module.cpp - This file is part of Element
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

#include "AppConfig.h"
#include "element_gui.h"

namespace Element {

#include "assets/AssetTreeView.cpp"
#include "base/DecibelScaleComponent.cpp"
#include "base/HighQualityMeter.cpp"
#include "base/Icons.cpp"
#include "base/LookAndFeel.cpp"
#include "base/TreeviewBase.cpp"
#include "docking/Dock.cpp"
#include "docking/DockArea.cpp"
#include "docking/DockItem.cpp"
#include "docking/DockLayout.cpp"

#include "embed/NativeComponent.cpp"

#if JUCE_MODULE_AVAILABLE_element_engines
 #include "base/GraphEditorBase.cpp"
 #include "base/MidiEditorBody.cpp"
 #include "base/PluginWindow.cpp"
 #include "base/TimelineBase.cpp"
 #include "base/TimelineClip.cpp"
#endif

}
