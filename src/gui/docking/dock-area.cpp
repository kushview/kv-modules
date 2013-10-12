

#include "element/gui/dock.hpp"

namespace Element {
namespace gui {


    DockArea::DockArea ()
    {
        layouts.add (new DockLayout (*this, false));
    }

    DockArea::~DockArea ()
    {
    }


    void DockArea::append (DockItem* const item)
    {
        if (DockLayout* layout = layouts.getLast())
        {
            layout->append (item);
            addAndMakeVisible (item);
            resized();
        }
    }

    void DockArea::detachItem (DockItem* item)
    {
        removeChildComponent (item);
        layouts[0]->remove (item);
    }

    DockArea::DockArea (Dock::Placement placement)
    {
        switch (placement)
        {
        case Dock::LeftArea:
        case Dock::RightArea:
            layouts.add (new DockLayout (*this, true));
            break;
        case Dock::TopArea:
        case Dock::BottomArea:
            layouts.add (new DockLayout (*this, false));
            break;
        default:
            layouts.add (new DockLayout (*this, false));
            break;
        }
    }

    void DockArea::resized ()
    {
        layouts[0]->layoutItems (0, 0, getWidth(), getHeight());
    }

}}
