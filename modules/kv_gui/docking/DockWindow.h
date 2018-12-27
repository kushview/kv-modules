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

#pragma once

namespace kv {

class DockContainer;

/** A window that has a DockContainer as the main component. This
    is used for Floating DockingItem */
class DockWindow : public DocumentWindow
{
public:
    /** Destructor */
    virtual ~DockWindow();
    
    /** Dock the geven item to this window */
    bool dockItem (DockItem* const item, DockPlacement placement);

    /** @internal */
    void closeButtonPressed() override;
    /** @internal */
    int getDesktopWindowStyleFlags() const override;

protected:
    DockWindow();
    
private:
    friend class Dock;
    std::unique_ptr<DockContainer> container;
};

}
