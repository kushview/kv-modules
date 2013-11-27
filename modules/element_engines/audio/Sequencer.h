/*
    Sequencer.h - This file is part of Element

    Copyright (C) 2013 BKE, LLC  All rights reserved.
      * Michael Fisher <mfisher@bketech.com>
*/

#ifndef ELEMENT_SEQUENCER_H
#define ELEMENT_SEQUENCER_H


    class ClipFactory;
    class ClipSource;
    class SequenceCursor;
    class SequenceModel;
    class SequencerTrack;
    class TrackModel;

    /** The engine-side main sequencer implementation */
    class Sequencer :  public GraphProcessor,
                       public ValueTree::Listener
    {
    public:

        typedef AudioPlayHead::CurrentPositionInfo Position;
        typedef GraphProcessor::Node::Ptr NodePtr;
        typedef ReferenceCountedArray<GraphProcessor::Node> NodeArray;

        /** Create a new sequencer graph
            Sequencer needs the Audio engine for managing clip data as
            well as media management globals */
        Sequencer (ClipFactory& c);
        ~Sequencer ();

        int32 numTracks() const;

        bool addTrack (const TrackModel& t);
        void assignTrack (int32 index, TrackModel& t);
        bool removeTrack (const TrackModel& t);

        bool tracksArePresent() const;
        ClipSource* trackClip (int32 track) const;
        const Array<SequencerTrack*>& tracks() const { return trackRefs; }

        const String getName() const { return "Sequencer"; }
        void prepareToPlay (double sampleRate, int estimatedBlockSize);
        void releaseResources();

        const Position& position() const { return playPos; }

        void setModel (const SequenceModel& m);
        SequenceModel model() const;

        void updateTrack (int32 index);

    private:

        ValueTree state;

        ClipFactory& engine;
        ScopedPointer<SequenceCursor> cursor;
        Array<SequencerTrack*> trackRefs;
        Position playPos;

        NodePtr createTrackFor (const TrackModel& track);
        NodePtr getNodeFor (const TrackModel& t);
        NodePtr getNodeFor (const ValueTree& vt);

        void preRenderNodes();
        void postRenderNodes();

        friend class ValueTree;
        void valueTreePropertyChanged (ValueTree& tree, const Identifier& property);
        void valueTreeChildAdded (ValueTree& parent, ValueTree& child);
        void valueTreeChildRemoved (ValueTree& parent, ValueTree& child);
        void valueTreeChildOrderChanged (ValueTree& parent);
        void valueTreeParentChanged (ValueTree& tree);

        JUCE_LEAK_DETECTOR (Sequencer)

    };

#endif // ELEMENT_SEQUENCER_H
