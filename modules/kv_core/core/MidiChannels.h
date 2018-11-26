
#pragma once

struct JUCE_API MidiChannels
{
    MidiChannels()
    {
       reset();
    }

    MidiChannels (const MidiChannels& o) { operator= (o);  }

    ~MidiChannels() = default;

    inline void reset()
    {
        channels.setRange (0, 17, false);
        channels.setBit (0, true);
    }

    inline void setChannel (const int channel)
    {
        jassert (channel >= 1 && channel <= 16);
        reset();
        channels.setBit (0, false);
        channels.setBit (channel, true);
    }

    inline void setChannels (const BigInteger& newChannels)
    {
        channels = newChannels;
    }

    inline void setOmni (const bool omni)               { channels.setBit (0, omni); }
    inline bool isOmni() const noexcept                 { return  channels[0]; }
    inline bool isOn (const int channel) const noexcept     { return  channels[0] ||  channels[channel]; }
    inline bool isNotOn (const int channel) const noexcept  { return !channels[0] && !channels[channel]; }
    
    inline MidiChannels& operator= (const MidiChannels& o)
    {
        channels = o.channels;
        return *this;
    }

    const BigInteger& get() const { return channels; }
private:
    BigInteger channels;
};
