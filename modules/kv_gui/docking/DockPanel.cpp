namespace kv {
    
void DockPanel::dockTo (DockItem* const target, Dock::Placement placement)
{
    if (placement == Dock::FloatingPlacement)
        return;
    
    auto* source = findParentComponentOfClass<DockItem>();
    
    if (placement == Dock::CenterPlacement)
    {
        if (source != nullptr)
            source->detach (this);
        
        target->panels.add (this);
        target->refreshPanelContainer();
        target->dock.resized();
        return;
    }
    
    DBG("Docking Panel: " << getName() << " to " << Dock::getDirectionString(placement)
        << " of Item: " << target->getName());
    
    auto* const targetParent = target->getParentArea();
    const bool wantsVerticalPlacement = placement == Dock::TopPlacement || placement == Dock::BottomPlacement;
    
    if (targetParent != nullptr && wantsVerticalPlacement == targetParent->isVertical())
    {
        // same direction as target parent area
        int offsetIdx = 0;
     
        if (wantsVerticalPlacement)
        {
            if (placement == Dock::BottomPlacement)
                ++offsetIdx;
        }
        else
        {
            if (placement == Dock::RightPlacement)
                ++offsetIdx;
        }
        
        if (source->getNumPanels() == 1)
        {
            source->detach();
            int insertIdx = targetParent->indexOf (target);
            insertIdx += offsetIdx;
            targetParent->insert (insertIdx, source);
        }
        else if (source->getNumPanels() > 1)
        {
            source->detach (this);
            int insertIdx = targetParent->indexOf (target);
            insertIdx += offsetIdx;
            targetParent->insert (insertIdx, new DockItem (source->dock, this));
        }
    }
    else if (targetParent != nullptr && target->getNumItems() <= 0)
    {
        // opposite direction as target parent area
        auto& area = target->area;
        area.setVisible (true);
        area.setVertical (! targetParent->isVertical());
        
        auto* item0 = new DockItem (target->dock, "Temp Panel", "Temp Panel");
        item0->tabs.clearTabs();
        item0->panels.clear();
        target->movePanelsTo (item0);
        area.append (item0);

        source->detach (this);
        area.append (new DockItem (target->dock, this));
        target->resized();
        target->repaint();
    }
    else
    {
        // unhandled docking condition
        jassertfalse;
    }
}

}
