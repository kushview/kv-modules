/*
 * workspace.h
 *
 *  Created on: Dec 23, 2012
 *      Author: mfisher
 */

#ifndef BTV_MAINWINDOW_HPP
#define BTV_MAINWINDOW_HPP

#include "element/juce.hpp"
#include "guiapp.hpp"

namespace Element {
namespace gui {

    class MainMenu;

class MainWindow : public DocumentWindow
{
public:

    MainWindow (GuiApp& gui_);
    virtual ~MainWindow();

    GuiApp& app() { return gui; }
    void closeButtonPressed();

private:

    GuiApp&     gui;
    ScopedPointer<MainMenu>   mainMenu;

};


}} /* namespace Btv::Gui */


#endif /* BTV_MAINWINDOW_HPP */
