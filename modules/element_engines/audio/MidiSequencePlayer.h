/*
 ==============================================================================

 This file is part of the JUCETICE project - Copyright 2007 by Lucio Asnaghi.

 JUCETICE is based around the JUCE library - "Jules' Utility Class Extensions"
 Copyright 2007 by Julian Storer.

 ------------------------------------------------------------------------------

 JUCE and JUCETICE can be redistributed and/or modified under the terms of
 the GNU Lesser General Public License, as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 JUCE and JUCETICE are distributed in the hope that they will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with JUCE and JUCETICE; if not, visit www.gnu.org/licenses or write to
 Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 Boston, MA 02111-1307 USA

 ==============================================================================
*/

#ifndef ELEMENT_MIDISEQUENCE_PROCESSOR_H
#define ELEMENT_MIDISEQUENCE_PROCESSOR_H

class Shuttle;

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

        /* Get the number of loops that have played since transport time zero (used for looping) */
        int loopRepeatIndex();

        /* Get the current playback position (accounting for loops) for the sequence, relative to the current transport position (used for looping) */
        double loopBeatPosition();

        /* Get the length of the sequence expressed as a number of beats */
        int getLengthInBeats() { return getLengthInBars() * getBeatsPerBar(); }

        /* Get the length of the sequence expressed as a number of bars */
        int getLengthInBars() { return 4; }

        /* Get the number of beats per bar (currently hard-coded to four) */
        double getBeatsPerBar() { return 4; }

        inline void setShuttle (Shuttle* s) { pShuttle = s; }
        inline Shuttle* shuttle() const { return pShuttle; }
        inline void setFrameOffset (int32 offset) { frameOffset = offset; }

    protected:

        MidiMessageSequence noteOffs;
        ScopedPointer<MidiMessageSequence> midiSequence;
        MidiMessage allNotesOff;

    private:

        Shuttle* pShuttle;
        int32 frameOffset;
        double lastEventTime;

    };



#endif

