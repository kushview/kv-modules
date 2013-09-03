/*
    ringbuffer.cpp - This file is part of Element
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

#include "element/ringbuffer.hpp"

namespace element {

RingBuffer::RingBuffer (uint32 capacity)
    : fifo (0), buffer (nullptr)
{
    setCapacity (capacity);
}

RingBuffer::~RingBuffer()
{
    fifo.reset();
    buffer = nullptr;
    block.free();
}

void
RingBuffer::setCapacity (uint32 newCapacity)
{
    size_t capacity = 1;
    while (capacity < newCapacity)
        capacity *= 2;

    block.allocate (capacity, true);
    buffer = block.getData();

    fifo.setTotalSize ((int) capacity);
    fifo.reset();
}

}
