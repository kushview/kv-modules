/*
    PluginManager.h - This file is part of Element
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

#ifndef ELEMENT_PLUGIN_MANAGER_H
#define ELEMENT_PLUGIN_MANAGER_H

#include "element/engine/Processor.h"
#include "element/Pointer.h"

namespace Element {

    class LV2World;
    class Settings;

    class PluginManager
    {
    public:

        PluginManager();

       #if ELEMENT_PLUGINHOST_LV2
        PluginManager (LV2World&);
       #endif

        ~PluginManager();

        void addDefaultFormats();
        void addFormat (AudioPluginFormat*);

        KnownPluginList& availablePlugins();

        AudioPluginFormatManager& formats();
        AudioPluginFormat* format (const String& fmt);
        template<class FormatType>
        inline FormatType* format()
        {
            for (int i = 0; i < formats().getNumFormats(); ++i)
                if (FormatType* fmt = dynamic_cast<FormatType*> (formats().getFormat (i)))
                    return fmt;
            return nullptr;
        }

        void saveUserPlugins (ApplicationProperties&);
        void restoreUserPlugins (ApplicationProperties&);

        Processor *createPlugin (const PluginDescription& desc, String& errorMsg);

    private:

        class Private;
        Scoped<Private> priv;

    };

}

#endif // ELEMENT_PLUGIN_MANAGER_H
