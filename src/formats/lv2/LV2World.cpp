/*
    lv2world.cpp - This file is part of Element
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

#include "element/juce.hpp"
#include "element/formats/lv2/LV2Module.h"
#include "element/formats/lv2/LV2World.h"


namespace Element {

    LV2World::LV2World (SymbolMap& s)
        : sym (s)
    {
        world.load_all();
        lv2_InputPort   = world.new_uri (LV2_CORE__InputPort);
        lv2_OutputPort  = world.new_uri (LV2_CORE__OutputPort);
        lv2_AudioPort   = world.new_uri (LV2_CORE__AudioPort);
        lv2_AtomPort    = world.new_uri (LV2_ATOM__AtomPort);
        lv2_ControlPort = world.new_uri (LV2_CORE__ControlPort);
        lv2_EventPort   = world.new_uri (LV2_EVENT__EventPort);
        lv2_CVPort      = world.new_uri (LV2_CORE__CVPort);
        midi_MidiEvent  = world.new_uri (LV2_MIDI__MidiEvent);
        work_schedule   = world.new_uri (LV2_WORKER__schedule);
        work_interface  = world.new_uri (LV2_WORKER__interface);
    }

    LV2World::~LV2World()
    {
#define elilv_node_free(n) lilv_node_free (const_cast<LilvNode*> (n))
        elilv_node_free (lv2_InputPort);
        elilv_node_free (lv2_OutputPort);
        elilv_node_free (lv2_AudioPort);
        elilv_node_free (lv2_AtomPort);
        elilv_node_free (lv2_ControlPort);
        elilv_node_free (lv2_EventPort);
        elilv_node_free (lv2_CVPort);
        elilv_node_free (midi_MidiEvent);
        elilv_node_free (work_schedule);
        elilv_node_free (work_interface);
    }

    LV2Module*
    LV2World::createModule (const String& uri)
    {
        if (isPluginSupported (uri))
        {
            Lilv::Plugin plugin (getPlugin (uri));
            return new LV2Module (*this, plugin);
        }

        return nullptr;
    }

    const LilvPlugin*
    LV2World::getPlugin (const String& uri)
    {
        LilvNode* p (world.new_uri (uri.toUTF8()));
        const LilvPlugin* plugin = lilv_plugins_get_by_uri (getAllPlugins(), p);
        lilv_node_free (p);

        return plugin;
    }

    const Lilv::Plugins
    LV2World::getAllPlugins()
    {
        return world.get_all_plugins();
    }

    bool
    LV2World::isFeatureSupported (const String& featureURI)
    {
#if 0
       if (features.contains (featureURI))
          return true;
#endif

       if (featureURI == LV2_WORKER__schedule)
          return true;

       return false;
    }

    bool
    LV2World::isPluginAvailable (const String& uri)
    {
        return (getPlugin (uri) != nullptr);
    }

    bool
    LV2World::isPluginSupported (const String& uri)
    {
        Lilv::Plugin p (getPlugin (uri));
        p.get_name();

        // Availability
        if (! isPluginAvailable (uri))
            return false; // Plugin not available

        // Required features support
        Lilv::Nodes featureNodes (p.get_required_features());
        LILV_FOREACH (nodes, iter, featureNodes)
        {
            Lilv::Node node (featureNodes.get (iter));
            String feature (node.as_uri());

            if (! isFeatureSupported (feature))
                return false; // Feature not supported
        }

        // Check we support this plugins port types
        for (int i = p.get_num_ports(); --i >= 0;)
        {
           Lilv::Port port (p.get_port_by_index (i));
           if (port.is_a (const_cast<LilvNode*> (lv2_CVPort)) ||
               port.is_a (const_cast<LilvNode*> (lv2_AtomPort)) ||
               port.is_a (const_cast<LilvNode*> (lv2_EventPort)))
              return false;
        }

        return true;
    }
}
