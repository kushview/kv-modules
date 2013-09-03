/*
  jack.hpp - Class wrappers for Jack Audio
 
  Copyright (C) 2013  BKE, LLC.  All rights reserved.
  Michael Fisher <mfisher@bketech.com>
 
 */
/** @file jack.hpp */

#ifndef ELEMENT_JACK_HPP
#define ELEMENT_JACK_HPP

#if HAVE_JACK

#include <jack/jack.h>

namespace element {

    class AudioIODeviceType;
    class AudioIODeviceCallback;

    class JackCallback;
	class JackClient;
	class JackPort;

	class Jack {
	public:

		static const char* audioPort;
		static const char* midiPort;

		/** Create an AudioIODevice with a given Jack Client */
		static AudioIODeviceType* createAudioIODeviceType (JackClient* client = nullptr);
	};

	class JackClient
	{
	public:

		JackClient();
		~JackClient();

		String open (const String& name, int jackOptions);

		String close();

		bool isOpen();

		int activate (AudioIODeviceCallback* newCallback);

		int deactivate();

		bool isActive();

		//==========================================================================
		JackPort registerPort (const String& name, const String& type, int flags, int bufferSize = 0);

		String getName();

		int getNameSize();

		//==========================================================================
		int getSampleRate();
		int getBufferSize();

		//==========================================================================
		void getPorts (StringArray& dest, String nameRegex = String::empty,
										  String typeRegex = String::empty,
										  uint64 portFlags = 0)
		{
			dest.clear();

			if (const char** ports = jack_get_ports (client, nameRegex.toUTF8(),
															 nameRegex.toUTF8(),
															 portFlags))
			{
				for (int j = 0; ports[j] != nullptr; ++j)
					dest.add (ports[j]);

				jack_free (ports);
			}
		}

		operator jack_client_t* () { return client; }

	private:

		JackClient (const JackClient&);
		JackClient& operator= (const JackClient&);

		jack_client_t *client;
		AudioIODeviceCallback *callback;

		class Internal;
		ScopedPointer<Internal> jack;
		friend class Internal;
	};

	class JackPort
	{
	public:

		JackPort (JackClient& c) : client (c), port (nullptr) {  }

		JackPort (JackClient& c, jack_port_t* p) : client (c), port (p)
		{

		}

		void* getBuffer (uint32 nframes) { return jack_port_get_buffer (port, nframes); }

		~JackPort() { }

		const char* name() const { return jack_port_name (port); }

		int unregister (bool enabled) const { return jack_port_unregister (client, port); }

		bool isInput()  { return flags() & JackPortIsInput; }
		bool isOutput() { return flags() & JackPortIsOutput; }
		bool isAudio()  { return true; }
		bool isMidi()   { return false; }

		int connect (const JackPort& other) { return jack_connect (client, name(), other.name()); }

		int flags() const { return jack_port_flags (port); }

		operator jack_port_t*() { return port; }

	private:
		JackClient&  client;
		jack_port_t* port;

	};

}

#endif /* HAVE_JACK */
#endif /* ELEMENT_JACK_HPP */
