/*
    This file is part of the element modules for the JUCE Library
    Copyright (C) 2014  Kushview, LLC.  All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

int PortProcessor::getChannelPort (uint32 port)
{
    jassert (port < (uint32) getNumPorts());

    int channel = 0;

    const bool isInput  = isPortInput (port);
    const PortType type = getPortType (port);

    for (uint32 p = 0; p < (uint32)getNumPorts(); ++p)
    {
        const PortType thisPortType = getPortType (p);
        const bool thisPortIsInput = isPortInput (p);

        if (type == thisPortType && p == port)
            return channel;

        // tally the channel only if type and flow match
        if (type == thisPortType && isInput == thisPortIsInput)
            ++channel;
    }

    return -1;
}

uint32
PortProcessor::getNumPorts()
{
    return getNumInputChannels() + getNumOutputChannels() +
           getNumParameters() + (acceptsMidi() ? 1 : 0) + (producesMidi() ? 1 : 0);
}

uint32
PortProcessor::getNumPorts (PortType type, bool isInput)
{
    uint32 count = 0;
    for (uint32 port = 0; port < getNumPorts(); ++port)
        if (isInput == isPortInput (port) && type == getPortType (port))
            ++count;
    return count;
}

uint32
PortProcessor::getNthPort (PortType type, int index, bool isInput, bool oneBased)
{
    int count = oneBased ? 0 : -1;

    jassert (getNumPorts() >= 0);
    uint32 nports = (uint32) getNumPorts();

    for (uint32 port = 0; port < nports; ++port)
    {
        if (type == getPortType (port) && isInput == isPortInput (port))
        {
            if (++count == index) {
                return port;
            }
        }
    }

    jassertfalse;
    return LV2UI_INVALID_PORT_INDEX;
}

bool
PortProcessor::isPortInput (uint32 port)
{
    if (port >= getNumPorts())
        jassertfalse;

    const int audioIns = getNumInputChannels();
    const int totalAudio = audioIns + getNumOutputChannels();
    const int totalMidi  = (int)acceptsMidi() + (int)producesMidi();
    const int numControl = getNumParameters();

    switch (getPortType(port).id())
    {
        case PortType::Audio:
            return (audioIns > 0 && port < audioIns);
            break;
        case PortType::Atom:
        {
            if (totalMidi == 1 && acceptsMidi())
                return true;
            if (totalMidi == 2 && (getNumPorts() - port) == 2)
                return true;
            return false;
            break;
        }
        case PortType::Control:
            return (numControl > 0 && (port >= totalAudio && port < totalAudio + numControl));
            break;
        case PortType::Unknown:
        default:
            break;
    }

    assert (false);
    return false;
}

PortType
PortProcessor::getPortType (uint32 port)
{
    const bool haveControls = getNumParameters() > 0;
    const bool haveAudio = (getNumInputChannels() + getNumOutputChannels()) > 0;

    if (haveAudio && port < (getNumInputChannels() + getNumOutputChannels()))
        return PortType::Audio;

    if (haveControls && port >= (getNumInputChannels() + getNumOutputChannels()) &&
        port < (getNumInputChannels() + getNumOutputChannels() + getNumParameters()))
        return PortType::Control;

    if (port >= (getNumInputChannels() + getNumOutputChannels() + getNumParameters()))
    {
        return PortType::Atom;
    }

    assert (false);
    return PortType::Unknown;
}

bool
PortProcessor::writeControlValue (uint32 port, float value)
{
    return writeToPort (port, sizeof (float), 0U, &value);
}

bool
PortProcessor::writeToPort (uint32 port, uint32 size, uint32 protocol, void const* data)
{
    return false;
}
