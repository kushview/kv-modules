
namespace kv {

DockContainer::DockContainer()
{
    root.reset (new DockArea (true));
    addAndMakeVisible (root.get());
}

DockContainer::~DockContainer()
{
    root = nullptr;
}

bool DockContainer::dockItem (DockItem* const item, Dock::Placement placement)
{
    if (! Dock::isDirectional (placement))
        return false;
    
    bool result = true;
    const int insertIdx = placement == Dock::TopPlacement || placement == Dock::LeftPlacement ? 0 : -1;
    const auto split = insertIdx < 0 ? Dock::SplitBefore : Dock::SplitAfter;
    
    if (root->isVertical() == Dock::isVertical (placement))
    {
        root->insert (insertIdx, item, split);
    }
    else
    {
        std::unique_ptr<DockArea> oldRoot;
        oldRoot.reset (root.release());
        removeChildComponent (oldRoot.get());
        root.reset (new DockArea (! oldRoot->isVertical()));
        addAndMakeVisible (root.get());
        root->append (oldRoot.release());
        root->insert (insertIdx, item, split);
    }

    resized();

    return result;
}

void DockContainer::paint (Graphics& g)
{
    
}

void DockContainer::resized()
{
    root->setBounds (getLocalBounds());
}

}
