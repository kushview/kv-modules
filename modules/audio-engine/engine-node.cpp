
#include "block-node.hpp"
#include "engine-graph.hpp"
#include "node.hpp"

namespace element {

    EngineGraph*
    EngineNode::parentGraph() const
    {
        return dynamic_cast<EngineGraph*> ((BlockNode*) parent);
    }

}
