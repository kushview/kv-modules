#pragma once

namespace kv {
    
class DockItemTabs : public TabbedComponent
{
public:
    DockItemTabs() = delete;
    DockItemTabs (TabbedButtonBar::Orientation orientation);
    virtual ~DockItemTabs();
    
protected:
    virtual TabBarButton* createTabButton (const String &tabName, int tabIndex) override;
};

}
