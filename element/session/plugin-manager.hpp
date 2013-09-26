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

        template<class FormatType>
        inline FormatType* format()
        {
            for (int i = 0; i < getNumFormats(); ++i)
                if (FormatType* fmt = dynamic_cast<FormatType*> (getFormat(i)))
                    return fmt;
            return nullptr;
        }

        KnownPluginList& allPlugins();

        void saveUserPlugins (Settings&);
        void restoreUser (Settings&);

    private:

        LV2World& lv2;

        class Private;
        Scoped<Private> priv;

    };

}

#endif // ELEMENT_PLUGIN_MANAGER_HPP
