/*
    This file is part of the Kushview Modules for JUCE
    Copyright (c) 2014-2019  Kushview, LLC.  All rights reserved.

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

#pragma once

namespace kv {
namespace FileHelpers {

int64 calculateStreamHashCode (juce::InputStream& stream);
int64 calculateFileHashCode (const juce::File& file);

bool overwriteFileWithNewDataIfDifferent (const juce::File& file, const void* data, size_t numBytes);
bool overwriteFileWithNewDataIfDifferent (const juce::File& file, const juce::MemoryOutputStream& newData);
bool overwriteFileWithNewDataIfDifferent (const juce::File& file, const juce::String& newData);

bool containsAnyNonHiddenFiles (const juce::File& folder);

juce::String unixStylePath (const juce::String& path);
juce::String windowsStylePath (const juce::String& path);
juce::String currentOSStylePath (const juce::String& path);

bool shouldPathsBeRelative (juce::String path1, juce::String path2);
bool isAbsolutePath (const juce::String& path);

// A windows-aware version of juce::File::getRelativePath()
juce::String getRelativePathFrom (const juce::File& file, const juce::File& sourceFolder);

// removes "/../" bits from the middle of the path
juce::String simplifyPath (juce::String::CharPointerType path);
juce::String simplifyPath (const juce::String& path);
}


class FileModificationDetector
{
public:
    FileModificationDetector (const juce::File& f)
        : file (f)
    {
    }

    const juce::File& getFile() const                     { return file; }
    void fileHasBeenRenamed (const juce::File& newFile)   { file = newFile; }

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
    juce::File file;
    juce::Time fileModificationTime;
    juce::int64 fileHashCode, fileSize;
};
}