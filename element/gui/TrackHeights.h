#ifndef ELEMENT_TRACK_HEIGHTS_H
#define ELEMENT_TRACK_HEIGHTS_H

#include <vector>
#include <element/Juce.h>
#include <element/Utils.h>

namespace Element {
namespace Gui {



    class TrackHeights
    {
    public:

        enum DefaultSize
        {
            Micro  = 12,
            Mini   = 18,
            Small  = 30,
            Normal = 60,
            Large  = 120,
            Larger = 240
        };


        TrackHeights()
            : mSpacing (1), mOffset (0), mDirty (0) { }

        /** Add a height to the end of the list */
        inline int
        add (int h = Normal)
        {
            mHeights.push_back (h);
            enablement.setBit (size() - 1);
            return size() - 1;
        }

        inline int
        set (int t, int h = Normal)
        {
            if (h < 0)
                h = -1;

            while (size() < t + 1)
                add();

            mHeights[t] = h;
            ++mDirty;

            return h;
        }

        inline int
        get (int track_index) const
        {
            return track_index < size() ? mHeights.at (track_index) : -1;
        }

        size_t  size()   const { return mHeights.size(); }
        bool    empty()  const { return mHeights.size() == 0; }

        int spacing() const { return mSpacing; }
        void setSpacing (int spacing)  { mSpacing = spacing; ++mDirty; }

        inline int
        trackAtY (int y) const
        {
            y -= mOffset;

            int total = 0;
            int track = 0;

            for (Vec::const_iterator i = mHeights.begin();
                 i != mHeights.end(); ++i)
            {
                if (! enablement [track])
                    continue;

                total += ((*i) + mSpacing);
                if (y < total)
                    break;
                ++track;
            }

            if (track < 0)
                track = 0;

            return track;
        }

        inline int
        trackY (int track) const
        {
            int y = mOffset;

            for (int i = 0; i < size(); ++i)
            {
                if (i == track)
                    return y;

                if (enablement [i])
                    y += (mHeights[i] + mSpacing);
            }

            return y;
        }

        inline int
        normalizedY (int y) const
        {
            int ny = 0;
            int span = 0;
            int track = 0;

            for (Vec::const_iterator i = mHeights.begin();
                 i != mHeights.end(); ++i)
            {
                if (! enablement [track])
                    continue;

                int gap = (*i) + mSpacing;
                span += gap;

                if (y < span)
                    return ny;

                ny += gap; ++i; ++track;

            }

            return ny;
        }

        inline int
        offset() const
        {
            return mOffset;
        }

        inline void
        setOffset (int dy)
        {
            mOffset = dy;
            ++mDirty;
        }

        inline void setEnabled (int track, bool isEnabled = true) { isEnabled ? enablement.setBit (track) : enablement.clearBit (track); }
        inline void disable (int track) { setEnabled (track, false); }

        inline int
        totalHeight (int endTrack = 0) const
        {
            int total = 0;

            endTrack = endTrack == 0 ? size() : endTrack;
            for (Vec::const_iterator i = mHeights.begin();
                 i != mHeights.end(); ++i)
            {
                if (--endTrack < 0)
                    break;

                if (enablement [endTrack])
                    total += *i + mSpacing;
            }

            return total;
        }

        /** Ensure entries are available.
            newly created tracks are visible by default*/
        inline void ensureTracks (int numTracks, int sz = (int) Normal)
        {
            if (sz <= 0)
                sz = TrackHeights::Normal;

            while (mHeights.size() < numTracks)
            {
                add (sz);
                int index = size() - 1;
                enablement.setBit (index);
            }
        }

        inline void setEnablement (const BigInteger& e)
        {
            enablement.setRange (0, mHeights.size(), false);
            enablement |= e;
        }

        inline bool trackIsVisible (int trackIndex) const {
            return enablement [trackIndex];
        }

        inline bool trackIsVisibleAtY (int y) const {
            return trackIsVisible (trackAtY (y));
        }

    private:

        typedef std::vector<int> Vec;
        BigInteger enablement;
        Vec mHeights;
        int mSpacing;
        int mOffset;
        int mDirty;

    };

}}

#endif // ELEMENT_TRACK_HEIGHTS_H
