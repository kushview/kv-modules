/*
    engine.hpp - This file is part of Element
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

#ifndef ELEMENT_ENGINE_BASE_HPP
#define ELEMENT_ENGINE_BASE_HPP

#include "element/juce.hpp"

namespace element {

    class Engine
    {
    public:

        virtual ~Engine() { }

        /** Get the graph player for this engine
            The engine implementation owns the player and is responsible for its deletion */
        virtual AudioIODeviceCallback& callback() = 0;

    };

}
#endif // ELEMENT_ENGINE_BASE_HPP
