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

#define returnValueIfNull(ptr, val) if (ptr == nullptr) return val

const char* Jack::audioPort = JACK_DEFAULT_AUDIO_TYPE;
const char* Jack::midiPort  = JACK_DEFAULT_MIDI_TYPE;

int Jack::getClientNameSize()
{
    return jack_client_name_size();
}

int Jack::getPortNameSize()
{
    return jack_port_name_size();
}

JackClient::JackClient (const String& _name,
                        int numMainInputs, const String& mainInputPrefix,
                        int numMainOutputs, const String& mainOutputPrefix)
    : client (nullptr),
      name (_name.isEmpty() ? KV_JACK_NAME : _name),
      mainInPrefix (mainInputPrefix),
      mainOutPrefix (mainOutputPrefix),
      numMainIns (numMainInputs),
      numMainOuts (numMainOutputs)
{ }

JackClient::~JackClient()
{
    this->close();
}

String JackClient::open (int opts)
{
    String error;

    jack_status_t status;
    client = jack_client_open (name.toUTF8(), (jack_options_t) opts, &status);

    if (nullptr == client)
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

JackPort::Ptr JackClient::registerPort (const String& name, const String& type,
                                        int flags, int bufferSize)
{
    returnValueIfNull (client, nullptr);

    String portName = name;
    if (portName.length() >= Jack::getPortNameSize())
        portName = portName.substring (0, Jack::getPortNameSize());

    auto* const cport = jack_port_register (
        client, portName.toUTF8(), type.toUTF8(), flags, bufferSize);
    
    return cport != nullptr ? new JackPort (*this, cport) : nullptr;
}

String JackClient::getName()
{
    returnValueIfNull (client, String());
    return jack_get_client_name (client);
}

int JackClient::getSampleRate()
{
    returnValueIfNull (client, 0);
    return jack_get_sample_rate (client);
}

int JackClient::getBufferSize()
{
    returnValueIfNull (client, 0);
    return jack_get_buffer_size(client);
}

void JackClient::getPorts (StringArray& dest, String nameRegex,
                           String typeRegex, uint64_t flags)
{
    dest.clear();

    if (const char** ports = jack_get_ports (client, nameRegex.toUTF8(),
                                             typeRegex.toUTF8(), flags))
    {
        for (int j = 0; ports[j] != 0; ++j)
            dest.add (ports [j]);
        jack_free (ports);
    }
}

//=============================================================================

JackPort::JackPort (JackClient& c, jack_port_t* p)
    : client (c), port (p) { }
JackPort::~JackPort() { port = nullptr; }

void* JackPort::getBuffer (uint32_t nframes) { return jack_port_get_buffer (port, nframes); }
const char* JackPort::getName() const { return jack_port_name (port); }

bool JackPort::isInput()  const { return getFlags() & JackPortIsInput; }
bool JackPort::isOutput() const { return getFlags() & JackPortIsOutput; }
bool JackPort::isAudio()  const { return true; }
bool JackPort::isMidi()   const { return false; }

int JackPort::connect (const JackPort& other) { return jack_connect (client, getName(), other.getName()); }

int JackPort::getFlags() const { return jack_port_flags (port); }
