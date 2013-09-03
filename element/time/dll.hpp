/*
  dll.hpp - A generic delay locked loop

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
/** @file dll.hpp */

#ifndef ELEMENT_DLL_HPP
#define ELEMENT_DLL_HPP

#include <cmath>

#ifndef M_PI
#define M_PI ((float) 3.14159265358979323846)
#endif
#ifndef M_SQRT2
#define M_SQRT2 ((float) 1.41421356237309504880)
#endif

namespace element {

class DelayLockedLoop {
public:

    /** Create a new DLL */
    inline DelayLockedLoop()
        : samplerate (44100.0), periodSize (1024.0),
          omega (0), b (0), c (0),
          e2(0), t0 (0), t1 (0),
          bandwidth (1.0f), frequency (0)
    {
        reset (0.0, 1024.0, 44100.0);
        resetLPF();
    }

    /** Reset the DLL with a time, period and rate */
    inline void
    reset (double now, double period, double rate)
    {
        samplerate = rate;
        periodSize = period;

        // init the loop
        e2 = periodSize / samplerate;
        t0 = now;
        t1 = t0 + e2;
    }

    /** Update the DLL with the next timestamp */
    inline void
    update (double time)
    {
        const double e = time - t1;

        t0 = t1;
        t1 += b * e + e2;
        e2 += c * e;
    }

    /** Set the dll's parameters. Bandwidth / Frequency */
    inline void
    setParams (double b, double f)
    {
        bandwidth = b;
        frequency = f;
        resetLPF();
    }

    /**  Return the difference in filtered time (t1 - t0) */
    inline double
    timeDiff()
    {
        return (t1 - t0);
    }

private:

    double samplerate, periodSize;
    double e2, t0, t1;

    double bandwidth, frequency;
    double omega, b, c;

    /** @internal Reset the LPF
        Called when bandwidth and frequency changes */
    inline void
    resetLPF()
    {
        omega = 2.0 * M_PI * bandwidth / frequency;
        b = M_SQRT2 * omega;
        c = omega * omega;
    }
};

}

#endif  /* ELEMENT_DLL_HPP */
