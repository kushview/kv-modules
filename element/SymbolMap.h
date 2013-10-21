/*
    symmap.hpp - This file is part of Element
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

#ifndef ELEMENT_SYMMAP_HPP
#define ELEMENT_SYMMAP_HPP

#include <map>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>

#include "element/Utils.h"

namespace Element {
    
    /** A function type for mapping uris */
    typedef boost::function<LV2_URID(const char*)> URIMapFunc;
    
    /** A function type for unmapping uris */
    typedef boost::function<const char*(LV2_URID)> URIUnmapFunc;
    
    /** Maintains a map of Strings/Symbols to integers

        This class also implements LV2 URID Map/Unmap features and is fully
        compatible with the current LV2 (1.4.0+) specification.
     */
    class SymbolMap
    {
    public:

        /** Create an empty symbol map and initialized LV2 URID features */
        SymbolMap() : urid_map (this), urid_unmap (this) { }
        
        ~SymbolMap() 
        { 
            clear();
        }
        
        /** Map a symbol/uri to an unsigned integer
         
            @param key The symbol to map
            @return A mapped URID, a return of 0 indicates failure
         */
        inline LV2_URID
        map (const char* key)
        {
            if (! contains (key))
            {
                const uint32_t urid ((LV2_URID) mapped.size());
                mapped [key] = urid;
                unmapped [urid] = std::string (key);
                return urid;
            }
            
            return mapped [key];
        }
        
        /** Containment test of a URI 
         
            @param uri The URI to test
            @return True if found
         */
        inline bool
        contains (const char* uri)
        {
            return Element::contains (mapped, uri);
        }
        
        /** Containment test of a URID 
            
            @param urid The URID to test
            @return True if found
         */
        inline bool
        contains (LV2_URID urid)
        {
            return Element::contains (unmapped, urid);
        }
        
        /** Unmap an already mapped id to its symbol 
         
            @param urid The URID to unmap
            @return The previously mapped symbol or 0 if the urid isn't in the cache
         */
        inline const char*
        unmap (LV2_URID urid)
        {
            if (contains (urid))
                return (const char*) unmapped [urid].c_str();

            return "";
        }
        
        /** Clear the SymbolMap */
        inline void
        clear()
        {
            mapped.clear();
            unmapped.clear();
        }
        
        /** Get a function to map symbols in this SymbolMap */
        inline URIMapFunc   map_func()   { return boost::bind (&SymbolMap::map, this, ::_1); }

        /** Get a function to unmap symbols in this SymbolMap */
        inline URIUnmapFunc unmap_func() { return boost::bind (&SymbolMap::unmap, this, ::_1); }

        /** Get the URID Map LV2 Feature.  Can be passed to an LV2 instance */
        inline LV2_Feature&  map_feature()   { return urid_map.feat; }

        /** Get the URID Unmap LV2 Feature. Can be passed to an LV2 instance */
        inline LV2_Feature&  unmap_feature() { return urid_unmap.feat; }

    private:
        
        std::map<std::string, LV2_URID> mapped;
        std::map<LV2_URID, std::string> unmapped;
        
        // LV2 URID Host Implementation follows ...

        class Map {
        public:

            inline static
            LV2_URID map (LV2_URID_Map_Handle handle, const char* uri)
            {
                SymbolMap* sym = (SymbolMap*) handle;
                return sym->map (uri);
            }

            Map (void *parent)
            {
                feat.URI    = LV2_URID__map;
                data.handle = parent;
                data.map    = &Map::map;
                feat.data   = &data;
            }

        private:
            LV2_Feature  feat;
            LV2_URID_Map data;

            friend class SymbolMap;
        } urid_map ;

        class Unmap {
        public:

            inline static const char*
            unmap (LV2_URID_Map_Handle handle, LV2_URID urid)
            {
                SymbolMap* sym = (SymbolMap*) handle;
                return sym->unmap (urid);
            }

            Unmap (void* symmap)
            {
                feat.URI    = LV2_URID__unmap;
                data.handle = symmap;
                data.unmap  = &Unmap::unmap;
                feat.data   = &data;
            }

        private:

            LV2_Feature    feat;
            LV2_URID_Unmap data;

            friend class SymbolMap;
        } urid_unmap ;
    };
}  /* namespace element */

#endif /* ELEMENT_SYMMAP_HPP */
