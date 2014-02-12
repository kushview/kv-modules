/*
    ClipSource.cpp - This file is part of Element

    Copyright (C) 2013 BKE, LLC  All rights reserved.
      * Michael Fisher <mfisher@bketech.com>
*/

#if JUCE_COMPLETION
#include "modules/element_engines/element_engines.h"
#endif

ClipSource::ClipSource()
    : frames (0, 0),
      looping (false)
{
    parentRate = 48000.f;
    connectValues();
}


ClipSource::~ClipSource()
{
    connectValues (true);
}

void
ClipSource::connectValues (bool disconnect)
{
    if (disconnect)
    {
        start.removeListener (this);
        length.removeListener (this);
        offset.removeListener (this);
        parentRate.removeListener (this);
    }
    else
    {
        start.addListener (this);
        length.addListener (this);
        offset.addListener (this);
        parentRate.addListener (this);
    }
}

void
ClipSource::setModel (const ClipModel& m)
{
    state = m.node();
    start.referTo (getModel().startValue());
    length.referTo (getModel().lengthValue());
}

ClipModel
ClipSource::getModel() const
{
    return ClipModel (state);
}

void
ClipSource::valueChanged (Value &value)
{
    if (start.refersToSameSourceAs (value) || length.refersToSameSourceAs (value))
        setTime (Range<double> (start.getValue(), length.getValue()));
    else if (parentRate.refersToSameSourceAs (value))
        setTime (Range<double> (start.getValue(), length.getValue()));
}
