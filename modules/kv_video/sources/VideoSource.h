
class JUCE_API VideoSource {
public:
    VideoSource();
    virtual ~VideoSource();
    virtual void tick() { }
};
