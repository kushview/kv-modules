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

DockItem* createDockAreaItemFor (Dock& parent)
{
    auto* item = new DockItem (parent, "", "");
    auto* area = new DockArea();
    item->setContentOwned (area);
    return item;
}

}

void Dock::detatchAll (DockItem* item)
{
}

Dock::Dock ()
{
    for (int i = 0; i < numPlacements; ++i)
        addAndMakeVisible (rootAreas.add (new DockArea ()));
    rootAreas[LeftPlacement]->setVertical (false);
    rootAreas[TopPlacement]->setVertical (true);
    rootAreas[BottomPlacement]->setVertical (true);
    rootAreas[RightPlacement]->setVertical (false);
}

Dock::~Dock()
{
    
}

DockItem* Dock::getItem (const String& id)
{
    return nullptr;
}

void Dock::resized()
{
    if (auto* const top = rootAreas [Dock::TopPlacement])
        top->setBounds (getLocalBounds());
}

DockItem* Dock::createItem (const String& itemId, const String& itemName,
                            Dock::Placement itemPlacement)
{
    std::unique_ptr<DockItem> item;
    item.reset (new DockItem (*this, itemId, itemName));
    
    auto& root = *rootAreas [itemPlacement];

    return item.release();
}

DockItem* Dock::createItem()
{
    return createItem (Uuid().toString(), "Dock Item", Dock::TopPlacement);
}

}
