/*
    sequence-cursor.hpp - This file is part of Element
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

#ifndef ELEMENT_SEQUENCE_CURSOR_HPP
#define ELEMENT_SEQUENCE_CURSOR_HPP

#include <raul/DoubleBuffer.hpp>

#include "element/engine/track.hpp"
#include "element/engine/sequence.hpp"
#include "element/types.h"

namespace element {

class Clip;
class Sequence;

class SequenceCursor
{
public:

	// Constructor.
    SequenceCursor (Sequence& seq, uint64_t frame = 0, int syncType = 0);
    ~SequenceCursor();

    /** Bi-directional locate
        @param frame The audio frame to seek
        @param sync If true clips will reset and/or seek to frame
     */
    void seek (uint64_t frame, bool sync = false);

    /** Current clip
        Returns the current clip for a given track
     */
    Clip* clip (uint32_t track) const;

    /** Returns the current frame (playhead) index */
    uint64_t frame() const;


    void setFrameTime (uint64_t frame);
    uint64_t frameTime() const;
    uint64_t setFrameTimeExpected() const;

    void setSyncType (int sync);
    int syncType() const;

    void addTrack();
    void removeTrack (const TrackPtr& track);
    void updateTrack (const TrackPtr& track);
    void updateTrackClip (const TrackPtr& track);
    uint32_t numTracks() const { return m_ntracks.get(); }

    // Reset cursor.
    void reset();

    // Reset track/clips cache.
    void resetClips();

    // Frame-time processor (increment only).
    void process (uint32_t nframes);

protected:

    Signal destroyed;

    typedef std::unique_ptr<Clip * []> ClipArray;

    Clip* seekClip (const TrackPtr& track, Clip* clip, int64_t frame) const;
    void updateClips (Clip** clips, uint32_t total_tracks);
    void removeTrack (uint32_t track);

private:

	// Instance variables.
    Sequence&      m_seq;

    uint64_t       m_frame;
    uint64_t       m_frame_time;
    uint64_t       m_frame_delta;
    int            m_sync_type;

    Raul::DoubleBuffer<uint32_t> m_ntracks;
    ClipArray  rt_clips;
    uint32_t   m_size;
};

}

#endif  /* ELEMENT_SEQUENCE_CURSOR_HPP */
