/*
    DockArea.cpp - This file is part of Element
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

DockArea::DockArea (const bool vertical)
    : layout (*this, vertical)
{ }

DockArea::DockArea (Dock::Placement placement)
    : layout (*this)
{
    switch (placement)
    {
        case Dock::TopPlacement:
        case Dock::BottomPlacement:
            layout.setVertical (false);
            break;
        case Dock::LeftPlacement:
        case Dock::RightPlacement:
        default:
            layout.setVertical (true);
            break;
    }
}

DockArea::~DockArea ()
{
}

void DockArea::append (DockItem* const item)
{
    insert (-1, item);
}

void DockArea::insert (int index, DockItem* const item, Dock::SplitType split)
{
    layout.insert (index, item, split);
    addAndMakeVisible (item);
    item->repaint();
    resized();
}

void DockArea::detachItem (DockItem* item)
{
    removeChildComponent (item);
    layout.remove (item);
}

void DockArea::setVertical (const bool vertical)
{
    layout.setVertical (vertical);
}

void DockArea::resized()
{
    layout.layoutItems (0, 0, getWidth(), getHeight());
}

}
