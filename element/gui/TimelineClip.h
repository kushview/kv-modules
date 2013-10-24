#ifndef TIMELINECLIP_H
#define TIMELINECLIP_H

#include <element/juce.hpp>

namespace Element {
namespace Gui {


    class TimelineBase;

    class TimelineClip : public Component
    {
    public:

        virtual ~TimelineClip();

        inline void
        adjustTrackIndex (int trackDelta, bool notify)
        {
            if (trackDelta != 0)
                setTrackIndex (trackIndex() + trackDelta, notify);
        }

        virtual bool moveable() const { return true; }
        virtual bool resizable() const { return true; }
        virtual bool timeIsMusical() const { return false; }

        virtual void getTime (Range<double>& time) const = 0;
        virtual void setTime (const Range<double>& time) = 0;
        virtual int32 trackIndex() const = 0;

        inline void
        setSelected (bool isSelected)
        {
            if (selected == isSelected)
                return;

            selected = isSelected;
            selectedStateChanged();
        }

        inline bool isSelected() const { return selected; }


        virtual bool hitTest (int x, int y);
        virtual void mouseDoubleClick (const MouseEvent &event);
        virtual void mouseDown (const MouseEvent& ev);
        virtual void mouseDrag (const MouseEvent& ev);
        virtual void mouseMove (const MouseEvent& e);
        virtual void mouseUp (const MouseEvent &event);



    protected:

        TimelineClip (TimelineBase&);
        void recycle();
        TimelineBase& timeline();
        const TimelineBase& timeline() const;

        virtual void reset() { }
        virtual void selectedStateChanged() { }
        virtual int32 trackRequested (const int32 track) { return track; }

        uint16 ticksPerBeat() const;

    private:

        friend class TimelineBase;

        TimelineBase& owner;
        ComponentDragger dragger;

        int lastSnap, currentTrack;
        bool isResizing, selected;

        void setTrackIndex (const int track, bool notify);
        void setTimeInternal (const Range<double>& time);
        void getTimeInternal (Range<double>& time);

    };


}}
#endif // TIMELINECLIP_H
