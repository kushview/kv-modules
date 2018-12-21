#pragma once

namespace kv {

class DockItem;

class DockPanel : public Component
{
public:
    DockPanel() { }
    virtual ~DockPanel() { }
    
    void dockTo (DockItem* const target, Dock::Placement placement);
    
    inline virtual void paint (Graphics& g) override
    {
        g.setColour (Colours::white);
        g.drawText (getName(), 0, 0, getWidth(), getHeight(), Justification::centred);
    }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DockPanel)
};

}
