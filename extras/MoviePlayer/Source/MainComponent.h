
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class TickService;

class MainContentComponent   : public AudioAppComponent,
                               public Button::Listener
{
public:
    MainContentComponent();
    ~MainContentComponent();

    void mouseDown (const MouseEvent&) override;
    void resized() override;
    
    void buttonClicked (Button*) override;
    void run();
    
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    
    AudioDeviceManager& getAudioDeviceManager() { return devices; }
    
private:
    kv::FFmpegDecoder decoder;

    TextButton openButton, audioButton, playButton, stopButton;
    Slider positionSlider;
    
    AudioDeviceManager devices;
    AudioSourcePlayer player;
    AudioTransportSource transport;
    
    ScopedPointer<TickService> tick;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
