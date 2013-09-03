/*
    track-list.cpp

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

#include <boost/foreach.hpp>
#include "track.hpp"
#if 0
namespace element {

TrackList::TrackList (Sequence& seq) : m_seq (seq) { }
TrackList::~TrackList() { }

TrackPtr
TrackList::append_track()
{
    TrackPtr track (create (0, size()));
    push_back (track);
    track->me = iterator_to (track);
    return back();
}

TrackPtr
TrackList::create (int type, int index)
{
    return std::make_shared<Track> (m_seq, type, index);
}

void
TrackList::ensure_tracks (uint32_t cnt)
{
    while (size() <= cnt)
        append_track();
}

TrackPtr
TrackList::first()
{
    if (size() == 0)
        return Track::null;
    return front();
}

TrackPtr
TrackList::last()
{
    if (size() == 0)
        return Track::null;
    return back();
}

bool
TrackList::insert_after (const TrackPtr& prev, const TrackPtr& track)
{
    assert (track);
    if (0 == prev)
    {
        track->me = insert (end(), track);
        track->m_index = 0;
    }
    else
    {
        iterator pit = prev->me;
        track->me = insert (++pit, track);
        int index = prev->index();
#if 1
        TrackList::const_iterator iter = track->me;
        while (iter != end())
        {
            const TrackPtr& t (*iter);
            t->m_index = ++index;
            ++iter;
        }
#endif
    }

    return true;
}


TrackList::iterator
TrackList::iterator_to (const Track& track)
{
    if (empty())
        return end();

    if (track.index() < size() / 2)
    {
        iterator it = begin();
        while (it != end())
        {
            if ((*it).get() == &track)
                return it;
            ++it;
        }
    }
    else
    {
        iterator it = end();
        while (it != begin())
        {
            --it;

            if ((*it).get() == &track)
                return it;
        }
    }

    return end();
}

TrackList::iterator
TrackList::iterator_to (const TrackPtr& track)
{
    return iterator_to (*track);
}

} /* namespace element */
#endif
