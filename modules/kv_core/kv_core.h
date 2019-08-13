/*
    This file is part of the Kushview Modules for JUCE
    Copyright (C) 2014-2019  Kushview, LLC.  All rights reserved.

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

    BEGIN_JUCE_MODULE_DECLARATION

    ID:               kv_core
    vendor:           Kushview
    version:          0.0.1
    name:             KV Core
    description:      KV Core
    website:          https://kushview.net
    license:          GPL

    dependencies:     juce_core, juce_events

    END_JUCE_MODULE_DECLARATION
 */

#pragma once
#define KV_CORE_H_INCLUDED

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_cryptography/juce_cryptography.h>

#if JUCE_MODULE_AVAILABLE_juce_data_structures
 #include <juce_data_structures/juce_data_structures.h>
#endif

#if _MSC_VER
 #pragma warning( disable : 4305 )
 #pragma warning( disable : 4512 )
 #pragma warning( disable : 4996 )
 #if _MSC_VER >= 1800
  #include <atomic>
  #include <memory>
 #endif
#else
 #include <atomic>
#endif

#include <set>

#if _MSC_VER
 #ifdef min
  #undef min
 #endif
 #ifdef max
  #undef max
 #endif

 #if _MSC_VER < 1800
  #define llrint(x) juce::roundToInt(x)
 #endif
#endif

namespace kv {
using namespace juce;
#include "core/AudioRingBuffer.h"
#include "core/Arc.h"
#include "core/Atomic.h"
#include "core/LinkedList.h"
#include "core/MatrixState.h"
#include "core/MidiChannels.h"
#include "core/Monitor.h"
#include "core/Parameter.h"
#include "core/Pointer.h"
#include "core/PortType.h"
#include "core/RingBuffer.h"
#include "core/Semaphore.h"
#include "core/Slugs.h"
#include "core/Types.h"
#include "core/WorkThread.h"

#include "math/Rational.h"

#include "time/DelayLockedLoop.h"
#include "time/Tempo.h"
#include "time/TimeScale.h"
#include "time/TimeStamp.h"

#include "util/Utils.h"
#include "util/RangeTypes.h"

#if 0
#include "util/FileHelpers.h"
#include "util/RelativePath.h"
#include "util/UUID.h"
#endif

#include "interprocess/SlaveProcess.h"
}
