/*
    Note.h - This file is part of Element
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

#ifndef ELEMENT_NOTE_H
#define ELEMENT_NOTE_H

#include "element/models/BlockModel.h"
#include "element/Slugs.h"
#include "element/Utils.h"

namespace Element {

    class Note;
    class Note :  public ObjectModel
    {
    public:

        struct EditDeltas
        {
            int note, channel;
            float velocity;
            double start, length;

            EditDeltas()
                : note (0), channel (0),
                  velocity (0.0), start (0.0),
                  length (0.0) { }

            ~EditDeltas() { }

            inline void reset()
            {
                EditDeltas zero;
                this->operator= (zero);
            }

            inline bool isZero() const
            {
                return false;
            }

            inline EditDeltas& operator= (const EditDeltas& other)
            {
                this->length = other.length;
                this->note = other.note;
                this->start = other.start;
                this->velocity = other.velocity;
                return *this;
            }
        };

        Note (const Note& n) : ObjectModel (n.node()) { }
        Note (const ValueTree& data) : ObjectModel (data) { }

        static inline Note
        make (const MidiMessage& midi, double beatLength)
        {
            if (! midi.isNoteOn())
                return Note (ValueTree::invalid);

            float velocity = (float)midi.getVelocity() / 127.f;
            return Note (midi.getNoteNumber(), midi.getTimeStamp(),
                         beatLength, midi.getChannel(), velocity);
        }

        static inline Note
        make (int note, double beat, double length = 1.0f, int channel = 1, float velocity = 0.8f) {
            return Note (note, beat, length, channel, velocity);
        }

        static inline Note make (const ValueTree& tree) {
            return Note (tree);
        }

        static inline Note make (const ObjectModel& object) {
            return make (object.node());
        }


        ~Note() { }

        /** Interfaces that deal with notes can set this id to be used
            in fast note lookup ops. See PatternInterface for an example.
            Usually, an interface will set this Id to something relavent
            when the Inferface-Side note is newly created.

            This funtion is thread/realtime safe.

            @param id The new ID to use
        */
        inline void setEventId (const int id) const
        {
            ValueTree temp = node();
            temp.setProperty ("eventId", id, nullptr);
        }

        inline bool isValid() const { return node().isValid(); }

        /** The event index in the interface-side implementation. */
        inline const int eventId() const { return node().getProperty ("eventId", 0); }

        /** Returns the current key id (midi note) for this model */
        inline const int keyId() const { return (int) node().getProperty (Slugs::id, 0); }

        /** Returns the channel of this note */
        inline const int channel() const { return (int) node().getProperty ("channel", 1); }

        /** Returns the velocity of this note as a ratio (0.0f to 1.0f) */
        inline const float velocity() const { return (float) node().getProperty("velocity", 0.8f); }

        /** The start position in beats */
        inline const double tickStart() const { return (double) node().getProperty ("start"); }

        /** Length of note in beats */
        inline const double beatLength() const { return (double) node().getProperty ("length"); }

        /** End of the note */
        inline const double tickEnd() const { return tickStart() + beatLength(); }

        /** Get the timings of this note as a Range */
        inline void getBeats (Range<double>& beats) const
        {
            beats.setStart (tickStart());
            beats.setLength (beatLength());
        }

        /** Resize this notes length in beats
            Note that this does not change the start time */
        inline void
        resize (EditDeltas& changes, double length)
        {
            if (length < 0.015625f) {
                // 1/64th note
                length = 0.015625f;
            }

            changes.length = length - beatLength();
        }

        /** Change the length */
        inline void changeLength (EditDeltas& d, double len) { resize (d, len); }

        /** Change channel */
        inline void
        changeChannel (EditDeltas& changes, int c)
        {
            c = Element::clampNoMoreThan (c, 1, 16);
            changes.channel = c - channel();
        }

        /** Change the key id for this note */
        inline void
        changeKeyId (EditDeltas& changes, int key)
        {
            jassert (isPositiveAndBelow (key, 128));
            changes.note = key - keyId();
        }

        /** Change this notes velocity */
        inline void
        changeVelocity (EditDeltas& changes, float vel)
        {
            changes.velocity = Element::clampNoMoreThan (vel, 0.0f, 1.0f) - velocity();
        }

        /** Move this note to a new start beat
            Length is unaffected */
        inline void move (EditDeltas& changes, const double beat)
        {
            changes.start = beat - tickStart();
        }

        /** Apply pending changes to this note */
        inline void
        applyEdits (EditDeltas& changes, bool reset = true)
        {
            if (changes.note != 0)
                node().setProperty (Slugs::id, keyId() + changes.note, nullptr);

            if (changes.channel != 0)
                node().setProperty ("channel", channel() + changes.channel, nullptr);

            if (changes.start != 0.0f)
                node().setProperty ("start", tickStart() + changes.start, nullptr);

            if (changes.length != 0.0f)
                node().setProperty ("length", beatLength() + changes.length, nullptr);

            if (reset)
                changes.reset();
        }

        inline MidiMessage
        noteOn() const
        {
            MidiMessage mm = MidiMessage::noteOn (channel(), keyId(), velocity());
            mm.setTimeStamp (tickStart());
            return mm;
        }

        inline MidiMessage
        noteOff() const
        {
            MidiMessage mm = MidiMessage::noteOff (channel(), keyId());
            mm.setTimeStamp (tickEnd());
            return mm;
        }

        inline void
        getMidi (MidiMessage& on, MidiMessage& off) const
        {
            on  = noteOn();
            off = noteOff();
        }

        inline ValueTree
        sequenceNode() const
        {
            return node().getParent();
        }

        inline bool operator== (const Note& n) const { return node() == n.node(); }
        inline bool operator!= (const Note& n) const { return node() != n.node(); }

    protected:

        friend class NoteSequence;

        Note (int note, double start, double length = 1.0f, int channel = 1, float velocity = 0.8f)
            : ObjectModel (Slugs::note)
        {
            node().setProperty (Slugs::id, note, nullptr);
            node().setProperty (Slugs::channel, channel, nullptr);
            node().setProperty (Slugs::start, start, nullptr);
            node().setProperty (Slugs::length, length, nullptr);
            node().setProperty (Slugs::velocity, velocity, nullptr);
            node().setProperty ("eventId", 0, nullptr);
        }
    };
}

#endif // ELEMENT_NOTE_H
