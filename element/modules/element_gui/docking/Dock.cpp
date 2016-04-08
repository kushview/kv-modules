/*
    This file is part of the element modules for the JUCE Library
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


class Dock::MiddleLayout : public Component
{
public:
    MiddleLayout (Dock& owner) : dock (owner), bar (&layout, 1, false)
    {
        addAndMakeVisible ((Component*)&owner.getTopArea());
        comps.add (&owner.getTopArea());
        layout.setItemLayout (0, 1, -1.0, 200);

        addAndMakeVisible (&bar);
        comps.add (&bar);
        layout.setItemLayout (1, 4, 4, 4);

        addAndMakeVisible ((Component*)&owner.getBottomArea());
        comps.add (&owner.getBottomArea());
        layout.setItemLayout (2, 1, -1.0, 200);
    }

    void resized()
    {
        const int numComponents = (dock.getBottomArea().isVisible()) ? 3 : 1;
        layout.layOutComponents ((Component**) &comps.getReference(0), numComponents,
                                 0, 0, getWidth(), getHeight(), true, true);
    }

private:
    Dock& dock;
    Array<void*> comps;
    StretchableLayoutManager    layout;
    StretchableLayoutResizerBar bar;

};

void
Dock::detatchAll (DockItem* item)
{

    for (int i = 0; i < areas.size(); ++i)
        { areas[i]->detachItem (item);  }

}

Dock::Dock ()
    : leftbar (&areaLayout, 1, true),
      rightbar (&areaLayout, 3, true)
{
    maximizedItem = nullptr;

    for (int i = 0; i <= Dock::RightArea; ++i)
    {
        areas.add (new DockArea ((Dock::Placement) i));
    }

    middleLayout = new MiddleLayout (*this);

    addAndMakeVisible (&getLeftArea());
    addAndMakeVisible (&leftbar);
    addAndMakeVisible (middleLayout);
    addAndMakeVisible (&rightbar);
    addAndMakeVisible (&getRightArea());

    // this goes left to right, with the middle being a vertical split
    areaLayout.setItemLayout (0, 10, -1.0, 200); // leftside
    areaLayout.setItemLayout (1, 4, 4, 4);      // bar
    areaLayout.setItemLayout (2, 10, -1.0, 600); // middle
    areaLayout.setItemLayout (3, 4, 4, 4);      // bar
    areaLayout.setItemLayout (4, 10, -1.0, 200); // right

}

Dock::~Dock ()
{
    middleLayout = nullptr;
}

DockItem* Dock::getItem (const String& id)
{
    for (int i = items.size(); --i >= 0; --i)
        if (items[i] && items[i]->getComponentID() == id)
            return items[i];

    return nullptr;
}

void Dock::resized()
{
    if (maximizedItem)
    {
        maximizedItem->getDockArea()->setBounds (getLocalBounds());
    }
    else
    {
        Component* comps[] = {  &getLeftArea(),
                                &leftbar,
                                middleLayout,
                                &rightbar,
                                &getRightArea()
                              };

        areaLayout.layOutComponents (
            comps + 2, 1, 0, 0, getWidth(), getHeight(), false, true
        );
    }
}

DockItem* Dock::createItem (const String& id, const String& name,
                            Dock::Placement placement)
{
    for (int i = 0; i < items.size(); ++i)
        if (items[i]->getComponentID() == id)
            return items[i];

    items.add (new DockItem (*this, id, name));
    DockItem* item (items.getLast());
    DockArea* area (areas [placement]);

    switch (placement)
    {
        case Dock::TopArea:
        case Dock::BottomArea:
        case Dock::LeftArea:
        case Dock::RightArea:
            area->append (item);
            break;
        case Dock::Floating: break;
        case Dock::Center: break;

        default: break;
    }

    return item;
}
