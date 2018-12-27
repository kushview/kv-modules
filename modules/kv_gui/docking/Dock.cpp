/*
    This file is part of the Kushview Modules for JUCE
    Copyright (C) 2014-2018  Kushview, LLC.  All rights reserved.

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

namespace DockHelpers {


}

Dock::Dock ()
{
    container.reset (new DockContainer());
    addAndMakeVisible (container.get());
}

Dock::~Dock()
{
    container.reset (nullptr);
}

DockPanel* Dock::getOrCreatePanel (const String& panelType)
{
    DockPanel* panel = nullptr;
    if (panelType == "GenericDockPanel")
    {
        panel = new DockPanel();
        panel->setName ("Panel");
    }
    return panel;
}

void Dock::resized()
{
    container->setBounds (getLocalBounds());
}

void Dock::dragOperationStarted (const DragAndDropTarget::SourceDetails& details)
{
//    if (auto* panel = dynamic_cast<DockPanel*> (details.sourceComponent.get()))
//        if (auto* item = dynamic_cast<DockItem*> (panel->findParentComponentOfClass<DockItem>()))
//            item->setMouseCursor (MouseCursor::CopyingCursor);
}

void Dock::dragOperationEnded (const DragAndDropTarget::SourceDetails& details)
{
//    if (auto* panel = dynamic_cast<DockPanel*> (details.sourceComponent.get()))
//        if (auto* item = dynamic_cast<DockItem*> (panel->findParentComponentOfClass<DockItem>()))
//            item->setMouseCursor (MouseCursor::NormalCursor);
}

DockItem* Dock::createItem (const String& itemName, Dock::Placement itemPlacement)
{
    auto* panel = getOrCreatePanel ("GenericDockPanel");
    
    if (! panel)
    {
        jassertfalse;
        return nullptr;
    }
    
    panel->setName (itemName);
    
    if (itemPlacement == Dock::FloatingPlacement)
    {
        auto* window = new DockWindow();
        auto* item = new DockItem (*this, panel);
        window->dockItem (item, Dock::TopPlacement);
        window->setVisible (true);
        window->centreWithSize (window->getWidth(), window->getHeight());
        window->addToDesktop();
        window->toFront (true);
        return item;
    }
    
    if (itemPlacement != LeftPlacement &&
        itemPlacement != TopPlacement &&
        itemPlacement != BottomPlacement &&
        itemPlacement != RightPlacement)
    {
        deleteAndZero (panel);
        return nullptr;
    }
    
    auto* item = new DockItem (*this, panel);
    if (! container->dockItem (item, itemPlacement))
        deleteAndZero (item);
    resized();
    return item;
}

void Dock::removeEmptyRootAreas()
{
    resized();
}

void Dock::startDragging (DockPanel* const panel)
{
    jassert (panel != nullptr);
    Image image (Image::ARGB, 1, 1, true);
    DragAndDropContainer::startDragging ("DockPanel", panel, image, true);
}

}
