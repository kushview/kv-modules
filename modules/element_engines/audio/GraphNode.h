#ifndef EL_GRAPH_NODE_H
#define EL_GRAPH_NODE_H

/** Represents one of the nodes, or processors, in an AudioProcessorGraph.

    To create a node, call ProcessorGraph::addNode().
*/
class JUCE_API  GraphNode   : public ReferenceCountedObject
{
public:

    /** The ID number assigned to this node.
        This is assigned by the graph that owns it, and can't be changed.
    */
    const uint32 nodeId;

    /** The actual processor object that this node represents. */
    Processor* audioProcessor() const noexcept           { return proc; }

    AudioPluginInstance* getAudioPluginInstance() const;

    /** The actual processor object dynamic_cast'd to ProcType */
    template<class ProcType>
    inline ProcType* processor() const { return dynamic_cast<ProcType*> (proc.get()); }

    /** A set of user-definable properties that are associated with this node.

        This can be used to attach values to the node for whatever purpose seems
        useful. For example, you might store an x and y position if your application
        is displaying the nodes on-screen.
    */
    NamedValueSet properties;

    /** Returns true if the process is a graph */
    bool isSubgraph() const noexcept;

    void setGain (const float g) {
        gain.set (g);
    }

    float getGain() const { return gain.get(); }
    float getLastGain() const {return lastGain.get(); }
    void updateGain() { lastGain.set (gain.get()); }

    ValueTree getMetadata() const { return metadata; }
    void setMetadata (const ValueTree& meta, bool copy = false)
    {
        metadata = (copy) ? meta.createCopy() : meta;
    }

    bool isMidiIONode() const;

    /* returns the parent graph. If one has not been set, then
       this will return nullptr */
    GraphProcessor* getParentGraph() const;

    void setInputRMS (int chan, float val);
    float getInputRMS(int chan) const { return (chan < inRMS.size()) ? inRMS.getUnchecked(chan)->get() : 0.0f; }
    void setOutputRMS (int chan, float val);
    float getOutpputRMS(int chan) const { return (chan < outRMS.size()) ? outRMS.getUnchecked(chan)->get() : 0.0f; }

private:
    friend class GraphProcessor;

    const ScopedPointer<Processor> proc;
    bool isPrepared;
    GraphNode (uint32 nodeId, Processor*) noexcept;

    void setParentGraph (GraphProcessor*);
    void prepare (double sampleRate, int blockSize, GraphProcessor*);
    void unprepare();

    AtomicValue<float> gain, lastGain;
    OwnedArray<AtomicValue<float> > inRMS, outRMS;

    ValueTree metadata;
    GraphProcessor* parent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphNode)
};

/** A convenient typedef for referring to a pointer to a node object. */
typedef ReferenceCountedObjectPtr<GraphNode> GraphNodePtr;

#endif // EL_GRAPH_NODE_H
