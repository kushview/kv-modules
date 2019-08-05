/*
    element_lv2.cpp - This file is part of Element
    Copyright (C) 2014-2016  Kushview, LLC.  All rights reserved.

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

#if defined (KV_LV2_H_INCLUDED) && ! JUCE_AMALGAMATED_INCLUDE
/* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of JUCE cpp file"
#endif

#include <lv2/state/state.h>

#include "kv_lv2/kv_lv2.h"

#include "common/PortBuffer.cpp"
#include "common/PortWriter.cpp"
#include "features/LV2Log.cpp"
#include "features/LV2Worker.cpp"

#if KV_LV2_PLUGIN_HOST
 #include "host/LV2Module.cpp"
 #include "host/LV2PluginFormat.cpp"
 #include "host/LV2World.cpp"
#endif
