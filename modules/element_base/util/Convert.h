/*
    Convert.h - This file is part of Element
    Copyright (C) 2013  Michael Fisher <mfisher31@gmail.com>

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

#ifndef LIB_UTIL_CONVERT_HPP
#define LIB_UTIL_CONVERT_HPP
#if 0
    // most of this is obsolete
/** Convert decibels to gain */
template<typename T>
inline T db_to_gain (T db, T inf = 90)
{
    return db > inf ? powf ((T) 10.0, db * (T) 0.05) : T();
}

/** Convert gain to decibels */
template<typename T>
inline T gain_to_db (T gain, T inf = 90)
{
    return gain > T() ? std::max (inf, (T) std::log10 (gain) * (T) 20.0)
                      : inf;
}

/** Convert a float to a std::string */
inline std::string
ftoa (float f)
{
    std::stringstream ss;
    ss << f;
    return ss.str();
}

/** Convert a string to float */
inline float
atof (const std::string &s)
{
    return boost::lexical_cast<float> (s);
}

/** Convert a c-string to float */
inline float
atof (const char* s)
{
    return std::atof (s);
}

template <typename T>
inline std::string
to_string (const T& val)
{
    std::stringstream ss;
    ss << val;
    return ss.str();
}

inline bool
path_is_uri (const std::string& path)
{
    return path.substr (0, path.find(":")) == std::string ("file");
}

/** */
inline std::string
path_to_uri (const std::string& path)
{
    if (path_is_uri (path))
        return path;

    std::string p ("file://");
    return p.append (path);
}

inline std::string
uri_to_path (const std::string& uri)
{
    std::string res = uri;
    boost::replace_all (res, "file://", "");
    return res;
}
#endif
#endif /* LIB_UTIL_CONVERT_HPP */
