#ifndef ELEMENT_PLUGIN_MANAGER_HPP
#define ELEMENT_PLUGIN_MANAGER_HPP

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

        void saveUserPlugins (Settings&);
        void restoreUser (Settings&);

        Processor *createPlugin (const PluginDescription& desc, String& errorMsg);

    private:

        class Private;
        Scoped<Private> priv;

    };

}

#endif // ELEMENT_PLUGIN_MANAGER_HPP
