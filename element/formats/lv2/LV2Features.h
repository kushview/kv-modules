/*
  lv2features.hpp - LV2 Feature management

  Copyright (C) 2013  BKE, LLC.  All rights reserved.
  Michael Fisher <mfisher@bketech.com>

*/
/** @file lv2features.hpp */

#ifndef BKE_LV2FEATURES_HPP
#define BKE_LV2FEATURES_HPP

#include "element/Juce.h"
#include <lv2/lv2plug.in/ns/lv2core/lv2.h>

namespace Element {

class LV2Feature
{
public:

    LV2Feature() { }
    virtual ~LV2Feature() { }
    virtual const LV2_Feature* getFeature() const = 0;
    virtual const String& getURI() const = 0;
};


class LV2FeatureArray
{
public:

    LV2FeatureArray() : needsBuilt (true) { }
    ~LV2FeatureArray() { }

    inline void add (LV2Feature* feature, bool rebuildArray = true)
    {
        ScopedPointer<LV2Feature> f (feature);
        if (f && ! contains (f->getURI()))
        {
            features.add (f.release());
            if (rebuildArray)
                buildArray();
        }
    }

    inline bool contains (const String& featureURI) const
    {
        for (int i = features.size(); --i >= 0; )
            if (features[i]->getURI() == featureURI)
                return true;
        return false;
    }

    /** Get a C-Style array of feautres */
    inline const LV2_Feature* const*
    getFeatures() const
    {
        jassert (needsBuilt == false);
        return array.getData();
    }

    inline int size() const { return features.size(); }
    inline const LV2_Feature* begin() const { return getFeatures() [0]; }
    inline const LV2_Feature* end()   const { return getFeatures() [features.size()]; }
    inline operator const LV2_Feature* const*() const { return getFeatures(); }

private:
    
    OwnedArray<LV2Feature>  features;
    HeapBlock<LV2_Feature*> array;
    bool needsBuilt;
    
    inline void buildArray()
    {
        needsBuilt = false;
        
        array.calloc (features.size() + 1);
        for (int i = 0; i < features.size(); ++i)
            array[i] = const_cast<LV2_Feature*> (features[i]->getFeature());
        
        array [features.size()] = nullptr;
    }

};

} /* namespace Bke */


#endif /* BKE_LV2FEATURES_HPP */
