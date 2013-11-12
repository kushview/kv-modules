/*
    ContentComponent.cpp - This file is part of Element
    Copyright (C) 2013  Michael Fisher <mfisher31@gmail.com>

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

#include <element/gui/MidiEditorBody.h>

#include "../models/Session.h"
#include "../EngineControl.h"
#include "../Globals.h"

#include "sequencer/SequencerComponent.h"

#include "Commands.h"
#include "ContentComponent.h"
#include "GraphEditorPanel.h"
#include "GuiApp.h"
#include "MainWindow.h"
#include "MainMenu.h"
#include "SessionTreePanel.h"
#include "Workspace.h"

namespace Element {
namespace Gui {


    ContentComponent::ContentComponent (GuiApp& app_)
        : gui(app_)
    {
        setOpaque (true);
        addAndMakeVisible (workspace = new Workspace());

        //workspace->setMainComponent (new Label (String ("Hello")));

        Dock& dock (workspace->getDock());
        DockItem* item = dock.createItem ("test2", "Test 2", Dock::BottomArea);
        item->setContentOwned (new MidiEditorBody (keyboard));

        workspace->setMainComponent (new SequencerComponent (gui));

        resized();
    }

    ContentComponent::~ContentComponent()
    {
        toolTips = nullptr;
    }

    void ContentComponent::childBoundsChanged (Component* child)
    {
    }

    void ContentComponent::paint (Graphics &g)
    {
        g.fillAll (Colours::darkgrey);
    }

    void ContentComponent::resized()
    {
        Rectangle<int> r (getLocalBounds());
        workspace->setBounds (r.reduced (2));
    }

    GuiApp& ContentComponent::app() { return gui; }
}}
