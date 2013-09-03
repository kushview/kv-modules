#ifndef ELEMENT_PORT_NODE_HPP
#define ELEMENT_PORT_NODE_HPP

#include "element/juce.hpp"
#include "element/list.hpp"
#include "element/port.hpp"
#include "element/types.h"

#include "node.hpp"

namespace element {

    class BlockNode;
    class EnginePort :  public EngineNode,
                        public SListHook
    {
    public:
        virtual ~EnginePort();

    protected:

    private:

        EnginePort (BlockNode* block, const String& name, uint32 index, PortType type);

        struct SetState
        {
            enum class State { Set, HalfSetCycle_1, HalfSetCycle_2 };
            SetState() : state(State::Set), value(0), time(0) { }

            State     state;  ///< State of buffer for setting control value
            Sample    value;  ///< Value currently being set
            FrameTime time;   ///< Time value was set
        };

        //BufferFactory&          _bufs;
        uint32                  m_index;
        PortType                m_type;
        //Atom                    m_value;
        //Atom                    m_min;
        //Atom                    m_max;
        //Raul::Array<SetState>*  m_set_states;
        //Raul::Array<SetState>*  m_prepared_set_states;
        //Raul::Array<BufferRef>* m_buffers;
        //Raul::Array<BufferRef>* m_prepared_buffers;
        bool                    m_monitored;
        bool                    m_set_by_user;
        bool                    m_is_morph;
        bool                    m_is_auto_morph;
        bool                    m_is_logarithmic;
        bool                    m_is_sample_rate;
        bool                    m_is_toggled;
    };
}
#endif // ELEMENT_PORT_NODE_HPP
