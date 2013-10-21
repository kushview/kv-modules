#ifndef ELEMENT_NODE_STORE_HPP
#define ELEMENT_NODE_STORE_HPP

#include <map>

#include "element/Core.h"
#include "element/Pointer.h"

namespace Element {

    class Node;

    class NodeStore
    {
    public:

        typedef std::map<const String, Shared<Node> > Objects;

        NodeStore();
        virtual ~NodeStore();

        void insertNode (Node* node);
        Node* find (const String& path);

        ReadWriteLock& lock() { return m_lock ; }

    private:

        Objects       m_objs;
        ReadWriteLock m_lock;

    };

    typedef Shared<NodeStore> NodeStorePtr;
}
#endif // ELEMENT_NODE_STORE_HPP
