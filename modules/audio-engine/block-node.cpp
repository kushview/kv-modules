
#include "block-node.hpp"
#include "context.hpp"

namespace element {

    BlockNode::BlockNode()
        : EngineNode (nullptr, String::empty)
    {

    }

    BlockNode::~BlockNode()
    {
        if (activated())
            deactivate();
    }

    void
    BlockNode::activate()
    {
        m_activated = true;
    }

    void
    BlockNode::deactivate()
    {
        m_activated = false;
    }

    void
    BlockNode::preProcess (Context& ctx)
    {

    }

    void
    BlockNode::postProcess (Context& ctx)
    {
    }

}
