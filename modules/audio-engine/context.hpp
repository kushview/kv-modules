
#ifndef ELEMENT_CONTEXT_HPP
#define ELEMENT_CONTEXT_HPP

#include "element/types.h"

namespace element {

class Context
{
public:

	enum class ID {
        audio,
        message
	};

    Context (ID id);
    virtual ~Context() { }

    inline ID id() const { return m_id; }

    inline void
    locate (FrameTime time, SampleCount nframes)
    {
        m_start   = time;
        m_end     = time + nframes;
        m_nframes = nframes;
	}

    inline void
    locate (const Context& other)
    {
        m_start   = other.m_start;
        m_end     = other.m_end;
        m_nframes = other.m_nframes;
	}

    inline FrameTime   start()    const { return m_start; }
    inline FrameTime   end()      const { return m_end; }
    inline SampleCount nframes()  const { return m_nframes; }
    inline bool        realtime() const { return m_realtime; }

protected:

    ID          m_id;         ///< Fast ID for this context
    FrameTime   m_start;      ///< Start frame of this cycle, timeline relative
    FrameTime   m_end;        ///< End frame of this cycle, timeline relative
    SampleCount m_nframes;    ///< Length of this cycle in frames
    bool        m_realtime;   ///< True iff context is hard realtime

};

}  /* namespace element */

#endif  /* ELEMENT_CONTEXT_HPP */
