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
    
void DockPanel::dockTo (DockItem* const target, Dock::Placement placement)
{
    if (placement == Dock::FloatingPlacement)
        return;
    
    auto* source = findParentComponentOfClass<DockItem>();
    auto* sourceArea = source->getParentArea();
    
    if (placement == Dock::CenterPlacement)
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
    
    const bool wantsVerticalPlacement = placement == Dock::TopPlacement || placement == Dock::BottomPlacement;
    
    if (wantsVerticalPlacement == targetArea->isVertical())
    {
        // same direction as target parent area
        int offsetIdx = 0;
     
        if (wantsVerticalPlacement)
        {
            if (placement == Dock::BottomPlacement)
                ++offsetIdx;
        }
        else
        {
            if (placement == Dock::RightPlacement)
                ++offsetIdx;
        }
        
        const Dock::SplitType split = sourceArea == targetArea
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
            targetArea->insert (insertIdx, new DockItem (source->dock, this), split);
        }
    }
    else if (target->getNumItems() <= 0)
    {
        // opposite direction as target parent area and not
        auto& area = target->area;
        auto& tabs = *target->tabs;
        tabs.setVisible (false);
        area.setVisible (true);
        area.setVertical (! targetArea->isVertical());
        
        const Dock::SplitType split = sourceArea == &area
            ? Dock::NoSplit : Dock::getSplitType (placement);
        
        std::unique_ptr<DockItem> item0;
        item0.reset (new DockItem (target->dock, "Temp Panel", "Temp Panel"));
        item0->tabs->clearTabs();
        item0->panels.clear();
        item0->setSize (target->getWidth(), target->getHeight());
        target->movePanelsTo (item0.get());
        area.append (item0.release());
        
        if (split != Dock::NoSplit)
        {
            source->detach (this);
            area.insert (-1, new DockItem (target->dock, this), split);
            target->resized();
            target->repaint();
        }
        else
        {
            jassertfalse;
        }
    }
    else
    {
        // unhandled docking condition
        jassertfalse;
    }
}

}
