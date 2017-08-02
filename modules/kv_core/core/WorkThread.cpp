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

#if _MSC_VER
 #pragma warning( disable : 4127 )
#endif

#if JUCE_DEBUG
#define KV_WORKER_LOG(x) Logger::writeToLog(x)
#else
#define KV_WORKER_LOG(x)
#endif

WorkThread::WorkThread (const String& name, uint32 bufsize, int32 priority)
    : Thread (name)
{
    nextWorkId = 0;
    bufferSize = (uint32) nextPowerOfTwo (bufsize);
    requests   = new RingBuffer (bufferSize);
    startThread (priority);
}

WorkThread::~WorkThread()
{
    signalThreadShouldExit();
    doExit = true;
    sem.post();
    waitForThreadToExit (100);
    requests = nullptr;
}

WorkerBase* WorkThread::getWorker (uint32 workerId) const
{
    if (workerId == 0)
        return nullptr;

    for (int i = 0; i < workers.size(); ++i)
        if (workers.getUnchecked(i)->workId == workerId)
            return workers.getUnchecked (i);

    return nullptr;
}


void WorkThread::registerWorker (WorkerBase* worker)
{
    worker->workId = ++nextWorkId;
    KV_WORKER_LOG (getThreadName() + " Registering worker: id = " + String (worker->workId));
    workers.addIfNotAlreadyThere (worker);
}

void WorkThread::removeWorker (WorkerBase* worker)
{
    KV_WORKER_LOG (getThreadName() + " Removing worker: id = " + String (worker->workId));
    workers.removeFirstMatchingValue (worker);
    worker->workId = 0;
}


void WorkThread::run()
{
    HeapBlock<uint8> buffer;
    int32 readBufferSize = 0;

    while (true)
    {
        sem.wait();
        if (doExit || threadShouldExit()) break;

        while (! validateMessage (*requests))
            Thread::sleep (6);

        if (doExit || threadShouldExit()) break;

        uint32 size = 0;
        if (requests->read (&size, sizeof (size)) < sizeof (size))
        {
            KV_WORKER_LOG ("WorkThread: error reading request: message size");
            continue;
        }

        uint32 workId;
        if (requests->read (&workId, sizeof (workId)) < sizeof (workId))
        {
            KV_WORKER_LOG ("WorkThread: error reading request: worker id");
            continue;
        }

        if (workId == 0)
            continue;

        if (size > static_cast<uint32> (readBufferSize))
        {
			readBufferSize = nextPowerOfTwo (size);
            buffer.realloc (readBufferSize);
        }

        if (requests->read (buffer.getData(), size) < size)
        {
            KV_WORKER_LOG (getThreadName() + ": error reading request: message body");
            continue;
        }

        KV_WORKER_LOG (getThreadName() + ": Finding Worker ID " + String (workId));

        {
            if (WorkerBase* const worker = getWorker (workId))
            {
                while (! worker->flag.setWorking (true)) {}
                worker->processRequest (size, buffer.getData());
                while (! worker->flag.setWorking (false)) {}
            }
        }

        if (threadShouldExit() || doExit)
            break;
    }

    KV_WORKER_LOG (getThreadName() + ": thread exited.");

    buffer.free();
}


bool WorkThread::scheduleWork (WorkerBase* worker, uint32 size, const void* data)
{
    jassert (size > 0 && worker && worker->workId != 0);
    if (! requests->canWrite (requiredSpace (size)))
        return false;

    if (requests->write (&size, sizeof(size)) < sizeof (uint32))
        return false;

    if (requests->write (&worker->workId, sizeof (worker->workId)) < sizeof (worker->workId))
        return false;

    if (requests->write (data, size) < size)
        return false;

    sem.post();
    return true;
}

bool WorkThread::validateMessage (RingBuffer& ring)
{
    uint32 size = 0;
    ring.peak (&size, sizeof (size));
    return ring.canRead (requiredSpace (size));
}

WorkerBase::WorkerBase (WorkThread& thread, uint32 bufsize)
    : owner (thread)
{
    responses = new RingBuffer (bufsize);
    response.calloc (bufsize);
    thread.registerWorker (this);
}

WorkerBase::~WorkerBase()
{
    while (flag.isWorking()) {
        Thread::sleep (100);
    }

    owner.removeWorker (this);
    responses = nullptr;
    response.free();
}

bool WorkerBase::scheduleWork (uint32 size, const void* data)
{
    return owner.scheduleWork (this, size, data);
}

bool WorkerBase::respondToWork (uint32 size, const void* data)
{
    if (! responses->canWrite (sizeof (size) + size))
        return false;

    if (responses->write (&size, sizeof (size)) < sizeof (size))
        return false;

    if (responses->write (data, size) < size)
        return false;

    return true;
}

void WorkerBase::processWorkResponses()
{
    uint32 remaining = responses->getReadSpace();
    uint32 size      = 0;

    while (remaining >= sizeof (uint32))
    {
        /* respond next cycle if response isn't ready */
        if (! validateMessage (*responses))
            return;

        responses->read (&size, sizeof (size));
        responses->read (response.getData(), size);
        processResponse (size, response.getData());
        remaining -= (sizeof (uint32) + size);
    }
}

bool WorkerBase::validateMessage (RingBuffer& ring)
{
    // the worker only validates message size
    uint32 size = 0;
    ring.peak (&size, sizeof(size));
    return ring.canRead (size + sizeof(size));
}

void WorkerBase::setSize (uint32 newSize)
{
    responses = new RingBuffer (newSize);
    response.realloc (newSize);
}
