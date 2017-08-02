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

#ifndef EL_LV2PLUGINMODEL_H
#define EL_LV2PLUGINMODEL_H

/** A C++ wrapper around LilvPlugin.
    This works with acquiring data/info about an LV2 plugin */
class LV2PluginModel
{
public:

    /** Create a new LV2PluginModel */
    LV2PluginModel (LV2World& world, const LilvPlugin* plugin_);

    /** Destructor */
    ~LV2PluginModel();

    /** Get the total number of ports for this plugin */
    uint32 getNumPorts() const;

    /** Get the number of ports for a given port type and flow (input or output) */
    uint32 getNumPorts (PortType type, bool isInput) const;

    /** Get the plugin's Author/Manufacturer name */
    String getAuthorName() const;

    /** Get the plugins class label (category) */
    String getClassLabel() const;

    /** Get the port intended to be used as a MIDI input */
    uint32 getMidiPort() const;

    /** Get the plugin's name */
    String getName() const;

    /** Get the plugin's notify port. e.g. the port intended to be
        used as a MIDI output */
    uint32 getNotifyPort() const;

    /** Get the LilvPlugin object for this Module */
    const LilvPlugin* getPlugin() const;

    /** Get the LilvPort for this Module (by index) */
    const LilvPort* getPort (uint32 index) const;

    /** Get a port's name */
    const String getPortName (uint32 port) const;

    /** Get the type of port for a port index */
    PortType getPortType (uint32 index) const;

    /** Get the URI for this plugin */
    String getURI() const;

    /** Returns true if the Plugin has one or more UIs
        @note Currently LV2 UIs aren't supported and this always
        will return false */
    inline bool hasEditor() const { return false; /* XXX */ }

    /** Returns true if the port is an Input */
    bool isPortInput (uint32 port) const;

    /** Returns true if the port is an Output */
    bool isPortOutput (uint32 port) const;

    /** Get the plugin's extension data
        @param uri The uri of the extesion
        @return A pointer to extension data or nullptr if not available
        @note This is in the LV2 Discovery Threading class */
    const void* getExtensionData (const String& uri) const;

protected:

    LV2World&           world;
    const LilvPlugin*   plugin;
    uint32              numPorts;

private:

    class Private;
    ScopedPointer<Private> priv;

};

#endif /* EL_LV2PluginModel_H */
