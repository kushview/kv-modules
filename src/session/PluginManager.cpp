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

#include "element/formats/lv2/LV2PluginFormat.h"
#include "element/formats/lv2/LV2World.h"
#include "element/session/PluginManager.h"
#include "element/session/WorldBase.h"
#include "element/SymbolMap.h"

namespace Element {


    class PluginManager::Private
    {
    public:
        Private()  {  }
        ~Private() {  }

        KnownPluginList allPlugins;
        AudioPluginFormatManager formats;
        OptionalPtr<LV2World> lv2;
        OptionalPtr<SymbolMap> symbols;

    };

    PluginManager::PluginManager()
    {
        priv = new Private();
        priv->symbols.setOwned (new SymbolMap());
        priv->lv2.setOwned (new LV2World (*priv->symbols));
    }

    PluginManager::PluginManager (LV2World& lv2)
    {
        priv = new Private();
        priv->lv2.setNonOwned (&lv2);
    }

    PluginManager::~PluginManager()
    {
        priv = nullptr;
    }

    void PluginManager::addDefaultFormats()
    {
        formats().addDefaultFormats();
        formats().addFormat (new LV2PluginFormat (*priv->lv2));
    }

    void
    PluginManager::addFormat (AudioPluginFormat* fmt)
    {
        formats().addFormat (fmt);
    }

    Processor*
    PluginManager::createPlugin (const PluginDescription &desc, String &errorMsg)
    {
        if (AudioPluginInstance* plugin = formats().createPluginInstance (desc, 44100.f, 1024, errorMsg))
            return dynamic_cast<Processor*> (plugin);
        return nullptr;
    }

#if 0
    template<class FormatType>
    FormatType* PluginManager::format()
    {
        for (int i = 0; i < formats().getNumFormats(); ++i)
            if (FormatType* fmt = dynamic_cast<FormatType*> (formats().getFormat (i)))
                return fmt;
        return nullptr;
    }
#endif
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
    PluginManager::saveUserPlugins (Settings& settings)
    {
        ScopedXml elm (priv->allPlugins.createXml());
        settings.getUserSettings()->setValue ("plugin-list", elm.get());
    }

    void
    PluginManager::restoreUser (Settings& settings)
    {
        ScopedXml xml (settings.getUserSettings()->getXmlValue ("plugin-list"));
        if (xml)
            priv->allPlugins.recreateFromXml (*xml);
    }
}
