
#ifndef BKE_TANGO_HPP
#define BKE_TANGO_HPP

#include "element/Juce.h"
#include "element/Pointer.h"

namespace Tango {

    inline Image
    getIconImage (const String& category, const String& slug, int size = 22)
    {
        String path ("/usr/share/icons/Tango/");
        path << size << "x" << size << "/" << category << "/" << slug << ".png";
        return ImageCache::getFromFile (File (path));
    }

    inline Drawable*
    getDrawable (const String& category, const String& slug)
    {
        String path ("/usr/share/icons/Tango/scalable/");
        path << category << "/" << slug << ".svg";
        File f (path);

        if (f.exists())
        {
            XmlDocument doc (f);
            ScopedPointer<XmlElement> elem (doc.getDocumentElement());
            return Drawable::createFromSVG (*elem);
        }

        return nullptr;
    }
}

#endif /* BKE_TANGO_HPP */
