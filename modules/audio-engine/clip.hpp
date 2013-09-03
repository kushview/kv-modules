/*
    clip.hpp

    Copyright (C) 2013  Michael Fisher <mfisher@bketech.com>

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

#ifndef ELEMENT_CLIP_HPP
#define ELEMENT_CLIP_HPP

#include <atomic>
#include <boost/intrusive_ptr.hpp>

#include "element/asset.hpp"
#include "element/list.hpp"
#include "element/signal.hpp"
#include "element/uri.hpp"

#include "element/util.hpp"
#include "element/util/fraction.hpp"
#include "element/util/range.hpp"

namespace element {

    class Clip;
    class ClipFactory;
    class Track;

    class Clip : public Link
    {
    public:

        typedef List<Clip> list_type;
        typedef typename list_type::iterator iterator;
        typedef typename list_type::const_iterator const_iterator;
        typedef typename list_type::node_algorithms node_algorithms;

        class Position : public FrameSpan<uint32_t>
        {
        public:

            Position() : m_offset(0) { }
            ~Position() { }
            uint32_t offset() const { return m_offset; }
            void set_offset (const uint32_t offset) { m_offset = offset; }

            Position& operator= (const Position& other)
            {
                FrameSpan<uint32_t>::operator = (other);
                m_offset = other.m_offset;
                return *this;
            }

        private:

            uint32_t m_offset;
        };

        explicit Clip (ClipFactory&);
        virtual ~Clip();

        const AssetID& asset() const { return m_asset; }
        String uri() const { return m_asset.uri(); }
        AssetType type() const { return m_asset.type(); }

        const Position& time() const { return m_time; }
        uint32_t duration()    const { return m_time.length(); }
        uint32_t frame_start() const { return m_time.inPoint(); }
        uint32_t frame_end()   const { return m_time.outPoint(); }
        uint32_t offset()      const { return m_time.offset(); }

        void set_time (const Position& pos) { m_time = pos; }
        void set_duration (uint32_t dur) { m_time.setLength (dur); }
        void set_frame_start (uint32_t start) { m_time.setInPoint (start); }
        void set_frame_end (uint32_t end) { m_time.setOutPoint(end); }
        void set_offset (uint32_t offset) { m_time.set_offset (offset); }

        bool is_orphan() const;
        Track* track() const;
        int track_index() const;

        iterator       position()       { return me; }
        const_iterator position() const { return me; }

        virtual void open()  = 0;
        virtual void close() = 0;
        virtual void seek (uint64_t audio_frame) =0;
        virtual void process (uint32_t start, uint32_t end) =0;
        virtual void reset (bool) { }

        inline static size_t
        hash_value (const Clip& clip)
        {
            size_t seed = 0;
            element::chained_hash (seed, clip.asset().hash());
            element::chained_hash (seed, clip.frame_start());
            element::chained_hash (seed, clip.frame_end());
            element::chained_hash (seed, clip.offset());
            return seed;
        }

        size_t hash_value() const { return Clip::hash_value (*this); }

        void ref();
        void deref();

        Clip* next();
        Clip* prev();


    protected:

        ClipFactory& factory() { return m_factory; }

    private:

        iterator        me;
        AssetID         m_asset;
        ClipFactory&    m_factory;
        Position        m_time;
        FractionalGain  m_gain;

        /** @internal - Track hook reference */
        std::atomic<Track*> a_track;

        /** @internal - The clips container */
        list_type* list;

        /** @internal - Get this clips parent container */
        list_type& container() const;

        /** @internal - Set the parent track */
        void set_track (Track*);

        std::atomic<unsigned> m_refs;

        friend class Track;
        friend class ClipBin;
        friend class ClipFactory;
    };

    typedef boost::intrusive_ptr<Clip> ClipRef;
    void intrusive_ptr_add_ref (Clip*);
    void intrusive_ptr_release (Clip*);

    inline void clip_dispose (Clip* clip) { clip->close(); delete clip; }

}

#endif
