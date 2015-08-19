/*
    world.cpp - This file is part of Element
    Copyright (C) 2014  Kushview, LLC.  All rights reserved.

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

static Module* world_load_module (const char* name)
{
    FileSearchPath emodPath (getenv ("ELEMENT_MODULE_PATH"));

    if (! (emodPath.getNumPaths() > 0))
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

typedef std::map<const String, Module*> ModuleMap;

class WorldBase::Private
{
public:
    Private() { }
    ~Private()
    {
        // kill all loaded modules
        OwnedArray<DynamicLibrary> libs;

		ModuleMap::iterator it = mods.begin();
        while (it != mods.end())
        {
            libs.add ((DynamicLibrary*) it->second->library);
            delete it->second;
        }

        mods.clear();

		for (int i = 0; i < libs.size(); ++i)
			libs.getUnchecked(i)->close();
        libs.clear (true);
    }

	
    std::map<const String, Module*> mods;
};


WorldBase::WorldBase()
{
    priv = new Private ();
}

WorldBase::~WorldBase()
{
    priv = nullptr;
}

int WorldBase::executeModule (const char* name)
{
    typedef std::map<const String, Module*> MAP;
    MAP::iterator mit = priv->mods.find (name);
    if (mit != priv->mods.end())
    {
        mit->second->run ((WorldData) this);
    }

    return -1;
}

bool WorldBase::loadModule (const char* name)
{
    ModuleMap::iterator it = priv->mods.find(name);
    if (it == priv->mods.end())
        return true;

    if (Module* mod = world_load_module (name))
    {
        mod->load (this);
        priv->mods.insert (std::make_pair (name, mod));
        return true;
    }

    return false;
}
