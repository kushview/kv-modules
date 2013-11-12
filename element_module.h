/*
    element_module.h - This file is part of Element
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

#ifndef ELEMENT_CORE_H_INCLUDED
#define ELEMENT_CORE_H_INCLUDED

#ifndef JUCE_MODULE_AVAILABLE_element
 /* If you fail to make sure that all your compile units are building JUCE with the same set of
    option flags, then there's a risk that different compile units will treat the classes as having
    different memory layouts, leading to very nasty memory corruption errors when they all get
    linked together. That's why it's best to always include the Introjucer-generated AppConfig.h
    file before any juce headers.

    Note that if you do have an AppConfig.h file and hit this warning, it means that it doesn't
    contain the JUCE_MODULE_AVAILABLE_xxx flags, which are necessary for some inter-module
    functionality to work correctly. In that case, you should either rebuild your AppConfig.h with
    the latest introjucer, or fix it manually to contain these flags.
 */
 #ifdef _MSC_VER
  #pragma message ("Have you included your AppConfig.h file before including the JUCE headers?")
 #else
  #warning "Have you included your AppConfig.h file before including the JUCE headers?"
 #endif
#endif

/** Config: ELEMENT_PLUGINHOST_LV2

    Enable classes for hosting LV2 plugins
*/
#ifndef ELEMENT_PLUGINHOST_LV2
    #define ELEMENT_PLUGINHOST_LV2 1
#endif


/** Config: ELEMENT_DOCKING

    Build the docking widgets classes
*/
#ifndef ELEMENT_DOCKING
    #define ELEMENT_DOCKING 1
#endif

/** Config: ELEMENT_JACK_DEVICE

    Build the docking widgets classes
*/
#ifndef ELEMENT_JACK_DEVICE
    #define ELEMENT_JACK_DEVICE 0
#endif

/** Config: ELEMENT_SESSION

    Build the Session/World base classes
*/
#ifndef ELEMENT_SESSION
    #define ELEMENT_SESSION 1
#endif

namespace Element { }

// Header include'ing is handled in implementation files.  This is so the library
// can be built and installed like a normal system library with a pkg-config file.
// The catch is that you MUST include the element source tree in your include path
// e.g. -I/path/to/element/sources

#endif   // ELEMENT_MODELS_H_INCLUDED
