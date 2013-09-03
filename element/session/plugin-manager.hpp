#ifndef ELEMENT_PLUGIN_MANAGER_HPP
#define ELEMENT_PLUGIN_MANAGER_HPP

#include "element/juce.hpp"
#include "element/pointer.hpp"

namespace element {

    class LV2World;
    class Settings;

    class PluginManager : public AudioPluginFormatManager
    {
    public:

        PluginManager (LV2World&);
        ~PluginManager();

        AudioPluginFormat* format (const String& fmt);
        KnownPluginList& allPlugins();

        void saveUserPlugins (Settings&);
        void restoreUserPlugins (Settings&);

    private:

        LV2World& lv2;

        class Private;
        Scoped<Private> priv;

    };

}

#endif // ELEMENT_PLUGIN_MANAGER_HPP
