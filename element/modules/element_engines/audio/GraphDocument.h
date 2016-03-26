/*
    GraphDocument.h - This file is part of Element
    Copyright (C) 2014  Kushview, LLC.  All rights reserved.

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

#ifndef ELEMENT_GRAPH_DOCUMENT_H
#define ELEMENT_GRAPH_DOCUMENT_H

class GraphController;
class PluginManager;

const char* const graphSuffix   = ".graph";
const char* const graphWildcard = "*.graph";

class GraphDocument :  public FileBasedDocument
{
public:
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

#endif /* ELEMENT_GRAPH_DOCUMENT_H */
