/*
    range.hpp - This file is part of Element
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

#ifndef ELEMENT_RANGE_HPP
#define ELEMENT_RANGE_HPP

#include "element/core.hpp"

namespace Element {

template<typename T>
class FrameSpan : public Range<T>
{

public:

    FrameSpan() : m_in (0), m_len (0) { }
    FrameSpan (T in, T out) { setRange (in, out); }

    inline T
    inPoint() const
    {
        return m_in;
    }

    inline T
    outPoint() const
    {
        return m_in + m_len;
    }

    inline T
    length() const
    {
        return m_len;
    }

    inline bool
    isZero() const
    {
        return m_len == 0;
    }

    inline bool
    isInverted() const
    {
        return outPoint() < inPoint();
    }

    inline void
    reverse()
    {
        setRange (outPoint(), inPoint());
    }

    inline void
    setLength (T len)
    {
        m_len = len;
    }

    inline void
    setRange (T in, T out)
    {
        setInPoint (in);
        setOutPoint (out);
    }

    inline void
    setInPoint (T in)
    {
        m_in = in;
    }

    inline void
    setOutPoint (T out)
    {
        m_len = out - m_in;
    }

    inline FrameSpan<T>&
    operator=(const FrameSpan<T>& other)
    {
        m_in  = other.m_in;
        m_len = other.m_len;
        return *this;
    }

private:

    T m_in;
    T m_len;

};
}
#endif // ELEMENT_RANGE_HPP
