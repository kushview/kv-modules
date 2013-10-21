/*
    SequenceModel.h - This file is part of BTV

    Copyright (C) 2013 BKE, LLC  All rights reserved.
      * Michael Fisher <mfisher@bketech.com>
*/

#ifndef BTV_SEQUENCE_MODEL_H
#define BTV_SEQUENCE_MODEL_H

#include "GraphModel.h"

namespace Element {

    /** This model describes a sequence which has tracks, which have clips
        for various media types */
    class SequenceModel : public ObjectModel
    {
    public:

        SequenceModel();
        SequenceModel (const ValueTree& data);
        SequenceModel (const SequenceModel& o);

        virtual ~SequenceModel();

    private:

        // disable
        SequenceModel& operator= (const SequenceModel&);

    };

}

#endif // BTV_SEQUENCE_MODEL_H
