/*
  window.hpp - Header description

  Copyright (C) 2013  BKE, LLC.  All rights reserved.
  mfisher <mfisher@bketech.com>

*/
/** @file window.hpp */

#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "element/juce.hpp"
#include "element/Signals.h"

namespace Element {
namespace gui {

    class GuiApp;

    /* A mixin for windows/dialogs that provides common signals */
    class WindowHook
    {
    public:

        WindowHook() { }
        virtual ~WindowHook() { Logger::writeToLog("~WindowHook()"); }
        inline Signal& signalClosed() { return closedSignal; }

    protected:

        Signal  closedSignal;

    };

    /** A juce DialogWindow that emits a closed signal */
    class Dialog : public DialogWindow,
            public WindowHook
    {
    public:
        Dialog (const String& name, GuiApp&);
        virtual ~Dialog();
        GuiApp& app() { return gui; }
        virtual void closeButtonPressed();

    private:
        GuiApp& gui;
    };


    /** A juce DocumentWindow that emits a closed signal */
    class Window : public DocumentWindow,
            public WindowHook
    {
    public:
        Window (const String& name, GuiApp&);
        virtual ~Window();
        GuiApp& app() { return gui; }
        void closeButtonPressed();

    protected:
        GuiApp& gui;
    };


}}
#endif
