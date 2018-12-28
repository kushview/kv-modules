
#include "MainComponent.h"

using namespace kv;

MainComponent::MainComponent()
{
    addAndMakeVisible (dock);
    
    addAndMakeVisible (addItemButton);
    addItemButton.setButtonText ("Add Item");
    addItemButton.onClick = std::bind (&MainComponent::addDockItem, this);
    
    addAndMakeVisible (placementCombo);
    placementCombo.addItem ("Top", 1 + DockPlacement::Top);
    placementCombo.addItem ("Left", 1 + DockPlacement::Left);
    placementCombo.addItem ("Right", 1 + DockPlacement::Right);
    placementCombo.addItem ("Bottom", 1 + DockPlacement::Bottom);
    placementCombo.addItem ("Floating", 1 + DockPlacement::Floating);
    placementCombo.setSelectedItemIndex (0);
    
    addAndMakeVisible (saveButton);
    saveButton.setButtonText ("Save Layout");
    saveButton.onClick = std::bind (&MainComponent::saveLayout, this);

    addAndMakeVisible (loadButton);
    loadButton.setButtonText ("Load Layout");
    loadButton.onClick = std::bind (&MainComponent::loadLayout, this);
    
    setSize (600 * 2, 400 * 2);
    
    // build typical layout
    std::function<String(int)> itemName = [](int index) -> String {
        return String("Item ") + String (index);
    };
    
    int itemNo = 0;
    auto* item1 = dock.createItem (itemName(itemNo++), DockPlacement::Top);
    auto* item2 = dock.createItem (itemName(itemNo++), DockPlacement::Top);
    
    auto* item3 = dock.createItem (itemName(itemNo++), DockPlacement::Top);
    item3->dockTo (item2, DockPlacement::Right);
    
    auto* item4 = dock.createItem (itemName(itemNo++), DockPlacement::Top);
    auto* item5 = dock.createItem (itemName(itemNo++), DockPlacement::Top);
    auto* item6 = dock.createItem (itemName(itemNo++), DockPlacement::Top);
    item4->dockTo (item1, DockPlacement::Right);
    item5->dockTo (item1, DockPlacement::Right);
    item6->dockTo (item1, DockPlacement::Right);
    
    for (int i = 0; i < 4; ++i)
    {
        auto* item = dock.createItem (itemName(itemNo++), DockPlacement::Top);
        item->dockTo (item2, DockPlacement::Center);
    }
}

MainComponent::~MainComponent()
{
    addItemButton.onClick = nullptr;
    loadButton.onClick = nullptr;
    saveButton.onClick = nullptr;
}

void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto r = getLocalBounds().reduced(2);
    auto r2 = r.removeFromTop(20);
    
    addItemButton.setBounds (r2.removeFromLeft(60));
    r2.removeFromLeft(2);
    placementCombo.setBounds (r2.removeFromLeft (90));
    r2.removeFromLeft (6);
    saveButton.setBounds (r2.removeFromLeft (60));
    r2.removeFromLeft(2);
    loadButton.setBounds (r2.removeFromLeft (60));
    
    r.removeFromTop (2);
    dock.setBounds (r);
}

void MainComponent::addDockItem()
{
    placement = placementCombo.getSelectedId() - 1;
    if (! DockPlacement::isValid (placement))
        placement = kv::DockPlacement::Top;
    
    static int itemNo = 1;
    String text = "Docking Item "; text << itemNo;
    
    if (auto* item = dock.createItem (text, placement))
    {
        ++itemNo;
    }
    else
    {
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon, "Error", "Could not add dock item");
    }
}

void MainComponent::saveLayout()
{
    FileChooser chooser ("Save Layout", File(), "*.xml", true, true, this);
    if (chooser.browseForFileToSave (true))
    {
        auto state = dock.getState();
        if (auto* xml = state.createXml())
        {
            xml->writeToFile (chooser.getResult(), String());
            deleteAndZero (xml);
        }
    }
}

void MainComponent::loadLayout()
{
    FileChooser chooser ("Load Layout", File(), "*.xml", true, true, this);
    ValueTree state;
    if (chooser.browseForFileToOpen())
    {
        if (auto* xml = XmlDocument::parse (chooser.getResult()))
        {
            state = ValueTree::fromXml (*xml);
            deleteAndZero (xml);
        }
    }

    if (state.isValid())
        dock.applyState (state);
}
