/*
    NativeComponent.cpp - This file is part of Element
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

#include "element/gui/embed/NativeComponent.h"

#if JUCE_LINUX
namespace {
#include <X11/Xutil.h>
#include <X11/Xlib.h>
}
#endif

namespace Element {

   typedef NativeComponent::Attachment    Attachment;
   typedef NativeComponent::AttachmentPtr AttachmentPtr;

   class WidgetWrapper
   {
   public:
      WidgetWrapper (void* handle_) : handle (handle_) , realized (false)
      {
         jassert (handle != nullptr);
      }

      virtual ~WidgetWrapper() { }

      virtual void handleMouseDown  (const MouseEvent& ev) { }
      virtual void handleMouseEnter (const MouseEvent& ev) { }
      virtual void handleMouseExit  (const MouseEvent& ev) { }
      virtual void handleMouseWheelMove (const MouseEvent& ev, const MouseWheelDetails& d) { }
      virtual void handleMouseMove  (const MouseEvent& ev) { }
      virtual void handleMouseUp    (const MouseEvent& ev) { }
      virtual void handleMouseDrag (const MouseEvent& ev) { }

      virtual void show() = 0;
      virtual void hide() = 0;

      virtual void expose (const Rectangle<int>& bounds) { }
      virtual void reparent (void* newParent, int x = 0, int y = 0) { }
      virtual void translate (int x, int y) { }
      virtual void resize (uint32 w, uint32 h) { }
      virtual void realize() { }

      virtual Rectangle<int> getBounds() = 0;
      virtual void* getHandle() { return handle; }

      bool isRealized() const { return realized; }

   protected:

      void setRealized (bool isRealized) { realized = isRealized; }

   private:
      void* handle;
      bool  realized;

   };


   class NativeAttachment : public Attachment,
   public ComponentMovementWatcher
   {
   public:

      NativeAttachment (Component& owner_, WidgetWrapper* wrapper)
          : ComponentMovementWatcher (&owner_),
            owner (owner_),
            widget (wrapper)
      {
         currentPeer = owner.getPeer();
         widget->hide();
      }

      void onTest() {
         std::cout << "tested\n";
      }

      ~NativeAttachment() { removeFromParent(); }

      void componentMovedOrResized (Component& comp, bool wasMoved, bool wasResized)
      {
         ComponentMovementWatcher::componentMovedOrResized (comp, wasMoved, wasResized);

         if (comp.isOnDesktop() && wasResized)
            componentMovedOrResized (wasMoved, wasResized);
      }

      void componentMovedOrResized (bool wasMoved, bool wasResized)
      {
         Component* const topComp = owner.getTopLevelComponent();
         if (topComp->getPeer() != nullptr)
         {

            Rectangle<int> area (topComp->getLocalArea (&owner, owner.getBoundsInParent()));

            std::cout << "componentMovedOrResized():"
                  << " x: " << area.getX() << " y: " << area.getY()
                  << " w: " << area.getWidth() << " h: " << area.getHeight() << std::endl;

            if (widget->isRealized())
            {
               widget->translate (area.getX(), area.getY());
               widget->resize (area.getWidth(), area.getHeight());
            }
            else
            {
               widget->translate (area.getX(), area.getY());
               widget->realize();
            }
         }
      }

      void componentPeerChanged()
      {
         ComponentPeer* const peer = owner.getPeer();
         if (currentPeer != peer)
         {
            removeFromParent();
            currentPeer = peer;

            if (peer != nullptr)
            {
               int x=0, y=0;

               Component* const topComp = owner.getTopLevelComponent();
               if (topComp->getPeer() != nullptr)
               {
                  Rectangle<int> area (topComp->getLocalArea (&owner, owner.getLocalBounds()));
                  x = area.getX(); y = area.getY();
               }

               widget->reparent (peer->getNativeHandle(), x, y);

               if (! widget->isRealized())
                  widget->realize();

               owner.setBounds (widget->getBounds());
            }
         }
      }

      void componentVisibilityChanged()
      {
         if (owner.isVisible())
         {
            widget->hide();
            widget->show();
         }
         else
            widget->hide();

      }

      void* getHandle() { return widget->getHandle(); }

      WidgetWrapper& getWidget() { return *widget; }

      Rectangle<int> getWidgetBounds() { return widget->getBounds(); }

   private:

      Component&      owner;
      ComponentPeer*  currentPeer;
      WidgetWrapper*  widget;

      virtual void removeFromParent() { }

   };

#if JUCE_LINUX
   //===========================================================================
   class XWindow : public WidgetWrapper,
   private Timer
   {
      typedef Rectangle<int> Rect;
   public:
      XWindow (Component& c, void* handle)
      : WidgetWrapper (handle), display (nullptr),
        root (None), me ((XID) handle), parent (None),
        children (nullptr), nchildren (0)
      {
         if (Display* connection = XOpenDisplay(0))
         {
            display = connection;
            queryTree();

            int x, y; uint32 w, h, b, d;
            XGetGeometry (display, me, &root, &x, &y, &w, &h, &b, &d);
            bounds = Rect (x, y, w, h);

            // reduce flicker until realized
            XResizeWindow (display, me, 1, 1);

            c.setSize (w, h);
         }
         else
            jassertfalse; // this won't work without a display !
      }

      virtual ~XWindow()
      {
         if (display)
         {
            XCloseDisplay (display);
         }
      }

      void realize()
      {
         if (! isRealized())
         {
            translate (bounds.getX(), bounds.getY());
            resize (bounds.getWidth(), bounds.getHeight());
            setRealized (true);
         }
      }

      void resize (uint32 w, uint32 h)
      {
         if (me != None)
         {
            w = jmax (w, uint32(1)); h = jmax (h, uint32(1));
            if (BadWindow != XResizeWindow (display, me, w, h))
            {
               bounds.setSize (w, h);
               XFlush (display);
               // signal resized
            }
         }
      }

      void* getHandle() { return (void*) me; }

   public:

      void removeFromParent()
      {
         reparent (None, 0, 0);
      }

      void reparent (void* handle, int x, int y)
      {
         if (parent != (XID) handle)
            if (BadMatch != XReparentWindow (display, me, (XID) handle, x, y))
               parent = (XID) handle;
      }

      void show()
      {
         if (me != None)
         {
            XMapRaised (display, me);
            XFlush (display);
         }
      }

      void
      hide()
      {
         if (me != None)
         {
            XUnmapWindow (display, me);
            XFlush (display);
         }
      }

      void translate (int x, int y)
      {
         XMoveWindow (display, me, x, y);
         bounds.setPosition (x, y);
         XFlush (display);
      }

      void expose (const Rectangle<int>& rect)
      {
#if 0
         if (ComponentPeer* const peer = getPeer())
         {
            peer->addMaskedRegion (getScreenBounds() - peer->getScreenPosition());
#endif
            if (me != None)
            {
               XEvent ev = { 0 };
               ev.xexpose.type = Expose;
               ev.xexpose.display = display;
               ev.xexpose.window = me;
               ev.xexpose.x = rect.getX();
               ev.xexpose.y = rect.getY();
               ev.xexpose.width = rect.getWidth();  //owner.getWidth();
               ev.xexpose.height = rect.getWidth(); //owner.getHeight();

               sendEvent (&ev);
            }
#if 0
         }
#endif
      }

      Rectangle<int> getBounds() { return bounds; }

      void handleMouseDown (const MouseEvent& e)
      {
         if (me == None)
            return;

         XEvent ev = { 0 };
         ev.xbutton.display = display;
         ev.xbutton.type = ButtonPress;
         ev.xbutton.window = me;
         ev.xbutton.root = root; //RootWindow (display, DefaultScreen (display));
         ev.xbutton.time = CurrentTime;
         ev.xbutton.x = e.x;
         ev.xbutton.y = e.y;
         ev.xbutton.x_root = e.getScreenX();
         ev.xbutton.y_root = e.getScreenY();

         translateJuceToXButtonModifiers (e, ev);
         sendEvent (&ev);
      }


      void handleMouseEnter (const MouseEvent& e)
      {
          if (me != 0)
          {
              XEvent ev = { 0 };
              ev.xcrossing.display = display;
              ev.xcrossing.type = EnterNotify;
              ev.xcrossing.window = me;
              ev.xcrossing.root = RootWindow (display, DefaultScreen (display));
              ev.xcrossing.time = CurrentTime;
              ev.xcrossing.x = e.x;
              ev.xcrossing.y = e.y;
              ev.xcrossing.x_root = e.getScreenX();
              ev.xcrossing.y_root = e.getScreenY();
              ev.xcrossing.mode = NotifyNormal; // NotifyGrab, NotifyUngrab
              ev.xcrossing.detail = NotifyAncestor; // NotifyVirtual, NotifyInferior, NotifyNonlinear,NotifyNonlinearVirtual

              translateJuceToXCrossingModifiers (e, ev);

              sendEvent (&ev);
          }
      }

      void handleMouseExit (const MouseEvent& e)
      {
          if (me != 0)
          {
              XEvent ev = { 0 };
              ev.xcrossing.display = display;
              ev.xcrossing.type = LeaveNotify;
              ev.xcrossing.window = me;
              ev.xcrossing.root = RootWindow (display, DefaultScreen (display));
              ev.xcrossing.time = CurrentTime;
              ev.xcrossing.x = e.x;
              ev.xcrossing.y = e.y;
              ev.xcrossing.x_root = e.getScreenX();
              ev.xcrossing.y_root = e.getScreenY();
              ev.xcrossing.mode = NotifyNormal; // NotifyGrab, NotifyUngrab
              ev.xcrossing.detail = NotifyAncestor; // NotifyVirtual, NotifyInferior, NotifyNonlinear,NotifyNonlinearVirtual
              ev.xcrossing.focus = True; //hasKeyboardFocus (true); // TODO - yes ?

              translateJuceToXCrossingModifiers (e, ev);

              sendEvent (&ev);
          }
      }

      void handleMouseMove (const MouseEvent& e)
      {
          if (me != 0)
          {
             std::cout << "mouseMove()\n";

              XEvent ev = { 0 };
              ev.xmotion.display = display;
              ev.xmotion.type = MotionNotify;
              ev.xmotion.window = me;
              ev.xmotion.root = RootWindow (display, DefaultScreen (display));
              ev.xmotion.time = CurrentTime;
              ev.xmotion.is_hint = NotifyNormal;
              ev.xmotion.x = e.x;
              ev.xmotion.y = e.y;
              ev.xmotion.x_root = e.getScreenX();
              ev.xmotion.y_root = e.getScreenY();

              sendEvent (&ev);
              expose (bounds);
          }
      }

      void handleMouseDrag (const MouseEvent& e)
      {
          if (me != 0)
          {
             std::cout << "mouseDrag()\n";
              XEvent ev = { 0 };
              ev.xmotion.display = display;
              ev.xmotion.type = MotionNotify;
              ev.xmotion.window = me;
              ev.xmotion.root = RootWindow (display, DefaultScreen (display));
              ev.xmotion.time = CurrentTime;
              ev.xmotion.x = e.x ;
              ev.xmotion.y = e.y;
              ev.xmotion.x_root = e.getScreenX();
              ev.xmotion.y_root = e.getScreenY();
              ev.xmotion.is_hint = NotifyNormal;

              translateJuceToXMotionModifiers (e, ev);
              sendEvent (&ev);
          }
      }

      void handleMouseUp (const MouseEvent& e)
      {
          if (me != 0)
          {
              XEvent ev = { 0 };
              ev.xbutton.display = display;
              ev.xbutton.type = ButtonRelease;
              ev.xbutton.window = me;
              ev.xbutton.root = RootWindow (display, DefaultScreen (display));
              ev.xbutton.time = CurrentTime;
              ev.xbutton.x = e.x;
              ev.xbutton.y = e.y;
              ev.xbutton.x_root = e.getScreenX();
              ev.xbutton.y_root = e.getScreenY();

              translateJuceToXButtonModifiers (e, ev);
              sendEvent (&ev);
          }
      }

      void handleMouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
      {
          if (me != 0)
          {
              XEvent ev = { 0 };
              ev.xbutton.display = display;
              ev.xbutton.type = ButtonPress;
              ev.xbutton.window = me;
              ev.xbutton.root = RootWindow (display, DefaultScreen (display));
              ev.xbutton.time = CurrentTime;
              ev.xbutton.x = e.x;
              ev.xbutton.y = e.y;
              ev.xbutton.x_root = e.getScreenX();
              ev.xbutton.y_root = e.getScreenY();

              translateJuceToXMouseWheelModifiers (e, wheel.deltaY, ev);
              sendEvent (&ev);

              // TODO - put a usleep here ?

              ev.xbutton.type = ButtonRelease;
              sendEvent (&ev);
          }
      }

      void
      sendEvent (XEvent* event, bool propogate = false, long mask = 0L)
      {
         if (me != None)
         {
            XSendEvent (display, me, False, None, event);
            XFlush (display);
         }
      }

   private:

      Display* display;
      Window root, parent, me;
      Window* children;
      uint32 nchildren;
      Rectangle<int> bounds;

      void queryTree() { XQueryTree (display, me, &root, &parent, &children, &nchildren); }

      void translateJuceToXButtonModifiers (const MouseEvent& e, XEvent& ev) noexcept
      {
          if (e.mods.isLeftButtonDown())
          {
              ev.xbutton.button = Button1;
              ev.xbutton.state |= Button1Mask;
          }
          else if (e.mods.isRightButtonDown())
          {
              ev.xbutton.button = Button3;
              ev.xbutton.state |= Button3Mask;
          }
          else if (e.mods.isMiddleButtonDown())
          {
              ev.xbutton.button = Button2;
              ev.xbutton.state |= Button2Mask;
          }
      }

      void translateJuceToXMotionModifiers (const MouseEvent& e, XEvent& ev) noexcept
      {
          if (e.mods.isLeftButtonDown())          ev.xmotion.state |= Button1Mask;
          else if (e.mods.isRightButtonDown())    ev.xmotion.state |= Button3Mask;
          else if (e.mods.isMiddleButtonDown())   ev.xmotion.state |= Button2Mask;
      }

      void translateJuceToXCrossingModifiers (const MouseEvent& e, XEvent& ev) noexcept
      {
          if (e.mods.isLeftButtonDown())          ev.xcrossing.state |= Button1Mask;
          else if (e.mods.isRightButtonDown())    ev.xcrossing.state |= Button3Mask;
          else if (e.mods.isMiddleButtonDown())   ev.xcrossing.state |= Button2Mask;
      }

      void translateJuceToXMouseWheelModifiers (const MouseEvent& e, const float increment, XEvent& ev) noexcept
      {
          if (increment < 0)
          {
              ev.xbutton.button = Button5;
              ev.xbutton.state |= Button5Mask;
          }
          else if (increment > 0)
          {
              ev.xbutton.button = Button4;
              ev.xbutton.state |= Button4Mask;
          }
      }

      void timerCallback()
      {
         XEvent ev = { 0 };
         if (True == XCheckWindowEvent (display, me, ConfigureNotify, &ev))
         {
            std::clog << "XWindow changed\n";
         }
      }

   };
    
#endif // JUCE_LINUX


   //
#define castNative(x) static_cast<NativeAttachment*>(x.get())

   //===========================================================================

   NativeComponent::NativeComponent (HandleType type)
   : handleType (type) { }

   NativeComponent::~NativeComponent() {}

   void NativeComponent::setHandle (void* handle)
   {
      if (handle != getHandle())
      {
         attachment = nullptr;

         if (handle != nullptr)
            attachment = attachViewToComponent (*this, handle, handleType);
      }
   }

   void* NativeComponent::getHandle()
   {
      if (NativeAttachment* obj = castNative (attachment))
         return obj->getHandle();

      return nullptr;
   }

   void NativeComponent::resizeToFit()
   {
      if (NativeAttachment* obj = castNative (attachment))
      {
         Rectangle<int> b (obj->getWidgetBounds());
         setSize (b.getWidth(), b.getHeight());
      }
   }

   void NativeComponent::showWidget()
   {
      if (NativeAttachment* obj = castNative (attachment))
      {
         obj->getWidget().show();
      }
   }

   void
   NativeComponent::paint (Graphics& g)
   {
      g.fillAll (Colours::black);
   }

   void
   NativeComponent::mouseDown (const MouseEvent& e)
   {
      if (NativeAttachment* obj = castNative (attachment))
      {
         toFront (false);
         obj->getWidget().handleMouseDown (e);
      }
   }

   void
   NativeComponent::mouseUp (const MouseEvent& e)
   {
      if (NativeAttachment* obj = castNative (attachment))
      {
         obj->getWidget().handleMouseUp (e);
      }
   }

   void
   NativeComponent::mouseEnter (const MouseEvent& e)
   {
      if (NativeAttachment* obj = castNative (attachment))
      {
         obj->getWidget().handleMouseEnter (e);
      }
   }

   void
   NativeComponent::mouseExit (const MouseEvent& e)
   {
      if (NativeAttachment* obj = castNative (attachment))
      {
         obj->getWidget().handleMouseExit (e);
      }
   }
   void NativeComponent::mouseMove (const MouseEvent& e)
   {
      if (NativeAttachment* obj = castNative (attachment))
      {
         obj->getWidget().handleMouseMove (e);
      }
   }

   void NativeComponent::mouseWheelMove   (const MouseEvent& e, const MouseWheelDetails& d)
   {
      if (NativeAttachment* obj = castNative (attachment))
      {
         obj->getWidget().handleMouseWheelMove (e, d);
      }
   }

   void NativeComponent::mouseDrag (const MouseEvent& e)
   {
      if (NativeAttachment* obj = castNative (attachment))
      {
         obj->getWidget().handleMouseDrag (e);
      }
   }

   void
   NativeComponent::resized()
   {
      resizeToFit();
   }

   Attachment*
   NativeComponent::attachViewToComponent (Component& c, void* handle, HandleType t)
   {
      Attachment* attachment (nullptr);

      switch (t)
      {
#if JUCE_LINUX
      case x11Handle: attachment = new NativeAttachment (c, new XWindow (c, handle)); break;
#elif JUCE_MAC
#elif JUCE_WINDOWS
#endif
      default: break;
      };

      return attachment;
   }
}
