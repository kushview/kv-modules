#ifndef TEMPO_H
#define TEMPO_H

#include "element/juce.hpp"

namespace Element {

    class Tempo
    {
    public:

        Tempo()  { }
        ~Tempo() { }

        static int
        framesPerBeat (double sampleRate, double bpmTempo)
        {
            return roundDoubleToInt (sampleRate * 60.0 / bpmTempo);
        }

    };
}


#endif // TEMPO_H
