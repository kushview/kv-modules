
#pragma once

namespace kv {

/** Manages panel layouts in DockAreas and the root Dock objects */
class DockLayout
{
public:
    DockLayout (Component& holder_, bool vertical = false);
    
    ~DockLayout() noexcept;
    
    /** Returns the index of the child component */
    inline int indexOf (Component* const child) const { return items.indexOf (child); }
    
    /** Returns the number of items in the layout */
    inline int getNumItems() const { return items.size(); }
    
    void insert (int index, Component* const child);
    
    void append (Component* child);
    
    void remove (Component* const child);
    
    void clear();
    
    void layoutItems (int x, int y, int w, int h);
    
    void layoutItems();
    
    inline void setVertical (bool isNowVertical) { vertical = isNowVertical; }
    
    inline bool isVertical() const { return vertical; }
    
    Component* root();
    
private:
    void buildComponentArray();
    bool vertical;
    Component& holder;
    OwnedArray<StretchableLayoutResizerBar> bars;
    Array<Component*> items;
    Array<Component*> comps;
    StretchableLayoutManager layout;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DockLayout)
};

}
