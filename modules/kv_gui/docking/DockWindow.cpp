
namespace kv {
    
DockWindow::DockWindow()
    : DocumentWindow ("DockWindow", Colours::black, DocumentWindow::closeButton, false)
{
    setUsingNativeTitleBar (true);
    setAlwaysOnTop (true);

    container.reset (new DockContainer());
    setContentNonOwned (container.get(), true);
    setSize (600, 400);
}

DockWindow::~DockWindow()
{
    clearContentComponent();
    container.reset (nullptr);
}

bool DockWindow::dockItem (DockItem* const item, DockPlacement placement)
{
    if (! placement.isDirectional())
        return false;
    return container->dockItem (item, placement);
}

void DockWindow::closeButtonPressed()
{
    delete this;
}

int DockWindow::getDesktopWindowStyleFlags() const
{
    return DocumentWindow::getDesktopWindowStyleFlags();
}

}
