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

#ifndef EL_SCREEN_H
#define EL_SCREEN_H

class Screen :  public Component
{
public:
    virtual ~Screen();

    const int type() const { return id; }
    static Screen* create (ScreenDisplay& disp, const int id);

    inline int getNumPages() const { return pages.getNumTabs(); }
    inline void addPage (const String& name, Component* page)
    {
        pages.addTab (name, Colours::transparentBlack, page, true);
    }

    /** Find a page by class type */
    template<class T>
    inline T* findPage()
    {
        for (int i = pages.getNumTabs(); --i >= 0;)
            if (T* p = dynamic_cast<T*> (pages.getTabContentComponent(i)))
                return p;
        return nullptr;
    }

    virtual void onDisplayUpdate() { }

    virtual void resized() {
        pages.setBounds (getLocalBounds());
    }

    ScreenDisplay& getDisplay();

protected:
    Screen (ScreenDisplay& disp, const String& name, const int id);

private:
    ScreenDisplay& owner;
    TabbedComponent pages;
    ValueTree props;
    int id;

    friend class ScreenDisplay;
};

template <class CompType>
class ScreenPage :  public CompType
{
public:
    ScreenPage() { }
    virtual ~ScreenPage() { }
};

#endif  /* EL_SCREEN_H */
