/*
    Signals.h - This file is part of Element
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

#ifndef SIGNALSLIB_HPP_INCLUDED
#define SIGNALSLIB_HPP_INCLUDED
#ifndef Q_MOC_RUN

/* this is what boost recommends to do about qt. */

#if defined(signals) && defined(QOBJECTDEFS_H) && \
  !defined(QT_MOC_CPP)
#  undef signals
#  define signals signals
#endif

#include <boost/bind.hpp>
#include <boost/signals2/signal.hpp>

namespace boost
{
    namespace sig = signals2;
}

#if defined (signals) && defined(QOBJECTDEFS_H) && ! defined (QT_MOC_CPP)
#  undef signals
// Restore the macro definition of "signals", as it was
// defined by Qt's <qobjectdefs.h>.
#  define signals protected
#endif

#include "element/Juce.h"

namespace Element {
    typedef boost::sig::signal<void()> Signal;
    typedef boost::sig::signal<void(float)> FloatSignal;
    typedef boost::sig::signal<void(const String&)> StringSignal;
}

#endif
#endif
