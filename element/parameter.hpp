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
#include "element/core.hpp"

namespace Element {

    class Parameter
    {
    public:

        struct Seed { float min, max, value; };

        Parameter ()
            : m_name("param"), m_symbol("param") { }

        Parameter (const String& name_, const String& symbol_)
            : m_name (name_), m_symbol (symbol_) { }

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


        const String& name() const { return m_name; }
        const String& symbol() const { return m_symbol; }

        inline double min() const { return m_seed.min; }
        inline double max() const { return m_seed.max; }

        inline void
        set (double value)
        {
            if (value != m_seed.value && value >= m_seed.min && value <= m_seed.max)
            {
                m_seed.value = value;
            }
        }

        inline void
        set (double min, double max, double value)
        {
            m_seed.min = min;
            m_seed.max = max;
            set (value);
        }

        inline double
        value() const { return m_seed.value; }

        inline double
        normal() const
        {
            return (m_seed.value - m_seed.min) / (m_seed.max - m_seed.min);
        }

        inline void
        setNormal (float val)
        {
            if (val < 0) val = 0;
            if (val > 1.0) val = 1.0;

            set (val * (m_seed.max - m_seed.min) + m_seed.min);
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
            return m_seed.min * pow (m_seed.max / m_seed.min, normal());
        }

        inline const Seed* seed() const
        {
            return &m_seed;
        }

        inline void
        seed (Seed& other)
        {
            other = m_seed;
        }

        inline void
        setSeed (const Seed &other)
        {
            m_seed = other;
        }

        inline void reset()
        {
            m_seed.min      = 0;
            m_seed.max      = 1;
            m_seed.value    = 1;
        }

    private:
        String m_name, m_symbol;
        Seed m_seed;

    };

}


#endif /* ELEMENT_PARAMETER_HPP */
