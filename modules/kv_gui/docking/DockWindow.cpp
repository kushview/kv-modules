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
    
DockWindow::DockWindow (Dock& dock)
    : DocumentWindow ("DockWindow", Colours::black, DocumentWindow::closeButton, false)
{
    setUsingNativeTitleBar (true);
    setAlwaysOnTop (true);

    container.reset (new DockContainer (dock));
    setContentNonOwned (container.get(), true);
    setSize (600, 400);
}

DockWindow::~DockWindow()
{
    clearContentComponent();
    container.reset (nullptr);
}

bool DockWindow::dockItem (DockItem* const item, DockPlacement placement)
{
    if (! placement.isDirectional())
        return false;
    return container->dockItem (item, placement);
}

void DockWindow::closeButtonPressed()
{
    delete this;
}

int DockWindow::getDesktopWindowStyleFlags() const
{
    return DocumentWindow::getDesktopWindowStyleFlags();
}

}
