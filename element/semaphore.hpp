/*
    semaphore.hpp - This file is part of Element

    Copyright 2012 David Robillard <http://drobilla.net>
    Adapted 2013 Michael Fisher <mfisher@bketech.com>
     * Converted to a C++ class

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef ELEMENT_SEMAPHORE_HPP
#define ELEMENT_SEMAPHORE_HPP

#ifdef __APPLE__
#    include <mach/mach.h>
     typedef semaphore_t SemType;
#elif defined(_WIN32)
#    include <windows.h>
     typedef HANDLE SemType;
#else
#    include <semaphore.h>
     typedef sem_t SemType;
#endif

namespace Element {

/**
   A counting semaphore.

   This is an integer that is always positive, and has two main operations:
   increment (post) and decrement (wait).  If a decrement can not be performed
   (i.e. the value is 0) the caller will be blocked until another thread posts
   and the operation can succeed.

   Semaphores can be created with any starting value, but typically this will
   be 0 so the semaphore can be used as a simple signal where each post
   corresponds to one wait.

   Semaphores are very efficient (much moreso than a mutex/cond pair).  In
   particular, at least on Linux, post is async-signal-safe, which means it
   does not block and will not be interrupted.  If you need to signal from
   a realtime thread, this is the most appropriate primitive to use.
*/

struct Semaphore
{

/** Create a semaphore. */
Semaphore();
Semaphore (unsigned initial);

~Semaphore();

bool init (unsigned initial);

/** Destroy the semaphore */
void destroy();

/** Increment (and signal any waiters).
    Realtime safe. */
void post();

/** Wait until count is > 0 */
void wait();

/** Non-blocking version of wait().
    @return true if decrement was successful (lock was acquired). */
bool tryWait();

private:
  SemType semaphore;

};

#ifdef __APPLE__

inline
Semaphore::Semaphore()
{
	init (0);
}

inline
Semaphore::Semaphore (unsigned initial)
{
	init (initial);
}

inline
Semaphore::~Semaphore()
{
	destroy();
}

inline bool
Semaphore::init (unsigned /* initial */)
{
    return semaphore_create (mach_task_self(), &semaphore, SYNC_POLICY_FIFO, 0)
          ? false : true;
}

inline void
Semaphore::destroy()
{
    semaphore_destroy (mach_task_self(), semaphore);
}

inline void
Semaphore::post()
{
  semaphore_signal (semaphore);
}

inline void
Semaphore::wait()
{
  semaphore_wait (semaphore);
}

inline bool
Semaphore::tryWait()
{
  const mach_timespec_t zero = { 0, 0 };
  return semaphore_timedwait (semaphore, zero) == KERN_SUCCESS;
}

#elif defined(_WIN32)

inline
Semaphore::Semaphore() {}

inline bool
Semaphore::init(unsigned initial)
{
  semaphore = CreateSemaphore(NULL, initial, LONG_MAX, NULL);
  return (semaphore) ? false : true;
}

inline void
Semaphore::~ContedSemaphore()
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
Semaphore::try_wait()
{
  WaitForSingleObject(semaphore, 0);
}

#else  /* !defined(__APPLE__) && !defined(_WIN32) */


inline
Semaphore::Semaphore() { init(0); }


inline
Semaphore::Semaphore (unsigned initial)
{
  init (initial);
}


inline bool
Semaphore::init(unsigned initial)
{
  return sem_init (&semaphore, 0, initial) ? false : true;
}


inline
Semaphore::~Semaphore()
{
  sem_destroy (&semaphore);
}


inline void
Semaphore::post()
{
  sem_post (&semaphore);
}


inline void
Semaphore::wait()
{
  /* Note that sem_wait always returns 0 in practice, except in
     gdb (at least), where it returns nonzero, so the while is
     necessary (and is the correct/safe solution in any case).
  */
  while (sem_wait (&semaphore) != 0) { }
}

inline bool
Semaphore::try_wait()
{
  return (sem_trywait (&semaphore) == 0);
}

#endif

} /* namespace element */

#endif  /* ELEMENT_SEMAPHORE_HPP */
