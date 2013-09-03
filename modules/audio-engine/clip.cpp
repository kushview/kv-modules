/*
    clip.cpp

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
#include "element/engine/clip.hpp"
#include "element/engine/sequence.hpp"

namespace element {

Clip::Clip (ClipFactory& f)
    : m_factory (f),
      m_gain (1, 8),
      list (nullptr)
{
    m_refs.store (0);
    set_track (nullptr);
}


// Default constructor.
Clip::~Clip()
{   
    assert (! is_linked());
    this->list = nullptr;
    set_track   (nullptr);
}

List<Clip>&
Clip::container() const
{
    assert (list);
    return *list;
}

void
Clip::set_track (Track* new_track)
{
    Track* t = nullptr;
    do {
        t = a_track.load();
    } while (! a_track.compare_exchange_weak (t, new_track));
}


int
Clip::track_index() const
{
    Track* t  = a_track.load();
    return t != 0  ? t->index() : -1;
}

bool
Clip::is_orphan() const
{
    return track() == nullptr;
}

Track*
Clip::track() const
{
    return a_track.load();
}

Clip*
Clip::next()
{
    // do this without iterators ?
    iterator it = me; ++it;
    Clip* n = it != container().end() ? &(*it) : nullptr;
    return (track() != nullptr) ? n : nullptr;
}

Clip*
Clip::prev()
{
    // do this without iterators ?
    if (! track())
        return nullptr;
    if (me == (*list).begin())
        return nullptr;
    iterator it = me;
    return &(*(--it));
}

void
Clip::ref()
{
    ++m_refs;
}

void
Clip::deref()
{
    if (--m_refs == 0)
    {
        // if it's linked else where then it doesn't need recycled
        if (! is_linked())
            m_factory.recycle (this);
    }
}

void intrusive_ptr_add_ref (Clip* clip)
{
    clip->ref();
}

void intrusive_ptr_release (Clip* clip)
{
    clip->deref();
}

} /* namespace element */
#endif
