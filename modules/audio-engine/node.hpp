#ifndef ELEMENT_ENGINE_NODE_HPP
#define ELEMENT_ENGINE_NODE_HPP

#include "element/core.hpp"
#include "element/node.hpp"

namespace Element {

    class EngineGraph;

    class EngineNode : public Node
    {
    public:

        virtual ~EngineNode() { }
        virtual EngineGraph* parentGraph() const;

        const String&   path()   const { return String::empty; }
        const Symbol& symbol() const { return m_symbol; }

    protected:

        EngineNode (EngineNode* parent_, const String& symbol_)
            : parent (parent_), m_symbol (symbol_) { }

    private:

        EngineNode* parent;
        String m_symbol;

    };

}
#endif // ELEMENT_ENGINE_NODE_HPP
