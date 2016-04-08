/*
    This file is part of the element modules for the JUCE Library
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

#ifndef EL_POINTER_H
#define EL_POINTER_H


#define ELEMENT_FORCE_BOOST_SHARED_PTR 0

#if ! ELEMENT_FORCE_BOOST_SHARED_PTR
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

#define Scoped ScopedPointer
#define OptionalPtr OptionalScopedPointer
#define ScopedXml ScopedPointer<XmlElement>



/* Shared Pointer wrapper helpers.
   This is done so other code doesn't have to worry about which API we're
   using */

template <class T, class U>
inline Shared<T>
dynamicPtrCast (const Shared<U>& sp)
{
#if (__cplusplus >= 201103L || __GXX_EXPERIMENTAL_CXX0X__) && ! ELEMENT_FORCE_BOOST_SHARED_PTR
    return std::dynamic_pointer_cast<T> (sp);
#elif ELEMENT_USE_TR1
    return std::tr1::dynamic_pointer_cast<T> (sp);
#else
    return boost::dynamic_pointer_cast<T> (sp);
#endif
}


template <class T, class U>
inline Shared<T>
constPtrCast (const Shared<U>& sp)
{
#if (__cplusplus >= 201103L || __GXX_EXPERIMENTAL_CXX0X__) && ! ELEMENT_FORCE_BOOST_SHARED_PTR
    return std::const_pointer_cast<T> (sp);
#elif ELEMENT_USE_TR1
    return std::tr1::const_pointer_cast<T> (sp);
#else
    return boost::const_pointer_cast<T> (sp);
#endif
}

template <class T, class U>
inline Shared<T>
staticPtrCast (const Shared<U>& sp)
{
#if (__cplusplus >= 201103L || __GXX_EXPERIMENTAL_CXX0X__) && ! ELEMENT_FORCE_BOOST_SHARED_PTR
    return std::static_pointer_cast<T> (sp);
#elif ELEMENT_USE_TR1
    return std::tr1::static_pointer_cast<T> (sp);
#else
    return boost::static_pointer_cast<T> (sp);
#endif
}

#if (__cplusplus >= 201103L || __GXX_EXPERIMENTAL_CXX0X__) && ! ELEMENT_FORCE_BOOST_SHARED_PTR
template <class T>
inline Shared<T> makeShared()
{
    return std::make_shared<T>();
}

template <class T, class A0>
inline Shared<T> makeShared(A0& a0)
{
    return std::make_shared<T> (a0);
}

template <class T, class A0, class A1>
inline Shared<T> makeShared (A0& a0, A1& a1)
{
    return std::make_shared<T> (a0, a1);
}

template <class T, class A0, class A1, class A2>
inline Shared<T> makeShared (A0& a0, A1& a1, A2& a2)
{
    return std::make_shared<T> (a0, a1, a2);
}
#else
template <class T>
inline Shared<T> makeShared()
{
    return Shared<T> (new T());
}

template <class T, class A0>
inline Shared<T> makeShared(A0& a0)
{
    return Shared<T> (new T(a0));
}

template <class T, class A0, class A1>
inline Shared<T> makeShared (A0& a0, A1& a1)
{
    return Shared<T> (new T(a0, a1));
}

template <class T, class A0, class A1, class A2>
inline Shared<T> makeShared (A0& a0, A1& a1, A2& a2)
{
    return Shared<T> (new T(a0, a1, a2));
}

template <class T, class A0, class A1, class A2, class A3>
inline Shared<T> makeShared (A0& a0, A1& a1, A2& a2, A3& a3)
{
    return Shared<T> (new T(a0, a1, a2, a3));
}

#endif

#endif /* EL_POINTER_H */
