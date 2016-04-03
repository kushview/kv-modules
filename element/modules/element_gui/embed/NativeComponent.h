/*
 native.hpp - Native Widget Embedding
 
 Copyright (C) 2014  Kushview, LLC.  All rights reserved.
 Michael Fisher <mfisher@kushview.net>
 
 */

/** @file native.hpp */

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
