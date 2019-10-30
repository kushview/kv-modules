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

class JackCallback;
class JackClient;
class JackPort;

class Jack
{
public:
    static const char* audioPort;
    static const char* midiPort;
    static AudioIODeviceType* createAudioIODeviceType (JackClient* client);
};

class JackClient
{
public:
    JackClient();
    ~JackClient();

    String open (const String& name, int options);
    String close();

    bool isOpen() const;

    int activate();
    int deactivate();
    bool isActive();

    JackPort registerPort (const String& name, const String& type, int flags, int bufsize = 0);

    String getName();

    int nameSize();
    int sampleRate();
    int bufferSize();

    inline void  getPorts (StringArray& dest, String name_regex = String(),
                           String type_regex = String(), uint64_t flags = 0)
    {
        dest.clear();

        if (const char** ports = jack_get_ports (client, name_regex.toUTF8(),
                                                 type_regex.toUTF8(), flags))
        {
            for (int j = 0; ports[j] != 0; ++j)
                dest.add (ports [j]);
            jack_free (ports);
        }
    }

    operator jack_client_t* () { return client; }

private:
    JackClient (const JackClient&);
    JackClient& operator= (const JackClient&);

    jack_client_t *client;

    class Internal;
    ScopedPointer<Internal> jack;
    friend class Internal;
};

class JackPort
{
public:
    JackPort (JackClient& c) : client (c), port (0) {  }
    JackPort (JackClient& c, jack_port_t* p) : client (c), port (p) { }
    ~JackPort() { port = nullptr; }

    void* getBuffer (uint32_t nframes) { return jack_port_get_buffer (port, nframes); }
    const char* getName() const { return jack_port_name (port); }

    bool isInput()  const { return getFlags() & JackPortIsInput; }
    bool isOutput() const { return getFlags() & JackPortIsOutput; }
    bool isAudio()  const { return true; }
    bool isMidi()   const { return false; }

    int connect (const JackPort& other) { return jack_connect (client, getName(), other.getName()); }

    int getFlags() const { return jack_port_flags (port); }
    operator jack_port_t*() { return port; }

private:
    JackClient&  client;
    jack_port_t* port;
};
