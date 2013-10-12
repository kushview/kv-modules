// TimeScale.cpp
//
/****************************************************************************
   Copyright (C) 2005-2012, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#include "element/time/timescale.hpp"

namespace Element {


void
TimeScale::reset()
{
    mNodes.setScoped (true);
    m_markers.setScoped (true);

	// Clear/reset location-markers...
    m_markers.clear();
    m_marker_cursor.reset();

	// Clear/reset tempo-map...
	mNodes.clear();
    mCursor.reset();

	// There must always be one node, always.
    addNode (0);

	// Commit new scale...
    updateScale();
}


// (Re)nitializer method.
void
TimeScale::clear (void)
{
    mSnapPerBeat = 4;
    mHorizontalZoom = 100;
    mVerticalZoom = 100;

    mDisplayFmt    = Frames;

    mSampleRate      = 44100;
    mTicksPerBeat   = 960;
    mPixelsPerBeat  = 32;

	// Clear/reset tempo-map...
	reset();
}


// Sync method.
void
TimeScale::sync (const TimeScale& ts)
{
	// Copy master parameters...
    mSampleRate     = ts.mSampleRate;
    mTicksPerBeat  = ts.mTicksPerBeat;
    mPixelsPerBeat = ts.mPixelsPerBeat;

	// Copy location markers...
	m_markers.clear();
    Marker *other_marker = ts.m_markers.first();
    while (other_marker)
    {
        m_markers.append (new Marker (*other_marker));
        other_marker = other_marker->next();
	}

    m_marker_cursor.reset();

	// Copy tempo-map nodes...
	mNodes.clear();
    Node *other_node = ts.nodes().first();
    while (other_node)
    {
        mNodes.append (new Node (this, other_node->frame,
                        other_node->tempo, other_node->beat_type,
                        other_node->beats_per_bar, other_node->beat_divisor));
        other_node = other_node->next();
	}

    mCursor.reset();
    updateScale();
}


// Copy method.
TimeScale&
TimeScale::copy (const TimeScale& ts)
{
    if (&ts != this)
    {
        mNodes.setScoped (true);
        m_markers.setScoped (true);

        mSampleRate      = ts.mSampleRate;
        mSnapPerBeat    = ts.mSnapPerBeat;
        mHorizontalZoom           = ts.mHorizontalZoom;
        mVerticalZoom           = ts.mVerticalZoom;
        mDisplayFmt      = ts.mDisplayFmt;

		// Sync/copy tempo-map nodes...
        sync (ts);
	}

	return *this;
}


// Update scale coefficient divisor factors.
void
TimeScale::Node::update()
{
    ticks_per_beat = ts->ticksPerBeat();
    tick_rate      = tempo * ticks_per_beat;
    beat_rate      = tempo;

    if (beat_divisor > beat_type)
    {
        unsigned short n = (beat_divisor - beat_type);
        ticks_per_beat >>= n;
        beat_rate *= float (1 << n);
    }
    else if (beat_divisor < beat_type)
    {
        unsigned short n = (beat_type - beat_divisor);
        ticks_per_beat <<= n;
        beat_rate /= float (1 << n);
	}
}


// Update time-scale node position metrics.
void
TimeScale::Node::reset (TimeScale::Node *node)
{
    if (bar > node->bar)
        frame = node->frameFromBar (bar);
	else
        bar = node->barFromFrame (frame);

    beat = node->beatFromFrame (frame);
    tick = node->tickFromFrame (frame);

    pixel = ts->pixelFromFrame (frame);
}


// Tempo accessor/convertors.
void
TimeScale::Node::setTempoEx (float extempo, unsigned short beattype_ex)
{
    if (beattype_ex > beat_type)
        extempo /= float (1 << (beattype_ex - beat_type));
    else if (beat_type > beattype_ex)
        extempo *= float (1 << (beat_type - beattype_ex));

    tempo = extempo;
}

float
TimeScale::Node::tempoEx (unsigned short beat_type_) const
{
    float extempo = tempo;

    if (beat_type > beat_type_)
        extempo /= float (1 << (beat_type - beat_type_));
    else if (beat_type_ > beat_type)
        extempo *= float (1 << (beat_type_ - beat_type));

    return extempo;
}


// Beat/frame snap filters.
unsigned long
TimeScale::Node::tickSnap (unsigned long tick_, unsigned short p) const
{
    unsigned long ticksnap = tick_ - tick;
    if (ts->snapPerBeat() > 0)
    {
        unsigned long q = ticks_per_beat / ts->snapPerBeat();
        ticksnap = q * ((ticksnap + (q >> p)) / q);
	}
    return tick + ticksnap;
}


// Time-scale cursor frame positioning reset.
void
TimeScale::Cursor::reset (TimeScale::Node *node)
{
    node = (node ? node : ts->nodes().first());
}


// Time-scale cursor node seeker (by frame).
TimeScale::Node*
TimeScale::Cursor::seekFrame (unsigned long iFrame)
{
    if (node == 0)
    {
		node = ts->nodes().first();
		if (node == 0)
			return 0;
	}

    if (iFrame > node->frame)
    {
		// Seek frame forward...
		while (node && node->next() && iFrame >= (node->next())->frame)
			node = node->next();
	}
    else if (iFrame < node->frame)
    {
		// Seek frame backward...
		while (node && node->frame > iFrame)
			node = node->prev();
		if (node == 0)
			node = ts->nodes().first();
	}

	return node;
}


// Time-scale cursor node seeker (by bar).
TimeScale::Node*
TimeScale::Cursor::seekBar (unsigned short sbar)
{
    if (node == 0)
    {
		node = ts->nodes().first();
		if (node == 0)
			return 0;
	}

    if (sbar > node->bar)
    {
		// Seek bar forward...
        while (node && node->next() && sbar >= (node->next())->bar)
			node = node->next();
	}
    else if (sbar < node->bar)
    {
		// Seek bar backward...
        while (node && node->bar > sbar)
			node = node->prev();
		if (node == 0)
			node = ts->nodes().first();
	}

	return node;
}


// Time-scale cursor node seeker (by beat).
TimeScale::Node*
TimeScale::Cursor::seekBeat (unsigned int sbeat)
{
	if (node == 0) {
		node = ts->nodes().first();
		if (node == 0)
			return 0;
	}

    if (sbeat > node->beat) {
		// Seek beat forward...
        while (node && node->next() && sbeat >= (node->next())->beat)
			node = node->next();
	}
	else
    if (sbeat < node->beat) {
		// Seek beat backward...
        while (node && node->beat > sbeat)
			node = node->prev();
		if (node == 0)
			node = ts->nodes().first();
	}

	return node;
}


// Time-scale cursor node seeker (by tick).
TimeScale::Node*
TimeScale::Cursor::seekTick (unsigned long stick)
{
    if (node == 0)
    {
		node = ts->nodes().first();
		if (node == 0)
			return 0;
	}

    if (stick > node->tick)
    {
		// Seek tick forward...
        while (node && node->next() && stick >= (node->next())->tick)
			node = node->next();
	}
    else if (stick < node->tick)
    {
		// Seek tick backward...
        while (node && node->tick > stick)
			node = node->prev();
		if (node == 0)
			node = ts->nodes().first();
	}

	return node;
}


// Time-scale cursor node seeker (by pixel).
TimeScale::Node*
TimeScale::Cursor::seekPixel (int px) const
{
	if (node == 0) {
		node = ts->nodes().first();
		if (node == 0)
			return 0;
	}

    if (px > node->pixel)
    {
		// Seek pixel forward...
        while (node && node->next() && px >= (node->next())->pixel)
			node = node->next();
	}
    else if (px < node->pixel)
    {
		// Seek tick backward...
        while (node && node->pixel > px)
			node = node->prev();
		if (node == 0)
			node = ts->nodes().first();
	}

	return node;
}


// Node list specifics.
TimeScale::Node*
TimeScale::addNode (unsigned long frame_, float tempo_, unsigned short beat_type_,
                     unsigned short beats_per_bar_, unsigned short beat_divisor_)
{
    Node *node	= 0;

	// Seek for the nearest preceding node...
    Node *prev = mCursor.seekFrame (frame_);
	// Snap frame to nearest bar...
    if (prev)
    {
        frame_ = prev->frameSnapToBar (frame_);
        prev   = mCursor.seekFrame (frame_);
	}

	// Either update existing node or add new one...
    Node *next = (prev ? prev->next() : 0);
    if (prev && prev->frame == frame_)
    {
		// Update exact matching node...
        node = prev;
        node->tempo = tempo_;
        node->beat_type = beat_type_;
        node->beats_per_bar = beats_per_bar_;
        node->beat_divisor = beat_divisor_;
    }
    else if (prev && prev->tempo == tempo_
                  && prev->beat_type == beat_type_
                  && prev->beats_per_bar == beats_per_bar_
                  && prev->beat_divisor == beat_divisor_)
    {
		// No need for a new node...
        return prev;
    }
    else if (next && next->tempo == tempo_
                  && next->beat_type == beat_type_
                  && next->beats_per_bar == beats_per_bar_
                  && next->beat_divisor == beat_divisor_)
    {
		// Update next exact matching node...
        node = next;
        node->frame = frame_;
        node->bar = 0;
    }
    else
    {
		// Add/insert a new node...
        node = new Node (this, frame_, tempo_, beat_type_, beats_per_bar_, beat_divisor_);
        if (prev)
            mNodes.insertAfter (node, prev);
		else
            mNodes.append (node);
	}

	// Update coefficients and positioning thereafter...
    updateNode (node);

    return node;
}


void
TimeScale::updateNode (TimeScale::Node *node)
{
	// Update coefficients...
    node->update();

	// Relocate internal cursor...
    mCursor.reset (node);

	// Update positioning on all nodes thereafter...
    Node *next = node;
    Node *prev = next->prev();
    while (next)
    {
        if (prev)
            next->reset (prev);

        prev = next;
        next = next->next();
	}

	// And update marker/bar positions too...
    updateMarkers (node->prev());
}


void
TimeScale::removeNode (TimeScale::Node *node)
{
	// Don't ever remove the very first node... 
    Node *node_prev = node->prev();
    if (node_prev == 0)
		return;

	// Relocate internal cursor...
    mCursor.reset(node_prev);

	// Update positioning on all nodes thereafter...
    Node *prev = node_prev;
    Node *next = node->next();
    while (next)
    {
        if (prev)
            next->reset (prev);

        prev = next;
        next = next->next();
	}

	// Actually remove/unlink the node...
    mNodes.remove (node);

	// Then update marker/bar positions too...
    updateMarkers (prev);
}



// Complete time-scale update method.
void
TimeScale::updateScale()
{
	// Update time-map independent coefficients...
    m_pixel_rate = 1.20f * float (mHorizontalZoom * mPixelsPerBeat);
    m_frame_rate = 60.0f * float (mSampleRate);

	// Update all nodes thereafter...
    Node *prev = 0;
    Node *next = mNodes.first();
    while (next)
    {
        next->update();
        if (prev)
            next->reset (prev);

        prev = next;
        next = next->next();
	}

	// Also update all marker/bar positions too...
    updateMarkers (mNodes.first());
}

#if 0
// Convert frames to time string and vice-versa.
unsigned long TimeScale::frameFromText (
	const QString& sText, bool bDelta, unsigned long iFrame )
{
	switch (m_displayFormat) {

		case BBT:
		{
			// Time frame code in bars.beats.ticks ...
			unsigned short bars  = sText.section('.', 0, 0).toUShort();
			unsigned int   beats = sText.section('.', 1, 1).toUInt();
			unsigned long  ticks = sText.section('.', 2).toULong();
			Node *pNode;
			if (bDelta) {
				pNode = m_cursor.seekFrame(iFrame);
				if (pNode)
					bars += pNode->bar;
			} else {
				if (bars > 0)
					--bars;
				if (beats > 0)
					--beats;
			}
			pNode = m_cursor.seekBar(bars);
			if (pNode) {
				beats += (bars - pNode->bar) * pNode->beatsPerBar;
				ticks += pNode->tick + beats * pNode->ticksPerBeat;
				iFrame = pNode->frameFromTick(ticks);
			}
			break;
		}

		case Time:
		{
			// Time frame code in hh:mm:ss.zzz ...
			unsigned int hh = sText.section(':', 0, 0).toUInt();
			unsigned int mm = sText.section(':', 1, 1).toUInt();
			float secs = sText.section(':', 2).toFloat();
			mm   += 60 * hh;
			secs += 60.f * float(mm);
			iFrame = uroundf(secs * float(m_iSampleRate));
			break;
		}

		case Frames:
		default:
		{
			iFrame = sText.toULong();
			break;
		}
	}

	return iFrame;
}


QString TimeScale::textFromFrame (
	unsigned long iFrame, bool bDelta, unsigned long iDelta )
{
	QString sText;

	switch (m_displayFormat) {

		case BBT:
		{
			// Time frame code in bars.beats.ticks ...
			unsigned short bars  = 0;
			unsigned int   beats = 0;
			unsigned long  ticks = 0;
			Node *pNode = m_cursor.seekFrame(iFrame);
			if (pNode) {
				if (bDelta) {
					ticks = pNode->tickFromFrame(iFrame + iDelta)
						  - pNode->tickFromFrame(iFrame);
				} else {
					ticks = pNode->tickFromFrame(iFrame)
						  - pNode->tick;
				}
				if (ticks >= (unsigned long) pNode->ticksPerBeat) {
					beats  = (unsigned int) (ticks / pNode->ticksPerBeat);
					ticks -= (unsigned long) (beats * pNode->ticksPerBeat);
				}
				if (beats >= (unsigned int) pNode->beatsPerBar) {
					bars   = (unsigned short) (beats / pNode->beatsPerBar);
					beats -= (unsigned int) (bars * pNode->beatsPerBar);
				}
				if (!bDelta)
					bars += pNode->bar;
			}
			if (!bDelta) {
				++bars;
				++beats;
			}
			sText.sprintf("%u.%u.%03lu", bars, beats, ticks);
			break;
		}

		case Time:
		{
			// Time frame code in hh:mm:ss.zzz ...
			unsigned int hh, mm, ss, zzz;
			float secs = float(bDelta ? iDelta : iFrame) / float(m_iSampleRate);
			hh = mm = ss = 0;
			if (secs >= 3600.0f) {
				hh = (unsigned int) (secs / 3600.0f);
				secs -= float(hh) * 3600.0f;
			}
			if (secs >= 60.0f) {
				mm = (unsigned int) (secs / 60.0f);
				secs -= float(mm) * 60.0f;
			}
			if (secs >= 0.0f) {
				ss = (unsigned int) secs;
				secs -= float(ss);
			}
			zzz = (unsigned int) (secs * 1000.0f);
			sText.sprintf("%02u:%02u:%02u.%03u", hh, mm, ss, zzz);
			break;
		}

		case Frames:
		default:
		{
			sText = QString::number(bDelta ? iDelta : iFrame);
			break;
		}
	}

	return sText;
}


// Convert ticks to time string and vice-versa.
unsigned long TimeScale::tickFromText (
	const QString& sText, bool bDelta, unsigned long iTick )
{
	unsigned long iFrame = 0;
	if (bDelta) {
		Node *pNode = m_cursor.seekTick(iTick);
		iFrame = (pNode ? pNode->frameFromTick(iTick) : 0);
	}
	return tickFromFrame(frameFromText(sText, bDelta, iFrame));
}


QString TimeScale::textFromTick (
	unsigned long iTick, bool bDelta, unsigned long iDelta )
{
	Node *pNode = m_cursor.seekTick(iTick);
	unsigned long iFrame = (pNode ? pNode->frameFromTick(iTick) : 0);
	if (bDelta > 0 && pNode) {
		iTick += iDelta;
		pNode  = m_cursor.seekTick(iTick);
		iDelta = (pNode ? pNode->frameFromTick(iTick) - iFrame : 0);
	}
	return textFromFrame(iFrame, bDelta, iDelta);
}
#endif

// Beat divisor (snap index) map.
static int s_snap_per_beat[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 21, 24, 28, 32, 48, 64, 96
};

const int c_snap_item_count = sizeof (s_snap_per_beat) / sizeof (int);

// Beat divisor (snap index) accessors.
unsigned short
TimeScale::snapFromIndex (int index)
{
    return s_snap_per_beat [index];
}


// Beat divisor (snap index) accessors.
int
TimeScale::indexFromSnap (unsigned short snap_per_beat)
{
    for (int snap = 0; snap < c_snap_item_count; ++snap) {
        if (s_snap_per_beat [snap] == snap_per_beat)
            return snap;
	}

	return 0;
}

#if 0
// Beat divisor (snap index) text item list.
QStringList TimeScale::snapItems ( int iSnap )
{
	QStringList items;

	if (iSnap == 0) {
		items.append(QObject::tr("None"));
		++iSnap;
	}

	QString sPrefix = QObject::tr("Beat");
	if (iSnap == 1) {
		items.append(sPrefix);
		++iSnap;
	}

	sPrefix += "/%1";
    while (iSnap < c_snap_item_count)
        items.append(sPrefix.arg(s_snap_per_beat[iSnap++]));

	return items;
}
#endif

// Tick/Frame range conversion (delta conversion).
unsigned long
TimeScale::frameFromTickRange ( unsigned long iTickStart, unsigned long iTickEnd )
{
    Node *pNode = mCursor.seekTick(iTickStart);
    unsigned long iFrameStart = (pNode ? pNode->frameFromTick(iTickStart) : 0);
    pNode = mCursor.seekTick(iTickEnd);
    unsigned long iFrameEnd = (pNode ? pNode->frameFromTick(iTickEnd) : 0);
	return (iFrameEnd > iFrameStart ? iFrameEnd - iFrameStart : 0);
}


unsigned long
TimeScale::tickFromFrameRange (unsigned long iFrameStart, unsigned long iFrameEnd)
{
    Node *pNode = mCursor.seekFrame(iFrameStart);
    unsigned long iTickStart = (pNode ? pNode->tickFromFrame(iFrameStart) : 0);
    pNode = mCursor.seekFrame(iFrameEnd);
    unsigned long iTickEnd = (pNode ? pNode->tickFromFrame(iFrameEnd) : 0);
	return (iTickEnd > iTickStart ? iTickEnd - iTickStart : 0);
}

// Location marker reset method.
void
TimeScale::MarkerCursor::reset (TimeScale::Marker *marker)
{
    marker = (marker ? marker : ts->markers().first());
}

// Location marker seek methods.
TimeScale::Marker*
TimeScale::MarkerCursor::seekFrame (unsigned long iFrame )
{
	if (marker == 0) {
		marker = ts->markers().first();
		if (marker == 0)
			return 0;
	}

    if (iFrame > marker->frame)
    {
		// Seek frame forward...
		while (marker && marker->next() && iFrame >= (marker->next())->frame)
			marker = marker->next();
    }
    else if (iFrame < marker->frame)
    {
		// Seek frame backward...
		while (marker && marker->frame > iFrame)
			marker = marker->prev();
		if (marker == 0)
			marker = ts->markers().first();
	}

	return marker;
}

TimeScale::Marker*
TimeScale::MarkerCursor::seekBar (unsigned short iBar )
{
    return seekFrame (ts->frameFromBar (iBar));
}

TimeScale::Marker*
TimeScale::MarkerCursor::seekBeat (unsigned int iBeat )
{
    return seekFrame (ts->frameFromBeat (iBeat));
}

TimeScale::Marker*
TimeScale::MarkerCursor::seekTick (unsigned long iTick)
{
    return seekFrame (ts->frameFromTick (iTick));
}

TimeScale::Marker*
TimeScale::MarkerCursor::seekPixel (int x)
{
    return seekFrame (ts->frameFromPixel (x));
}



// Location markers list specifics.
TimeScale::Marker*
TimeScale::addMarker (unsigned long target_frame, const std::string& txt,
                                                   const std::string& rgb )
{
    Marker *marker	= 0;

	// Snap to nearest bar...
    unsigned short nearest_bar = 0;

    Node *prev = mCursor.seekFrame (target_frame);
    if (prev)
    {
        nearest_bar  = prev->barFromFrame (target_frame);
        target_frame = prev->frameFromBar (nearest_bar);
	}

	// Seek for the nearest marker...
    Marker *nearest_marker = m_marker_cursor.seekFrame (target_frame);

	// Either update existing marker or add new one...
    if (nearest_marker && nearest_marker->frame == target_frame)
    {
		// Update exact matching marker...
        marker = nearest_marker;
        marker->bar = nearest_bar;
        marker->text = txt;
        marker->color = rgb;
    }
    else
    {
		// Add/insert a new marker...
        marker = new Marker(target_frame, nearest_bar, txt, rgb);

        if (nearest_marker && nearest_marker->frame > target_frame)
            m_markers.insertBefore (marker, nearest_marker);
        else if (nearest_marker && nearest_marker->frame < target_frame)
            m_markers.insertAfter (marker, nearest_marker);
		else
            m_markers.append (marker);
	}

	// Update positioning...
    updateMarker (marker);

    return marker;
}


void
TimeScale::updateMarker (TimeScale::Marker *pMarker)
{
	// Relocate internal cursor...
    m_marker_cursor.reset (pMarker);
}


void
TimeScale::removeMarker (TimeScale::Marker *pMarker)
{
	// Actually remove/unlink the marker
	// and relocate internal cursor...
	Marker *pMarkerPrev = pMarker->prev();
    m_markers.remove (pMarker);
    m_marker_cursor.reset (pMarkerPrev);
}


// Update markers from given node position.
void
TimeScale::updateMarkers (TimeScale::Node *pNode)
{
	if (pNode == 0)
		pNode = mNodes.first();
	if (pNode == 0)
		return;

    Marker *pMarker = m_marker_cursor.seekFrame(pNode->frame);
	while (pMarker) {
		while (pNode->next() && pMarker->frame > pNode->next()->frame)
			pNode = pNode->next();
		if (pMarker->frame >= pNode->frame)
            pMarker->frame  = pNode->frameFromBar (pMarker->bar);
		pMarker = pMarker->next();
	}
}

}  /* namespace element */

// end of TimeScale.cpp
