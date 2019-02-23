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

#ifndef EL_PORT_WRITER_H
#define EL_PORT_WRITER_H

#if KV_USE_CXX11
/** Function type for writing to a port buffer. Params are in order port, size, protocol, buffer
    @note This is compatible with lvtk's port write ui method */
typedef std::function<void(uint32_t, uint32_t, uint32_t, void const*)> PortWriteFunction;
#endif

/** A simple type for writing/reading port values/messages through a ringbuffer */
struct PortEvent
{
    uint32 index;       ///< The port index
    uint32 protocol;    ///< The port protocol
    union {
        double decimal; ///< Timestamp as a decimal, units depends on context
        int64 frames;   ///< Timestamp in audio frames
    } time;
    uint32 size;        ///< The size of data
};


#endif  /* EL_PORT_WRITER_H */
