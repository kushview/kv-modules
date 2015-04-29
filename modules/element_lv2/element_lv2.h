/*
    element_lv2.h - This file is part of Element
    the element modules, LLC.  All rights reserved.

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

#ifndef ELEMENT_LV2_H_INCLUDED
#define ELEMENT_LV2_H_INCLUDED

/** Config: ELEMENT_LV2_PLUGIN_HOST
    Set this to enable LV2 support. This option is available so that
    the LV2 module can be included but disabled on a per-platform
    basis (default is enabled)
 */
#ifndef ELEMENT_LV2_PLUGIN_HOST
 #define ELEMENT_LV2_PLUGIN_HOST 1
#endif

#if ELEMENT_LV2_PLUGIN_HOST

#include "../element_engines/element_engines.h"
#include "modules/juce_audio_formats/juce_audio_formats.h"
#include "modules/juce_audio_processors/juce_audio_processors.h"

#include <lilv/lilv.h>
#include <suil/suil.h>
#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/event/event.h>
#include <lv2/lv2plug.in/ns/ext/log/log.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include <lv2/lv2plug.in/ns/ext/uri-map/uri-map.h>
#include <lv2/lv2plug.in/ns/ext/worker/worker.h>

namespace Element {
#include "common/URIs.h"
#include "common/PortBuffer.h"
#include "common/PortWriter.h"

#include "features/LV2Features.h"
#include "features/LV2Log.h"
#include "features/LV2Worker.h"
#include "features/SymbolMap.h"

#include "host/LV2World.h"
#include "host/LV2Module.h"
#include "host/LV2Parameter.h"
#include "host/LV2PluginFormat.h"
#include "host/LV2PluginModel.h"
//#include "host/PortProcessor.h"
}

#endif // ELEMENT_LV2_PLUGIN_HOST
#endif // ELEMENT_LV2_H_INCLUDED
