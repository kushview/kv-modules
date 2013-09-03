/*
    node.hpp - This file is part of Element
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

#ifndef ELEMENT_NODE_HPP
#define ELEMENT_NODE_HPP

#include <map>

#include "element/core.hpp"
#include "element/pointer.hpp"
#include "element/uri.hpp"

namespace element {

    class Connection;
    typedef String Symbol;

    class Resource {
    public:
        Resource() { }

    protected:
        NamedValueSet m_props;
    };

    class Node;
    typedef std::pair<const Node*, const Node*> NodePair;

    class Node :  public Resource
    {
    public:

        struct Connection
        {
            virtual ~Connection() { }
            virtual const String& head() const = 0;
            virtual const String& tail() const = 0;
        };

        typedef std::map<NodePair, Shared<Connection> > Connections;

        enum GraphType {
            graphType,
            blockType,
            portType
        };

        Node() { }
        virtual ~Node() { }

        virtual GraphType      type()   const = 0;
        virtual const String&    path()   const = 0;
        virtual const Symbol&  symbol() const = 0;

        virtual uint32 numPorts() const { return 0; }
        virtual Node*  port (uint32 /*index*/) { return nullptr; }

    protected:

        Connections m_connects;

    };

}


#endif // ELEMENT_NODE_HPP
