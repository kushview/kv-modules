

#include "element/docs/graph-document.hpp"
#include "element/engine/graph-controller.hpp"
#include "element/engine/graph-processor.hpp"

namespace element {

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
        AudioPluginInstance* plugin = dynamic_cast <AudioPluginInstance*> (node->getProcessor());
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
        node->getProcessor()->getStateInformation (m);
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

        AudioPluginInstance* instance = plugins.createPluginInstance (pd, errorMessage);

        if (instance == nullptr)
        {
            // xxx handle ins + outs
        }

        if (instance == nullptr)
            return;

        GraphProcessor::Node::Ptr node (
                    graph.getGraph().addNode (instance, xml.getIntAttribute ("uid")));

        std::clog << "node loaded: " << node->getProcessor()->getName() << std::endl;

        if (const XmlElement* const state = xml.getChildByName ("state"))
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

}
