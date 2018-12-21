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

DockItem::DockItem (Dock& parent, const String& id, const String& name)
    : Component (name), dock (parent),
      tabs (TabbedButtonBar::TabsAtTop)
{
    setComponentID (id);
    addAndMakeVisible (tabs);
    
    addChildComponent (overlay, 9000);
    overlay.setAlpha (0.50);
    
    auto* panel = new DockPanel();
    panel->setName (name);
    panels.add (panel);
    buildTabs();
    tabs.setCurrentTabIndex(0);
}

DockItem::DockItem (Dock& parent, DockPanel* panel)
    : Component (panel->getName()), dock (parent),
      tabs (TabbedButtonBar::TabsAtTop)
{
    setComponentID (panel->getComponentID());
    addAndMakeVisible (tabs);
    
    addChildComponent (overlay, 9000);
    overlay.setAlpha (0.50);
    
    panels.add (panel);
    buildTabs();
    tabs.setCurrentTabIndex (panels.indexOf (panel));
}

DockItem::~DockItem()
{
    tabs.clearTabs();
    panels.clear();
}

DockPanel* DockItem::getCurrentPanel() const
{
    return dynamic_cast<DockPanel*> (tabs.getCurrentContentComponent());
}

void DockItem::dockTo (DockItem* const target, Dock::Placement placement)
{
    for (auto* const panel : panels)
        panel->dockTo (target, placement);
}

void DockItem::buildTabs()
{
    const auto lastIndex = tabs.getCurrentTabIndex();
    tabs.clearTabs();
    for (auto* const panel : panels)
        tabs.addTab (panel->getName(), Colours::black, panel, false);
    tabs.setCurrentTabIndex (jlimit (0, panels.size() - 1, lastIndex));
}

void DockItem::paint (Graphics& g)
{
    g.fillAll (Colour (0xff595959));
}

void DockItem::resized()
{
    auto ir = getLocalBounds();
    ir.removeFromTop (20);
    ir.removeFromLeft (2);
    ir.removeFromRight (2);
    if (overlay.isVisible())
        overlay.centreWithSize (getWidth() - 2, getHeight() - 2);
    tabs.setBounds (ir);
}

void DockItem::mouseDown (const MouseEvent& ev)
{
    Component::mouseDown (ev);
    if (DockPanel* const panel = getCurrentPanel())
    {
        Image image (Image::ARGB, 1, 1, true);
        dock.startDragging ("DockPanel", panel, image, true);
    }
}

bool DockItem::isInterestedInDragSource (const SourceDetails& details)
{
    return details.description.toString() == "DockPanel" ||
           details.description.toString() == "DockItem";
}

void DockItem::itemDropped (const SourceDetails& dragSourceDetails)
{
    overlay.setVisible (false);
    
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
    buildTabs();
}

void DockItem::itemDragEnter (const SourceDetails&)
{
    overlay.toFront (true);
    overlay.setVisible (true);
    resized();
}

void DockItem::itemDragExit (const SourceDetails&)
{
    overlay.setVisible (false);
    resized();
}

bool DockItem::shouldDrawDragImageWhenOver() { return false; }

}
