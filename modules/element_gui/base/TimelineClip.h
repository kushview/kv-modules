/*
    TimelineClip.h - This file is part of Element
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
#ifndef ELEMENT_TIMELINE_CLIP_H
#define ELEMENT_TIMELINE_CLIP_H

class TimelineBase;

class TimelineClip : public Component
{
public:

    virtual ~TimelineClip();

    inline void
    adjustTrackIndex (int trackDelta, bool notify)
    {
        if (trackDelta != 0)
            setTrackIndex (trackIndex() + trackDelta, notify);
    }

    virtual bool moveable() const { return true; }
    virtual bool resizable() const { return true; }
    virtual bool timeIsMusical() const { return false; }

    virtual void getTime (Range<double>& time) const = 0;
    virtual void setTime (const Range<double>& time) = 0;
    virtual TimeUnit getTimeUnit() const { return TimeUnit::Seconds; }
    virtual int32 trackIndex() const = 0;

    inline void
    setSelected (bool isSelected)
    {
        if (selected == isSelected)
            return;

        selected = isSelected;
        selectedStateChanged();
    }

    inline bool isSelected() const { return selected; }


    virtual bool hitTest (int x, int y);
    virtual void mouseDoubleClick (const MouseEvent &event);
    virtual void mouseDown (const MouseEvent& ev);
    virtual void mouseDrag (const MouseEvent& ev);
    virtual void mouseMove (const MouseEvent& e);
    virtual void mouseUp (const MouseEvent &event);



protected:

    TimelineClip (TimelineBase&);
    void recycle();
    TimelineBase& timeline();
    const TimelineBase& timeline() const;

    virtual void reset() { }
    virtual void selectedStateChanged() { }
    virtual int32 trackRequested (const int32 track) { return track; }

    uint16 ticksPerBeat() const;

private:

    friend class TimelineBase;

    TimelineBase& owner;
    ComponentDragger dragger;

    int lastSnap, currentTrack;
    bool isResizing, selected;

    void setTrackIndex (const int track, bool notify);
    void setTimeInternal (const Range<double>& time);
    void getTimeInternal (Range<double>& time);

};

#endif /* ELEMENT_TIMELINE_CLIP_H */
