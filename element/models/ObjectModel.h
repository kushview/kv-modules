/*
    ObjectModel.h - This file is part of BTV

    Copyright (C) 2013 BKE, LLC  All rights reserved.
      * Michael Fisher <mfisher@bketech.com>
*/

#ifndef BTV_OBJECT_MODEL_H
#define BTV_OBJECT_MODEL_H

#include <element/juce.hpp>
#include <element/Pointer.h>

namespace Element {

    /** A thin wrapper around a juce ValueTree */
    class ObjectModel
    {
    public:


        explicit ObjectModel (const ValueTree& data = ValueTree::invalid);
        ObjectModel (const Identifier& slugId);

        /** Get a property from the underlying ValueTree */
        inline var getProperty (const Identifier& id, const var& d = var::null) const { return objectData.getProperty (id, d); }

        /** Get a property as a juce Value from the ValueTree */
        Value getPropertyAsValue (const Identifier& property);

        /** Get the ValueTree's type */
        inline Identifier getType() const { return objectData.getType(); }

        /** Determine this objects ValueTree type */
        inline bool hasType (const Identifier& type) const { return objectData.hasType (type); }

        /** Access to the underlying ValueTree (const version) */
        const ValueTree& node() const { return objectData; }

        /** Access to the underlying ValueTree */
        ValueTree& node() { return objectData; }

        /** Replace this objects ValueTree with another
            If you need to do something special when data is set, then override
            the canAcceptData and setNodeData methods

            @param The new data to use
        */
        void setData (const ValueTree& data);

        /** Count the number of children with a type */
        int32 countChildrenOfType (const Identifier& slug) const;

    protected:

        /** Override this to handle special data validation This is called
            during setData

            @param data The new data to set
        */
        virtual bool canAcceptData (const ValueTree& data);

        /** Override this to handle special data setting. This is called during
            setData and only if canAcceptData returns true

            @param data The data being setData
        */
        virtual void setNodeData (const ValueTree& data);

    private:

        ValueTree objectData;

    };

    /** Typedef for a shared ObjectModel */
    typedef Shared<ObjectModel> ObjectPtr;

    /** Convenience wrapper for  ::dynamic_ptr_cast */
    template<class T, class U> inline Shared<T>
    dynamicPtrCast (const Shared<U>& ptr) {
        return Element::dynamic_ptr_cast<T> (ptr);
    }

    /** cast another shared pointer to a shared Object Model */
    template<class T> inline ObjectPtr
    objectPtrCast (const Shared<T>& ptr) {
        return Element::dynamic_ptr_cast<ObjectModel> (ptr);
    }

}

#endif  /* BTV_OBJECT_MODEL_H */
