/*
    This file is part of the element modules for the JUCE Library
    Copyright (c) 2015 - Michael Fisher <mfisher31@gmail.com>.

    Permission to use, copy, modify, and/or distribute this software for any purpose with
    or without fee is hereby granted, provided that the above copyright notice and this
    permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
    TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
    NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
    DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
    IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
    CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef ELEMENT_JUCE_RINGBUFFER_H
#define ELEMENT_JUCE_RINGBUFFER_H

class RingBuffer
{

public:
    
    RingBuffer (int32 capacity);
    ~RingBuffer();

    void setCapacity (int32 newCapacity);
    inline size_t size() const { return (size_t) fifo.getTotalSize(); }
    
    inline bool canRead  (uint32 bytes) const { return bytes <= (uint32) fifo.getNumReady() && bytes != 0; }
    inline uint32 getReadSpace() const { return fifo.getNumReady(); }

    inline bool canWrite (uint32 bytes) const { return bytes <= (uint32) fifo.getFreeSpace() && bytes != 0; }
    inline uint32 getWriteSpace() const { return (uint32) fifo.getFreeSpace(); }

    inline uint32
    peak (void* dest, uint32 size)
    {
        return read (dest, size, false);
    }
    
    inline uint32
    read (void* dest, uint32 size, bool advance = true)
    {
        buffer = block.getData();
        fifo.prepareToRead (size, vec1.index, vec1.size, vec2.index, vec2.size);
        
        if (vec1.size > 0)
            memcpy (dest, buffer + vec1.index, vec1.size);
        
        if (vec2.size > 0)
            memcpy ((uint8*) dest + vec1.size, buffer + vec2.index, vec2.size);
        
        if (advance)
            fifo.finishedRead (vec1.size + vec2.size);
        
        return vec1.size + vec2.size;
    }
    
    template <typename T>
    inline uint32 read (T& dest)
    {
        return read (&dest, sizeof (T));
    }
    
    inline void advanceReadPointer (const uint32) {
        jassertfalse;
    }

    inline uint32
    write (const void* src, uint32 bytes)
    {
        buffer = block.getData();  
        fifo.prepareToWrite (bytes, vec1.index, vec1.size, vec2.index, vec2.size);

        if (vec1.size > 0)
            memcpy (buffer + vec1.index, src, vec1.size);

        if (vec2.size > 0)
            memcpy (buffer + vec2.index, (uint8*)src + vec1.size, vec2.size);

        fifo.finishedWrite (vec1.size + vec2.size);
        return vec1.size + vec2.size;
    }

    template <typename T>
    inline uint32 write (const T& src)
    {
        write (&src, sizeof (T));
    }

    struct Vector {
        uint32 size;
        void*  buffer;
    };
    
private:
    
    struct Vec
    {
        int32 size;
        int32 index;
    };

    Vec vec1, vec2;
    AbstractFifo fifo;
    HeapBlock<uint8> block;
    uint8* buffer;
    
};

#endif /* ELEMENT_JUCE_RINGBUFFER_H */
