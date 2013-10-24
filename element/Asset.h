/*
    Asset.h- This file is part of Element
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

#ifndef ELEMENT_ASSET_H
#define ELEMENT_ASSET_H

#include <string>

#include "element/models/ObjectModel.h"
#include "element/AssetTree.h"
#include "element/AssetType.h"
#include "element/Pointer.h"
#include "element/Slugs.h"
#include "element/URI.h"
#include "element/Utils.h"

namespace Element {

    class World;

    const char* asset_file_extensions (AssetType type);
    const char* asset_file_filter (AssetType type);

    class Asset;
    typedef Shared<Asset> AssetPtr;


    class AssetNode :  public ObjectModel
    {
    public:

        AssetNode (const AssetItem& item)
            : ObjectModel (Slugs::asset)
        {

        }

    };


    class AssetID
    {
    public:

        inline AssetID()
            : m_root (AssetType::Unknown),
              m_id(0),
              m_uri ("urn:nil")
        { }

        /** creates a valid/unique hashed id */
        inline AssetID (AssetType root, const String& uri)
            : m_root (root), m_id (AssetID::compute (root, uri)), m_uri (uri.toUTF8())
        { }

        inline AssetID (const AssetID& o)
        {
            m_root = o.type();
            m_id   = o.hash();
            m_uri  = o.uri();
        }

        inline AssetID&
        operator= (const AssetID& o)
        {
            m_root = o.type();
            m_id   = o.hash();
            m_uri  = o.uri();
            return *this;
        }

        inline bool operator== (const AssetID& other) const { return other.m_id == m_id; }
        inline bool operator!= (const AssetID& other) const { return other.m_id != m_id; }

        inline HashValue hash() const { return m_id; }
        inline AssetType type() const { return m_root; }
        inline String typeString() const { return m_root.toString(); }
        inline const String& uri() const { return m_uri; }
        inline bool is_valid() const { return m_id != 0 && m_root != AssetType::Unknown; }
        inline operator bool() const { return is_valid(); }

    private:

        AssetType   m_root;
        HashValue   m_id;
        String      m_uri;

        static inline HashValue
        compute (AssetType t, const String& uuid)
        {
            HashValue seed = hash_string (uuid.toStdString());
            return Element::chained_hash (seed, t);
        }

    };

    struct AssetMeta
    {
        String name;
        String author;
        String description;
    };

    /** An asset... Some kind of resource that Element understands */
    class Asset
    {
    public:

        virtual ~Asset() { }

        static AssetPtr create (World& world, const URI& uri);
        inline static AssetPtr null() { return AssetPtr(); }

        virtual void fillMetadata (AssetMeta&) const =0;

        inline const AssetID& id() const { return m_id; }
        inline AssetType type() const { return m_id.type(); }
        inline HashValue hash() const { return m_id.hash(); }
        inline String uri() const { return m_id.uri(); }

    protected:

        explicit Asset (AssetType root, const String& uri)
            : m_id (root, uri), m_uri (uri)
        { }

    private:

        const AssetID     m_id;
        const String m_uri;

    };
}

#endif /* ELEMENT_ASSET_H */
