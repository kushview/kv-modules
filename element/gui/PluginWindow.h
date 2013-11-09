#ifndef PLUGINWINDOW_HPP
#define PLUGINWINDOW_HPP

#include <element/Juce.h>
#include <element/engine/GraphProcessor.h>

namespace Element {
namespace Gui {

    //==============================================================================
    /** A desktop window containing a plugin's UI.
    */
    class PluginWindow  : public DocumentWindow
    {

        typedef Element::GraphProcessor::Node GraphNode;
        PluginWindow (Component* const uiComp, GraphNode* node, const bool generic);

    public:

        static PluginWindow* getWindowFor (GraphNode* node, bool useGenericView);
        static void closeCurrentlyOpenWindowsFor (const uint32 nodeId);
        static void closeAllCurrentlyOpenWindows();

        ~PluginWindow();

        void moved();
        void closeButtonPressed();
        void resized();

    private:
        GraphNode* owner;
        bool isGeneric;
    };

}}


#endif // PLUGINWINDOW_HPP
