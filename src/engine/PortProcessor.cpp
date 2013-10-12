
#include "element/core.hpp"
#include "element/engine/PortProcessor.h"
#include "element/engine/graph-processor.hpp"

namespace Element {


    //==============================================================================
    PortProcessor::PortProcessor (const PortType type_, bool isInputPort)
        : portType (type_),
          graph (nullptr),
          portIsInput (isInputPort)
    {
        if (portType.id() == PortType::Audio)
        {
            isInput() ? setPlayConfigDetails (0, 1, getSampleRate(), getBlockSize())
                      : setPlayConfigDetails (1, 0, getSampleRate(), getBlockSize());
        }
    }

    PortProcessor::~PortProcessor()
    {
    }

    uint32
    PortProcessor::getNumPorts()
    {
        return uint32 (1);
    }

    PortType
    PortProcessor::getPortType (uint32 port)
    {
        return portType;
    }

    const String PortProcessor::getName() const
    {
        String name = portType.name();
        isInput() ? name << String("In") : name << String("Out");
        return name;
    }

    void PortProcessor::fillInPluginDescription (PluginDescription& d) const
    {
        d.name = getName();
        d.fileOrIdentifier = portType.uri();
        d.uid  = portType.uri().hashCode();
        d.category = "Ports";
        d.pluginFormatName = "Internal";
        d.manufacturerName = "BKE, LLC";
        d.version = "1.0";
        d.isInstrument = false;
        d.numInputChannels  = getNumInputChannels();
        d.numOutputChannels = getNumOutputChannels();
    }

    void PortProcessor::prepareToPlay (double, int)
    {
        jassert (graph != nullptr);
    }

    void PortProcessor::releaseResources()
    {
    }

    void PortProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
    {
        jassert (graph != nullptr);

        switch (portType.id())
        {
            case PortType::Audio:
            {
                if (isOutput())
                {
                    for (int i = jmin (graph->currentAudioOutputBuffer.getNumChannels(),
                                       buffer.getNumChannels()); --i >= 0;)
                    {
                        graph->currentAudioOutputBuffer.addFrom (i, 0, buffer, i, 0, buffer.getNumSamples());
                    }
                }
                else
                {
                    for (int i = jmin (graph->currentAudioInputBuffer->getNumChannels(),
                                       buffer.getNumChannels()); --i >= 0;)
                    {
                        buffer.copyFrom (i, 0, *graph->currentAudioInputBuffer, i, 0, buffer.getNumSamples());
                    }

                    break;
                }
            }
            case PortType::Atom:
            {
                if (isOutput())
                {
                    graph->currentMidiOutputBuffer.addEvents (midiMessages, 0, buffer.getNumSamples(), 0);
                }
                else
                {
                    midiMessages.addEvents (*graph->currentMidiInputBuffer, 0, buffer.getNumSamples(), 0);
                }
            }
            default:
                break;
        }
    }

    bool PortProcessor::silenceInProducesSilenceOut() const
    {
        return isOutput();
    }

    double PortProcessor::getTailLengthSeconds() const
    {
        return 0;
    }

    bool PortProcessor::acceptsMidi() const
    {
        return portType == PortType::Atom && isOutput();
    }

    bool PortProcessor::producesMidi() const
    {
        return portType ==  PortType::Atom && isInput();
    }

    const String PortProcessor::getInputChannelName (int channelIndex) const
    {
        if (PortType::Audio == portType.id() && isOutput())
            return getName() + String (channelIndex + 1);

        return String::empty;
    }

    const String PortProcessor::getOutputChannelName (int channelIndex) const
    {
        if (PortType::Audio == portType.id() && isInput())
            return getName() + String (channelIndex + 1);

        return String::empty;
    }

    bool PortProcessor::isInputChannelStereoPair (int /*index*/) const
    {
        return false;
    }

    bool PortProcessor::isOutputChannelStereoPair (int /*index*/) const
    {
        return false;
    }

    bool PortProcessor::isInput() const   { return portIsInput; }
    bool PortProcessor::isOutput() const  { return ! portIsInput; }

    #if 1
    bool PortProcessor::hasEditor() const                  { return false; }
    AudioProcessorEditor* PortProcessor::createEditor()    { return nullptr; }
    #endif
    int PortProcessor::getNumParameters()                  { return 0; }
    const String PortProcessor::getParameterName (int)     { return String::empty; }

    float PortProcessor::getParameter (int)                { return 0.0f; }
    const String PortProcessor::getParameterText (int)     { return String::empty; }
    void PortProcessor::setParameter (int, float)          { }

    int PortProcessor::getNumPrograms()                    { return 0; }
    int PortProcessor::getCurrentProgram()                 { return 0; }
    void PortProcessor::setCurrentProgram (int)            { }

    const String PortProcessor::getProgramName (int)       { return String::empty; }
    void PortProcessor::changeProgramName (int, const String&) { }

    void PortProcessor::getStateInformation (juce::MemoryBlock&) { }
    void PortProcessor::setStateInformation (const void*, int) { }

    void PortProcessor::setGraph (GraphProcessor* const newGraph)
    {
        graph = newGraph;

        if (graph != nullptr)
        {
    #if 0
            setPlayConfigDetails (portType == audioOutputNode ? graph->getNumOutputChannels() : 0,
                                  portType == audioInputNode ? graph->getNumInputChannels() : 0,
                                  getSampleRate(),
                                  getBlockSize());
    #endif
            updateHostDisplay();
        }
    }

}
