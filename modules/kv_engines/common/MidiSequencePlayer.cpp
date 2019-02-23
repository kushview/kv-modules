/*
    This file is part of the Kushview Modules for JUCE
    Copyright (c) 2014-2019  Kushview, LLC.  All rights reserved.

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

#define NOTE_CHANNEL       1
#define NOTE_VELOCITY      0.8f
#define NOTE_PREFRAMES     0.001

MidiSequencePlayer::MidiSequencePlayer()
    : midiSequence (new MidiMessageSequence())
{
    numBars     = 4;
    frameOffset = 0;
    shuttle.setOwned (new Shuttle());
}

MidiSequencePlayer::~MidiSequencePlayer ()
{
    midiSequence = nullptr;
    shuttle.clear();
}

void MidiSequencePlayer::prepareToPlay (double /*sampleRate*/, int /* blockSize */)
{
	noteOffs.clear();
}

void MidiSequencePlayer::releaseResources()
{ }

void MidiSequencePlayer::renderSequence (int numSamples, MidiBuffer& midiMessages)
{
    renderSequence (midiMessages, *midiSequence, 0, numSamples);
}

void MidiSequencePlayer::renderSequence (MidiBuffer& target, const MidiMessageSequence& seq,
                                         int32 startInSequence, int32 numSamples)
{
#if 1
    Midi::renderSequence (target, seq, shuttle->getTimeScale(), startInSequence, numSamples);
#else
    const TimeScale& ts (shuttle->getTimeScale());
    const int32 numEvents = seq.getNumEvents();
    const double start = (double) ts.tickFromFrame (frameOffset + startInSequence);

    for (int32 i = seq.getNextIndexAtTime (start); i < numEvents;)
    {
        const EventHolder* const ev = seq.getEventPointer(i);
        const double tick = ev->message.getTimeStamp();
        const int32 frameInSeq = ts.frameFromTick (static_cast<unsigned long> (tick));
        const int32 timeStamp = frameInSeq - startInSequence;

        if (timeStamp >= numSamples)
            break;

        target.addEvent (ev->message, timeStamp);

        /* if (ev->message.isNoteOn())
        {
            const double ots = ev->noteOffObject->message.getTimeStamp() / (double) Shuttle::PPQ;
            if (ots >= (double) getBeatLength())
            {
                ts.frameFromTick (static_cast<unsigned long> (ots * (double) Shuttle::PPQ));
            }
        }
         */

        lastEventTime = tick;

        ++i;
    }
#endif
}

int32 MidiSequencePlayer::getLoopRepeatIndex() const
{
    return static_cast<int> (floor (shuttle->getPositionBeats())) / (double) getBeatLength();
}

double MidiSequencePlayer::getLoopBeatPosition() const
{
    return shuttle->getPositionBeats() - static_cast<double> (getLoopRepeatIndex() * getBeatLength());
}

namespace Midi {


void renderSequence (MidiBuffer& target, const MidiMessageSequence& seq, const TimeScale& ts,
                     int32 startFrame, int32 numSamples)
{
#if 1
    const int32 numEvents = seq.getNumEvents();
    const double start = (double) ts.tickFromFrame (startFrame);
    for (int32 i = seq.getNextIndexAtTime (start); i < numEvents;)
    {
        const auto* const ev = seq.getEventPointer (i);
        const double tick = ev->message.getTimeStamp();
        const int frameInSeq = (int) ts.frameFromTick (static_cast<unsigned long> (tick));
        const int timeStamp = frameInSeq - startFrame;

        if (timeStamp >= numSamples)
            break;

        target.addEvent (ev->message, timeStamp);

        /* if (ev->message.isNoteOn())
         {
         const double ots = ev->noteOffObject->message.getTimeStamp() / (double) Shuttle::PPQ;
         if (ots >= (double) getBeatLength())
         {
         ts.frameFromTick (static_cast<unsigned long> (ots * (double) Shuttle::PPQ));
         }
         }
         */

        ++i;
    }
#endif
}

}
