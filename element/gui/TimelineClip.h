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

        virtual bool moveable() const { return true; }
        virtual bool resizable() const { return true; }
        virtual bool timeIsMusical() const { return false; }

        virtual void getTime (Range<double>& time) const = 0;
        virtual void setTime (const Range<double>& time) = 0;


        inline void
        setSelected (bool isSelected)
        {
            if (selected == isSelected)
                return;

            selected = isSelected;
            selectedStateChanged();
        }

        inline bool isSelected() const { return selected; }
        inline const int& trackIndex() const { return currentTrack; }

        virtual bool hitTest (int x, int y);
        virtual void mouseDoubleClick (const MouseEvent &event);
        virtual void mouseDown (const MouseEvent& ev);
        virtual void mouseDrag (const MouseEvent& ev);
        virtual void mouseMove (const MouseEvent& e);
        virtual void mouseUp (const MouseEvent &event);

        inline void adjustTrackIndex (int trackDelta, bool notify) {
            if (trackDelta != 0)
                setTrackIndex (currentTrack + trackDelta, notify);
        }

    protected:

        TimelineClip (TimelineBase&);

        void recycle();
        virtual void reset() { }
        virtual void selectedStateChanged() { }
        TimelineBase& timeline();

        virtual int trackRequested (const int& track) { return track; }

    private:

        friend class TimelineBase;

        TimelineBase& owner;
        ComponentDragger dragger;
        ComponentBoundsConstrainer limit;

        int lastSnap, currentTrack;
        bool isResizing, selected;

        void setTrackIndex (const int track, bool notify);

    };


}}
#endif // TIMELINECLIP_H
