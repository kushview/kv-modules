/*
    plugin-manager.cpp - This file is part of Element
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

#include "element/session/plugin-manager.hpp"
#include "element/session/world.hpp"
#include "lv2format.hpp"

namespace Element {


    class PluginManager::Private
    {
    public:
        Private()  {  }
        ~Private() {  }

        KnownPluginList allPlugins;

    };

    PluginManager::PluginManager (LV2World& lv2_)
        : lv2 (lv2_)
    {
        priv.reset (new Private());
        addFormat (new LV2PluginFormat (lv2_));
    }

    PluginManager::~PluginManager()
    {
        priv.reset (nullptr);
    }

    AudioPluginFormat*
    PluginManager::format (const String& name)
    {
        for (int i = 0; i < getNumFormats(); ++i)
        {
            AudioPluginFormat* fmt = getFormat(i);
            if (fmt && fmt->getName() == name)
                return fmt;
        }

        return nullptr;
    }

    KnownPluginList&
    PluginManager::allPlugins()
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
