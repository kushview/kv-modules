/*
    This file is part of the Kushview Modules for JUCE
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

namespace LV2Callbacks
{
    static LV2_Worker_Status scheduleWork (LV2_Worker_Schedule_Handle handle, uint32_t size, const void* data)
    {
        LV2Worker* worker = reinterpret_cast<LV2Worker*> (handle);
        if (! worker->scheduleWork (size, data))
            return LV2_WORKER_ERR_NO_SPACE;
        return LV2_WORKER_SUCCESS;
    }

    static LV2_Worker_Status workRespond (LV2_Worker_Respond_Handle handle, uint32_t size, const void* data)
    {
        LV2Worker* worker = reinterpret_cast<LV2Worker*> (handle);
        if (! worker->respondToWork (size, data))
            return LV2_WORKER_ERR_NO_SPACE;
        return LV2_WORKER_SUCCESS;
    }
}

LV2Worker::LV2Worker (WorkThread& thread, uint32 bufsize,
                      LV2_Handle handle,
                      LV2_Worker_Interface* iface)
    : WorkerBase (thread, bufsize)
{
    setInterface (handle, iface);

    uri = LV2_WORKER__schedule;
    feat.URI  = uri.toRawUTF8();
    data.handle = this;
    data.schedule_work = &LV2Callbacks::scheduleWork;
    feat.data = (void*) &data;
}

LV2Worker::~LV2Worker()
{
    plugin = nullptr;
    worker = nullptr;
    zerostruct (feat);
    zerostruct (data);
    uri = String::empty;
}

void LV2Worker::setInterface (LV2_Handle handle, LV2_Worker_Interface* iface)
{
    plugin = handle;
    worker = iface;
}

const String& LV2Worker::getURI() const { return uri; }
const LV2_Feature* LV2Worker::getFeature() const { return &feat; }

void LV2Worker::processRequest (uint32 size, const void* requestData)
{
    jassert (worker != nullptr && plugin != nullptr);
    worker->work (plugin, LV2Callbacks::workRespond, this, size, requestData);
}

void LV2Worker::processResponse (uint32 size, const void* responseData)
{
    jassert (worker != nullptr && plugin != nullptr);
    worker->work_response (plugin, size, responseData);
}

void LV2Worker::endRun()
{
    jassert (worker != nullptr && plugin != nullptr);
    if (worker->end_run)
        worker->end_run (plugin);
}
