/*
    Processor.cpp - This file is part of Element
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

uint32 Processor::getPortForAudioChannel (AudioProcessor* proc, int chan, bool isInput)
{
    return (isInput) ? static_cast<uint32> (chan)
                     : static_cast<uint32> (proc->getTotalNumInputChannels() + chan);
}

uint32 Processor::getNumPorts (AudioProcessor* proc)
{
    return proc->getTotalNumInputChannels() + proc->getTotalNumOutputChannels() +
        proc->getNumParameters() + (proc->acceptsMidi() ? 1 : 0) + (proc->producesMidi() ? 1 : 0);
}

uint32 Processor::getNumPorts (AudioProcessor* proc, PortType type, bool isInput)
{
    uint32 count = 0;
    for (uint32 port = 0; port < getNumPorts(proc); ++port)
        if (isInput == isPortInput (proc, port) && type == getPortType (proc, port))
            ++count;
    return count;
}

PortType Processor::getPortType (AudioProcessor* proc, uint32 port)
{
    const bool haveControls = proc->getNumParameters() > 0;
    const bool haveAudio = (proc->getTotalNumInputChannels() + proc->getTotalNumOutputChannels()) > 0;
    
    if (haveAudio && port < (proc->getTotalNumInputChannels() + proc->getTotalNumOutputChannels()))
        return PortType::Audio;
    
    if (haveControls && port >= (proc->getTotalNumInputChannels() + proc->getTotalNumOutputChannels()) &&
        port < (proc->getTotalNumInputChannels() + proc->getTotalNumOutputChannels() + proc->getNumParameters()))
        return PortType::Control;
    
    if (port >= (proc->getTotalNumInputChannels() + proc->getTotalNumOutputChannels() + proc->getNumParameters()))
    {
        return PortType::Atom;
    }
    
    assert (false);
    return PortType::Unknown;
}

bool Processor::isPortInput (AudioProcessor* proc, uint32 port)
{
    if (port >= getNumPorts (proc))
        jassertfalse;
    
    const int audioIns = proc->getTotalNumInputChannels();
    const int totalAudio = audioIns + proc->getTotalNumOutputChannels();
    const int totalMidi  = (int)proc->acceptsMidi() + (int)proc->producesMidi();
    const int numControl = proc->getNumParameters();
    
    switch (getPortType(proc, port).id())
    {
        case PortType::Audio:
            return (audioIns > 0 && port < audioIns);
            break;
        case PortType::Atom:
        {
            if (totalMidi == 1 && proc->acceptsMidi())
                return true;
            if (totalMidi == 2 && (getNumPorts(proc) - port) == 2)
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

bool Processor::writeToPort (AudioProcessor*, uint32 port, uint32 size, uint32 protocol, void const* data)
{
    jassertfalse;
    return true;
}

int Processor::getChannelPort (uint32 port)
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

uint32 Processor::getNumPorts()
{
    return getNumPorts (this);
}

uint32 Processor::getNumPorts (PortType type, bool isInput)
{
    return getNumPorts (this, type, isInput);
}

uint32 Processor::getNthPort (PortType type, int index, bool isInput, bool oneBased)
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

bool Processor::isPortInput (uint32 port)
{
    return isPortInput (this, port);
}

PortType Processor::getPortType (uint32 port)
{
    return getPortType (this, port);
}

bool Processor::writeControlValue (uint32 port, float value)
{
    return writeToPort (port, sizeof (float), 0U, &value);
}

bool Processor::writeToPort (uint32 port, uint32 size, uint32 protocol, void const* data)
{
    return writeToPort (this, port, size, protocol, data);
}
