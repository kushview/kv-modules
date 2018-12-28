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

DockContainer::DockContainer()
{
    root.reset (new DockArea (true));
    addAndMakeVisible (root.get());
}

DockContainer::~DockContainer()
{
    root = nullptr;
}

DockArea& DockContainer::getRootArea() { jassert (root != nullptr); return *root; }

bool DockContainer::dockItem (DockItem* const item, DockPlacement placement)
{
    if (! placement.isDirectional())
        return false;
    
    bool result = true;
    const int insertIdx = placement == DockPlacement::Top || placement == DockPlacement::Left ? 0 : -1;
    const auto split = insertIdx < 0 ? Dock::SplitBefore : Dock::SplitAfter;
    
    if (root->isVertical() == placement.isVertical())
    {
        root->insert (insertIdx, item, split);
    }
    else
    {
        std::unique_ptr<DockArea> oldRoot;
        oldRoot.reset (root.release());
        removeChildComponent (oldRoot.get());
        root.reset (new DockArea (! oldRoot->isVertical()));
        addAndMakeVisible (root.get());
        root->append (oldRoot.release());
        root->insert (insertIdx, item, split);
    }

    resized();

    return result;
}

void DockContainer::paint (Graphics& g)
{
    
}

void DockContainer::resized()
{
    root->setBounds (getLocalBounds());
}

}
