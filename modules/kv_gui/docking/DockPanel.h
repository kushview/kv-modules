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

class DockItem;

struct DockPanelInfo
{
    String name;
    String description;
};

class DockPanel : public Component
{
public:
    /** Destructor */
    virtual ~DockPanel();

    /** Dock the panel to another Dock Item */
    void dockTo (DockItem* const target, DockPlacement placement);

    /** @internal */
    void paint (Graphics& g) override;
    /** @internal */
    void resized() override;

protected:
    friend class Dock;
    friend class DockItem;
    /** Constructor */
    DockPanel()
    {
        
    }
    
    std::unique_ptr<Component> content;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DockPanel)
};

class DockPanelType
{
public:
    virtual ~DockPanelType() { }
    
protected:
    DockPanelType() { }
};

}
