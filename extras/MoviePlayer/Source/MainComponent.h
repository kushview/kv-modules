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
