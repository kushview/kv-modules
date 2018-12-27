
#pragma once

namespace kv {

class DockItem;

class DockContainer : public Component
{
public:
    DockContainer();
    ~DockContainer();

    bool dockItem (DockItem* const item, Dock::Placement placement);

    /** @internal */
    void resized() override;
    /** @internal */
    void paint (Graphics&) override;

private:
    std::unique_ptr<DockArea> root;
};

}
