/*
    sequence-cursor.cpp

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

#if 0
#include "element/session.hpp"
#include "element/engine/clip.hpp"
#include "element/engine/sequence-cursor.hpp"


namespace element {

// Constructor.
SequenceCursor::SequenceCursor (Sequence& seq, uint64_t frame, int sync_type)
    : m_seq (seq),
      m_ntracks (0)
{
    m_frame      = frame;
    m_sync_type  = sync_type;
    m_size       = 0;

    resetClips();
	reset();
}


// Destructor.
SequenceCursor::~SequenceCursor()
{
    destroyed();
    m_seq.removeCursor (this);
    rt_clips.reset();
}


// Clip sync flag accessor.
void
SequenceCursor::setSyncType (int sync)
{
    m_sync_type = sync;
}

int
SequenceCursor::syncType() const
{
    return m_sync_type;
}


// General bi-directional locate method.
void
SequenceCursor::seek (uint64_t frame, bool sync)
{
#if 1
    if (frame == m_frame)
        return;

    TrackList::const_iterator ti = m_seq.tracks().begin();
    TrackList::const_iterator te = m_seq.tracks().end();

    const uint32_t ntracks = (uint32_t) numTracks();
    uint32_t index = 0;

    while (ti != te && index < ntracks)
    {
        const TrackPtr& track (*ti);
        Clip* clip      = 0;
        Clip* clip_last = rt_clips [index];

        // Optimize if seeking forward...
        if (frame > m_frame)
            clip = clip_last;

        // Locate first clip not past the target frame position..
        clip = seekClip (track, clip, frame);

        // Update cursor track clip...
        rt_clips [index] = clip;

#if 1
        // Now something fulcral for clips around...
        // FIXME: if (track->sync_type() == m_sync_type)
        {
            // Tell whether play-head is after loop-start position...
            // FIXME: bool is_looping = (frame >= p_seq->loop_start());
            const bool is_looping = frame > 0;

            // Care for old/previous clip...
            if (clip_last && clip_last != clip)
            {
                clip_last->reset (is_looping);
            }

            // Set final position within target clip...
            if (clip && sync)
            {
                // Take care of overlapping clips...
                const uint64_t clip_end = clip->frame_end();

                Clip::iterator c = clip->position();
                Clip::iterator e = track->clips_end();
                while (c != e)
                {
                    Clip& clip (*c);

                    uint64_t clip_start = clip.frame_start();

                    if (clip_start > clip_end)
                    {
                        break;
                    }

                    if (frame >= clip_start && frame < clip_start + clip.duration())
                    {
                        clip.seek (frame - clip_start);
                    }
                    else
                    {
                        clip.reset (is_looping);
                    }

                    ++c;
                }
            }
        }
#endif
        ++ti; ++index;
	}

    m_frame = frame;
#endif
}

uint64_t
SequenceCursor::frame() const
{
    return m_frame;
}

void
SequenceCursor::setFrameTime (uint64_t frame_time)
{
    m_frame_time = frame_time;
    m_frame_delta = m_frame - frame_time;
}

uint64_t
SequenceCursor::frameTime (void) const
{
    return m_frame_time;
}

uint64_t
SequenceCursor::setFrameTimeExpected() const
{
    return m_frame_time + m_frame_delta;
}


Clip*
SequenceCursor::clip (uint32_t track) const
{
    return (track < m_ntracks.get() ? rt_clips [track] : 0);
}

Clip*
SequenceCursor::seekClip (const TrackPtr& track, Clip* clip, int64_t frame) const
{
    if (! clip)
        clip = track->clips().size() > 0 ? &(*track->clips_begin())
                                     : nullptr;
    //std::clog << "seek sz: " << track->clips().size() << std::endl;
    while (clip != nullptr)
    {
        //std::clog << "seek clip frame: " << clip->duration() << std::endl;
        if (frame >= clip->frame_start() &&
            frame <  clip->frame_end())
        {
           // std::clog << "cursor clip\n";
            return clip;
        }

        clip = clip->next();
    }
    return 0;
}

void
SequenceCursor::addTrack()
{
#ifndef NDEBUG_1
    //std::clog << "SequenceCursor::add_track()\n";
#endif

    const uint32_t track_cnt = m_ntracks.get() + 1;

    if (m_size < track_cnt)
    {
        m_size += track_cnt;
        ClipArray new_clips (new Clip* [m_size]);
        updateClips (new_clips.get(), track_cnt);

        //XXX: m_seq.seession().lock();
        rt_clips.swap (new_clips);
        //XXX: m_seq.seession().unlock();
    }
    else
    {
        //XXX: m_seq.seession().lock();
        updateClips (rt_clips.get(), track_cnt);
        //XXX: m_seq.seession().unlock();
    }

    if (! m_ntracks.set (track_cnt))
        { }

#ifndef NDEBUG
     //std::clog << "\tnum tracks: " << m_ntracks.get() << std::endl;
#endif
}

void
SequenceCursor::updateTrack (const TrackPtr& track)
{
#ifndef NDEBUG
  //  std::clog << "SequenceCursor::update_track()" << std::endl;
#endif

    const int index = track->index();
    if (index >= 0 && index < numTracks())
    {
        Clip* clip = seekClip (track, 0, m_frame);
        if (clip && m_frame >= clip->frame_start() &&
                m_frame <  clip->frame_end())
        {
            clip->seek (m_frame - clip->frame_start());
        }

        rt_clips [index] = clip;
    }
}

void
SequenceCursor::removeTrack (const TrackPtr& track)
{
    const int index = track->index();
    if (index >= 0 && uint32_t (index) < numTracks())
        removeTrack ((uint32_t) index);
}

void
SequenceCursor::removeTrack (uint32_t track)
{
#if 1
    if (m_ntracks.set (m_ntracks.get() - 1))
    {
        //XXX: m_seq.seession().lock();
        for ( ; track < numTracks(); ++track)
            rt_clips [track] = rt_clips [track + 1];
        rt_clips [track] = 0;
        //XXX: m_seq.seession().unlock();
    }
#endif
}

void
SequenceCursor::updateTrackClip (const TrackPtr& track)
{
    const int index = track->index();
    if (index >= 0)
    {
        Clip* clip = rt_clips [index];
#if 1
        if (clip)
        {
            if (m_frame >= clip->frame_start() &&
                m_frame <  clip->frame_end())
            {
                clip->seek (m_frame - clip->frame_start());
            }
            else
            {
                // FIXME: clip->reset (m_frame >= p_seq->loopStart());
			}
		}
#endif
	}
}

void
SequenceCursor::updateClips (Clip** clips, uint32_t ntracks)
{
    // Reset clip positions...
    TrackList::const_iterator it  = m_seq.tracks().begin();
    TrackList::const_iterator end = m_seq.tracks().end();

    int cnt = 0;

    while (it != end && cnt < ntracks)
    {
        const TrackPtr& track (*it);
        Clip* clip = seekClip (track, 0, m_frame);

        if (clip && m_frame >= clip->frame_start() &&
                    m_frame <  clip->frame_end())
        {
            clip->seek (m_frame - clip->frame_start());
        }

        clips [cnt++] = clip;
        ++it;
    }
}

void
SequenceCursor::reset()
{
    m_frame_time = 0;
    m_frame_delta = m_frame;
}

void
SequenceCursor::resetClips()
{
    const uint32_t ntracks = m_seq.numTracks();
    m_size = (ntracks << 1);

    if (m_size > 0)
    {
        ClipArray new_clips (new Clip* [m_size]);
        updateClips (new_clips.get(), ntracks);

        //XXX: m_seq.seession().lock();
        rt_clips.swap (new_clips);
        //XXX: m_seq.seession().unlock();
    }

    m_ntracks.set (ntracks);
}

void
SequenceCursor::process (unsigned int nframes)
{
    m_frame_time += nframes;
}

}
#endif
