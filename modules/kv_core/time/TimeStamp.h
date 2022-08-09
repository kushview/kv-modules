/*
    This file is part of the Kushview Modules for JUCE
    Copyright (c) 2014-2019  Kushview, LLC.  All rights reserved.

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

#pragma once

namespace kv {

class TimeUnit
{
public:
    enum ID
    {
        Beats   = 0,
        Frames  = 1,
        Seconds = 2,
        Ticks   = 3,
        UserDefined = 4
    };

    inline TimeUnit() : type (Seconds) { }
    inline TimeUnit (const TimeUnit& o) : type (o.type) { }
    inline TimeUnit (const TimeUnit::ID& ot) : type (ot) { }
    inline operator ID() const { return getType(); }
    inline bool operator== (const TimeUnit& o) const { return o.type == type; }
    inline bool operator!= (const TimeUnit& o) const { return o.type != type; }
    inline ID getType() const { return juce::isPositiveAndBelow (type, (int32) UserDefined) ? (ID) type : UserDefined; }
    inline int32 getTypeId() const { return type; }

private:
    int32 type;
};

}
