
#include "plugin-block.hpp"

namespace element {

    PluginBlock::PluginBlock (Processor *proc, const Symbol &symbol)
    {

    }

    PluginBlock::~PluginBlock()
    {

    }

    void
    PluginBlock::activate()
    {
        BlockNode::activate();
    }

    void
    PluginBlock::deactivate()
    {
        BlockNode::deactivate();
    }
}
