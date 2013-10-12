/*
  window-manager.hpp - Header description

  Copyright (C) 2013  BKE, LLC.  All rights reserved.
  mfisher <mfisher@bketech.com>

*/
/** @file window-manager.hpp */

#ifndef WINDOW_MANAGER_HPP_
#define WINDOW_MANAGER_HPP_

#include "element/juce.hpp"
#include "commands.hpp"
#include "window.hpp"

namespace Element {
namespace gui {

    class GuiApp;

    class WindowManager
    {
    public:

        WindowManager (GuiApp& app);
        ~WindowManager()
        {
            for (Window* w : activeWindows)
            {
                w->setVisible (false);
                w->removeFromDesktop();
            }

            activeWindows.clear (true);
        }

        /** Show and manage a window or dialog

            The window manager takes ownership of the passed-in object
            The type needs to at least inherrit Compnent and WindowHook

            Another option would be to inherrit Component only, but provide
            yourself the required  Signal& signalClosed()  method */

        inline void show (Window* window)
        {
            activeWindows.addIfNotAlreadyThere (window);
            window->signalClosed().connect (
                        boost::bind (&WindowManager::onWindowClosed, this, window));
        }

        inline void manage (DialogWindow* dialog)
        {

        }

    private:

        void onWindowClosed (Window* c)
        {
            jassert (activeWindows.contains (c));
            c->setVisible (false);
            activeWindows.removeObject (c, true);
        }

        OwnedArray<Window> activeWindows;
        GuiApp& gui;

    };

}}

#endif
