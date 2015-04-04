/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

#ifndef ELEMENT_GUI_GRAPH_EDITOR_H
#define ELEMENT_GUI_GRAPH_EDITOR_H

class GraphController;
class FilterComponent;
class ConnectorComponent;
class PinComponent;

/** A panel that displays and edits a ProcessorGraph. */

class GraphEditorBase   : public Component,
                          public ChangeListener,
                          public DragAndDropTarget
{
public:

    GraphEditorBase (GraphController&);
    virtual ~GraphEditorBase();

    void paint (Graphics& g);

    virtual void mouseDown (const MouseEvent& e);

    void createNewPlugin (const PluginDescription* desc, int x, int y);

    FilterComponent* getComponentForFilter (const uint32 filterID) const;
    ConnectorComponent* getComponentForConnection (const Arc& conn) const;
    PinComponent* findPinAt (const int x, const int y) const;

    void resized();
    void changeListenerCallback (ChangeBroadcaster*);
    void onGraphChanged();
    void updateComponents();

    void beginConnectorDrag (const uint32 sourceFilterID, const int sourceFilterChannel,
                             const uint32 destFilterID, const int destFilterChannel,
                             const MouseEvent& e);
    void dragConnector (const MouseEvent& e);
    void endDraggingConnector (const MouseEvent& e);

    virtual bool isInterestedInDragSource (const SourceDetails& dragSourceDetails) { return true; }
    //virtual void itemDragEnter (const SourceDetails& dragSourceDetails);
    //virtual void itemDragMove (const SourceDetails& dragSourceDetails);
    //virtual void itemDragExit (const SourceDetails& dragSourceDetails);
    void itemDropped (const SourceDetails& details)
    {
        Logger::writeToLog (details.description);
    }

    virtual bool shouldDrawDragImageWhenOver() { return true; }

protected:
    GraphController& graph;

private:
    ScopedPointer<ConnectorComponent> draggingConnector;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphEditorBase)
};

#endif   // ELEMENT_GUI_GRAPH_EDITOR_H
