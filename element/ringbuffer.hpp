/*
    ringbuffer.hpp - This file is part of Element
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
/** @file ringbuffer.hpp */

#ifndef ELEMENT_RINGBUFFER_HPP
#define ELEMENT_RINGBUFFER_HPP

#include "core.hpp"

namespace element {

    class RingBuffer
    {
    public:

        RingBuffer (uint32 capacity);
        ~RingBuffer();

        void setCapacity (uint32 newCapacity);

        inline bool canRead  (uint32 bytes) const  { return bytes <= (uint32) fifo.getNumReady() && bytes != 0; }
        inline uint32 readSpace() const { return fifo.getNumReady(); }

        inline bool canWrite (uint32 bytes) const { return bytes <= (uint32) fifo.getFreeSpace() && bytes != 0; }
        inline uint32 writeSpace() const { return (uint32) fifo.getFreeSpace(); }

        inline uint32 size() const { return (uint32) fifo.getTotalSize(); }

        inline uint32
        write (const char* src, uint32 bytes)
        {
            buffer = block.getData();
                       
            fifo.prepareToWrite (bytes, vec1.index, vec1.size, vec2.index, vec2.size);

            if (vec1.size > 0)
                memcpy (buffer + vec1.index, src, vec1.size);

            if (vec2.size > 0)
                memcpy (buffer + vec2.index, src + vec1.size, vec2.size);

            fifo.finishedWrite (vec1.size + vec2.size);

            return vec1.size + vec2.size;
        }

        inline uint32 write (const void* src, uint32 bytes)
        {
           return write ((const char*) src, bytes);
        }

        template <typename T>
        uint32 writeType (const T& src)
        {
            write ((char*) &src, sizeof (T));
        }

        inline uint32
        read (char* dest, uint32 size, bool advance = true)
        {
            buffer = block.getData();
            
            fifo.prepareToRead (size, vec1.index, vec1.size, vec2.index, vec2.size);

            if (vec1.size > 0)
                memcpy (dest, buffer + vec1.index, vec1.size);

            if (vec2.size > 0)
                memcpy (dest + vec1.size, buffer + vec2.index, vec2.size);

            if (advance)
                fifo.finishedRead (vec1.size + vec2.size);

            return vec1.size + vec2.size;
        }

        inline uint32
        read (void* dest, uint32 size, bool advance = true)
        {
           return read ((char*) dest, size, advance);
        }

        struct Vector {
            uint32_t size;
            void*    buffer;
        };
#if 0
        void getReadVector (Vector* vec)
        {
            int i1, b1, i2, b2;
            fifo.getReadVector (i1, b1, i2, b2);

            char* buf = (char*) block.getData();

            vec[0].size   = (uint32_t) b1;
            vec[0].buffer = (void*) (buf + i1);
            vec[1].size   = (uint32_t) b2;
            vec[1].buffer = (void*) (buf + i2);
        }

        void getWriteVector (Vector* vec)
        {
            int i1, b1, i2, b2;
            fifo.getWriteVector (i1, b1, i2, b2);

            char* buf = (char*) block.getData();

            vec[0].size   = (uint32_t) b1;
            vec[0].buffer = (void*) (buf + i1);
            vec[1].size   = (uint32_t) b2;
            vec[1].buffer = (void*) (buf + i2);
        }

        void readAdvance (uint32_t bytes)  { fifo.finishedRead ((int) bytes); }
        void writeAdvance (uint32_t bytes) { fifo.finishedWrite ((int) bytes); }
#endif
        CriticalSection& getReadLock() { return readLock; }
        CriticalSection& getWriteLock() { return writeLock; }

    private:

        struct Vec
        {
            int size;
            int index;
        };

        Vec vec1, vec2;
        AbstractFifo fifo;
        HeapBlock<char> block;
        char* buffer;
        CriticalSection readLock, writeLock;
    };
}

#endif /* ELEMENT_RINGBUFFER_HPP */
