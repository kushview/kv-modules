/*
   linked-list.hpp

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

#ifndef ELEMENT_LINKEDLIST_HPP
#define ELEMENT_LINKEDLIST_HPP

/** A doubly linked list
    The main usage here is to aid in adapting other work(s) by rncbc.  New code
    should use something boost::intrusive  @see list.hpp */
template <class Node>
class LinkedList
{
public:

	// Default constructor.
    LinkedList() : p_first(0), p_last(0), m_count(0), p_free_list(0), m_scoped(false) { }

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
    void insertAfter  (Node *node, Node *prev = 0);
    void insertBefore (Node *node, Node *next = 0);

	// Unlink/remove methods.
    void unlink (Node *node);
    void remove (Node *node);

	// Reset method.
	void clear();

	// Random accessors.
    Node* at (int index) const;

	// Aliased methods and operators.
    void prepend (Node *node) { insertBefore (node); }
    void append (Node *node)  { insertAfter (node);  }

    Node *operator[] (int index) const { return at(index); }

    /** find the index of a node */
    int find (Node *node) const;

    /** Base list node */
	class Link
	{
	public:

		// Constructor.
        Link() : p_prev(0), p_next(0), p_next_free(0) {}

		// Linked node getters.
        Node *prev() const { return p_prev; }
        Node *next() const { return p_next; }

		// Linked node setters.
        void set_prev (Node *prev) { p_prev = prev; }
        void set_next (Node *next) { p_next = next; }

		// Linked free node accessors.
        Node *next_free() const { return p_next_free; }
        void set_next_free (Node *pnext_free) { p_next_free = pnext_free; }

	private:

        Node *p_prev;
        Node *p_next;
        Node *p_next_free;
	};

	//----------------------------------------------------------------------
	// class List<Node>::Iterator -- List iterator (aka cursor).
	//

    class iterator
	{
	public:

        /* ctors */
        iterator(LinkedList<Node>& list)   : m_list(list), p_node(0) { }
        iterator(const iterator& it) : m_list(it.list()), p_node(it.node()) { }

        /* methods */
        iterator& first() { p_node = m_list.first(); return *this; }
        iterator& next()  { p_node = p_node->next(); return *this; }
        iterator& prev()  { p_node = p_node->prev(); return *this; }
        iterator& last()  { p_node = m_list.last(); return *this; }

        /* Operators */
        iterator& operator= (const iterator& iter) { p_node = iter.p_node; return *this; }
        iterator& operator= (Node *pNode) { p_node = pNode; return *this; }

        iterator& operator++ () { return next(); }
        iterator  operator++ (int) { iterator it(*this); next(); return it; }

        iterator& operator-- () { return prev(); }
        iterator  operator-- (int) { iterator it(*this); prev(); return it; }

        /* Access */
        const LinkedList<Node>& list() const { return m_list; }
        Node *node() const { return p_node; }

	private:

		// Instance variables.
        LinkedList<Node>& m_list;
		// Current cursory node reference.
        Node *p_node;
	};

private:

    // Instance variables.
    Node *p_first;
    Node *p_last;
    int m_count;

	// The reclaimed freelist.
    Node *p_free_list;
    bool m_scoped;
};


// Insert methods.

template <class Node>
void
LinkedList<Node>::insertAfter (Node *node, Node *previous)
{
    if (previous == 0)
        previous = p_last;
	
    node->set_prev (previous);
    if (previous)
    {
        node->set_next (previous->next());
        if (previous->next())
            (previous->next())->set_prev (node);
		else
            p_last = node;
        previous->set_next (node);
    }
    else
    {
        p_first = p_last = node;
        node->set_next(0);
	}

    ++m_count;
}

template <class Node>
void LinkedList<Node>::insertBefore (Node *node, Node *next_node)
{
    if (next_node == 0)
        next_node = p_first;

    node->set_next (next_node);

    if (next_node)
    {
        node->set_prev (next_node->prev());
        if (next_node->prev())
            (next_node->prev())->set_next(node);
		else
            p_first = node;
        next_node->set_prev (node);
    }
    else
    {
        p_last = p_first = node;
        node->set_prev (0);
	}

    ++m_count;
}


// Unlink method.
template <class Node>
void LinkedList<Node>::unlink (Node *node)
{
    if (node->prev())
        (node->prev())->set_next(node->next());
	else
        p_first = node->next();

    if (node->next())
        (node->next())->set_prev (node->prev());
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
        Node *pnext_free = p_free_list;
        node->set_next_free (pnext_free);
        p_free_list = node;
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
    Node *free_list = p_free_list;
    while (free_list)
    {
        Node *next_free = free_list->next_free();
        delete free_list;
        free_list = next_free;
	}

    // Force clean up.
    p_first = p_last = 0;
    m_count = 0;
    p_free_list = 0;
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


#endif  // ELEMENT_LINKEDLIST_HPP


// end of List.h
