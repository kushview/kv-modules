/*
    This file is part of the Kushview Modules for JUCE
    Copyright (c) 2014-2019  Kushview, LLC.  All rights reserved.

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

#include <lv2/state/state.h>

namespace kv {

class LV2State
{
public:
    LV2State() = default;
    ~LV2State() = default;

    void insert (uint32 key, const void* value, size_t size, uint32 type)
    {
        auto* const entry = entries.add (new Entry());
        entry->key = key;
        entry->data.allocate (size, true);
        memcpy (entry->data, value, size);
        entry->size = size;
        entry->type = type;
    }

    const void* retrieve (uint32 key, size_t* size, uint32* type) const
    {
        for (const auto* entry : entries)
        {
            if (key == entry->key)
            {
                *size = entry->size;
                *type = entry->type;
                return (const void*) entry->data.getData();
            }
        }

        return nullptr;
    }

private:
    struct Entry
    {
        uint32 key;
        HeapBlock<uint8> data;
        size_t size;
        uint32 type;
    };

    OwnedArray<Entry> entries;
};

namespace LV2Callbacks {

inline unsigned uiSupported (const char* hostType, const char* uiType)
{
    String host (hostType);
    String ui (uiType);

    if (host == LV2_UI__X11UI && ui == LV2_UI__X11UI)
        return 2;
    else if (ui == LVTK__JUCEUI)
        return 1;

    return suil_ui_supported (hostType, uiType);
}

LV2_State_Status store (LV2_State_Handle handle,
                        uint32_t         key,
                        const void*      value,
                        size_t           size,
                        uint32_t         type,
                        uint32_t         flags)
{
    if ((flags & LV2_STATE_IS_POD && flags & LV2_STATE_IS_PORTABLE))
    {
        LV2State* const state = static_cast<LV2State*> (handle);
        state->insert (key, value, size, type);
        return LV2_STATE_SUCCESS;
    }

    return LV2_STATE_ERR_BAD_FLAGS;
}

const void* retrieve (LV2_State_Handle handle, uint32_t key, 
                      size_t *size, uint32_t *type, uint32_t *flags)
{
    LV2State* const state = static_cast<LV2State*> (handle);
    *flags = 0;
    return state->retrieve (key, size, type);
}

}

class LV2Module::Private
{
public:
    Private (LV2Module& module)
        : owner (module) { }

    ~Private() { }

    LV2ModuleUI* instantiateUI (const LilvUI* uiNode,
                                const LilvNode* containerType,
                                const LilvNode* widgetType,
                                const LV2_Feature* const * features)
    {
       #if 1
        suil = owner.getWorld().getSuilHost();
        const LilvNode* uri = lilv_ui_get_uri (uiNode);
        const LilvPlugin* plugin = owner.getPlugin();
        auto uiptr = std::unique_ptr<LV2ModuleUI> (new LV2ModuleUI (owner.getWorld(), owner));

        auto* instance = suil_instance_new (suil, uiptr.get(),
                            lilv_node_as_uri (containerType),
                            lilv_node_as_uri (lilv_plugin_get_uri (plugin)),
                            lilv_node_as_uri (uri),
                            lilv_node_as_uri (widgetType),
                            lilv_uri_to_path (lilv_node_as_uri (lilv_ui_get_bundle_uri (uiNode))),
                            lilv_uri_to_path (lilv_node_as_uri (lilv_ui_get_binary_uri (uiNode))),
                            features);

        if (instance != nullptr)
        {
            uiptr->instance = instance;
            ui = uiptr.release();
        }
       #endif
        return ui;
    }

    ChannelConfig channels;
    HeapBlock<float> mins, maxes, defaults, values;
    LV2ModuleUI::Ptr ui;

    void sendControlValues()
    {
        if (! ui && ! owner.onPortNotify)
            return;

        for (const auto* port : ports.getPorts())
        {
            if (PortType::Control != port->type)
                continue;

            if (ui)
                ui->portEvent ((uint32_t)port->index, sizeof(float), 
                               0, (void*) &values [port->index]);
            if (owner.onPortNotify)
                owner.onPortNotify ((uint32_t)port->index, sizeof(float), 
                                    0, (void*) &values [port->index]);
        }
    }

    static const void * getPortValue (const char *port_symbol, void *user_data, uint32_t *size, uint32_t *type)
    {
        LV2Module::Private* priv = static_cast<LV2Module::Private*> (user_data);
        int portIdx = -1;
        for (const auto* port : priv->ports.getPorts())
        {
            if (port->symbol == port_symbol && port->type == PortType::Control) {
                portIdx = port->index;
                break;
            }
        }

        if (portIdx >= 0)
        {
            *size = sizeof (float);
            *type = priv->owner.map (LV2_ATOM__Float);
            return &priv->values [portIdx];
        }
        else
        {
            *size = 0;
            *type = 0;
        }

        return nullptr;
    }

    static void setPortValue (const char* port_symbol,
                                      void*       user_data,
                                      const void* value,
                                      uint32_t    size,
                                      uint32_t    type)
    {
        auto* priv = (Private*) user_data;
        auto& plugin = priv->owner;

        if (type != priv->owner.map (LV2_ATOM__Float))
            return;

        int portIdx = -1;
        const PortDescription* port = nullptr;
        for (const auto* p : priv->ports.getPorts())
        {
            port = p;
            if (port->symbol == port_symbol && port->type == PortType::Control) {
                portIdx = port->index;
                break;
            }
        }

        if (portIdx >= 0 && port)
        {
            priv->values[portIdx] = *((float*) value);
        }
    }
    
private:
    friend class LV2Module;
    LV2Module& owner;
    SuilHost* suil;
    SuilInstance* instanceUI = 0;
    PortList ports;
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
   for (int32 p = 0; p < numPorts; ++p)
   {
       const bool isInput  = isPortInput (p);
       const PortType type = getPortType (p);

       if (type == PortType::Atom)
       {

       }
       else if (type == PortType::Audio)
       {

       }
       else if (type == PortType::Control)
       {
           connectPort (p, &priv->values [p]);
       }
       else if (type == PortType::CV)
       {

       }
   }
}

void LV2Module::init()
{
    events.reset (new RingBuffer (4096));
    evbufsize = jmax (evbufsize, static_cast<uint32> (256));
    evbuf.realloc (evbufsize);
    evbuf.clear (evbufsize);

    notifications.reset (new RingBuffer (4096));
    ntbufsize = jmax (ntbufsize, static_cast<uint32> (256));
    ntbuf.realloc (ntbufsize);
    ntbuf.clear (ntbufsize);

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
        const auto type = getPortType (p);
        const bool isInput (lilv_port_is_a (plugin, port, world.lv2_InputPort));
        LilvNode* nameNode = lilv_port_get_name (plugin, port);
        const String name = lilv_node_as_string (nameNode);
        lilv_node_free (nameNode); nameNode = nullptr;
        const String symbol  = lilv_node_as_string (lilv_port_get_symbol (plugin, port));
        priv->ports.add (type, p, priv->ports.size (type, isInput),
                         symbol, name, isInput);
        priv->channels.addPort (type, p, isInput);
        priv->values[p] = priv->defaults[p];
    }

    // load related GUIs
    auto* related = lilv_plugin_get_related (plugin, world.ui_UI);
    LILV_FOREACH (nodes, iter, related)
    {
        const auto* res = lilv_nodes_get (related, iter);
        lilv_world_load_resource (world.getWorld(), res);
    }
    lilv_nodes_free (related);
}


String LV2Module::getStateString() const
{
    if (instance == nullptr)
        return String();
    
    auto* const map = (LV2_URID_Map*) world.getFeatureArray().getFeature (LV2_URID__map)->getFeature()->data;
    auto* const unmap = (LV2_URID_Unmap*) world.getFeatureArray().getFeature (LV2_URID__unmap)->getFeature()->data;
    const String descURI = "http://kushview.net/kv/state";

    String result;
    const LV2_Feature* const features[] = { nullptr };
    
    if (auto* state = lilv_state_new_from_instance (plugin, instance, 
        map, 0, 0, 0, 0, 
        Private::getPortValue, priv.get(), 
        LV2_STATE_IS_POD, // flags
        features))
    {
        char* strState = lilv_state_to_string (world.getWorld(), map, unmap, state, descURI.toRawUTF8(), 0);
        result = String::fromUTF8 (strState);
        std::free (strState);

        lilv_state_free (state);
    }

    return result;
}

void LV2Module::setStateString (const String& stateStr)
{
    if (instance == nullptr)
        return;
    auto* const map = (LV2_URID_Map*) world.getFeatureArray().getFeature (LV2_URID__map)->getFeature()->data;
    auto* const unmap = (LV2_URID_Unmap*) world.getFeatureArray().getFeature (LV2_URID__unmap)->getFeature()->data;
    if (auto* state = lilv_state_new_from_string (world.getWorld(), map, stateStr.toRawUTF8()))
    {
        const LV2_Feature* const features[] = { nullptr };
        lilv_state_restore (state, instance, Private::setPortValue, 
                            priv.get(), LV2_STATE_IS_POD, features);
        lilv_state_free (state);
        priv->sendControlValues();
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
        jassert (worker != nullptr);
        worker->setSize (2048);
        worker->setInterface (lilv_instance_get_handle (instance),
                              (LV2_Worker_Interface*) data);
    }
    else if (worker)
    {
        features.removeFirstMatchingValue (worker->getFeature());
        worker = nullptr;
    }

    startTimerHz (60);
    return Result::ok();
}

void LV2Module::activate()
{
   if (instance && ! active)
   {
       lilv_instance_activate (instance);
       activatePorts();
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
    stopTimer();
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
   return String();
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
   return String();
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

   return String();
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

    return String();
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
    if (bestUI.isNotEmpty())
        return true;

    LilvUIs* uis = lilv_plugin_get_uis (plugin);
    if (nullptr == uis)
        return false;

    LILV_FOREACH (uis, iter, uis)
    {
        const LilvUI* ui = lilv_uis_get (uis, iter);
        const unsigned quality = lilv_ui_is_supported (ui, &LV2Callbacks::uiSupported, world.ui_JUCEUI, nullptr);
        const auto uri = String::fromUTF8 (lilv_node_as_string (lilv_ui_get_uri (ui)));
        
        if (quality == 1)
        {
            bestUI = uri;
            break;
        }
    }

    lilv_uis_free (uis);
    
    return bestUI.isNotEmpty();
}

void LV2Module::clearEditor()
{
    if (priv->ui)
    {
        auto ui = priv->ui;
        priv->ui = nullptr;
        ui->unload();
        ui = nullptr;
    }
}

uint32 LV2Module::getPortIndex (const String& symbol) const
{
    for (const auto* port : priv->ports.getPorts())
        if (port->symbol == symbol)
            return static_cast<uint32> (port->index);
    return LV2UI_INVALID_PORT_INDEX;
}

LV2ModuleUI* LV2Module::createEditor()
{
    if (priv->ui)
        return priv->ui;
    
    LV2ModuleUI* instance = nullptr;

    if (LilvUIs* uis = lilv_plugin_get_uis (plugin))
    {
        LILV_FOREACH(uis, iter, uis)
        {
            const LilvUI* uiNode = lilv_uis_get (uis, iter);
            const unsigned quality = lilv_ui_is_supported (uiNode, &LV2Callbacks::uiSupported, world.ui_JUCEUI, nullptr);

            if (quality == 1)
            {
                instance = priv->instantiateUI (uiNode,
                    world.ui_JUCEUI, world.ui_JUCEUI,
                    world.getFeatureArray().getFeatures());
                break;
            }
        }

        lilv_uis_free (uis);
    }

    if (instance != nullptr)
    {
        jassert (instance == priv->ui.get());
        priv->sendControlValues();
    }

    return instance;
}

void LV2Module::sendPortEvents()
{
    priv->sendControlValues();
}

bool LV2Module::isPortInput (uint32 index) const
{
   return lilv_port_is_a (plugin, getPort (index), world.lv2_InputPort);
}

bool LV2Module::isPortOutput (uint32 index) const
{
   return lilv_port_is_a (plugin, getPort (index), world.lv2_OutputPort);
}

void LV2Module::timerCallback()
{
    PortEvent ev;
    
    static const uint32 pnsize = sizeof (PortEvent);

    for (;;)
    {
        if (! notifications->canRead (pnsize))
            break;

        notifications->read (ev, false);
        if (ev.size > 0 && notifications->canRead (pnsize + ev.size))
        {
            notifications->advance (pnsize, false);
            notifications->read (ntbuf, ev.size, true);

            if (ev.protocol == 0)
            {
                if (auto ui = priv->ui)
                    ui->portEvent (ev.index, ev.size, ev.protocol, ntbuf.getData());
                if (onPortNotify)
                    onPortNotify (ev.index, ev.size, ev.protocol, ntbuf.getData());
            }        
        }
    }
}

void LV2Module::run (uint32 nframes)
{
    PortEvent ev;
    
    static const uint32 pesize = sizeof (PortEvent);

    for (;;)
    {
        if (! events->canRead (pesize))
            break;
        events->read (ev, false);
        if (ev.size > 0 && events->canRead (pesize + ev.size))
        {
            events->advance (pesize, false);
            events->read (evbuf, ev.size, true);

            if (ev.protocol == 0)
            {
                const bool changed = priv->values[ev.index] != *((float*) evbuf.getData());
                priv->values[ev.index] = *((float*) evbuf.getData());
                if (changed && notifications->canWrite (pesize + ev.size))
                {
                    notifications->write (ev);
                    notifications->write (evbuf.getData(), ev.size);
                }
            }
        }
    }

    for (const auto* const port : priv->ports.getPorts())
        if (port->type == PortType::Control)
            connectPort (static_cast<uint32> (port->index), &priv->values[port->index]);
    
    if (worker)
        worker->processWorkResponses();

    lilv_instance_run (instance, nframes);

    if (worker)
        worker->endRun();
}

uint32 LV2Module::map (const String& uri) const
{
    if (const auto* f = world.getFeatureArray().getFeature (LV2_URID__map))
    {
        auto* map = (LV2_URID_Map*) f->getFeature()->data; 
        return map->map (map->handle, uri.toRawUTF8());
    }
    return 0;
}

void LV2Module::write (uint32 port, uint32 size, uint32 protocol, const void* buffer)
{
    PortEvent event;
    zerostruct (event);
    event.index     = port;
    event.size      = size;
    event.protocol  = protocol;

    if (events->canWrite (sizeof (PortEvent) + size))
    {
        events->write (event);
        events->write (buffer, event.size);
    }
    else
    {
        DBG("[kv] lv2 plugin write buffer full.");
    }
}

}
