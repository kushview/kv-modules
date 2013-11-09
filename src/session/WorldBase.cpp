/*
    world.cpp - This file is part of Element
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

#include <map>

#include "element/Juce.h"
#include "element/Pointer.h"
#include "element/Utils.h"

#include "element/session/Module.h"
#include "element/session/WorldBase.h"

using namespace std;

namespace Element {

    /* Function type for loading an Element module */
    typedef Module* (*ModuleLoadFunc)();

    static Module*
    world_load_module (const File& file)
    {
        if (! file.existsAsFile())
            return nullptr;

        DynamicLibrary* lib = new DynamicLibrary (file.getFullPathName());
        if (ModuleLoadFunc load_module = (ModuleLoadFunc) lib->getFunction ("element_module_load"))
        {
            Module* mod  = load_module();
            mod->library = lib;
            return mod;
        }

        if (lib) {
            delete lib;
        }

        return nullptr;
    }

    static Module*
    world_load_module (const char* name)
    {
        FileSearchPath emodPath (getenv ("ELEMENT_MODULE_PATH"));

        if (! emodPath.getNumPaths() > 0)
        {
            Logger::writeToLog ("[element] setting module path");
            File p ("/usr/local/lib/element/modules");
            emodPath.add (p);
        }

        Array<File> modules;
        String module = "";
        module << name << Module::extension();
        emodPath.findChildFiles (modules, File::findFiles, false, module);

        if (modules.size() > 0)
            return world_load_module (modules.getFirst());

        return nullptr;
    }

    class World::Private
    {
    public:
        Private()
            : settings (new Settings())
        { }

        ~Private()
        {
            settings = nullptr;

            // kill all loaded modules
            OwnedArray<DynamicLibrary> libs;
            for (auto& mod : mods)
            {
                libs.add (mod.second->library);
                delete mod.second;
            }

            mods.clear();

            for (DynamicLibrary* l : libs)
                l->close();
            libs.clear (true);

        }

        Shared<Engine> engine;
        map<const String, Module*> mods;
        ScopedPointer<Settings> settings;

    private:

    };


    World::World()
    {
        priv = new Private ();
    }

    World::~World()
    {
        priv = nullptr;
    }

    int
    World::executeModule (const char* name)
    {
        typedef map<const String, Module*> MAP;
        MAP::iterator mit = priv->mods.find (name);
        if (mit != priv->mods.end())
        {
            mit->second->run (this);
        }

        return -1;
    }

    bool
    World::loadModule (const char* name)
    {
        if (Element::contains (priv->mods, name))
            return true;

        if (Module* mod = world_load_module (name))
        {
            mod->load (this);
            priv->mods.insert (make_pair (name, mod));
            return true;
        }

        return false;
    }

    Settings&
    World::settings()
    {
        return *priv->settings;
    }


    Shared<Engine>
    World::engine()
    {
        return priv->engine;
    }

    void
    World::setEngine (Shared<Engine> e)
    {
        Shared<Engine> oe (priv->engine);
        priv->engine = e;
    }

}
