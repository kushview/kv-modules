
#include "element/arc.hpp"

namespace Element {

    Arc::Arc (uint32 sn, uint32 sp, uint32 dn, uint32 dp) noexcept
        : sourceNode (sn), sourcePort (sp), destNode (dn), destPort (dp)  { }

}
