/*
    Arc.cpp - This file is part of Element
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

#include "element/Arc.h"

namespace Element {

    Arc::Arc (uint32 sn, uint32 sp, uint32 dn, uint32 dp) noexcept
        : sourceNode (sn), sourcePort (sp), destNode (dn), destPort (dp)  { }

    ValueTree
    Arc::makeState() const
    {
        ValueTree v ("arc");
        v.setProperty ("sourceNode", (int32) sourceNode, nullptr);
        v.setProperty ("sourcePort", (int32) sourcePort, nullptr);
        v.setProperty ("destNode", (int32) destNode, nullptr);
        v.setProperty ("destPort", (int32) destPort, nullptr);
        return v;
    }
}
