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

// Change this to enable logging of various LV2 activities
#ifndef LV2_LOGGING
 #define LV2_LOGGING 0
#endif

#if LV2_LOGGING
 #define JUCE_LV2_LOG(a) Logger::writeToLog(a);
#else
 #define JUCE_LV2_LOG(a)
#endif

static ScopedPointer<URIs> uris;

class LV2PluginInstance  : public AudioPluginInstance
{
public:
    LV2PluginInstance (LV2World& world, LV2Module* module_)
        : wantsMidiMessages (false),
          initialised (false),
          isPowerOn (false),
          tempBuffer (1, 1),
          module (module_)
    {
        LV2_URID_Map* map = nullptr;
        if (LV2Feature* feat = world.getFeatureArray().getFeature (LV2_URID__map))
            map = (LV2_URID_Map*) feat->getFeature()->data;

        jassert (map != nullptr);
        jassert (module != nullptr);

        if (uris == nullptr)
            uris = new URIs (map);

        atomSequence = map->map (map->handle, LV2_ATOM__Sequence);
        midiEvent    = map->map (map->handle, LV2_MIDI__MidiEvent);

        numPorts   = module->getNumPorts();
        midiPort   = module->getMidiPort();
        notifyPort = module->getNotifyPort();

        buffers.ensureStorageAllocated (numPorts);
        while (buffers.size() < numPorts)
            buffers.add (nullptr);

        // TODO: channel/param mapping should all go in LV2Module
        const LilvPlugin* plugin (module->getPlugin());
        for (uint32 p = 0; p < numPorts; ++p)
        {
            const LilvPort* port (module->getPort (p));
            const bool input = module->isPortInput (p);
            const PortType type = module->getPortType (p);

            if (input)
            {
                if (PortType::Atom == type)
                {
                    PortBuffer* buf = new PortBuffer (uris, uris->atom_Sequence, 4096);
                    buffers.set (p, buf);
                    jassert (buf->getPortData() != nullptr);
                    module->connectPort (p, buf->getPortData());
                }
                else if (PortType::Control == type)
                {
                    LV2Parameter* param = new LV2Parameter (plugin, port);
                    params.add (param);

                    float min = 0.0f, max = 1.0f, def = 0.0f;
                    module->getPortRange (p, min, max, def);
                    param->setMinMaxValue (min, max, def);
                }
                else if (PortType::Event == type)
                {
                    buffers.set (p, new PortBuffer (uris, uris->event_Event, 4096));
                    module->connectPort (p, buffers.getUnchecked(p)->getPortData());
                }
            }
            else
            {
                if (PortType::Atom == type)
                {
                    buffers.set (p, new PortBuffer (uris, uris->atom_Sequence, 4096));
                    module->connectPort (p, buffers.getUnchecked(p)->getPortData());
                }
                else if (PortType::Control == type)
                {

                }
                else if (PortType::Event == type)
                {
                    buffers.set (p, new PortBuffer (uris, uris->event_Event, 4096));
                    module->connectPort (p, buffers.getUnchecked(p)->getPortData());
                }
            }
        }

        const ChannelConfig& channels (module->getChannelConfig());
        setPlayConfigDetails (channels.getNumAudioInputs(),
                              channels.getNumAudioOutputs(), 44100.0, 1024);
    }


    ~LV2PluginInstance()
    {
        module = nullptr;
    }

    //=========================================================================
    uint32 getNumPorts() const { return module->getNumPorts(); }
    uint32 getNumPorts (PortType type, bool isInput) const { return module->getNumPorts (type, isInput); }
    PortType getPortType (uint32 port) const { return module->getPortType (port); }
    bool isPortInput (uint32 port)     const { return module->isPortInput (port); }
    bool isPortOutput (uint32 port)    const { return module->isPortOutput (port); }
    bool writeToPort (uint32 port, uint32 size, uint32 protocol, const void* data)
    {
        const PortEvent ev = { port, protocol, (double)4.0, size };
        Logger::writeToLog ("Write: time: " + String(ev.time.decimal) + String(" frames: ") + String(ev.time.frames));
        return true;
    }

    //=========================================================================
    void fillInPluginDescription (PluginDescription& desc) const
    {
        desc.name = module->getName();

        desc.descriptiveName = String();
        if (desc.descriptiveName.isEmpty())
            desc.descriptiveName = desc.name;

        desc.fileOrIdentifier = module->getURI();
        desc.uid = desc.fileOrIdentifier.hashCode();

//        desc.lastFileModTime = 0;
        desc.pluginFormatName = "LV2";

        desc.category = module->getClassLabel();
        desc.manufacturerName = module->getAuthorName();
        desc.version = "";

        desc.numInputChannels  = module->getNumPorts (PortType::Audio, true);
        desc.numOutputChannels = module->getNumPorts (PortType::Audio, false);
        desc.isInstrument = module->getMidiPort() != LV2UI_INVALID_PORT_INDEX;
    }

    void initialise()
    {
        if (initialised)
            return;

       #if JUCE_WINDOWS
        // On Windows it's highly advisable to create your plugins using the message thread,
        // because many plugins need a chance to create HWNDs that will get their
        // messages delivered by the main message thread, and that's not possible from
        // a background thread.
        jassert (MessageManager::getInstance()->isThisTheMessageThread());
       #endif

        wantsMidiMessages = midiPort != LV2UI_INVALID_PORT_INDEX;

        initialised = true;
        setLatencySamples (0);
    }

    double getTailLengthSeconds() const { return 0.0f; }
    void* getPlatformSpecificData()  { return module->getHandle(); }
    const String getName() const     { return module->getName(); }
    bool silenceInProducesSilenceOut() const { return false; }
    bool acceptsMidi()  const        { return wantsMidiMessages; }
    bool producesMidi() const        { return notifyPort != LV2UI_INVALID_PORT_INDEX; }

    //==============================================================================
    void prepareToPlay (double sampleRate, int blockSize)
    {
        const ChannelConfig& channels (module->getChannelConfig());
        setPlayConfigDetails (channels.getNumAudioInputs(),
                              channels.getNumAudioOutputs(),
                              sampleRate, blockSize);
        initialise();

        if (initialised)
        {
            module->setSampleRate (sampleRate);
            tempBuffer.setSize (jmax (1, getTotalNumOutputChannels()), blockSize);
            module->activate();
        }
    }

    void releaseResources()
    {
        if (initialised)
            module->deactivate();

        tempBuffer.setSize (1, 1);
    }

    void processBlock (AudioSampleBuffer& audio, MidiBuffer& midi)
    {
        const int32 numSamples = audio.getNumSamples();

        if (! initialised)
        {
            for (int i = 0; i < getTotalNumOutputChannels(); ++i)
                audio.clear (i, 0, numSamples);

            return;
        }

        if (AudioPlayHead* const playHead = getPlayHead ())
        {
            AudioPlayHead::CurrentPositionInfo position;
            playHead->getCurrentPosition (position);

            if (position.isLooping)
            { }
            else
            { }
        }

        const ChannelConfig& chans (module->getChannelConfig());

        for (PortBuffer* buf : buffers) {
            if (buf)
                buf->clear();
        }

        if (wantsMidiMessages)
        {
            PortBuffer* const buf = buffers.getUnchecked (midiPort);

            MidiBuffer::Iterator iter (midi);
            const uint8* d = nullptr;  int s = 0, f = 0;

            while (iter.getNextEvent (d, s, f)) {
                buf->addEvent (f, (uint32)s, midiEvent, d);
            }
        }

        for (int32 i = getTotalNumInputChannels(); --i >= 0;)
            module->connectPort (chans.getAudioInputPort(i), audio.getWritePointer (i));

        for (int32 i = getTotalNumOutputChannels(); --i >= 0;)
            module->connectPort (chans.getAudioOutputPort(i), tempBuffer.getWritePointer (i));

        module->run ((uint32) numSamples);

        for (int32 i = getTotalNumOutputChannels(); --i >= 0;)
            audio.copyFrom (i, 0, tempBuffer.getWritePointer (i), numSamples);

        if (notifyPort != LV2UI_INVALID_PORT_INDEX)
        {
            PortBuffer* const buf = buffers.getUnchecked (notifyPort);
            jassert (buf != nullptr);

            midi.clear();
            LV2_ATOM_SEQUENCE_FOREACH ((LV2_Atom_Sequence*) buf->getPortData(), ev)
            {
                if (ev->body.type == uris->midi_MidiEvent)
                {
                    midi.addEvent (LV2_ATOM_BODY_CONST (&ev->body),
                                   ev->body.size, (int32)ev->time.frames);
                }
            }
        }
        else
        {
            midi.clear();
        }
    }

    bool hasEditor() const { return module->hasEditor(); }

    AudioProcessorEditor* createEditor();

    const String getInputChannelName (int index) const
    {
        const ChannelConfig& chans (module->getChannelConfig());
        if (! isPositiveAndBelow (index, chans.getNumAudioInputs()))
            return String ("Audio In ") + String (index + 1);
        return module->getPortName (chans.getAudioPort (index, true));
    }

    bool isInputChannelStereoPair (int index) const { return false; }

    const String getOutputChannelName (int index) const
    {
        const ChannelConfig& chans (module->getChannelConfig());
        if (! isPositiveAndBelow (index, chans.getNumAudioOutputs()))
            return String ("Audio Out ") + String (index + 1);

        return module->getPortName (chans.getAudioPort (index, false));
    }

    bool isOutputChannelStereoPair (int index) const { return false; }


    //==============================================================================
    int getNumParameters() { return params.size(); }

    const String
    getParameterName (int index)
    {
        if (isPositiveAndBelow (index, params.size()))
            return params.getUnchecked(index)->getName();
        return String();
    }


    float
    getParameter (int index)
    {
        if (isPositiveAndBelow (index, params.size()))
            return static_cast<float> (params.getUnchecked(index)->getNormalValue());

        return 0.0f;
    }

    const String getParameterText (int index)
    {
        if (! isPositiveAndBelow (index, params.size()))
            return String (getParameter (index));

        LV2Parameter* const param = params.getUnchecked (index);
        return String (static_cast<float> (param->getValue()));
    }

    void writeControlValue (int, float) { /* TODO */}

    void setParameter (int index, float newValue)
    {
        if (isPositiveAndBelow (index, params.size()))
        {
            LV2Parameter* const param = params.getUnchecked (index);
            param->setNormalValue (newValue);
            module->setControlValue (param->getPortIndex(), param->getValue());
            writeControlValue (param->getPortIndex(), param->getValue());
        }
    }


    int getParameterNumSteps (int index)
    {
        if (isPositiveAndBelow(index, params.size()))
            return params.getUnchecked(index)->getNumSteps();
        return AudioProcessor::getParameterNumSteps (index);
    }


    float getParameterDefaultValue (int index)
    {
        if (LV2Parameter* const param = params [index])
        {
            float min, max, def;
            module->getPortRange (param->getPortIndex(), min, max, def);
            return def;
        }

        return AudioProcessor::getParameterDefaultValue (index);
    }

    String getParameterLabel (int index) const
    {
        return String();
    }

    bool isParameterAutomatable (int index) const { return true; }

    //==============================================================================
    int getNumPrograms()          { return 0; }
    int getCurrentProgram()       { return 0; }
    void setCurrentProgram (int /*index*/) { }
    const String getProgramName (int /*index*/)  { return String(); }
    void changeProgramName (int /*index*/, const String& /*name*/) { }

    //==============================================================================
    void getStateInformation (MemoryBlock& mb)                  { ; }
    void getCurrentProgramStateInformation (MemoryBlock& mb)    { ; }
    void setStateInformation (const void* data, int size)               { ; }
    void setCurrentProgramStateInformation (const void* data, int size) { ; }

    //==============================================================================
    void timerCallback() { }

    void handleAsyncUpdate()
    {
        // indicates that something about the plugin has changed..
        // updateHostDisplay();
    }

private:
    CriticalSection lock, midiInLock;
    bool wantsMidiMessages, initialised, isPowerOn;
    mutable StringArray programNames;

    AudioSampleBuffer tempBuffer;
    ScopedPointer<LV2Module> module;
    OwnedArray<LV2Parameter> params;
    OwnedArray<PortBuffer> buffers;

    uint32 numPorts;
    uint32 midiPort;
    uint32 notifyPort;
    uint32 atomSequence, midiEvent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LV2PluginInstance)
};

class LV2EditorJuce : public AudioProcessorEditor
{
public:
    LV2EditorJuce (LV2PluginInstance* p, SuilInstance* si)
        : AudioProcessorEditor (p),
          instance (si),
          widget (nullptr),
          plugin (p)
    {
        widget = (Component*) suil_instance_get_widget (instance);
        addAndMakeVisible (widget);
        setSize (jmax(300, widget->getWidth()),
                 jmax(200, widget->getHeight()));
    }

    ~LV2EditorJuce()
    {
        plugin->editorBeingDeleted (this);
        removeChildComponent (widget);
        if (instance)
            suil_instance_free (instance);
    }

    void resized() {
        widget->setBounds (getLocalBounds());
    }

    SuilInstance* instance;
    Component* widget;
    LV2PluginInstance* plugin;
};

AudioProcessorEditor* LV2PluginInstance::createEditor()
{
    jassert (module->hasEditor());
    if (SuilInstance* i = module->createEditor())
        return new LV2EditorJuce (this, i);
    return nullptr;
}


class LV2PluginFormat::Internal
{
public:

    Internal()
    {
        useExternalData = false;
        world.setOwned (new LV2World ());
        init();
    }

    Internal (LV2World& w)
    {
        useExternalData = true;
        world.setNonOwned (&w);
        init();
    }

    ~Internal()
    {
        world.clear();
    }

    LV2PluginModel* createModel (const String& uri)
    {
        return world->createPluginModel (uri);
    }

    LV2Module* createModule (const String& uri)
    {
        return world->createModule (uri);
    }

    OptionalScopedPointer<LV2World> world;
    SymbolMap symbols;

private:
    bool useExternalData;

    void init()
    {
        createProvidedFeatures();
    }

    void createProvidedFeatures()
    {
        world->addFeature (symbols.createMapFeature(), false);
        world->addFeature (symbols.createUnmapFeature(), false);
        world->addFeature (symbols.createLegacyMapFeature(), false);
        world->addFeature (new LV2Log(), true);
    }
};

LV2PluginFormat::LV2PluginFormat() : priv (new Internal()) { }
LV2PluginFormat::LV2PluginFormat (LV2World& w) : priv (new Internal (w)) { }
LV2PluginFormat::~LV2PluginFormat() { priv = nullptr; }

void LV2PluginFormat::findAllTypesForFile (OwnedArray <PluginDescription>& results,
                                           const String& fileOrIdentifier)
{
    if (! fileMightContainThisPluginType (fileOrIdentifier))
        return;

    ScopedPointer<PluginDescription> desc (new PluginDescription());
    desc->fileOrIdentifier = fileOrIdentifier;
    desc->pluginFormatName = String ("LV2");
    desc->uid = 0;

    try
    {
        ScopedPointer<AudioPluginInstance> instance (createInstanceFromDescription (*desc.get(), 44100.0, 1024));
        if (LV2PluginInstance* const p = dynamic_cast <LV2PluginInstance*> (instance.get()))
        {
            p->fillInPluginDescription (*desc.get());
            results.add (desc.release());
        }
    }
    catch (...)
    {
        JUCE_LV2_LOG("crashed: " + String(desc->name));
    }
}

#if 0
AudioPluginInstance* LV2PluginFormat::createInstanceFromDescription (const PluginDescription& desc, double sampleRate, int buffersize)
{
    if (desc.pluginFormatName != String ("LV2"))
        return nullptr;

    if (LV2Module* module = priv->createModule (desc.fileOrIdentifier))
    {
        Result res (module->instantiate (sampleRate));
        if (res.wasOk())
        {
            module->activate();
            return new LV2PluginInstance (*priv->world, module);
        }
        else
        {
            JUCE_LV2_LOG (res.getErrorMessage());
            return nullptr;
        }
    }

    JUCE_LV2_LOG ("Failed creating LV2 plugin instance");
    return nullptr;
}
#endif

bool LV2PluginFormat::fileMightContainThisPluginType (const String& fileOrIdentifier)
{
    bool maybe = fileOrIdentifier.contains ("http:") ||
                 fileOrIdentifier.contains ("https:") ||
                 fileOrIdentifier.contains ("urn:");

    if (! maybe && File::isAbsolutePath (fileOrIdentifier))
    {
        const File file (fileOrIdentifier);
        maybe = file.getChildFile("manifest.ttl").existsAsFile();
    }

    return maybe;
}

String LV2PluginFormat::getNameOfPluginFromIdentifier (const String& fileOrIdentifier)
{
    if (const LilvPlugin* plugin = priv->world->getPlugin (fileOrIdentifier))
    {
        LilvNode* node = lilv_plugin_get_name (plugin);
        const String name = CharPointer_UTF8 (lilv_node_as_string (node));
        lilv_node_free (node);
    }

    return fileOrIdentifier;
}

StringArray LV2PluginFormat::searchPathsForPlugins (const FileSearchPath&, bool, bool)
{
    StringArray list;
    const LilvPlugins* plugins (priv->world->getAllPlugins());
    LILV_FOREACH (plugins, iter, plugins)
    {
        const LilvPlugin* plugin = lilv_plugins_get (plugins, iter);
        const String uri = CharPointer_UTF8 (lilv_node_as_uri (lilv_plugin_get_uri (plugin)));
        if (priv->world->isPluginSupported (uri))
            list.add (uri);
    }

    return list;
}

FileSearchPath LV2PluginFormat::getDefaultLocationsToSearch() { return FileSearchPath(); }

bool LV2PluginFormat::doesPluginStillExist (const PluginDescription& desc)
{
    StringArray plugins (searchPathsForPlugins (FileSearchPath(), true));
    return plugins.contains (desc.fileOrIdentifier);
}

void LV2PluginFormat::createPluginInstance (const PluginDescription& desc, double initialSampleRate,
                                            int initialBufferSize, void* userData,
                                            PluginCreationCallback callback)
{
    if (desc.pluginFormatName != String ("LV2"))
    {
        callback (userData, nullptr, "Not an LV2 plugin");
        return;
    }

    if (LV2Module* module = priv->createModule (desc.fileOrIdentifier))
    {
        Result res (module->instantiate (initialSampleRate));
        if (res.wasOk())
        {
            module->activate();
            auto instance = std::unique_ptr<LV2PluginInstance> (new LV2PluginInstance (*priv->world, module));
            callback (userData, instance.release(), {});
        }
        else
        {
            deleteAndZero (module);
            JUCE_LV2_LOG (res.getErrorMessage());
            callback (userData, nullptr, res.getErrorMessage());
        }
    }

    JUCE_LV2_LOG ("Failed creating LV2 plugin instance");
    callback (userData, nullptr, "Failed creating LV2 plugin instance");
}


SymbolMap& LV2PluginFormat::getSymbolMap() { return priv->symbols; }
