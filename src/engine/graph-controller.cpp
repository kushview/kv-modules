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

#include "element/juce.hpp"
#include "element/engine/graph-controller.hpp"

namespace element {

//==============================================================================
const int GraphController::midiChannelNumber = 0x1000;

GraphController::GraphController (GraphProcessor& pg, PluginManager& pm)
    : FileBasedDocument (filenameSuffix, filenameWildcard, "Load a graph", "Save a graph"),
      pluginManager (pm), graph(pg), lastUID (0)
{
#if 0
    InternalPluginFormat internalFormat;
    addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::audioInputFilter),
               0.5f, 0.1f);

    addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::midiInputFilter),
               0.25f, 0.1f);

    addFilter (internalFormat.getDescriptionFor (InternalPluginFormat::audioOutputFilter),
               0.5f, 0.9f);
#endif
    setChangedFlag (false);
}

GraphController::~GraphController()
{
    graph.clear();
}

uint32 GraphController::getNextUID() noexcept
{
    return ++lastUID;
}

//==============================================================================
int GraphController::getNumFilters() const noexcept
{
    return graph.getNumNodes();
}

const GraphProcessor::Node::Ptr GraphController::getNode (const int index) const noexcept
{
    return graph.getNode (index);
}

const GraphProcessor::Node::Ptr GraphController::getNodeForId (const uint32 uid) const noexcept
{
    return graph.getNodeForId (uid);
}

void GraphController::addFilter (const PluginDescription* desc, double x, double y)
{
    if (desc != nullptr)
    {
        String errorMessage;
        AudioPluginInstance* instance = pluginManager.createPluginInstance (*desc, errorMessage);

        GraphProcessor::Node* node = nullptr;

        if (instance != nullptr)
            node = graph.addNode (instance);

        if (node != nullptr)
        {
            node->properties.set ("x", x);
            node->properties.set ("y", y);
            changed();
        }
        else
        {
            AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                         TRANS ("Couldn't create filter"),
                                         errorMessage);
        }
    }
}

void GraphController::removeFilter (const uint32 uid)
{
  //  PluginWindow::closeCurrentlyOpenWindowsFor (uid);

    if (graph.removeNode (uid))
        changed();
}

void GraphController::disconnectFilter (const uint32 id)
{
    if (graph.disconnectNode (id))
        changed();
}

void GraphController::removeIllegalConnections()
{
    if (graph.removeIllegalConnections())
        changed();
}

void GraphController::setNodePosition (const int nodeId, double x, double y)
{
    const GraphProcessor::Node::Ptr n (graph.getNodeForId (nodeId));

    if (n != nullptr)
    {
        n->properties.set ("x", jlimit (0.0, 1.0, x));
        n->properties.set ("y", jlimit (0.0, 1.0, y));
    }
}

void GraphController::getNodePosition (const int nodeId, double& x, double& y) const
{
    x = y = 0;

    const GraphProcessor::Node::Ptr n (graph.getNodeForId (nodeId));

    if (n != nullptr)
    {
        x = (double) n->properties ["x"];
        y = (double) n->properties ["y"];
    }
}

//==============================================================================
int GraphController::getNumConnections() const noexcept
{
    return graph.getNumConnections();
}

const GraphProcessor::Connection* GraphController::getConnection (const int index) const noexcept
{
    return graph.getConnection (index);
}

const GraphProcessor::Connection* GraphController::getConnectionBetween (uint32 sourceFilterUID, int sourceFilterChannel,
                                                                          uint32 destFilterUID, int destFilterChannel) const noexcept
{
    return graph.getConnectionBetween (sourceFilterUID, sourceFilterChannel,
                                       destFilterUID, destFilterChannel);
}

bool GraphController::canConnect (uint32 sourceFilterUID, int sourceFilterChannel,
                              uint32 destFilterUID, int destFilterChannel) const noexcept
{
    return graph.canConnect (sourceFilterUID, sourceFilterChannel,
                             destFilterUID, destFilterChannel);
}

bool GraphController::addConnection (uint32 sourceFilterUID, int sourceFilterChannel,
                                 uint32 destFilterUID, int destFilterChannel)
{
    const bool result = graph.addConnection (sourceFilterUID, sourceFilterChannel,
                                             destFilterUID, destFilterChannel);

    if (result)
        changed();

    return result;
}

void GraphController::removeConnection (const int index)
{
    graph.removeConnection (index);
    changed();
}

void GraphController::removeConnection (uint32 sourceFilterUID, int sourceFilterChannel,
                                    uint32 destFilterUID, int destFilterChannel)
{
    if (graph.removeConnection (sourceFilterUID, sourceFilterChannel,
                                destFilterUID, destFilterChannel))
        changed();
}

void GraphController::clear()
{
    graph.clear();
    changed();
}

//==============================================================================
String GraphController::getDocumentTitle()
{
    if (! getFile().exists())
        return "Untitled Graph";

    return getFile().getFileNameWithoutExtension();
}

Result GraphController::loadDocument (const File& file)
{
    XmlDocument doc (file);
    ScopedPointer<XmlElement> xml (doc.getDocumentElement());

    if (xml == nullptr || ! xml->hasTagName ("GraphDocument"))
        return Result::fail ("Not a valid filter graph file");

    restoreFromXml (*xml);
    return Result::ok();
}

Result GraphController::saveDocument (const File& file)
{
    ScopedPointer<XmlElement> xml (createXml());

    if (! xml->writeToFile (file, String::empty))
        return Result::fail ("Couldn't write to the file");

    return Result::ok();
}

File GraphController::getLastDocumentOpened()
{
    RecentlyOpenedFilesList recentFiles;
    return File::nonexistent;
#if 0
    recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                        ->getValue ("recentGraphDocumentFiles"));

    return recentFiles.getFile (0);
#endif
}

void GraphController::setLastDocumentOpened (const File& /*file*/)
{
#if 0
    RecentlyOpenedFilesList recentFiles;

    recentFiles.restoreFromString (getAppProperties().getUserSettings()
                                        ->getValue ("recentGraphDocumentFiles"));

    recentFiles.addFile (file);

    getAppProperties().getUserSettings()
        ->setValue ("recentGraphDocumentFiles", recentFiles.toString());
#endif
}

//==============================================================================
static XmlElement* createNodeXml (GraphProcessor::Node* const node) noexcept
{
    AudioPluginInstance* plugin = dynamic_cast <AudioPluginInstance*> (node->getProcessor());

    if (plugin == nullptr)
    {
        jassertfalse
        return nullptr;
    }

    XmlElement* e = new XmlElement ("FILTER");
    e->setAttribute ("uid", (int) node->nodeId);
    e->setAttribute ("x", node->properties ["x"].toString());
    e->setAttribute ("y", node->properties ["y"].toString());
    e->setAttribute ("uiLastX", node->properties ["uiLastX"].toString());
    e->setAttribute ("uiLastY", node->properties ["uiLastY"].toString());

    PluginDescription pd;
    plugin->fillInPluginDescription (pd);

    e->addChildElement (pd.createXml());

    XmlElement* state = new XmlElement ("STATE");

    MemoryBlock m;
    node->getProcessor()->getStateInformation (m);
    state->addTextElement (m.toBase64Encoding());
    e->addChildElement (state);

    return e;
}

void GraphController::createNodeFromXml (const XmlElement& xml)
{
    PluginDescription pd;

    forEachXmlChildElement (xml, e)
    {
        if (pd.loadFromXml (*e))
            break;
    }

    String errorMessage;

    AudioPluginInstance* instance = pluginManager.createPluginInstance (pd, errorMessage);

    if (instance == nullptr)
    {
        // xxx handle ins + outs
    }

    if (instance == nullptr)
        return;

    GraphProcessor::Node::Ptr node (graph.addNode (instance, xml.getIntAttribute ("uid")));

    if (const XmlElement* const state = xml.getChildByName ("STATE"))
    {
        MemoryBlock m;
        m.fromBase64Encoding (state->getAllSubText());

        node->getProcessor()->setStateInformation (m.getData(), (int) m.getSize());
    }

    node->properties.set ("x", xml.getDoubleAttribute ("x"));
    node->properties.set ("y", xml.getDoubleAttribute ("y"));
    node->properties.set ("uiLastX", xml.getIntAttribute ("uiLastX"));
    node->properties.set ("uiLastY", xml.getIntAttribute ("uiLastY"));
}

XmlElement* GraphController::createXml() const
{
    XmlElement* xml = new XmlElement ("GraphDocument");

    for (int i = 0; i < graph.getNumNodes(); ++i)
        xml->addChildElement (createNodeXml (graph.getNode (i)));

    for (int i = 0; i < graph.getNumConnections(); ++i)
    {
        const GraphProcessor::Connection* const fc = graph.getConnection(i);

        XmlElement* e = new XmlElement ("CONNECTION");

        e->setAttribute ("srcFilter", (int) fc->sourceNode);
        e->setAttribute ("srcChannel", (int) fc->sourcePort);
        e->setAttribute ("dstFilter", (int) fc->destNode);
        e->setAttribute ("dstChannel", (int) fc->destPort);

        xml->addChildElement (e);
    }

    return xml;
}

void GraphController::restoreFromXml (const XmlElement& xml)
{
    clear();

    forEachXmlChildElementWithTagName (xml, e, "FILTER")
    {
        createNodeFromXml (*e);
        changed();
    }

    forEachXmlChildElementWithTagName (xml, e, "CONNECTION")
    {
        addConnection ((uint32) e->getIntAttribute ("srcFilter"),
                       e->getIntAttribute ("srcChannel"),
                       (uint32) e->getIntAttribute ("dstFilter"),
                       e->getIntAttribute ("dstChannel"));
    }

    graph.removeIllegalConnections();
}

}
