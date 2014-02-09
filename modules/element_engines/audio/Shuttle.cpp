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
    mFramesPerBeat  = Tempo::framesPerBeat ((double) ts.sampleRate(), ts.tempo());
    mBeatsPerFrame  = 1.0f / mFramesPerBeat;
    playing = recording = false;
}

Shuttle::~Shuttle() { }

bool Shuttle::isLooping()   const { return looping; }
bool Shuttle::isPlaying()   const { return playing; }
bool Shuttle::isRecording() const { return recording; }

double Shuttle::getFramesPerBeat() const { return mFramesPerBeat; }
double Shuttle::getBeatsPerFrame() const { return mBeatsPerFrame; }

void Shuttle::setDurationBeats (const float beats)
{
    setDurationFrames (mFramesPerBeat * beats);
}

void Shuttle::setDurationSeconds (const double seconds)
{
    setDurationFrames (llrint (getSampleRate() * seconds));
}

void Shuttle::setDurationFrames (const uint32 df) { duration = df; }

const double Shuttle::lengthInBeats()    const { return lengthInSeconds() * (getTempo() / 60.0f); }
const uint32 Shuttle::lengthInFrames()   const { return duration; }
const double Shuttle::lengthInSeconds()  const { return (double) lengthInFrames() / (double) ts.sampleRate(); }

const double Shuttle::positionInBeats()   const { return positionInSeconds() * (getTempo() / 60.0f); }
const int32  Shuttle::positionInFrames()  const { return framePos; }
const double Shuttle::positionInSeconds() const { return (double) framePos / (double) ts.sampleRate(); }

void Shuttle::resetRecording() { }

const TimeScale& Shuttle::getTimeScale() const { return ts; }
float Shuttle::getTempo() const { return ts.tempo(); }

void Shuttle::setTempo (float bpm)
{
    if (getTempo() != bpm)
    {
        double oldTime = positionInBeats();
        ts.setTempo (bpm);
        ts.updateScale();
        mFramesPerBeat = Tempo::framesPerBeat (ts.sampleRate(), ts.tempo());
        framePos = llrint (oldTime * mFramesPerBeat);
    }
}

double Shuttle::getSampleRate() const
{
    return (double) ts.sampleRate();
}

void Shuttle::setSampleRate (double rate)
{
    if (sampleRate == rate)
        return;

    const double oldTime = positionInSeconds();
    const double oldLenSec = (double) lengthInSeconds();
    ts.setSampleRate (rate);
    ts.updateScale();

    framePos        = llrint (oldTime * ts.sampleRate());
    duration        = (uint32) (oldLenSec * (float) ts.sampleRate());
    mFramesPerBeat  = Tempo::framesPerBeat (ts.sampleRate(), ts.tempo());
    mBeatsPerFrame  = 1.0f / mFramesPerBeat;
}

int Shuttle::remainingFrames() const { return lengthInFrames() - framePos; }

void Shuttle::advance (int nframes)
{
    framePos += nframes;
    if (duration > 0 && framePos > lengthInFrames())
        framePos = framePos - lengthInFrames();
}

bool Shuttle::getCurrentPosition (CurrentPositionInfo &result)
{
    result.bpm = (double) ts.tempo();
    result.frameRate = AudioPlayHead::fps24;

    result.isLooping   = true;
    result.isPlaying   = this->isPlaying();
    result.isRecording = this->isRecording();

    result.ppqLoopStart = ppqLoopStart;
    result.ppqLoopEnd   = ppqLoopEnd;
    result.ppqPosition  = ts.tickFromFrame (framePos);
    result.ppqPositionOfLastBarStart = 0.0f;

    result.editOriginTime = 0.0f;
    result.timeInSamples  = positionInFrames();
    result.timeInSeconds  = positionInSeconds();
    result.timeSigDenominator = ts.beatsPerBar();
    result.timeSigDenominator = ts.beatDivisor();

    return true;
}
