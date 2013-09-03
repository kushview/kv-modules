/*
    sequence.cpp

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
#include <boost/foreach.hpp>

#include "element/pointer.hpp"
#include "element/engine/sequence.hpp"
#include "element/engine/sequence-cursor.hpp"
#include "element/session.hpp"
#include "element/engine/track.hpp"

namespace element {


    Sequence::Properties::Properties() { }
    void Sequence::Properties::clear() { }


    Sequence::Sequence (Session& s, const URI& uri)
        : m_sess(s), m_tracks (*this)
    {
        m_props.uri = uri;
    }

    Sequence::~Sequence()
    { }

    Sequence*
    Sequence::create (Session& sess, const URI& uri)
    {
        Sequence* seq = new Sequence (sess, uri);
        URI u (uri);
#if 0
        if (u.is_file())
        {
            seq->set_name (fs::path (u.local_file()).leaf().c_str());
        }
        else if (uri.scheme() == "session")
        {
            seq->set_name ("Untitled");
        }
#endif
        return seq;
    }


    SequenceCursor*
    Sequence::createCursor (uint32_t frame, uint32_t type)
    {
        SequenceCursor* cursor = new SequenceCursor (*this, frame, type);
        m_cursors.push_back (cursor);
        cursor->resetClips();
        return cursor;
    }


    void
    Sequence::removeCursor (SequenceCursor* cursor)
    {
        element::removeall (m_cursors, cursor);
    }


    uint32_t
    Sequence::sampleRate() const
    {
        return m_props.scale.samplerate();
    }

    void
    Sequence::setSampleRate (uint32_t samplerate)
    {
        m_props.scale.setSampleRate (samplerate);
    }

    TrackPtr
    Sequence::appendTrack()
    {
        TrackPtr t = m_tracks.append_track();
        if (t)
            for (auto cursor : m_cursors)
                cursor->addTrack();
        return t;
    }

    void
    Sequence::insertTrack (const TrackPtr& prev, const TrackPtr& track)
    {
        if (m_tracks.insert_after (prev, track))
            for (auto cursor : m_cursors)
                cursor->addTrack();
    }

    void
    Sequence::moveTrack (const TrackPtr& prev, const TrackPtr& track)
    { }

    void
    Sequence::updateTrack (const TrackPtr& track)
    {
        for (auto* cursor : m_cursors)
            cursor->updateTrack (track);
    }
}
#endif
