
#ifndef ELEMENT_SHUTTLE_H
#define ELEMENT_SHUTTLE_H

#include "element/juce.hpp"
#include "element/time/Tempo.h"
#include "element/time/TimeScale.h"

namespace Element
{

    /** A mini-transport for use in a processable that can loop */
    class Shuttle :  public AudioPlayHead
    {
    public:

        Shuttle()
        {
            bpmTempo   = 120.0f;
            framePos   = 0;
            sampleRate = 44100.f;
            mFramesPerBeat  = Tempo::framesPerBeat (sampleRate, bpmTempo);
            playing = recording = false;
        }

        ~Shuttle() { }

        bool isPlaying()   const { return playing; }
        bool isRecording() const { return recording; }

        const double lengthInBeats()     const { return lengthInSeconds() * (bpmTempo / 60.0f); }
        const int64  lengthInFrames()    const { return 441000; }
        const double lengthInSeconds()   const { return (double) lengthInFrames() / sampleRate; }

        const double positionInBeats()   const { return positionInSeconds() * (bpmTempo / 60.0f); }
        const int positionInFrames()     const { return framePos; }
        const double positionInSeconds() const { return (double) framePos / sampleRate; }

        void resetRecording() { }

        bool willSendAllNotesOff() const { return false; }
        bool willStop() const { return false; }
       // bool willStopRecord() const { return true; }

        float framesPerBeat() const { return mFramesPerBeat; }

        const double& tempo() const { return bpmTempo; }

        inline void setBpmTempo (float bpm)
        {
            if (bpmTempo != bpm)
            {
                double oldTime = positionInBeats();
                bpmTempo = bpm;
                mFramesPerBeat = Tempo::framesPerBeat (sampleRate, bpmTempo);
                framePos = llrint (oldTime * mFramesPerBeat);
            }
        }

        inline void setSampleRate (double rate)
        {
            if (sampleRate == rate)
                return;

            const double oldTime = positionInSeconds();
            sampleRate = rate;
            framePos   = llrint (oldTime * sampleRate);
            mFramesPerBeat  = Tempo::framesPerBeat (sampleRate, bpmTempo);
        }

        inline int remainingFrames() const { return lengthInFrames() - framePos; }

        inline void advance (int nframes)
        {
            framePos += nframes;
            if (framePos > lengthInFrames())
                framePos = framePos - lengthInFrames();
        }

        inline bool getCurrentPosition (CurrentPositionInfo &result)
        {
            result.bpm = (double) bpmTempo;
            result.frameRate = AudioPlayHead::fps24;

            result.isLooping   = true;
            result.isPlaying   = this->isPlaying();
            result.isRecording = this->isRecording();

            result.ppqLoopStart = 0.0f;
            result.ppqLoopEnd   = 0.0f;
            result.ppqPosition  = 0.0f;
            result.ppqPositionOfLastBarStart = 0.0f;

            result.editOriginTime = 0.0f;
            result.timeInSamples  = positionInFrames();
            result.timeInSeconds  = positionInSeconds();
            result.timeSigDenominator = 4;
            result.timeSigDenominator = 4;

            return true;
        }

    protected:

        bool playing, recording;

    private:

        double bpmTempo;
        double mFramesPerBeat;
        int64 framePos, localOffset;
        double sampleRate;
        TimeScale ts;

    };
}

#endif // ELEMENT_SHUTTLE_H
