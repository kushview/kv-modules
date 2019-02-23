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

static void maybeFlipLastItem (DockPanel* panel, DockArea* sourceArea)
{
    if (sourceArea != nullptr && sourceArea->getNumItems() == 1)
    {
        DBG(panel->getName() << ": parent area items: " << sourceArea->getNumItems());
        if (auto* sourceParentArea = sourceArea->findParentComponentOfClass<DockArea>())
        {
            DBG(panel->getName() << ": flip last item");
            auto* itemRef = sourceArea->getItem (0);
            const auto sizes = sourceParentArea->getSizesString();
            sourceArea->remove (itemRef);
            sourceParentArea->remove (sourceArea);
            sourceParentArea->insert (-1, itemRef);
            sourceParentArea->setSizes (sizes);
        }
        else
        {
            DBG(panel->getName() << ":source area has no parent");
        }
    }
}

DockPanel::DockPanel() {}
DockPanel::~DockPanel() {}

void DockPanel::dockTo (DockItem* const target, DockPlacement placement)
{
    if (placement.isFloating())
        return;
    
    auto* const source = findParentComponentOfClass<DockItem>();
    auto* const sourceItem = source;
    auto* const sourceArea = sourceItem->getParentArea();
    auto* const targetItem = target;
    auto* const targetArea = targetItem->getParentArea();
    auto* const targetPanel = targetItem->getCurrentPanel();

    if (placement == DockPlacement::Center)
    {
        if (sourceArea != nullptr)
        {
            //noop
        }

        if (sourceItem != nullptr)
        {
            DBG(getName() << ": detaching panel from source");
            sourceItem->detach (this);

            if (sourceItem->getNumPanels() <= 0)
            {
                DBG(getName() << ": source now empty");
            }
            
            if (sourceArea != targetArea)
            {
                maybeFlipLastItem (this, sourceArea);
            }
        }
        
        targetItem->panels.add (this);
        targetItem->refreshPanelContainer (this);
        
        if (sourceArea != nullptr)
        {
            //noop
        }

        return;
    }
    
    DBG("Docking Panel: " << getName() << " to " << Dock::getDirectionString(placement)
        << " of Item: " << targetPanel->getName());
    
    if (nullptr == targetArea)
    {
        jassertfalse; // need an area to dock to
        return;
    }

    Dock& dock (target->dock);

    if (placement.isVertical() == targetArea->isVertical() 
            && (sourceItem != targetItem || sourceItem->getNumPanels() > 1))
    {
        // Same direction as target parent area
        // Not same item unless source has 2 or more panels
        DBG(getName() << ": area is same direction");
        int offsetIdx = 0;

        const bool inSameArea = sourceArea == targetArea;
        
        DBG(getName() << ": same area " << (int) inSameArea);

        int srcIdx = sourceArea->indexOf (sourceItem);
        int tgtIdx = targetArea->indexOf (targetItem);

        const Dock::SplitType split = inSameArea && sourceItem->getNumPanels() == 1
            ? Dock::NoSplit : Dock::getSplitType (placement);
        
        if (sourceItem->getNumPanels() == 1)
        {
            DBG(getName() << ": source item single panel");
            if (inSameArea)
            {
                // same area, so just move the item
                if (tgtIdx < srcIdx)
                {
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
                }
                else if (tgtIdx > srcIdx)
                {
                    if (placement.isVertical())
                    {
                        if (placement == DockPlacement::Top)
                            --offsetIdx;
                    }
                    else
                    {
                        if (placement == DockPlacement::Left)
                            --offsetIdx;
                    }
                }
                
                DBG(getName() << ": move " << srcIdx << " to " << tgtIdx);
                targetArea->moveItem (srcIdx, tgtIdx + offsetIdx);
            }
            else
            {
                // not in same area, need to detach from source and flip if needed
                sourceItem->detach();
                maybeFlipLastItem (this, sourceArea);

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
                
                int insertIdx = targetArea->indexOf (target) + offsetIdx;
                targetArea->insert (insertIdx, source, split);
            }
        }
        else if (source->getNumPanels() > 1)
        {
            DBG(getName() << ": source item panels: " << source->getNumPanels());
            sourceItem->detach (this);
            maybeFlipLastItem (this, sourceArea);

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

            int insertIdx = offsetIdx + targetArea->indexOf (target);
            targetArea->insert (insertIdx, dock.getOrCreateItem (this), split);
        }
    }
    else if (placement.isVertical() != targetArea->isVertical())
    {
        DBG(getName() << ": area is opposite direction");
        // opposite direction as parent
        // Create a new area, flip orientation, add target item, and insert source item.
        const int insertAreaIdx = targetArea->indexOf (target);
        const auto sizes = targetArea->getSizesString();

        DockArea* newArea = dock.getOrCreateArea (placement.isVertical());
        newArea->setSize (targetItem->getWidth(), targetItem->getHeight());
        targetItem->detach();
        newArea->append (targetItem);
        
        const int insertPanelIdx = placement == DockPlacement::Left || placement == DockPlacement::Top ? 0 : -1;
        
        if (sourceItem->getNumPanels() == 1)
        {
            sourceItem->detach();
            newArea->insert (insertPanelIdx, source);
        }
        else if (sourceItem->getNumPanels() > 1)
        {
            sourceItem->detach (this);
            newArea->insert (insertPanelIdx, dock.getOrCreateItem (this));
        }
        else
        {
            newArea = nullptr;
            jassertfalse; // unhandled docking condition or target lost?
        }
        
        if (newArea != nullptr)
        {
            newArea->resized();
            targetArea->insert (insertAreaIdx, newArea, Dock::NoSplit);
            jassert (targetArea->getParentComponent() != nullptr);
            targetArea->setSizes (sizes);
        }
    }
    else
    {
        // unhandled docking condition
        jassertfalse;
    }

    dock.dumpOrphanAreas();
    // dock.removeOrphanObjects();
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
