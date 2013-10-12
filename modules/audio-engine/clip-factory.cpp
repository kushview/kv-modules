/*
    clip-factory.cpp - This file is part of Element
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
#include "element/engine/clip-factory.hpp"

using namespace std;

namespace element {


    class AudioClip : public Clip
    {
    public:

        AudioClip (ClipFactory& f) : Clip(f) { }

        virtual ~AudioClip() { }

        virtual void open()  { }
        virtual void close() { }
        virtual void seek (uint64_t) { }
        virtual void process (uint32_t start, uint32_t end) { }
    };

    ClipFactory::ClipFactory (Session& s)
        : m_free (*this), m_sess (s)
    { }

    ClipFactory::~ClipFactory()
    {
        m_free.m_clips.clear_and_dispose (Element::clip_dispose);
#if 0
        for (int i =0; i < m_max_threads; ++i)
        {
            const shared_ptr<Thread>& t (m_threads [i]);
            if (t && t->isRunning())
            {
                do {
                    std::clog << "ClipFactory(): stopping thread: " << i + 1 << std::endl;
                    t->set_run_state (false);
                    t->sync();
                } while (! t->wait (100));
            }
        }
#endif

    }

    ClipRef
    ClipFactory::get ()
    {
        ClipRef ret (create());

        assert (ret);
        assert (! ret->is_linked());
        assert (ret->list == nullptr);

        ret->set_frame_start (0);
        if (ret->duration() < 1)
            ret->set_duration (44100);

        return ret;
    }

    ClipRef ClipFactory::create()
    {
        ClipRef clip = nullptr;

        std::clog << "ClipFactory::create(): num free: " << m_free.m_clips.size() << std::endl;
        if (! m_free.m_clips.empty())
        {
            std::clog << "ClipFactory::create(): taking existing clip\n";
            clip.reset (&m_free.m_clips.back());
            m_free.m_clips.erase (m_free.m_clips.iterator_to (*clip));
            clip->list = nullptr;
        }

        if (! clip)
        {
            std::clog << "ClipFactory::create(): create new clip\n";
            clip.reset (new AudioClip (*this));
            clip->list = nullptr;
        }

        assert (clip);
        std::clog << " size: " << m_free.size() << std::endl;
        return clip;
    }

    void
    ClipFactory::recycle (Clip* clip)
    {
        std::string msg = "";
        msg += std::string ("free clips: ");
        std::clog << msg << m_free.m_clips.size() << std::endl;
        if (clip->is_linked())
        {
            msg = "recycled linked clip: ";
            m_free.m_clips.splice (m_free.begin(),
                                   clip->container(),
                                   clip->me);
            clip->me = m_free.m_clips.iterator_to (*clip);
        }
        else
        {
            msg = "recycled unlinked clip: ";
            clip->me = m_free.m_clips.insert (m_free.begin(), *clip);
        }


        assert (clip->is_linked());

        clip->set_track (nullptr);
        clip->list = &m_free.m_clips;
        clip->close();
    }
#if 0
    std::shared_ptr<TrackBufferThread>
    ClipFactory::test_thread()
    {
        std::shared_ptr<Thread> thrd (m_threads [m_thread_index]);

        if (! thrd) {
            thrd = m_threads [m_thread_index] = std::make_shared<Thread> (128);
        }

        thrd->start (Thread::HighPriority);

        if (++m_thread_index == m_max_threads)
            m_thread_index = 0;

        return thrd;
    }
#endif
}
#endif
