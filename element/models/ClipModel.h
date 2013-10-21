#ifndef BTV_CLIP_MODEL_H
#define BTV_CLIP_MODEL_H

#include "element/Slugs.h"
#include "element/models/ObjectModel.h"

namespace Element {


    class ClipModel :  public ObjectModel
    {
    public:

        ClipModel (const ValueTree& data) : ObjectModel (data) {}

        ClipModel (ValueTree& data, double start, double length, double offset = 0.0f)
            : ObjectModel (data)
        {
            assert (data.isValid());
            assert (node().hasType (Slugs::clip));
            node().setProperty ("start", start, nullptr);
            node().setProperty ("length", length, nullptr);
            node().setProperty ("offset", offset, nullptr);
            setMissingProperties();
        }

        ClipModel (double start, double length, double offset = 0.0f)
            : ObjectModel (Slugs::clip)
        {
            node().setProperty ("start", start, nullptr);
            node().setProperty ("length", length, nullptr);
            node().setProperty ("offset", offset, nullptr);
            setMissingProperties();
        }

        ClipModel (const File& file)
            : ObjectModel (Slugs::clip)
        {
            setMissingProperties();
            node().setProperty ("file", file.getFullPathName(), nullptr);
        }

        ClipModel (const ClipModel& other)
            : ObjectModel (other.node())
        { }

        virtual ~ClipModel() { }

        inline void
        setTime (const Range<double>& time, double clipOffset = 0.0f)
        {
            startValue()  = time.getStart();
            lengthValue() = time.getLength();
            offsetValue() = clipOffset;
        }

        inline double start()  const { return node().getProperty ("start"); }
        inline Value startValue()    { return node().getPropertyAsValue ("start", nullptr); }
        inline double length() const { return node().getProperty (Slugs::length); }
        inline Value lengthValue()   { return node().getPropertyAsValue (Slugs::length, nullptr); }
        inline double offset() const { return node().getProperty ("offset"); }
        inline Value offsetValue()   { return node().getPropertyAsValue ("offset", nullptr); }
        inline double end()    const { return start() + length(); }

        inline bool isValid()  const { return node().isValid() && node().hasType (Slugs::clip); }

        inline bool operator== (const ClipModel& m) const { return node() == m.node(); }
        inline bool operator!= (const ClipModel& m) const { return node() != m.node(); }

    private:

        ClipModel& operator= (const ClipModel&);

        inline void
        setMissingProperties()
        {
            if (! node().hasProperty("start"))
                node().setProperty ("start", 0.0f, nullptr);
            if (! node().hasProperty("length"))
                node().setProperty ("length", 1.0f, nullptr);
            if (! node().hasProperty("offset"))
                node().setProperty ("offset", 1.0f, nullptr);
        }

    };
}

#endif // BTV_CLIP_MODEL_H
