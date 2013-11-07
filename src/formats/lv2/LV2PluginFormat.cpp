/*
    LV2PluginFormat.cpp - This file is part of Element
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

#include "element/Parameter.h"
#include "element/engine/Processor.h"
#include "element/formats/lv2/LV2PluginFormat.h"
#include "element/formats/lv2/LV2Module.h"
#include "element/formats/lv2/LV2World.h"

//==============================================================================
// Change this to disable logging of various VST activities
#ifndef LV2_LOGGING
 #define LV2_LOGGING 1
#endif

#if VST_LOGGING
 #define JUCE_LV2_LOG(a) Logger::writeToLog(a);
#else
 #define JUCE_LV2_LOG(a)
#endif


namespace Element {


class LV2Parameter :  public Parameter
{
public:
    LV2Parameter (const LilvPlugin* pg, const LilvPort* pt)
        : Parameter (),
          portIndex (lilv_port_get_index (pg, pt)),
          plugin (pg),
          port (pt)

    {
        LilvNode* n = lilv_port_get_name (plugin, port);
        const LilvNode* s = lilv_port_get_symbol (plugin, port);

        setName (lilv_node_as_string (n));
        setSymbol (lilv_node_as_string (s));

        lilv_node_free (n);
    }

    uint32 getPortIndex() const { return portIndex; }

private:

    uint32 portIndex;
    const LilvPlugin* plugin;
    const LilvPort*   port;

};


//==============================================================================
class LV2PluginInstance     : public Processor
{
private:
    SymbolMap &sym;

public:

    LV2PluginInstance (LV2World& world, LV2Module* module_)
        : sym (world.symbols()),
          wantsMidiMessages (false),
          initialised (false),
          isPowerOn (false),
          tempBuffer (1, 1),
          module (module_)
    {
        assert (module != nullptr);

        numPorts   = module->getNumPorts();
        midiPort   = module->getMidiPort();
        notifyPort = ELEMENT_INVALID_PORT;

        const LilvPlugin* plugin (module->getPlugin());
        for (uint32 p = 0; p < numPorts; ++p)
        {
            const LilvPort* port (module->getPort (p));
            const bool input = module->isPortInput (p);
            const PortType type = module->getPortType (p);

            if (input)
            {
                switch (type.id())
                {
                    case PortType::Audio: audioIns.add (p); break;
                    case PortType::Atom: atomIns.add (p); break;
                    case PortType::Control: ctlIns.add(p); params.add (new LV2Parameter (plugin, port)); break;
                    case PortType::CV: cvIns.add (p); break;
                    case PortType::Unknown:
                    default:
                        break;
                }
            }
            else
            {
                switch (type.id())
                {
                    case PortType::Audio: audioOuts.add (p); break;
                    case PortType::Atom: atomOuts.add (p); break;
                    case PortType::Control: ctlOuts.add (p); break;
                    case PortType::CV: cvOuts.add (p); break;
                    case PortType::Unknown:
                    default:
                        break;
                }
            }
        }

#if JUCE_DEBUG
        for (int i = 0; i < audioIns.size(); ++i) {
            std::clog << "audio in: " << i << " to port " << audioIns.getUnchecked(i) << std::endl;
        }

        for (int i = 0; i < audioOuts.size(); ++i) {
            std::clog << "audio out: " << i << " to port " << audioOuts.getUnchecked(i) << std::endl;
        }

        for (int i = 0; i < ctlIns.size(); ++i) {
            std::clog << "ctl in: " << i << " to port " << ctlIns.getUnchecked(i) << std::endl;
        }
#endif

        setPlayConfigDetails (audioIns.size(), audioOuts.size(), 44100.0, 1024);
    }


    ~LV2PluginInstance()
    {
        module = nullptr;
    }

    //=========================================================================
    uint32 getNumPorts(){ return module->getNumPorts(); }
    uint32 getNumPorts (PortType type, bool isInput) { return module->getNumPorts (type, isInput); }
    PortType getPortType (uint32 port) { return module->getPortType (port); }
    bool isPortInput (uint32 port)     { return module->isPortInput (port); }
    bool isPortOutput (uint32 port)    { return module->isPortOutput (port); }

    //=========================================================================
    void fillInPluginDescription (PluginDescription& desc) const
    {
        desc.name = module->getName();

        desc.descriptiveName = String::empty;
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
        desc.isInstrument = false; //XXXX
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

        wantsMidiMessages = midiPort != ELEMENT_INVALID_PORT;

        initialised = true;
        setLatencySamples (0);
    }

    double getTailLengthSeconds() const { return 0.0f; }
    void* getPlatformSpecificData()  { return module->getHandle(); }
    const String getName() const     { return module->getName(); }
    bool silenceInProducesSilenceOut() const { return false; }
    bool acceptsMidi()  const        { return wantsMidiMessages; }
    bool producesMidi() const        { return notifyPort != ELEMENT_INVALID_PORT; }

    //==============================================================================
    void prepareToPlay (double sampleRate, int blockSize)
    {
        setPlayConfigDetails (audioIns.size(), audioOuts.size(), sampleRate, blockSize);
        initialise();

        if (initialised)
        {
            module->setSampleRate (sampleRate);
            tempBuffer.setSize (jmax (1, audioOuts.size()), blockSize);
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
            for (int i = 0; i < getNumOutputChannels(); ++i)
                audio.clear (i, 0, numSamples);

            return;
        }

        for (int32 i = audioIns.size(); --i >= 0;) {
            module->connectPort (audioIns.getUnchecked(i), audio.getSampleData (i));
        }

        for (int32 i = audioOuts.size(); --i >= 0;) {
            module->connectPort (audioOuts.getUnchecked(i), tempBuffer.getSampleData (i));
        }

        float value = 1.0f;
        module->connectPort (0, &value);

        module->run ((uint32) numSamples);

        for (int i = getNumOutputChannels(); --i >= 0;)
            audio.copyFrom (i, 0, tempBuffer.getSampleData (i), numSamples);
    }

#if 0
    void processBlock (AudioSampleBuffer& buffer, AtomBuffer& atoms)
    {
        // FIXME: module->readEvents();
        const int numSamples = buffer.getNumSamples();

        if (initialised)
        {
            if (AudioPlayHead* const playHead = getPlayHead())
            {
                AudioPlayHead::CurrentPositionInfo position;
                playHead->getCurrentPosition (position);

                switch (position.frameRate)
                {
                    case AudioPlayHead::fps24:
                        break;
                    case AudioPlayHead::fps25:
                        break;
                    case AudioPlayHead::fps30:
                        break;

                    default: break;
                }

                if (position.isLooping)
                { }
                else
                { }
            }

            if (wantsMidiMessages)
            {
                uint32 control (module->getMidiPort());
                if (LV2UI_INVALID_PORT_INDEX != control)
                    module->connectPort (control, atoms.getBuffer());
            }

            for (int i = getNumInputChannels(); --i >= 0;)
            {} //FIXME:   module->connectChannel (i, buffer.getSampleData(i),Bke::audioPort, true);

            for (int i = getNumOutputChannels(); --i >= 0;)
            {} //FIXME:   module->connectChannel (i, tempBuffer.getSampleData (i), Bke::audioPort, false);


            const uint32 notify (module->getNotifyPort());

            if (notify != LV2UI_INVALID_PORT_INDEX)
            {
                atomBuffer.clear (false);
                module->connectPort (notify, atomBuffer.getBuffer());
            }

            module->process (uint32 (numSamples));

            // clear input atoms, to make room for notifications
            atoms.clear (true);

            if (notify != LV2UI_INVALID_PORT_INDEX)
            {
                atoms.addEvents (atomBuffer, numSamples);
#if 0
                LV2_ATOM_SEQUENCE_FOREACH (atomBuffer.getAtomSequence(), ev)
                {
                    String blank (LV2_ATOM__Blank);
                    lvtk::Atom atom (&ev->body);
                    lvtk::AtomObject obj (atom.as_object());

                    std::cout << "blank: " << sym.unmap ((uint32_t) blank.hashCode()) << std::endl;
                    std::cout << "frames: " << ev->time.frames
                              << " type: " << ev->body.type << " size: " << ev->body.size << " => " << sym.unmap (ev->body.type) << "\n"
                              << " otype: " << obj.otype() << " => " << sym.unmap (obj.otype()) << "\n"
                              << "----- " << std::endl;
                }
#endif
            }

            for (int i = getNumOutputChannels(); --i >= 0;)
                buffer.copyFrom (i, 0, tempBuffer.getSampleData (i), numSamples);
        }
        else
        {
            // Not initialised, so just bypass..
            for (int i = 0; i < getNumOutputChannels(); ++i)
                buffer.clear (i, 0, numSamples);
        }

        {
            // copy any incoming midi..
            // const ScopedLock sl (midiInLock);
            // midiMessages.swapWith (incomingMidi);
            // incomingMidi.clear();
        }
    }
#endif

    //==============================================================================
    bool hasEditor() const { return false; }
    AudioProcessorEditor* createEditor() { return nullptr; }

    //==============================================================================
    const String getInputChannelName (int index) const
    {
        return String::empty;
    }

    bool isInputChannelStereoPair (int index) const { return false; }

    const String getOutputChannelName (int index) const
    {
        return String::empty;
    }

    bool isOutputChannelStereoPair (int index) const { return false; }

    //==============================================================================
    int getNumParameters() { return params.size(); }

    float getParameter (int index)
    {
        if (isPositiveAndBelow (index, params.size()))
            return params[index]->normal();
        return 0.0f;
    }

    void setParameter (int index, float newValue)
    {
        if (isPositiveAndBelow (index, params.size()))
            params.getUnchecked(index)->setNormal (newValue);
    }

    const String getParameterName (int index)
    {
        if (isPositiveAndBelow (index, params.size()))
            return params.getUnchecked(index)->getName();
        return String::empty;
    }

    const String getParameterText (int index)
    {
        return String::empty;
    }

    String getParameterLabel (int index) const
    {
        return String::empty;
    }

    bool isParameterAutomatable (int index) const { return true; }

    //==============================================================================
    int getNumPrograms()          { return 0; }
    int getCurrentProgram()       { return 0; }
    void setCurrentProgram (int /*index*/) { }
    const String getProgramName (int /*index*/)  { return String::empty; }
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
    OwnedArray<Parameter> params;

    uint32 numPorts;
    uint32 midiPort;
    uint32 notifyPort;

    // port/channel maps
    Array<uint32> audioIns, audioOuts, atomIns, atomOuts;
    Array<uint32> ctlIns, ctlOuts, cvIns, cvOuts, evIns, evOuts;

    const char* getCategory() const { return module->getClassLabel().toUTF8(); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LV2PluginInstance)
};


class LV2PluginFormat::Private
{
public:

    Private (LV2World& w)
    {
        world.setNonOwned (&w);

    }

    LV2Module* createModule (const String& uri)
    {
        if (LV2Module* module = world->createModule (uri))
        {
            module->init();
            return module;
        }

        return nullptr;
    }

    OptionalScopedPointer<LV2World> world;
    LV2FeatureArray features;
};

LV2PluginFormat::LV2PluginFormat (LV2World& w) : priv (new LV2PluginFormat::Private (w)) { }
LV2PluginFormat::~LV2PluginFormat() { priv = nullptr; }

SymbolMap& LV2PluginFormat::symbols() { return priv->world->symbols(); }

void
LV2PluginFormat::findAllTypesForFile (OwnedArray <PluginDescription>& results,
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
        String msg = "Discovering LV2 Plugin: ";
        msg << fileOrIdentifier;
        JUCE_LV2_LOG (msg);

        ScopedPointer<AudioPluginInstance> instance (createInstanceFromDescription (*desc.get(), 44100, 1024));
        if (LV2PluginInstance* const p = dynamic_cast <LV2PluginInstance*> (instance.get()))
        {
            p->fillInPluginDescription (*desc.get());
            results.add (desc.release());
        }
    }
    catch (...)
    {
        // crashed while loading...
    }
}

AudioPluginInstance*
LV2PluginFormat::createInstanceFromDescription (const PluginDescription& desc, double sampleRate, int buffersize)
{
    if (desc.pluginFormatName != String ("LV2"))
        return nullptr;

    JUCE_LV2_LOG ("Creating LV2 Module");

    if (LV2Module* module = priv->createModule (desc.fileOrIdentifier))
    {
        JUCE_LV2_LOG ("  instantiating from module.");

        module->instantiate (sampleRate, priv->features);
        return new LV2PluginInstance (*priv->world, module);
    }

    JUCE_LV2_LOG ("Failed creating LV2 plugin instance");

    return nullptr;
}

bool LV2PluginFormat::fileMightContainThisPluginType (const String& fileOrIdentifier)
{
    bool maybe = fileOrIdentifier.contains ("http:") ||
                 fileOrIdentifier.contains ("urn.");

    if (! maybe)
    {
        // TODO: check if its an .lv2 bundle
    }

    return maybe;
}

String
LV2PluginFormat::getNameOfPluginFromIdentifier (const String& fileOrIdentifier)
{
    return fileOrIdentifier;
}

StringArray
LV2PluginFormat::searchPathsForPlugins (const FileSearchPath&, bool)
{
    StringArray list;
    Lilv::Plugins plugins (priv->world->getAllPlugins());

    LILV_FOREACH (plugins, iter, plugins)
    {
        Lilv::Plugin plugin (plugins.get (iter));
        String uri (plugin.get_uri().as_string());
        if (priv->world->isPluginSupported (uri))
            list.add (uri);
    }

    return list;
}

FileSearchPath LV2PluginFormat::getDefaultLocationsToSearch() { return FileSearchPath(); }

bool
LV2PluginFormat::doesPluginStillExist (const PluginDescription& desc)
{
    FileSearchPath placeholder;
    StringArray plugins (searchPathsForPlugins (placeholder, true));
    return plugins.contains (desc.fileOrIdentifier);
}

}
