/*
    Shuttle.h - This file is part of Element
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

#ifndef ELEMENT_SHUTTLE_H
#define ELEMENT_SHUTTLE_H

/** A mini-transport for use in a processable that can loop */
class Shuttle :  public AudioPlayHead
{
public:

    static const int32 PPQ;

    /** This function should be used when loading midi data.  e.g. opening
        a midi file that has a different ppq than the Shuttle */
    static double scaledTick (double sourceTick, const int32 srcPpq);

    struct Position :  public AudioPlayHead::CurrentPositionInfo
    {
        double timeInBeats;
    };

    Shuttle();
    ~Shuttle();

    bool isLooping()   const;
    bool isPlaying()   const;
    bool isRecording() const;

    double getFramesPerBeat() const;
    double getBeatsPerFrame() const;

    void setDurationBeats (const float beats);
    void setDurationFrames (const uint32 df);
    void setDurationSeconds (const double seconds);

    const double lengthInBeats()    const;
    const uint32 lengthInFrames()   const;
    const double lengthInSeconds()  const;

    const double positionInBeats()   const;
    const int32  positionInFrames()  const;
    const double positionInSeconds() const;

    void resetRecording();

    const TimeScale& getTimeScale() const;
    float getTempo() const;
    void setTempo (float bpm);

    double getSampleRate() const;
    void setSampleRate (double rate);
    int remainingFrames() const;
    void advance (int nframes);
    bool getCurrentPosition (CurrentPositionInfo &result);

protected:

    bool playing, recording, looping;

private:

    double mFramesPerBeat;
    double mBeatsPerFrame;

    int64 framePos;
    uint32 duration;
    double sampleRate;
    TimeScale ts;

    double ppqLoopStart;
    double ppqLoopEnd;

};

#endif // ELEMENT_SHUTTLE_H
