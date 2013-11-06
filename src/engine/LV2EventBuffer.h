/*
  evbuf.hpp - LV2 Evbuf in C++

  Copyright (C) 2013  BKE, LLC.  All rights reserved.
  Michael Fisher <mfisher@bketech.com>

*/
/** @file evbuf.hpp */

#ifndef ELEMENT_LV2EVBUF_HPP
#define ELEMENT_LV2EVBUF_HPP

#include "element/juce.hpp"
#include "lv2_evbuf.h"

namespace Element {

    class LV2Evbuf
    {
    public:

        class Iterator
        {
        public:

            bool isValid()         { return lv2_evbuf_is_valid (iter); }

        protected:

            Iterator (LV2Evbuf& evbuf_, LV2_Evbuf_Iterator iter_)
                : evbuf (evbuf_), iter (lv2_evbuf_begin (evbuf_))
            { }

            Iterator next()
            {
                return Iterator (evbuf, lv2_evbuf_next (iter));
            }

        private:
            LV2Evbuf& evbuf;
            LV2_Evbuf_Iterator iter;

            friend class LV2Evbuf;
        };

        enum Type {
            lv2Event = LV2_EVBUF_EVENT,
            lv2Atom  = LV2_EVBUF_ATOM
        };

        LV2Evbuf (uint32_t capacity_, Type type,
                  uint32_t atomChunk,
                  uint32_t atomSequence)
            : capacity (capacity_), evbuf (nullptr), isInput (true)
        {
            assert (atomChunk != 0 && atomSequence != 0);
            evbuf = lv2_evbuf_new (capacity, (LV2_Evbuf_Type) type,
                                   atomChunk, atomSequence);
            iter  = lv2_evbuf_begin (evbuf);
        }

        ~LV2Evbuf()
        {
            if (evbuf != nullptr)
                lv2_evbuf_free (evbuf);
        }

        inline void setType (Type type)   { lv2_evbuf_set_type (evbuf, (LV2_Evbuf_Type)type); }

        inline void
        reset (bool input)
        {
            if (isInput != input)
                isInput = input;

            reset();
        }

        inline void
        reset()
        {
            lv2_evbuf_reset (evbuf, isInput);
            iter = lv2_evbuf_begin (evbuf);
        }

        inline uint32 getSize()           { return (uint32) lv2_evbuf_get_size (evbuf); }
        inline void*  getBuffer()         { return lv2_evbuf_get_buffer (evbuf); }
        inline uint32 getCapacity() const { return capacity; }

        inline bool
        write (uint32_t frames, uint32_t subframes,
               uint32_t type,
               uint32_t size,
               const uint8_t* data)
        {
            return lv2_evbuf_write (&iter, frames, subframes, type, size, data);
        }

        void removeUntil (uint32 frame);

        inline LV2_Evbuf* cobj()          { return evbuf; }
        inline operator LV2_Evbuf*()      { return evbuf; }

    private:
        uint32_t           capacity;
        LV2_Evbuf_Iterator iter;
        LV2_Evbuf*         evbuf;
        bool               isInput;
    };


    typedef LV2Evbuf Evbuf;

}  /* namespace element */

#endif  /* ELEMENT_LV2EVBUF_HPP */
