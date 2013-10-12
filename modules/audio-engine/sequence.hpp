/*
    sequence.hpp

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

#ifndef LIB_SEQUENCE_HPP
#define LIB_SEQUENCE_HPP

#include "element/asset.hpp"
#include "element/atomic.hpp"
#include "element/pointer.hpp"
#include "element/processor.hpp"
#include "element/signal.hpp"

#include "element/engine/clip-factory.hpp"
#include "element/engine/track.hpp"
#include "element/time/timescale.hpp"

namespace element {

class Session;
class SequenceCursor;

/** A container for tracks */
class Sequence
{
public:

    typedef TrackList::iterator track_iterator;

    Signal changed;

    ~Sequence();

    static Sequence* create (Session& sess, const URI& uri);

    SequenceCursor* createCursor (uint32_t frame = 0, uint32_t type = 0);
    void removeCursor (SequenceCursor* cursor);

    Session& session() { return m_sess; }

    const std::string& name() const { return m_props.name; }
    void setName (const std::string& name) { m_props.name = name; }

    const URI& uri() const { return m_props.uri; }

    TimeScale& timescale() { return m_props.scale; }
    uint32_t sampleRate() const;
    void setSampleRate (uint32_t sampleRate);

    inline TrackList&     tracks()             { return m_tracks; }
    inline track_iterator tracksBegin()        { return m_tracks.begin(); }
    inline track_iterator tracksEnd()          { return m_tracks.end(); }
    inline uint32_t       numTracks()  const   { return m_tracks.size(); }

    TrackPtr appendTrack();
    void insertTrack (const TrackPtr& prev, const TrackPtr& track);
    void moveTrack   (const TrackPtr& prev, const TrackPtr& track);
    void updateTrack (const TrackPtr& track);

    struct Properties
    {
        Properties();
        void clear();

        Properties& operator= (const Properties&);

        std::string name;
        URI         uri;
        TimeScale   scale;
    };

protected:

    Sequence (Session&, const URI& name);

private:

    typedef std::vector<SequenceCursor*> CursorVec;

    AtomicLock   m_lock;
    Session&     m_sess;
    Properties   m_props;
    TrackList    m_tracks;
    CursorVec    m_cursors;

    friend class Session;

};

typedef Shared<Sequence> SequencePtr;

}

#endif
