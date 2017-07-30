
class JUCE_API FFmpegDecoder
{
public:
    FFmpegDecoder();
    virtual ~FFmpegDecoder();
    
    /** Opens a media file for reading */
    bool openFile (const File& file);
    
    /** Stops and closes this decoder */
    void close();
    
private:
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
    
    void tick() override;
};
