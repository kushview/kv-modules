
#ifndef ELEMENT_JUCE_ATOMBUFFER_H
#define ELEMENT_JUCE_ATOMBUFFER_H

/** A multi-buffer of sorts. Capable of handling data for all LV2 Port
    types.  A PortBuffer can dynamically change types if needed. @see
    PortBuffer::setType */
class PortBuffer
{
public:
    const URIs* uris;
    
    PortBuffer (const URIs* ids, uint32 bufferType = 0, uint32 bufferSize = 0);
    ~PortBuffer();
    
    bool addEvent (int64 frames, uint32 size, uint32 type, const uint8* data);
    
    void clear();
    
	inline uint32 getCapacity() const { return capacity; }
    
    void* getPortData();
    
    inline uint32 getType() const { return type; }
	inline void setType (uint32 newType) { type = newType; }
    
	inline bool isAudio()    const { return type == uris->atom_Sound; }
	inline bool isControl()  const { return type == uris->atom_Float; }
    inline bool isEvent()    const { return type == uris->event_Event; }
	inline bool isSequence() const { return type == uris->atom_Sequence; }
    
    void reset (const bool forOutput = false);
    
private:
    uint32 type, capacity;
    HeapBlock<uint8> block;
    
    union {
        LV2_Atom*         atom;
        LV2_Event_Buffer* event;
    } buffer;
};

#endif
