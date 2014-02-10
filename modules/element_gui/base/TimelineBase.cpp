/*
    TimelineBase.cpp - This file is part of Element
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

    void
    TimelineIndicator::setPosition (double time, bool musical)
    {
        if (musical) {
            const float bpm = (float) timeline()->timeScale().tempo();
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

    TimelineBase*
    TimelineIndicator::timeline() const
    {
        if (owner == nullptr)
        {
            owner = findParentComponentOfClass<TimelineBase>();
            if (owner == nullptr) {
                jassertfalse; // this needs to be owned by a timeline base object
            }
        }

        return owner;
    }

    double
    TimelineIndicator::getUnits() const
    {
        return timeline()->xToTime (getBoundsInParent().getX());
    }


    void
    TimelineBase::recycleClip (TimelineClip* clip)
    {
        clips.removeObject (clip, false);
        removeChildComponent (clip);
        freeClips.add (clip);
    }

    void
    TimelineBase::handleAsyncUpdate()
    {
        for (TimelineClip* clip : clips)
        {
            if (clip->isShowing()) {
                updateClip (clip);

                clip->setBounds (clip->getBounds());
                clip->repaint();
            }
        }

        repaint();
    }

    TimelineBase::TimelineBase()
    {
        heights.ensureTracks (512, TrackHeights::Mini);
        heights.setOffset (0);

        mTrackWidth = 0;
        mMaxTrackWidth = 200;

        scale.setSampleRate (44100);
        scale.setTempo (120.f);
        scale.setTicksPerBeat (1920);
        scale.updateScale();

        pixPerUnit = (double) scale.pixelsPerBeat();

        timeSpan.setLength (4.0);
        addAndMakeVisible (playheadIndicator = new TimelineIndicator);
        playheadIndicator->setAlwaysOnTop (true);

        tempo.addListener (this);
        scale.setDisplayFormat (TimeScale::BBT);
    }

    TimelineBase::~TimelineBase()
    {
        tempo.removeListener (this);
    }

    void
    TimelineBase::addTimelineClip (TimelineClip* clip, int track)
    {
        if (clip == nullptr)
            return;

        freeClips.removeObject (clip, false);
        addAndMakeVisible (clip);
        clips.addIfNotAlreadyThere (clip);
        // clip->setTrackIndex (track, true);
        updateClip (clip);
    }


    void
    TimelineBase::paint (Graphics& g)
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
            r.setX (0);
            r.setWidth (mTrackWidth);
            r.setY (heights.trackY (track));
            r.setHeight (heights.get (track));

            if (r.getY() > getHeight() || track > getNumTracks())
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
    }

    void
    TimelineBase::resized()
    {

    }

    double
    TimelineBase::getMajorTickSize()
    {
        return 0.0f;
    }



    void
    TimelineBase::mouseDown (const MouseEvent &ev)
    {
        if (this == getComponentAt (ev.getPosition()))
        {
            if (ev.x > mTrackWidth)
                timelineBodyClicked (ev, heights.trackAtY (ev.y));

            if (ev.x < mTrackWidth)
                timelineTrackHeadersClicked (ev, heights.trackAtY (ev.y));
        }
    }

    void
    TimelineBase::mouseDrag (const MouseEvent &ev)
    {

    }

    void
    TimelineBase::mouseUp (const MouseEvent &ev)
    {

    }

    void
    TimelineBase::removeClips()
    {
        freeClips.clear();
        clips.clear();
    }

    void TimelineBase::scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart) { }
    void TimelineBase::sliderValueChanged (Slider* slider) { }

    void
    TimelineBase::updateClip (TimelineClip* clip)
    {
        assert (clip);
        addAndMakeVisible (clip);

        Range<double> time;
        clip->getTime (time);

        //if (clip->timeIsMusical())
        {
#if 0
            double s = 1.0f / (double) Shuttle::PPQ;
            double start = s * time.getStart();
            double end   = s * time.getEnd();
            double tempo = (double) scale.tempo();

            time.setStart (60.f / tempo * start);
            time.setEnd   (60.f / tempo * end);
#endif
        }

        const Range<int> hrange (trackHeight (clip->trackIndex()));
        clip->setBounds (timeToX (time.getStart()), hrange.getStart(),
                         timeToWidth (time), hrange.getLength());
    }

    void
    TimelineBase::valueChanged (Value &value)
    {
        bool updateTimeScale = false;

        if (value.refersToSameSourceAs (tempo))
        {
            updateTimeScale = true;
            scale.setTempo ((double) tempo.getValue());
            scale.setPixelsPerBeat (60.f * ((double)scale.tempo() / 120.f));
        }

        if (updateTimeScale)
            scale.updateScale();
    }
