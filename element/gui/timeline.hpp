/*
    timeline.hpp - This file is part of Element
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

#ifndef ELEMENT_GUI_TIMELINE_HPP
#define ELEMENT_GUI_TIMELINE_HPP

#include "element/juce.hpp"
#include "element/gui/icon-button.hpp"
#include "element/gui/tango.hpp"
#include "element/parameter.hpp"
#include "element/pointer.hpp"

namespace element {
namespace gui {

    class Timeline;

    class Dummy : public Component
    {
    public:
        Dummy() {}
        ~Dummy() {}
        void paint (Graphics& g)
        {
            g.setOpacity (.6f);
            g.fillAll(Colours::lightskyblue);
        }
    };


    class Ruler : public Component
    {
    public:
        Ruler() {}
        ~Ruler() {}

        void paint (Graphics& g)
        {

        }

    private:

    };


    class TimelineHeader : public Component
    {
    public:

        TimelineHeader (Timeline& parent);
        virtual ~TimelineHeader();

        void paint (Graphics& g);
        void resized();

    private:
        Timeline &owner;

        int middle;
        void paintDivider (Graphics& g);
        void paintTicks (Graphics& g);

    };


    class TimelineItem : public Component
    {
    public:
        TimelineItem() {}
        ~TimelineItem() {}

        void paint (Graphics& g)
        {
            g.fillAll (Colours::lightgreen);
        }

    private:

        double offset, start, duration;
    };


    class TimelineBody : public Component
    {
    public:
        TimelineBody (Timeline& parent);
        virtual ~TimelineBody();
        void paint (Graphics& g);

    private:
        Timeline& owner;
    };


    class Timeline : public Component,
                     public Slider::Listener,
                     public ScrollBar::Listener
    {

    public:

        Timeline();

        ~Timeline();

        void paint (Graphics& g);

        void scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart);

        void sliderValueChanged (Slider* slider);

        void resized();

        double getBegin() const { return begin; }
        double getOffset() const { return offset; }
        double getDuration() const { return duration; }
        double getPixPerUnit() const { return (double) pixPerUnit; }

        double getMajorTickSize();

        enum ColourIDs {
            bodybackgroundColourId      = 0x9900001,
            headerBackgroundColourId    = 0x9900002,
        };

        void getVisibleRange (double &in, double &out)
        {
            in = xToTime (0);
            out = xToTime (body.getWidth());
        }


        class TestItem : public Component
        {
        public:
            TestItem (Timeline& parent) : timeline (parent)
            {
                start = 0; duration = 10;
            }

            void paint (Graphics& g)
            {
                g.fillAll (Colours::whitesmoke);
                g.setColour (Colours::lightgrey);
                g.setOpacity (0.80);
                g.drawRect(0,0,getWidth(),getHeight(),2);
            }

            bool isInVisibleRange()
            {
                double in, out, end;
                end = start + duration;
                timeline.getVisibleRange (in, out);
                return ((in >= start && in <= end) || (out >= start && out <= end));
            }

            void parentSizeChanged()
            {


            }

            void mouseDown (const MouseEvent& ev)
            {
                dragger.startDraggingComponent (this, ev);
            }

            void mouseDrag (const MouseEvent& ev)
            {
                dragger.dragComponent (this, ev, nullptr);
                start = timeline.xToTime (getBoundsInParent().getX() + 1 - timeline.trackWidth);
                repaint();
            }

        private:
            ComponentDragger dragger;
            double start, duration;
            Timeline& timeline;

        };

        ScopedPointer<TestItem> testItem;


        inline void setTrackWidth (int newWidth) { trackWidth = newWidth; }

    protected:

    private:

        TimelineHeader header;
        TimelineBody   body;

        ScrollBar hscroll, vscroll;
        int scrollBarWidth;
        Parameter zoom;


        class Indicator;
        Indicator* ind;

        int trackWidth;

        double begin, duration, offset;

        double pixPerUnit;

        double xToTime (int x);
        int timeToX (double time);
        double getTime();

        friend class TimelineBody;
        friend class TimelineHeader;

    };

}}

#endif /* ELEMENT_GUI_TIMELINE_HPP */
