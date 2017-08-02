/*
    This file is part of the Kushview Modules for JUCE
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

namespace LV2Callbacks
{
#if 0
    inline unsigned
    uiSupported (const char* hostType, const char* uiType)
    {
        String host (hostType);
        String ui (uiType);

        if (host == LV2_UI__X11UI && ui == LV2_UI__X11UI)
            return 2;
        else if (ui == LV2_UI__JuceUI)
            return 1;

        return suil_ui_supported (hostType, uiType);
    }
#endif
}

class LV2PluginModel::Private
{

public:

    Private (LV2PluginModel& module)
        : owner (module) { }

    ~Private() { }

    SuilInstance* instantiateUI (const LilvUI* ui,
                                 const LilvNode* containerType,
                                 const LilvNode* widgetType,
                                 const LV2_Feature* const * features)
    {
        const LilvNode* uri = lilv_ui_get_uri (ui);
        const LilvPlugin* plugin = owner.getPlugin();

        SuilInstance* instance = suil_instance_new (suil, &owner,
                  lilv_node_as_uri (containerType),
                  lilv_node_as_uri (lilv_plugin_get_uri (plugin)),
                  lilv_node_as_uri (uri),
                  lilv_node_as_uri (widgetType),
                  lilv_uri_to_path (lilv_node_as_uri (lilv_ui_get_bundle_uri (ui))),
                  lilv_uri_to_path (lilv_node_as_uri (lilv_ui_get_binary_uri (ui))),
                  features );

        if (instance != nullptr)
        {
            // do something here?
        }

        return instance;
    }

    ChannelConfig channels;
    HeapBlock<float> mins, maxes, defaults, values;

private:

    LV2PluginModel& owner;
    SuilHost* suil;

};

LV2PluginModel::LV2PluginModel (LV2World& world_, const LilvPlugin* plugin_)
   : world (world_),
     plugin (plugin_),
     numPorts (lilv_plugin_get_num_ports (plugin_))
{
    priv = new Private (*this);
}

LV2PluginModel::~LV2PluginModel()
{
}

String
LV2PluginModel::getAuthorName() const
{
   if (LilvNode* node = lilv_plugin_get_author_name (plugin))
   {
       String name (CharPointer_UTF8 (lilv_node_as_string (node)));
       lilv_node_free (node);
       return name;
   }
   return String::empty;
}

String
LV2PluginModel::getClassLabel() const
{
   if (const LilvPluginClass* klass = lilv_plugin_get_class (plugin))
       if (const LilvNode* node = lilv_plugin_class_get_label (klass))
           return CharPointer_UTF8 (lilv_node_as_string (node));
   return String::empty;
}

String
LV2PluginModel::getName() const
{
   if (LilvNode* node = lilv_plugin_get_name (plugin))
   {
       String name = CharPointer_UTF8 (lilv_node_as_string (node));
       lilv_node_free (node);
       return name;
   }

   return String::empty;
}

uint32 LV2PluginModel::getNumPorts() const { return numPorts; }

uint32
LV2PluginModel::getNumPorts (PortType type, bool isInput) const
{
   if (type == PortType::Unknown)
       return 0;

   const LilvNode* flow = isInput ? world.lv2_InputPort : world.lv2_OutputPort;
   const LilvNode* kind = type == PortType::Audio ? world.lv2_AudioPort
                        : type == PortType::Atom  ? world.lv2_AtomPort
                        : type == PortType::Control ? world.lv2_ControlPort
                        : type == PortType::CV ? world.lv2_CVPort : nullptr;

   if (kind == nullptr || flow == nullptr)
       return 0;

   return lilv_plugin_get_num_ports_of_class (plugin, kind, flow, nullptr);
}

const LilvPort*
LV2PluginModel::getPort (uint32 port) const
{
    return lilv_plugin_get_port_by_index (plugin, port);
}

uint32
LV2PluginModel::getMidiPort() const
{
   for (uint32 i = 0; i < numPorts; ++i)
   {
       const LilvPort* port (getPort (i));
       if (lilv_port_is_a (plugin, port, world.lv2_AtomPort) &&
           lilv_port_is_a (plugin, port, world.lv2_InputPort) &&
           lilv_port_supports_event (plugin, port, world.midi_MidiEvent))
           return i;
   }

   return LV2UI_INVALID_PORT_INDEX;
}

const LilvPlugin* LV2PluginModel::getPlugin() const { return plugin; }

uint32
LV2PluginModel::getNotifyPort() const
{
    for (uint32 i = 0; i < numPorts; ++i)
    {
        const LilvPort* port (getPort (i));
        if (lilv_port_is_a (plugin, port, world.lv2_AtomPort) &&
            lilv_port_is_a (plugin, port, world.lv2_OutputPort) &&
            lilv_port_supports_event (plugin, port, world.midi_MidiEvent))
        {
            return i;
        }
    }

    return LV2UI_INVALID_PORT_INDEX;
}

const String
LV2PluginModel::getPortName (uint32 index) const
{
    if (const LilvPort* port = getPort (index))
    {
        LilvNode* node = lilv_port_get_name (plugin, port);
        const String name = CharPointer_UTF8 (lilv_node_as_string (node));
        lilv_node_free (node);
        return name;
    }

    return String::empty;
}

PortType
LV2PluginModel::getPortType (uint32 i) const
{
   const LilvPort* port (lilv_plugin_get_port_by_index (plugin, i));

   if (lilv_port_is_a (plugin, port, world.lv2_AudioPort))
       return PortType::Audio;
   else if (lilv_port_is_a (plugin, port, world.lv2_AtomPort))
       return PortType::Atom;
   else if (lilv_port_is_a (plugin, port, world.lv2_ControlPort))
       return PortType::Control;
   else if (lilv_port_is_a (plugin, port, world.lv2_CVPort))
       return PortType::CV;
   else if (lilv_port_is_a (plugin, port, world.lv2_EventPort))
       return PortType::Event;

   return PortType::Unknown;
}


String
LV2PluginModel::getURI() const
{
   return lilv_node_as_string (lilv_plugin_get_uri (plugin));
}

bool
LV2PluginModel::isPortInput (uint32 index) const
{
   return lilv_port_is_a (plugin, getPort (index), world.lv2_InputPort);
}

bool
LV2PluginModel::isPortOutput (uint32 index) const
{
   return lilv_port_is_a (plugin, getPort (index), world.lv2_OutputPort);
}
