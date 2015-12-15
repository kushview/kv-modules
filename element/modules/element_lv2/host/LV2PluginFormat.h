/*
    This file is part of the lvtk_plugins JUCE module
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

#ifndef LVTK_JUCE_LV2FORMAT_H
#define LVTK_JUCE_LV2FORMAT_H


class LV2World;
class SymbolMap;


/** Implements a plugin format manager for LV2 plugins in Juce Apps. */
class LV2PluginFormat :   public AudioPluginFormat
{
public:
    /** Create a self-contained LV2PluginFormat (owns all data) */
    LV2PluginFormat();

    /** Create an LV2PluginFormat using an external LV2World object */
    LV2PluginFormat (LV2World& lv2);

    ~LV2PluginFormat();

    String getName() const { return "LV2"; }
    void findAllTypesForFile (OwnedArray <PluginDescription>& descrips, const String& identifier);
    AudioPluginInstance* createInstanceFromDescription (const PluginDescription& desc, double sampleRate, int bufferSize);
    bool fileMightContainThisPluginType (const String& fileOrIdentifier);
    String getNameOfPluginFromIdentifier (const String& fileOrIdentifier);
    StringArray searchPathsForPlugins (const FileSearchPath&, bool recursive);
    bool doesPluginStillExist (const PluginDescription&);
    FileSearchPath getDefaultLocationsToSearch();
    bool canScanForPlugins() const { return true; }
    virtual bool pluginNeedsRescanning (const PluginDescription&) { return false; }

    SymbolMap& getSymbolMap();

private:
    class Internal;
    ScopedPointer<Internal> priv;
};


#endif   // LVTK_JUCE_LV2FORMAT_H
