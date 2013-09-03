/*
    list.hpp

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


#ifndef ELEMENT_LIST_HPP
#define ELEMENT_LIST_HPP

#include <boost/intrusive/list.hpp>
#include <boost/intrusive/slist_hook.hpp>

namespace element {

    /** templates for creating fast linked lists.
        'most' list operations happen in constant time */

    typedef boost::intrusive::slist_base_hook<>   SListHook;

    typedef boost::intrusive::list_base_hook<>   LinkStandard;
    typedef boost::intrusive::list_base_hook<
                boost::intrusive::link_mode<
                boost::intrusive::auto_unlink> > LinkAutoUnlink;


    /** A linked listed.
        This is a wrapper around a boost::intrusive::list
        The template param N must be some kind of boost::intrusive::list_x_hook */
    template <class N>
    class List
        : public boost::intrusive::list<N>
    {
    public:

        typedef boost::intrusive::list<N>   Container;
        typedef boost::intrusive::list<N>   list_type;
        typedef typename list_type::const_iterator const_iterator;
        typedef typename list_type::iterator iterator;

#if 1
        inline int
        indexOf (N const& node) const
        {
            int index = -1;
            const_iterator nd   = this->iterator_to (node);
            const_iterator iter = this->begin();
            while (iter != nd) { ++nd; ++index; }
            return index;
        }

        /** linear time */
        inline N*
        at (int index)
        {
            const_iterator b = this->begin();
            const_iterator e = this->end();

            int c = 0;

            //TODO: do a split search based on proposed index
            for (const_iterator i = b; i != e; ++i)
            {
                if (index == c)
                    return &(*i);
                ++c;
            }

            return 0;
        }
    #endif

    };


    class Link : public LinkStandard
    {
    public:

        Link() { }
        virtual ~Link() { }

    };
}

#endif  // ELEMENT_LIST_HPP
