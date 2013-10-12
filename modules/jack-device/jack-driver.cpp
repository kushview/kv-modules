

#include "element/core.hpp"
#include "juce/AppConfig.h"
#include "juce/modules/juce_audio_devices/juce_audio_devices.h"

#include "jack.hpp"

namespace Element {


    //#if JUCE_DEBUG
     #define JACK_LOGGING_ENABLED 0
    //#endif

	#if JACK_LOGGING_ENABLED
	namespace
	{
		void jack_Log (const String& s)
		{
			std::cerr << s << std::endl;
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


	//==============================================================================
    #ifndef ELEMENT_JACK_NAME
     #define ELEMENT_JACK_NAME "Element"
	#endif



	//==============================================================================
    class JackDevice   : public AudioIODevice
	{
	public:
        JackDevice (JackClient& client_,
					  const String& deviceName,
					  const String& inId,
					  const String& outId)
            : AudioIODevice (deviceName, "JackDevice"),
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
        int getDefaultBufferSize()                  { return client.bufferSize(); }

		String open (const BigInteger& inputChannels, const BigInteger& outputChannels,
					 double /* sampleRate */, int /* bufferSizeSamples */)
		{
            jack_Log ("opening client");
            lastError = client.open (ELEMENT_JACK_NAME, JackNoStartServer);
            if (lastError.isNotEmpty())
            {
                jack_Log (lastError);
                return lastError;
            }

            jack_on_shutdown (client, JackDevice::shutdownCallback, this);
            jack_set_error_function (JackDevice::errorCallback);
            jack_set_port_connect_callback (client, JackDevice::portConnectCallback, this);
            jack_set_process_callback (client, JackDevice::processCallback, this);
            jack_set_thread_init_callback (client, JackDevice::threadInitCallback, this);

			return lastError;
		}

		void close()
		{
            jack_Log ("client close");
			lastError = client.close();
		}

		void start (AudioIODeviceCallback* newCallback)
		{
			if (client.isOpen() && newCallback != callback)
			{
                jack_Log ("jack start");

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

		String inputId, outputId;

	private:
		void process (const int numSamples)
		{
	#if 0
			const ScopedLock sl (callbackLock);

			if (callback != nullptr)
			{
				if ((numActiveInChans + numActiveOutChans) > 0)
					callback->audioDeviceIOCallback (const_cast <const float**> (inChans.getData()), numActiveInChans,
													 outChans, numActiveOutChans, numSamples);
			}
			else
			{
				for (int i = 0; i < numActiveOutChans; ++i)
					zeromem (outChans[i], sizeof (float) * numSamples);
			}
	#endif
		}


		void updateActivePorts()
		{

		}

        static int processCallback (jack_nframes_t nframes, void* arg)
        {
            JackDevice* jack = (JackDevice*) arg;
            jack->process ((int) nframes);
            return 0;
        }

		static void portConnectCallback (jack_port_id_t, jack_port_id_t, int, void* arg)
		{
            jack_Log ("JackIODevice::portConnectCallback");
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
            : AudioIODeviceType ("JackDevice"),
              hasScanned (false), client (client_)
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
            {
                client = nullptr;
            }
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

		void portConnectionChange()    { callDeviceChangeListeners(); }

	private:
		StringArray inputNames, outputNames, inputIds, outputIds;
		bool hasScanned;

		ScopedPointer<JackClient> client;
		bool clientIsManaged;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JackDeviceType)
	};

    void JackDevice::sendDeviceChangedCallback()
	{

	}

	//==============================================================================
	AudioIODeviceType* Jack::createAudioIODeviceType (JackClient* client)
	{
        return new JackDeviceType (client);
	}
    
}
