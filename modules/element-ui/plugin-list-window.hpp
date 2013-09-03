#ifndef PLUGINLISTWINDOW_HPP
#define PLUGINLISTWINDOW_HPP

#include "window.hpp"

namespace element {
namespace gui {

    class PluginListWindow  : public Window
    {
    public:
        PluginListWindow (GuiApp& gui, AudioPluginFormatManager& formatManager)
            : Window (gui, "Available Plugins"),
              owner (owner_)
        {
            setUsingNativeTitleBar (true);
#if 0
            const File deadMansPedalFile (getAppProperties().getUserSettings()
                                          ->getFile().getSiblingFile ("RecentlyCrashedPluginsList"));
#endif
            setContentOwned (new PluginListComponent (formatManager,
                                                      owner.app().world().plugins().allPlugins(),
                                                      File::nonexistent,
                                                      nullptr), true);
            setResizable (true, false);
            setResizeLimits (300, 400, 800, 1500);
            setTopLeftPosition (60, 60);

            restoreWindowStateFromString (owner.app().world().settings().getUserSettings()->getValue ("listWindowPos"));
            setVisible (true);
        }

        ~PluginListWindow()
        {
            //getAppProperties().getUserSettings()->setValue ("listWindowPos", getWindowStateAsString());
            clearContentComponent();
        }


        void closeButtonPressed()
        {
            owner.app().world().plugins().saveUserPlugins (
                        owner.app().world().settings());
            closedSignal();
            delete this; // XXX !!!! don't do this !!!
        }

    private:
        MainWindow& owner;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginListWindow)
    };



    class Content  : public Component
    {
    public:

        Content (MainWindow& parent)
            : owner (parent)
        {
            setSize (800, 600);
        }

        ~Content()
        {
            std::clog << "~Content()\n";
        }

        void paint (Graphics& g) { }

        void resized()
        {

        }

    private:

        Shared<Engine> getEngine()
        {
            return owner.app().engine();
        }

        MainWindow& owner;
        //Workspace workspace;

    };

}
}
#endif // PLUGINLISTWINDOW_HPP
