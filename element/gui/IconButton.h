/*
 * IconButton.hpp
 *
 *  Created on: Jan 25, 2013
 *      Author: mfisher
 */

#ifndef BKE_ICONBUTTON_HPP
#define BKE_ICONBUTTON_HPP

#include "element/juce.hpp"
#include "element/gui/Tango.h"
#include "element/Signals.h"

namespace Element {
namespace Gui {

    class IconButton : public Button,
                       public ButtonListener
    {
        //boost::signal<void()> clickedSignal;

    public:

        IconButton (const String& section, const String& slug,
                    bool toggle = false)
            : Button (slug), padding (0), isToggle (toggle)
        {
            icon = Tango::getDrawable (section, slug);
            addListener (this);
        }

        //boost::signal<void()>& signalClicked() { return clickedSignal; }

        void mouseDown (const MouseEvent& ev)
        {
//            if (isToggle)
  //              setToggleState (! getToggleState(), true);

            Button::mouseDown (ev);
        }

        bool hitTest (int x, int y)
        {
            // detect alpha maybe
            return Button::hitTest (x, y);
        }

        void paintButton (Graphics& g, bool mouseIsOver, bool mouseIsDown)
        {
            g.setColour (Colours::grey);
            g.fillRoundedRectangle (0,0,getWidth(),getHeight(), 3);

            g.setColour (Colours::darkgrey);
            g.drawRoundedRectangle (0, 0, getWidth(), getHeight(), 3, 1);

            Rectangle<float> r (4, 4, getHeight() - 8, getHeight() - 8);

            icon->drawWithin (g, r, RectanglePlacement::centred,
                                    getIconAlpha (mouseIsDown, mouseIsOver));
        }

        void buttonClicked (Button* btn) {
        //    clickedSignal();
        }

    private:
        ScopedPointer<Drawable> icon;
        float padding;
        bool isToggle;

        const float getIconAlpha (bool isDown, bool isOver)
        {
            if (isToggle && getToggleState())
            {
                return 1.0;
            }
            else if (isDown)
            {
               return 0.95;
            }
            else if (isOver)
            {
                return 0.80;
            }
            else
            {
                return 0.70;
            }
        }
    };


    class TransportButtons : public Component
    {
    public:
        TransportButtons()
            : record ("actions", "media-record", true),
              stop ("actions", "media-playback-stop"),
              play ("actions", "media-playback-start", true)
        {
            addAndMakeVisible (&record);
            record.setSize (22, 22);
            layout.setItemLayout (0,22, 22, 22);

            addAndMakeVisible (&stop);
            stop.setSize (22, 22);
            layout.setItemLayout (1, 22, 22, 22);

            addAndMakeVisible (&play);
            play.setSize (22, 22);
            layout.setItemLayout (2, 22, 22, 22);
        }

        void resized()
        {
            Component* comps[] = { &record, &stop, &play };
            layout.layOutComponents (comps, 3, 0, 0, getWidth(), 22, false, false);
        }

    private:

        IconButton record,stop,play;
        StretchableLayoutManager layout;
    };


}} // namespace Bke

#endif /* BKE_ICONBUTTON_HPP */
