
#include "element/gui/dock.hpp"

namespace Element {
namespace gui {

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

        void
        resized()
        {
            layout.layOutComponents ((Component**) &comps.getReference(0), 3, 0, 0, getWidth(), getHeight(), true, true);
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

    DockItem*
    Dock::getItem (const String& id)
    {
        for (int i = items.size(); --i >= 0; --i)
            if (items[i] && items[i]->getComponentID() == id)
                return items[i];

        return nullptr;
    }

    void
    Dock::resized()
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

    DockItem*
    Dock::createItem (const String& id, const String& name, Dock::Placement placement)
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

}}


