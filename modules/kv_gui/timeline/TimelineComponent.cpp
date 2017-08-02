/*
    This file is part of the Kushview Modules for JUCE
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

TimelineIndicator::TimelineIndicator()
    : owner (nullptr)
{
    color = Colours::black;
    setSize (1, 400);
    setMouseCursor (MouseCursor::LeftRightResizeCursor);
    pos = 1;
    lastSnap = 0;
    shouldSnap = false;
    isDragable = true;
}

TimelineIndicator::~TimelineIndicator() { }

void TimelineIndicator::setPosition (double time, bool musical)
{
    if (musical) {
        const float bpm = (float) timeline()->timeScale().getTempo();
        time = (time * (60.0f / bpm));
    }

    pos.setValue (time);
    parentSizeChanged();
}

void TimelineIndicator::paint (Graphics& g)
{
#if 0
    g.setColour (Colours::black);
    g.setOpacity (0.30);
    g.drawVerticalLine (0, 0, getHeight());
    g.drawVerticalLine (2, 0, getHeight());
#endif
    g.setColour (color);
    g.setOpacity (0.98);
    g.drawVerticalLine (0, 0, getHeight());
}


void TimelineIndicator::mouseDown (const MouseEvent& ev)
{
    if (dragable())
        dragger.startDraggingComponent (this, ev);
}

void TimelineIndicator::mouseDrag (const MouseEvent& ev)
{
    if (! dragable())
        return;

    Rectangle<int> r (getBoundsInParent());

    dragger.dragComponent (this, ev, nullptr);
    int snap = shouldSnap ? timeline()->pixelSnap (getBoundsInParent().getX())
                          : getBoundsInParent().getX();

    setBounds (r); // reset it back to normal

    if (snap != lastSnap || ! shouldSnap)
    {
        setBounds (snap, 0, 1, timeline()->getHeight());
        pos.setValue (getUnits());
        lastSnap = snap;
    }
}

void TimelineIndicator::parentSizeChanged()
{
    // DBG ("TimelineIndicator::parentSizeChanged: pos = " + String (position()) + " x = " + String(timeline()->timeToX (position())));
    setBounds (timeline()->timeToX (position()),
               0, 1, timeline()->getHeight());
}

double TimelineIndicator::position() const { return (double) pos.getValue(); }

void TimelineIndicator::setColour (const Colour& newColor)
{
    color = newColor;
    this->repaint();
}

TimelineComponent* TimelineIndicator::timeline() const
{
    if (owner == nullptr)
    {
        owner = findParentComponentOfClass<TimelineComponent>();
        if (owner == nullptr) {
            jassertfalse; // this needs to be owned by a timeline base object
        }
    }

    return owner;
}

double TimelineIndicator::getUnits() const
{
    return timeline()->xToTime (getBoundsInParent().getX());
}


void TimelineComponent::recycleClip (TimelineClip* clip)
{
    clips.removeObject (clip, false);
    removeChildComponent (clip);
    freeClips.add (clip);
}

void TimelineComponent::handleAsyncUpdate()
{
    for (int i = 0; i < clips.size(); ++i)
    {
		TimelineClip* const clip = clips.getUnchecked(i);
        if (clip->isShowing()) {
            updateClip (clip);

            clip->setBounds (clip->getBounds());
            clip->repaint();
        }
    }

    repaint();
}

TimelineComponent::TimelineComponent()
{
    freeClips.clear();
    clips.clear();
    pixelOffset = 0;
    heights.ensureTracks (512, TrackHeights::Mini);
    heights.setOffset (0);

    mTrackWidth = 0;
    mMaxTrackWidth = 200;

    scale.setSampleRate (48000);
    scale.setTempo (120.f);
    scale.setTicksPerBeat (1920);
    scale.setPixelsPerBeat (30);
    scale.updateScale();

    pixPerUnit = (double) scale.pixelsPerBeat();

    timeSpan.setLength (1.0);
    addAndMakeVisible (playheadIndicator = new TimelineIndicator);
    playheadIndicator->setAlwaysOnTop (true);

    tempo.addListener (this);
    scale.setDisplayFormat (TimeScale::BBT);
}

TimelineComponent::~TimelineComponent()
{
    tempo.removeListener (this);
}

void TimelineComponent::addTimelineClip (TimelineClip* clip, int track)
{
    if (clip == nullptr)
        return;

    freeClips.removeObject (clip, false);
    addAndMakeVisible (clip);
    clips.addIfNotAlreadyThere (clip);
    // clip->setTrackIndex (track, true);
    updateClip (clip);
}

void TimelineComponent::paintOverChildren (Graphics& g)
{
    const int numTracks = getNumTracks();
    int track = heights.trackAtY (0);
    Rectangle<int> r;

    while (true)
    {
        r.setX (0);
        r.setWidth (mTrackWidth);
        r.setY (heights.trackY (track));
        r.setHeight (heights.get (track));

        if (r.getY() > getHeight() || track >= numTracks)
            break;

        if (! heights.trackIsVisible (track)) {
            ++track;
            continue;
        }

        if (mTrackWidth > 0) {
            g.saveState();
            paintTrackHeader (g, track, r);
            g.restoreState();
        }

#if 0
        r.setX (mTrackWidth);
        r.setWidth (getWidth() - mTrackWidth);
        r.setHeight (r.getHeight() + heights.spacing());

        g.saveState();
        paintTrackLane (g, track, r);
        g.restoreState();
#endif
        ++track;
    }
}

void TimelineComponent::paint (Graphics& g)
{
    g.setColour (Colour (0xff454545));
    g.fillAll();

    // track header divider line
    g.setColour (Colours::black.withAlpha (0.5f));
    g.drawVerticalLine (mTrackWidth + 1, 0, getHeight());

    int track = heights.trackAtY (0);
    Rectangle<int> r;
    while (true)
    {
        if (r.getY() > getHeight() || track >= getNumTracks())
            break;

        if (! heights.trackIsVisible (track)) {
            ++track;
            continue;
        }

        r.setX (mTrackWidth);
        r.setY (heights.trackY (track));
        r.setWidth (getWidth() - mTrackWidth);
        r.setHeight (heights.get (track) + heights.spacing());

        g.saveState();
        paintTrackLane (g, track, r);
        g.restoreState();

        ++track;
    }

#if 0
    int track = heights.trackAtY (0);
    Rectangle<int> r;

    while (true)
    {
        r.setX (0);
        r.setWidth (mTrackWidth);
        r.setY (heights.trackY (track));
        r.setHeight (heights.get (track));

        if (r.getY() > getHeight() || track >= getNumTracks())
            break;

        if (! heights.trackIsVisible (track)) {
            ++track;
            continue;
        }

        if (mTrackWidth > 0) {
            g.saveState();
            paintTrackHeader (g, track, r);
            g.restoreState();
        }

        r.setX (mTrackWidth);
        r.setWidth (getWidth() - mTrackWidth);
        r.setHeight (r.getHeight() + heights.spacing());

        g.saveState();
        paintTrackLane (g, track, r);
        g.restoreState();

        ++track;
    }
#endif

#if 0
    // paint grid lines
    g.saveState();
    g.setColour (Colours::white);

    for (int beat = scale.beatFromPixel (timeToX (timeSpan.getStart())) - 1;
         ++beat;)
    {
        const int pixel = scale.pixelFromBeat (beat) + mTrackWidth;

        if (pixel < 0)
            continue;

        if (scale.beatIsBar (beat))
        {
            g.saveState();
            g.setColour(Colours::white);
            g.drawText (String(beat), pixel, 0, 20, 16, Justification::left, true);
            g.restoreState();
            g.setOpacity (0.20);
        }
        else
            g.setOpacity (0.10);

        g.drawVerticalLine (pixel, 0, getHeight());

        if (pixel > getWidth())
            break;
    }

    g.restoreState();
    g.resetToDefaultState();
#endif

}

void TimelineComponent::resized()
{

}

double TimelineComponent::getMajorTickSize()
{
    return 0.0f;
}

TimelineClip* TimelineComponent::getFirstClipOnTrack (int track) const
{
	for (int i = 0; i < clips.size(); ++i) {
		if (TimelineClip* c = clips.getUnchecked(i))
	       if (c->trackIndex() == track)
		       return c;
   }
   return nullptr;
}


void
TimelineComponent::mouseDown (const MouseEvent &ev)
{
    dragX = ev.x; dragY = ev.y;

    if (this == getComponentAt (ev.getPosition()))
    {
        if (ev.x > mTrackWidth)
            timelineBodyClicked (ev, heights.trackAtY (ev.y));

        if (ev.x < mTrackWidth)
            timelineTrackHeadersClicked (ev, heights.trackAtY (ev.y));
    }
}

void
TimelineComponent::mouseDrag (const MouseEvent &ev)
{
    //if (ev.mods.isAltDown())
    {
        const int deltaX = ev.x - dragX;
        const int deltaY = ev.y - dragY;
        // DBG ("dx: " << deltaX << " dy: " << deltaY << " y: " << ev.y << " time: " << xToTime (ev.x));

        pixelOffset += deltaX;

        if (deltaY)
            setTrackHeightsOffset (deltaY, true);

        triggerAsyncUpdate();
    }

    dragX = ev.x;
    dragY = ev.y;
}

void
TimelineComponent::mouseUp (const MouseEvent &ev)
{
    triggerAsyncUpdate();
}

void
TimelineComponent::removeClips()
{
    if (freeClips.size())
        freeClips.clear();

    if (clips.size())
        clips.clear();
}

void TimelineComponent::scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart) { }
void TimelineComponent::sliderValueChanged (Slider* slider) { }

void
TimelineComponent::updateClip (TimelineClip* clip)
{
    jassert (clip);

    ClipRange<double> time;
    clip->getClipRange (time);

    const Range<int> hrange (trackHeight (clip->trackIndex()));
    clip->setBounds (timeToX (time.getStart(), clip->getTimeUnit()), hrange.getStart(),
                     timeToWidth (time, clip->getTimeUnit()), hrange.getLength());
}

void
TimelineComponent::valueChanged (Value &value)
{
    bool updateTimeScale = false;

    if (value.refersToSameSourceAs (tempo))
    {
        updateTimeScale = true;
        scale.setTempo ((double) tempo.getValue());
        scale.setPixelsPerBeat (60.f * ((double)scale.getTempo() / 120.f));
    }

    if (updateTimeScale)
        scale.updateScale();
}

/* Time Conversions */
int32
TimelineComponent::tickToX (const double tick) const
{
    return mTrackWidth + pixelOffset + scale.pixelFromTick (llrint (tick));
}

double
TimelineComponent::tickToTime (const double tick) const
{
    int64 frame = scale.frameFromTick (tick);
    return (double) frame / (double) scale.getSampleRate();
}

int32
TimelineComponent::beatToX (double beat) const
{
    return tickToX (beat * (double) scale.ticksPerBeat());
}

int32
TimelineComponent::timeToX (double time, const TimeUnit unit) const
{
    switch (unit)
    {
        case TimeUnit::Beats:   return beatToX (time); break;
        case TimeUnit::Frames:  return frameToX (time); break;
        case TimeUnit::Seconds: return secondsToX (time); break;
        case TimeUnit::Ticks:   return tickToX (time); break;
        default: break;
    }

    jassertfalse;
    return secondsToX (time);
}

int32
TimelineComponent::secondsToX (double time) const
{
    int64 frame = llrint (time * (double) scale.getSampleRate());
    int pixel = scale.pixelFromFrame (frame);
    return pixel + mTrackWidth + pixelOffset;
}

int32
TimelineComponent::frameToX (double f) const
{
    int pixel = scale.pixelFromFrame (llrint (f));
    return pixel + mTrackWidth + pixelOffset;
}

int64
TimelineComponent::xToFrame (int x) const
{
    normalX (x);
    return scale.frameFromPixel (x);
}

double
TimelineComponent::xToSeconds (int32 x) const
{
    normalX (x);
    return ((double) scale.frameFromPixel (x) / (double) scale.getSampleRate());
}

double
TimelineComponent::xToTicks (int32 x, bool snap) const
{
    normalX (x);
    return (double) snap ? scale.tickFromPixel (scale.pixelSnap (x))
                         : scale.tickFromPixel (x);
}

double
TimelineComponent::xToTime (int x, const TimeUnit unit) const
{
    switch (unit)
    {
        case TimeUnit::Beats:   return xToBeat (x); break;
        case TimeUnit::Frames:  return xToFrame (x); break;
        case TimeUnit::Seconds: return xToSeconds (x); break;
        case TimeUnit::Ticks:   return xToTicks (x); break;
        default: break;
    }

    jassertfalse;
    return xToSeconds (x);
}

double TimelineComponent::xToBeat (int x, bool snap) const
{
    return xToTicks (x) / (double) scale.ticksPerBeat();
}


int TimelineComponent::timeToWidth (const Range<double>& time, const TimeUnit unit) const
{
    return timeToX (time.getEnd(), unit) - timeToX (time.getStart(), unit);
}

int TimelineComponent::timeToWidth (const double& length, TimeUnit /*unit*/) const
{
    return scale.pixelFromFrame (scale.getSampleRate() * length);
}
