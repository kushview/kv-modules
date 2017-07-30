
#include "MainComponent.h"

using kv::FFmpegDecoder;

class TickService : public HighResolutionTimer
{
public:
    TickService (MainContentComponent& u, FFmpegDecoder& d)
        : ui(u), decoder(d) { }
    ~TickService() { }
    
    void hiResTimerCallback() override
    {
        
    }

private:
    MainContentComponent& ui;
    FFmpegDecoder& decoder;
};

MainContentComponent::MainContentComponent()
{
    tick = new TickService (*this, decoder);
    tick->startTimer (1000);
    
    devices.addAudioCallback (&player);
    player.setSource (&transport);
    // video.setVideoReader (nullptr);

    transport.setSource (nullptr);
    // addAndMakeVisible (video);

    addAndMakeVisible (openButton);
    openButton.setButtonText ("Open");
    openButton.addListener (this);
    
    addAndMakeVisible (audioButton);
    audioButton.setButtonText ("Audio");
    audioButton.addListener (this);

    addAndMakeVisible (playButton);
    playButton.setButtonText ("P");
    
    addAndMakeVisible(stopButton);
    stopButton.setButtonText ("S");
    
    addAndMakeVisible (positionSlider);
    positionSlider.setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    
    devices.initialiseWithDefaultDevices (0, 2);
    setSize (640, 360 + 28);
}

MainContentComponent::~MainContentComponent()
{
    devices.removeAudioCallback (&player);
    openButton.removeListener (this);
    // video.setVideoReader (nullptr);
}

void MainContentComponent::mouseDown (const MouseEvent& ev)
{
    // noop
}

void MainContentComponent::resized()
{
    // video.setBounds (0, 0, getWidth(), getHeight() - 28);
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
        decoder.close();
        
        bool movieLoaded = false;
        bool cancelled = false;
        
        FileChooser chooser ("Choose a Movie File", File(), "*.mp4");
        if (chooser.browseForFileToOpen())
        {
            movieLoaded = decoder.openFile (chooser.getResult());
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
}

void MainContentComponent::run()
{
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
