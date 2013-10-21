#ifndef GRAPHMODEL_H
#define GRAPHMODEL_H

#include "BlockModel.h"

namespace Element {

    class GraphModel :  public BlockModel
    {
    public:

        GraphModel() : BlockModel ("graph") { }
        ~GraphModel() { }

        inline int32 countBlocks() const {
            return countChildrenOfType (Slugs::block);
        }

    private:

    };

}
#endif // GRAPHMODEL_H
