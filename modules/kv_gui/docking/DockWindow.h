
#pragma once

namespace kv {

class DockContainer;

class DockWindow : public DocumentWindow
{
public:
    virtual ~DockWindow();
    
    bool dockItem (DockItem* const item, Dock::Placement placement);

    /** @internal */
    void closeButtonPressed() override;
    /** @internal */
    int getDesktopWindowStyleFlags() const override;
protected:
    DockWindow();
    
private:
    friend class Dock;
    std::unique_ptr<DockContainer> container;
};

}
