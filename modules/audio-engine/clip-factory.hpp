/*
    clip-factory.hpp - This file is part of Element
    Copyright (C) 2013  Michael Fisher <mfisher@bketech.com>

    Element is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Element is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Element; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef ELEMENT_CLIP_FACTORY_HPP
#define ELEMENT_CLIP_FACTORY_HPP

#include "clip.hpp"
#include "clip-bin.hpp"

namespace element {

    class Session;

    class ClipFactory
    {
    public:

        ClipFactory (Session&);
        ~ClipFactory();
        ClipRef get();

    protected:

    private:

        friend class Clip;
        friend class ClipBin;

        Session& m_sess;
        ClipBin  m_free;

#if 0
        typedef  TrackBufferThread                     Thread;
        typedef  std::vector<std::shared_ptr<Thread> > Threads;
        Threads  m_threads;
        uint32_t m_max_threads;
        uint32_t m_thread_index;
#endif

        ClipRef create();
        void recycle (Clip* clip);
    };

}

#endif
