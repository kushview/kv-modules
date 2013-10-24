/*
    TimelineClip.cpp - This file is part of Element
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

#include "element/gui/TimelineBase.h"
#include "element/gui/TimelineClip.h"

namespace Element {
namespace Gui {

    TimelineClip::TimelineClip (TimelineBase& timeline)
        : owner (timeline)
    {
        setInterceptsMouseClicks (true, true);
        setName ("TimelineClip");
        lastSnap = getBoundsInParent().getX();
        isResizing = false;
    }

    TimelineClip::~TimelineClip() { }

    bool
    TimelineClip::hitTest (int x, int y)
    {
        return true;
    }

    void
    TimelineClip::mouseDoubleClick (const MouseEvent &event)
    {
        timeline().clipDoubleClicked (this, event);
    }

    void
    TimelineClip::mouseDown (const MouseEvent& ev)
    {
        owner.clipClicked (this, ev);

        if (ev.mods.isPopupMenu()) {
            PopupMenu menu;
            menu.addItem (1, "Menu Item");
            menu.show();
        }

        isResizing = ev.x >= getWidth() - 2;

        if (ev.mods.isLeftButtonDown()) {
            dragger.startDraggingComponent (this, ev);
        }
    }

    void
    TimelineClip::mouseDrag (const MouseEvent& ev)
    {
        int changes = 0;

        // use the dragger to compute an x delta from the last drag
        Rectangle<int> r (getBoundsInParent());
        dragger.dragComponent (this, ev, nullptr);
        int32 pixel = owner.pixelSnap (getBoundsInParent().getX());
        int32 snap = pixel;
        setBounds (r); // reset it back to normal

        if (snap != lastSnap)
        {
            // apply movement if snapping has changed
            Range<double> time;
            getTime (time);

            if (! isResizing)
            {
                double oldStart = time.getStart();
                double oldEnd   = time.getEnd();

                time = time.movedToStartAt (owner.xToTime (snap));
                setTime (time);

                owner.clipMoved (this, ev, time.getStart() - oldStart,
                                           time.getEnd() - oldEnd);
            }
            else
            {
                Range<double> old = time;

                int snapResize = owner.pixelSnap (getBoundsInParent().getX() + ev.x);
                pixel = owner.timeToX (time.getEnd());

                while (snapResize <= snap) {
                    snapResize = owner.pixelSnap (pixel++);
                }

                time.setEnd (owner.xToTime (snapResize));
                setTime (time);

                owner.clipMoved (this, ev, 0.0f, time.getEnd() - old.getEnd());
            }

            lastSnap = snap;
            ++changes;
        }

        /* Track Up/Down handling */
        if (ev.y < 0 && ! isResizing)
        {
            int nextTrack = owner.heights.trackAtY (ev.y + getBoundsInParent().getY());
            if (nextTrack < 0) nextTrack = 0;
            setTrackIndex (nextTrack, true);
            ++changes;
        }
        else if (ev.y > getHeight() && ! isResizing)
        {
            int nextTrack = owner.heights.trackAtY (ev.y + getBoundsInParent().getY());
            setTrackIndex (nextTrack, true);
            ++changes;
        }

        if (changes)
            owner.updateClip (this);
    }

    void
    TimelineClip::mouseUp (const MouseEvent &ev)
    {
        isResizing = false;
    }

    void
    TimelineClip::recycle()
    {
        reset();
        timeline().recycleClip (this);
    }

    void
    TimelineClip::setTrackIndex (const int32 track, bool notify)
    {
        const int oldTrack = trackIndex();
        if (track == oldTrack)
            return;

        const int32 nextTrack = trackRequested (track);
        if (oldTrack != nextTrack && notify)
            owner.clipChangedTrack (this, nextTrack - oldTrack);
    }

    uint16 TimelineClip::ticksPerBeat() const { return timeline().timeScale().ticksPerBeat(); }

    TimelineBase& TimelineClip::timeline() { return owner; }
    const TimelineBase& TimelineClip::timeline() const { return owner; }

    void
    TimelineClip::mouseMove (const MouseEvent& e)
    {
        if (e.x >= getWidth() - 2)
        {
            setMouseCursor (MouseCursor (MouseCursor::LeftRightResizeCursor));
        }
        else
        {
            setMouseCursor (MouseCursor (MouseCursor::NormalCursor));
        }
    }

    void
    TimelineClip::setTimeInternal (const Range<double>& time)
    {
        setTime (time);
    }

    void
    TimelineClip::getTimeInternal (Range<double>& time)
    {

    }
}}
