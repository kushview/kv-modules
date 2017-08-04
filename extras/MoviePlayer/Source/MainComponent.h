
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class TickService;
class VideoDisplayComponent;
class MainContentComponent   : public AudioAppComponent,
                               public Button::Listener
{
public:
    MainContentComponent();
    ~MainContentComponent();

    void mouseDown (const MouseEvent&) override;
    void resized() override;
    
    void buttonClicked (Button*) override;
    
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    
    AudioDeviceManager& getAudioDeviceManager() { return devices; }
    
private:
    TextButton openButton, audioButton, playButton, stopButton;
    Slider positionSlider;
    
    AudioDeviceManager devices;
    AudioSourcePlayer player;
    AudioTransportSource transport;
    
    friend class TickService;
    ScopedPointer<TickService> tick;
    ScopedPointer<VideoDisplayComponent> display;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
