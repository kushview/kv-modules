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


#include "element/juce.hpp"
#include "element/time/timescale.hpp"
#include "element/time/Tempo.h"

#define PROP_SEQROWOFFSET                     T("sRoff")
#define PROP_SEQCOLSIZE                       T("sCsize")
#define PROP_SEQNOTESNAP                      T("sNsnap")
#define PROP_SEQNOTELENGTH                    T("sNlen")
#define PROP_SEQBAR                           T("sNbar")
#define PROP_SEQENABLED                       T("sEnabled")

#include "element/engine/Shuttle.h"

namespace Element {


    /** A single track midi sequencer with record functionality */
    class MidiSequencePlayer : public Timer
    {
    public:

        typedef MidiMessageSequence::MidiEventHolder EventHolder;

        MidiSequencePlayer();
        ~MidiSequencePlayer();

        void renderSequence (int numSamples, MidiBuffer& midiMessages);
        void renderSequence (MidiBuffer& target, int32 startFrame, int32 endFrame);
        void prepareToPlay (double sampleRate, int blockSize);
        void releaseResources();

        void timerCallback();

        bool timeSignatureChanged (const int barsCount, const int timeDenominator);
        bool playingPositionChanged (const float absolutePosition);

        /* sequence editing */

        inline EventHolder* eventHolder (int noteId) const {
            assert (notes.contains (noteId));
            return notes [noteId];
        }

        inline bool removeNote (EventHolder* ev) {
            notes.removeValue (ev);
            return true;
        }

        int noteIndex (const int noteNumber, const float beatNumber, const float noteLength,
                       const int channel = 1, const float velocity = 0.8f);

        int addNote (const int noteNumber, const float beatNumber, const float noteLength,
                              const int channel = 1, const float velocity = 0.8f);

        bool addNote (const int noteNumber, const float beatNumber);
#if 0
        bool addNote (const int noteNumber, const float beatNumber, const float noteLength,
                      const int channel = 1, const float velocity = 0.8f);
#endif
        bool removeNote (const int noteNumber, const float beatNumber, const float noteLength);
        bool moveNote (const int oldNote, const float oldBeat, const int noteNumber,
                       const float beatNumber, const float noteLength);
        bool resizeNote (const int noteNumber, const float beatNumber, const float noteLength);
        bool clearAllNotes();

        /** Return the specified note on */
        void getNoteOnIndexed (const int index, int& note, float& beat, float& length);

        /** Counts the available note ons */
        int countNoteOns() const;

        /* Set the midi sequence being played by the sequencer */
        void setSequence (const MidiMessageSequence* mseq);

        /* Get the midi sequence being played by the sequencer */
        MidiMessageSequence* getSequence() { return midiSequence; }

        /* Get the number of loops that have played since transport time zero (used for looping) */
        int loopRepeatIndex();

        /* Get the current playback position (accounting for loops) for the sequence, relative to the current transport position (used for looping) */
        double loopBeatPosition();

        /* Get the length of the sequence expressed as a number of beats */
        int getLengthInBeats() { return getLengthInBars() * getBeatsPerBar(); }

        /* Get the length of the sequence expressed as a number of bars */
        int getLengthInBars() { return 1; }

        /* Get the number of beats per bar (currently hard-coded to four) */
        double getBeatsPerBar() { return 4; }

        void setShuttle (Shuttle* s) { shuttle = s; }

        void setRecordingFlag (bool f) { stopRecording = f; }

    protected:

        // internal, used when rendering midi events to ensure loops render appropriately and note offs don't get lost
        void renderEventsInRange (const MidiMessageSequence& sourceMidi,
                                  MidiBuffer* midiBuffer,
                                  double beatCount,
                                  double frameEndBeatCount,
                                  const int frameCounter,
                                  const int framesPerBeat,
                                  const int nextBlockFrameNumber,
                                  const int seqIndex,
                                  const int blockSize);

        void cleanUpNoteOffs (double fromTime, double toTime);

        MidiMessageSequence noteOffs;
        ScopedPointer<MidiMessageSequence> midiSequence;
        MidiMessageSequence recordingSequence;

        bool doAllNotesOff;
        MidiMessage allNotesOff;

    private:

        int lastNoteId;
        HashMap<int, EventHolder*> notes;

        bool stopRecording;
        Shuttle* shuttle;
        double currentSampleRate;
        int currentBlockSize;
        bool enabled;
        float insertLength, insertVelocity, insertChannel;
        int noteOnStamps [128];

    };

}

#endif

