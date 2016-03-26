/*
    This file is part of the lvtk_plugins JUCE module
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

namespace LV2Callbacks {

    int vprintf (LV2_Log_Handle handle, LV2_URID type, const char* fmt, va_list ap)
    {
        // TODO: Lock
        return std::vfprintf (stderr, fmt, ap);
    }
    
    int printf (LV2_Log_Handle handle, LV2_URID type, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        const int ret = LV2Callbacks::vprintf (handle, type, fmt, args);
        va_end(args);
        return ret;
    }
}

LV2Log::LV2Log()
{
    uri = LV2_LOG__log;
    feat.URI    = uri.toRawUTF8();
    log.handle  = this;
    log.printf  = &LV2Callbacks::printf;
    log.vprintf = &LV2Callbacks::vprintf;
    feat.data   = (void*) &log;
}

LV2Log::~LV2Log()
{
    
}
