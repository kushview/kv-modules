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

class DockArea;
class DockItem;
class DockLayout;

class DockItemFactory
{
public:
    virtual ~DockItemFactory() { }
    virtual Component* createDockItemContent (int typeId) = 0;

protected:
    DockItemFactory() { }
};

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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DockLayout)
};

class Dock : public Component
{
public:
    enum Placement
    {
        TopPlacement = 0,
        BottomPlacement,
        LeftPlacement,
        RightPlacement,
        CenterPlacement,
        FloatingPlacement,
        numPlacements
    };

    Dock();
    virtual ~Dock();
    
    inline static String getDirectionString (const int placement)
    {
        switch (placement)
        {
            case TopPlacement:      return "Top"; break;
            case BottomPlacement:   return "Bottom"; break;
            case LeftPlacement:     return "Left"; break;
            case RightPlacement:    return "Right"; break;
            case CenterPlacement:   return "Center"; break;
            case FloatingPlacement: return "Floating"; break;
        }
        return {};
    }
    
    DockItem* createItem();
    DockItem* createItem (const String& itemId, const String& itemName,
                          Dock::Placement placement);
    DockItem* getItem (const String& itemId);

    /** @internal */
    void paint (Graphics&) override { }
    /** @internal */
    void resized() override;

private:
    friend class DockItem;

    OwnedArray<DockArea> rootAreas [numPlacements];
    DockLayout verticalLayout;
    DockLayout horizontalLayout;
    
    DockItem* maximizedItem = nullptr;
    void detatchAll (DockItem* item);
    void removeEmptyRootAreas();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Dock)
};

class DockArea : public Component
{
public:
    explicit DockArea (const bool vertical = false);
    DockArea (Dock::Placement placement);

    ~DockArea();
    
    int indexOf (DockItem* const item) const { return layout.indexOf ((Component*) item); }
    
    /** Append a DockItem to the end of the layout */
    void append (DockItem* const item);
    
    /** Insert a DockItem at a specific location */
    void insert (int index, DockItem* const item);
    
    void detachItem (DockItem* item);
    void setVertical (const bool vertical);
    bool isVertical() const         { return layout.getIsVertical(); }
    
    /** @internal */
    void paint (Graphics&) override { }
    /** @internal */
    void resized() override;

private:
    void disposeEmptyLayouts();
    DockLayout layout;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DockArea)
};

class DockItem : public Component,
                 public DragAndDropTarget
{
public:
    DockItem (Dock& parent, const String& slug, const String& name);
    virtual ~DockItem();

    void append (const String& itemID);
    void dockTo (DockItem* target, Dock::Placement placement);

    DockArea* getParentArea() const { return dynamic_cast<DockArea*> (getParentComponent()); }

    void layoutItems();

    /** Component */
    void paint (Graphics& g) override;

    void setContentOwned (Component* component)
    {
        if (content == component)
            return;
        
        content = nullptr;
        content = component;
        addAndMakeVisible (content);
    }

    void resized() override
    {
        if (isMaximized())
        {
            if (content != nullptr)
                content->setBounds (getLocalBounds());
        }
        else
        {
            overlay.centreWithSize (getWidth() - 2, getHeight() - 2);

            if (content != nullptr)
                content->setBounds (36, 0, getWidth() - 36, getHeight());
        }
    }

    void mouseDown (const MouseEvent& ev) override;

    /* Drag and drop */

    bool isInterestedInDragSource (const SourceDetails& dragSourceDetails) override
    {
        if (dragSourceDetails.description.toString() == "dock-item")
            return true;
        return false;
    }

    void itemDropped (const SourceDetails& dragSourceDetails) override
    {
        overlay.setVisible (false);
        
        Component* comp = dragSourceDetails.sourceComponent.get();
        DockItem* const item = dynamic_cast<DockItem*> (comp);
        if (item == this || item == nullptr)
            return;
        
        const auto x = dragSourceDetails.localPosition.getX();
        const auto y = dragSourceDetails.localPosition.getY();
        Dock::Placement placement = Dock::CenterPlacement;
        
        if (x >= 0 && x < 30 && y >= 30 && y < getHeight() - 30)
            placement = Dock::LeftPlacement;
        else if (x >= getWidth() - 30 && x < getWidth() && y >= 30 && y < getHeight() - 30)
            placement = Dock::RightPlacement;
        if (y >= 0 && y < 30 && x >= 30 && x < getWidth() - 30)
            placement = Dock::TopPlacement;
        else if (y >= getHeight() - 30 && y < getHeight() && x >= 30 && x < getWidth() - 30)
            placement = Dock::BottomPlacement;
        
        item->dockTo (this, placement);
    }

    virtual void itemDragEnter (const SourceDetails&) override
    {
        overlay.toFront (false);
        overlay.setVisible (true);
    }

    virtual void itemDragExit (const SourceDetails&) override
    {
        overlay.setVisible (false);
    }

    bool isMaximized() const { return dock.maximizedItem == this; }

    void setMaximized (const bool shouldBeMaximized)
    {
        if (shouldBeMaximized == isMaximized())
            return;

        if (shouldBeMaximized) {
            dock.maximizedItem = this;
        } else {
            dock.maximizedItem = nullptr;
        }

        dock.resized();
    }

private:
    Dock&      dock;
    DockLayout layout;

    friend class DockArea;
    friend class Dock;

    bool dragging;

    ScopedPointer<Component> content;

    void detach()
    {
        if (DockArea* area = getParentArea())
        {
            area->detachItem (this);
            area->resized();
        }
    }

    class DragOverlay : public Component
    {
    public:
        DragOverlay() { }
        void paint (Graphics &g)
        {
            g.setOpacity (.50);
            g.fillAll (Colours::teal);

            g.setColour (Colours::black);
            g.drawRect (0, 0, getWidth(), getHeight(),2);
            g.drawRect (30, 30, getWidth() - 60, getHeight() - 60);
        }

    private:

    } overlay;

    class Grip  : public Component,
                  public DragAndDropContainer
    {
    public:
        Grip (Component& parent_) : parent(parent_)
        {
           #if JUCE_IOS
            setSize (36, 48);
           #else
            setSize (12, 16);
           #endif
        }

        void paint (Graphics& g)
        {
            g.setColour(Colours::white);

            const int pad = 1;
            const int size = 1;
            int x = 4, y = 0;

            for (int i = 0 ; i < getHeight() / (pad + size); ++i)
            {
                y = i * (pad + size);
                g.fillEllipse ((float)x, (float)y + 3.0f, (float)size, (float)size);
                g.fillEllipse ((float)x + 2.0f, (float)y + 3.0f, (float)size, (float)size);
            }
        }

        void mouseDown (const MouseEvent&)
        {
            parent.setAlpha (0.9);
            startDragging ("dock-item" ,&parent, Image(), true);
        }

        void mouseDrag (const MouseEvent&)
        {
        }

        void mouseUp (const MouseEvent&)
        {
            parent.setAlpha (1);
        }

    private:
        ComponentDragger dragger;
        Component& parent;

    } grip;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DockItem)
};

}
