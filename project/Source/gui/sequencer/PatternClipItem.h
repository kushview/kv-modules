/*
    PatternClip.h - This file is part of BTV

    Copyright (C) 2013 BKE, LLC  All rights reserved.
      * Michael Fisher <mfisher@bketech.com>
*/

#ifndef ELEMENT_PATTERN_CLIP_H
#define ELEMENT_PATTERN_CLIP_H

#include "../../sampler/Pattern.h"
#include "SequencerClipItem.h"

namespace Element {
namespace Gui {

    class PatternClipItem :  public SequencerClipItem
    {
    public:

        PatternClipItem (SequencerComponent& s, const ClipModel& t, PatternPtr p)
            : SequencerClipItem (s, t),
              pattern (p)
        {
            setModel (t);
            name.referTo (p->getPropertyAsValue (Slugs::name));
            model.lengthValue() = timeline().timeScale().beatsPerBar() *
                    p->totalBars() * timeline().timeScale().ppq();
        }

        ~PatternClipItem() { }

        virtual void paint (Graphics &g)
        {
            g.setColour (Colours::lightgreen);
            g.fillAll();
            g.setColour (Colours::lightblue.contrasting (1.0f));
            g.drawFittedText (name.toString(), 0, 0, getWidth(), getHeight(),
                              Justification::centred, 1);
        }

        void getTime (Range<double> &time) const
        {
            time.setStart (model.start());
            time.setLength (model.length());
        }

        void setTime (const Range<double> &time)
        {
            model.setTime (time);
        }

    protected:

        Range<double> tickSpan;

        PatternPtr pattern;
        friend class SequencerComponent;
        Value name;

    };

}}
#endif // ELEMENT_PATTERN_CLIP_H
