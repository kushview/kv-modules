#ifndef SIGNALSLIB_HPP_INCLUDED
#define SIGNALSLIB_HPP_INCLUDED
#ifndef Q_MOC_RUN

/* this is what boost recommends to do about qt */

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

#include <element/juce.hpp>

namespace Element {
    typedef boost::sig::signal<void()> Signal;
    typedef boost::sig::signal<void(float)> FloatSignal;
    typedef boost::sig::signal<void(const String&)> StringSignal;
}

#endif
#endif
