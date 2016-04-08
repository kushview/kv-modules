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

#ifndef ELEMENT_NATIVE_H
#define ELEMENT_NATIVE_H


class NativeComponent : public Component
{
public:

    enum HandleType
    {
        carbonHandle,
        nsviewHandle,
        windowsHandle,
        x11Handle,
        unknownHandle,
#if   JUCE_LINUX
        defaultHandle = x11Handle
#elif JUCE_MAC
        defaultHandle = nsviewHandle
#elif JUCE_WINDOWS
        defaultHandle = windowsHandle
#else
        defaultHandle = unknownHandle
#endif
    };

    typedef ReferenceCountedObject Attachment;
    typedef ReferenceCountedObjectPtr<Attachment> AttachmentPtr;


    NativeComponent (HandleType = defaultHandle);
    ~NativeComponent();

    void setHandle (void* handle);

    void* getHandle();

    void resizeToFit();

    void showWidget();

    void paint (Graphics& g);

    void resized();

    void mouseUp    (const MouseEvent& e);
    void mouseDown  (const MouseEvent& e);
    void mouseEnter (const MouseEvent& e);
    void mouseExit  (const MouseEvent& e);
    void mouseMove  (const MouseEvent& e);
    void mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& d);
    void mouseDrag  (const MouseEvent& e);

    /** @internal */
    static Attachment* attachViewToComponent (Component&, void*, HandleType);

private:
    HandleType    handleType;
    AttachmentPtr attachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NativeComponent)

};

#endif /* ELEMENT_NATIVE_H */
