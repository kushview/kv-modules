
#include "element/engine/Processor.h"

namespace Element {

int
Processor::getChannelPort (uint32 port)
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
Processor::getNumPorts()
{
    return getNumInputChannels() + getNumOutputChannels() +
           getNumParameters() + (acceptsMidi() ? 1 : 0) + (producesMidi() ? 1 : 0);
}

uint32
Processor::getNumPorts (PortType type, bool isInput)
{
    uint32 count = 0;
    for (uint32 port = 0; port < getNumPorts(); ++port)
        if (isInput == isPortInput (port) && type == getPortType (port))
            ++count;
    return count;
}

uint32
Processor::getNthPort (PortType type, int index, bool isInput, bool oneBased)
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
    return ELEMENT_INVALID_PORT;
}

bool
Processor::isPortInput (uint32 port)
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
Processor::getPortType (uint32 port)
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

}
