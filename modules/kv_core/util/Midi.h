/*
    This file is part of the element modules for the JUCE Library
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

#ifndef EL_MIDI_H
#define EL_MIDI_H

typedef uint8  MidiByte;

namespace Midi {

    inline int
    channel (const MidiByte* msg)
    {
        if ((msg[0] & 0xf0) != 0xf0)
            return (msg[0] & 0xf) + 1;
        return 0;
    }

    inline uint16
    combine (const MidiByte msb, const MidiByte lsb)
    {
        return lsb | (msb << 7);
    }

    inline bool noteIsWhiteKey (int note, bool inverted = false)
    {
        if (inverted)
            note = 127 - note;

        switch (note % 12)
        {
            case 0:
            case 2:
            case 4:
            case 5:
            case 7:
            case 9:
            case 11:
                return true;
            default:
                return false;
        }
    }

    inline String noteToText (int note, int zeroOctave = -2)
    {
        String letter = "Note";
        switch (note % 12)
        {
            case 0: letter = "C"; break;
            case 1: letter = "C#"; break;
            case 2: letter = "D"; break;
            case 3: letter = "D#"; break;
            case 4: letter = "E"; break;
            case 5: letter = "F"; break;
            case 6: letter = "F#"; break;
            case 7: letter = "G"; break;
            case 8: letter = "G#"; break;
            case 9: letter = "A"; break;
            case 10: letter = "A#"; break;
            case 11: letter = "B"; break;
        }

        letter << (int) ((note / 12) + zeroOctave);
        return letter;
    }

    inline String noteToTextInverted (int note, int zeroOctave = -2)
    {
        return noteToText (127 - note, zeroOctave);
    }

    inline void split (uint16 src, MidiByte* dst)
    {
        // XXX:  This isn't correct
        dst [0] = static_cast<MidiByte> (src / 128);
        dst [1] = static_cast<MidiByte> (src % 128);
    }

    inline bool
    isController (MidiByte* msg)
    {
        return (msg[0] & 0xf0) == 0xb0;
    }

    inline int
    pitchValue (MidiByte* msg)
    {
        return combine (msg[2], msg[1]);
    }


    /** Helper class to manage the 'state' of a MIDI bank */
    class Bank
    {
        uint8 msb, lsb;

    public:

        Bank() : msb (0), lsb (0) { }

        /** Return the coarse bank (msb) */
        inline int coarse() const { return (int) msb; }

        /** Return the fine bank (lsb) */
        inline int fine() const { return (int) lsb; }

        /** Return the bank as a 14bit combined value (lsb & msb) */
        inline int bank() const { return combine (msb, lsb); }

        /** Update the bank from a MIDI bank message. Does nothing if @c msg
            isn't MIDI cc 0 or cc 32 */
        inline void
        update (const MidiByte*)
        {
            /* needs implemented */
            jassertfalse;
        }
    };

} /* namespace Midi */


#endif  /* EL_MIDI_H */
