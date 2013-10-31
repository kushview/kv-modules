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
#include "element/Utils.h"

namespace Element {

    class AssetNode :  public ObjectModel
    {
    public:

        AssetNode (const AssetItem& item)
            : ObjectModel (Slugs::asset)
        { }

    };

}

#endif /* ELEMENT_ASSET_H */
