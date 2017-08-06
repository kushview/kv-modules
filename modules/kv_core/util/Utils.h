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

/** Convenient typdef for a string vector */
typedef std::vector<std::string> StringVec;

/** Value type to for hashing things
        @note compatible with std/boost hashing */
typedef size_t HashValue;


/** Restrict a value between a lower and upper value */
template<typename VAL> static inline VAL
clampNoMoreThan (const VAL& input, const VAL& lower, const VAL& upper)
{
    if (input < lower) {
        return lower;
    }

    if (input > upper) {
        return upper;
    }

    return input;
}

/** Restrict a value between a lower and upper value */
template<typename VAL> static inline VAL
clampBelow (const VAL& input, const VAL& lower, const VAL& upper)
{
    if (input < lower) {
        return lower;
    }

    if (input >= upper) {
        return upper - 1;
    }

    return input;
}

template<typename VAL> static inline bool
isWithinRange (const VAL& input, const VAL& lower, const VAL& upper)
{
    return (input >= lower && input < upper);
}

template<typename SEQ>
static int64_t
indexOf (SEQ& seq, typename SEQ::value_type const& val)
{
    int index = 0;
    for (typename SEQ::value_type const& i : seq)
        if (i == val)
            return index;
        else
            ++index;

    return -1;
}

/** Shortcut for containment test on a map
       lumiera.org - Hermann Vosseler <Ichthyostega@web.de> */
template <typename MAP>
inline bool
contains (MAP& map, typename MAP::key_type const& key)
{
    return map.find(key) != map.end();
}

/** Shortcut for set value containment test
       lumiera.org - Hermann Vosseler <Ichthyostega@web.de> */
template <typename T>
inline bool
contains (std::set<T>& set, T const& val)
{
    return set.end() != set.find (val);
}

/** Shortcut for string value containment test
        lumiera.org - Hermann Vosseler <Ichthyostega@web.de> */
template <typename T> inline bool
contains (std::string const& str, const T& val)
{
    return str.find (val) != std::string::npos;
}

/** Shortcut for brute-force containment test in any sequential container */
template <typename SEQ> inline bool
contains (SEQ const& cont, typename SEQ::const_reference val)
{
    typename SEQ::const_iterator begin = cont.begin();
    typename SEQ::const_iterator end   = cont.end();
    return end != std::find (begin, end, val);
}

/** Shortcut for removing all copies of an Element in any sequential collection */
template <typename SEQ> inline typename SEQ::iterator
removeall (SEQ& coll, typename SEQ::value_type& val)
{
    typename SEQ::iterator collEnd = coll.end();
    return coll.erase (std::remove (coll.begin(), collEnd, val), collEnd);
}

template <typename SEQ> inline typename SEQ::iterator
removeall (SEQ& coll, const typename SEQ::value_type& val)
{
    typename SEQ::iterator collEnd = coll.end();
    return coll.erase (std::remove (coll.begin(), collEnd, val), collEnd);
}

/** Accumulate a hash value
        @param accum Value to add to
        @param changed New data to hash
        @return Combined Hash Value */
template<typename VAL> inline HashValue
chainedHash (HashValue accum, VAL changed)
{
    std::hash<HashValue> h1;
    HashValue seed = h1 (changed);
    accum = h1 (seed);
    return accum;
}

/** Create a hash value from a string
        @return The HashValue */
inline HashValue
hashString (const std::string& str)
{
    std::hash<std::string> hashit;
    return hashit (str);
}
