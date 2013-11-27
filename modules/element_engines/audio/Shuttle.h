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

        static const int32 PPQ = 1920;

        /** This function should be used when loading midi data.  e.g. opening
            a midi file that has a different ppq than the Shuttle */
        static inline double
        scaledTick (double sourceTick, const int32 srcPpq)
        {
            if (srcPpq == Shuttle::PPQ || srcPpq <= 0)
                return sourceTick;

            return sourceTick * ((double) Shuttle::PPQ / (double) srcPpq);
        }

        struct Position :  public AudioPlayHead::CurrentPositionInfo
        {
            double timeInBeats;
        };

        Shuttle()
        {
            ts.setTempo (120.0f);
            ts.setSampleRate (44100);
            ts.setTicksPerBeat (Shuttle::PPQ);
            ts.updateScale();

            framePos = 0;
            mFramesPerBeat  = Tempo::framesPerBeat ((double) ts.sampleRate(), ts.tempo());
            mBeatsPerFrame  = 1.0f / mFramesPerBeat;
            playing = recording = false;
        }

        ~Shuttle() { }

        inline bool isLooping()   const { return looping; }
        inline bool isPlaying()   const { return playing; }
        inline bool isRecording() const { return recording; }

        inline double framesPerBeat() const { return mFramesPerBeat; }
        inline double beatsPerFrame() const { return mBeatsPerFrame; }

        const double lengthInBeats()     const { return lengthInSeconds() * (tempo() / 60.0f); }
        const int64  lengthInFrames()    const { return 441000; }
        const double lengthInSeconds()   const { return (double) lengthInFrames() / sampleRate; }

        const double positionInBeats()   const { return positionInSeconds() * (tempo() / 60.0f); }
        const int32  positionInFrames()  const { return framePos; }
        const double positionInSeconds() const { return (double) framePos / (double) ts.sampleRate(); }

        void resetRecording() { }

        const TimeScale& scale() const { return ts; }
        float tempo() const { return ts.tempo(); }

        inline void
        setTempo (float bpm)
        {
            if (tempo() != bpm)
            {
                double oldTime = positionInBeats();
                ts.setTempo (bpm);
                ts.updateScale();
                mFramesPerBeat = Tempo::framesPerBeat ((double) ts.sampleRate(), ts.tempo());
                framePos = llrint (oldTime * mFramesPerBeat);
            }
        }

        inline void
        setSampleRate (double rate)
        {
            if (sampleRate == rate)
                return;

            const double oldTime = positionInSeconds();
            ts.setSampleRate (rate);
            ts.updateScale();

            framePos        = llrint (oldTime * ts.sampleRate());
            mFramesPerBeat  = Tempo::framesPerBeat (ts.sampleRate(), ts.tempo());
            mBeatsPerFrame  = 1.0f / mFramesPerBeat;
        }

        inline int remainingFrames() const { return lengthInFrames() - framePos; }

        inline void advance (int nframes)
        {
            framePos += nframes;
            if (framePos > lengthInFrames())
                framePos = framePos - lengthInFrames();
        }

        inline bool
        getCurrentPosition (CurrentPositionInfo &result)
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

    protected:

        bool playing, recording, looping;

    private:

        double mFramesPerBeat;
        double mBeatsPerFrame;

        int64 framePos;
        double sampleRate;
        TimeScale ts;

        double ppqLoopStart;
        double ppqLoopEnd;

    };


#endif // ELEMENT_SHUTTLE_H
