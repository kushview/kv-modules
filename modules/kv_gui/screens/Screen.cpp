/*
    This file is part of the element modules for the JUCE Library
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

ScreenDisplay& Screen::getDisplay() { return owner; }

Screen::Screen (ScreenDisplay& disp, const String& name, const int t)
    : owner (disp),
      pages (TabbedButtonBar::TabsAtLeft),
      props ("screen"), id (t)
{
    props.setProperty (Slugs::name, name, nullptr);
    props.setProperty (Slugs::type, (int32) type(), nullptr);

    addAndMakeVisible (&pages);
    pages.setOutline (0);
    pages.setIndent (6);
    pages.setTabBarDepth (18);
    pages.setAlwaysOnTop (true);

    pages.setOrientation (TabbedButtonBar::TabsAtBottom);
}

Screen::~Screen()
{
    removeChildComponent (&pages);
}

Screen* Screen::create (ScreenDisplay& disp, const int screenId)
{
    jassertfalse;
    return nullptr;
}
