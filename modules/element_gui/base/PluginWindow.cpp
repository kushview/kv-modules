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



    class PluginWindow;
    static Array <PluginWindow*> activePluginWindows;

    PluginWindow::PluginWindow (Component* const ui, GraphProcessor::Node* node, const bool generic)
        : DocumentWindow (ui->getName(), Colours::lightgrey,
                          DocumentWindow::minimiseButton | DocumentWindow::closeButton),
          owner (node), isGeneric (generic)
    {
        setContentOwned (ui, true);
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

    void PluginWindow::resized()
    {
        DocumentWindow::resized();
    }

    PluginWindow* PluginWindow::getWindowFor (GraphProcessor::Node* node,
                                              bool useGenericView)
    {
        for (int i = activePluginWindows.size(); --i >= 0;)
            if (activePluginWindows.getUnchecked(i)->owner == node
                 && activePluginWindows.getUnchecked(i)->isGeneric == useGenericView)
                return activePluginWindows.getUnchecked(i);

        AudioProcessorEditor* ui = nullptr;

        if (! useGenericView)
        {
            ui = node->audioProcessor()->createEditorIfNeeded();

            if (ui == nullptr)
                useGenericView = true;
        }

        if (useGenericView)
            ui = new GenericAudioProcessorEditor (node->audioProcessor());

        if (ui != nullptr)
        {
            if (AudioPluginInstance* const plugin = dynamic_cast <AudioPluginInstance*> (node->audioProcessor()))
                ui->setName (plugin->getName());

            return new PluginWindow (ui, node, useGenericView);
        }

        return nullptr;
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

