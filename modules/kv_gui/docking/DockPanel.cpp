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

#ifndef KV_DOCKING_NESTING
 #define KV_DOCKING_NESTING 0
#endif

DockPanel::DockPanel (const Identifier& panelType) 
    : identifier (panelType) { }

DockPanel::~DockPanel() { }

void DockPanel::dockTo (DockItem* const target, DockPlacement placement)
{
    if (placement == DockPlacement::Floating)
        return;
    
    auto* source = findParentComponentOfClass<DockItem>();
    auto* sourceArea = source->getParentArea();
    
    if (placement == DockPlacement::Center)
    {
        if (source != nullptr)
            source->detach (this);
        
        target->panels.add (this);
        target->refreshPanelContainer (this);
        return;
    }
    
    DBG("Docking Panel: " << getName() << " to " << Dock::getDirectionString(placement)
        << " of Item: " << target->getName());
    
    auto* const targetArea = target->getParentArea();
    if (nullptr == targetArea)
    {
        // need an area to dock to
        jassertfalse;
        return;
    }

    Dock& dock (target->dock);

    if (placement.isVertical() == targetArea->isVertical() && (source != target || source->getNumPanels() > 1))
    {
        // Same direction as target parent area
        // Not same item unless source has 2 or more panels
        int offsetIdx = 0;
     
        if (placement.isVertical())
        {
            if (placement == DockPlacement::Bottom)
                ++offsetIdx;
        }
        else
        {
            if (placement == DockPlacement::Right)
                ++offsetIdx;
        }
        
        const Dock::SplitType split = sourceArea == targetArea && source->getNumPanels() == 1
            ? Dock::NoSplit : Dock::getSplitType (placement);
        
        if (source->getNumPanels() == 1)
        {
            source->detach();
            int insertIdx = targetArea->indexOf (target);
            insertIdx += offsetIdx;
            targetArea->insert (insertIdx, source, split);
        }
        else if (source->getNumPanels() > 1)
        {
            source->detach (this);
            int insertIdx = targetArea->indexOf (target);
            insertIdx += offsetIdx;
            targetArea->insert (insertIdx, dock.getOrCreateItem (this), split);
        }
    }
    else if (placement.isVertical() != targetArea->isVertical())
    {
        // opposite direction as parent
        // Create a new area, flip orientation, add target item, and insert source item.
        const int insertAreaIdx = targetArea->indexOf (target);
        
        DockArea* newArea = dock.getOrCreateArea (placement.isVertical());
        newArea->setSize (target->getWidth(), target->getHeight());
        target->detach();
        newArea->append (target);
        
        const int insertPanelIdx = placement == DockPlacement::Left || placement == DockPlacement::Top ? 0 : -1;
        
        if (source->getNumPanels() > 1)
        {
            source->detach (this);
            newArea->insert (insertPanelIdx, dock.getOrCreateItem (this));
        }
        else if (source->getNumPanels() == 1)
        {
            source->detach();
            newArea->insert (insertPanelIdx, source);
        }
        else
        {
            newArea = nullptr;
            jassertfalse; // unhandled docking condition, target lost?
        }
        
        if (newArea != nullptr)
        {
            newArea->resized();
            targetArea->insert (insertAreaIdx, newArea, Dock::NoSplit);
        }
    }
    else
    {
        // unhandled docking condition
        jassertfalse;
    }
}

void DockPanel::close()
{
    // TODO: handle single panel panel in a floating window

    if (auto* source = findParentComponentOfClass<DockItem>())
    {
        source->detach (this);
    }
}

void DockPanel::undock()
{
    DBG("undocking not yet supported");
}

ValueTree DockPanel::getState() const
{
    ValueTree state (Slugs::panel);
    state.setProperty (Slugs::name, getName(), nullptr)
         .setProperty (Slugs::type, getTypeString(), nullptr)
         .setProperty (Slugs::bounds, getLocalBounds().toString(), nullptr);
    return state;
}

void DockPanel::paint (Graphics& g)
{
    g.setColour (Colours::white);
    g.drawText (getName(), 0, 0, getWidth(), getHeight(), Justification::centred);
}

void DockPanel::resized()
{

}
    
}
