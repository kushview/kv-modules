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

DockItem* Dock::createItem (const String& itemName, DockPlacement placement)
{
    auto* panel = getOrCreatePanel ("GenericDockPanel");
    
    if (! panel)
    {
        jassertfalse;
        return nullptr;
    }
    
    panel->setName (itemName);
    
    if (placement.isFloating())
    {
        auto* window = new DockWindow();
        auto* item = new DockItem (*this, panel);
        window->dockItem (item, DockPlacement::Top);
        window->setVisible (true);
        window->centreWithSize (window->getWidth(), window->getHeight());
        window->addToDesktop();
        window->toFront (true);
        return item;
    }
    
    if (! placement.isDirectional())
    {
        deleteAndZero (panel);
        return nullptr;
    }
    
    auto* item = new DockItem (*this, panel);
    if (! container->dockItem (item, placement))
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

ValueTree Dock::getState() const
{
    ValueTree state ("dock");
    state.setProperty ("bounds", getLocalBounds().toString(), nullptr);
    state.addChild (container->root->getState(), -1, nullptr);
    return state;
}

void Dock::loadArea (DockArea& area, const ValueTree& state)
{
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        const auto child = state.getChild (i);
        const auto bounds = Dock::getBounds (child);
        
        if (child.hasType ("item"))
        {
            auto* item = new DockItem (*this, "loaded", "Loaded");
            item->tabs->clearTabs();
            item->panels.clear();
            item->setBounds (bounds);
            loadItem (*item, child);
            area.append (item);
        }
        else if (child.hasType ("area"))
        {
            auto* newArea = new DockArea (! area.isVertical());
            newArea->setBounds (bounds);
            loadArea (*newArea, child);
            area.append (newArea);
        }
    }
}

void Dock::loadItem (DockItem& item, const ValueTree& state)
{ 
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        const auto child = state.getChild (i);
        
        if (child.hasType ("panel"))
        {
            auto* panel = new DockPanel();
            item.panels.add (panel);
            panel->setBounds (item.getLocalBounds());
            panel->setName (child.getProperty (Slugs::name, "Panel").toString());
        }
        else
        {
            DBG(child.toXmlString());
            jassertfalse; // not a panel?
        }
    }

    item.refreshPanelContainer();
}

bool Dock::applyState (const ValueTree& state)
{
    if (! state.hasType ("dock"))
        return false;

    std::unique_ptr<DockContainer> newContainer;
    
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        const auto child = state.getChild (i);
        if (child.hasType ("area"))
        {
            if (newContainer == nullptr)
            {
                newContainer.reset (new DockContainer());
                const auto bounds = Dock::getBounds (child);
                newContainer->setBounds (bounds);
                newContainer->resized();
                auto& area (newContainer->getRootArea());
                area.setVertical ((bool) child.getProperty ("vertical", true));
                area.setBounds (bounds);
                loadArea (area, child);
            }
            else
            {
                // root state should only have one area child
                jassertfalse;
            }
        }
    }

    if (newContainer)
    {
        removeChildComponent (container.get());
        container.swap (newContainer);
        addAndMakeVisible (container.get());
        resized();
        return true;
    }

    return false;
}

}
