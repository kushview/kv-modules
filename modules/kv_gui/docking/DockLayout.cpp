/*
    DockLayout.cpp - This file is part of Element
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


DockLayout::DockLayout (Component& holder_, bool vertical)
    : isVertical (vertical), holder (holder_)
{ }

DockLayout::~DockLayout() { }

void
DockLayout::append (DockItem* item)
{
    if (! items.contains (item))
    {
        if (items.size() > 0)
        {
            int index = comps.size();
            bars.add (new StretchableLayoutResizerBar (&layout, index,
                                                       ! isVertical));
            comps.add (bars.getLast());
            holder.addAndMakeVisible (bars.getLast());
            layout.setItemLayout (index, 4, 4, 4);
        }

        layout.setItemLayout (comps.size(), 30, -1.0, 100);
        comps.add (item);
        items.add (item);
    }
}

void DockLayout::remove (DockItem* const child)
{
    bool wasRemoved = true;

    if (items.contains (child))
    {
        items.removeFirstMatchingValue (child);
        wasRemoved = true;
    }

    if (wasRemoved)
        buildComponentArray();
}

void DockLayout::layoutItems (int x, int y, int w, int h)
{
    if (comps.size() > 0)
    {
        layout.layOutComponents ((Component**) &comps.getReference(0), comps.size(), x, y, w, h, isVertical, true);
    }
}

void DockLayout::layoutItems()
{
    layoutItems (0, 0, holder.getWidth(), holder.getHeight());
}

DockItem* DockLayout::root()
{
    if (items.size() > 0) {
        return items [0];
    }

    return nullptr;
}

void DockLayout::buildComponentArray()
{
    bars.clear (true);
    layout.clearAllItems();
    comps.clear();

    for (int i = 0; i < items.size(); ++i)
    {
        layout.setItemLayout (comps.size(), 30, -1.0, 100);
        comps.add ((Component*) items [i]);

        if (i != items.size() -1)
        {
            int index = comps.size();
            bars.add (new StretchableLayoutResizerBar (&layout, index,
                                                       ! isVertical));
            comps.add (bars.getLast());
            holder.addAndMakeVisible (bars.getLast());
            layout.setItemLayout (index, 4, 4, 4);
        }
    }
}
