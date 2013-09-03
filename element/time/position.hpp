/*
    position.hpp - This file is part of Element
    Copyright (C) 2013  Michael Fisher <mfisher@bketech.com>

    Element is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Element is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Element; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

namespace element {

    struct Position
    {
        int64_t frame;              ///< Frame number on a "Timeline"
        float   speed;              ///< Speed 1.0 = play : 0.0 = stop
        int32_t bar;                ///< The Current Bar
        int32_t beat;               ///< The Current Beat within the bar
        int32_t tick;               ///< Time as a tick value.
        double  bar_start_tick;     ///< The tick at the begining of 'bar'
        float   beats_per_bar;      ///< Time signature numerator
        float   beat_type;          ///< Time signature denominator
        double  ticks_per_beat;     ///< Number of ticks per one beat
        double  beats_per_minute;   ///< Tempo
    };

}
