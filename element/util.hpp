/*
  util.hpp

  Copyright (C) 2013  Michael R Fisher <mfisher31@gmail.com>

  This is free software: you can redistribute it and/or modify it under the
  terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or any later version.

  It is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this software.  If not, see <http://www.gnu.org/licenses/>.

*/
/** @file util.hpp */

#ifndef ELEMENT_UTIL_HPP
#define ELEMENT_UTIL_HPP

#include <sys/stat.h>

#include <algorithm>
#include <functional>
#include <set>
#include <string>
#include <vector>

namespace Element {

    /** Convenient typdef for a string vector */
    typedef std::vector<std::string> StringVec;

    /** Value type to for hashing things
        @note compatible with std/boost hashing */
    typedef size_t HashValue;


    /** Restrict a value between a lower and upper value */
    template<typename VAL> static inline VAL
    clamp (const VAL& input, const VAL& lower, const VAL& upper)
    {
        if (input < lower) {
            return lower;
        }

        if (input >= upper) {
            return upper;
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
    index_of (SEQ& seq, typename SEQ::value_type const& val)
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
   template <typename T>
   inline bool
   contains (std::string const& str, const T& val)
   {
       return str.find (val) != std::string::npos;
   }

    /**
       Shortcut for brute-force containment test in any sequential container
       lumiera.org - Hermann Vosseler <Ichthyostega@web.de>
     */
    template <typename SEQ>
    inline bool
    contains (SEQ const& cont, typename SEQ::const_reference val)
    {
      typename SEQ::const_iterator begin = cont.begin();
      typename SEQ::const_iterator end   = cont.end();

      return end != std::find (begin, end, val);
    }

    /**
       Shortcut for removing all copies of an Element in any sequential collection
       lumiera.org - Hermann Vosseler <Ichthyostega@web.de>
     */
    template <typename SEQ>
    inline typename SEQ::iterator
    removeall (SEQ& coll, typename SEQ::value_type& val)
    {
        typename SEQ::iterator collEnd = coll.end();
        return coll.erase (std::remove (coll.begin(), collEnd, val), collEnd);
    }

    template <typename SEQ>
    inline typename SEQ::iterator
    removeall (SEQ& coll, const typename SEQ::value_type& val)
    {
        typename SEQ::iterator collEnd = coll.end();
        return coll.erase (std::remove (coll.begin(), collEnd, val), collEnd);
    }

    /** Accumulate a hash value

        @param accum Value to add to
        @param changed New data to hash
        @return Combined Hash Value
     */
    template<typename VAL>
    inline HashValue
    chained_hash (HashValue accum, VAL changed)
    {
        std::hash<HashValue> h1;
        HashValue seed = h1 (changed);
        accum = h1 (seed);
        return accum;
    }

    /**
       Create a hash value from a string
       @return The HashValue
     */
    inline HashValue
    hash_string (const std::string& str)
    {
        std::hash<std::string> hashit;
        return hashit (str);
    }

} /* namespace element */

/* Include the "util" folder */
#include "element/util/convert.hpp"

#endif /* ELEMENT_UTIL_HPP */
