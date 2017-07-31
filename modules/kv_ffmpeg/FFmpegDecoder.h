
class JUCE_API FFmpegDecoder
{
public:
    class Sink
    {
    public:
        Sink() { }
        virtual ~Sink() { }
        virtual void ffmpegVideoDecoded (AVFrame* frame) { }
        virtual void ffmpegAudioDecoded (AVFrame* frame) { }
    };
    
    FFmpegDecoder (Sink&);
    virtual ~FFmpegDecoder();
    
    /** Opens a media file for reading */
    bool openFile (const File& file);
    
    /** Stops and closes this decoder */
    void close();
    
    void read();
    
private:
    Sink& sink;
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
    
    void tick() override;
    
private:
    struct Pimpl;
    friend class Pimpl;
    friend class ContainerDeletePolicy<Pimpl>;
    ScopedPointer<Pimpl> pimpl;
};
