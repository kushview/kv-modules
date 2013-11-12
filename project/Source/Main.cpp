/*
    Main.cpp - This file is part of Element
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

#include <boost/bind.hpp>

#include <element/Juce.h>
#include <element/Pointer.h>
#include <element/Session.h>

#include "engine/AudioEngine.h"
#include "engine/InternalFormat.h"
#include "models/Session.h"

#include "gui/Alerts.h"
#include "gui/GuiApp.h"

#include "Globals.h"


namespace Element {

class Main  : public JUCEApplication
{

    Scoped<Globals>     world;
    Shared<AudioEngine> engine;
    Scoped<Gui::GuiApp> gui;

public:

   //==============================================================================
   Main()  {  }

   const String getApplicationName()       { return "BTV"; }
   const String getApplicationVersion()    { return "3.0.0"; }
   bool moreThanOneInstanceAllowed()       { return true; }

   //==============================================================================
   void initialise (const String& /* commandLine */)
   {
       int64 millis = Time::currentTimeMillis();

       // this isn't necessarily set in stone, just a quick setup for testing
       Logger::writeToLog ("Creating global data");
       world = new Globals();

       Settings& settings (world->settings());

       Logger::writeToLog ("Restoring device settings");
       {
           ScopedXml dxml (settings.getUserSettings()->getXmlValue ("devices"));
           world->devices().initialise (16, 16, dxml.get(), true, "default", nullptr);
       }

       Logger::writeToLog ("Creating engine");
       engine.reset (new AudioEngine (*world));
       world->setEngine (engine); // this will also instantiate the session

       // global data is ready, so now we can start using it;

       Logger::writeToLog ("Restoring plugin list");
       PluginManager& plugins (world->plugins());
       plugins.addDefaultFormats();
       plugins.addFormat (new InternalFormat (*engine));
       plugins.restoreUserPlugins (settings);

       Logger::writeToLog ("creating GUI");
       gui = Gui::GuiApp::create (*world);

       Logger::writeToLog ("Activate Audio Engine");
       engine->activate();

       Logger::writeToLog("Launch Gui");
       gui->run();
       Logger::writeToLog("Gui launched");

       millis = Time::currentTimeMillis() - millis;

       String timeMsg ("\nBTV started in ");
       timeMsg << ((float)millis / 1000.f) << " seconds.";
       Logger::writeToLog (timeMsg);
   }


   void timerCallback()
   {

   }

   void shutdown()
   {
       PluginManager& plugins (world->plugins());
       Settings& settings (world->settings());
       plugins.saveUserPlugins (settings);

       if (ScopedXml el = world->devices().createStateXml())
           settings.getUserSettings()->setValue ("devices", el);

       engine->deactivate();
       world->setEngine (Shared<Engine>());
       engine.reset();

       std::clog << "Going away with " << engine.use_count() << " AudioEngine refs out there\n";

       world = nullptr;
   }

   //==============================================================================
   void
   systemRequestedQuit()
   {
       if (gui->shutdownApp())
       {
           gui = nullptr;
           this->quit();
       }
   }

   void
   anotherInstanceStarted (const String& commandLine)
   {

   }

};

} /* namespace Element */

START_JUCE_APPLICATION (Element::Main)
