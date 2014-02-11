/*
    ClipSource.cpp - This file is part of Element

    Copyright (C) 2013 BKE, LLC  All rights reserved.
      * Michael Fisher <mfisher@bketech.com>
*/

ClipSource::ClipSource()
    : frames (0, 48000),
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
}
