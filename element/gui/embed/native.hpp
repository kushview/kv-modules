/*
 native.hpp - Native Widget Embedding
 
 Copyright (C) 2013  BKE, LLC.  All rights reserved.
 Michael Fisher <mfisher@bketech.com>
 
 */

/** @file native.hpp */

#ifndef BKE_NATIVE_HPP
#define BKE_NATIVE_HPP

#include "element/juce.hpp"

namespace element {
    
    class NativeComponent : public Component
    {
    public:
        
        enum HandleType
        {
            carbonHandle,
            nsviewHandle,
            windowsHandle,
            x11Handle,
#if   JUCE_LINUX
            defaultHandle = x11Handle
#elif JUCE_MAC
            defaultHandle = nsviewHandle
#elif JUCE_WINDOWS
            defaultHandle = windowsHandle
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
    
} /* namespace Bke */

#endif /* BKE_NATIVE_HPP */
