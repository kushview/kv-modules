
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
    
    // build typical layout
    std::function<String(int)> itemName = [](int index) -> String {
        return String("Item ") + String (index);
    };
    
    int itemNo = 0;
    auto* item1 = dock.createItem (Uuid().toString(), itemName(itemNo++), Dock::TopPlacement);
    auto* item2 = dock.createItem (Uuid().toString(), itemName(itemNo++), Dock::TopPlacement);
    
    auto* item3 = dock.createItem (Uuid().toString(), itemName(itemNo++), Dock::TopPlacement);
    item3->dockTo (item2, Dock::RightPlacement);
    
    auto* item4 = dock.createItem (Uuid().toString(), itemName(itemNo++), Dock::TopPlacement);
    auto* item5 = dock.createItem (Uuid().toString(), itemName(itemNo++), Dock::TopPlacement);
    auto* item6 = dock.createItem (Uuid().toString(), itemName(itemNo++), Dock::TopPlacement);
    item4->dockTo (item1, Dock::RightPlacement);
    item5->dockTo (item1, Dock::RightPlacement);
    item6->dockTo (item1, Dock::RightPlacement);
    
    for (int i = 0; i < 4; ++i)
    {
        auto* item = dock.createItem (Uuid().toString(), itemName(itemNo++), Dock::TopPlacement);
        item->dockTo (item2, Dock::CenterPlacement);
    }
}

MainComponent::~MainComponent()
{
    addItemButton.onClick = nullptr;
}

void MainComponent::paint (Graphics& g)
{
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
