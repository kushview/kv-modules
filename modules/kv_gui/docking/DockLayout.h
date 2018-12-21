
#pragma once

namespace kv {

/** Manages panel layouts in DockAreas and the root Dock objects */
class DockLayout
{
public:
    DockLayout (Component& holder_, bool vertical = false);
    ~DockLayout ();
    
    /** Returns the index of the child component */
    inline int indexOf (Component* const child) const { return items.indexOf (child); }
    void clear();
    
    void append (Component* child);
    void insert (int index, Component* const child);
    void remove (Component* const child);
    
    void layoutItems (int x, int y, int w, int h);
    void layoutItems();
    void setIsVertical (bool vertical) { isVertical = vertical; }
    bool getIsVertical() const { return isVertical; }
    Component* root();
    
private:
    void buildComponentArray();
    bool isVertical;
    Component& holder;
    OwnedArray<StretchableLayoutResizerBar> bars;
    Array<Component*> items;
    Array<Component*> comps;
    StretchableLayoutManager layout;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DockLayout)
};

}
