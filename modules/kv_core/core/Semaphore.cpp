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

#ifdef __APPLE__

inline
Semaphore::Semaphore()
{
    init(0);
}

inline
Semaphore::Semaphore(unsigned initial)
{
    init(initial);
}

inline
Semaphore::~Semaphore()
{
    destroy();
}

inline bool
Semaphore::init(unsigned /* initial */)
{
    return semaphore_create(mach_task_self(), &semaphore, SYNC_POLICY_FIFO, 0)
        ? false : true;
}

inline void
Semaphore::destroy()
{
    semaphore_destroy(mach_task_self(), semaphore);
}

inline void
Semaphore::post()
{
    semaphore_signal(semaphore);
}

inline void
Semaphore::wait()
{
    semaphore_wait(semaphore);
}

inline bool
Semaphore::tryWait()
{
    const mach_timespec_t zero = { 0, 0 };
    return semaphore_timedwait(semaphore, zero) == KERN_SUCCESS;
}

#elif defined(_WIN32) || defined(_WIN64)

inline Semaphore::Semaphore()
{
    init (0);
}

inline bool Semaphore::init(unsigned initial)
{
    semaphore = CreateSemaphore (NULL, initial, LONG_MAX, NULL);
    return (semaphore) ? false : true;
}

inline Semaphore::~Semaphore()
{
    CloseHandle(semaphore);
}

inline void
Semaphore::post()
{
    ReleaseSemaphore(semaphore, 1, NULL);
}

inline void
Semaphore::wait()
{
    WaitForSingleObject(semaphore, INFINITE);
}

inline bool
Semaphore::tryWait()
{
    WaitForSingleObject(semaphore, 0);
}

#else  /* !defined(__APPLE__) && !defined(_WIN32) */


inline
Semaphore::Semaphore() { init(0); }


inline
Semaphore::Semaphore(unsigned initial)
{
    init(initial);
}


inline bool
Semaphore::init(unsigned initial)
{
    return sem_init(&semaphore, 0, initial) ? false : true;
}


inline
Semaphore::~Semaphore()
{
    sem_destroy(&semaphore);
}


inline void
Semaphore::post()
{
    sem_post(&semaphore);
}


inline void
Semaphore::wait()
{
    /* Note that sem_wait always returns 0 in practice, except in
    gdb (at least), where it returns nonzero, so the while is
    necessary (and is the correct/safe solution in any case).
    */
    while (sem_wait(&semaphore) != 0) {}
}

inline bool
Semaphore::tryWait()
{
    return (sem_trywait(&semaphore) == 0);
}

#endif
