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

struct Jack
{
    static const char* audioPort;
    static const char* midiPort;
    static AudioIODeviceType* createAudioIODeviceType (JackClient* client);

    static int getClientNameSize();
    static int getPortNameSize();
};

class JackPort final : public ReferenceCountedObject
{
public:
    using Ptr = ReferenceCountedObjectPtr<JackPort>;

    ~JackPort();

    void* getBuffer (uint32_t nframes);
    const char* getName() const;
    bool isInput()  const;
    bool isOutput() const;
    bool isAudio()  const;
    bool isMidi()   const;
    int connect (const JackPort& other);
    int getFlags() const;

    operator jack_port_t*() const { return port; }

private:
    friend class JackClient;
    JackPort (JackClient& c, jack_port_t* p);
    JackClient& client;
    jack_port_t* port;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JackPort);
};

class JackClient final
{
public:
    /** Make a new JACK client */
    explicit JackClient (const String& name = String(),
                         int numMainInputs = 2,
                         const String& mainInputPrefix = "main_in_",
                         int numMainOutputs = 2,
                         const String& mainOutputPrefix = "main_out_");

    ~JackClient();

    int getNumMainInputs() const { return numMainIns; }
    int getNumMainOutputs() const { return numMainOuts; }
    const String& getMainOutputPrefix() const { return mainInPrefix; }
    const String& getMainInputPrefix()  const { return mainOutPrefix; }

    /** Open the client */
    String open (int options);

    /** Close the client */
    String close();

    /** Returns true if client is open */
    bool isOpen() const;

    /** Activate the client */
    int activate();

    /** Deactivate the client */
    int deactivate();

    /** Returns true if the client is active */
    bool isActive();

    /** Register a new port */
    JackPort::Ptr registerPort (const String& name, const String& type, 
                                int flags, int bufsize = 0);

    /** Returns the client's name */
    String getName();

    /** Returns the current sample rate */
    int getSampleRate();

    /** Returns the current buffer size */
    int getBufferSize();

    /** Query for ports */
    void getPorts (StringArray& dest, String nameRegex = String(),
                   String typeRegex = String(), uint64_t flags = 0);

    operator jack_client_t* () const { return client; }

private:
    JackClient (const JackClient&);
    JackClient& operator= (const JackClient&);
    jack_client_t *client;
    String name, mainInPrefix, mainOutPrefix;
    int numMainIns, numMainOuts;
    Array<JackPort::Ptr> ports;
};
