
//==============================================================================
namespace GraphRender
{

//==============================================================================
class Task
{
public:
    Task() { }
    virtual ~Task()  { }

    virtual void perform (AudioSampleBuffer& sharedBufferChans,
                          const OwnedArray <MidiBuffer>& sharedMidiBuffers,
                          const int numSamples) = 0;

    JUCE_LEAK_DETECTOR (Task)
};

//==============================================================================
class ClearChannelOp : public Task
{
public:
    ClearChannelOp (const int channelNum_)
        : channelNum (channelNum_)
    {}

    void perform (AudioSampleBuffer& sharedBufferChans, const OwnedArray <MidiBuffer>&, const int numSamples)
    {
        sharedBufferChans.clear (channelNum, 0, numSamples);
    }

private:
    const int channelNum;

    JUCE_DECLARE_NON_COPYABLE (ClearChannelOp)
};

//==============================================================================
class CopyChannelOp : public Task
{
public:
    CopyChannelOp (const int srcChannelNum_, const int dstChannelNum_)
        : srcChannelNum (srcChannelNum_),
          dstChannelNum (dstChannelNum_)
    {}

    void perform (AudioSampleBuffer& sharedBufferChans, const OwnedArray <MidiBuffer>&, const int numSamples)
    {
        sharedBufferChans.copyFrom (dstChannelNum, 0, sharedBufferChans, srcChannelNum, 0, numSamples);
    }

private:
    const int srcChannelNum, dstChannelNum;

    JUCE_DECLARE_NON_COPYABLE (CopyChannelOp)
};

//==============================================================================
class AddChannelOp : public Task
{
public:
    AddChannelOp (const int srcChannelNum_, const int dstChannelNum_)
        : srcChannelNum (srcChannelNum_),
          dstChannelNum (dstChannelNum_)
    {}

    void perform (AudioSampleBuffer& sharedBufferChans, const OwnedArray <MidiBuffer>&, const int numSamples)
    {
        sharedBufferChans.addFrom (dstChannelNum, 0, sharedBufferChans, srcChannelNum, 0, numSamples);
    }

private:
    const int srcChannelNum, dstChannelNum;

    JUCE_DECLARE_NON_COPYABLE (AddChannelOp)
};

//==============================================================================
class ClearMidiBufferOp : public Task
{
public:
    ClearMidiBufferOp (const int bufferNum_)
        : bufferNum (bufferNum_)
    {}

    void perform (AudioSampleBuffer&, const OwnedArray <MidiBuffer>& sharedMidiBuffers, const int)
    {
        sharedMidiBuffers.getUnchecked (bufferNum)->clear();
    }

private:
    const int bufferNum;

    JUCE_DECLARE_NON_COPYABLE (ClearMidiBufferOp)
};

//==============================================================================
class CopyMidiBufferOp : public Task
{
public:
    CopyMidiBufferOp (const int srcBufferNum_, const int dstBufferNum_)
        : srcBufferNum (srcBufferNum_),
          dstBufferNum (dstBufferNum_)
    {}

    void perform (AudioSampleBuffer&, const OwnedArray <MidiBuffer>& sharedMidiBuffers, const int)
    {
        *sharedMidiBuffers.getUnchecked (dstBufferNum) = *sharedMidiBuffers.getUnchecked (srcBufferNum);
    }

private:
    const int srcBufferNum, dstBufferNum;

    JUCE_DECLARE_NON_COPYABLE (CopyMidiBufferOp)
};

//==============================================================================
class AddMidiBufferOp : public Task
{
public:
    AddMidiBufferOp (const int srcBufferNum_, const int dstBufferNum_)
        : srcBufferNum (srcBufferNum_),
          dstBufferNum (dstBufferNum_)
    { }

    void perform (AudioSampleBuffer&, const OwnedArray <MidiBuffer>& sharedMidiBuffers, const int numSamples)
    {
        sharedMidiBuffers.getUnchecked (dstBufferNum)
            ->addEvents (*sharedMidiBuffers.getUnchecked (srcBufferNum), 0, numSamples, 0);
    }

private:
    const int srcBufferNum, dstBufferNum;

    JUCE_DECLARE_NON_COPYABLE (AddMidiBufferOp)
};

//==============================================================================
class DelayChannelOp : public Task
{
public:
    DelayChannelOp (const int channel_, const int numSamplesDelay_)
        : channel (channel_),
          bufferSize (numSamplesDelay_ + 1),
          readIndex (0), writeIndex (numSamplesDelay_)
    {
        buffer.calloc ((size_t) bufferSize);
    }

    void perform (AudioSampleBuffer& sharedBufferChans, const OwnedArray <MidiBuffer>&, const int numSamples)
    {
        float* data = sharedBufferChans.getSampleData (channel, 0);

        for (int i = numSamples; --i >= 0;)
        {
            buffer [writeIndex] = *data;
            *data++ = buffer [readIndex];

            if (++readIndex  >= bufferSize) readIndex = 0;
            if (++writeIndex >= bufferSize) writeIndex = 0;
        }
    }

private:
    HeapBlock<float> buffer;
    const int channel, bufferSize;
    int readIndex, writeIndex;

    JUCE_DECLARE_NON_COPYABLE (DelayChannelOp)
};


//==============================================================================
class ProcessBufferOp : public Task
{
public:
    ProcessBufferOp (const ProcessorGraph::Node::Ptr& node_,
                     const Array <int>& audioChannelsToUse_,
                     const int totalChans_,
                     const int midiBufferToUse_)
        : node (node_),
          processor (node_->getProcessor()),
          audioChannelsToUse (audioChannelsToUse_),
          totalChans (jmax (1, totalChans_)),
          midiBufferToUse (midiBufferToUse_)
    {
        channels.calloc ((size_t) totalChans);

        while (audioChannelsToUse.size() < totalChans)
            audioChannelsToUse.add (0);
    }

    void perform (AudioSampleBuffer& sharedBufferChans, const OwnedArray <MidiBuffer>& sharedMidiBuffers, const int numSamples)
    {
        for (int i = totalChans; --i >= 0;)
            channels[i] = sharedBufferChans.getSampleData (audioChannelsToUse.getUnchecked (i), 0);

        AudioSampleBuffer buffer (channels, totalChans, numSamples);

        //processor->processBlock (buffer, *sharedMidiBuffers.getUnchecked (midiBufferToUse));
    }

    const ProcessorGraph::Node::Ptr node;
    AudioProcessor* const processor;

private:
    Array <int> audioChannelsToUse;
    HeapBlock <float*> channels;
    int totalChans;
    int midiBufferToUse;

    JUCE_DECLARE_NON_COPYABLE (ProcessBufferOp)
};

//==============================================================================
/** Used to calculate the correct sequence of rendering ops needed, based on
    the best re-use of shared buffers at each stage.
*/
class ProcessorGraphBuilder
{
public:
    //==============================================================================
    ProcessorGraphBuilder (ProcessorGraph& graph_,
                                   const Array<void*>& orderedNodes_,
                                   Array<void*>& renderingOps)
        : graph (graph_),
          orderedNodes (orderedNodes_),
          totalLatency (0)
    {
        nodeIds.add ((uint32) zeroNodeID); // first buffer is read-only zeros
        channels.add (0);

        midiNodeIds.add ((uint32) zeroNodeID);

        for (int i = 0; i < orderedNodes.size(); ++i)
        {
            createRenderingOpsForNode ((ProcessorGraph::Node*) orderedNodes.getUnchecked(i),
                                       renderingOps, i);

            markAnyUnusedBuffersAsFree (i);
        }

        //XXX: graph.setLatencySamples (totalLatency);
    }

    int getNumBuffersNeeded() const         { return nodeIds.size(); }
    int getNumMidiBuffersNeeded() const     { return midiNodeIds.size(); }

private:
    //==============================================================================
    ProcessorGraph& graph;
    const Array<void*>& orderedNodes;
    Array <int> channels;
    Array <uint32> nodeIds, midiNodeIds;

    enum { freeNodeID = 0xffffffff, zeroNodeID = 0xfffffffe };

    static bool isNodeBusy (uint32 nodeID) noexcept { return nodeID != freeNodeID && nodeID != zeroNodeID; }

    Array <uint32> nodeDelayIDs;
    Array <int> nodeDelays;
    int totalLatency;

    int getNodeDelay (const uint32 nodeID) const          { return nodeDelays [nodeDelayIDs.indexOf (nodeID)]; }

    void setNodeDelay (const uint32 nodeID, const int latency)
    {
        const int index = nodeDelayIDs.indexOf (nodeID);

        if (index >= 0)
        {
            nodeDelays.set (index, latency);
        }
        else
        {
            nodeDelayIDs.add (nodeID);
            nodeDelays.add (latency);
        }
    }

    int getInputLatency (const uint32 nodeID) const
    {
        int maxLatency = 0;

        for (int i = graph.getNumConnections(); --i >= 0;)
        {
            const ProcessorGraph::Connection* const c = graph.getConnection (i);

            if (c->destNode == nodeID)
                maxLatency = jmax (maxLatency, getNodeDelay (c->sourceNode));
        }

        return maxLatency;
    }

    //==============================================================================
    void createRenderingOpsForNode (ProcessorGraph::Node* const node,
                                    Array<void*>& renderingOps,
                                    const int ourRenderingIndex)
    {
        const int numIns  = node->getProcessor()->getNumInputChannels();
        const int numOuts = node->getProcessor()->getNumOutputChannels();
        const int totalChans = jmax (numIns, numOuts);

        Array <int> audioChannelsToUse;
        int midiBufferToUse = -1;

        int maxLatency = getInputLatency (node->nodeId);

        for (int inputChan = 0; inputChan < numIns; ++inputChan)
        {
            // get a list of all the inputs to this node
            Array <uint32> sourceNodes;
            Array<int> sourceOutputChans;

            for (int i = graph.getNumConnections(); --i >= 0;)
            {
                const ProcessorGraph::Connection* const c = graph.getConnection (i);

                if (c->destNode == node->nodeId && c->destPort == inputChan)
                {
                    sourceNodes.add (c->sourceNode);
                    sourceOutputChans.add (c->sourcePort);
                }
            }

            int bufIndex = -1;

            if (sourceNodes.size() == 0)
            {
                // unconnected input channel

                if (inputChan >= numOuts)
                {
                    bufIndex = getReadOnlyEmptyBuffer();
                    jassert (bufIndex >= 0);
                }
                else
                {
                    bufIndex = getFreeBuffer (false);
                    renderingOps.add (new ClearChannelOp (bufIndex));
                }
            }
            else if (sourceNodes.size() == 1)
            {
                // channel with a straightforward single input..
                const uint32 srcNode = sourceNodes.getUnchecked(0);
                const int srcChan = sourceOutputChans.getUnchecked(0);

                bufIndex = getBufferContaining (srcNode, srcChan);

                if (bufIndex < 0)
                {
                    // if not found, this is probably a feedback loop
                    bufIndex = getReadOnlyEmptyBuffer();
                    jassert (bufIndex >= 0);
                }

                if (inputChan < numOuts
                     && isBufferNeededLater (ourRenderingIndex,
                                             inputChan,
                                             srcNode, srcChan))
                {
                    // can't mess up this channel because it's needed later by another node, so we
                    // need to use a copy of it..
                    const int newFreeBuffer = getFreeBuffer (false);

                    renderingOps.add (new CopyChannelOp (bufIndex, newFreeBuffer));

                    bufIndex = newFreeBuffer;
                }

                const int nodeDelay = getNodeDelay (srcNode);

                if (nodeDelay < maxLatency)
                    renderingOps.add (new DelayChannelOp (bufIndex, maxLatency - nodeDelay));
            }
            else
            {
                // channel with a mix of several inputs..

                // try to find a re-usable channel from our inputs..
                int reusableInputIndex = -1;

                for (int i = 0; i < sourceNodes.size(); ++i)
                {
                    const int sourceBufIndex = getBufferContaining (sourceNodes.getUnchecked(i),
                                                                    sourceOutputChans.getUnchecked(i));

                    if (sourceBufIndex >= 0
                        && ! isBufferNeededLater (ourRenderingIndex,
                                                  inputChan,
                                                  sourceNodes.getUnchecked(i),
                                                  sourceOutputChans.getUnchecked(i)))
                    {
                        // we've found one of our input chans that can be re-used..
                        reusableInputIndex = i;
                        bufIndex = sourceBufIndex;

                        const int nodeDelay = getNodeDelay (sourceNodes.getUnchecked (i));
                        if (nodeDelay < maxLatency)
                            renderingOps.add (new DelayChannelOp (sourceBufIndex, maxLatency - nodeDelay));

                        break;
                    }
                }

                if (reusableInputIndex < 0)
                {
                    // can't re-use any of our input chans, so get a new one and copy everything into it..
                    bufIndex = getFreeBuffer (false);
                    jassert (bufIndex != 0);

                    const int srcIndex = getBufferContaining (sourceNodes.getUnchecked (0),
                                                              sourceOutputChans.getUnchecked (0));
                    if (srcIndex < 0)
                    {
                        // if not found, this is probably a feedback loop
                        renderingOps.add (new ClearChannelOp (bufIndex));
                    }
                    else
                    {
                        renderingOps.add (new CopyChannelOp (srcIndex, bufIndex));
                    }

                    reusableInputIndex = 0;
                    const int nodeDelay = getNodeDelay (sourceNodes.getFirst());

                    if (nodeDelay < maxLatency)
                        renderingOps.add (new DelayChannelOp (bufIndex, maxLatency - nodeDelay));
                }

                for (int j = 0; j < sourceNodes.size(); ++j)
                {
                    if (j != reusableInputIndex)
                    {
                        int srcIndex = getBufferContaining (sourceNodes.getUnchecked(j),
                                                            sourceOutputChans.getUnchecked(j));
                        if (srcIndex >= 0)
                        {
                            const int nodeDelay = getNodeDelay (sourceNodes.getUnchecked (j));

                            if (nodeDelay < maxLatency)
                            {
                                if (! isBufferNeededLater (ourRenderingIndex, inputChan,
                                                           sourceNodes.getUnchecked(j),
                                                           sourceOutputChans.getUnchecked(j)))
                                {
                                    renderingOps.add (new DelayChannelOp (srcIndex, maxLatency - nodeDelay));
                                }
                                else // buffer is reused elsewhere, can't be delayed
                                {
                                    const int bufferToDelay = getFreeBuffer (false);
                                    renderingOps.add (new CopyChannelOp (srcIndex, bufferToDelay));
                                    renderingOps.add (new DelayChannelOp (bufferToDelay, maxLatency - nodeDelay));
                                    srcIndex = bufferToDelay;
                                }
                            }

                            renderingOps.add (new AddChannelOp (srcIndex, bufIndex));
                        }
                    }
                }
            }

            jassert (bufIndex >= 0);
            audioChannelsToUse.add (bufIndex);

            if (inputChan < numOuts)
                markBufferAsContaining (bufIndex, node->nodeId, inputChan);
        }

        for (int outputChan = numIns; outputChan < numOuts; ++outputChan)
        {
            const int bufIndex = getFreeBuffer (false);
            jassert (bufIndex != 0);
            audioChannelsToUse.add (bufIndex);

            markBufferAsContaining (bufIndex, node->nodeId, outputChan);
        }

        // Now the same thing for midi..
        Array <uint32> midiSourceNodes;

        for (int i = graph.getNumConnections(); --i >= 0;)
        {
            const ProcessorGraph::Connection* const c = graph.getConnection (i);

            if (c->destNode == node->nodeId && c->destPort == ProcessorGraph::midiChannelIndex)
                midiSourceNodes.add (c->sourceNode);
        }

        if (midiSourceNodes.size() == 0)
        {
            // No midi inputs..
            midiBufferToUse = getFreeBuffer (true); // need to pick a buffer even if the processor doesn't use midi

            if (node->getProcessor()->acceptsMidi() || node->getProcessor()->producesMidi())
                 renderingOps.add (new ClearMidiBufferOp (midiBufferToUse));
        }
        else if (midiSourceNodes.size() == 1)
        {
            // One midi input..
            midiBufferToUse = getBufferContaining (midiSourceNodes.getUnchecked(0),
                                                   ProcessorGraph::midiChannelIndex);

            if (midiBufferToUse >= 0)
            {
                if (isBufferNeededLater (ourRenderingIndex,
                                         ProcessorGraph::midiChannelIndex,
                                         midiSourceNodes.getUnchecked(0),
                                         ProcessorGraph::midiChannelIndex))
                {
                    // can't mess up this channel because it's needed later by another node, so we
                    // need to use a copy of it..
                    const int newFreeBuffer = getFreeBuffer (true);
                    renderingOps.add (new CopyMidiBufferOp (midiBufferToUse, newFreeBuffer));
                    midiBufferToUse = newFreeBuffer;
                }
            }
            else
            {
                // probably a feedback loop, so just use an empty one..
                midiBufferToUse = getFreeBuffer (true); // need to pick a buffer even if the processor doesn't use midi
            }
        }
        else
        {
            // More than one midi input being mixed..
            int reusableInputIndex = -1;

            for (int i = 0; i < midiSourceNodes.size(); ++i)
            {
                const int sourceBufIndex = getBufferContaining (midiSourceNodes.getUnchecked(i),
                                                                ProcessorGraph::midiChannelIndex);

                if (sourceBufIndex >= 0
                     && ! isBufferNeededLater (ourRenderingIndex,
                                               ProcessorGraph::midiChannelIndex,
                                               midiSourceNodes.getUnchecked(i),
                                               ProcessorGraph::midiChannelIndex))
                {
                    // we've found one of our input buffers that can be re-used..
                    reusableInputIndex = i;
                    midiBufferToUse = sourceBufIndex;
                    break;
                }
            }

            if (reusableInputIndex < 0)
            {
                // can't re-use any of our input buffers, so get a new one and copy everything into it..
                midiBufferToUse = getFreeBuffer (true);
                jassert (midiBufferToUse >= 0);

                const int srcIndex = getBufferContaining (midiSourceNodes.getUnchecked(0),
                                                          ProcessorGraph::midiChannelIndex);
                if (srcIndex >= 0)
                    renderingOps.add (new CopyMidiBufferOp (srcIndex, midiBufferToUse));
                else
                    renderingOps.add (new ClearMidiBufferOp (midiBufferToUse));

                reusableInputIndex = 0;
            }

            for (int j = 0; j < midiSourceNodes.size(); ++j)
            {
                if (j != reusableInputIndex)
                {
                    const int srcIndex = getBufferContaining (midiSourceNodes.getUnchecked(j),
                                                              ProcessorGraph::midiChannelIndex);
                    if (srcIndex >= 0)
                        renderingOps.add (new AddMidiBufferOp (srcIndex, midiBufferToUse));
                }
            }
        }

        if (node->getProcessor()->producesMidi())
            markBufferAsContaining (midiBufferToUse, node->nodeId,
                                    ProcessorGraph::midiChannelIndex);

        setNodeDelay (node->nodeId, maxLatency + node->getProcessor()->getLatencySamples());

        if (numOuts == 0)
            totalLatency = maxLatency;

        renderingOps.add (new ProcessBufferOp (node, audioChannelsToUse,
                                               totalChans, midiBufferToUse));
    }

    //==============================================================================
    int getFreeBuffer (const bool forMidi)
    {
        if (forMidi)
        {
            for (int i = 1; i < midiNodeIds.size(); ++i)
                if (midiNodeIds.getUnchecked(i) == freeNodeID)
                    return i;

            midiNodeIds.add ((uint32) freeNodeID);
            return midiNodeIds.size() - 1;
        }
        else
        {
            for (int i = 1; i < nodeIds.size(); ++i)
                if (nodeIds.getUnchecked(i) == freeNodeID)
                    return i;

            nodeIds.add ((uint32) freeNodeID);
            channels.add (0);
            return nodeIds.size() - 1;
        }
    }

    int getReadOnlyEmptyBuffer() const noexcept
    {
        return 0;
    }

    int getBufferContaining (const uint32 nodeId, const int outputChannel) const noexcept
    {
        if (outputChannel == ProcessorGraph::midiChannelIndex)
        {
            for (int i = midiNodeIds.size(); --i >= 0;)
                if (midiNodeIds.getUnchecked(i) == nodeId)
                    return i;
        }
        else
        {
            for (int i = nodeIds.size(); --i >= 0;)
                if (nodeIds.getUnchecked(i) == nodeId
                     && channels.getUnchecked(i) == outputChannel)
                    return i;
        }

        return -1;
    }

    void markAnyUnusedBuffersAsFree (const int stepIndex)
    {
        for (int i = 0; i < nodeIds.size(); ++i)
        {
            if (isNodeBusy (nodeIds.getUnchecked(i))
                 && ! isBufferNeededLater (stepIndex, -1,
                                           nodeIds.getUnchecked(i),
                                           channels.getUnchecked(i)))
            {
                nodeIds.set (i, (uint32) freeNodeID);
            }
        }

        for (int i = 0; i < midiNodeIds.size(); ++i)
        {
            if (isNodeBusy (midiNodeIds.getUnchecked(i))
                 && ! isBufferNeededLater (stepIndex, -1,
                                           midiNodeIds.getUnchecked(i),
                                           ProcessorGraph::midiChannelIndex))
            {
                midiNodeIds.set (i, (uint32) freeNodeID);
            }
        }
    }

    bool isBufferNeededLater (int stepIndexToSearchFrom,
                              int inputChannelOfIndexToIgnore,
                              const uint32 nodeId,
                              const int outputChanIndex) const
    {
        while (stepIndexToSearchFrom < orderedNodes.size())
        {
            const ProcessorGraph::Node* const node = (const ProcessorGraph::Node*) orderedNodes.getUnchecked (stepIndexToSearchFrom);

            if (outputChanIndex == ProcessorGraph::midiChannelIndex)
            {
                if (inputChannelOfIndexToIgnore != ProcessorGraph::midiChannelIndex
                     && graph.getConnectionBetween (nodeId, ProcessorGraph::midiChannelIndex,
                                                    node->nodeId, ProcessorGraph::midiChannelIndex) != nullptr)
                    return true;
            }
            else
            {
                for (int i = 0; i < node->getProcessor()->getNumPorts(); ++i)
                    if (i != inputChannelOfIndexToIgnore
                         && graph.getConnectionBetween (nodeId, outputChanIndex,
                                                        node->nodeId, i) != nullptr)
                        return true;
            }

            inputChannelOfIndexToIgnore = -1;
            ++stepIndexToSearchFrom;
        }

        return false;
    }

    void markBufferAsContaining (int bufferNum, uint32 nodeId, int outputIndex)
    {
        if (outputIndex == ProcessorGraph::midiChannelIndex)
        {
            jassert (bufferNum > 0 && bufferNum < midiNodeIds.size());

            midiNodeIds.set (bufferNum, nodeId);
        }
        else
        {
            jassert (bufferNum >= 0 && bufferNum < nodeIds.size());

            nodeIds.set (bufferNum, nodeId);
            channels.set (bufferNum, outputIndex);
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProcessorGraphBuilder)
};

}
