/*
    This file is part of the Kushview Modules for JUCE
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

#ifndef EL_LV2WORLD_H
#define EL_LV2WORLD_H

#define LV2_UI__JuceUI LV2_UI_PREFIX "JuceUI"

class  LV2Module;
class  LV2PluginModel;

/** Slim wrapper around LilvWorld.  Publishes commonly used LilvNodes and
    manages heavy weight features (like LV2 Worker) */
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
    const LilvNode*   ui_X11UI;
    const LilvNode*   ui_JuceUI;

    /** Create an LV2Module for a uri string */
    LV2Module* createModule (const String& uri);

    /** Create a plugin model for a uri string */
    LV2PluginModel* createPluginModel (const String& uri);

    /** Fill a PluginDescription for a plugin uri */
    void fillPluginDescription (const String& uri, PluginDescription& desc) const;

    /** Get an LilvPlugin for a uri string */
    const LilvPlugin* getPlugin (const String& uri) const;

    /** Get all Available Plugins */
    const LilvPlugins* getAllPlugins() const;

    /** Returns true if a feature is supported */
    bool isFeatureSupported (const String& featureURI);

    /** Returns true if the plugin uri is availble */
    bool isPluginAvailable (const String& uri);

    /** Returns true if the plugin is supported on this system */
    bool isPluginSupported (const String& uri);
    bool isPluginSupported (const LilvPlugin* plugin);

    /** Return the underlying LilvWorld* pointer */
    inline LilvWorld* getWorld() const { return world; }

    /** Add a supported feature */
    inline void addFeature (LV2Feature* feat, bool rebuild = true) { features.add (feat, rebuild); }

    /** Get supported features */
    inline LV2FeatureArray& getFeatureArray() { return features; }

    /** Get supported features as a juce array.
        This can be used when instantiating plugins and uis. Don't
        forget to terminate the array with nullptr before passing
        to a plugin instance */
    inline void getFeatures (Array<const LV2_Feature*>& feats) const { features.getFeatures (feats); }

    /** Get a worker thread */
    inline WorkThread& getWorkThread();

    /** Returns the total number of available worker threads */
    inline int32 getNumWorkThreads() const { return numThreads; }

    inline SuilHost* getSuilHost() { return suil; }

private:
    LilvWorld* world;
    SuilHost* suil;
    LV2FeatureArray features;

    // a simple rotating thread pool
    int32 currentThread, numThreads;
    OwnedArray<WorkThread> threads;
};

#endif /* EL_LV2WORLD_H */
