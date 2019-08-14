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

class PortBuffer final
{
public:
    PortBuffer (bool inputPort, uint32 portType, uint32 dataType, uint32 bufferSize);
    ~PortBuffer();

    void clear();
    void reset();
    
    bool addEvent (int64 frames, uint32 size, uint32 type, const uint8* data);

	inline uint32 getCapacity() const { return capacity; }
    void* getPortData() const;
    
    inline uint32 getType()  const { return type; }

    inline bool isAtom()     const { return type == PortType::Atom; }
	inline bool isAudio()    const { return type == PortType::Audio; }
	inline bool isControl()  const { return type == PortType::Control; }
    inline bool isEvent()    const { return type == PortType::Event; }
	inline bool isSequence() const { return isAtom(); }

    void referTo (void* location) { buffer.referred = location; referenced = true; }
    void updateBufferType (LV2_URID_Map* map);

private:
    uint32_t type           = 0;
    uint32_t capacity       = 0;
    uint32_t bufferType     = 0;
    bool input              = true;

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
