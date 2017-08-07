/*
    This file is part of the Kushview Modules for JUCE
    Copyright (C) 2017  Kushview, LLC.  All rights reserved.

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

#include "MainComponent.h"

using kv::FFmpegDecoder;
using kv::FFmpegVideoSource;

#include <chrono>
#include <thread>

class VideoDisplayComponent : public Component, 
                              public Timer
{
public:
    VideoDisplayComponent()
    {
        dirty = false;
        startTimerHz (60);
    }
    
    void paint (Graphics& g) override
    {
        const ScopedLock sl (displayLock);
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
        const ScopedLock sl (displayLock);
        if (dirty)
        {
            dirty = false;
            repaint();
        }
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
};

class TickService : private Thread
{
public:
    TickService (MainContentComponent& u)
        : Thread ("timer"),
          ui (u)
    { }
    
    ~TickService()
    {
        stop();
    }
    
    bool openFile (const File& file)
    {
        source.openFile (file);
        return true;
    }

    void start()
    {
        if (isPlaying())
            stop();
        
        jassert (playing == false);
        intervalNanoseconds = static_cast<size_t> (1000000000.0 / source.getRealFrameRate().ratio());
        playing = true;
        source.prepareToRender();
        startThread (9);
    }
    
    void stop()
    {
        if (isThreadRunning())
            stopThread (100);
        
        source.releaseResources();
        playing = false;
    }
    
    bool isPlaying() const { return playing; }
    
    kv::VideoSource& getVideoSource() { return source; }

private:
    std::atomic<size_t> intervalNanoseconds;

    bool playing;
    MainContentComponent& ui;
    FFmpegVideoSource source;
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
        double pts = 0.0;
        double lastPts = 0.0;
        const double ptsInterval = 1.0 / 60.0;
        
        bool firstFrame = false;
        auto& display (*ui.display);
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
            
            {
                const ScopedLock sl (display.getDisplayLock());
                source.videoTick (pts);
                displayImage = source.findImage (0.0);
                display.setImage (displayImage);
            }
            
           #if 0
            const double s = (currentTime - zeroTime).count() / 1000000000.0;
            DBG("frame: " << frame << " pts: " << pts << " seconds: " << s);
           #endif

            ++frame;
            lastPts = pts;
            pts += ptsInterval;

            if (threadShouldExit())
                break;

            std::this_thread::sleep_until (nextTime);
        }
    }
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
    
    addAndMakeVisible (stopButton);
    stopButton.setButtonText ("S");
    stopButton.addListener (this);
    
    addAndMakeVisible (positionSlider);
    positionSlider.setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    
    devices.initialiseWithDefaultDevices (0, 2);
    setSize (640, 360 + 28);
}

MainContentComponent::~MainContentComponent()
{
    openButton.removeListener (this);
    devices.removeAudioCallback (&player);
    tick->stop();
    tick = nullptr;
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
            display->resized();            
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

void MainContentComponent::getNextAudioBlock (const AudioSourceChannelInfo& buf)
{
    auto& source (tick->getVideoSource());
    source.renderAudio (buf);
}
