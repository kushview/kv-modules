#ifndef PLUGINWINDOW_H
#define PLUGINWINDOW_H


    /** A desktop window containing a plugin's UI. */
    class PluginWindow  : public DocumentWindow
    {

        typedef GraphProcessor::Node GraphNode;
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

#endif // PLUGINWINDOW_H
