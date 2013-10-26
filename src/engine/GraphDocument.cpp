/*
    GraphDocument.cpp - This file is part of Element
    Copyright (C) 2013  Michael Fisher <mfisher31@gmail.com>

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

#include "element/engine/GraphController.h"
#include "element/engine/GraphDocument.h"
#include "element/engine/GraphProcessor.h"

namespace Element {

    static XmlElement* createNodeXml (GraphProcessor::Node* const node) noexcept;

    GraphDocument::GraphDocument (GraphController& g, PluginManager& p)
        : FileBasedDocument (graphSuffix, graphWildcard,
                             "Load a graph", "Save a graph"),
          graph (g), plugins (p)
    {
        rootTag = "graph";
    }

    GraphDocument::~GraphDocument() { }

    XmlElement*
    GraphDocument::createXml() const
    {
        XmlElement* xml = new XmlElement (rootTag);

        for (int i = 0; i < graph.getNumFilters(); ++i)
            xml->addChildElement (createNodeXml (graph.getNode (i)));

        for (int i = 0; i < graph.getNumConnections(); ++i)
        {
            const GraphProcessor::Connection* const fc = graph.getConnection(i);
            XmlElement* e = new XmlElement ("arc");
            e->setAttribute ("source-block", (int) fc->sourceNode);
            e->setAttribute ("source-port", (int) fc->sourcePort);
            e->setAttribute ("dest-block", (int) fc->destNode);
            e->setAttribute ("dest-port", (int) fc->destPort);

            xml->addChildElement (e);
        }

        return xml;
    }

    void GraphDocument::restoreFromXml (const XmlElement& xml)
    {
        graph.clear();
        forEachXmlChildElementWithTagName (xml, e, "node")
        {
            createNodeFromXml (*e);
            changed();
        }

        forEachXmlChildElementWithTagName (xml, e, "arc")
        {
            std::clog << "connecting " <<
            graph.addConnection ((uint32)e->getIntAttribute ("source-block"),
                                 e->getIntAttribute ("source-port"),
                                 (uint32)e->getIntAttribute ("dest-block"),
                                 e->getIntAttribute ("dest-port")) << std::endl;
            std::clog << e->getIntAttribute ("source-block") << " "
                      << e->getIntAttribute ("source-port") << " "
                      << e->getIntAttribute ("dest-block") << " "
                      << e->getIntAttribute ("dest-port") << std::endl;
        }

        graph.removeIllegalConnections();
    }

    //==============================================================================
    String GraphDocument::getDocumentTitle()
    {
        if (! getFile().exists())
            return "Untitled Graph";

        return getFile().getFileNameWithoutExtension();
    }

    Result GraphDocument::loadDocument (const File& file)
    {
        ScopedPointer<XmlElement> xml (XmlDocument::parse (file));

        if (xml == nullptr || ! xml->hasTagName (rootTag))
            return Result::fail ("Not a valid file");

        restoreFromXml (*xml);
        return Result::ok();
    }

    Result GraphDocument::saveDocument (const File& file)
    {
        ScopedPointer<XmlElement> xml (createXml());

        if (! xml->writeToFile (file, String::empty))
            return Result::fail ("Couldn't write to the file");

        return Result::ok();
    }

    File GraphDocument::getLastDocumentOpened()
    {
        return File::nonexistent;
    }

    void GraphDocument::setLastDocumentOpened (const File& /*file*/)
    { }


    //==============================================================================
    static XmlElement* createNodeXml (GraphProcessor::Node* const node) noexcept
    {
        AudioPluginInstance* plugin = dynamic_cast <AudioPluginInstance*> (node->audioProcessor());
        if (plugin == nullptr)
        {
            jassertfalse
            return nullptr;
        }

        XmlElement* e = new XmlElement ("node");

        e->setAttribute ("uid", (int) node->nodeId);
        e->setAttribute ("x", node->properties ["x"].toString());
        e->setAttribute ("y", node->properties ["y"].toString());
        e->setAttribute ("uiLastX", node->properties ["uiLastX"].toString());
        e->setAttribute ("uiLastY", node->properties ["uiLastY"].toString());

        PluginDescription pd;
        plugin->fillInPluginDescription (pd);
        e->addChildElement (pd.createXml());

        std::clog << "saving: " << pd.name << std::endl;

        XmlElement* state = new XmlElement ("state");
        MemoryBlock m;
        node->audioProcessor()->getStateInformation (m);
        state->addTextElement (m.toBase64Encoding());
        e->addChildElement (state);

        return e;
    }

    void GraphDocument::createNodeFromXml (const XmlElement& xml)
    {
        std::clog << "GraphDocument::createNodeFromXml\n";
        PluginDescription pd;

        forEachXmlChildElement (xml, e)
        {
            if (pd.loadFromXml (*e))
                break;
        }

        String errorMessage;
        Processor* instance = plugins.createPlugin (pd, errorMessage);

        if (instance == nullptr)
        {
            // xxx handle ins + outs
        }

        if (instance == nullptr)
            return;

        GraphProcessor::Node::Ptr node (
                    graph.getGraph().addNode (instance, xml.getIntAttribute ("uid")));

        if (const XmlElement* const state = xml.getChildByName ("state"))
        {
            MemoryBlock m;
            m.fromBase64Encoding (state->getAllSubText());
            node->audioProcessor()->setStateInformation (m.getData(), (int) m.getSize());
        }

        node->properties.set ("x", xml.getDoubleAttribute ("x"));
        node->properties.set ("y", xml.getDoubleAttribute ("y"));
        node->properties.set ("uiLastX", xml.getIntAttribute ("uiLastX"));
        node->properties.set ("uiLastY", xml.getIntAttribute ("uiLastY"));
    }

}
