/*
    asset.cpp - This file is part of Element
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

#include "element/Asset.h"
#include "element/session/WorldBase.h"

namespace Element {


const char*
asset_file_extensions (AssetType type)
{

    switch (type)
    {
        case AssetType::sequence:
            return ".esq";
        case AssetType::audio:
            return "*.wav";
        case AssetType::midi:
            return ".mid";
        default: break;
    }

    return "";
}



AssetPtr
Asset::create (World&, const URI&)
{
    Logger::writeToLog ("Asset::create() not implemented");

#if 0
    Asset* asset = 0;

    switch (uri.asset_type())
    {
        case AssetType::audio:
        {
            break;
        }
    }
#endif

    return AssetPtr();
}

}
