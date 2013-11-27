/*
  uris.hpp - Common URIs for working with LV2 Plugins/UIs

  Copyright (C) 2013  Michael R Fisher <mfisher31@gmail.com>

  This is free software: you can redistribute it and/or modify it under the
  terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or any later version.

  It is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this software.  If not, see <http://www.gnu.org/licenses/>.

*/
/** @file uris.hpp */

#ifndef ELEMENT_URIS_H
#define ELEMENT_URIS_H

#if 0
#include <string>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <lv2/lv2plug.in/ns/ext/patch/patch.h>
#include <lv2/lv2plug.in/ns/ext/time/time.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include <lv2/lv2plug.in/ns/extensions/units/units.h>

namespace Element {


/** Function type for mapping a URI */
typedef boost::function<LV2_URID(const char*)> URIMapFunc;

/** Function type for unmapping a URID */
typedef boost::function<const char*(LV2_URID)> URIUnmapFunc;


struct URIs
{
    URIs (URIMapFunc map)
        : atom_Blank          (map, LV2_ATOM__Blank),
          atom_Bool           (map, LV2_ATOM__Bool),
          atom_Chunk          (map, LV2_ATOM__Chunk),
          atom_eventTransfer  (map, LV2_ATOM__eventTransfer),
          atom_Event          (map, LV2_ATOM__Event),
          atom_Float          (map, LV2_ATOM__Float),
          atom_Resource       (map, LV2_ATOM__Resource),
          atom_Sequence       (map, LV2_ATOM__Sequence),
          atom_Sound          (map, LV2_ATOM__Sound),
          atom_URI            (map, LV2_ATOM__URI),
          atom_URID           (map, LV2_ATOM__URID),
          lv2_ControlPort     (map, LV2_CORE__ControlPort),
          midi_MidiEvent      (map, LV2_MIDI__MidiEvent),
          patch_Get           (map, LV2_PATCH__Get),
          patch_Set           (map, LV2_PATCH__Set),
          patch_Response      (map, LV2_PATCH__Response),
          patch_property      (map, LV2_PATCH__property),
          patch_request       (map, LV2_PATCH__request),
          patch_subject       (map, LV2_PATCH__subject),
          patch_value         (map, LV2_PATCH__value),
          time_bar            (map, LV2_TIME__bar),
          time_barBeat        (map, LV2_TIME__barBeat),
          time_beatUnit       (map, LV2_TIME__beatUnit),
          time_frame          (map, LV2_TIME__frame),
          time_Position       (map, LV2_TIME__Position),
          time_beatsPerBar    (map, LV2_TIME__beatsPerBar),
          time_beatsPerMinute (map, LV2_TIME__beatsPerMinute),
          time_speed          (map, LV2_TIME__speed),
          units_frame         (map, LV2_UNITS__frame),
          units_beat          (map, LV2_UNITS__beat)
    { }

    struct Entry
    {
        Entry (URIMapFunc map, const char* uri_)
            : uri(uri_), id (map (uri_)) { }

        operator LV2_URID()    const { return id; }
        operator std::string() const { return uri; }

        // these are public so they can be used for va_args etc etc
        std::string     uri;
        LV2_URID        id;
    };

    /* lv2 entries */
    const Entry atom_Blank;
    const Entry atom_Bool;
    const Entry atom_Chunk;
    const Entry atom_eventTransfer;
    const Entry atom_Event;
    const Entry atom_Float;
    const Entry atom_Resource;
    const Entry atom_Sequence;
    const Entry atom_Sound;
    const Entry atom_URI;
    const Entry atom_URID;

    const Entry lv2_ControlPort;

    const Entry midi_MidiEvent;

    const Entry patch_Get;
    const Entry patch_Set;
    const Entry patch_Response;
    const Entry patch_property;
    const Entry patch_request;
    const Entry patch_subject;
    const Entry patch_value;

    const Entry time_bar;
    const Entry time_barBeat;
    const Entry time_beatUnit;
    const Entry time_frame;
    const Entry time_Position;
    const Entry time_beatsPerBar;
    const Entry time_beatsPerMinute;
    const Entry time_speed;

    const Entry units_frame;
    const Entry units_beat;

    /* element specific */
};

} /* namespace element */
#endif
#endif /* ELEMENT_URIS_H */
