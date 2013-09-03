
#include "context.hpp"

namespace element {

Context::Context (ID id)
    : m_id (id), m_start (0), m_end (0),
      m_nframes (0), m_realtime (true)
{ }

}  /* namespace element */
