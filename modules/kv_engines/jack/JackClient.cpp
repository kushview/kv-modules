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

static void kv_jack_log (const char* msg)
{
   std::cerr << "[jack] " << msg << std::endl;
}

static void kv_jack_dump_error (const jack_status_t status)
{
   if ((status & JackServerFailed) || (status & JackServerError))
      kv_jack_log("Unable to connect to JACK server");
   else if (status & JackVersionError)
      kv_jack_log("Client's protocol version does not match");
   else if (status & JackInvalidOption)
      kv_jack_log("The operation contained an invalid or unsupported option");
   else if (status & JackNameNotUnique)
      kv_jack_log("The desired client name was not unique");
   else if (status & JackNoSuchClient)
      kv_jack_log("Requested client does not exist");
   else if (status & JackInitFailure)
      kv_jack_log("Unable to initialize client");
   else
      kv_jack_log("Unknown client error");
}

#define returnValueIfNull(ptr, val) if (ptr == 0) return val

const char* Jack::audioPort = JACK_DEFAULT_AUDIO_TYPE;
const char* Jack::midiPort  = JACK_DEFAULT_MIDI_TYPE;

struct JackClient::Internal
{
public:
    Internal() { }
    ~Internal() { }

    static void shutdown(void *arg)
    {

    }

    static int process (jack_nframes_t nframes, void* arg)
    {
        JackClient* client = (JackClient*) arg;
        return 0;
    }

private:
    friend class JackClient;
};

JackClient::JackClient()
    : client(0), jack (new Internal())
{}

JackClient::~JackClient()
{
    this->close();
}

String JackClient::open (const String& name, int opts)
{
    String error;

    jack_status_t status;
    client = jack_client_open (name.toUTF8(), (jack_options_t) opts, &status);

    if (0 == client)
    {
        kv_jack_dump_error (status);
        error = "Could not open JACK client";
    }

    return error;
}

String JackClient::close()
{
    returnValueIfNull (client, "JACK server not running");

    String error;

    if (int e = deactivate())
    {
        error << "JACK deactivation error on client close: " << e;
    }

    jack_client_close (client);
    client = nullptr;

    return error;
}

bool JackClient::isOpen() const
{
    return (client != 0);
}

int JackClient::activate()
{
    returnValueIfNull (client, -1);
    return jack_activate (client);
}

int JackClient::deactivate()
{
    returnValueIfNull (client, -1);
    return jack_deactivate (client);
}

bool JackClient::isActive()
{
    return (isOpen());
}

JackPort JackClient::registerPort (const String& name, const String& type,
                                   int flags, int bufferSize)
{
    returnValueIfNull (client, JackPort (*this));

    String clientName = name;

    if (clientName.length() >= nameSize())
        clientName = clientName.substring (0, nameSize());

    jack_port_t* const cport = jack_port_register (client, clientName.toUTF8(), type.toUTF8(),
                                                   flags, bufferSize);
    return JackPort (*this, cport);
}

String JackClient::getName()
{
    returnValueIfNull (client, String());
    return jack_get_client_name (client);
}

int JackClient::nameSize()
{
    return jack_client_name_size();
}

int JackClient::sampleRate()
{
    returnValueIfNull (client, 0);
    return jack_get_sample_rate (client);
}

int JackClient::bufferSize()
{
    returnValueIfNull (client, 0);
    return jack_get_buffer_size(client);
}
