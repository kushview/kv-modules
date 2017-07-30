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

#ifndef EL_SCREEN_DISPLAY_H
#define EL_SCREEN_DISPLAY_H

class Screen;
class ScreenManager;

class ScreenDisplay  : public Component,
                       public Timer,
                       public ComboBoxListener,
                       public LabelListener
{
public:
    ScreenDisplay ();
    ~ScreenDisplay();

    void setTitle (const String& titleText);
    void setScreen (const int screenId);

    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    void labelTextChanged (Label* labelThatHasChanged);
    void mouseDown (const MouseEvent& e);
    void mouseDoubleClick (const MouseEvent& e);

private:
    class Dispatch;
    friend class Dispatch;
    ScopedPointer<Dispatch> dispatch;

    void timerCallback();
    void runDispatchLoop();

    ScopedPointer<ScreenManager> screens;
    Screen* currentScreen;
    friend class ContentComponent;

    ScopedPointer<ComboBox> comboBox;
    ScopedPointer<Label> title;
    ScopedPointer<Label> noteLabel;
    ScopedPointer<Component> screen;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScreenDisplay)
};

#endif   // EL_SCREEN_DISPLAY_H
