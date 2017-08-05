
#include "MainComponent.h"

using kv::FFmpegDecoder;
using kv::FFmpegVideoSource;

namespace kv {

}

class VideoDisplayComponent : public Component,
public Timer
{
public:
    VideoDisplayComponent()
    {
        dirty = false;
        startTimerHz (15);
    }
    
    void paint (Graphics& g) override
    {
        if (displayImage.isValid() && ! displayImage.isNull())
            g.drawImage (displayImage, 0, 0, getWidth(), getHeight(),
                         0, 0, displayImage.getWidth(), displayImage.getHeight());
        else
        {
            g.fillAll (Colours::black);
            g.setColour (Colours::greenyellow);
            g.drawText ("NO VIDEO :(", 0, 0, getWidth(), getHeight(), Justification::centred);
        }
    }
    
    void timerCallback() override
    {
        if (dirty)
        {
            dirty = false;
            repaint();
        }
    }
    
    void setImage (const Image& image)
    {
        // MessageManagerLock lock;
        displayImage = image;
        dirty = true;
    }
    
private:
    Image displayImage;
    bool dirty;
};

class TickService : public HighResolutionTimer
{
public:
    TickService (MainContentComponent& u)
        : timebase (1, 60),
          ui (u)
    { }
    
    ~TickService() { }
    
    void start()
    {
        if (isTimerRunning())
            stopTimer();
        
        masterPTS = 0;
        playing = true;
        interval = roundDoubleToInt (1000.0 * timebase.ratio());
        startTimer (interval);
    }
    
    void stop()
    {
        playing = false;
        stopTimer();
    }
    
    bool isPlaying() const { return playing; }
    
    void hiResTimerCallback() override
    {
        if (playing)
        {
            source.process (masterPTS);
            displayImage = source.findImage ((double) masterPTS * 0.001);
            ui.display->setImage (displayImage);
            
            masterPTS += interval;
            if (masterPTS % 1000 == 0)
            {
                DBG("tick seconds: " << (double)masterPTS / 1000.0);
            }
        }
    }
    
    std::atomic<int64> masterPTS;
    
    int interval;
    bool playing;
    kv::Rational timebase;
    MainContentComponent& ui;
    FFmpegVideoSource source;
    Image displayImage;
};

MainContentComponent::MainContentComponent()
{
    tick = new TickService (*this);
    addAndMakeVisible (display = new VideoDisplayComponent());
    
    devices.addAudioCallback (&player);
    player.setSource (this);

    addAndMakeVisible (openButton);
    openButton.setButtonText ("Open");
    openButton.addListener (this);
    
    addAndMakeVisible (audioButton);
    audioButton.setButtonText ("Audio");
    audioButton.addListener (this);

    addAndMakeVisible (playButton);
    playButton.setButtonText ("P");
    playButton.addListener (this);
    
    addAndMakeVisible(stopButton);
    stopButton.setButtonText ("S");
    stopButton.addListener (this);
    
    addAndMakeVisible (positionSlider);
    positionSlider.setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    
    devices.initialiseWithDefaultDevices (0, 2);
    setSize (640, 360 + 28);
}

MainContentComponent::~MainContentComponent()
{
    tick->stopTimer();
    tick = nullptr;
    devices.removeAudioCallback (&player);
    openButton.removeListener (this);
}

void MainContentComponent::mouseDown (const MouseEvent& ev)
{
    // noop
}

void MainContentComponent::resized()
{
    display->setBounds (0, 0, getWidth(), getHeight() - 28);
    openButton.setBounds (0, getHeight() - 28, 90, 28);
    audioButton.setBounds (openButton.getRight(), getHeight() - 28,
                           90, 28);
    playButton.setBounds (0, 0, 28, 28);
    stopButton.setBounds (32, 0, 28, 28);
    
    positionSlider.setBounds (audioButton.getRight(), getHeight() - 28,
                              getWidth() - audioButton.getRight(), 28);
}

void MainContentComponent::buttonClicked (Button* button)
{
    if (button == &openButton)
    {
        bool movieLoaded = false;
        bool cancelled = false;
        
        FileChooser chooser ("Choose a Movie File", File(), "*.mp4");
        if (chooser.browseForFileToOpen())
        {
            tick->stop();
            display->setImage (Image());
            tick->source.openFile (chooser.getResult());
            movieLoaded = true;
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
    }
    
    if (button == &audioButton)
    {
        DialogWindow::LaunchOptions dialog;
        dialog.content.setOwned (new AudioDeviceSelectorComponent(
            devices, 0, 0, 2, 2, false, false, true,false ));
        dialog.dialogTitle = "Audio Device";
        dialog.componentToCentreAround = this;
        dialog.content->setSize (384, 444);
        dialog.resizable = false;
        dialog.runModal();
    }
    
    if (button == &playButton)
    {
        tick->start();
    }
    
    if (button == &stopButton)
    {
        tick->stop();
    }
}

void MainContentComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transport.prepareToPlay (samplesPerBlockExpected, sampleRate);
}

void MainContentComponent::releaseResources()
{
    transport.releaseResources();
}

void MainContentComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    transport.getNextAudioBlock (bufferToFill);
}
