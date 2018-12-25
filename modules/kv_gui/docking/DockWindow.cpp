
namespace kv {
    
DockWindow::DockWindow()
    : DocumentWindow ("DockWindow", Colours::black, DocumentWindow::closeButton, false)
{
    setSize (600, 400);
    setUsingNativeTitleBar (true);
}

DockWindow::~DockWindow()
{
}

}
