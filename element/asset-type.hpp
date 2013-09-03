/*
    asset-type.hpp - This file is part of Element
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

#ifndef ELEMENT_ASSET_TYPE_HPP
#define ELEMENT_ASSET_TYPE_HPP

#include "element/core.hpp"
#include "element/types.h"

namespace element {

class AssetType
{
public:

    /** Numeric symbol for this asset type. */
    enum Root
    {
        audio    = 0, // Regular audio
        midi     = 1, // Regular MIDI
        sequence = 2, // Element Sequence Asset
        video    = 3, // Video (not supported yet)
        invalid  = 4
    };

    static const uint32_t numTypes = 4;
    inline AssetType() : root (invalid) { }

    inline AssetType (const AssetType& other)
    {
        root = other.root;
    }

    AssetType (const Root root)
        : root (root)
    { }

    AssetType (const std::string& str)
        : root (invalid)
    {
        if (str == "audio")
            root = audio;
        if (str == "midi")
            root = midi;
        if (str == "sequence")
            root = sequence;
        else if (str == "video")
            root = video;
    }

    /** Inverse of the from-string constructor */
    const char* toString() const
    {
        switch (root)
        {
            case audio:    return "audio";
            case midi:     return "midi";
            case sequence: return "sequence";
            case video:    return "video";
            default:       return "unknown"; // reeeally shouldn't ever happen
        }
    }

    const char* toURI() const
    {
        switch (root)
        {
            case audio:    return "urn:datatype:audio";
            case midi:     return "urn:datatype:midi";
            case sequence: return "urn:datatype:sequence";
            case video:    return "urn:datatype:video";
            default:       return "unknown"; // reeeally shouldn't ever happen
        }
    }

    inline operator uint32_t() const { return (uint32_t) root; }

    /** AssetType iterator */
    class iterator {
    public:

        iterator (uint32_t index) : index(index) { }

        AssetType  operator*() { return AssetType((Root)index); }
        iterator& operator++() { ++index; return *this; } // yes, prefix only
        bool operator==(const iterator& other) { return (index == other.index); }
        bool operator!=(const iterator& other) { return (index != other.index); }

    private:

        friend class AssetType;
        uint32_t index;

    };

    static iterator begin() { return iterator (0); }
    static iterator end()   { return iterator (numTypes); }

    bool operator==(const Root symbol) { return (root == symbol); }
    bool operator!=(const Root symbol) { return (root != symbol); }

    bool operator==(const AssetType other) { return (root == other.root); }
    bool operator!=(const AssetType other) { return (root != other.root); }

    AssetType& operator= (const AssetType other) { root = other.root; return *this; }

private:

    Root root; // could be const if not for the string constructor

};

}

#endif // ASSETTYPE_HPP
