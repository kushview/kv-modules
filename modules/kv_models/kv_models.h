/*
    kv_models.h - This file is part of the Kushview Modules for JUCE
    Copyright (c) 2014-2019  Kushview, LLC.  All rights reserved.

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

/*
  BEGIN_JUCE_MODULE_DECLARATION

  ID:               kv_models
  vendor:           Kushview, LLC
  version:          0.0.1
  name:             Element Models
  description:      Element support for hosting and writing LV2 plugins
  website:          https://kushview.net
  license:          GPL

  dependencies:     kv_core, juce_data_structures

  END_JUCE_MODULE_DECLARATION
 */

#ifndef KV_MODELS_H_INCLUDED
#define KV_MODELS_H_INCLUDED

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_data_structures/juce_data_structures.h>
#include <kv_core/kv_core.h>

namespace kv {

#include "models/ObjectModel.h"

}

#endif   // KV_MODELS_H_INCLUDED
