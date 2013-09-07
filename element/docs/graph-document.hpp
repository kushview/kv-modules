#ifndef GRAPHDOCUMENT_HPP
#define GRAPHDOCUMENT_HPP

#include "element/juce.hpp"

namespace element {

    class GraphController;
    class PluginManager;

    const char* const graphSuffix   = ".graph";
    const char* const graphWildcard = "*.graph";

    class GraphDocument :  public FileBasedDocument
    {
        GraphDocument (GraphController& ctl, PluginManager& plugins);
        virtual ~GraphDocument();

        XmlElement* createXml() const;
        void restoreFromXml (const XmlElement& xml);

        String getDocumentTitle();
        Result loadDocument (const File& file);
        Result saveDocument (const File& file);
        File getLastDocumentOpened();
        void setLastDocumentOpened (const File& file);

    private:

        void createNodeFromXml (const XmlElement& xml);

        String rootTag;

        GraphController& graph;
        PluginManager&   plugins;

    };

}

#endif // GRAPHDOCUMENT_HPP
