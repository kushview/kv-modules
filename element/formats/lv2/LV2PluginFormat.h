/*
  lv2format.hpp - Implementing the LV2 Host API for Juce Apps

  Copyright (C) 2013  BKE, LLC.  All rights reserved.
  Michael Fisher <mfisher@bketech.com>

*/
/** @file lv2format.hpp */

#ifndef ELEMENT_LV2FORMAT_HPP
#define ELEMENT_LV2FORMAT_HPP

#include "element/Juce.h"

namespace Element {

    class LV2World;
    class SymbolMap;

//==============================================================================
/**
    Implements a plugin format manager for LV2 plugins in Juce Apps.
*/
class LV2PluginFormat :   public AudioPluginFormat
{
public:
    //==============================================================================
    LV2PluginFormat (LV2World& lv2);
    ~LV2PluginFormat();

    //==============================================================================
    String getName() const { return "LV2"; }
    void findAllTypesForFile (OwnedArray <PluginDescription>& descrips, const String& identifier);
    AudioPluginInstance* createInstanceFromDescription (const PluginDescription& desc, double initialSampleRate,
                                                        int initialBufferSize);
    bool fileMightContainThisPluginType (const String& fileOrIdentifier);
    String getNameOfPluginFromIdentifier (const String& fileOrIdentifier);
    StringArray searchPathsForPlugins (const FileSearchPath&, bool recursive);
    bool doesPluginStillExist (const PluginDescription&);
    FileSearchPath getDefaultLocationsToSearch();
    bool canScanForPlugins() const { return true; }
    virtual bool pluginNeedsRescanning (const PluginDescription&) { return false; }
    SymbolMap& symbols();

private:

    class Private;
    ScopedPointer<Private> priv;

};


} /* namespace element */

#endif   // ELEMENT_LV2FORMAT_HPP
