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
class DockPanel;

class DockFactory
{
public:
    virtual ~DockFactory() { }
    virtual DockPanel* createDockPanel (int typeId) = 0;

protected:
    DockFactory() { }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DockFactory)
};

class Dock : public Component,
             public DragAndDropContainer
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
    DockItem* createItem (const String& itemId, const String& itemName, Dock::Placement placement);
    DockItem* getItem (const String& itemId);

    /** @internal */
    void paint (Graphics&) override { }
    /** @internal */
    void resized() override;
    /** @internal */
    void mouseMove (const MouseEvent& ev) override { }
    /** @internal */
    void dragOperationStarted (const DragAndDropTarget::SourceDetails& details) override;
    /** @internal */
    void dragOperationEnded (const DragAndDropTarget::SourceDetails& details) override;
    
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
    bool isVertical() const { return layout.getIsVertical(); }
    
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
    virtual ~DockItem();
    
    void dockTo (DockItem* target, Dock::Placement placement);

    DockArea* getParentArea() const { return dynamic_cast<DockArea*> (getParentComponent()); }
    
    int getNumPanels() const { return panels.size(); }
    int getCurrentPanelIndex() const { return tabs.getCurrentTabIndex(); }
    DockPanel* getCurrentPanel() const;
    
    int getNumItems() const;
    
    /** @internal */
    void paint (Graphics& g) override;
    /** @internal */
    void resized() override;
    /** @internal */
    void mouseDown (const MouseEvent& ev) override;
    /** @internal */
    bool isInterestedInDragSource (const SourceDetails& details) override;
    /** @internal */
    void itemDropped (const SourceDetails& dragSourceDetails) override;
    /** @internal */
    void itemDragEnter (const SourceDetails&) override;
    /** @internal */
    void itemDragExit (const SourceDetails&) override;
    /** @internal */
    bool shouldDrawDragImageWhenOver() override;
    
private:
    friend class DockArea;
    friend class DockPanel;
    friend class Dock;
    
    DockItem (Dock& parent, DockPanel* panel);
    DockItem (Dock& parent, const String& slug, const String& name);
    
    Dock& dock;
    bool dragging = false;
    TabbedComponent tabs;
    DockArea area;
    OwnedArray<DockPanel> panels;
    
    void detach (DockPanel* const panel)
    {
        panels.removeObject (panel, false);
        if (panels.size() == 0)
            detach();
        else
            buildTabs();
    }
    
    void detach()
    {
        if (auto* const area = getParentArea())
        {
            area->detachItem (this);
            area->resized();
        }
    }
    
    void buildTabs();
    
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DockItem)
};

}
