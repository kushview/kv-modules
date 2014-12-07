/*
    TimelineClip.cpp - This file is part of Element
    Copyright (C) 2014  Kushview, LLC.  All rights reserved.

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

#if JUCE_COMPLETION
#include "modules/element_gui/element_gui.h"
#endif

TimelineClip::TimelineClip (TimelineBase& timeline)
    : owner (timeline)
{
    selected = false;
    setInterceptsMouseClicks (true, true);
    setName ("TimelineClip");
    lastSnap = getBoundsInParent().getX();
    isResizing = trimming = false;
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

void TimelineClip::mouseDown (const MouseEvent& ev)
{
    owner.clipClicked (this, ev);

#if 0
    if (ev.mods.isPopupMenu()) {
        PopupMenu menu;
        menu.addItem (1, "Menu Item");
        menu.show();
    }
#endif

    isResizing = ev.x >= getWidth() - 2;
    trimming   = ev.x >= 0 && ev.x < 3;


    if (ev.mods.isLeftButtonDown()) {
        dragger.startDraggingComponent (this, ev);
        mouseDownX = ev.x;
        getClipRangeInternal (dragRange);
    }
}

void
TimelineClip::mouseDrag (const MouseEvent& ev)
{
    int changes = 0;

    // use the dragger to compute an x delta from the last drag
    Rectangle<int> r (getBoundsInParent());
    dragger.dragComponent (this, ev, nullptr);
    const int32 pixel = getBoundsInParent().getX();
    int32 snap = pixel;
    setBounds (r); // reset it back to normal
    const TimeUnit unit (getTimeUnit());

   // if (snap != lastSnap)
    {
        // apply movement if snapping has changed
        ClipRange<double> time;
        const ClipRange<double> old;
        getClipRangeInternal (time);
        getClipRangeInternal (const_cast<ClipRange<double>& >(old));

        if (! isResizing && ! trimming)
        {
            time.setStart (owner.xToTime (pixel, unit));
            time.setLength (old.getLength());
            time.setOffset (old.getOffset());
            setClipRangeInternal (time);

            owner.clipMoved (this, ev, time.getStart() - old.getStart(),
                                       time.getEnd() - old.getEnd());
        }
        else if (isResizing)
        {
            time.setEnd (owner.xToTime (getBoundsInParent().getX() + ev.x, unit));
            setClipRangeInternal (time);
            owner.clipMoved (this, ev, 0.0f, time.getEnd() - old.getEnd());
        }
        else if (trimming)
        {
            time.setStart (owner.xToTime (getBoundsInParent().getX() + ev.x, unit));
            time.setOffset (dragRange.getOffset() + (time.getStart() - dragRange.getStart()));
            setClipRangeInternal (time);
            owner.clipMoved (this, ev, 0.0f, time.getOffset() - old.getOffset());
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

    if (changes) {
        owner.updateClip (this);
        repaint();
    }
}

void
TimelineClip::mouseUp (const MouseEvent &ev)
{
    isResizing = false;
    trimming = false;
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
TimelineClip::mouseMove (const MouseEvent& ev)
{
    if (ev.x >= getWidth() - 2 || (ev.x >= 0 && ev.x < 3))
    {
        setMouseCursor (MouseCursor (MouseCursor::LeftRightResizeCursor));
    }
    else
    {
        setMouseCursor (MouseCursor (MouseCursor::NormalCursor));
    }
}

void TimelineClip::getClipRangeInternal (ClipRange<double>& range)
{
    getClipRange (range);
}

void TimelineClip::setClipRangeInternal (const ClipRange<double>& range)
{
    setClipRange (range);
}
