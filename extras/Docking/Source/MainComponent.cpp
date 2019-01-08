
#include "MainComponent.h"

using namespace kv;

class GenericDockPanel : public DockPanel
{
public:
    GenericDockPanel (const String& panelName) 
        : DockPanel ("GenericDockPanel")
    { 
        setName (panelName);
    }
    ~GenericDockPanel() { }

    void showPopupMenu() override
    {
        PopupMenu menu;
        menu.addItem (1, "Close Panel");
        menu.addItem (2, "Undock Panel");
        const auto result = menu.show();

        switch (result)
        {
            case 1: {
                close();
            } break;

            case 2: {
                undock();
            } break;

            default: break;
        }
    }
};

class GenericPanelType : public DockPanelType
{
public:
    int lastPanelNo = 0;
    const Identifier genericType { "GenericDockPanel" };

    void getAllTypes (OwnedArray<DockPanelInfo>& types) override
    {
        auto* type = types.add (new DockPanelInfo());
        type->identifier = genericType;
        type->name = "Generic";
        type->description = "A generic panel for development purposes";
    }

    DockPanel* createPanel (const Identifier& panelType) override
    {
        if (panelType == genericType)
        {
            ++lastPanelNo;
            return new GenericDockPanel (String("Generic ") + String(lastPanelNo));
        }
        return nullptr;
    }
};

MainComponent::MainComponent()
{
    addAndMakeVisible (dock);
    dock.registerPanelType (new GenericPanelType());
    
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
    auto* item1 = dock.createItem ("GenericDockPanel", DockPlacement::Top);
    auto* item2 = dock.createItem ("GenericDockPanel", DockPlacement::Top);
    
    auto* item3 = dock.createItem ("GenericDockPanel", DockPlacement::Top);
    item3->dockTo (item2, DockPlacement::Right);
    
    auto* item4 = dock.createItem ("GenericDockPanel", DockPlacement::Top);
    auto* item5 = dock.createItem ("GenericDockPanel", DockPlacement::Top);
    auto* item6 = dock.createItem ("GenericDockPanel", DockPlacement::Top);
    item4->dockTo (item1, DockPlacement::Right);
    item5->dockTo (item1, DockPlacement::Right);
    item6->dockTo (item1, DockPlacement::Right);
    
    for (int i = 0; i < 4; ++i)
    {
        auto* item = dock.createItem ("GenericDockPanel", DockPlacement::Top);
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
    String text = "Item "; text << itemNo;
    
    if (auto* item = dock.createItem ("GenericDockPanel", placement))
    {
        ++itemNo;
        if (auto* panel = item->getCurrentPanel())
            panel->setName (text);
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
    {
        dock.applyState (state);
    }
}
