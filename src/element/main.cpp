/*
    main.cpp - This file is part of Element
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

#include "element/juce.hpp"
#include "element/session.hpp"

namespace element {

   class Main  : public JUCEApplication
   {

       ScopedPointer<World> world;

   public:

      //==============================================================================
      Main() {  }

      const String getApplicationName()       { return "Element"; }
      const String getApplicationVersion()    { return "0.0.1"; }
      bool moreThanOneInstanceAllowed()       { return false; }

      //==============================================================================
      void initialise (const String& /* commandLine */)
      {
          world = new World();
          DeviceManager& devs (world->devices());
          devs.initialise (2, 2, nullptr, true);

          StringArray required;
          required.add ("audio-engine");
          required.add ("jack-device");
          required.add ("element-ui");

          String maybeError;
          for (const String& mod : required)
          {
              maybeError = "failed to load required module: ";
              maybeError << mod;

              if (! world->loadModule (mod.toRawUTF8())) {
                  Logger::writeToLog (maybeError);
                  world = nullptr;
                  quit();
              }
          }

          if (world)
                world->execute ("element-ui");

      }

      void timerCallback() {  }

      void shutdown()
      {
         Logger::writeToLog ("shutdown");
         world = nullptr;
      }

      //==============================================================================
      void systemRequestedQuit()
      {
          Logger::writeToLog ("requested quit");
          this->quit();
      }

      void anotherInstanceStarted (const String& commandLine) { }

   };
}

START_JUCE_APPLICATION (element::Main)
