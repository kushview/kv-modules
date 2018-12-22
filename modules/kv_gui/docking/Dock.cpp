/*
    This file is part of the Kushview Modules for JUCE
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

namespace DockHelpers {


}

Dock::Dock ()
    : verticalLayout (*this, true),
      horizontalLayout (*this, false)
{
    
}

Dock::~Dock()
{
    for (int i = 0; i < numPlacements; ++i)
        rootAreas[i].clear();
}

void Dock::detatchAll (DockItem* item)
{
}

DockItem* Dock::getItem (const String& id)
{
    return nullptr;
}

void Dock::resized()
{
    verticalLayout.layoutItems();
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

DockItem* Dock::createItem (const String& itemId, const String& itemName,
                            Dock::Placement itemPlacement)
{
    if (itemPlacement != TopPlacement)
        return nullptr;
    
    if (itemPlacement != LeftPlacement &&
        itemPlacement != TopPlacement &&
        itemPlacement != BottomPlacement &&
        itemPlacement != RightPlacement)
    {
        return nullptr;
    }
    
    const int insertIdx = 0;
    auto& areas = rootAreas [itemPlacement];
    auto* area = areas.insert (insertIdx, new DockArea (itemPlacement));
    auto* item = new DockItem (*this, itemId, itemName);
    addAndMakeVisible (area);
    area->append (item);
    verticalLayout.insert (insertIdx, area);
    resized();
    
    return item;
}

DockItem* Dock::createItem()
{
    return createItem (Uuid().toString(), "Dock Item", Dock::TopPlacement);
}

void Dock::removeEmptyRootAreas()
{
    verticalLayout.clear();
    
    for (int i = 0; i < numPlacements; ++i)
    {
        auto& area = rootAreas [i];
        for (int j = area.size(); --j >= 0;)
        {
            if (area[j]->getNumChildComponents() <= 0)
                area.remove (j);
        }
    }
    
    auto& topAreas = rootAreas [Dock::TopPlacement];
    for (auto* const area : topAreas)
        verticalLayout.append (area);
    
   #if 0
    // attempt to remove invalid items
    for (int i = items.size(); --i >= 0;)
        if (items[i]->panels.size() <= 0)
            if (items[i]->area.getNumChildComponents() <= 0)
                items.remove (i, true);
   #endif

    resized();
}

}
