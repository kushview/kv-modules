/*
  lv2nodes.hpp - Common Lilv nodes

  Copyright (C) 2013  BKE, LLC.  All rights reserved.
  Michael Fisher <mfisher@bketech.com>

*/

#ifndef ELEMENT_LV2WORLD_H
#define ELEMENT_LV2WORLD_H

#include <lilv/lilvmm.hpp>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/event/event.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <lv2/lv2plug.in/ns/ext/worker/worker.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

#include "element/Juce.h"

namespace Element {

    class  LV2Module;

    class LV2World
    {
    public:

        LV2World();
        ~LV2World();

        const LilvNode*   lv2_InputPort;
        const LilvNode*   lv2_OutputPort;
        const LilvNode*   lv2_AudioPort;
        const LilvNode*   lv2_AtomPort;
        const LilvNode*   lv2_ControlPort;
        const LilvNode*   lv2_EventPort;
        const LilvNode*   lv2_CVPort;
        const LilvNode*   midi_MidiEvent;
        const LilvNode*   work_schedule;
        const LilvNode*   work_interface;

        LV2Module* createModule (const String& uri);

        const LilvPlugin* getPlugin (const String& uri);
        const Lilv::Plugins getAllPlugins();

        bool isFeatureSupported (const String& featureURI);
        bool isPluginAvailable (const String& uri);
        bool isPluginSupported (const String& uri);

        Lilv::World& lilvWorld() { return world; }

    private:

        Lilv::World world;

    };

}

#endif /* ELEMENT_LV2WORLD_H */
