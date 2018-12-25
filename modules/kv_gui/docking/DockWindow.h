
#pragma once

namespace kv {
    
class DockWindow : public DocumentWindow
{
public:
    virtual ~DockWindow();
    
protected:
    DockWindow();
    
private:
    friend class Dock;
};

}
