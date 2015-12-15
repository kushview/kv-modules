/*
    This file is part of the element modules for the JUCE Library
    Copyright (c) 2013 - Michael Fisher <mfisher31@gmail.com>.

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

RingBuffer::RingBuffer (int32 capacity)
    : fifo (1), buffer (nullptr)
{
    setCapacity (capacity);
}

RingBuffer::~RingBuffer()
{
    fifo.reset();
    fifo.setTotalSize (1);
    buffer = nullptr;
    block.free();
}

void RingBuffer::setCapacity (int32 newCapacity)
{
    newCapacity = nextPowerOfTwo (newCapacity);
    
    if (fifo.getTotalSize() != newCapacity)
    {
        HeapBlock<uint8> newBlock;
        newBlock.allocate (newCapacity, true);
        {
            block.swapWith (newBlock);
            buffer = block.getData();
            fifo.setTotalSize (newCapacity);
        }
    }

    fifo.reset();
}
