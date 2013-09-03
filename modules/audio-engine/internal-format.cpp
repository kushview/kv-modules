/*
    InternalFilters.cpp - This file is part of Element
    Copyright (C) 2013  Michael Fisher <mfisher31@gmail.com>

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

#include "element/juce.hpp"
#include "element/engine/graph-processor.hpp"

#include "internal-format.hpp"

namespace element {


    //==============================================================================
    InternalFormat::InternalFormat()
    {
        {
            GraphProcessor::AudioGraphIOProcessor p (GraphProcessor::AudioGraphIOProcessor::audioOutputNode);
            p.fillInPluginDescription (audioOutDesc);
        }

        {
            GraphProcessor::AudioGraphIOProcessor p (GraphProcessor::AudioGraphIOProcessor::audioInputNode);
            p.fillInPluginDescription (audioInDesc);
        }

        {
            GraphProcessor::AudioGraphIOProcessor p (GraphProcessor::AudioGraphIOProcessor::midiInputNode);
            p.fillInPluginDescription (midiInDesc);
        }
    }

    AudioPluginInstance*
    InternalFormat::createInstanceFromDescription (const PluginDescription& desc)
    {
        if (desc.name == audioOutDesc.name)
        {
            return new GraphProcessor::AudioGraphIOProcessor (GraphProcessor::AudioGraphIOProcessor::audioOutputNode);
        }
        else if (desc.name == audioInDesc.name)
        {
            return new GraphProcessor::AudioGraphIOProcessor (GraphProcessor::AudioGraphIOProcessor::audioInputNode);
        }
        else if (desc.name == midiInDesc.name)
        {
            return new GraphProcessor::AudioGraphIOProcessor (GraphProcessor::AudioGraphIOProcessor::midiInputNode);
        }

        return nullptr;
    }

    const PluginDescription*
    InternalFormat::description (const InternalFormat::ID type)
    {
        switch (type)
        {
            case audioInputDevice:      return &audioInDesc;
            case audioOutputDevice:     return &audioOutDesc;
            case midiInputDevice:       return &midiInDesc;
            default:                    break;
        }

        return nullptr;
    }

    void
    InternalFormat::getAllTypes (OwnedArray <PluginDescription>& results)
    {
        for (int i = 0; i < (int) audioOutputPort; ++i)
            results.add (new PluginDescription (*description ((InternalFormat::ID) i)));
    }

}
