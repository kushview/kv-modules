/*
    PluginManager.cpp - This file is part of Element
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

#include "./PluginWrapper.h"

#if JUCE_MODULE_AVAILABLE_lvtk_plugins
using namespace lvtk;
#endif

    class PluginManager::Private
    {
    public:

        Private()
            : sampleRate (44100.0f),
              blockSize (1024)
        {  }

        ~Private() {  }

        KnownPluginList allPlugins;
        AudioPluginFormatManager formats;
        OptionalPtr<SymbolMap> symbols;

       #if JUCE_MODULE_AVAILABLE_lvtk_plugins
        OptionalPtr<LV2World> lv2;
       #endif

        double sampleRate;
        int    blockSize;
    };

    PluginManager::PluginManager()
    {
        priv = new Private();
      #if JUCE_MODULE_AVAILABLE_lvtk_plugins
        priv->symbols.setOwned (new SymbolMap ());
        priv->lv2.setOwned (new LV2World());
      #endif
    }

   #if JUCE_MODULE_AVAILABLE_lvtk_plugins
    PluginManager::PluginManager (LV2World& lv2)
    {
        priv = new Private();
        priv->lv2.setNonOwned (&lv2);
    }
   #endif

    PluginManager::~PluginManager()
    {
        priv = nullptr;
    }

    void PluginManager::addDefaultFormats()
    {
        formats().addDefaultFormats();
       #if JUCE_MODULE_AVAILABLE_lvtk_plugins
        formats().addFormat (new LV2PluginFormat (*priv->lv2));
       #endif
    }

    void
    PluginManager::addFormat (AudioPluginFormat* fmt)
    {
        formats().addFormat (fmt);
    }

    Processor*
    PluginManager::createPlugin (const PluginDescription &desc, String &errorMsg)
    {
        if (AudioPluginInstance* instance = formats().createPluginInstance (desc, priv->sampleRate, priv->blockSize, errorMsg))
        {
            if (Processor* plugin = dynamic_cast<Processor*> (instance))
            {
                return plugin;
            }
            else
            {
                return new PluginWrapper (instance);
            }
        }

        return nullptr;
    }

    AudioPluginFormat*
    PluginManager::format (const String& name)
    {
        for (int i = 0; i < formats().getNumFormats(); ++i)
        {
            AudioPluginFormat* fmt = priv->formats.getFormat (i);
            if (fmt && fmt->getName() == name)
                return fmt;
        }

        return nullptr;
    }

    AudioPluginFormatManager&
    PluginManager::formats()
    {
        return priv->formats;
    }


    KnownPluginList&
    PluginManager::availablePlugins()
    {
        return priv->allPlugins;
    }

    void
    PluginManager::saveUserPlugins (ApplicationProperties& settings)
    {
        ScopedXml elm (priv->allPlugins.createXml());
        settings.getUserSettings()->setValue ("plugin-list", elm.get());
    }

    void
    PluginManager::setPlayConfig (double sampleRate, int blockSize)
    {
        priv->sampleRate = sampleRate;
        priv->blockSize  = blockSize;
    }

    void
    PluginManager::restoreUserPlugins (ApplicationProperties& settings)
    {
        if (ScopedXml xml = settings.getUserSettings()->getXmlValue ("plugin-list"))
            restoreUserPlugins (*xml);
    }

    void
    PluginManager::restoreUserPlugins (const XmlElement& xml)
    {
        priv->allPlugins.recreateFromXml (xml);
    }
