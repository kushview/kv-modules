/*
    AudioEngine.h - This file is part of Element
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

#ifndef ELEMENT_AUDIO_ENGINE_H
#define ELEMENT_AUDIO_ENGINE_H

#include <element/formats/PluginManager.h>
#include <element/Engine.h>
#include <element/Pointer.h>

namespace juce {

    class AudioIODeviceCallback;

}

namespace Element {

    class ClipFactory;
    class GraphProcessor;
    class Globals;
    class EngineControl;
    class PatternInterface;
    class Pattern;
    class Transport;

    class AudioEngine : public Element::Engine
    {
    public:

        AudioEngine (Globals&);
        ~AudioEngine();

        void activate();
        void deactivate();

        Shared<EngineControl> controller();

        // Member access
        ClipFactory& clips();
        Globals& globals();
        GraphProcessor& graph();
        Transport* transport();

        // Engine methods
        AudioIODeviceCallback& callback();

    private:

        class Callback;
        Callback* cb;

        class Private;
        ScopedPointer<Private> priv;

        Globals& world;
    };
}

#endif // ELEMENT_AUDIO_ENGINE_H
