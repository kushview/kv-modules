/*
    LV2Module.cpp - This file is part of Element
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

#if COMPLETION
 #include "KSP1.h"
#endif

namespace LV2Callbacks {
    inline unsigned uiSupported (const char* hostType, const char* uiType)
    {
        String host (hostType);
        String ui (uiType);
        
        if (host == LV2_UI__X11UI && ui == LV2_UI__X11UI)
            return 2;
        else if (ui == LV2_UI__JuceUI)
            return 1;
        
        return suil_ui_supported (hostType, uiType);
    }
}

class LV2Module::Private {
public:
    Private (LV2Module& module)
        : owner (module) { }
    
    ~Private() { }
    
    SuilInstance* instantiateUI (const LilvUI* ui,
                                 const LilvNode* containerType,
                                 const LilvNode* widgetType,
                                 const LV2_Feature* const * features)
    {
        suil = owner.getWorld().getSuilHost();
        const LilvNode* uri = lilv_ui_get_uri (ui);
        const LilvPlugin* plugin = owner.getPlugin();
        
        SuilInstance* instance = suil_instance_new (suil, &owner,
                  lilv_node_as_uri (containerType),
                  lilv_node_as_uri (lilv_plugin_get_uri (plugin)),
                  lilv_node_as_uri (uri),
                  lilv_node_as_uri (widgetType),
                  lilv_uri_to_path (lilv_node_as_uri (lilv_ui_get_bundle_uri (ui))),
                  lilv_uri_to_path (lilv_node_as_uri (lilv_ui_get_binary_uri (ui))),
                  features);
        
        if (instance != nullptr)
        {
            // do something here?
        }
        
        return instance;
    }
    
    ChannelConfig channels;
    HeapBlock<float> mins, maxes, defaults, values;
    
private:
    LV2Module& owner;
    SuilHost* suil;
};

LV2Module::LV2Module (LV2World& world_, const LilvPlugin* plugin_)
   : instance (nullptr),
     plugin (plugin_),
     world (world_),
     active (false),
     currentSampleRate (44100.0),
     numPorts (lilv_plugin_get_num_ports (plugin_)),
     events (nullptr)
{
    priv = new Private (*this);
    init();
}

LV2Module::~LV2Module()
{
    freeInstance();
    worker = nullptr;
}

void LV2Module::activatePorts()
{
   jassert (instance != nullptr);
   for (int32 p = 0; p < numPorts; ++p)
   {
       //const bool isInput  = isPortInput (p);
       const PortType type = getPortType (p);
       
       if (type == PortType::Atom)
       {
           
       }
       else if (type == PortType::Audio)
       {
           
       }
       else if (type == PortType::Control)
       {
           // normally this would ONLY be done during 'run'. However,
           // control ports are only connected once, here, during activation
           connectPort (p, priv->values.getData() + p);
       }
       else if (type == PortType::CV)
       {
           
       }
   }
}

void LV2Module::init()
{
    // create and set default port values
    priv->mins.allocate (numPorts, true);
    priv->maxes.allocate (numPorts, true);
    priv->defaults.allocate (numPorts, true);
    priv->values.allocate (numPorts, true);
    lilv_plugin_get_port_ranges_float (plugin, priv->mins, priv->maxes, priv->defaults);
    
    // initialize each port
    for (uint32 p = 0; p < numPorts; ++p)
    {
        const LilvPort* port (lilv_plugin_get_port_by_index (plugin, p));
        const bool isInput (lilv_port_is_a (plugin, port, world.lv2_InputPort));
        priv->channels.addPort (getPortType (p), p, isInput);
        priv->values [p] = priv->defaults [p];
    }
}

Result LV2Module::instantiate (double samplerate)
{
    freeInstance();
    currentSampleRate = samplerate;
    
    features.clearQuick();
    world.getFeatures (features);
    
    // check for a worker interface
    LilvNodes* nodes = lilv_plugin_get_extension_data (plugin);
    LILV_FOREACH (nodes, iter, nodes)
    {
        const LilvNode* node = lilv_nodes_get (nodes, iter);
        if (lilv_node_equals (node, world.work_interface))
        {
            worker = new LV2Worker (world.getWorkThread(), 1);
            features.add (worker->getFeature());
        }
    }
    lilv_nodes_free (nodes); nodes = nullptr;        
    
    features.add (nullptr);
    instance = lilv_plugin_instantiate (plugin, samplerate,
                                        features.getRawDataPointer());
    if (instance == nullptr) {
        features.clearQuick();
        worker = nullptr;
        return Result::fail ("Could not instantiate plugin.");
    }
    
    if (const void* data = getExtensionData (LV2_WORKER__interface))
    {
        assert (worker != nullptr);
        worker->setSize (2048);
        worker->setInterface (lilv_instance_get_handle (instance),
                              (LV2_Worker_Interface*) data);
    }
    else if (worker)
    {
        features.removeFirstMatchingValue (worker->getFeature());
        worker = nullptr;
    }
    
    return Result::ok();
}

void LV2Module::activate()
{
   if (instance && ! active)
   {
       activatePorts();
       lilv_instance_activate (instance);
       active = true;
   }
}

void LV2Module::cleanup()
{
   if (instance != nullptr)
   {
       // TODO: Check if Lilv takes care of calling cleanup
   }
}

void LV2Module::deactivate()
{
   if (instance != nullptr && active)
   {
       lilv_instance_deactivate (instance);
       active = false;
   }
}

bool LV2Module::isActive() const
{
   return instance && active;
}

void LV2Module::freeInstance()
{
    if (instance != nullptr)
    {
        deactivate();
        worker = nullptr;
        lilv_instance_free (instance);
        instance = nullptr;
    }
}

void LV2Module::setSampleRate (double newSampleRate)
{
    if (newSampleRate == currentSampleRate)
        return;
    
    if (instance != nullptr)
    {
        const bool wasActive = isActive();
        deactivate();
        
        freeInstance();
        instantiate (newSampleRate);
                
        jassert (currentSampleRate == newSampleRate);
        
        if (wasActive)
            activate();
    }
}

Result LV2Module::allocateEventBuffers()
{
   for (int i = 0; i < numPorts; ++i)
   { }

   return Result::ok();
}

void LV2Module::connectChannel (const PortType type, const int32 channel, void* data, const bool isInput)
{
    connectPort (priv->channels.getPort (type, channel, isInput), data);
}

void LV2Module::connectPort (uint32 port, void* data)
{
    lilv_instance_connect_port (instance, port, data);
}

String LV2Module::getAuthorName() const
{
   if (LilvNode* node = lilv_plugin_get_author_name (plugin))
   {
       String name (CharPointer_UTF8 (lilv_node_as_string (node)));
       lilv_node_free (node);
       return name;
   }
   return String::empty;
}

const ChannelConfig& LV2Module::getChannelConfig() const
{
    return priv->channels;
}

String LV2Module::getClassLabel() const
{
   if (const LilvPluginClass* klass = lilv_plugin_get_class (plugin))
       if (const LilvNode* node = lilv_plugin_class_get_label (klass))
           return CharPointer_UTF8 (lilv_node_as_string (node));
   return String::empty;
}

const void* LV2Module::getExtensionData (const String& uri) const
{
    return instance ? lilv_instance_get_extension_data (instance, uri.toUTF8()) : nullptr;
}

LV2_Handle LV2Module::getHandle()
{
    return instance ? lilv_instance_get_handle (instance) : nullptr;
}

String LV2Module::getName() const
{
   if (LilvNode* node = lilv_plugin_get_name (plugin))
   {
       String name = CharPointer_UTF8 (lilv_node_as_string (node));
       lilv_node_free (node);
       return name;
   }

   return String::empty;
}

uint32 LV2Module::getNumPorts() const { return numPorts; }

uint32 LV2Module::getNumPorts (PortType type, bool isInput) const
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

const LilvPort* LV2Module::getPort (uint32 port) const
{
    return lilv_plugin_get_port_by_index (plugin, port);
}

uint32 LV2Module::getMidiPort() const
{
   for (uint32 i = 0; i < getNumPorts(); ++i)
   {
       const LilvPort* port (getPort (i));
       if ((lilv_port_is_a (plugin, port, world.lv2_AtomPort) || lilv_port_is_a (plugin, port, world.lv2_EventPort))&&
           lilv_port_is_a (plugin, port, world.lv2_InputPort) &&
           lilv_port_supports_event (plugin, port, world.midi_MidiEvent))
           return i;
   }

   return LV2UI_INVALID_PORT_INDEX;
}

const LilvPlugin* LV2Module::getPlugin() const { return plugin; }

uint32 LV2Module::getNotifyPort() const
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

const String LV2Module::getPortName (uint32 index) const
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

void LV2Module::getPortRange (uint32 port, float& min, float& max, float& def) const
{
    if (port >= numPorts)
        return;
    
    min = priv->mins [port];
    max = priv->maxes [port];
    def = priv->defaults [port];
}

PortType LV2Module::getPortType (uint32 i) const
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

String LV2Module::getURI() const
{
   return lilv_node_as_string (lilv_plugin_get_uri (plugin));
}

bool LV2Module::isLoaded() const { return instance != nullptr; }

bool LV2Module::hasEditor() const
{
    bool hasJuceUI = false;

    if (LilvUIs* uis = lilv_plugin_get_uis (plugin))
    {
        LILV_FOREACH(uis, iter, uis)
        {
            const LilvUI* ui = lilv_uis_get (uis, iter);
            const unsigned quality = lilv_ui_is_supported (ui, &LV2Callbacks::uiSupported, world.ui_JuceUI, nullptr);

            if (quality == 1)
            {
                const String uri = lilv_node_as_string (lilv_ui_get_uri (ui));
                DBG (uri << " quality: " << (int) quality);
                hasJuceUI = true;
            }
        }

        lilv_uis_free (uis);
    }

    return hasJuceUI;
}

SuilInstance* LV2Module::createEditor()
{
    SuilInstance* instance = nullptr;
    if (LilvUIs* uis = lilv_plugin_get_uis (plugin))
    {
        LILV_FOREACH(uis, iter, uis)
        {
            const LilvUI* ui = lilv_uis_get (uis, iter);
            const unsigned quality = lilv_ui_is_supported (ui, &LV2Callbacks::uiSupported, world.ui_JuceUI, nullptr);

            if (quality == 1)
            {
                world.getFeatureArray().getFeatures();
                instance = priv->instantiateUI (ui,
                    world.ui_JuceUI, world.ui_JuceUI,
                    world.getFeatureArray().getFeatures());
                break;
            }
        }

        lilv_uis_free (uis);
    }

    return instance;
}

bool LV2Module::isPortInput (uint32 index) const
{
   return lilv_port_is_a (plugin, getPort (index), world.lv2_InputPort);
}

bool LV2Module::isPortOutput (uint32 index) const
{
   return lilv_port_is_a (plugin, getPort (index), world.lv2_OutputPort);
}

void LV2Module::run (uint32 nframes)
{
    if (worker)
        worker->processWorkResponses();
    
    lilv_instance_run (instance, nframes);
    
    if (worker)
        worker->endRun();
}

void LV2Module::setControlValue (uint32 port, float value)
{    
    if (port >= numPorts)
        return;
        
    priv->values [port] = value;
}
