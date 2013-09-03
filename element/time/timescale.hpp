/****************************************************************************
   timescale.hpp

   Copyright (C) 2005-2012, rncbc aka Rui Nuno Capela. All rights reserved.
   Adapted/Modified 2013 Michael Fisher <mfisher31@gmail.com>

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

#ifndef ELEMENT_TIMESCALE_HPP
#define ELEMENT_TIMESCALE_HPP

#include <string>

#include "element/linked-list.hpp"
#include "element/util.hpp"

namespace element {

/** Time scaling helper class */
class TimeScale
{
public:

	// Available display-formats.
	enum DisplayFormat { Frames = 0, Time, BBT };

    TimeScale() : m_display_fmt (Frames), m_cursor (this), m_marker_cursor(this) { clear(); }
    TimeScale (const TimeScale& ts) : m_cursor (this), m_marker_cursor(this) { copy(ts); }
    TimeScale& operator=(const TimeScale& ts) { return copy(ts); }

    /** Reset the node list */
	void reset();

    /** Clear/sync/initialize the list */
	void clear();

    /** Sync timing values from another timescale */
    void sync (const TimeScale& ts);

    /** Copy the timescale.
        @param ts The timescale to copy from
        @note Also calls sync */
    TimeScale& copy (const TimeScale& ts);

	// Sample rate (frames per second)
    void setSampleRate (unsigned int rate) { m_sample_rate = rate; }
    unsigned int samplerate() const { return m_sample_rate; }

	// Resolution (ticks per quarter note; PPQN)
    void setTicksPerBeat (unsigned short ticks) { m_ticks_per_beat = ticks; }
    unsigned short ticksPerBeat() const { return m_ticks_per_beat; }

    // Pixels per beat (width).
    void setPixelsPerBeat (unsigned short ppb) { m_pixels_per_beat = ppb; }
    unsigned short pixelsPerBeat() const { return m_pixels_per_beat; }

	// Beat divisor (snap) accessors.
    void setSnapPerBeat (unsigned short snap) { m_snap_per_beat = snap; }
    unsigned short snapPerBeat() const { return m_snap_per_beat; }

	// Horizontal zoom factor.
    void setHorizontalZoom (unsigned short iHorizontalZoom) { m_h_zoom = iHorizontalZoom; }
    unsigned short horizontalZoom() const { return m_h_zoom; }

	// Vertical zoom factor.
    void setVerticalZoom (unsigned short vzoom) { m_v_zoom = vzoom; }
    unsigned short verticalZoom() const { return m_v_zoom; }

	// Fastest rounding-from-float helper.
    static unsigned long uroundf (float x) { return (unsigned long) (x >= 0.0f ? x + 0.5f : x - 0.5f); }

	// Beat divisor (snap index) accessors.
    static unsigned short snapFromIndex (int index);
    static int indexFromSnap (unsigned short snap);

#if 0
	// Beat divisor (snap index) text item list.
    static QStringList snapItems (int iSnap = 0);
#endif

	// Time scale node declaration.
    class Node : public LinkedList<Node>::Link
	{
	public:

		// Constructor.
        Node (TimeScale *timescale, unsigned long frame_ = 0, float tempo_ = 120.0f,
              unsigned short beattype_ = 2, unsigned short beats_per_bar_ = 4,
              unsigned short beat_divisor_ = 2)
            : frame(frame_), bar(0), beat(0), tick(0), pixel(0),
              tempo(tempo_), beat_type (beattype_),
              beats_per_bar (beats_per_bar_),
              beat_divisor (beat_divisor_),
              ticks_per_beat(0), ts (timescale),
              tick_rate (1.0f), beat_rate (1.0f)
        { }

		// Update node scale coefficients.
		void update();

		// Update node position metrics.
        void reset (Node *node);

		// Tempo accessor/convertors.
        void setTempoEx (float fTempo, unsigned short iBeatType = 2);
        float tempoEx (unsigned short iBeatType = 2) const;

		// Frame/bar convertors.
        unsigned short barFromFrame (unsigned long iFrame) const { return bar + uroundf ((beat_rate * (iFrame - frame)) / (ts->frameRate() * beats_per_bar)); }
        unsigned long frameFromBar (unsigned short iBar) const { return frame + uroundf ((ts->frameRate() * beats_per_bar * (iBar - bar)) / beat_rate); }

		// Frame/beat convertors.
        unsigned int beatFromFrame (unsigned long iFrame) const { return beat + uroundf ((beat_rate * (iFrame - frame)) / ts->frameRate()); }
        unsigned long frameFromBeat (unsigned int iBeat) const { return frame + uroundf ((ts->frameRate() * (iBeat - beat)) / beat_rate); }

		// Frame/tick convertors.
        unsigned long tickFromFrame (unsigned long iFrame) const { return tick + uroundf ((tick_rate * (iFrame - frame)) / ts->frameRate()); }
        unsigned long frameFromTick (unsigned long iTick) const { return frame + uroundf((ts->frameRate() * (iTick - tick)) / tick_rate); }

		// Tick/beat convertors.
        unsigned int beatFromTick (unsigned long iTick) const { return beat + ((iTick - tick) / ticks_per_beat); }
        unsigned long tickFromBeat (unsigned int iBeat) const { return tick + (ticks_per_beat * (iBeat - beat)); }

		// Tick/bar convertors.
        unsigned short barFromTick (unsigned long iTick) const { return bar + ((iTick - tick) / (ticks_per_beat * beats_per_bar)); }
        unsigned long tickFromBar (unsigned short iBar) const { return tick + (ticks_per_beat * beats_per_bar * (iBar - bar)) ; }

		// Tick/pixel convertors.
        unsigned long tickFromPixel (int x) const { return tick + uroundf ((tick_rate * (x - pixel)) / ts->pixelRate()); }
        int pixelFromTick (unsigned long iTick) const { return pixel + uroundf ((ts->pixelRate() * (iTick - tick)) / tick_rate); }

		// Beat/pixel convertors.
        unsigned int beatFromPixel (int x) const { return beat + uroundf ((beat_rate * (x - pixel)) / ts->pixelRate()); }
        int pixelFromBeat (unsigned int b) const { return pixel + uroundf ((ts->pixelRate() * (b - beat)) / beat_rate); }

		// Pixel/beat rate convertor.
        unsigned short pixelsPerBeat() const { return uroundf(ts->pixelRate() / beat_rate); }

		// Bar/pixel convertors.
        unsigned short barFromPixel (int x) const { return bar + uroundf ((beat_rate * (x - pixel)) / (ts->pixelRate() * beats_per_bar)); }
        int pixelFromBar (unsigned short b) const { return pixel + uroundf ((ts->pixelRate() * beats_per_bar * (b - bar)) / beat_rate); }

		// Bar/beat convertors.
        unsigned short barFromBeat (unsigned int iBeat) const { return bar + ((iBeat - beat) / beats_per_bar); }
        unsigned int beatFromBar (unsigned short iBar) const  { return beat + (beats_per_bar * (iBar - bar)); }

        bool beatIsBar (unsigned int iBeat) const { return ((iBeat - beat) % beats_per_bar) == 0; }

		// Frame/bar quantizer.
        unsigned long frameSnapToBar (unsigned long frame) const { return frameFromBar (barFromFrame (frame)); }

		// Beat snap filters.
        unsigned long tickSnap (unsigned long tick_, unsigned short p = 1) const;
        unsigned long frameSnap (unsigned long frame_) const { return frameFromTick (tickSnap (tickFromFrame (frame))); }
        int pixelSnap (int x) const { return pixelFromTick (tickSnap (tickFromPixel (x))); }

		// Node keys.
		unsigned long  frame;
		unsigned short bar;
		unsigned int   beat;
		unsigned long  tick;
		int            pixel;

		// Node payload.
		float          tempo;
        unsigned short beat_type;
        unsigned short beats_per_bar;
        unsigned short beat_divisor;

        unsigned short ticks_per_beat;

	protected:

		// Node owner.
		TimeScale *ts;

		// Node cached coefficients.
        float tick_rate;
        float beat_rate;
	};

	// Node list accessor.
    const LinkedList<Node>& nodes() const { return m_nodes; }

	// To optimize and keep track of current frame
	// position, mostly like an sequence cursor/iterator.
	class Cursor
	{
	public:

        Cursor (TimeScale *scale) : ts(scale), node(0) { }
        TimeScale *timescale() const { return ts; }

        void reset (Node *node = 0);

        Node* seekFrame (unsigned long frame);
        Node* seekBar   (unsigned short bar);
        Node* seekBeat  (unsigned int beat);
        Node* seekTick  (unsigned long tick);
        Node* seekPixel (int x);

	protected:

		TimeScale *ts;
		Node *node;
	};

	Cursor& cursor() { return m_cursor; }

	// Node list specifics.
    Node *addNode (unsigned long iFrame = 0, float fTempo = 120.0f,
                   unsigned short iBeatType = 2, unsigned short iBeatsPerBar = 4,
                   unsigned short iBeatDivisor = 2);

    void updateNode (Node *node);
    void removeNode (Node *node);

    /** Update date the timescale as a whole */
    void updateScale();

	// Frame/pixel convertors.
    int pixelFromFrame (unsigned long frame) const { return uroundf((m_pixel_rate * frame) / m_frame_rate); }
    unsigned long frameFromPixel (int x) const { return uroundf((m_frame_rate * x) / m_pixel_rate); }

	// Frame/bar general converters.
    unsigned short
    barFromFrame (unsigned long frame)
	{
        Node *node = m_cursor.seekFrame (frame);
        return (node ? node->barFromFrame (frame) : 0);
	}

    unsigned long
    frameFromBar (unsigned short bar)
	{
        Node *node = m_cursor.seekBar (bar);
        return (node ? node->frameFromBar (bar) : 0);
	}

	// Frame/beat general converters.
    unsigned int
    beatFromFrame (unsigned long frame)
	{
        Node *node = m_cursor.seekFrame (frame);
        return (node ? node->beatFromFrame (frame) : 0);
	}

    unsigned long
    frameFromBeat (unsigned int iBeat)
	{
        Node *node = m_cursor.seekBeat(iBeat);
        return (node ? node->frameFromBeat(iBeat) : 0);
	}

	// Frame/tick general converters.
    unsigned long
    tickFromFrame (unsigned long iFrame)
	{
        Node *node = m_cursor.seekFrame (iFrame);
        return (node ? node->tickFromFrame (iFrame) : 0);
	}

    unsigned long
    frameFromTick (unsigned long iTick)
	{
        Node *node = m_cursor.seekTick(iTick);
        return (node ? node->frameFromTick(iTick) : 0);
	}

	// Tick/pixel general converters.
    unsigned long
    tickFromPixel (int x)
	{
        Node *node = m_cursor.seekPixel (x);
        return (node ? node->tickFromPixel (x) : 0);
	}

    int
    pixelFromTick (unsigned long tick)
	{
        Node *node = m_cursor.seekTick (tick);
        return (node ? node->pixelFromTick (tick) : 0);
	}

	// Beat/pixel composite converters.
    unsigned int
    beatFromPixel (int x)
	{
        Node *node = m_cursor.seekPixel(x);
        return (node ? node->beatFromPixel(x) : 0);
	}

    int
    pixelFromBeat (unsigned int iBeat)
	{
        Node *node = m_cursor.seekBeat (iBeat);
        return (node ? node->pixelFromBeat(iBeat) : 0);
	}

	// Bar/beat predicate.
    bool
    beatIsBar (unsigned int beat)
	{
        Node *node = m_cursor.seekBeat (beat);
        return (node ? node->beatIsBar (beat) : false);
	}

	// Snap functions.
    unsigned long
    tickSnap (unsigned long iTick)
	{
        Node *node = m_cursor.seekTick(iTick);
        return (node ? node->tickSnap(iTick) : iTick);
	}

    unsigned long
    frameSnap (unsigned long frame)
	{
        Node *node = m_cursor.seekFrame (frame);
        return (node ? node->frameSnap (frame) : frame);
	}

    int
    pixelSnap (int x)
	{
        Node *node = m_cursor.seekPixel(x);
        return (node ? node->pixelSnap (x) : x);
	}

	// Display-format accessors.
    void set_display_format (DisplayFormat displayFormat) { m_display_fmt = displayFormat; }
    DisplayFormat display_format() const { return m_display_fmt; }

#if 0
	// Convert frames to time string and vice-versa.
    QString textFromFrame (unsigned long iFrame, bool bDelta = false, unsigned long iDelta = 0);
    unsigned long frameFromText (const QString& sText, bool bDelta = false, unsigned long iFrame = 0);

	// Convert ticks to time string and vice-versa.
    QString textFromTick (unsigned long iTick, bool bDelta = false, unsigned long iDelta = 0);
    unsigned long tickFromText (const QString& sText, bool bDelta = false, unsigned long iTick = 0);
#endif

    /** Set the tempo
        @param tempo The tempo to use (BPM) */
    void
    setTempo (float tempo)
	{
        if (Node *node = m_nodes.first())
            node->tempo = tempo;
	}

    /** Return the timescale's current tempo (at the first node) */
    float
    tempo() const
	{
        Node *node = m_nodes.first();
        return (node ? node->tempo : 120.0f);
	}

	// Tempo convertors (default's quarter notes per minute)
    void
    setTempoEx (float tempo, unsigned short beat_type = 2)
	{
        Node *node = m_nodes.first();
        if (node) node->setTempoEx (tempo, beat_type);
	}

    float
    tempoEx (unsigned short iBeatType = 2) const
	{
        Node *node = m_nodes.first();
        return (node ? node->tempoEx (iBeatType) : 120.0f);
	}

	// Tempo beat type (if not standard 2=quarter note)
    void
    setBeatType (unsigned short beat_type)
	{
        Node *node = m_nodes.first();
        if (node) node->beat_type = beat_type;
	}

    unsigned short
    beatType() const
	{
        Node *node = m_nodes.first();
        return (node ? node->beat_type : 2);
	}

    /** Set beats per bar (time signature numerator)
        @param bpb The time signature's numerator */
    void
    setBeatsPerBar (unsigned short bpb)
	{
        if (Node *node = m_nodes.first())
            node->beats_per_bar = bpb;
	}

    unsigned short
    beatsPerBar() const
	{
        Node *node = m_nodes.first();
        return (node ? node->beats_per_bar : 4);
	}

	// Time signature (denominator)
    void
    setBeatDivisor (unsigned short divisor)
	{
        Node *node = m_nodes.first();
        if (node) node->beat_divisor = divisor;
	}

    unsigned short
    beatDivisor() const
	{
        Node *node = m_nodes.first();
        return (node ? node->beat_divisor : 2);
	}

	// Tick/Frame range conversion (delta conversion).
    unsigned long frameFromTickRange (unsigned long tick_start, unsigned long tick_end);
    unsigned long tickFromFrameRange (unsigned long frame_start, unsigned long frame_end);

#if 1
	// Location marker declaration.
    class Marker : public LinkedList<Marker>::Link
	{
	public:

		// Constructor.
        Marker (unsigned long iFrame, unsigned short iBar,
                const std::string& sText, const std::string& rgbColor = std::string("#545454"))
            : frame (iFrame), bar(iBar), text(sText), color (rgbColor) { }

		// Copy constructor.
        Marker (const Marker& marker) : frame (marker.frame),
            bar (marker.bar), text(marker.text), color(marker.color) { }

		// Marker keys.
		unsigned long  frame;
		unsigned short bar;

		// Marker payload.
        std::string text;
        std::string color;
	};

	// To optimize and keep track of current frame
	// position, mostly like an sequence cursor/iterator.
	class MarkerCursor
	{
	public:

		// Constructor.
        MarkerCursor (TimeScale *tscale) : ts(tscale), marker(0) { }

		// Time scale accessor.
		TimeScale *timeScale() const { return ts; }

		// Reset method.
        void reset (Marker *marker = 0);

		// Seek methods.
        Marker *seekFrame (unsigned long iFrame);
        Marker *seekBar (unsigned short iBar);
        Marker *seekBeat (unsigned int iBeat);
        Marker *seekTick (unsigned long iTick);
        Marker *seekPixel (int x);

		// Notable markers accessors
		Marker *first() const { return ts->m_markers.first(); }
		Marker *last()  const { return ts->m_markers.last();  }

	protected:

		// Member variables.
		TimeScale *ts;
		Marker *marker;
	};

	// Markers list accessor.
    MarkerCursor& markers() { return m_marker_cursor; }

	// Marker list specifics.
    Marker *addMarker (unsigned long iFrame, const std::string& text,
                                              const std::string& color = std::string ("#000000"));
    void updateMarker (Marker *marker);
    void removeMarker (Marker *marker);

	// Update markers from given node position.
    void updateMarkers (Node *n);

#endif

protected:

	// Tempo-map independent coefficients.
    float pixelRate() const { return m_pixel_rate; }
    float frameRate() const { return m_frame_rate; }

private:

    unsigned short m_snap_per_beat;    ///< Snap per beat (divisor).
    unsigned short m_h_zoom;           ///< Horizontal zoom factor.
    unsigned short m_v_zoom;           ///< Vertical zoom factor.

    DisplayFormat  m_display_fmt;      ///< Textual display format.

    unsigned int   m_sample_rate;      ///< Sample rate (frames per second)
    unsigned short m_ticks_per_beat;   ///< Tticks per quarter note (PPQN)
    unsigned short m_pixels_per_beat;  ///< Pixels per beat (width).

	// Tempo-map node list.
    LinkedList<Node> m_nodes;

	// Internal node cursor.
	Cursor m_cursor;

	// Tempo-map independent coefficients.
    float m_pixel_rate;
    float m_frame_rate;

	// Location marker list.
    LinkedList<Marker> m_markers;

	// Internal node cursor.
    MarkerCursor m_marker_cursor;
};

} /* namespace element */

#endif	// ELEMENT_TIMESCALE_HPP


// end of TimeScale.h
