#ifndef ELEMENT_PLUGIN_WINDOW_H
#define ELEMENT_PLUGIN_WINDOW_H

/** A desktop window containing a plugin's UI. */
class PluginWindow  : public DocumentWindow
{
public:
    typedef GraphProcessor::Node GraphNode;
    static PluginWindow* getWindowFor (GraphNode* node);
    static PluginWindow* createWindowFor (GraphNode* node, Component* editor);
    
    static void closeCurrentlyOpenWindowsFor (const uint32 nodeId);
    static void closeAllCurrentlyOpenWindows();

    ~PluginWindow();

    Toolbar* getToolbar() const;
    
    // component/document window
    void moved();
    void closeButtonPressed();
    void resized();

private:
    PluginWindow (Component* const uiComp, GraphNode* node);
    GraphNode* owner;

};

#endif // ELEMENT_PLUGIN_WINDOW_H
