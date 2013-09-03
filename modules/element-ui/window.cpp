/*
  window.cpp - Class Description

  Copyright (C) 2013  BKE, LLC.  All rights reserved.
  mfisher <mfisher@bketech.com>

*/
/** @file window.cpp */

#include "guiapp.hpp"
#include "window.hpp"

namespace element {
namespace gui {


    Dialog::Dialog (const String& name, GuiApp& gui_)
        : DialogWindow (name, Colours::darkgrey, true, true),
          gui (gui_)
    {
        setUsingNativeTitleBar (true);
        setResizable (true, false);
    }

    Dialog::~Dialog()
    {

    }

    void
    Dialog::closeButtonPressed()
    {
        closedSignal();
    }


    Window::Window (const String& name, GuiApp& app_)
        : DocumentWindow (name, Colours::darkgrey,
                                DocumentWindow::closeButton |
                                DocumentWindow::minimiseButton,
                                true),
          gui (app_)
    {
        setUsingNativeTitleBar (true);
        setResizable (true, false);
    }

    Window::~Window() {}

    void
    Window::closeButtonPressed()
    {
        closedSignal();
    }

}}  /* namespace element::gui */
