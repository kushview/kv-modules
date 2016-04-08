/*
    This file is part of the element modules for the JUCE Library
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

#ifndef ELEMENT_H
#define ELEMENT_H

#include <juce/juce.h>
#include <element/modules/config.h>
#include <element/modules/version.h>

#if JUCE_MODULE_AVAILABLE_element_base
 #include <element/modules/element_base/element_base.h>
#endif

#if JUCE_MODULE_AVAILABLE_element_engines
 #include <element/modules/element_engines/element_engines.h>
#endif

#if JUCE_MODULE_AVAILABLE_element_gui
 #include <element/modules/element_gui/element_gui.h>
#endif

#if JUCE_MODULE_AVAILABLE_element_lv2
 #include <element/modules/element_lv2/element_lv2.h>
#endif

#if JUCE_MODULE_AVAILABLE_element_models
 #include <element/modules/element_models/element_models.h>
#endif

#endif
