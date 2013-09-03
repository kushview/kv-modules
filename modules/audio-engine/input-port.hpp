#ifndef ELEMENT_INPUT_PORT_HPP
#define ELEMENT_INPUT_PORT_HPP

#include "element/engine/port-node.hpp"

namespace element {

    class InputPort : public EnginePort
    {
    public:
        InputPort();
        ~InputPort();
    };

}

#endif // ELEMENT_INPUT_PORT_HPP
