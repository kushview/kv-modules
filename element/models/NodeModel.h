#ifndef NODEMODEL_H
#define NODEMODEL_H

#include "ObjectModel.h"

namespace Element {

    class NodeModel : public ObjectModel
    {
    public:

        NodeModel (const ValueTree& data)
            : ObjectModel (data) { }

        NodeModel (const Identifier& nodeType)
            : ObjectModel (nodeType) { }

    };
}

#endif // NODEMODEL_H
