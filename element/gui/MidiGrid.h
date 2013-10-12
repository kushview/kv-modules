/*
    MidiGrid.h - This file is part of Element
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

#ifndef ELEMENT_MIDI_GRID_H
#define ELEMENT_MIDI_GRID_H

#include "element/gui/timeline.hpp"

namespace Element {
namespace gui {

    class MidiGrid;

    class MidiGridItem : public TimelineItem
    {
    public:

        virtual ~MidiGridItem() { }

    protected:

        MidiGridItem() { }

    };


    class MidiGridSelectedItems : public SelectedItemSet<MidiGridItem*>
    {
    public:

        void itemSelected (MidiGridItem* item);
        void itemDeselected (MidiGridItem* item);

    };

    class MidiGrid : public Timeline,
                     public LassoSource<MidiGridItem*>
    {
    public:

        typedef SelectedItemSet<MidiGridItem*> Selection;

        MidiGrid();
        ~MidiGrid();

        enum ColourIds
        {
            blackKeyColourId                 = 0x99002001,
            blackKeyBrightColourId           = 0x99002002,
            whiteKeyColourId                 = 0x99002003,
            whiteKeyBrightColourId           = 0x99002004,
            backGridColourId                 = 0x99002005,
            headerColourId                   = 0x99002006,
            indicatorColourId                = 0x99002007,
            noteFillColourId                 = 0x99002008,
            noteBorderColourId               = 0x99002009
        };


        //==============================================================================
        Selection& getLassoSelection();
        void findLassoItemsInArea (Array<MidiGridItem*>& itemsFound,
                                   const Rectangle<int>& area);

        /** @internal */
        void paint(Graphics& g);
        /** @internal */
        void resized();
        /** @internal */
        void mouseDown (const MouseEvent& e);
        /** @internal */
        void mouseDrag (const MouseEvent& e);
        /** @internal */
        void mouseUp (const MouseEvent& e);
        /** @internal */
        void mouseMove (const MouseEvent& e);
        /** @internal */
        void mouseExit (const MouseEvent& e);

    protected:

        OwnedArray<MidiGridItem> notes;


        //==============================================================================
        int divDenominator;
        int snapQuantize;

        int numBars;
        int barWidth;
        float beatDelta;

        MidiGridSelectedItems selectedNotes;
        LassoComponent<MidiGridItem*>* lassoComponent;

    };

}}

#endif // ELEMENT_MIDI_GRID_H
