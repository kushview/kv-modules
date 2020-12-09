/*
    This file is part of the Kushview Modules for JUCE
    Copyright (c) 2014-2019  Kushview, LLC.  All rights reserved.

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

#pragma once

namespace kv {

class TimelineComponent;

template<typename T>
class ClipRange : public Range<T> {
public:

    ClipRange() : offset (T()) { }
    inline T getOffset() const { return offset; }
    inline void setOffset (const T val) { offset = val; }

    inline ClipRange<T>& operator= (const ClipRange& o) {
        Range<T>::operator= (o);
        offset = o.offset;
        return *this;
    }

private:
    T offset;
};

class TimelineClip : public Component
{
public:

    virtual ~TimelineClip();

    enum EditType {
        Move = 1,
        TrimLeft,
        TrimRight
    };

    inline void
    adjustTrackIndex (int trackDelta, bool notify)
    {
        if (trackDelta != 0)
            setTrackIndex (trackIndex() + trackDelta, notify);
    }

    virtual bool moveable() const { return true; }
    virtual bool resizable() const { return true; }
    virtual bool timeIsMusical() const { return false; }

    virtual void getTime (Range<double>&) const { }
    virtual void setTime (const Range<double>&) { }
    virtual void getClipRange (ClipRange<double>& loc) {
        getTime (loc);
        loc.setOffset (0);
    }
    virtual void setClipRange (const ClipRange<double>& loc) {
        setTime (loc);
    }

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

    TimelineClip (TimelineComponent&);
    void recycle();
    TimelineComponent& timeline();
    const TimelineComponent& timeline() const;

    virtual void reset() { }
    virtual void selectedStateChanged() { }
    virtual int32 trackRequested (const int32 track) { return track; }

    uint16 ticksPerBeat() const;

private:

    friend class TimelineComponent;

    TimelineComponent& owner;
    ComponentDragger dragger;

    int lastSnap, currentTrack;
    bool isResizing, trimming, selected;
    ClipRange<double> dragRange;
    int mouseDownX;

    void setTrackIndex (const int track, bool notify);
    void setTimeInternal (const Range<double>& time);
    void getTimeInternal (Range<double>& time);
    void setClipRangeInternal (const ClipRange<double>& range);
    void getClipRangeInternal (ClipRange<double>& range);

};

}
