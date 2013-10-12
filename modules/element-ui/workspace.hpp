/*
 * Dock.h
 *
 *  Created on: Jan 5, 2013
 *      Author: mfisher
 */

#ifndef DOCK_H_
#define DOCK_H_

#include "element/juce.hpp"
#include "element/gui/dock.hpp"
#include "guiapp.hpp"

namespace Element {
namespace gui {


    class Workspace
    {
    public:

        Workspace (GuiApp& g)
            : gui(g)
        {

        }

        virtual ~Workspace() { }

        void setMainComponent (Component* c)
        {

        }

        void paint (Graphics& g) { }

        void mouseDown (const MouseEvent& ev)
        {

        }

        void resized() {


        }

    private:

        GuiApp&            gui;
        Element::gui::Dock dock;

    };


}}



#endif /* DOCK_H_ */
