/*
    ClipSource.cpp - This file is part of Element

    Copyright (C) 2013 BKE, LLC  All rights reserved.
      * Michael Fisher <mfisher@bketech.com>
*/



ClipSource::ClipSource()
    : frames (0, 44100),
      looping (false)
{
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
    }
    else
    {
        start.addListener (this);
        length.addListener (this);
        offset.addListener (this);
    }
}

void
ClipSource::setModel (const ClipModel& m)
{
    state  = m.node();
    start.referTo (model().startValue());
    length.referTo (model().lengthValue());
}

ClipModel
ClipSource::model() const
{
    return ClipModel (state);
}

void
ClipSource::valueChanged (Value &value)
{
    if (start.refersToSameSourceAs (value) || length.refersToSameSourceAs (value))
        setTime (start.getValue(), length.getValue());
}
