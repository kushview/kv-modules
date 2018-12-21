namespace kv {
    
void DockPanel::dockTo (DockItem* const target, Dock::Placement placement)
{
    if (placement == Dock::FloatingPlacement)
        return;
    
    auto* source = findParentComponentOfClass<DockItem>();
    
    if (placement == Dock::CenterPlacement)
    {
        if (source != nullptr)
        {
            source->detach (this);
        }
        
        target->panels.add (this);
        target->buildTabs();
        target->dock.resized();
        return;
    }
    
    DBG("Docking Panel: " << getName() << " to " << Dock::getDirectionString(placement)
        << " of Item: " << target->getName());
    
    auto* const targetParent = target->getParentArea();
    const bool wantsVerticalPlacement = placement == Dock::TopPlacement || placement == Dock::BottomPlacement;
    
    if (targetParent != nullptr && wantsVerticalPlacement == targetParent->isVertical())
    {
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
            DBG("[A] index: " << insertIdx);
            insertIdx += offsetIdx;
            DBG("[B] index: " << insertIdx);
            targetParent->insert (insertIdx, source);
        }
        else if (source->getNumPanels() > 1)
        {
            source->detach (this);
            int insertIdx = targetParent->indexOf (target);
            DBG("[A] index: " << insertIdx);
            insertIdx += offsetIdx;
            DBG("[B] index: " << insertIdx);
            targetParent->insert (insertIdx, new DockItem (source->dock, this));
        }
    }
    else
    {
        DBG("opposite direction as parent area");
    }
}
    
}
