/*
    NoteClipItem.h - This file is part of Element
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

#ifndef ELEMENT_NOTE_CLIPITEM_H
#define ELEMENT_NOTE_CLIPITEM_H

#include "element/models/NoteSequence.h"
#include "element/gui/TimelineClip.h"
#include "element/util/Midi.h"

namespace Element {
namespace Gui {

    class NoteClipItem :  public TimelineClip
    {
    public:

        typedef std::pair<MidiMessage, MidiMessage> MsgPair;

        NoteClipItem (TimelineBase& owner, const Element::Note& m)
            : TimelineClip (owner),
              model (m)
        {
            assert (model.isValid());
            assert (isPositiveAndBelow (model.keyId(), 128));
            assert (model.channel() >= 1 && model.channel() <= 16);
            colour.addColour (0.0, Colours::lightsalmon);
            colour.addColour (1.0, Colours::red);
            trackRequested (127 - model.keyId());
        }

        virtual ~NoteClipItem() { }

        inline void setModel (const Element::Note& n) {
            model = n;
        }

        inline void reset() {
            Element::Note null (Element::Note::make (ValueTree::invalid));
            setModel (null);
        }

        inline void
        getTime (Range<double> &time) const
        {
            time.setStart (60.f / 120.f * model.beatStart());
            time.setLength (60.f / 120.f * model.beatLength());
        }

        inline
        void setTime (const Range<double> &time)
        {
            model.moveToBeat (deltas, time.getStart() * (120.0f / 60.f));
            model.resize (deltas, time.getLength() * (120.0f / 60.f));
            model.applyEditDeltas (deltas);
        }


        inline void paint (Graphics &g)
        {
            if (! isSelected())
                g.setColour (fillColor (model.velocity()));
            else
                g.setColour (Colours::aqua);

            g.fillAll();

            g.setColour (Colours::black);
            g.drawRect (getLocalBounds(), 1);

            if (model.isValid())
                g.drawFittedText (Midi::noteToText (model.keyId()),
                                  getLocalBounds(), Justification::centred, 1);
        }

        inline int trackRequested (const int &track)
        {
            if (isPositiveAndBelow (track, 128))
            {
                model.changeKeyId (deltas, 127 - track);
                model.applyEditDeltas (deltas);
                return track;
            }

            return 127 - model.keyId();
        }

        const int channel() const { return model.channel(); }
        const int keyId() const { return model.keyId(); }
        const float velocity() const { return model.velocity(); }

        MsgPair makeMidi() const
        {
            MidiMessage non (MidiMessage::noteOn (model.channel(),
                                                  model.keyId(),
                                                  (float) model.velocity()));
            MidiMessage noff (MidiMessage::noteOff (model.channel(),
                                                       model.keyId(),
                                                       model.velocity()));
            non.setTimeStamp (model.beatStart());
            noff.setTimeStamp (model.beatEnd());
            return std::make_pair (non, noff);
        }

        const Element::Note& note() const { return model; }

    private:

        Element::Note model;
        Element::Note::EditDeltas deltas;

        Range<double> beatTime;
        int midiChannel, midiNote;
        float noteVelocity;

        ColourGradient colour;

        inline Colour fillColor (float velocity)
        {
            return colour.getColourAtPosition (velocity);
        }
    };

}}

#endif // ELEMENT_NOTE_CLIPITEM_H
