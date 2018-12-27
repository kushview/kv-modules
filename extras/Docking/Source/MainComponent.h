
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class MainComponent   : public Component
{
public:
    MainComponent();
    ~MainComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    kv::Dock dock;
    TextButton addItemButton;
    ComboBox placementCombo;
    int placement = kv::DockPlacement::Top;
    void addDockItem();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
