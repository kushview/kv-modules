#ifndef ELEMENT_ENGINE_BLOCK_HPP
#define ELEMENT_ENGINE_BLOCK_HPP

#include "element/list.hpp"

#include "context.hpp"
#include "node.hpp"

namespace element {

    class Context;

    class BlockNode : public EngineNode,
                      public SListHook
    {
    public:

        BlockNode();
        virtual ~BlockNode();

        Array<BlockNode*>& sources() { return m_sources; }
        Array<BlockNode*>& dependants() { return m_dependants; }

        bool traversed() const { return m_traversed; }
        void setTraversed (bool yn) { m_traversed = yn; }

        GraphType type() const { return Node::blockType; }

        virtual void activate(); /* bufferfactory */
        virtual void deactivate();
        bool activated() const { return m_activated; }

        virtual void preProcess (Context& ctx);
        virtual void process (Context& ctx) = 0;
        virtual void postProcess (Context& ctx);

    private:
        //Processor*              _plugin;
       // Raul::Array<PortImpl*>* _ports; ///< Access in audio thread only
        Context::ID         m_context;
        uint32              m_polyphony;
        Array<BlockNode*>   m_sources;
        Array<BlockNode*>   m_dependants;

        bool                m_activated;
        bool                m_traversed;
    };

}

#endif // ELEMENT_ENGINE_BLOCK_HPP
