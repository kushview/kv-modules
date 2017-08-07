/*
    This file is part of the Kushview Modules for JUCE
    Copyright (C) 2017  Kushview, LLC.  All rights reserved.

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

struct MediaDescription
{
    double audioSampleRate;
    double durationSeconds;
};

class FFmpegFrameQueue;
class FFmpegStreamQueue;

/** Decodes media inputs to AVFrames */
class JUCE_API FFmpegDecoder
{
public:
    /** To be safe, subclasses of this should NOT be deleted before the decoder */
    class Sink
    {
    public:
        Sink() { }
        virtual ~Sink() { }
        
        /** Called the instant a video frame is read from a packet */
        virtual void videoFrameDecoded (const AVStream* stream, AVFrame* frame) { }
        
        /** Called the instant an audio frame is read from a packet */
        virtual void audioFrameDecoded (const AVStream* stream, AVFrame* frame) { }
        
        /** Called the instant a subtitle frame is read from a packet */
        virtual void subtitleFrameDecoded (const AVStream* stream, AVFrame* frame) { }
    };
    
    explicit FFmpegDecoder (Sink* s = nullptr, FFmpegStreamQueue* q = nullptr);
    virtual ~FFmpegDecoder();
    
    /** Opens a media file for reading */
    bool openFile (const File& file);
    
    /** Stops and closes this decoder */
    void close();
    
    /** Read the next packet and call sink methods */
    void read();
    
    /** Returns the duration of the media */
    double duration() const { return 1.0; }
    
    /** Fills a MediaDescription struct */
    void getDescription (MediaDescription& desc) { }
    
    /** Sets the sink. The Passed in sync is owned by the caller. */
    void setSink (Sink* newSink)            { sink = newSink; }
    
    /** Returns the width video stream. If no video is available then zero
        will be returned.
     */
    int getWidth() const;

    /** Returns the width video stream. If no video is available then zero
        will be returned.
     */
    int getHeight() const;

    /** Returns pixel format of the format */
    AVPixelFormat getPixelFormat() const;
    
    Rational getRealFrameRate() const;
    
private:
    Sink* sink;
    struct Pimpl;
    friend struct Pimpl;
    friend struct ContainerDeletePolicy<Pimpl>;
    ScopedPointer<Pimpl> pimpl;
};

class JUCE_API FFmpegVideoSource : public VideoSource
{
public:
    FFmpegVideoSource();
    ~FFmpegVideoSource();
    void openFile (const File& file);
    
    Rational getRealFrameRate() const;
    void videoTick (const double seconds) override;
    void renderAudio (const AudioSourceChannelInfo&) override;

    /** currently here for testing only */    
    Image findImage (double pts);
    
private:
    struct Pimpl;
    friend class Pimpl;
    friend class ContainerDeletePolicy<Pimpl>;
    ScopedPointer<Pimpl> pimpl;
};
