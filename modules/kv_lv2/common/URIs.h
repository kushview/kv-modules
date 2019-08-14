/*
    This file is part of the Kushview Modules for JUCE
    Copyright (c) 2014-2019  Kushview, LLC.  All rights reserved.

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

#pragma once

namespace kv {

class URIs
{
public:
    inline URIs() {}
    inline URIs (LV2_URID_Map* map)
    { 
        init (map);
    }

	URIs(const URIs& o);
	URIs& operator= (const URIs& o);

    LV2_URID atom_Float = 0;
    LV2_URID atom_Sequence;
    LV2_URID atom_Sound;
    LV2_URID event_Event;
    LV2_URID midi_MidiEvent;

    bool ready() const { return atom_Float > 0; }

    void init (LV2_URID_Map* map)
    {
        if (ready())
            return;
        atom_Float     = map->map (map->handle, LV2_ATOM__Float);
        atom_Sequence  = map->map (map->handle, LV2_ATOM__Sequence);
        atom_Sound     = map->map (map->handle, LV2_ATOM__Sound);
        event_Event    = map->map (map->handle, LV2_EVENT__Event);
        midi_MidiEvent = map->map (map->handle, LV2_MIDI__MidiEvent);
    }
};

}
