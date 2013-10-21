/*
    audio-engine.cpp - This file is part of Element
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
#include "element/Engine.h"
#include "element/Session.h"

#include "element/engine/graph-player.hpp"
#include "element/engine/port-processor.hpp"

#include "engine-graph.hpp"
#include "internal-format.hpp"

namespace Element {

    class TestPlayer :  public GraphPlayer
    {
    public:
        TestPlayer ()
            : world (nullptr)
        {

        }

    private:

        World* world;

    };

    class AudioEngine :  public Engine
    {
    public:

        AudioEngine (World& w)
            : world (w)
        {
            testPlayer.setRootGraph (&graph);
        }

        ~AudioEngine() { }

        AudioIODeviceCallback& callback()
        {
            return testPlayer;
        }

    private:

        World&        world;
        EngineGraph   graph;
        TestPlayer    testPlayer;

    };

    class AudioEngineModule : public Module
    {
    public:

        AudioEngineModule()  { }
        ~AudioEngineModule() { }

        void load (World* world)
        {
            assert (world->engine() == nullptr);

            const AudioPluginFormat* f = world->plugins().format ("Internal");
            if (f == nullptr)
                world->plugins().addFormat (new InternalFormat());

            world->setEngine (makeShared<AudioEngine> (*world));
        }

    private:



    };
}

extern "C" {

    Element::Module* JUCE_CALLTYPE
    element_module_load()
    {
        return new Element::AudioEngineModule();
    }

}
