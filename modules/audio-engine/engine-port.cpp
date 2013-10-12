
#include "block-node.hpp"
#include "port-node.hpp"

namespace Element {

    EnginePort::EnginePort (BlockNode *block, const String &name,
                        uint32 index, PortType type)
        : EngineNode (block, name),
          m_index (index),
          m_type (type),
          m_monitored (false),
          m_set_by_user (false),
          m_is_morph (false),
          m_is_auto_morph (false),
          m_is_logarithmic (false),
          m_is_sample_rate (false),
          m_is_toggled (false)
    { }

    EnginePort::~EnginePort()
    {

    }

}
