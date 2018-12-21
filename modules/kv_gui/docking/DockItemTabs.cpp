
namespace kv {

DockItemTabs::DockItemTabs (TabbedButtonBar::Orientation orientation)
    : TabbedComponent (orientation) { }
DockItemTabs::~DockItemTabs() { }

TabBarButton* DockItemTabs::createTabButton (const String &tabName, int tabIndex)
{
    return new TabBarButton();
}

}
