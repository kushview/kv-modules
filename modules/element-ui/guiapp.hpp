/*
 * App.hpp
 *
 *  Created on: Jan 7, 2013
 *      Author: mfisher
 */

#ifndef BTV_GUIAPP_HPP
#define BTV_GUIAPP_HPP

#include "element/session/app-controller.hpp"
#include "element/engine.hpp"
#include "element/pointer.hpp"
#include "element/session.hpp"
#include "element/gui/Style.h"

#include "preferences.hpp"
#include "window-manager.hpp"

namespace Element {

    class World;

namespace gui {

    class MainWindow;

    class GuiApp :  public AppController
    {
    public:

        ~GuiApp();

        static Shared<GuiApp> create (World&);
        void run();

        ApplicationCommandManager& commander() { return commandManager; }

        void openWindow (const String& uri)
        {
            String msg ("Open window: ");
            msg << uri;
            Logger::writeToLog (msg);
        }

        void openDialog (const String& uri)
        {

#if JUCE_DEBUG
            String msg ("Open dialog: ");
            msg << uri;
            Logger::writeToLog (msg);
#endif

            if (uri == "element://dialogs/preferences")
            {
                DialogWindow::LaunchOptions opts;
                opts.dialogBackgroundColour = Colours::darkgrey;
                opts.content.set (new PreferencesComponent (*this), true);
                opts.dialogTitle = "Element Preferences";
                opts.resizable = false;
                opts.useBottomRightCornerResizer = false;
                opts.useNativeTitleBar = true;
                opts.componentToCentreAround = (Component*) mainWindow.get();
                DialogWindow* dw = opts.launchAsync();
            }
        }

        Shared<Engine> engine() { return audioEngine; }

    protected:

        GuiApp (Element::World&);

    private:

        Scoped<WindowManager>     windowManager;
        Scoped<MainWindow>        mainWindow;

        Shared<Engine>            audioEngine;
        ApplicationCommandManager commandManager;
        Element::gui::Style       elementStyle;

        void showSplash();

    };


    }

} /* namespace Drumbot::Gui */

#endif /* BTV_GUIAPP_HPP */
