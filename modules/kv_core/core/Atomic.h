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

#pragma once

#if 1
template<typename ValueType>
class AtomicValue
{
public:
    explicit AtomicValue (ValueType initial = ValueType())
        : state (ReadWrite)
    {
        values[0] = values[1] = initial;
        readValue = &values [0];
    }

    inline const ValueType& get() const { return *readValue.load(); }

    inline bool
    set (ValueType newValue)
    {
        State expected = ReadWrite;
        if (state.compare_exchange_strong (expected, ReadLock))
        {
            values[1]  = newValue;
            readValue  = &values[1];
            state      = WriteRead;
            return true;
        }

        expected = WriteRead;

        if (state.compare_exchange_strong (expected, LockRead))
        {
            values[0] = newValue;
            readValue = &values[0];
            state     = ReadWrite;
            return true;
        }

        return false;
    }

    inline ValueType
    exchange (ValueType newValue)
    {
        ValueType existingValue = get();

        while (! this->set (newValue))
            ; // spin a little

        return existingValue;
    }

    inline void
    exchange (ValueType nextValue, ValueType& previousValue)
    {
        previousValue = exchange (nextValue);
    }

    inline void
    exchangeAndDelete (ValueType nextValue)
    {
        ValueType ptr = exchange (nextValue);
        if (ptr != nullptr)
            delete ptr;
    }

private:
    enum State
    {
        ReadWrite,
        ReadLock,
        WriteRead,
        LockRead
    };

    std::atomic<State>         state;
    std::atomic<ValueType*>    readValue;
    ValueType                  values[2];
};


class AtomicLock
{
public:

    AtomicLock()
        : a_mutex(),
          a_locks(0)
    { }

    inline bool acquire()
    {
        return ! a_mutex.test_and_set (std::memory_order_acquire);
    }

    inline void release()
    {
        a_mutex.clear (std::memory_order_release);
    }

    inline void lock()
    {
        a_locks.set (a_locks.get() + 1);
        if (a_locks.get() == 1)
            while (! acquire())
                ; // spin
    }

    inline void unlock()
    {
        a_locks.set (a_locks.get() - 1);
        if (a_locks.get() < 1)
        {
            a_locks.set(0);
            release();
        }
    }

    inline bool isBusy() const { return a_locks.get() > 0; }

private:
    std::atomic_flag    a_mutex;
    AtomicValue<int>    a_locks;
};

#else //MSVC

template<typename ValueType>
class AtomicValue
{
public:
    explicit AtomicValue (ValueType initial = ValueType())
        : state (ReadWrite)
    {
        values[0] = values[1] = initial;
        readValue.set(&values [0]);
    }

	inline ValueType get() const { return *readValue.get(); }

    inline bool set (ValueType newValue)
    {
        State expected = ReadWrite;
		if (expected == state.compareAndSetValue (expected, ReadLock))
        {
            values[1]  = newValue;
            readValue.set(&values[1]);
            state      = WriteRead;
            return true;
        }

        expected = WriteRead;

		if (expected == state.compareAndSetValue (expected, LockRead))
        {
            values[0] = newValue;
            readValue.set(&values[0]);
            state     = ReadWrite;
            return true;
        }

        return false;
    }

    inline ValueType
    exchange (ValueType newValue)
    {
        ValueType existingValue = get();

        while (! this->set (newValue))
            ; // spin a little

        return existingValue;
    }

    inline void
    exchange (ValueType nextValue, ValueType& previousValue)
    {
        previousValue = exchange (nextValue);
    }

    inline void
    exchangeAndDelete (ValueType nextValue)
    {
        ValueType ptr = exchange (nextValue);
        if (ptr != nullptr)
            delete ptr;
    }

private:

    enum State
    {
        ReadWrite,
        ReadLock,
        WriteRead,
        LockRead
    };

    Atomic<State>         state;
    Atomic<ValueType*>    readValue;
    ValueType             values[2];
};

#if 1

class AtomicLock
{
public:
    AtomicLock() { }

    inline bool acquire()
    {
		return a_mutex.set(1);
    }

    inline void release()
    {
       while (! a_mutex.set(0))
			;
    }

    inline void lock()
    {
        a_locks.set (a_locks.get() + 1);
        if (a_locks.get() == 1)
            while (! acquire())
                ; // spin
    }

    inline void unlock()
    {
        a_locks.set (a_locks.get() - 1);
        if (a_locks.get() < 1)
        {
            a_locks.set(0);
            release();
        }
    }

    inline bool isBusy() const {  return a_locks.get() > 0; }

private:
    AtomicValue<int>    a_mutex;
    AtomicValue<int>    a_locks;
};

#endif
#endif
