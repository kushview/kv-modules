/*
    This file is part of the element modules for the JUCE Library
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

#ifndef EL_MONITOR_H_INCLUDED
#define EL_MONITOR_H_INCLUDED

class Monitor
{
public:

    Monitor (const Array<int>& p, const int pt)
        : path (p), port (pt), value (0.0f)
    { }

    ~Monitor() { }

    const Array<int> path;
    const int port;

    const int procNode() const { return *path.end(); }

    /** The non-realtime thread should call this at regular intervals */
    inline float get() const {
        return value.get();
    }

    /** The audio thread should call this on a regular basis to update
        with a realtime value.  e.g. audio peaks, transport position, etc.. */
    inline void set (const float val) {
        value.set (val);
    }

private:

    AtomicValue<float> value;

};

#endif // EL_MONITOR_H
