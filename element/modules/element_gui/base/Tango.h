/*
    Tango.h - This file is part of Element
    Copyright (C) 2013  Michael Fisher <mfisher31@gmail.com>

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

#ifndef ELEMENT_TANGO_H
#define ELEMENT_TANGO_H

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

#endif /* ELEMENT_TANGO_H */
