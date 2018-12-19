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
      layout (*this, false), dragging (false), grip (*this)
{
    setComponentID (id);
    addAndMakeVisible (grip);
    addChildComponent (overlay);
    overlay.setAlpha (0.50);
}

DockItem::~DockItem()
{
}

void DockItem::append (const String& itemID)
{
    if (DockItem* child = dock.getItem (itemID))
    {
       layout.append (child);
    }
}

void DockItem::dockTo (DockItem* target, Dock::Placement placement)
{
    if (placement == Dock::FloatingPlacement)
        return;

    if (DockArea* area = target->getDockArea())
    {
        detach();
        area->append (this);
    }
}


void DockItem::paint (Graphics& g)
{
    g.fillAll (Colour (0xff595959));
}

void DockItem::layoutItems()
{
    Rectangle<int> b (getBoundsInParent());
    //area.layoutItems (b.getX(), b.getY(), b.getWidth(), b.getHeight());
}


void DockItem::mouseDown (const MouseEvent& ev)
{
    Component::mouseDown(ev);
}

}
