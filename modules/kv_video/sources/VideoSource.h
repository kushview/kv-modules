/*
    This file is part of the Kushview Modules for JUCE
    Copyright (c) 2017-2019  Kushview, LLC.  All rights reserved.

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

class JUCE_API VideoSource
{
public:
    VideoSource();
    virtual ~VideoSource();

    /** Prepare internal resources here */
    virtual void prepareToRender() { }

    /** Release all resources */
    virtual void releaseResources() { }

    /** Called for every frame probably in a video thread */
    virtual void videoTick (const double seconds) { }
    
    /** The width of this source
        This is required for syncronous video sources
     */
    virtual int getWidth() const { return 0; }

    /** The height of this source
        This is required for syncronous video sources
     */
    virtual int getHeight() const { return 0; }
    
    virtual void renderAudio (const AudioSourceChannelInfo&) { }
};
