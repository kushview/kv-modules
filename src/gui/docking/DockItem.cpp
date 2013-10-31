/*
 * dock-item.cpp
 *
 *  Created on: Jan 29, 2013
 *      Author: mfisher
 */

#include "element/gui/Dock.h"


namespace Element {
namespace Gui {


DockItem::DockItem (Dock& parent, const String& id, const String& name)
    : Component (name), dock (parent), 
      layout (*this, false), dragging (false), grip (*this)
{
    setComponentID (id);
    addAndMakeVisible (&grip);
    addChildComponent(&overlay);
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
    if (placement == Dock::Floating)
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
}
