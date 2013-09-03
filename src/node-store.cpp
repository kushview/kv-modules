
#include "element/core.hpp"
#include "element/node-store.hpp"
#include "element/node.hpp"
#include "element/pointer.hpp"

using namespace std;

namespace element {

    NodeStore::NodeStore()
    {

    }

    NodeStore::~NodeStore()
    {

    }

    void
    NodeStore::insertNode (Node* node)
    {
        // Null nodes cannot be inserted
        jassert (node);

        if (m_objs.find (node->path()) != m_objs.end())
            return;

        m_objs.insert (make_pair (node->path(), Shared<Node> (node)));

        for (uint32_t i = 0; i < node->numPorts(); ++i)
            insertNode (node->port (i));
    }

    Node*
    NodeStore::find (const String& path)
    {
        const Objects::iterator i = m_objs.find (path);
        return (i == m_objs.end()) ? nullptr : i->second.get();
    }
}
