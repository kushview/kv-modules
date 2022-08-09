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
/** Manipulates a cross-platform partial file path. (Needed because juce::File is designed
    for absolute paths on the active OS)
*/
class RelativePath
{
public:

    enum RootFolder {
        unknown,
        projectFolder,
        buildTargetFolder
    };

    RelativePath() : root (unknown) {}

    RelativePath (const juce::String& relPath, const RootFolder rootType)
        : path (FileHelpers::unixStylePath (relPath)), root (rootType) { }

    RelativePath (const juce::File& file, const juce::File& rootFolder, const RootFolder rootType)
        : path (FileHelpers::unixStylePath (FileHelpers::getRelativePathFrom (file, rootFolder))), root (rootType)
    { }

    RootFolder getRoot() const                              { return root; }

    juce::String toUnixStyle() const                              { return FileHelpers::unixStylePath (path); }
    juce::String toWindowsStyle() const                           { return FileHelpers::windowsStylePath (path); }

    juce::String getFileName() const                              { return getFakeFile().getFileName(); }
    juce::String getFileNameWithoutExtension() const              { return getFakeFile().getFileNameWithoutExtension(); }

    juce::String getFileExtension() const                         { return getFakeFile().getFileExtension(); }
    bool hasFileExtension (const juce::String& extension) const   { return getFakeFile().hasFileExtension (extension); }
    bool isAbsolute() const                                 { return FileHelpers::isAbsolutePath (path); }

    RelativePath withFileExtension (const juce::String& extension) const
    {
        return RelativePath (path.upToLastOccurrenceOf (".", ! extension.startsWithChar ('.'), false) + extension, root);
    }

    RelativePath getParentDirectory() const
    {
        juce::String p (path);
        if (path.endsWithChar ('/'))
            p = p.dropLastCharacters (1);

        return RelativePath (p.upToLastOccurrenceOf ("/", false, false), root);
    }

    RelativePath getChildFile (const juce::String& subpath) const
    {
        if (FileHelpers::isAbsolutePath (subpath))
            return RelativePath (subpath, root);

        juce::String p (toUnixStyle());
        if (! p.endsWithChar ('/'))
            p << '/';

        return RelativePath (p + subpath, root);
    }

    RelativePath rebased (const juce::File& originalRoot, const juce::File& newRoot, const RootFolder newRootType) const
    {
        if (isAbsolute())
            return RelativePath (path, newRootType);

        return RelativePath (FileHelpers::getRelativePathFrom (originalRoot.getChildFile (toUnixStyle()), newRoot), newRootType);
    }

private:

    juce::String path;
    RootFolder root;

    juce::File getFakeFile() const
    {
        // This method gets called very often, so we'll cache this directory.
        static const juce::File currentWorkingDirectory (juce::File::getCurrentWorkingDirectory());
        return currentWorkingDirectory.getChildFile (path);
    }
};
}
