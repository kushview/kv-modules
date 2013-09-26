#ifndef GRAPHPLAYER_HPP
#define GRAPHPLAYER_HPP

#include "element/juce.hpp"

namespace element {

    class GraphProcessor;

    class JUCE_API  GraphPlayer    : public AudioIODeviceCallback,
                                     public MidiInputCallback
    {
    public:

        //==============================================================================
        GraphPlayer();

        /** Destructor. */
        virtual ~GraphPlayer();

        //==============================================================================
        /** Sets the processor that should be played.

            The processor that is passed in will not be deleted or owned by this object.
            To stop anything playing, pass in 0 to this method.
        */
        void setRootGraph (GraphProcessor* graph);

        /** Returns the current audio processor that is being played.
        */
        GraphProcessor* rootGraph() const { return processor; }

        /** Returns a midi message collector that you can pass midi messages to if you
            want them to be injected into the midi stream that is being sent to the
            processor.
        */
        MidiMessageCollector& midiCollector() { return messageCollector; }

        //==============================================================================
        /** @internal */
        void audioDeviceIOCallback (const float** inputChannelData,
                                    int totalNumInputChannels,
                                    float** outputChannelData,
                                    int totalNumOutputChannels,
                                    int numSamples) override;
        /** @internal */
        void audioDeviceAboutToStart (AudioIODevice*) override;
        /** @internal */
        void audioDeviceStopped() override;
        /** @internal */
        void handleIncomingMidiMessage (MidiInput* i, const MidiMessage&) override;

    private:

        //==============================================================================
        GraphProcessor* processor;
        CriticalSection lock;
        double sampleRate;
        int blockSize;
        bool isPrepared;

        int numInputChans, numOutputChans;
        HeapBlock<float*> channels;
        AudioSampleBuffer tempBuffer;

        MidiBuffer incomingMidi;
        MidiMessageCollector messageCollector;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphPlayer)
    };



}
#endif // GRAPHPLAYER_HPP
