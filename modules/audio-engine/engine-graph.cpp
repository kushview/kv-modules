
#include "compiled-graph.hpp"
#include "engine-graph.hpp"

namespace Element {

    void
    EngineGraph::addBlock (BlockNode& block)
    {
        m_blocks.push_front (block);
    }

    void
    EngineGraph::removeBlock (BlockNode& block)
    {
        m_blocks.erase (Blocks::s_iterator_to (block));
    }

    void
    EngineGraph::addPort (EnginePort& port)
    {
        m_ports.push_front (port);
    }

    void
    EngineGraph::removePort (EnginePort& port)
    {
        m_ports.erase (m_ports.iterator_to (port));
    }

    void
    EngineGraph::setCompiledGraph (CompiledGraph* graph)
    {

    }

    void
    EngineGraph::process (Context &ctx)
    {

    }

    static inline void
    graph_compile_recursive (BlockNode* n, CompiledGraph* output)
    {
        jassert (output != nullptr);

        if (n == nullptr || n->traversed())
            return;

        n->setTraversed (true);

        for (auto& p : n->sources())
            if (! p->traversed())
                graph_compile_recursive (p, output);

        output->push_back (CompiledBlock (n, n->sources()));
    }

    CompiledGraph*
    EngineGraph::compile()
    {
        CompiledGraph* const graph = new CompiledGraph();

        for (auto& b : m_blocks)
            b.setTraversed (false);

        // Either a sink or connected to our output ports:
        for (auto& b : m_blocks)
            if (! b.traversed() && ! b.dependants().size())
                graph_compile_recursive (&b, graph);

        // Traverse any blocks we didn't hit yet
        for (auto& b : m_blocks)
            if (! b.traversed())
                graph_compile_recursive (&b, graph);

        if (graph->size() != m_blocks.size())
        {
            Logger::writeToLog ("Failed to compile graph");
            delete graph;
            return nullptr;
        }

        return graph;
    }
}
