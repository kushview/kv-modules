/*
  lv2features.hpp - LV2 Feature management

  Copyright (C) 2013  BKE, LLC.  All rights reserved.
  Michael Fisher <mfisher@bketech.com>

*/
/** @file lv2features.hpp */

#ifndef BKE_LV2FEATURES_HPP
#define BKE_LV2FEATURES_HPP

#include "element/juce.hpp"
#include <lv2/lv2plug.in/ns/lv2core/lv2.h>

namespace Element {

class LV2Feature
{
public:

    LV2Feature() { }
    virtual ~LV2Feature() { }

    virtual String getURI() = 0;
    virtual LV2_Feature& getFeature() = 0;

};


class LV2FeatureArray
{
public:

    LV2FeatureArray() : needsBuilt (true) { }
    ~LV2FeatureArray() { }

    inline void add (LV2Feature* feature)
    {
        ScopedPointer<LV2Feature> f (feature);
        if (f && ! contains (f->getURI()))
        {
            needsBuilt = true;
            features.add (f.release());
        }
    }

    inline bool contains (const String& featureURI)
    {
        for (int i = features.size(); --i >=0; )
            if (features[i]->getURI() == featureURI)
                return true;

        return false;
    }

    template <class DataType>
    inline DataType* getFeatureData (const String& feature)
    {
       for (int i = features.size(); --i >=0; )
          if (features[i]->getURI() == feature)
              return &features[i]->getFeature();
       return nullptr;
    }

    LV2_Feature* getFeature (const String& feature)
    {
       for (int i = features.size(); --i >=0; )
                 if (features[i]->getURI() == feature)
                     return &features[i]->getFeature();
       return nullptr;
    }

    /** Get a C-Style array of feautres */
    inline LV2_Feature** getFeatures()
    {
        if (needsBuilt)
        {
            array.calloc (features.size() + 1);

            for (int i = 0; i < features.size(); ++i)
            {
                array[i] = &features[i]->getFeature();
            }

            array[features.size()] = 0;

            needsBuilt = false;
        }

        return array.getData();
    }

    inline int size() const                          { return features.size(); }
    inline operator LV2_Feature**()                  { return getFeatures(); }

private:
    OwnedArray<LV2Feature>  features;
    HeapBlock<LV2_Feature*> array;
    bool needsBuilt;

};

} /* namespace Bke */


#endif /* BKE_LV2FEATURES_HPP */
