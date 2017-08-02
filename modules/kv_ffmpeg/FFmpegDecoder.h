
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
    
    int getWidth() const;
    int getHeight() const;
    AVPixelFormat getPixelFormat() const;
    
private:
    Sink* sink;
    struct Pimpl;
    friend struct Pimpl;
    friend struct ContainerDeletePolicy<Pimpl>;
    ScopedPointer<Pimpl> pimpl;
};

class JUCE_API FFmpegReader
{
public:
};

class JUCE_API FFmpegVideoSource : public VideoSource
{
public:
    FFmpegVideoSource();
    ~FFmpegVideoSource();
    void openFile (const File& file);
    
    void tick() override;
    void process (int64 pts);
    
    Image findImage (double pts);
    
private:
    struct Pimpl;
    friend class Pimpl;
    friend class ContainerDeletePolicy<Pimpl>;
    ScopedPointer<Pimpl> pimpl;
};
