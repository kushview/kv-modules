/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#ifndef ELEMENT_INTERNAL_FORMAT_HPP
#define ELEMENT_INTERNAL_FORMAT_HPP

#include "element/engine/graph-processor.hpp"
#include "element/engine/port-processor.hpp"

namespace Element
{

    /**
        Manages the internal plugin types.
    */
    class InternalFormat   : public AudioPluginFormat
    {
    public:

        enum ID
        {
            audioInputDevice = 0,
            audioOutputDevice,
            midiInputDevice,
            audioInputPort,
            audioOutputPort,
            atomInputPort,
            atomOutputPort,
            controlInputPort,
            controlOutputPort,
            cvOutPort,
            cvInPort,
            midiInputPort,
            midiOutputPort,
            internalTypesEnd
        };

        InternalFormat();
        ~InternalFormat() { }

        const PluginDescription* description (const InternalFormat::ID type);
        void getAllTypes (OwnedArray <PluginDescription>& results);

        String getName() const                                      { return "Internal"; }
        bool fileMightContainThisPluginType (const String&)         { return false; }
        FileSearchPath getDefaultLocationsToSearch()                { return FileSearchPath(); }
        bool canScanForPlugins() const                              { return false; }
        void findAllTypesForFile (OwnedArray <PluginDescription>&, const String&) { }
        bool doesPluginStillExist (const PluginDescription&)        { return true; }
        String getNameOfPluginFromIdentifier (const String& fileOrIdentifier)   { return fileOrIdentifier; }
        bool pluginNeedsRescanning (const PluginDescription&)       { return false; }
        StringArray searchPathsForPlugins (const FileSearchPath&, bool) { return StringArray(); }
        AudioPluginInstance* createInstanceFromDescription (const PluginDescription& desc);

    private:

        PluginDescription audioInDesc;
        PluginDescription audioOutDesc;
        PluginDescription midiInDesc;
    };

}

#endif   // ELEMENT_INTERNAL_FORMAT_HPP
