/*
    fraction.hpp - This file is part of Element
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
/** @headerfile util/fraction.hpp */

#ifndef ELEMENT_FRACTION_HPP
#define ELEMENT_FRACTION_HPP


    struct FractionalGain
    {
        inline FractionalGain (int n, int d)
            : num(n), den(d), vden(d) { set (1.0f); }

        int   num;
        int   den;
        float ratio;

        inline void
        set (float r)
        {
            ratio = r;

            den = vden;
            while (r != int (r) && den < 20)
            {
                den += 2;
                r *= 4.0f;
            }

            num = int (r);
        }

    private:

        int vden;
    };



#endif  /* ELEMENT_FRACTION_HPP */
