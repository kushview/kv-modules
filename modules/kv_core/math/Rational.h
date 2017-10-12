/*
    This file is part of the Kushview Modules for JUCE
    Copyright (C) 2014-2017  Kushview, LLC.  All rights reserved.

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

#pragma once

/** Representation of a rational number */
struct Rational
{
    explicit Rational (const int n = 1, const int d = 1)
        : numerator (n), denominator (d) { }
    
    Rational (const Rational& o) { operator= (o); }
    Rational& operator= (const Rational& o)
    {
        numerator   = o.numerator;
        denominator = o.denominator;
        return *this;
    }
    
    /** The numerator */
    int numerator;
    
    /** The denominator */
    int denominator;
    
    /** Returns a floating point version (numerator / denominator) */
    double ratio() const
    { 
        jassert (denominator > 0);
        return (double)numerator / (double)denominator;
    }

    /** Returns an Inverted rational */
    Rational inverted() const           { return Rational (denominator, numerator);  }

    /** Returns an inverted ratio (denominator / numerator) */
    double invertedRatio() const        { return inverted().ratio(); }
};
