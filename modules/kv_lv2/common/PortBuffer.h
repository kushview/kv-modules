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

/** A multi-buffer of sorts. Capable of handling data for all LV2 Port
    types.  A PortBuffer can dynamically change types if needed. @see
    PortBuffer::setType */
class PortBuffer
{
public:
    const URIs* uris;
    PortBuffer (uint32 bufferType = 0, uint32 bufferSize = 0) {}
    PortBuffer (const URIs* ids, uint32 bufferType = 0, uint32 bufferSize = 0);
    ~PortBuffer();

    void clear();
    void reset (const bool forOutput = false);
    
    bool addEvent (int64 frames, uint32 size, uint32 type, const uint8* data);

	inline uint32 getCapacity() const { return capacity; }
    void* getPortData() const;
    
    inline uint32 getType() const { return type; }
	inline void setType (uint32 newType) { type = newType; }

	inline bool isAudio()    const { return type == atom_Sound; }
	inline bool isControl()  const { return type == atom_Float; }
    inline bool isEvent()    const { return type == event_Event; }
	inline bool isSequence() const { return type == atom_Sequence; }

    void setTypes (std::function<uint32_t (const char*)> map);
private:
    uint32_t type       = 0;
    uint32_t capacity   = 0;
    uint32_t atom_Float     = 0;
    uint32_t atom_Sequence  = 0;
    uint32_t atom_Sound     = 0;
    uint32_t event_Event    = 0;
    uint32_t midi_MidiEvent = 0;
    
    std::unique_ptr<uint8[]> data;
    bool referenced = false;

    union {
        void*             referred;
        float*            control;
        float*            audio;
        float*            cv;
        LV2_Atom*         atom;
        LV2_Event_Buffer* event;
    } buffer;
};

}
