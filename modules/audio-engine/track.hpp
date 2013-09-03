/*
    track.hpp - This file is part of Element
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


#ifndef ENGINE_TRACK_HPP
#define ENGINE_TRACK_HPP

#include <list>

#include "element/types.h"

#include "clip.hpp"
#include "clip-bin.hpp"


namespace element {

class Clip;
class Sequence;
class Track;
class TrackList;

typedef Shared<Track> TrackPtr;

/** Track container.
    This is the main track model. A track maintains a list of Clips */
class Track
{
public:

    Signal changed;

    typedef std::list<Shared<Track> >  list_type;

    Track (Sequence& seq, int type, int index);
    ~Track();

    ClipBin&       clips()       { return m_clips; }
    Clip::iterator clips_begin() { return m_clips.begin(); }
    Clip::iterator clips_end()   { return m_clips.end(); }
    void clear();

    Clip* create_clip (const AssetID& asset, uint32_t frame = 0);
    bool  accept_clip (ClipRef clip);
    void  remove_clip (ClipRef clip);


    ClipRef first() const;

    Sequence& sequence() const;

    int sync_type() const;
    int index() const;

    std::string name() const { return m_props.name; }

    struct Properties
    {
        Properties() { clear(); }
        std::string name;
        Properties& operator=(const Properties& o)
        {
            name = o.name;
            return *this;
        }

        inline void
        clear()
        {
            name = "Track";
        }
    };

    inline const Properties& properties() const { return m_props; }
    inline void
    set_properties (const Properties& props)
    {
        m_props = props;
        changed();
    }

    static TrackPtr null;

private:

    Sequence&  m_seq;
    ClipBin    m_clips;

    list_type::iterator me;
    Properties m_props;

    int        m_sync_type;
    int        m_index;

    friend class Clip;
    friend class TrackList;

    void accept_clip_internal (ClipRef clip);
    void insert_clip (ClipRef clip);
};


/** A list of tracks in a sequence
    @note Implemented in @file track-list.cpp */
class TrackList : public std::list<Shared<Track> >
{
public:

    TrackList (Sequence& seq);
    ~TrackList();

    TrackPtr first();
    TrackPtr last();

    /** Append a new track to the end of the list
        @return The newly created track or nullptr */
    TrackPtr append_track();

    TrackPtr create (int, int);
    bool insert_after (const TrackPtr& prev, const TrackPtr& track);

    iterator iterator_to (const Track& track);
    iterator iterator_to (const TrackPtr& track);

    void ensure_tracks (uint32_t cnt);
    int sync_type() const;

private:

    Sequence&    m_seq;
    friend class Track;

};

}

#endif  /* ENGINE_TRACK_HPP */
