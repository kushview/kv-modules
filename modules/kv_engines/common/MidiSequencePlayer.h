/*
    This file is part of the element modules for the JUCE Library
    Copyright (C) 2014  Kushview, LLC.  All rights reserved.

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

#ifndef EL_MIDI_SEQUENCE_PLAYER_H
#define EL_MIDI_SEQUENCE_PLAYER_H

class Shuttle;
class TimeScale;

namespace Midi {
    void renderSequence (MidiBuffer& target, const MidiMessageSequence& seq, const TimeScale& ts, int32 startFrame, int32 numSamples);
}

/** A single track midi sequencer with record functionality */
class MidiSequencePlayer
{
public:

    typedef MidiMessageSequence::MidiEventHolder EventHolder;

    MidiSequencePlayer();
    ~MidiSequencePlayer();

    void renderSequence (int numSamples, MidiBuffer& midiMessages);
    void renderSequence (MidiBuffer& target, const MidiMessageSequence& seq, int32 startFrame, int32 numSamples);

    void prepareToPlay (double sampleRate, int blockSize);
    void releaseResources();

    /* Get the number of loops that have played since transport time zero (used
       for looping) */
    int32 getLoopRepeatIndex() const;

    /* Get the current playback position (accounting for loops) for the sequence,
       relative to the current transport position (used for looping) */
    double getLoopBeatPosition() const;

    /* Get the length of the sequence expressed as a number of beats */
    inline int32 getBeatLength() const { return getBarLength() * getBeatsPerBar(); }

    /* Get the length of the sequence expressed as a number of bars */
    inline int32 getBarLength() const { return numBars; }
    inline void setBarLength (int32 newNumBars) { numBars = newNumBars; }

    /* Get the number of beats per bar (currently hard-coded to four) */
    int32 getBeatsPerBar() const { return 4; }

    inline void setShuttle (Shuttle* s) { shuttle.setNonOwned(s); }
    inline Shuttle* getShuttle() const { return shuttle; }
    inline void setFrameOffset (int32 offset) { frameOffset = offset; }

protected:
    MidiMessageSequence noteOffs;
    ScopedPointer<MidiMessageSequence> midiSequence;
    MidiMessage allNotesOff;

private:
    OptionalScopedPointer<Shuttle> shuttle;
    int32 frameOffset;
    double lastEventTime;
    int32 numBars;
};

#endif
