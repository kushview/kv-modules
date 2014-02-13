/*
    MidiSequencePlayer.cpp - This file is part of Element
    Copyright (C) 2013  Michael Fisher <mfisher31@gmail.com>

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

#if JUCE_COMPLETION
#include "modules/element_base/element_base.h"
#include "MidiSequencePlayer.h"
#include "JuceHeader.h"
#endif

#define NOTE_CHANNEL       1
#define NOTE_VELOCITY      0.8f
#define NOTE_PREFRAMES     0.001


MidiSequencePlayer::MidiSequencePlayer()
    : midiSequence (new MidiMessageSequence())
{
    numBars     = 4;
    frameOffset = 0;
    shuttle    = nullptr;
}

MidiSequencePlayer::~MidiSequencePlayer ()
{
    midiSequence = nullptr;
    shuttle      = nullptr;
}

void MidiSequencePlayer::prepareToPlay (double sampleRate, int /* blockSize */)
{
	noteOffs.clear();
}

void
MidiSequencePlayer::releaseResources()
{ }

void
MidiSequencePlayer::renderSequence (MidiBuffer& target, const MidiMessageSequence& seq,
                                    int32 startInSequence, int32 numSamples)
{
    const TimeScale& ts (shuttle->getTimeScale());
    const int32 numEvents = seq.getNumEvents();
    const double start = (double) ts.tickFromFrame (frameOffset + startInSequence);

    for (int32 ev = seq.getNextIndexAtTime (start); ev < numEvents;)
    {
        const double tick = seq.getEventTime (ev);
        const int32 frameInSeq = ts.frameFromTick (tick);
        const int32 timeStamp  = frameInSeq - startInSequence;

        MidiMessage* msg = &seq.getEventPointer(ev)->message;
        if (timeStamp >= numSamples || ! msg)
        {
            break;
        }

        target.addEvent (*msg, timeStamp);

        lastEventTime = tick;
        ++ev;
    }
}

int32
MidiSequencePlayer::getLoopRepeatIndex() const
{ 
    return static_cast<int> (floor (shuttle->positionInBeats())) / (double) getBeatLength();
}

double
MidiSequencePlayer::getLoopBeatPosition() const
{
    return shuttle->positionInBeats() - (getLoopRepeatIndex() * getBeatLength());
}

