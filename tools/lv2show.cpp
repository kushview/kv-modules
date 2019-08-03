
#include "kv/kv.h"

using namespace juce;

class LV2Show : public JUCEApplication,
                public AsyncUpdater
{
    juce::AudioPluginFormatManager plugins;
    std::unique_ptr<DocumentWindow> window;
    std::unique_ptr<AudioPluginInstance> plugin;

    class PluginWindow : public DocumentWindow
    {
    public:
        PluginWindow()
            : DocumentWindow ("plugin", Colours::black, DocumentWindow::allButtons, true)
        { }

        void closeButtonPressed() override {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

public:
    LV2Show() { }

    const String getApplicationName() override       { return "LV2 Show"; }
    const String getApplicationVersion() override    { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override       { return false; }

    void initialise (const String& commandLine) override
    {
        devices.initialiseWithDefaultDevices (2 ,2);
        devices.addAudioCallback (&player);
        devices.addMidiInputCallback (String(), &player);

        auto* lv2 = new kv::LV2PluginFormat();
        plugins.addFormat (lv2); // takes ownership

        for (const auto& uri : lv2->searchPathsForPlugins (lv2->getDefaultLocationsToSearch(), false, false))
            Logger::writeToLog (uri);

         Logger::writeToLog (juce::newLine + "Scanning...");

        KnownPluginList list;
        PluginDirectoryScanner scanner (
            list, *lv2, 
            lv2->getDefaultLocationsToSearch(), 
            true, File(), false);
    
        String name;
        while (scanner.scanNextFile (false, name)) {
            Logger::writeToLog (String ("PLUGIN: " + name));
        }

        Logger::writeToLog (juce::newLine + "Listing...");

        for (int i = 0; i < list.getNumTypes(); ++i)
        {
            const auto* const type = list.getType (i);
            Logger::writeToLog (type->name);
        }

        PluginDescription desc;
        desc.pluginFormatName = "LV2";
        // desc.fileOrIdentifier = "http://lv2plug.in/plugins/eg-amp";
        desc.fileOrIdentifier = "https://kushview.net/plugins/roboverb";
        if (auto* instance = lv2->createInstanceFromDescription (desc, 44100.0, 1024))
        {
            plugin.reset (instance);
            player.setProcessor (instance);
            DBG("name: " << instance->getName());
            DBG("params: " << instance->getParameters().size());
            AudioProcessorEditor* editor = nullptr;
            if (plugin->hasEditor())
                editor = plugin->createEditorIfNeeded();
            else
                editor = new GenericAudioProcessorEditor (instance);
            window.reset (new PluginWindow());
            window->setName (plugin->getName());
            window->setContentOwned (editor, true);
            window->centreWithSize (window->getWidth(), window->getHeight());
            window->setVisible (true);
        }
        else
        {
            quit();
        }
    }

    void shutdown() override
    {
        devices.closeAudioDevice();
        devices.removeAudioCallback (&player);
        devices.removeMidiInputCallback (String(), &player);
        player.setProcessor (nullptr);
        window.reset();
        plugin.reset();
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
        ignoreUnused (commandLine);
    }

    void handleAsyncUpdate() override
    {
       
    }

private:
    AudioDeviceManager devices;
    AudioProcessorPlayer player;
};

START_JUCE_APPLICATION (LV2Show)
