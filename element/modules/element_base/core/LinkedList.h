/*
   LinkedList.h

   Copyright (C) 2005-2012, rncbc aka Rui Nuno Capela. All rights reserved.
   Modified 2013 Michael Fisher  <mfisher31@gmail.com>
    - Adapted from qtractor for Element

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef ELEMENT_LINKEDLIST_H
#define ELEMENT_LINKEDLIST_H


/** A doubly linked list
    The main usage here is to aid in adapting other work(s) by rncbc.  New code
    should use something boost::intrusive  @see list.hpp */
template <class Node>
class LinkedList
{
public:

	// Default constructor.
    LinkedList() : p_first(0), p_last(0), m_count(0), pFreeList(0), m_scoped(false) { }

	// Default destructor.
    ~LinkedList() { clear(); }

	// Accessors.
    Node* first() const { return p_first; }
    Node* last()  const { return p_last;  }

    int count() const { return m_count; }

	// Property accessors.
    void setScoped (bool scoped) { m_scoped = scoped; }
    bool isScoped() const { return m_scoped; }

	// Insert methods.
    void insertAfter  (Node *node, Node *prev = nullptr);
    void insertBefore (Node *node, Node *next = nullptr);

	// Unlink/remove methods.
    void unlink (Node *node);
    void remove (Node *node);

	// Reset method.
	void clear();

	// Random accessors.
    Node* at (int index) const;

	// Aliased methods and operators.
    void prepend (Node *node) { insertBefore (node); }
    void append (Node *node)  { insertAfter (node); }

    Node *operator[] (int index) const { return at(index); }

    /** find the index of a node */
    int find (Node *node) const;

    /** Base list node */
	class Link
	{
	public:

		// Constructor.
        Link() : prevNode (nullptr),
                 nextNode (nullptr),
                 nextFreeNode (nullptr) { }

		// Linked node getters.
        Node *prev() const { return prevNode; }
        Node *next() const { return nextNode; }

		// Linked node setters.
        void setPrevious (Node *prev) { prevNode = prev; }
        void setNext (Node *next) { nextNode = next; }

		// Linked free node accessors.
        Node *nextFree() const { return nextFreeNode; }
        void setNextFree (Node *node) { nextFreeNode = node; }

	private:

        Node *prevNode;
        Node *nextNode;
        Node *nextFreeNode;

	};

    class iterator
	{
	public:

        inline iterator (LinkedList<Node>& list, Node* ptr) : nodes(list), nodePtr (ptr) { }
        inline iterator (const iterator& it) : nodes (it.nodes), nodePtr (it.nodePtr) { }

        inline iterator& first() { nodePtr = nodes.first(); return *this; }
        inline iterator& next()  { nodePtr = nodePtr != nullptr ? nodePtr->next() : nullptr; return *this; }
        inline iterator& prev()  { nodePtr = nodePtr == nullptr ? this->last() : nodePtr->prev(); return *this; }
        inline iterator& last()  { nodePtr = nodes.last(); return *this; }

        /* Operators */
        inline bool operator== (const iterator& other) const { return nodePtr == other.nodePtr; }
        inline bool operator!= (const iterator& other) const { return nodePtr != other.nodePtr; }

        inline iterator& operator= (const iterator& iter) { nodePtr = iter.nodePtr; return *this; }
        inline iterator& operator= (Node *pNode) { nodePtr = pNode; return *this; }

        inline iterator& operator++ () { return next(); }
        inline iterator  operator++ (int) { iterator it(*this); next(); return it; }

        inline iterator& operator-- () { return prev(); }
        inline iterator  operator-- (int) { iterator it(*this); prev(); return it; }

        inline Node* operator->() { return nodePtr; }
        inline const Node* operator->() const { return nodePtr; }

        inline Node* operator*() { return nodePtr; }
        inline const Node* operator*() const { return nodePtr; }

        /* Access */
        const LinkedList<Node>& list() const { return nodes; }
        Node *node() const { return nodePtr; }

	private:

        LinkedList<Node>& nodes;
        Node *nodePtr;

	};

    iterator begin() { return iterator (*this, first()); }
    iterator end() { return iterator (*this, nullptr); }

private:

    // Instance variables.
    Node *p_first;
    Node *p_last;
    int m_count;

	// The reclaimed freelist.
    Node *pFreeList;
    bool m_scoped;
};


// Insert methods.

template <class Node>
void
LinkedList<Node>::insertAfter (Node *node, Node *previous)
{
    if (previous == 0)
        previous = p_last;
	
    node->setPrevious (previous);
    if (previous)
    {
        node->setNext (previous->next());
        if (previous->next())
            (previous->next())->setPrevious (node);
		else
            p_last = node;
        previous->setNext (node);
    }
    else
    {
        p_first = p_last = node;
        node->setNext(0);
	}

    ++m_count;
}

template <class Node>
void LinkedList<Node>::insertBefore (Node *node, Node *next_node)
{
    if (next_node == 0)
        next_node = p_first;

    node->setNext (next_node);

    if (next_node)
    {
        node->setPrevious (next_node->prev());
        if (next_node->prev())
            (next_node->prev())->setNext(node);
		else
            p_first = node;
        next_node->setPrevious (node);
    }
    else
    {
        p_last = p_first = node;
        node->setPrevious (0);
	}

    ++m_count;
}


// Unlink method.
template <class Node>
void LinkedList<Node>::unlink (Node *node)
{
    if (node->prev())
        (node->prev())->setNext(node->next());
	else
        p_first = node->next();

    if (node->next())
        (node->next())->setPrevious (node->prev());
	else
        p_last = node->prev();

    --m_count;
}


// Remove method.
template <class Node>
void
LinkedList<Node>::remove (Node *node)
{
    unlink (node);

	// Add it to the alternate free list.
    if (m_scoped)
    {
        Node *pnextFree = pFreeList;
        node->setNextFree (pnextFree);
        pFreeList = node;
	}

}


// Reset methods.
template <class Node>
void
LinkedList<Node>::clear (void)
{
	// Remove pending items.
    Node *last = p_last;
    while (last)
    {
        remove (last);
        last = p_last;
	}

	// Free the free-list altogether...
    Node *free_list = pFreeList;
    while (free_list)
    {
        Node *nextFree = free_list->nextFree();
        delete free_list;
        free_list = nextFree;
	}

    // Force clean up.
    p_first = p_last = 0;
    m_count = 0;
    pFreeList = 0;
}


// Random accessor.
template <class Node>
Node* LinkedList<Node>::at (int index) const
{
	int i;
    Node *node;

    if (index < 0 || index >= m_count)
		return 0;

    if (index > (m_count >> 1))
    {
        for (i = m_count - 1, node = p_last; node && i > index; --i, node = node->prev())
        { ; }
    }
    else
    {
        for (i = 0, node = p_first; node && i < index; ++i, node = node->next())
        { ; }
	}

    return node;
}


// Node searcher.
template <class Node>
int LinkedList<Node>::find (Node *node) const
{
    int index = 0;
    Node *n = p_first;

    while (n)
    {
        if (node == n)
            return index;

        n = n->next();

        ++index;
	}

	return (-1);
}



#endif  /* ELEMENT_LINKEDLIST_H */
