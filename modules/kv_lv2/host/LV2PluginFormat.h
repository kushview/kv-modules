/*
    This file is part of the Kushview Modules for JUCE
    Copyright (c) 2014-2019  Kushview, LLC.  All rights reserved.

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

#ifndef EL_LV2FORMAT_H
#define EL_LV2FORMAT_H

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

    String getName() const override { return "LV2"; }

    void findAllTypesForFile (OwnedArray <PluginDescription>& descrips, const String& identifier) override;
    bool fileMightContainThisPluginType (const String& fileOrIdentifier) override;
    String getNameOfPluginFromIdentifier (const String& fileOrIdentifier) override;
    bool pluginNeedsRescanning (const PluginDescription&) override { return false; }
    bool doesPluginStillExist (const PluginDescription&) override;
    bool canScanForPlugins() const override { return true; }
    StringArray searchPathsForPlugins (const FileSearchPath&, bool recursive,
                                       bool allowPluginsWhichRequireAsynchronousInstantiation = false) override;
    FileSearchPath getDefaultLocationsToSearch() override;
    
    SymbolMap& getSymbolMap();

protected:
    void createPluginInstance (const PluginDescription&, double initialSampleRate,
                               int initialBufferSize, void* userData,
                               PluginCreationCallback) override;

    bool requiresUnblockedMessageThreadDuringCreation (const PluginDescription&) const noexcept override { return false; }

private:
    class Internal;
    ScopedPointer<Internal> priv;
};


#endif   // EL_LV2FORMAT_H
