/*
    DockItem.cpp - This file is part of Element
    Copyright (C) 2014  Kushview, LLC.  All rights reserved.

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

namespace kv {

class DockItem::DragOverlay : public Component
{
public:
    DragOverlay() = default;
    ~DragOverlay() = default;
    void paint (Graphics &g) override
    {
        g.setOpacity (0.40);
        g.fillAll (Colours::white);
        
        g.setColour (Colours::black);
        g.drawRect (0, 0, getWidth(), getHeight(),2);
        g.drawRect (30, 30, getWidth() - 60, getHeight() - 60);
    }
};

DockItem::DockItem (Dock& parent, const String& id, const String& name)
    : Component (name), dock (parent)
{
    setComponentID (id);
    
    tabs.reset (new DockItemTabs());
    addAndMakeVisible (tabs.get());
    
    addChildComponent (area);
    
    overlay.reset (new DragOverlay());
    addChildComponent (overlay.get(), 9000);
    overlay->setAlpha (0.50);
    
    auto* panel = new DockPanel();
    panel->setName (name);
    panels.add (panel);
    refreshPanelContainer();

    tabs->setCurrentTabIndex (0);
    
    parent.items.add (this);
}

DockItem::DockItem (Dock& parent, DockPanel* panel)
    : Component (panel->getName()), dock (parent)
{
    setComponentID (panel->getComponentID());
    tabs.reset (new DockItemTabs());
    addAndMakeVisible (tabs.get());
    
    overlay.reset (new DragOverlay());
    addChildComponent (overlay.get(), 9000);
    overlay->setAlpha (0.50);
    
    panels.add (panel);
    refreshPanelContainer();
    tabs->setCurrentTabIndex (panels.indexOf (panel));
    
    parent.items.add (this);
}

DockItem::~DockItem()
{
    overlay = nullptr;
    tabs->clearTabs();
    tabs = nullptr;
    panels.clear();
}

int DockItem::getCurrentPanelIndex() const { return tabs->getCurrentTabIndex(); }
DockPanel* DockItem::getCurrentPanel() const
{
    return dynamic_cast<DockPanel*> (tabs->getCurrentContentComponent());
}

void DockItem::dockTo (DockItem* const target, Dock::Placement placement)
{
    for (auto* const panel : panels)
        panel->dockTo (target, placement);
    dock.removeEmptyRootAreas();
}

void DockItem::detach (DockPanel* const panel)
{
    panels.removeObject (panel, false);
    if (panels.size() == 0)
        detach();
    else
        refreshPanelContainer();
}

void DockItem::detach()
{
    if (auto* const area = getParentArea())
    {
        area->detachItem (this);
        area->resized();
        if (auto* const parent = area->findParentComponentOfClass<DockItem>())
            if (parent->getNumItems() <= 0)
                parent->detach();
    }
}

void DockItem::movePanelsTo (DockItem* const target)
{
    Array<DockPanel*> tempPanels;
    for (auto* const panel : panels)
        tempPanels.add (panel);
    panels.clearQuick (false);
    refreshPanelContainer();
    
    for (auto* const panel : tempPanels)
        target->panels.add (panel);
    tempPanels.clearQuick();
    target->refreshPanelContainer();
}

void DockItem::refreshPanelContainer (DockPanel* const panelToSelect)
{
    auto lastIndex = tabs->getCurrentTabIndex();
    tabs->clearTabs();
    const auto colour = findColour(DocumentWindow::backgroundColourId);
    
    for (auto* const panel : panels)
        tabs->addTab (panel->getName(), colour, panel, false);
    
    if (panelToSelect != nullptr && panels.contains (panelToSelect))
        lastIndex = panels.indexOf (panelToSelect);

    if (panels.size() > 0)
        tabs->setCurrentTabIndex (jlimit (0, panels.size() - 1, lastIndex));
}

void DockItem::paint (Graphics& g) { }

void DockItem::resized()
{
    const int indent = 0;
    if (panels.size() > 0)
    {
        auto ir = getLocalBounds().reduced (indent);
        if (overlay->isVisible())
            overlay->centreWithSize (getWidth() - 2, getHeight() - 2);
        tabs->setBounds (ir);
    }
    else
    {
        area.setBounds (getLocalBounds().reduced (indent));
    }
}

void DockItem::mouseDown (const MouseEvent& ev)
{
    if (DockPanel* const panel = getCurrentPanel())
    {
        Image image (Image::ARGB, 1, 1, true);
        dock.startDragging ("DockPanel", panel, image, true);
    }
}

bool DockItem::isInterestedInDragSource (const SourceDetails& details)
{
    // don't dock to container items
    if (getNumItems() > 0)
        return false;

    return details.description.toString() == "DockPanel" ||
           details.description.toString() == "DockItem";
}

void DockItem::itemDropped (const SourceDetails& dragSourceDetails)
{
    overlay->setVisible (false);
    
    auto* const panel = dynamic_cast<DockPanel*> (dragSourceDetails.sourceComponent.get());
    auto* const item  = (panel != nullptr) ? panel->findParentComponentOfClass<DockItem>() : nullptr;
    if (panel == nullptr || item == nullptr)
        return;
    
    const auto x = dragSourceDetails.localPosition.getX();
    const auto y = dragSourceDetails.localPosition.getY();
    Dock::Placement placement = Dock::CenterPlacement;
    
    if (x >= 0 && x < 30 && y >= 30 && y < getHeight() - 30)
        placement = Dock::LeftPlacement;
    else if (x >= getWidth() - 30 && x < getWidth() && y >= 30 && y < getHeight() - 30)
        placement = Dock::RightPlacement;
    if (y >= 0 && y < 30 && x >= 30 && x < getWidth() - 30)
        placement = Dock::TopPlacement;
    else if (y >= getHeight() - 30 && y < getHeight() && x >= 30 && x < getWidth() - 30)
        placement = Dock::BottomPlacement;
    
    const bool isMyPanel = panels.contains (panel);
    
    // same item center, don't add to container of self
    if (isMyPanel && placement == Dock::CenterPlacement)
        return;
    
    panel->dockTo (this, placement);
    
    dock.removeEmptyRootAreas();
    refreshPanelContainer();
}

void DockItem::itemDragEnter (const SourceDetails&)
{
    overlay->toFront (true);
    overlay->setVisible (true);
    resized();
}

void DockItem::itemDragExit (const SourceDetails&)
{
    overlay->setVisible (false);
    resized();
}

bool DockItem::shouldDrawDragImageWhenOver() { return false; }

}
