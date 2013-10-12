/*
    MidiGrid.cpp - This file is part of Element
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

#include "element/gui/MidiGrid.h"

namespace Element {
namespace gui {


    static bool midiGridIsAddOrResizeEvent (const MouseEvent& e)
    {
        return (e.mods.isLeftButtonDown() && e.mods.isCommandDown());
    }

    static bool midiGridIsLassoEvent (const MouseEvent& e)
    {
        return (e.mods.isLeftButtonDown() && ! e.mods.isCommandDown());
    }


    void MidiGridSelectedItems::itemSelected (MidiGridItem* item)
    {

    }

    void MidiGridSelectedItems::itemDeselected (MidiGridItem* item)
    {

    }

    MidiGrid::MidiGrid()
    {
       addAndMakeVisible (lassoComponent = new LassoComponent<MidiGridItem*>());
    }

    MidiGrid::~MidiGrid()
    {
        deleteAndZero (lassoComponent);
    }

    MidiGrid::Selection& MidiGrid::getLassoSelection()  { return selectedNotes; }


    void
    MidiGrid::findLassoItemsInArea (Array<MidiGridItem*>& itemsFound,
                                    const Rectangle<int>& area)
    {
        for (int i = 0; i < notes.size (); i++)
        {
            MidiGridItem* note = notes.getUnchecked (i);
            if ((note->getX() >= area.getX() && note->getX() < area.getX() + area.getWidth())
                && (note->getY() >= area.getY() && note->getY() < area.getY() + area.getHeight()))
            {
                itemsFound.addIfNotAlreadyThere (note);
                selectedNotes.addToSelection (note);
            }
            else
            {
                selectedNotes.deselect (note);
            }
        }
    }


    void MidiGrid::paint(Graphics& g)
    {
        Timeline::paint (g);
    }

    void MidiGrid::resized()
    {
        Timeline::resized();
    }

    void MidiGrid::mouseDown (const MouseEvent& e)
    {
        if (midiGridIsLassoEvent (e))
            lassoComponent->beginLasso (e, this);
    }

    void MidiGrid::mouseDrag (const MouseEvent& e)
    {
        if (midiGridIsLassoEvent(e))
            lassoComponent->dragLasso (e);
    }

    void MidiGrid::mouseUp (const MouseEvent& e)
    {
        if (midiGridIsLassoEvent (e))
            lassoComponent->endLasso();
    }

    void MidiGrid::mouseMove (const MouseEvent& e)
    {

    }

    void MidiGrid::mouseExit (const MouseEvent& e)
    {

    }



}}
