/*
    This file is part of the Kushview Modules for JUCE
    Copyright (C) 2014-2017  Kushview, LLC.  All rights reserved.

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

#if defined (KV_CORE_H_INCLUDED) && ! JUCE_AMALGAMATED_INCLUDE
/* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of JUCE cpp file"
#endif

#include <map>
#include <juce_core/juce_core.h>
#include "kv_core.h"

#if JUCE_WINDOWS
 #include <windows.h>
#endif

namespace kv {
 #include "core/Arc.cpp"
 #include "core/MatrixState.cpp"
 #include "core/RingBuffer.cpp"
 #include "core/Semaphore.cpp"
 #include "core/WorkThread.cpp"
 #include "time/TimeScale.cpp"
 #include "util/FileHelpers.cpp"
 #include "util/UUID.cpp"
 #include "interprocess/SlaveProcess.cpp"
}
