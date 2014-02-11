/*
    ClipSource.h - This file is part of Element

    Copyright (C) 2013 BKE, LLC  All rights reserved.
      * Michael Fisher <mfisher@bketech.com>
*/

#ifndef ELEMENT_CLIP_SOURCE_H
#define ELEMENT_CLIP_SOURCE_H


    class ClipFactory;

    class ClipData
    {
    public:

        ClipData() { }
        virtual ~ClipData() { }

        virtual void prepare (double rate, int32 blockSize) { }
        virtual void unprepare () { }

        MidiMessageSequence midi;

    private:

        friend class ClipFactory;
        int64 hash;

    };

    class ClipSource :  public LinkedList<ClipSource>::Link,
                        public PositionableAudioSource,
                        public ValueListener
    {
    public:

        typedef AudioPlayHead::CurrentPositionInfo Position;

        ClipSource();
        virtual ~ClipSource();

        inline void
        seekIfContainsFrame (const int64& f)
        {
            if (frames.contains (f))
                seekTrackFrame (f);
        }

        inline const FrameSpan& range() const { return frames; }
        inline bool isInRangeOf (const int64& frame) const
        {
            return frames.contains (frame);
        }

        inline int64 duration()    const { return frames.getLength(); }
        inline int64 frameStart()  const { return frames.getStart(); }
        inline int64 frameEnd()    const { return frames.getEnd(); }
        inline int64 frameLength() const { return frames.getLength(); }
        inline int32 frameOffset() const { return clipOffset; }

        ClipModel getModel() const;

        inline bool hasData() const { return data.get() != nullptr; }
        inline bool isEmpty() const { return ! hasData(); }

        /** Request the clip seek it's data in alignment with the track frame */
        inline void seekTrackFrame (const int64& trackFrame) { seekLocalFrame (trackFrame - frames.getStart()); }

        /** Request the clip seek it's data to it's local frame */
        virtual void seekLocalFrame (const int64& frame) = 0;

        inline void setNextReadPosition (int64 newPosition) { framePos.set (newPosition); }
        inline int64 getNextReadPosition() const { return framePos.get(); }
        inline int64 getTotalLength() const { return frames.getLength(); }
        inline bool isLooping() const { return looping; }
        inline void setLooping (bool shouldLoop) { looping = shouldLoop; }

        void valueChanged (Value &value);

    protected:

        inline virtual void setTime (const Range<double>& time) {
            setTimeSeconds (time);
        }

        inline void setTimeSeconds (const Range<double>& time) {
            setTimeSeconds (time.getStart(), time.getLength());
        }

        inline void setTimeSeconds (const double in, const double len) {
            frames.setStart  (llrint (in * 48000.0f));
            frames.setLength (llrint (len * 48000.0f));
        }

        inline const ClipData* clipData() const { return data.get(); }
        inline const ClipData* getClipData() const { return data.get(); }

    private:

        friend class ClipFactory;
        friend class Sequencer;
        friend class SequencerTrack;

        ValueTree state;

        // realtime params
        Shared<ClipData> sdata;
        AtomicValue<ClipData*> data;

        inline bool
        setData (Shared<ClipData> d)
        {
            Shared<ClipData> old = sdata;
            sdata = d;

            bool res = data.set (sdata.get());
            if (! res)
                sdata = old;

            return res;
        }

        int32 clipOffset;
        FrameSpan frames;
        AtomicValue<int64> framePos;
        bool looping;

        // model listening, non realtime params
        Value start, length, offset;

        /** (un)register clip values for Value listening */
        void connectValues (bool disconnect = false);

        /** Set the clip model, ClipFactory will use this when creating new sources
            and restoring recycled clips */
        void setModel (const ClipModel& model);

    };

    class ClipBin :  public LinkedList<ClipSource>
    {
    public:

        ClipBin() { }
        ~ClipBin() { }
    };



#endif // ELEMENT_CLIP_SOURCE_H
