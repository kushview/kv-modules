/*
    SequencerTrack.cpp - This file is part of Element
    Copyright (C) 2013  Michael Fisher <mfisher31@gmail.com>

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


SequencerTrack::SequencerTrack (Sequencer& o, const TrackModel& t)
    : sequencer (o)
{
    track = new TrackModel (t);
    state = track->state();

    armed.referTo (track->armedValue());
    muted.referTo (track->mutedValue());
    soloed.referTo (track->soloedValue());
    volume.referTo (track->volumeValue());

    bin.setScoped (true);

    setPlayConfigDetails (2, 2, o.getSampleRate(), o.getBlockSize());
}

SequencerTrack::~SequencerTrack()
{

}

ClipSource*
SequencerTrack::cursorClip() const
{
    return sequencer.trackClip (trackIndex);
}

void
SequencerTrack::prepareToPlay (double sampleRate, int blockSize)
{
    setPlayConfigDetails (getNumInputChannels(), getNumOutputChannels(),
                          sampleRate, blockSize);

    for (ClipSource* clip : bin) {
        clip->prepareToPlay (blockSize, sampleRate);
    }
}

void
SequencerTrack::releaseResources()
{
    for (ClipSource* clip : bin) {
        clip->releaseResources();
    }
}

void
SequencerTrack::processBlock (AudioSampleBuffer &buffer, MidiBuffer &midi)
{
    if (! sequencer.position().isPlaying)
        return;

    if (ClipSource* src = cursorClip())
    {
        AudioSourceChannelInfo info (buffer);
        info.clearActiveBufferRegion();

        const int64 start = sequencer.position().timeInSamples - src->frameStart();
        const int64 end   = start + buffer.getNumSamples();


        DBG ("frame start: " + String (src->frameStart()) + " start: " + String(start) + " end " + String (end));
        if (end > 0 && end < src->frameEnd()) {
            src->setNextReadPosition (start);
            src->getNextAudioBlock (info);
        }
    }
}

void SequencerTrack::processBlockBypassed (AudioSampleBuffer&, MidiBuffer&)
{

}
