/*
  pointer.hpp - Different pointer types
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
/** @file pointer.hpp */

#ifndef ELEMENT_POINTER_HPP
#define ELEMENT_POINTER_HPP

#include "element/core.hpp"

#define ELEMENT_FORCE_BOOST_SHARED_PTR 0

#if ! ELEMENT_USE_BOOST_SHARED_PTR
    #if __cplusplus >= 201103L
        #include <memory>
        template<class T> using Unique = std::unique_ptr<T>;
        template<class T> using Shared = std::shared_ptr<T>;
        template<class T> using Weak   = std::weak_ptr<T>;
    #elif __GXX_EXPERIMENTAL_CXX0X__
        #include <memory>
        #define Shared std::shared_ptr
        #define Weak std::tr1::weak_ptr
    #elif ELEMENT_USE_TR1
        #include <tr1/memory>
        #define Shared std::tr1::shared_ptr
        #define Weak std::tr1::weak_ptr
    #else
        #include <boost/shared_ptr.hpp>
        #include <boost/weak_ptr.hpp>
        #define Shared boost::shared_ptr
        #define Weak boost::weak_ptr
    #endif
#else
    #include <boost/shared_ptr.hpp>
    #include <boost/weak_ptr.hpp>
    #define Shared boost::shared_ptr
    #define Weak boost::weak_ptr

#endif

#include <boost/scoped_ptr.hpp>
#define Scoped boost::scoped_ptr
#define ScopedXml boost::scoped_ptr<juce::XmlElement>

namespace element
{

/* Shared Pointer wrapper helpers.
   This is done so other code doesn't have to worry about which API we're
   using */

template <class T, class U>
inline Shared<T>
dynamic_ptr_cast (const Shared<U>& sp)
{
#if __cplusplus >= 201103L || __GXX_EXPERIMENTAL_CXX0X__
    return std::dynamic_pointer_cast<T> (sp);
#elif ELEMENT_USE_TR1
    return std::tr1::dynamic_pointer_cast<T> (sp);
#else
    return boost::dynamic_pointer_cast<T> (sp);
#endif
}


template <class T, class U>
inline Shared<T>
const_ptr_cast (const Shared<U>& sp)
{
#if __cplusplus >= 201103L || __GXX_EXPERIMENTAL_CXX0X__
    return std::const_pointer_cast<T> (sp);
#elif ELEMENT_USE_TR1
    return std::tr1::const_pointer_cast<T> (sp);
#else
    return boost::const_pointer_cast<T> (sp);
#endif
}

template <class T, class U>
inline Shared<T>
static_ptr_cast (const Shared<U>& sp)
{
#if __cplusplus >= 201103L || __GXX_EXPERIMENTAL_CXX0X__
    return std::static_pointer_cast<T> (sp);
#elif ELEMENT_USE_TR1
    return std::tr1::static_pointer_cast<T> (sp);
#else
    return boost::static_pointer_cast<T> (sp);
#endif
}

#if __cplusplus >= 201103L || __GXX_EXPERIMENTAL_CXX0X__
template <class T>
inline Shared<T> make_sptr()
{
    return std::make_shared<T>();
}

template <class T, class A0>
inline Shared<T> make_sptr(A0& a0)
{
    return std::make_shared<T> (a0);
}

template <class T, class A0, class A1>
inline Shared<T> make_sptr (A0& a0, A1& a1)
{
    return std::make_shared<T> (a0, a1);
}

template <class T, class A0, class A1, class A2>
inline Shared<T> make_sptr (A0& a0, A1& a1, A2& a2)
{
    return std::make_shared<T> (a0, a1, a2);
}
#else
#endif

}  /* namespace element */

#endif /* ELEMENT_POINTER_HPP */
