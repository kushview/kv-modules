/*
    This file is part of the Kushview Modules for JUCE
    Copyright (C) 2014  Kushview, LLC.  All rights reserved.

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

#ifndef EL_PARAMETER_H
#define EL_PARAMETER_H


/** Some kind of parameter to control Audio DSP classes with. */
class Parameter
{
public:

    inline Parameter() : name ("Parameter"), symbol ("parameter") { }

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

    /** Get this parameter's name */
    const String& getName()   const { return name; }

    /** Get this parameters symbol */
    const String& getSymbol() const { return symbol; }

    /** Get the minumum value */
    inline double getMin() const { return seed.min; }

    /** Get the maximum value */
    inline double getMax() const { return seed.max; }

    /** Set the value. If value is outside of min and max, this method does
        nothing.
        @param value The new value. */
    inline void
    setValue (double value)
    {
        if (value != seed.value && value >= seed.min && value <= seed.max)
        {
            seed.value = value;
        }
    }

    /** Set the min, max, and value all in one-shot */
    inline void
    setMinMaxValue (double min, double max, double value)
    {
        seed.min = min;
        seed.max = max;

        if (seed.min == 0.0)
            seed.maxMinRatio = 1.0f;
        else
            seed.maxMinRatio = seed.max / seed.min;

        setValue (value);
    }

    /** Get the current value */
    inline const double& getValue() const { return seed.value; }

    /** Get the value normalized to the range 0.0-1.0 */
    inline double
    getNormalValue() const
    {
        if (seed.min == 0.0 && seed.max == 1.0)
            return seed.value;

        return (seed.value - seed.min) / (seed.max - seed.min);
    }

    /** Set the value from a normalized source
        @param val The normalized (0.0-1.0 ranged) value to set
        @note the input should be in the range of 0.0-1.0. this value
        will be appropriately adjusted to sit with the min/max range
        of this parameter */
    inline void
    setNormalValue (float val)
    {
        val = jlimit (0.0f, 1.0f, val);

        if (seed.min == 0.0 && seed.max == 1.0)
            setValue (val);
        else
            setValue (val * (seed.max - seed.min) + seed.min);
    }

    /** This returns a value scaled to a different min max range */
    inline double
    scaleOf (double min, double max)
    {
        return getNormalValue() * (max - min) + min;
    }

    /** Get a logarithmic value
        @note This will produce unexpected results if min/max aren't both positive
        or both are negative.   (e.g. setting min = -100 and max = 25 would be bad) */
    inline double
    getValueLogarithmic() const
    {
        return seed.min * pow (seed.maxMinRatio, getNormalValue());
    }

    /** Reset to min = 0.0, max =1.0, value == 1.0 */
    inline void
    reset()
    {
        seed.maxMinRatio = 1.0;
        seed.min         = 0;
        seed.max         = 1;
        seed.value       = 1;
    }

protected:

    /** Set the parameter's name */
    inline void setName   (const String& n) { name = n; }

    /** Set the parameter's symbol */
    inline void setSymbol (const String& s) { symbol = s; }

private:

    String name, symbol;

    struct Seed
    {
        Seed() : min (0.0), max (1.0), value (1.0),
                 maxMinRatio (1.0)
        { }

        double min, max, value;
        double maxMinRatio; // cached min / max (not used yet)

    };

    Seed seed;
};


#endif /* EL_PARAMETER_H */
