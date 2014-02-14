/*
    Shuttle.cpp - This file is part of Element
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
#include "JuceHeader.h"
#include "Shuttle.h"
#endif

const int32 Shuttle::PPQ = 1920;

double
Shuttle::scaledTick (double sourceTick, const int32 srcPpq)
{
    if (srcPpq == Shuttle::PPQ || srcPpq <= 0)
        return sourceTick;

    return sourceTick * ((double) Shuttle::PPQ / (double) srcPpq);
}

Shuttle::Shuttle()
{
    ts.setTempo (120.0f);
    ts.setSampleRate (48000);
    duration = 48000 * 4;
    ts.setTicksPerBeat (Shuttle::PPQ);
    ts.updateScale();

    framePos = 0;
    framesPerBeat  = Tempo::framesPerBeat ((double) ts.getSampleRate(), ts.getTempo());
    beatsPerFrame  = 1.0f / framesPerBeat;
    playing = recording = looping = false;
}

Shuttle::~Shuttle() { }

double Shuttle::getBeatsPerFrame() const { return beatsPerFrame; }
double Shuttle::getFramesPerBeat() const { return framesPerBeat; }

bool
Shuttle::getCurrentPosition (CurrentPositionInfo &result)
{
    result.bpm = (double) ts.getTempo();
    result.frameRate = AudioPlayHead::fps24;

    result.isLooping   = true;
    result.isPlaying   = this->isPlaying();
    result.isRecording = this->isRecording();

    result.ppqLoopStart = ppqLoopStart;
    result.ppqLoopEnd   = ppqLoopEnd;
    result.ppqPosition  = ts.tickFromFrame (framePos);
    result.ppqPositionOfLastBarStart = 0.0f;

    result.editOriginTime = 0.0f;
    result.timeInSamples  = getPositionSeconds();
    result.timeInSeconds  = getPositionSeconds();
    result.timeSigNumerator = ts.beatsPerBar();
    result.timeSigDenominator = ts.beatDivisor();

    return true;
}

const double Shuttle::getLengthBeats()    const { return getLengthSeconds() * (getTempo() / 60.0f); }
const uint32 Shuttle::getLengthFrames()   const { return duration; }
const double Shuttle::getLengthSeconds()  const { return (double) duration / (double) ts.getSampleRate(); }

const double Shuttle::getPositionBeats()   const { return getPositionSeconds() * (getTempo() / 60.0f); }
const int32  Shuttle::getPositionFrames()  const { return framePos; }
const double Shuttle::getPositionSeconds() const { return (double) framePos / (double) ts.getSampleRate(); }

int    Shuttle::getRemainingFrames() const { return getLengthFrames() - framePos; }
double Shuttle::getSampleRate()      const { return (double) ts.getSampleRate(); }
float  Shuttle::getTempo()           const { return ts.getTempo(); }
const TimeScale& Shuttle::getTimeScale() const { return ts; }

bool Shuttle::isLooping()   const { return looping; }
bool Shuttle::isPlaying()   const { return playing; }
bool Shuttle::isRecording() const { return recording; }

void
Shuttle::resetRecording()
{
    // TODO:
}

void Shuttle::setLengthBeats   (const float beats) { setLengthFrames (framesPerBeat * beats); }
void Shuttle::setLengthSeconds (const double seconds) { setLengthFrames (llrint (getSampleRate() * seconds)); }
void Shuttle::setLengthFrames  (const uint32 df) { duration = df; }

void
Shuttle::setTempo (float bpm)
{
    if (getTempo() != bpm)
    {
        double oldTime = getPositionBeats();
        ts.setTempo (bpm);
        ts.updateScale();
        framesPerBeat = Tempo::framesPerBeat (ts.getSampleRate(), ts.getTempo());
        framePos = llrint (oldTime * framesPerBeat);
    }
}

void
Shuttle::setSampleRate (double rate)
{
    if (sampleRate == rate)
        return;

    const double oldTime = getPositionSeconds();
    const double oldLenSec = (double) getLengthSeconds();
    ts.setSampleRate (rate);
    ts.updateScale();

    framePos        = llrint (oldTime * ts.getSampleRate());
    duration        = (uint32) (oldLenSec * (float) ts.getSampleRate());
    framesPerBeat  = Tempo::framesPerBeat (ts.getSampleRate(), ts.getTempo());
    beatsPerFrame  = 1.0f / framesPerBeat;
}



void
Shuttle::advance (int nframes)
{
    framePos += nframes;
    if (duration > 0 && framePos >= duration)
        framePos = framePos - duration;
}


