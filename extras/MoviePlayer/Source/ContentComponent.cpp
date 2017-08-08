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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "ContentComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

/** Simple audio source used for resampling audio from the media file
    This may get absracted in to the Video source api one way or the
    other
 */
class VideoAudioSource : public AudioSource
{
public:
    VideoAudioSource (kv::VideoSource& src)
        : video (src)
    { }
    void prepareToPlay (int blockSize, double sampleRate) override { }
    void releaseResources() override { }
    void getNextAudioBlock (const AudioSourceChannelInfo& buf) override {
        video.renderAudio (buf);
    }
private:
    kv::VideoSource& video;
};

/** Component used as a video display */
class VideoDisplayComponent : public Component,
                              public Timer
{
public:
    ContentComponent& content;
    VideoDisplayComponent (ContentComponent& c)
        : content (c)
    {
        dirty = false;
        startTimerHz (60);
    }

    void paint (Graphics& g) override
    {
        const ScopedLock sl (displayLock);
        if (displayImage.isValid() && ! displayImage.isNull())
        {
            g.drawImageWithin (displayImage, 0, 0, getWidth(), getHeight(), RectanglePlacement::centred);
        }
        else
        {
            g.fillAll (Colours::black);
            g.setColour (Colours::greenyellow);
            g.drawText ("NO VIDEO :(", 0, 0, getWidth(), getHeight(), Justification::centred);
        }
    }

    void timerCallback() override
    {
        content.displayRefreshed();
        updateDirty();
    }

    void setImage (const Image& image)
    {
        displayImage = image;
        dirty = true;
    }

    CriticalSection& getDisplayLock() { return displayLock; }

private:
    CriticalSection displayLock;
    Image displayImage;
    bool dirty;

    void updateDirty()
    {
        const ScopedLock sl (displayLock);
        if (dirty)
        {
            dirty = false;
            repaint();
        }
    }
};

using kv::FFmpegDecoder;
using kv::FFmpegVideoSource;

#include <chrono>
#include <thread>

/** A high precision timer that drives the video source */
class TickService : private Thread
{
public:
    TickService (ContentComponent& u)
        : Thread ("timer"), ui (u),
          videoAudioSource (source),
          resampler (&videoAudioSource, false)
    {
        pts = 0.0;
        playing = 0;
    }

    ~TickService()
    {
        stop();
    }

    bool openFile (const File& file)
    {
        stop();
        source.openFile (file);
        start();
        return true;
    }

    void start()
    {
        stop();
        jassert (isPlaying() == false);
        
        intervalNanoseconds = static_cast<size_t> (1000000000.0 / source.getRealFrameRate().ratio());
        intervalSeconds = source.getRealFrameRate().invertedRatio();
        
        source.prepareToRender();
        
        startThread (9);
    }

    void stop()
    {
        setPlaying (false);
        stopThread (1000);
        jassert (isThreadRunning() == false);
        source.releaseResources();
    }

    void requestPlayPosition (const double seconds)
    {
        nextPts = seconds;
    }

    void setPlaying (const bool isNowPlaying) { playing = isNowPlaying ? 1 : 0; }
    bool isPlaying() const { return playing != 0; }
    
    void setTargetSampleRate (double sampleRate)
    {
        // FIXME: right now assuming all video files are 48KHz
        resampler.setResamplingRatio (48000.0 / sampleRate);
    }
    
    kv::VideoSource& getVideoSource() { return source; }
    kv::AudioSource& getAudioSource() { return resampler; }
    double getCurrentPTS() const { return pts; }
    
private:
    std::atomic<size_t> intervalNanoseconds;
    std::atomic<double> intervalSeconds;
    
    std::atomic<int> playing;
    std::atomic<double> pts;
    std::atomic<double> nextPts;
    std::atomic<double> lastPts;

    ContentComponent& ui;
    FFmpegVideoSource source;
    VideoAudioSource videoAudioSource;
    ResamplingAudioSource resampler;
    
    Image displayImage;

    friend class Thread;
    void run() override
    {
        using namespace std::chrono;
        const nanoseconds interval (intervalNanoseconds);
        auto currentTime = high_resolution_clock::now();
        auto nextTime = currentTime;
        auto zeroTime = currentTime;
        auto errorMargin = nanoseconds::zero();

        int frame = 0;
        nextPts = pts = 0.0;
        const double ptsInterval = 1.0 / source.getRealFrameRate().ratio();

        bool firstFrame = false;
        auto& display (ui.getVideoDisplay());
        while (true)
        {
            currentTime = high_resolution_clock::now();
            if (! firstFrame)
            {
                firstFrame = true;
                zeroTime = currentTime;
            }

            errorMargin = (currentTime > nextTime) ? currentTime - nextTime
                                                   : nanoseconds::zero();
            nextTime = currentTime + interval - errorMargin;

            if (isPlaying())
            {
                { const ScopedLock sl (display.getDisplayLock());
                source.videoTick (pts);
                displayImage = source.findImage (0.0);
                display.setImage (displayImage); }
                pts = pts + ptsInterval;
                ++frame;
            }

            if (threadShouldExit())
                break;

            std::this_thread::sleep_until (nextTime);
        }
    }
};

//[/MiscUserDefs]

//==============================================================================
ContentComponent::ContentComponent ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    tick = new TickService (*this);
    devices.addAudioCallback (&player);
    player.setSource (this);
    //[/Constructor_pre]

    addAndMakeVisible (videoDisplay = new VideoDisplayComponent (*this));
    videoDisplay->setName ("VideoDisplay");

    addAndMakeVisible (openButton = new TextButton ("OpenButton"));
    openButton->setButtonText (TRANS("Open"));
    openButton->addListener (this);

    addAndMakeVisible (positionSlider = new Slider ("positionSlider"));
    positionSlider->setRange (0, 1, 0);
    positionSlider->setSliderStyle (Slider::LinearHorizontal);
    positionSlider->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    positionSlider->addListener (this);

    addAndMakeVisible (forwardButton = new TextButton ("ForwardButton"));
    forwardButton->setButtonText (TRANS(">>"));
    forwardButton->addListener (this);

    addAndMakeVisible (backButton = new TextButton ("backButton"));
    backButton->setButtonText (TRANS("<<"));
    backButton->addListener (this);

    addAndMakeVisible (playButton = new TextButton ("PlayButton"));
    playButton->setButtonText (TRANS(">"));
    playButton->addListener (this);

    addAndMakeVisible (stopButton = new TextButton ("stopButton"));
    stopButton->setButtonText (TRANS("[ ]"));
    stopButton->addListener (this);

    addAndMakeVisible (timecodeLabel = new Label ("timecodeLabel",
                                                  TRANS("00:00:00")));
    timecodeLabel->setFont (Font (18.00f, Font::plain).withTypefaceStyle ("Regular"));
    timecodeLabel->setJustificationType (Justification::centredLeft);
    timecodeLabel->setEditable (false, false, false);
    timecodeLabel->setColour (TextEditor::textColourId, Colours::black);
    timecodeLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (volumeSlider = new Slider ("VolumeSlider"));
    volumeSlider->setTooltip (TRANS("Audio Volume"));
    volumeSlider->setRange (-70, 12, 0.25);
    volumeSlider->setSliderStyle (Slider::LinearBar);
    volumeSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    volumeSlider->addListener (this);

    addAndMakeVisible (audioButton = new TextButton ("AudioButton"));
    audioButton->setButtonText (TRANS("Audio"));
    audioButton->addListener (this);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    sliderValueChanged (volumeSlider);
    lastGain = (float) gain;
    devices.initialiseWithDefaultDevices (0, 2);
    tick->setPlaying (false);
    tick->start();
    //[/Constructor]
}

ContentComponent::~ContentComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    tick->stop();
    
    if (auto* d = devices.getCurrentAudioDevice())
        d->stop();
    devices.removeAudioCallback (&player);
    player.setSource (nullptr);
    
    //[/Destructor_pre]

    videoDisplay = nullptr;
    openButton = nullptr;
    positionSlider = nullptr;
    forwardButton = nullptr;
    backButton = nullptr;
    playButton = nullptr;
    stopButton = nullptr;
    timecodeLabel = nullptr;
    volumeSlider = nullptr;
    audioButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    tick = nullptr;
    //[/Destructor]
}

//==============================================================================
void ContentComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ContentComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    videoDisplay->setBounds (0, 0, proportionOfWidth (1.0000f), getHeight() - 36);
    openButton->setBounds ((getWidth() - 4 - 48) + -4 - 48, 4, 48, 24);
    positionSlider->setBounds (160, getHeight() - 6 - 24, getWidth() - 273, 24);
    forwardButton->setBounds (72, getHeight() - 34, 32, 32);
    backButton->setBounds (2, getHeight() - 34, 32, 32);
    playButton->setBounds (37, getHeight() - 34, 32, 32);
    stopButton->setBounds (121, getHeight() - 34, 32, 32);
    timecodeLabel->setBounds (8, 8, 128, 24);
    volumeSlider->setBounds (getWidth() - 8 - 96, getHeight() - 32, 96, 26);
    audioButton->setBounds (getWidth() - 4 - 48, 4, 48, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ContentComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == openButton)
    {
        //[UserButtonCode_openButton] -- add your button handler code here..
        bool movieLoaded = false;
        bool cancelled = false;

        FileChooser chooser ("Choose a Movie File", File(), "*.mp4");
        if (chooser.browseForFileToOpen())
        {
            movieLoaded = tick->openFile (chooser.getResult());
        }
        else
        {
            cancelled = true;
        }

        if (cancelled)
            return;

        if (! movieLoaded)
        {
            AlertWindow::showMessageBox (AlertWindow::WarningIcon, "Error",
                                         "Could not open movie file");
        }
        else
        {

        }
        //[/UserButtonCode_openButton]
    }
    else if (buttonThatWasClicked == forwardButton)
    {
        //[UserButtonCode_forwardButton] -- add your button handler code here..
        //[/UserButtonCode_forwardButton]
    }
    else if (buttonThatWasClicked == backButton)
    {
        //[UserButtonCode_backButton] -- add your button handler code here..
        //[/UserButtonCode_backButton]
    }
    else if (buttonThatWasClicked == playButton)
    {
        //[UserButtonCode_playButton] -- add your button handler code here..
        tick->setPlaying (! tick->isPlaying());
        stabilizeComponents();
        //[/UserButtonCode_playButton]
    }
    else if (buttonThatWasClicked == stopButton)
    {
        //[UserButtonCode_stopButton] -- add your button handler code here..
        tick->setPlaying (false);
        tick->requestPlayPosition (0.0);
        stabilizeComponents();
        //[/UserButtonCode_stopButton]
    }
    else if (buttonThatWasClicked == audioButton)
    {
        //[UserButtonCode_audioButton] -- add your button handler code here..
        DialogWindow::LaunchOptions dialog;
        dialog.content.setOwned (new AudioDeviceSelectorComponent (
                                devices, 0, 0, 2, 2, false, false, true,false));
        dialog.dialogTitle = "Audio Device";
        dialog.componentToCentreAround = this;
        dialog.content->setSize (384, 444);
        dialog.resizable = false;
        dialog.runModal();
        //[/UserButtonCode_audioButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void ContentComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == positionSlider)
    {
        //[UserSliderCode_positionSlider] -- add your slider handling code here..
        tick->requestPlayPosition (positionSlider->getValue());
        //[/UserSliderCode_positionSlider]
    }
    else if (sliderThatWasMoved == volumeSlider)
    {
        //[UserSliderCode_volumeSlider] -- add your slider handling code here..
        gain = Decibels::decibelsToGain (volumeSlider->getValue(), volumeSlider->getMinimum());
        //[/UserSliderCode_volumeSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
VideoDisplayComponent& ContentComponent::getVideoDisplay() { jassert(videoDisplay); return *videoDisplay; }

void ContentComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    if (! tick)
        return;
    
    tick->getAudioSource().prepareToPlay (samplesPerBlockExpected, sampleRate);
    tick->setTargetSampleRate (sampleRate);
}

void ContentComponent::releaseResources()
{
    if (tick)
        tick->getAudioSource().releaseResources();
}

void ContentComponent::getNextAudioBlock (const AudioSourceChannelInfo& buf)
{
    if (! tick)
    {
        buf.clearActiveBufferRegion();
        return;
    }
    
    auto& source (tick->getAudioSource());
    source.getNextAudioBlock (buf);

    if (gain != lastGain)
    {
        buf.buffer->applyGainRamp (buf.startSample, buf.numSamples, lastGain, gain);
        lastGain = (float) gain;
    }
    else
    {
        buf.buffer->applyGain (gain);
    }
}

void ContentComponent::stabilizeComponents()
{
    playButton->setToggleState (tick->isPlaying(), dontSendNotification);
    playButton->setButtonText (playButton->getToggleState() ? "||" : ">");
}

void ContentComponent::displayRefreshed()
{
    if (! tick)
        return;
    
    if (tick->isPlaying())
    {
        timecodeLabel->setText (String(tick->getCurrentPTS(), 1),
                                dontSendNotification);
    }
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ContentComponent" componentName=""
                 parentClasses="public AudioAppComponent" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <GENERICCOMPONENT name="VideoDisplay" id="e58591e57002c5aa" memberName="videoDisplay"
                    virtualName="" explicitFocusOrder="0" pos="0 0 100% 36M" class="VideoDisplayComponent"
                    params="*this"/>
  <TEXTBUTTON name="OpenButton" id="4eae98cbc122baae" memberName="openButton"
              virtualName="" explicitFocusOrder="0" pos="-4r 4 48 24" posRelativeX="979517d4887d9531"
              buttonText="Open" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <SLIDER name="positionSlider" id="2522ccd49b117336" memberName="positionSlider"
          virtualName="" explicitFocusOrder="0" pos="160 6Rr 273M 24" min="0"
          max="1" int="0" style="LinearHorizontal" textBoxPos="NoTextBox"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1"
          needsCallback="1"/>
  <TEXTBUTTON name="ForwardButton" id="d08712945d2f2299" memberName="forwardButton"
              virtualName="" explicitFocusOrder="0" pos="72 34R 32 32" buttonText="&gt;&gt;"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="backButton" id="6fc9b1d1be2e9ee3" memberName="backButton"
              virtualName="" explicitFocusOrder="0" pos="2 34R 32 32" buttonText="&lt;&lt;"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="PlayButton" id="b8a4342275cbd63" memberName="playButton"
              virtualName="" explicitFocusOrder="0" pos="37 34R 32 32" buttonText="&gt;"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="stopButton" id="ff90c3bbcad821e1" memberName="stopButton"
              virtualName="" explicitFocusOrder="0" pos="121 34R 32 32" buttonText="[ ]"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="timecodeLabel" id="4dbdbfe5ee7da251" memberName="timecodeLabel"
         virtualName="" explicitFocusOrder="0" pos="8 8 128 24" edTextCol="ff000000"
         edBkgCol="0" labelText="00:00:00" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="18"
         kerning="0" bold="0" italic="0" justification="33"/>
  <SLIDER name="VolumeSlider" id="51d1dcb4238f3fec" memberName="volumeSlider"
          virtualName="" explicitFocusOrder="0" pos="8Rr 32R 96 26" tooltip="Audio Volume"
          min="-70" max="12" int="0.25" style="LinearBar" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"
          needsCallback="1"/>
  <TEXTBUTTON name="AudioButton" id="979517d4887d9531" memberName="audioButton"
              virtualName="" explicitFocusOrder="0" pos="4Rr 4 48 24" buttonText="Audio"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
