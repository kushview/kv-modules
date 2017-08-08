/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.1.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"

class VideoAudioSource;
class TickService;
class VideoDisplayComponent;

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
                                                                    //[/Comments]
*/
class ContentComponent  : public AudioAppComponent,
                          public ButtonListener,
                          public SliderListener
{
public:
    //==============================================================================
    ContentComponent ();
    ~ContentComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    VideoDisplayComponent& getVideoDisplay();
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    AudioDeviceManager devices;
    AudioSourcePlayer player;
    AudioTransportSource transport;

    friend class TickService;
    ScopedPointer<TickService> tick;

    std::atomic<float> gain, lastGain;
    ScopedPointer<VideoAudioSource> video;
    ScopedPointer<ResamplingAudioSource> resampler;
    
    friend class VideoDisplayComponent;
    void stabilizeComponents();
    void displayRefreshed();
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<VideoDisplayComponent> videoDisplay;
    ScopedPointer<TextButton> openButton;
    ScopedPointer<Slider> positionSlider;
    ScopedPointer<TextButton> forwardButton;
    ScopedPointer<TextButton> backButton;
    ScopedPointer<TextButton> playButton;
    ScopedPointer<TextButton> stopButton;
    ScopedPointer<Label> timecodeLabel;
    ScopedPointer<Slider> volumeSlider;
    ScopedPointer<TextButton> audioButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ContentComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]
