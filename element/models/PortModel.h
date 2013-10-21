#ifndef PORTMODEL_H
#define PORTMODEL_H

#include "NodeModel.h"

namespace Element {

    class PortModel :  public NodeModel
    {

        PortModel()
            : NodeModel ("port")
        { }

    };

}


#endif // PORTMODEL_H
