/*
    WorldBase.h - This file is part of Element
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

#ifndef ELEMENT_WORLD_H
#define ELEMENT_WORLD_H

class Engine;

/** A global collection of an appilcation/plugin's data */
class WorldBase {
public:

    WorldBase();
    virtual ~WorldBase();

    virtual bool loadModule (const char* moduleName);
    virtual int executeModule (const char* entryModule);

    Shared<Engine>  engine();
    virtual void setEngine (Shared<Engine> engine);

    void setAppName (const String& name) { appName = name; }
    virtual const String& getAppName() const { return appName; }

private:

    class Private;
    Scoped<Private> priv;
    String appName;

};



#endif // ELEMENT_WORLD_H
