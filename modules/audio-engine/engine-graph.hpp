#ifndef ELEMENT_ENGINE_GRAPH_HPP
#define ELEMENT_ENGINE_GRAPH_HPP

#include <boost/intrusive/slist.hpp>

#include "element/engine/GraphProcessor.h"

#include "block-node.hpp"
#include "port-node.hpp"

namespace Element
{

    class CompiledGraph;

    class EngineGraph :  public BlockNode,
                         public GraphProcessor
    {
    public:

        typedef BlockNode::GraphType Type;

        EngineGraph() { }
        virtual ~EngineGraph() { }
        Type type() const { return BlockNode::graphType; }

        // graph specific methods
        typedef boost::intrusive::slist<
            BlockNode, boost::intrusive::constant_time_size<true> > Blocks;

        typedef boost::intrusive::slist<
            EnginePort, boost::intrusive::constant_time_size<true> > Ports;

        void addBlock (BlockNode& block);
        void removeBlock (BlockNode& block);

        uint32 numPortsNotRt() const;

        void addPort (EnginePort& port);
        void removePort (EnginePort& port);

        void process (Context &ctx);

        Blocks&       blocks()       { return m_blocks; }
        const Blocks& blocks() const { return m_blocks; }

        CompiledGraph* compile();
        void setCompiledGraph (CompiledGraph*);

    private:

        Blocks m_blocks;
        Ports  m_ports;

    };

}

#endif // ELEMENT_ENGINE_GRAPH_HPP
