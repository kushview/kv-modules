/*
    NoteSequence.h - This file is part of Element
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

#ifndef ELEMENT_NOTESEQUENCE_H
#define ELEMENT_NOTESEQUENCE_H

#include "element/models/Note.h"

namespace Element {

    /** A multi-purpose container for notes. This emits an add/removed signal
        as well as a ValueTree listener interface. */
    class NoteSequence :  public ObjectModel
    {
    public:

        /** Create an empty sequence */
        NoteSequence() : ObjectModel ("note-events") { }

        /** Create an empty sequence */
        NoteSequence (const ValueTree& data)
            : ObjectModel (data)
        { }

        NoteSequence (const NoteSequence& o)
            : ObjectModel (o.node())
        { }

        ~NoteSequence() { }

        inline void
        getKeysWithEvents (BigInteger& keys)
        {
            for (int i = node().getNumChildren(); --i >= 0;)
            {
                Note n (node().getChild (i));
                keys.setBit (n.keyId(), true);
            }
        }

        /** Add a new note to the sequence */
        inline Note
        addNote (int note, double beat, double length = 1.0f, int channel = 1)
        {
            Note n (note, beat, length, channel);
            if (node().isValid())
                node().addChild (n.node(), -1, nullptr);

            return n;
        }

        /** Add a note from a value tree object */
        inline Note
        addNote (const ValueTree& tree)
        {
            if (tree.hasType (Slugs::note))
                return Note::make (ValueTree::invalid);

            if (tree.getParent().isValid())
            {
                ValueTree p = tree.getParent();
                p.removeChild (tree, nullptr);
            }

            Note nt = Note::make (tree);
            node().addChild (nt.node(), -1, nullptr);

            std::clog << node().toXmlString() << std::endl;
            return nt;
        }

        /** Remove a note from the sequence */
        inline void
        removeNote (const Note& note)
        {
            if (note.node().isAChildOf (node()))
                node().removeChild (note.node(), nullptr);
        }

        /** Add a value tree listener
            This is so things that don't know about this specific class
            can still be listeners of the sequence/notes */
        inline void addValueListener (ValueTree::Listener* client) {
            node().addListener (client);
        }

        /** Remove a value tree listener */
        inline void removeValueListener (ValueTree::Listener* client) {
            node().removeListener (client);
        }


        /** Add a note from Midi */
        inline Note
        addMidi (const MidiMessage& msg, double len = 1.0f)
        {
            Note n = Note::make (msg, len);
            node().addChild (n.node(), -1, nullptr);
            return n;
        }

        inline bool operator== (const NoteSequence& seq) const { return seq.node() == this->node(); }
        inline bool operator!= (const NoteSequence& seq) const { return seq.node() != this->node(); }

        inline NoteSequence& operator= (const NoteSequence& o) {
            setNodeData (o.node());
            return *this;
        }

    private:

        friend class Note;

    };

}

#endif /* ELEMENT_NOTESEQUENCE_H */
