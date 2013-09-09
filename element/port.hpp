/*
  port.hpp - Header description

  Copyright (C) 2013  BKE, LLC.  All rights reserved.
  mfisher <mfisher@bketech.com>

*/
/** @file port.hpp */

#ifndef ELEMENT_PORT_HPP
#define ELEMENT_PORT_HPP

#include <inttypes.h>

#include <boost/function.hpp>

#include "element/core.hpp"
#include "element/node.hpp"

#define ELEMENT_INVALID_PORT JUCE_INVALID_PORT

namespace element {

    /** Function type for writing to a port buffer
        Params are in order port, size, protocol, buffer
        @note This is compatible with lvtk's port write ui method */
    typedef boost::function<void(uint32_t, uint32_t, uint32_t, void const*)> PortWriteFunction;

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

#endif  /* ELEMENT_PORT_HPP */
