
#pragma once

#include "kv/kv.h"
using namespace juce;

class MainComponent   : public Component
{
public:
    MainComponent();
    ~MainComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    kv::Dock dock;
    TextButton addItemButton, saveButton, loadButton;
    ComboBox placementCombo;
    int placement = kv::DockPlacement::Top;
    void addDockItem();
    void saveLayout();
    void loadLayout();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
