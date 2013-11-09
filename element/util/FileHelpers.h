/*
    FileHelpers.h - This file is part of Element

    Copyright (C) 2013  Michael Fisher <mfisher31@gmail.com>
      * Adapted from Introjucer - http://juce.com

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

#ifndef ELEMENT_FILE_HELPERS_H
#define ELEMENT_FILE_HELPERS_H

#include <element/Juce.h>

namespace Element {
namespace FileHelpers {

    int64 calculateStreamHashCode (InputStream& stream);
    int64 calculateFileHashCode (const File& file);

    bool overwriteFileWithNewDataIfDifferent (const File& file, const void* data, size_t numBytes);
    bool overwriteFileWithNewDataIfDifferent (const File& file, const MemoryOutputStream& newData);
    bool overwriteFileWithNewDataIfDifferent (const File& file, const String& newData);

    bool containsAnyNonHiddenFiles (const File& folder);

    String unixStylePath (const String& path);
    String windowsStylePath (const String& path);
    String currentOSStylePath (const String& path);

    bool shouldPathsBeRelative (String path1, String path2);
    bool isAbsolutePath (const String& path);

    // A windows-aware version of File::getRelativePath()
    String getRelativePathFrom (const File& file, const File& sourceFolder);

    // removes "/../" bits from the middle of the path
    String simplifyPath (String::CharPointerType path);
    String simplifyPath (const String& path);
}


class FileModificationDetector
{
public:
    FileModificationDetector (const File& f)
        : file (f)
    {
    }

    const File& getFile() const                     { return file; }
    void fileHasBeenRenamed (const File& newFile)   { file = newFile; }

    bool hasBeenModified() const
    {
        return fileModificationTime != file.getLastModificationTime()
                 && (fileSize != file.getSize()
                      || FileHelpers::calculateFileHashCode (file) != fileHashCode);
    }

    void updateHash()
    {
        fileModificationTime = file.getLastModificationTime();
        fileSize = file.getSize();
        fileHashCode = FileHelpers::calculateFileHashCode (file);
    }

private:
    File file;
    Time fileModificationTime;
    int64 fileHashCode, fileSize;
};

}

#endif   // ELEMENT_FILE_HELPERS_H
