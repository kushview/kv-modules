/*
  PortWriter.h - Header description
*/

#ifndef ELEMENT_PORT_WRITER_H
#define ELEMENT_PORT_WRITER_H

#include <inttypes.h>
#include <functional>

#include "element/engine/PortType.h"

namespace Element {

    /** Function type for writing to a port buffer
        Params are in order port, size, protocol, buffer
        @note This is compatible with lvtk's port write ui method */
    typedef std::function<void(uint32_t, uint32_t, uint32_t, void const*)> PortWriteFunction;

    struct PortEvent
    {
        uint32 index;
        uint32 protocol;
        union {
            double decimal;
            int64 frames;
        } time;
        uint32 size;
    };

}

#endif  /* ELEMENT_PORT_WRITER_H */
