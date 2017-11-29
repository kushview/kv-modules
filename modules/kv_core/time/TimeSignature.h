
#pragma once

namespace kv
{
    struct TimeSignature : public Rational
    {
        explicit TimeSignature (const int beatsPerBar = 4, const int beatUnit = 4)
            : Rational (beatsPerBar, beatUnit) { }
    };
}
