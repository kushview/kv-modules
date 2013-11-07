/*
  parameter.hpp - A generic parameter

  Copyright (C) 2013  Michael R Fisher <mfisher31@gmail.com>

  This is free software: you can redistribute it and/or modify it under the
  terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or any later version.

  It is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this software.  If not, see <http://www.gnu.org/licenses/>.

*/
/** @file parameter.hpp */

#ifndef ELEMENT_PARAMETER_HPP
#define ELEMENT_PARAMETER_HPP

#include <cmath>
#include "element/juce.hpp"

namespace Element {

    class Parameter
    {
    public:



        inline Parameter() : name("Parameter"), symbol("parameter") { }
        inline Parameter (const String& name_, const String& symbol_)
            : name (name_), symbol (symbol_) { }

        inline ~Parameter() { }

        inline static double
        mapLog (double value, double min, double max, double k)
        {
            double y = (value - min) / (max - min);
            return log (1 + y * (exp (k) - 1)) / k;
        }


        inline static double
        mapExp (double value, double min, double max, double k)
        {
            double x = value;
            return min + (max - min) * ((exp (k * x) - 1) / (exp (k) - 1));
        }


        const String& getName()   const { return name; }
        const String& getSymbol() const { return symbol; }

        inline double min() const { return seed.min; }
        inline double max() const { return seed.max; }

        inline void
        set (double value)
        {
            if (value != seed.value && value >= seed.min && value <= seed.max)
            {
                seed.value = value;
            }
        }

        inline void
        set (double min, double max, double value)
        {
            seed.min = min;
            seed.max = max;
            set (value);
        }

        inline double
        value() const { return seed.value; }

        inline double
        normal() const
        {
            return (seed.value - seed.min) / (seed.max - seed.min);
        }

        inline void
        setNormal (float val)
        {
            if (val < 0) val = 0;
            if (val > 1.0) val = 1.0;

            set (val * (seed.max - seed.min) + seed.min);
        }

        inline double
        scaleOf (double min, double max)
        {
            return normal() * (max - min) + min;
        }

        inline double
        logarithmic() const
        {
            // double value = lower * pow (upper / lower, step / (steps - 1))
            return seed.min * pow (seed.max / seed.min, normal());
        }


        inline void
        reset()
        {
            seed.min      = 0;
            seed.max      = 1;
            seed.value    = 1;
        }

    protected:

        inline void setName (const String& n) { name = n; }
        inline void setSymbol (const String& s) { symbol = s; }

    private:

        String name, symbol;

        struct Seed
        {
            String name, symbol;
            double min, max, value;
        };

        Seed seed;

    };

}


#endif /* ELEMENT_PARAMETER_HPP */
