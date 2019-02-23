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

#if JUCE_DEBUG
 #define JACK_LOGGING_ENABLED 1
#endif

#if JACK_LOGGING_ENABLED
namespace {
void jack_Log (const String& s)
{
    Logger::writeToLog(s);
}

void dumpJackErrorMessage (const jack_status_t status)
{
    if (status & JackServerFailed || status & JackServerError)  jack_Log ("Unable to connect to JACK server");
    if (status & JackVersionError)      jack_Log ("Client's protocol version does not match");
    if (status & JackInvalidOption)     jack_Log ("The operation contained an invalid or unsupported option");
    if (status & JackNameNotUnique)     jack_Log ("The desired client name was not unique");
    if (status & JackNoSuchClient)      jack_Log ("Requested client does not exist");
    if (status & JackInitFailure)       jack_Log ("Unable to initialize client");
}
}
#else
#define dumpJackErrorMessage(a) {}
#define jack_Log(...) {}
#endif

#ifndef KV_JACK_NAME
 #define KV_JACK_NAME "Element"
#endif

class JackDevice   : public AudioIODevice
{
public:
    JackDevice (JackClient& client_, const String& deviceName,
                const String& inId, const String& outId)
        : AudioIODevice (deviceName, "JACK"),
          inputId (inId),
          outputId (outId),
          callback (nullptr),
          client (client_)
    { }

    ~JackDevice()
    {
        close();
    }

    StringArray getChannelNames (bool forInput) const
    {
        StringArray names;
        return names;
    }

    StringArray getOutputChannelNames()         { return getChannelNames (false); }
    StringArray getInputChannelNames()          { return getChannelNames (true); }
    int getNumSampleRates()                     { return 1; }
    double getSampleRate (int /*index*/)        { return client.sampleRate(); }
    int getNumBufferSizesAvailable()            { return 1; }
    int getBufferSizeSamples (int /*index*/)    { return getDefaultBufferSize(); }
    int getDefaultBufferSize() override         { return client.bufferSize(); }

    String open (const BigInteger& inputChannels, const BigInteger& outputChannels,
                 double /* sampleRate */, int /* bufferSizeSamples */)
    {
        jack_Log ("opening client");
        lastError = client.open (KV_JACK_NAME, 0);
        if (lastError.isNotEmpty())
        {
            jack_Log (lastError);
            return lastError;
        }

        DBG("num inputs: " << inputChannels.getHighestBit());
        DBG("num outputs: " << outputChannels.getHighestBit());


        jack_on_shutdown (client, JackDevice::shutdownCallback, this);
        jack_set_error_function (JackDevice::errorCallback);
        jack_set_port_connect_callback (client, JackDevice::portConnectCallback, this);
        jack_set_process_callback (client, JackDevice::processCallback, this);
        jack_set_thread_init_callback (client, JackDevice::threadInitCallback, this);
        jack_set_port_registration_callback (client, JackDevice::_portRegistration, this);

        client.registerPort ("audio_1", Jack::audioPort, JackPortIsOutput);
        client.registerPort ("audio_2", Jack::audioPort, JackPortIsOutput);

        return lastError;
    }

    void close()
    {
        lastError = client.close();
    }

    void start (AudioIODeviceCallback* newCallback)
    {
        if (client.isOpen() && newCallback != callback)
        {
            if (newCallback != nullptr)
                newCallback->audioDeviceAboutToStart (this);

            AudioIODeviceCallback* const oldCallback = callback;
            {
                const ScopedLock sl (callbackLock);
                callback = newCallback;

                if (callback != nullptr)
                    client.activate();
            }

            if (oldCallback != nullptr)
                oldCallback->audioDeviceStopped();
        }
    }

    void stop()
    {
        jack_Log ("jack stop \n");
        start (nullptr);
        client.deactivate();
    }

    bool isOpen()                           { return client.isOpen(); }
    bool isPlaying()                        { return callback != nullptr; }


    Array<double> getAvailableSampleRates() override
    {
        Array<double> rates;
        const double _rates[] = { 44100.0, 48000.0 };
        rates.addArray(_rates, 2);
        return rates;
    }

    Array<int> getAvailableBufferSizes() override
    {
        Array<int> sizes;
        const int _sizes[] = { 256, 512, 1024, 2048 };
        sizes.addArray(_sizes, 4);
        return sizes;
    }

    int getCurrentBufferSizeSamples()       { return getBufferSizeSamples (0); }
    double getCurrentSampleRate()           { return getSampleRate (0); }


    int getCurrentBitDepth()                { return 32; }
    String getLastError()                   { return lastError; }

    BigInteger getActiveOutputChannels() const { return 2; }
    BigInteger getActiveInputChannels()  const { return 2; }

    int getOutputLatencyInSamples()
    {
        int latency = 0;
        return latency;
    }

    int getInputLatencyInSamples()
    {
        int latency = 0;
        return latency;
    }

    void portRegistration (jack_port_id_t port, const bool wasRegistered)
    {
        const JackPort p (client, jack_port_by_id (client, port));
        if (wasRegistered)
        {
            DBG ("port registered: " << p.getName());
        }
        else
        {
            DBG ("port deregistered: " << p.getName());
        }
    }

    String inputId, outputId;

private:
    void process (const int numSamples)
    {
        const ScopedLock sl (callbackLock);

        if (callback != nullptr)
        {

        }
        else
        {

        }
    }

    void updateActivePorts()
    {

    }

    static int processCallback (jack_nframes_t nframes, void* arg)
    {
        JackDevice* jack = (JackDevice*) arg;
        jack->process (static_cast<int> (nframes));
        return 0;
    }

    static void portConnectCallback (jack_port_id_t, jack_port_id_t, int, void* arg)
    {
        jack_Log ("JackIODevice::portConnectCallback");
    }



    static void _portRegistration (jack_port_id_t port, int reg, void *arg)
    {
        ((JackDevice*)arg)->portRegistration (port, reg > 0);
    }

    static void threadInitCallback (void* /* callbackArgument */)
    {
        jack_Log ("JackIODevice::threadInitCallback");
    }

    static void shutdownCallback (void* arg)
    {
        jack_Log ("JackIODevice::shutdown");
    }

    static void errorCallback (const char* msg)
    {
        jack_Log ("JackIODevice::errorCallback " + String (msg));
    }

    static void sendDeviceChangedCallback();

    JackClient& client;

    String lastError;
    AudioIODeviceCallback* callback;
    CriticalSection callbackLock;
};

class JackDeviceType  : public AudioIODeviceType
{
public:
    JackDeviceType (JackClient* client_)
        : AudioIODeviceType ("JACK"),
          hasScanned (false),
          client (client_)
    {
        if (client == nullptr)
        {
            clientIsManaged = true;
            client = new JackClient();
        }

        inputNames.add ("default");
    }

    ~JackDeviceType()
    {
        deleteClientIfNeeded();
    }

    void deleteClientIfNeeded()
    {
        if (client != nullptr && clientIsManaged)
            client = nullptr;
        else
            client.release();
    }

    void scanForDevices() { hasScanned = true; }

    StringArray getDeviceNames (bool /* forInput */) const
    {
        jassert (hasScanned); // need to call scanForDevices() before doing this
        return inputNames;
    }

    int getDefaultDeviceIndex (bool /* forInput */) const
    {
        jassert (hasScanned); // need to call scanForDevices() before doing this
        return 0;
    }

    bool hasSeparateInputsAndOutputs() const    { return false; }

    int getIndexOfDevice (AudioIODevice* device, bool asInput) const
    {
        jassert (hasScanned); // need to call scanForDevices() before doing this
        return 0;
    }

    AudioIODevice* createDevice (const String& outputDeviceName,
                                 const String& inputDeviceName)
    {
        jassert (hasScanned); // need to call scanForDevices() before doing this
        return new JackDevice (*client, "clientname", "input", "output");
    }

    void portConnectionChange() { callDeviceChangeListeners(); }

private:
    StringArray inputNames, outputNames, inputIds, outputIds;
    bool hasScanned;
    ScopedPointer<JackClient> client;
    bool clientIsManaged;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JackDeviceType)
};

void JackDevice::sendDeviceChangedCallback() { }

AudioIODeviceType* Jack::createAudioIODeviceType (JackClient* client)
{
    return new JackDeviceType (client);
}
