
#include "block-node.hpp"
#include "engine-graph.hpp"
#include "node.hpp"

namespace Element {

    EngineGraph*
    EngineNode::parentGraph() const
    {
        return dynamic_cast<EngineGraph*> ((BlockNode*) parent);
    }

}
