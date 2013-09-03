/*
    session.cpp

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

#if 1

#include "element/session.hpp"

namespace element {

    class Session::Private
    {
    public:

        Private (Session& s) : sess(s) { }

    private:
        Session& sess;
    };

    Session::Session()
        : m_seq_counter(0)
    {
        priv.reset (new Private (*this));
    }

    Session::~Session() { }


    // below this point is testing code that probably isn't relavent

#if 0
    /* Session state (xml) */
    void
    Session::load_element (const xml_node& e)
    {
        xml_attribute version = e.attribute ("version");
        xml_attribute path    = e.attribute ("href");
        m_props.path          = path.as_string (fs::sessions_dir().c_str());
        xml_attribute name    = e.attribute ("name");
        m_props.name          = name.as_string ("Untitled-0");
        xml_attribute desc    = e.attribute ("description");
        m_props.description   = desc.as_string ("Session description...");

        // load assets
        // load default sequence
    }

    void
    Session::save_element (xml_node& e)
    {
        const Session::Properties& props = m_props;

        e.append_attribute ("version").set_value ("0.0.1_alpha0");

        /* Save session properties */
        e.append_attribute("name").set_value (props.name.c_str());
        URI session_path = URI::from_local_path (props.path);
        e.append_attribute("href").set_value (session_path.c_str());
        e.append_attribute("description").set_value (props.description.c_str());

        /* Save the asset list */
        xml_node assets = e.append_child ("assets");

        /* TODO: save xml from AssetManager */

        /* Save the session's default sequence (until multi/nested sequence support) */
        xml_node sequence = e.append_child ("sequence");
        // TODO  p_active_sequence->save_element (sequence);

    }
#endif


#if 0
    void
    Session::test_process (uint32_t nframes)
    {

        //
        uint32_t frame = priv->cursor->frame();
        uint32_t end = frame + nframes;

        for (int i = 0; i < priv->cursor->numTracks(); ++i)
        {
            Clip* clip = priv->cursor->clip (i);
            if (clip) {
               clip->process (frame, end);
            }
        }

        // yep, a corny 4 second loop
        if (end >= p_sequence->sampleRate() * 4) {
            end = 0;
        }

        priv->cursor->seek (end);
        priv->cursor->process (nframes);

    }
#endif
}

#endif
