/*
    track.cpp - This file is part of Element
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
#include <iostream>

#include "element/engine/clip.hpp"
#include "element/engine/sequence.hpp"
#include "element/engine/track.hpp"
#include "element/session.hpp"

namespace element {

static bool
clip_compare_frame_start (const Clip& lhs, const Clip& rhs)
{
    return lhs.frame_start() < rhs.frame_start();
}

TrackPtr Track::null = TrackPtr();

Track::Track (Sequence &seq, int type, int index)
    : m_seq (seq),
      m_clips (seq.session().clips()),
      m_sync_type (type),
      m_index (index)
{ }

Track::~Track()
{
    clear();
}

bool
Track::accept_clip (ClipRef clip)
{
    accept_clip_internal (clip);
    return (clip->track() && clip->is_linked());
}

void
Track::remove_clip (ClipRef clip)
{
    if (clip->track() != this)
        std::clog << "warning: removing un-affiliated track clip\n"
                  << "   " << clip->uri() << std::endl;

    clip->close();
    clip->set_track (nullptr);

    ClipBin remove (m_seq.session().clips());
    m_seq.session().lock();
    remove.insertSorted (clip);
    m_seq.session().unlock();

    remove.clear();
}

ClipRef
Track::first() const
{
    if (m_clips.size()) {
        return ClipRef (const_cast<Clip*> (&(*m_clips.begin())));
    }
    return ClipRef (nullptr);
}

Clip*
Track::create_clip (const AssetID& asset, uint32_t frame)
{
    return nullptr;
}

void
Track::accept_clip_internal (ClipRef clip)
{
    insert_clip (clip);
    clip->set_track (this);
    clip->open();
}

void
Track::clear()
{
    std::clog << "Track::clear(): size: " << m_clips.size() << std::endl;
    m_clips.clear();
}

void
Track::insert_clip (ClipRef clip)
{
    m_seq.session().lock();
    m_clips.insertSorted (clip);
    m_seq.session().unlock();
}

int
Track::index() const
{
    return m_index;
}

Sequence&
Track::sequence() const
{
    return m_seq;
}

int
Track::sync_type() const
{
    return m_sync_type;
}

}
#endif
