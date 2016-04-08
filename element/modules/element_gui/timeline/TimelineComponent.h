/*
    This file is part of the element modules for the JUCE Library
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

#ifndef EL_TIMELINE_BASE_H
#define EL_TIMELINE_BASE_H

class TimelineComponent;
class TimelineClip;

class TimelineIndicator : public Component
{
public:

   TimelineIndicator();
   virtual ~TimelineIndicator();

   void paint (Graphics& g);

   void mouseDown (const MouseEvent& ev);
   void mouseDrag (const MouseEvent& ev);
   void parentSizeChanged();

   void setPosition (double time, bool musical);
   double position() const;
   Value& positionValue() { return pos; }

   void setColour (const Colour& newColor);

   inline bool dragable() const { return isDragable; }
   inline void setDragable (bool m) { isDragable = m; }

   inline bool snapState() const { return shouldSnap; }
   inline void setSnapState (bool snap) { shouldSnap = snap; }

protected:

   TimelineComponent* timeline() const;

private:

   int lastSnap;
   bool shouldSnap, isDragable;

   mutable TimelineComponent* owner;
   ComponentDragger dragger;
   Colour           color;

   Value pos;
   double getUnits() const;
};


struct TimelinePosition : public AudioPlayHead::CurrentPositionInfo
{ };

class TimelineComponent : public AsyncUpdater,
                     public Component,
                     public Slider::Listener,
                     public ScrollBar::Listener,
                     public Value::Listener,
                     protected Timer
{

public:
    TimelineComponent();
    virtual ~TimelineComponent();

    inline double getBegin() const { return timeSpan.getStart(); }

    inline double offset() const { return timeOffset; }
    inline double duration() const { return timeSpan.getLength(); }
    inline double getPixPerUnit() const { return (double) pixPerUnit; }

    double getMajorTickSize();

    enum ColourIDs {
        bodyBackgroundColourId      = 0x9900001,
        headerBackgroundColourId    = 0x9900002
    };

    inline void setAllTrackHeights (const int height) {
        for (int i = getNumTracks(); --i >= 0;)
            setTrackHeight (i, height);
    }

    inline void setTrackHeight (int track, int height)
    {
        heights.set (track, (height <= 0) ? TrackHeights::Normal : height);
    }

    inline void getVisibleRange (double &in, double &out)
    {
        in = xToTime ((int) timeOffset);
        out = xToTime ((int) timeOffset + getWidth());
    }


    inline int pixelSnap (int pixel) const
    {
        return scale.pixelSnap (pixel - mTrackWidth) + mTrackWidth;
    }


    inline void setTrackWidth (int newWidth)
    {
        mTrackWidth = clampNoMoreThan (newWidth, 0, mMaxTrackWidth);
        resized();
    }

    inline void setMaxTrackWidth (int newWidth)
    {
        mMaxTrackWidth = newWidth;

        if (mTrackWidth > mMaxTrackWidth) {
            mTrackWidth = mMaxTrackWidth;
            resized();
        }
    }

    inline int getTrackWidth() const
    {
        return mTrackWidth;
    }

    virtual int getNumTracks() const = 0;

    inline int getTracksTotalHeight() const {
        return heights.totalHeight (getNumTracks());
    }

    inline void setTrackHeightsOffset (int offset, bool offsetIsDelta)
    {

        if (offsetIsDelta)
            offset = heights.offset() + offset;

        if (offset > 0)
            offset = 0;

        if (offset <= -getTracksTotalHeight() + getHeight() / 2)
            offset = -getTracksTotalHeight() + getHeight() / 2;

        if (offset != heights.offset())
        {
            heights.setOffset (offset);
            triggerAsyncUpdate();
        }
    }

    inline void setTrackVisibility (const BigInteger& vis)
    {
        heights.setEnablement (vis);
        triggerAsyncUpdate();
    }

    const TimeScale& timeScale() const { return scale; }

    virtual void paint (Graphics& g);
    virtual void paintOverChildren (Graphics& g);
    virtual void resized();

    virtual void mouseDown (const MouseEvent &event);
    virtual void mouseDrag (const MouseEvent &event);
    virtual void mouseUp (const MouseEvent &event);

    void scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart);
    void sliderValueChanged (Slider* slider);

    inline TimelineIndicator* indicator() { return playheadIndicator.get(); }
    inline void setIndicator (TimelineIndicator* indicator)
    {

        addAndMakeVisible (playheadIndicator = indicator);
    }

    void valueChanged (Value &value);

    inline double getTempo() const { return tempo.getValue(); }
    inline Value& getTempoValue() { return tempo; }
    inline void setTempo (double bpm) { tempo.setValue (bpm); }
    inline double getTimeOffset (TimeUnit unit = TimeUnit::Seconds) const { return xToTime (0, unit); }
    inline int getPixelOffset() const { return pixelOffset; }

    inline uint16 ticksPerBeat() const { return scale.ticksPerBeat(); }

    int timeToWidth (const Range<double>& time, const TimeUnit unit = TimeUnit::Seconds) const;
    int timeToWidth (const double& length, TimeUnit unit = TimeUnit::Seconds) const;

    double getTime() const;

protected:

    /** Called when a clip is clicked on the timeline.
        Override this if you need to modify clips related to the passed clip
        e.g. a selection

        @param clip The clip that was clicked
        @param clipEvent The MouseEvent of the click
    */
    virtual void clipClicked (TimelineClip*, const MouseEvent&) { }

    /** Called when a clip is double clicked on the timeline.
        Override this if you need to modify clips related to the passed clip
        e.g. a selection

        @param clip The clip that was clicked
        @param clipEvent The MouseEvent of the click
    */
    virtual void clipDoubleClicked (TimelineClip*, const MouseEvent&) { }

    /** Called when a clip is moved on the timeline.
        Override this if you need to modify clips related to the passed clip
        e.g. a selection

        @param clip The clip that was clicked
        @param clipEvent The MouseEvent of the click
        @param deltaStart The change in the start time
        @param deltaEnd The change in the end time
    */
    virtual void clipMoved (TimelineClip*, const MouseEvent&, double, double) { }

    /** Called when a clip is moved from one track to another
        Override this if you need to modify clips related to the passed clip
        e.g. a selection

        @param clip The clip that was moved
        @param deltaTracks The change in track index
    */
    virtual void clipChangedTrack (TimelineClip*, int) { }


    /** Called when the timeline body is clicked */
    virtual void timelineBodyClicked (const MouseEvent&, int) { }

    /** Called when the timeline header area is clicked */
    virtual void timelineTrackHeadersClicked (const MouseEvent&, int) { }

    /** Add a clip to the timeline at a given track */
    void addTimelineClip (TimelineClip* clip, int track = 0);

    /** Returns a track index for a MouseEvent */
    inline const int trackAt (const MouseEvent& ev) const {
        return heights.trackAtY (ev.y);
    }

    inline const int trackAt (const Point<int>& point) const {
        return heights.trackAtY (point.getY());
    }

    int32 beatToX (double beat) const;
    int32 frameToX (double frame) const;
    int32 secondsToX (double time) const;
    int32 tickToX (const double tick) const;
    int32 timeToX (double time, TimeUnit unit = TimeUnit::Seconds) const;

    int64 xToFrame    (int x) const;
    double xToSeconds (int32 x) const;
    double xToTicks   (int32 x, bool snap = false) const;
    double xToBeat    (int x, bool snap = false) const;
    double xToTime    (int x, const TimeUnit unit = TimeUnit::Seconds) const;

    double tickToTime (const double tick) const;



    void recycleClip (TimelineClip* clip);
    void updateClip (TimelineClip* clip);

    TimelineClip* getFirstClipOnTrack (int track) const;

    template<class ClipType>
    inline ClipType* findFreeClip()
    {
        for (int i = 0; i < freeClips.size(); ++i)
        {
            if (ClipType* clip = dynamic_cast<ClipType*> (freeClips.getUnchecked(i)))
            {
                freeClips.removeObject (clip, false);
                clips.add (clip);
                return clip;
            }
        }
        return nullptr;
    }

    virtual void paintTrackHeader (Graphics&, int, const Rectangle<int>&) { }
    virtual void paintTrackLane (Graphics&, int, const Rectangle<int>&) { }
    virtual void refreshComponentForTrack (const int) { }

    virtual inline Range<int>
    trackHeight (int track) const
    {
        Range<int> ret;
        ret.setStart (heights.trackY (track));
        ret.setLength (heights.get (track));
        return ret;
    }

    void removeClips();
    friend class TimelineClip;

    friend class Timer;
    virtual void timerCallback() { }

private:


    ScopedPointer<Component> topLeftWidget;
    ScopedPointer<TimelineIndicator> playheadIndicator;

    int mTrackWidth, mMaxTrackWidth;

    TimeScale scale;
    Value tempo;

    TrackHeights heights;
    TimelinePosition position;
    Range<double> timeSpan; // a.k.a the suggested visible range
    double timeOffset;
    int pixelOffset, dragX, dragY;
    double pixPerUnit;

    OwnedArray<TimelineClip> clips, freeClips;

    friend class TimelineBody;
    friend class TimelineHeader;
    friend class TimelineIndicator;

    friend class AsyncUpdater;
    void handleAsyncUpdate();

    void normalX (int32& x) const
    {
        x -= mTrackWidth;
        x -= pixelOffset;
    }
};

#endif /* EL_TIMELINE_BASE_H */
