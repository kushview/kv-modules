#ifndef TEMPO_H
#define TEMPO_H

#include "element/juce.hpp"

namespace Element {

    class Tempo
    {
    public:

        Tempo()  { }
        ~Tempo() { }

        inline static int32
        framesPerBeat (double sampleRate, double bpmTempo)
        {
            return roundDoubleToInt (sampleRate * 60.0f / bpmTempo);
        }

    };
}


#endif // TEMPO_H
