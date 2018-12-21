
#include "MainComponent.h"

using namespace kv;

MainComponent::MainComponent()
{
    addAndMakeVisible (dock);
    addAndMakeVisible (addItemButton);
    addItemButton.setButtonText ("Add Item");
    addItemButton.onClick = std::bind (&MainComponent::addDockItem, this);
    
    addAndMakeVisible (placementCombo);
    placementCombo.addItem ("Top", 1 + Dock::TopPlacement);
    placementCombo.addItem ("Left", 1 + Dock::LeftPlacement);
    placementCombo.addItem ("Right", 1 + Dock::RightPlacement);
    placementCombo.addItem ("Bottom", 1 + Dock::BottomPlacement);
    placementCombo.addItem ("Floating", 1 + Dock::FloatingPlacement);
    
    setSize (600 * 2, 400 * 2);
}

MainComponent::~MainComponent()
{
    addItemButton.onClick = nullptr;
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

//    g.setFont (Font (16.0f));
//    g.setColour (Colours::white);
//    g.drawText ("Hello World!", getLocalBounds(), Justification::centred, true);
}

void MainComponent::resized()
{
    auto r = getLocalBounds().reduced(2);
    auto r2 = r.removeFromTop(20);
    
    addItemButton.setBounds (r2.removeFromLeft(60));
    r2.removeFromLeft(2);
    placementCombo.setBounds (r2.removeFromLeft (90));
    r.removeFromTop (2);
    dock.setBounds (r);
}

void MainComponent::addDockItem ()
{
    placement = placementCombo.getSelectedId() - 1;
    if (! isPositiveAndBelow (placement, Dock::numPlacements))
        placement = kv::Dock::TopPlacement;
    
    static int itemNo = 1;
    String text = "Docking Item "; text << itemNo;
    
    if (auto* item = dock.createItem (Uuid().toString(), text,
        static_cast<Dock::Placement> (placement)))
    {
        ++itemNo;
    }
    else
    {
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon, "Error", "Could not add dock item");
    }
}
