/*
  ==============================================================================

   This file is part of Element
   Copyright (c) 2013 - Michael Fisher mfisher31@gmail.com

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

  ==============================================================================
*/

#ifndef ELEMENT_BUFFER_HPP
#define ELEMENT_BUFFER_HPP

#include "element/core.hpp"
#include <boost/intrusive_ptr.hpp>

namespace Element {
    class Buffer;
    class BufferFactory;
}

void intrusive_ptr_add_ref (Element::Buffer* b);
void intrusive_ptr_release (Element::Buffer* b);

namespace Element {

class Buffer
{
public:

    enum DataType {
        controlData,
        audioData,
        midiData,
        atomData,
        eventData
    };

    virtual ~Buffer();

    virtual void clear() noexcept = 0;
    virtual void clear (int start, int end) = 0;

    bool isManaged() const { return factory != nullptr; }

    uint32 getCapacity() const { return capacity; }
    DataType getType()   const { return dataType; }
    uint32 getSubType()  const { return subType; }

    void* getChannelData (int channel = 0);

protected:

    explicit Buffer (DataType dataType, uint32 subtype = 0);

private:

    void attach (BufferFactory* owner);
    void recycle();

    BufferFactory*      factory;
    std::atomic<uint32> refs;
    DataType            dataType;
    uint32              subType;
    uint32              capacity;
    Buffer*             next;

    friend class BufferFactory;
    friend void intrusive_ptr_add_ref (Buffer*);
    friend void intrusive_ptr_release (Buffer*);
};

class BufferRef : public boost::intrusive_ptr<Buffer>
{
public:
    BufferRef() :  boost::intrusive_ptr<Buffer>() { }
    BufferRef (Buffer* buf)
        : boost::intrusive_ptr<Buffer> (buf, true) { }
};
}

#endif // ELEMENT_BUFFER_HPP
