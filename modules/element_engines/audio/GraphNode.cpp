GraphNode::GraphNode (const uint32 nodeId_, Processor* const processor_) noexcept
    : nodeId (nodeId_),
      proc (processor_),
      isPrepared (false),
      metadata ("metadata")
{
    parent = nullptr;
    gain.set(1.0f); lastGain.set(1.0f);
    jassert (proc != nullptr);
}

bool GraphNode::isMidiIONode() const
{
    typedef GraphProcessor::AudioGraphIOProcessor IOP;
    if (IOP* iop = dynamic_cast<IOP*> (proc.get()))
        return iop->getType() == IOP::midiInputNode || iop->getType() == IOP::midiOutputNode;
    return false;
}

void GraphNode::setInputRMS (int chan, float val)
{
    if (chan < inRMS.size()) {
        inRMS.getUnchecked(chan)->set(val);
    }
}

void GraphNode::setOutputRMS (int chan, float val)
{
    if (chan < outRMS.size()) {
        outRMS.getUnchecked(chan)->set(val);
    }
}

bool GraphNode::isSubgraph() const noexcept
{
    return (dynamic_cast<GraphProcessor*> (proc.get()) != nullptr);
}

void GraphNode::prepare (const double sampleRate, const int blockSize,
                                    GraphProcessor* const graph)
{
    if (! isPrepared)
    {
        AudioPluginInstance* instance = getAudioPluginInstance();
        instance->setPlayConfigDetails (instance->getNumInputChannels(),
                                        instance->getNumOutputChannels(),
                                        sampleRate, blockSize);
        setParentGraph (graph);
        instance->prepareToPlay (sampleRate, blockSize);

        inRMS.clearQuick(true);
        for (int i = 0; i < instance->getNumInputChannels(); ++i)
        {
            AtomicValue<float>* avf = new AtomicValue<float>();
            avf->set(0);
            inRMS.add (avf);
        }

        outRMS.clearQuick(true);
        for (int i = 0; i < instance->getNumOutputChannels(); ++i)
        {
            AtomicValue<float>* avf = new AtomicValue<float>();
            avf->set(0);
            outRMS.add(avf);
        }

        isPrepared = true;
    }
}

void GraphNode::unprepare()
{
    if (isPrepared)
    {
        isPrepared = false;
        proc->releaseResources();
        parent = nullptr;
    }
}

AudioPluginInstance* GraphNode::getAudioPluginInstance() const
{
    if (PluginWrapper* wrapper = dynamic_cast<PluginWrapper*> (proc.get()))
        return wrapper->getWrappedAudioPluginInstance();

    return dynamic_cast<AudioPluginInstance*> (proc.get());
}

GraphProcessor* GraphNode::getParentGraph() const
{
    return parent;
}

void GraphNode::setParentGraph (GraphProcessor* const graph)
{
    parent = graph;
    if (GraphProcessor::AudioGraphIOProcessor* const ioProc
            = dynamic_cast<GraphProcessor::AudioGraphIOProcessor*> (proc.get()))
        ioProc->setParentGraph (graph);
    else if (GraphPort* const ioProc = dynamic_cast <GraphPort*> (proc.get()))
        ioProc->setGraph (graph);
}
