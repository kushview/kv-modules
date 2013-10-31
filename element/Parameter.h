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

        struct Seed { float min, max, value; };

        Parameter ()
            : mName("param"), mSymbol("param") { }

        Parameter (const String& name_, const String& symbol_)
            : mName (name_), mSymbol (symbol_) { }

        ~Parameter() { }

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


        const String& name() const { return mName; }
        const String& symbol() const { return mSymbol; }

        inline double min() const { return mSeed.min; }
        inline double max() const { return mSeed.max; }

        inline void
        set (double value)
        {
            if (value != mSeed.value && value >= mSeed.min && value <= mSeed.max)
            {
                mSeed.value = value;
            }
        }

        inline void
        set (double min, double max, double value)
        {
            mSeed.min = min;
            mSeed.max = max;
            set (value);
        }

        inline double
        value() const { return mSeed.value; }

        inline double
        normal() const
        {
            return (mSeed.value - mSeed.min) / (mSeed.max - mSeed.min);
        }

        inline void
        setNormal (float val)
        {
            if (val < 0) val = 0;
            if (val > 1.0) val = 1.0;

            set (val * (mSeed.max - mSeed.min) + mSeed.min);
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
            return mSeed.min * pow (mSeed.max / mSeed.min, normal());
        }

        inline const Seed* seed() const
        {
            return &mSeed;
        }

        inline void
        seed (Seed& other)
        {
            other = mSeed;
        }

        inline void
        setSeed (const Seed &other)
        {
            mSeed = other;
        }

        inline void reset()
        {
            mSeed.min      = 0;
            mSeed.max      = 1;
            mSeed.value    = 1;
        }

    private:

        String mName, mSymbol;
        Seed mSeed;

    };

}


#endif /* ELEMENT_PARAMETER_HPP */
