/*
    This file is part of the element modules for the JUCE Library
    Copyright (c) 2013 - Michael Fisher <mfisher31@gmail.com>.

    Permission to use, copy, modify, and/or distribute this software for any purpose with
    or without fee is hereby granted, provided that the above copyright notice and this
    permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
    TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
    NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
    DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
    IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
    CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef ELEMENT_JUCE_WORKTHREAD_H
#define ELEMENT_JUCE_WORKTHREAD_H

class WorkerBase;

/** A worker thread
    Capable of scheduling non-realtime work from a realtime context. */
class WorkThread :  public Thread
{
public:
    
    WorkThread (const String& name, uint32 bufsize, int32 priority = 5);
    ~WorkThread();
    
    inline static uint32 requiredSpace (uint32 msgSize) { return msgSize + (2 * sizeof (uint32)); }
    
protected:
    
    friend class WorkerBase;
    
    /** Register a worker for scheduling. Does not take ownership */
    void registerWorker (WorkerBase* worker);
    
    /** Deregister a worker from scheduling. Does not delete the worker */
    void removeWorker (WorkerBase* worker);
    
    /** Schedule non-realtime work
        Workers will call this in Worker::scheduleWork */
    bool scheduleWork (WorkerBase* worker, uint32 size, const void* data);

private:
    
    uint32 bufferSize;
    
    WorkerBase* getWorker (uint32 workerId) const;
    Array<WorkerBase*, CriticalSection> workers;
    
    uint32 nextWorkId;
    
    Semaphore sem;
    bool doExit;
    
    ScopedPointer<RingBuffer> requests;  ///< requests to process
    
    /** @internal Validate a ringbuffer for message completeness */
    bool validateMessage (RingBuffer& ring);
    
    /** @internal The work thread function */
    void run();
};

/** A flag that indicates whether work is happening or not */
class WorkFlag
{
public:
    WorkFlag() { flag = 0; }
    inline bool isWorking() const { return flag.get() != 0; }
    
private:
    Atomic<int32> flag;
    inline bool setWorking (bool status) { return flag.compareAndSetBool (status ? 1 : 0, status ? 0 : 1); }
    friend class WorkThread;
};

class WorkerBase
{
public:
    /** Create a new Worker
        @param thread The WorkThread to use when scheduling
        @param bufsize Size to use for internal response buffers */
    WorkerBase (WorkThread& thread, uint32 bufsize);
    virtual ~WorkerBase();
    
    /** Returns true if the worker is currently working */
    inline bool isWorking() const { return flag.isWorking(); }
    
    /** Schedule work (realtime thread).
        Work will be scheduled, and the thread will call Worker::processRequest
        when the data is queued */
    bool scheduleWork (uint32 size, const void* data);
    
    /** Respond from work (worker thread). Call this during processRequest if you
        need to send a response into the realtime thread. 
        @see processWorkResponses, @see processResponse */
    bool respondToWork (uint32 size, const void* data);
    
    /** Deliver pending responses (realtime thread)
        This must be called regularly from the realtime thread. For each read
        response, Worker::processResponse will be called */
    void processWorkResponses();
    
    /** Set the internal buffer size for responses */
    void setSize (uint32 newSize);
    
protected:
    /** Process work (worker thread) */
    virtual void processRequest (uint32 size, const void* data) = 0;
    
    /** Process work responses (realtime thread) */
    virtual void processResponse (uint32 size, const void* data) = 0;
    
private:
    WorkThread& owner;
    uint32 workId;                       ///< The thread assigned id for this worker
    WorkFlag flag;                       ///< A flag for when work is being processed
    
    ScopedPointer<RingBuffer> responses; ///< responses from work
    HeapBlock<uint8>          response;  ///< buffer to write a response
    
    bool validateMessage (RingBuffer& ring);
    
    friend class WorkThread;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WorkerBase);
};


#endif // ELEMENT_JUCE_WORKTHREAD_H
