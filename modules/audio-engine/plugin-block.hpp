#ifndef ELEMENT_PLUGIN_BLOCK_HPP
#define ELEMENT_PLUGIN_BLOCK_HPP

#include "block-node.hpp"

namespace Element {

    class Processor;

    class PluginBlock :  public BlockNode
    {
    public:

        PluginBlock (Processor* proc, const Symbol& sym);
        virtual ~PluginBlock();

        void activate();
        void deactivate();
    };

}

#endif // ELEMENT_PLUGIN_BLOCK_HPP
