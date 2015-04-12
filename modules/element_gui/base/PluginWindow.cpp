/*
    PluginWindow.cpp - This file is part of Element
    Copyright (C) 2014  Kushview, LLC.  All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

static Array <PluginWindow*> activePluginWindows;

class PluginWindowToolbar : public Toolbar
{
public:
    PluginWindowToolbar() { }
    ~PluginWindowToolbar() { }
    
};

class PluginWindowContent : public Component
{
public:
    PluginWindowContent (Component* const _editor)
        : editor (_editor)
    {
        addAndMakeVisible (toolbar = new PluginWindowToolbar());
        addAndMakeVisible (editor);
        setSize (editor->getWidth(), editor->getHeight());
        toolbar->setBounds (0, 0, getWidth(), 48);
        resized();
    }
    
    ~PluginWindowContent() { }
    
    void resized()
    {
        Rectangle<int> r (getLocalBounds());
        
        if (toolbar->getThickness())
        {
            toolbar->setBounds (r.removeFromTop (toolbar->getThickness()));
        }
        
        editor->setBounds (r);
    }
    
    Toolbar* getToolbar() const { return toolbar.get(); }
    
private:
    ScopedPointer<PluginWindowToolbar> toolbar;
    ScopedPointer<Component> editor, leftPanel, rightPanel;
};

PluginWindow::PluginWindow (Component* const ui, GraphProcessor::Node* node)
    : DocumentWindow (ui->getName(), Colours::lightgrey,
                      DocumentWindow::minimiseButton | DocumentWindow::closeButton),
      owner (node)
{
#if 0
    setContentOwned (new PluginWindowContent (ui), true);
#else
    setContentOwned (ui, true);
#endif
    
    setTopLeftPosition (owner->properties.getWithDefault ("uiLastX", Random::getSystemRandom().nextInt (500)),
                        owner->properties.getWithDefault ("uiLastY", Random::getSystemRandom().nextInt (500)));
    setVisible (true);
    setResizable(true, false);
    activePluginWindows.add (this);
}

void PluginWindow::closeCurrentlyOpenWindowsFor (const uint32 nodeId)
{
    for (int i = activePluginWindows.size(); --i >= 0;)
        if (activePluginWindows.getUnchecked(i)->owner->nodeId == nodeId)
            delete activePluginWindows.getUnchecked(i);
}

void PluginWindow::closeAllCurrentlyOpenWindows()
{
    if (activePluginWindows.size() > 0)
    {
        for (int i = activePluginWindows.size(); --i >= 0;)
            delete activePluginWindows.getUnchecked(i);

        Component dummyModalComp;
        dummyModalComp.enterModalState();
        MessageManager::getInstance()->runDispatchLoopUntil (50);
    }
}

Toolbar* PluginWindow::getToolbar() const
{
    if (PluginWindowContent* pwc = dynamic_cast<PluginWindowContent*> (getContentComponent()))
        return pwc->getToolbar();
    return nullptr;
}

void PluginWindow::resized()
{
    DocumentWindow::resized();
}

PluginWindow* PluginWindow::getWindowFor (GraphProcessor::Node* node)
{
    for (int i = activePluginWindows.size(); --i >= 0;)
        if (activePluginWindows.getUnchecked(i)->owner == node)
            return activePluginWindows.getUnchecked(i);

    return nullptr;
}

PluginWindow* PluginWindow::createWindowFor (GraphNode* node, Component* ed)
{
    return new PluginWindow (ed, node);
}

PluginWindow::~PluginWindow()
{
    activePluginWindows.removeFirstMatchingValue (this);
    clearContentComponent();
}

void PluginWindow::moved()
{
    owner->properties.set ("uiLastX", getX());
    owner->properties.set ("uiLastY", getY());
}

void PluginWindow::closeButtonPressed()
{
    delete this;
}
