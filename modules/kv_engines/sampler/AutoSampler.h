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

#pragma once

namespace kv {

class AutoSampler : public AsyncUpdater
{
public:
    struct NoteInfo
    {
        Uuid layerId;
        int index   = 0;
        int note    = 0;
        
        int64 start = 0;
        int64 stop  = 0;

        File file;
        std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> writer;
    };

    struct LayerInfo
    {
        LayerInfo() = default;
        LayerInfo (const String& layerId, int layerVelocity)
            : uuid (layerId), velocity (static_cast<uint8> (layerVelocity)) { }
        LayerInfo (const String& layerId, uint8 layerVelocity)
            : uuid (layerId), velocity (layerVelocity) { }
        LayerInfo (const LayerInfo& o) { operator= (o); }

        Uuid    uuid;
        uint8   velocity        = 127;
        int     noteLength      = 3000;
        int     tailLength      = 1000;
        int     midiChannel     = 1;
        int     midiProgram     = -1;

        LayerInfo& operator= (const LayerInfo& o)
        {
            uuid            = o.uuid;
            velocity        = o.velocity;
            noteLength      = o.noteLength;
            tailLength      = o.tailLength;
            midiChannel     = o.midiChannel;
            midiProgram     = o.midiProgram;
            return *this;
        }
    };

    struct CaptureDetails
    {
        MidiMessageSequence sequence;
        OwnedArray<NoteInfo> samples;
        
        int getNumSamples() const { return samples.size(); }
        NoteInfo* getCaptureInfo (const int i) const { return samples.getUnchecked (i); }
        
        int getNextSampleIndex (const int64 frame)
        {
            const auto total = samples.size();
            int i;

            for (i = 0; i < total; ++i)
                if (samples.getUnchecked(i)->start >= frame || samples.getUnchecked(i)->stop >= frame)
                    return i;

            return i;
        }

        int64 getHighestEndFrame() const
        {
            int64 frame = 0;
            for (auto* const sample : samples)
                if (sample->stop > frame)
                    frame = sample->stop;
            return frame;
        }
    };

    struct Context
    {
        int source                  = 0;

        int keyStart                = 36;   // C2
        int keyEnd                  = 60;   // C4
        int keyStride               = 4;    // 4 semi tones

        String baseName             = "Sample";
        int noteLength              = 3000;
        int tailLength              = 1000;

        Array<LayerInfo> layers;

        String instrumentName       = "Instrument";
        String outputPath           = String();
        String format               = "wave";
        int channels                = 2;
        int bitDepth                = 16;
        int latency                 = 0;
        double sampleRate           = 44100.0;

        ValueTree createValueTree() const;
        void writeToFile (const File& file) const;
        void restoreFromFile (const File& file);

        File getCaptureDir() const;
        CaptureDetails* createLayerRenderDetails (const int layer, 
                                                  const double sourceSampleRate,
                                                  juce::AudioFormatManager& formats,
                                                  juce::TimeSliceThread& thread) const;
    };

    AutoSampler (juce::AudioFormatManager& f);
    ~AutoSampler();

    //=========================================================================
    void prepare (double newSampleRate, int newBlockSize);
    void renderCycleBegin();
    void getNextMidiBlock (MidiBuffer& midi, int nframes);
    void writeAudioFrames (AudioSampleBuffer& audio);
    void renderCycleEnd();
    void release();

    //=========================================================================
    Result start (const Context& newContext, int latencySamples = 0);
    void cancel();

    //=========================================================================
    /** Returns true if currently rendering or rendering has been requested */
    bool isRendering() const { return renderingRequest.get() != 0 || rendering.get() != 0; }
    
    //=========================================================================
    /** Update the context.  The properties here may not be used for rendering
        It is here so that when previewing the settings match */
    void setContext (const Context& newContext);

    /** Returns the current render context used for preview or rendering
        Only use this in the audio thread or lock with getCallbackLock() */
    const Context& getContext() const { return context; }

    /** Returns the current source type.  Only use in audio thread or lock
        with getCallbackLock() */
    int getSourceType() const { return context.source; }

    //=========================================================================
    /** Returns sample metadata after rendering has completed */
    ValueTree getSamples() const { return samples; }

    double getProgress() const { return progressValue; }
     
    String getNextProgressTitle()
    {
        if (steps.isEmpty())
            return { };
        progressValue = static_cast<double> (totalSteps - steps.size()) / static_cast<double> (totalSteps);
        auto result = steps [0];
        steps.remove (0);
        return result;
    }

    //=========================================================================
    CriticalSection& getCallbackLock() { return lock; }

    //=========================================================================
    /** @internal */
    void handleAsyncUpdate() override;

    std::function<void()> onStopped;
    std::function<void()> onStarted;
    std::function<void()> onCancelled;
    std::function<void()> onProgress;

private:
    Identifier samplesType { "samples" };
    ValueTree samples;
    TimeSliceThread thread;
    AudioFormatManager& formats;
    
    CriticalSection lock;
    Context context;
    bool prepared = false;
    double sampleRate = 0.0;
    int blockSize = 0;

    Atomic<int> rendering { 0 };
    Atomic<int> renderingRequest { 0 };
    Atomic<int> shouldCancel { 0 };

    int writerDelay = 0;

    int64 frame = 0;
    int event = 0;
    int layer = 0;
    int nlayers = 0;
    
    HeapBlock<float*> channels;
    OwnedArray<CaptureDetails> details;

    StringArray steps;
    int totalSteps;

    struct Started : public AsyncUpdater
    {
        Started (AutoSampler& r) : render (r) { }
        void handleAsyncUpdate() { if (render.onStarted) render.onStarted(); }
        AutoSampler& render;
    } started;

    struct Stopped : public AsyncUpdater
    {
        Stopped (AutoSampler& r) : render (r) {}
        void handleAsyncUpdate()  { if (render.onStopped) render.onStopped(); }
        AutoSampler& render;
    } stopped;

    struct Cancelled : public AsyncUpdater
    {
        Cancelled (AutoSampler& r) : render (r) { }
        void handleAsyncUpdate()  { if (render.onCancelled) render.onCancelled(); }
        AutoSampler& render;
    } cancelled;

    struct Progress : public AsyncUpdater
    {
        Progress (AutoSampler& r) : render (r) { }
        void handleAsyncUpdate()  { if (render.onProgress) render.onProgress(); }
        AutoSampler& render;
    } progress;
    double progressValue = 0.0;

    void reset();
};

}
