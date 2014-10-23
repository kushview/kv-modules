/*
  jack.hpp - Jack Audio helper class

  Copyright (C) 2014  Kushview, LLC.  All rights reserved.

 */
/** @file jack.hpp */

#if ELEMENT_JACK_DEVICE

#ifndef ELEMENT_JACK_H
#define ELEMENT_JACK_H

#include <vector>
#include <jack/jack.h>

#include "element/Juce.h"
#include "element/Pointer.h"

namespace juce {

     class AudioIODeviceType;
}

namespace Element {


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

        inline void
        getPorts (std::vector<String>& dest,
                   String name_regex = String(),
                   String type_regex = String(),
                   uint64_t flags = 0)
        {
            dest.clear();

            if (const char** ports = jack_get_ports (client, name_regex.toUTF8(),
                                                     type_regex.toUTF8(), flags))
            {
                for (int j = 0; ports[j] != 0; ++j)
                    dest.push_back (ports[j]);
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
        ~JackPort() { }

        void* buffer (uint32_t nframes) { return jack_port_get_buffer (port, nframes); }
        const char* name() const { return jack_port_name (port); }

        bool isInput()  const { return flags() & JackPortIsInput; }
        bool isOutput() const { return flags() & JackPortIsOutput; }
        bool isAudio()  const { return true; }
        bool isMidi()   const { return false; }

        int connect (const JackPort& other) { return jack_connect (client, name(), other.name()); }

        int flags() const { return jack_port_flags (port); }
        operator jack_port_t*() { return port; }

    private:

        JackClient&  client;
        jack_port_t* port;

    };

}
#endif /* ELEMENT_JACK_H */

#endif
