/*
    RangeTypes.h - This file is part of Element
    Copyright (C) 2014  Kushview, LLC.  All rights reserved.

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

#ifndef ELEMENT_RANGE_TYPES_H
#define ELEMENT_RANGE_TYPES_H

#include <stdint.h>

class FrameSpan :  public Range<int64_t>
{

public:

    inline FrameSpan (int64_t i, int64_t o) : Range<int64_t> (i, o) { }
    inline ~FrameSpan() { }
    inline int64_t inPoint()  const { return getStart(); }
    inline int64_t outPoint() const { return getEnd(); }
    inline int64_t length() const { return getLength(); }
    inline bool isZero() const { return getLength() == 0; }
    inline void setInPoint (const int64_t in) { setStart (in); }
    inline void setOutPoint (const int64_t out) { setEnd (out); }

private:

};

#endif // ELEMENT_RANGE_TYPES_H
