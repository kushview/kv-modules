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
class DockContainer;
class DockItem;
class DockItemTabs;
class DockLayout;
class DockPanel;

class Dock : public Component,
             public DragAndDropContainer
{
public:
    enum SplitType {
        NoSplit,
        SplitBefore,
        SplitAfter
    };

    Dock();
    virtual ~Dock();
    
    inline static SplitType getSplitType (const DockPlacement placement)
    {
        SplitType split = NoSplit;
        
        switch (placement)
        {
            case DockPlacement::Top:
            case DockPlacement::Left:
                split = SplitAfter;
                break;
            case DockPlacement::Bottom:
            case DockPlacement::Right:
                split = SplitBefore;
                break;
            default:
                break;
        }
        
        return split;
    }
    
    inline static String getSplitString (const int splitType)
    {
        switch (splitType)
        {
            case NoSplit:       return "No split"; break;
            case SplitBefore:   return "Split before"; break;
            case SplitAfter:    return "Split after"; break;
        }
        
        return "Unknown Split";
    }
    
    inline static String getDirectionString (const int placement)
    {
        const DockPlacement info (placement);
        return info.toString();
    }
    
    /** Create a default panel with a given name */
    DockItem* createItem (const String& panelName, DockPlacement placement);
    
    /** Start a drag operation on the passed in DockPanel */
    void startDragging (DockPanel* const panel);
    
    /** @internal */
    inline virtual void paint (Graphics& g) override
    {
        g.fillAll (findColour(DocumentWindow::backgroundColourId).darker());
    }
    
    /** @internal */
    void resized() override;
    /** @internal */
    void mouseMove (const MouseEvent& ev) override { }
    /** @internal */
    void dragOperationStarted (const DragAndDropTarget::SourceDetails& details) override;
    /** @internal */
    void dragOperationEnded (const DragAndDropTarget::SourceDetails& details) override;

protected:
    virtual DockPanel* getOrCreatePanel (const String&);

private:
    friend class DockItem;
    std::unique_ptr<DockContainer> container;
    
    void removeEmptyRootAreas();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Dock)
};

class JUCE_API DockArea : public Component
{
public:
    ~DockArea();
    
    int indexOf (DockItem* const item) const { return layout.indexOf ((Component*) item); }
    
    /** Returns the number of items in the layout */
    int getNumItems() const { return layout.getNumItems(); }
    
    /** Append a DockArea to the end of the layout */
    void append (DockArea* const area);
    
    /** Append a DockItem to the end of the layout */
    void append (DockItem* const item);
    
    /** Insert a DockItem at a specific location */
    void insert (int index, DockItem* const item, Dock::SplitType split = Dock::NoSplit);
    
    /** Insert a DockArea at a specific location */
    void insert (int index, DockArea* const area, Dock::SplitType split = Dock::NoSplit);
    
    void detachItem (DockItem* item);
    void remove (DockArea* const area);
    void setVertical (const bool vertical);
    bool isVertical() const { return layout.isVertical(); }
    
    /** @internal */
    inline virtual void paint (Graphics&) override { }
    /** @internal */
    void resized() override;

private:
    friend class Dock;
    friend class DockContainer;
    friend class DockItem;
    friend class DockPanel;
    
    explicit DockArea (const bool vertical = false);
    DockArea (DockPlacement placement);
    
    void disposeEmptyLayouts();
    
    DockLayout layout;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DockArea)
};

class JUCE_API DockItem : public Component,
                          public DragAndDropTarget
{
public:
    virtual ~DockItem();
    
    /** Dock all panels in this item to the target item */
    void dockTo (DockItem* target, DockPlacement placement);
    
    /** Returns the dock */
    Dock* getDock() const { return const_cast<Dock*> (&dock); }
    
    /** Returns the DockArea which contains this item */
    DockArea* getParentArea() const { return dynamic_cast<DockArea*> (getParentComponent()); }
    
    /** Returns the number of panels in this item's container */
    int getNumPanels() const { return panels.size(); }
    
    /** Returns the current panel index */
    int getCurrentPanelIndex() const;

    /** Returns the current panel object */
    DockPanel* getCurrentPanel() const;
    
    /** @internal */
    void paint (Graphics&) override;
    /** @internal */
    void resized() override;
    /** @internal */
    void mouseDown (const MouseEvent&) override;
    /** @internal */
    bool isInterestedInDragSource (const SourceDetails&) override;
    /** @internal */
    void itemDropped (const SourceDetails&) override;
    /** @internal */
    void itemDragEnter (const SourceDetails&) override;
    /** @internal */
    void itemDragMove (const SourceDetails&) override;
    /** @internal */
    void itemDragExit (const SourceDetails&) override;
    /** @internal */
    bool shouldDrawDragImageWhenOver() override;
    
private:
    friend class Dock;
    friend class DockArea;
    friend class DockPanel;
    
    DockItem (Dock& parent, DockPanel* panel);
    DockItem (Dock& parent, const String& slug, const String& name);
    
    Dock& dock;
    bool dragging = false;
    std::unique_ptr<DockItemTabs> tabs;
    OwnedArray<DockPanel> panels;
    
    void detach (DockPanel* const panel);
    void detach();
    
    void movePanelsTo (DockItem* const target);

    void refreshPanelContainer (DockPanel* const panelToSelect = nullptr);
    
    class DragOverlay;
    std::unique_ptr<DragOverlay> overlay;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DockItem)
};

}
