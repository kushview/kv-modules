
#include "element/arc.hpp"

namespace element {

    Arc::Arc (uint32 sn, uint32 sp, uint32 dn, uint32 dp) noexcept
        : sourceNode (sn), sourcePort (sp), destNode (dn), destPort (dp)  { }

}
