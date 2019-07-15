/*
    This file is part of the Kushview Modules for JUCE
    Copyright (c) 2014-2019  Kushview, LLC.  All rights reserved.

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

namespace kv {

File AutoSampler::Context::getCaptureDir() const
{
    String path = outputPath;
    if (! File::isAbsolutePath (path))
        return File();
    
    if (! File::isAbsolutePath (path))
    {
        jassertfalse;
        return File();
    }

    File directory (path);
    return directory.getChildFile ("capture");
}

ValueTree AutoSampler::Context::createValueTree() const
{
    ValueTree context ("AutoSamplerContext");
    context.setProperty ("source",     source, nullptr)
            .setProperty ("keyStart",   keyStart, nullptr)
            .setProperty ("keyEnd",     keyEnd, nullptr)
            .setProperty ("keyStride",  keyStride, nullptr)
            .setProperty ("baseName",   baseName, nullptr)
            .setProperty ("noteLength", noteLength, nullptr)
            .setProperty ("tailLength", tailLength, nullptr)
            .setProperty ("instrumentName", instrumentName, nullptr)
            .setProperty ("outputPath", outputPath, nullptr)
            .setProperty ("channels",   channels, nullptr);
    
    auto layers = context.getOrCreateChildWithName ("layers", nullptr);
    
    for (int i = 0; i < 4; ++i)
    {
        #if 0
        ValueTree layer ("layer");
        layer.setProperty ("enabled", layerEnabled [i], nullptr)
             .setProperty ("velocity", layerVelocities [i], nullptr);
        layers.appendChild (layer, nullptr);
        #endif
    }

    return context;
}

AutoSampler::CaptureDetails* AutoSampler::Context::createLayerRenderDetails (const int layerIdx,
                                                                             const double sourceSampleRate,
                                                                             AudioFormatManager& formats,
                                                                             TimeSliceThread& thread) const
{
    jassert (sourceSampleRate > 0.0);
    jassert (isPositiveAndBelow (layerIdx, layers.size()));
    jassert (keyStride > 0);
    const auto& layer = layers.getReference (layerIdx);

    std::unique_ptr<CaptureDetails> details (new CaptureDetails());
    auto& seq = details->sequence;

    int key = keyStart;
    int64 frame = 0;

    const int64 noteFrames = static_cast<int64> (sourceSampleRate * ((double) layer.noteLength / 1000.0));
    const int64 tailFrames = static_cast<int64> (sourceSampleRate * ((double) layer.tailLength / 1000.0));
   
    const File directory (getCaptureDir());
    const auto extension = ".wav"; // FormatType::getFileExtension (FormatType::fromSlug (format));

    if (isPositiveAndBelow (layer.midiProgram, 127))
    {
        auto pgc = MidiMessage::programChange (layer.midiChannel, layer.midiProgram);
        pgc.setTimeStamp (static_cast<double> (frame));
        seq.addEvent (pgc);
        frame += roundToInt (sourceSampleRate); // program delay
    }

    while (key <= keyEnd)
    {
        auto* const sample  = details->samples.add (new NoteInfo());
        sample->layerId     = layer.uuid;
        sample->index       = details->samples.size() - 1;
        sample->note        = key;
    
        String identifier;
        identifier << String(layerIdx).paddedLeft ('0', 3) << "_"
                   << String(key).paddedLeft ('0', 3);
        String fileName = identifier;
        fileName << "." << extension;
        sample->file = directory.getChildFile (fileName);

        auto noteOn  = MidiMessage::noteOn (layer.midiChannel, key, layer.velocity);
        noteOn.setTimeStamp (static_cast<double> (frame));
        sample->start = frame;
        frame += noteFrames;
        
        auto noteOff = MidiMessage::noteOff (layer.midiChannel, key);
        noteOff.setTimeStamp (static_cast<double> (frame));
        frame += tailFrames;
        sample->stop = frame;

        seq.addEvent (noteOn);
        seq.addEvent (noteOff);
        key += keyStride;
    }
    
    return details.release();
}

void AutoSampler::Context::writeToFile (const File& file) const
{
    const auto tree = createValueTree();
    
    if (auto* xml = tree.createXml())
    {
        xml->writeToFile (file, String());
        deleteAndZero (xml);
    }
}

void AutoSampler::Context::restoreFromFile (const File& file)
{
    if (auto* xml = XmlDocument::parse (file))
    {
        auto tree               = ValueTree::fromXml (*xml);
        auto& ctx               = *this;
        ctx.source              = tree.getProperty ("source", ctx.source);
        ctx.baseName            = tree.getProperty ("baseName", ctx.baseName);
        ctx.instrumentName      = tree.getProperty ("instrumentName", ctx.instrumentName);
        ctx.keyEnd              = tree.getProperty ("keyEnd", ctx.keyEnd);
        ctx.keyStart            = tree.getProperty ("keyStart", ctx.keyStart);
        ctx.keyStride           = tree.getProperty ("keyStride", ctx.keyStride);
        ctx.noteLength          = tree.getProperty ("noteLength", ctx.noteLength);
        ctx.tailLength          = tree.getProperty ("tailLength", ctx.tailLength);
        ctx.outputPath          = tree.getProperty ("outputPath", ctx.outputPath);
        ctx.channels            = tree.getProperty ("channels", ctx.channels);
        auto layers = tree.getChildWithName ("layers");
        
        for (int i = 0; i < 4; ++i)
        {
            #if 0
            auto l = layers.getChild (i);
            ctx.layerEnabled[i]     = (bool) l.getProperty("enabled", ctx.layerEnabled [i]);
            ctx.layerVelocities[i]  = l.getProperty("velocity", ctx.layerVelocities [i]);
            #endif
        }

        deleteAndZero (xml);
    }
}

AutoSampler::AutoSampler (AudioFormatManager& f)
    : formats (f), 
      thread ("kvasmp"),
      started (*this),
      stopped (*this),
      cancelled (*this),
      progress (*this)
{ }

AutoSampler::~AutoSampler()
{
    thread.stopThread (2 * 1000);
}

void AutoSampler::reset()
{
    frame = 0;
    layer = 0;
}

void AutoSampler::prepare (double newSampleRate, int newBufferSize)
{
    if (prepared)
        release();
    jassert (! prepared);
    reset();
    sampleRate  = newSampleRate;
    blockSize   = newBufferSize;
    thread.startThread();
    prepared = true;

    const int sampleFileNumChans = 2;
    channels.calloc (sampleFileNumChans + 2);
}

void AutoSampler::renderCycleBegin()
{
    if (renderingRequest.get() != rendering.get())
    {
        rendering.set (renderingRequest.get());
        if (isRendering())
        {
            DBG("[VCP] rendering started");
            stopped.cancelPendingUpdate();
            started.cancelPendingUpdate();
            started.triggerAsyncUpdate();
            reset();
        }
        else
        {
            DBG("[VCP] rendering stopped");
            started.cancelPendingUpdate();
            stopped.cancelPendingUpdate();
            triggerAsyncUpdate();
        }
    }
}

void AutoSampler::getNextMidiBlock (MidiBuffer& buffer, int nframes)
{
    
    if (! isRendering())
        return;

    ScopedLock sl (getCallbackLock());

    if (layer >= nlayers)
    {
        renderingRequest.compareAndSetBool (0, 1);
        return;
    }

    auto* const detail  = details.getUnchecked (layer);
    const auto& midi    = detail->sequence;
    const int numEvents = midi.getNumEvents();
    const double start  = static_cast<double> (frame);
    const int64 endFrame  = frame + nframes;
    const double end    = static_cast<double> (endFrame);
    bool layerChanged   = false;
    int i;
    
    buffer.clear();

    for (i = midi.getNextIndexAtTime (start); i < numEvents;)
    {
        const auto* const ev = midi.getEventPointer (i);
        const auto& msg = ev->message;
        const double timestamp = msg.getTimeStamp();
        if (timestamp >= end)
            break;
        
        buffer.addEvent (msg, roundToInt (timestamp - start));

       #if 1
        if (msg.isProgramChange())
        {
            DBG("[VCP] program change: " << msg.getProgramChangeNumber());
        }
        if (msg.isNoteOn())
        {
            DBG("[VCP] note on: " << MidiMessage::getMidiNoteName (msg.getNoteNumber(), true, true, 4) << " - "
                << static_cast<int64> (msg.getTimeStamp()) << " velocity: " << (int) msg.getVelocity());
        }
        else if (msg.isNoteOff())
        {
            DBG("[VCP] note off: " << MidiMessage::getMidiNoteName (msg.getNoteNumber(), true, true, 4) << " - "
                << static_cast<int64> (msg.getTimeStamp()));
        }
       #endif

        ++i;
    }
}

void AutoSampler::writeAudioFrames (AudioSampleBuffer& audio)
{
    if (! isRendering())
        return;

    if (layer >= nlayers)
    {
        renderingRequest.compareAndSetBool (0, 1);
        return;
    }

    ScopedLock sl (getCallbackLock());

    const int nframes           = audio.getNumSamples();
    auto* const detail          = details.getUnchecked (layer);
    const int numDetails        = detail->getNumSamples();
    const auto lastStopFrame    = detail->getHighestEndFrame();
    const int64 startFrame      = frame - writerDelay;
    const int64 endFrame        = startFrame + nframes;
    
    for (int i = detail->getNextSampleIndex (startFrame); i < numDetails;)
    {
        auto* const render = detail->getCaptureInfo (i);
        if (render->start >= endFrame)
            break;
   
        if (render->start >= startFrame && render->start < endFrame)
        {
            progress.triggerAsyncUpdate();
            const int localFrame = render->start - startFrame;
            for (int c = 0; c < context.channels; ++c)
                channels[c] = audio.getWritePointer (c, localFrame);
            render->writer->write (channels.get(), endFrame - render->start);
        }
        else if (render->stop >= startFrame && render->stop < endFrame)
        {
            for (int c = 0; c < context.channels; ++c)
                channels[c] = audio.getWritePointer (c);
            render->writer->write (channels.get(), render->stop - startFrame);
        }
        else if (startFrame >= render->start && startFrame < render->stop)
        {
            for (int c = 0; c < context.channels; ++c)
                channels[c] = audio.getWritePointer (c);
            render->writer->write (channels.get(), nframes);
        }

        ++i;
    }
    
    if (lastStopFrame >= startFrame && lastStopFrame < endFrame)
    {
        ++layer;
        frame = 0;
        event = 0;
    }
    else
    {
        frame += nframes;
    }
}

void AutoSampler::renderCycleEnd()
{

}

void AutoSampler::release()
{
    details.clearQuick (true);
    prepared = false;
}

void AutoSampler::handleAsyncUpdate()
{
    if (isRendering())
    {
        jassertfalse;
        return;
    }

    OwnedArray<CaptureDetails> old;
    Context ctx;
    {
        ScopedLock sl (getCallbackLock());
        details.swapWith (old);
        ctx = context;
    }

    const auto captureDir = ctx.getCaptureDir();
    const auto projectDir = captureDir.getParentDirectory();
    const auto samplesDir = projectDir.getChildFile ("samples");
    const bool wasCancelled = shouldCancel.get() == 1;

    if (! wasCancelled)
    {
        ValueTree manifest ("samples");
        
        for (auto* detail : old)
        {
            for (auto* const info : detail->samples)
            {
                ValueTree sample ("sample");
                info->writer.reset();
                auto totalTime = static_cast<double> (info->stop - info->start) / sampleRate;

                sample.setProperty ("uuid", Uuid().toString(), nullptr)
                      .setProperty ("set", info->layerId.toString(), nullptr)
                      .setProperty ("file", info->file.getFileName(), nullptr)
                      .setProperty ("note", info->note, nullptr)
                      .setProperty ("sampleRate", sampleRate, nullptr)
                      .setProperty ("length", totalTime, nullptr)
                      .setProperty ("timeIn", 0.0, nullptr)
                      .setProperty ("timeOut", totalTime, nullptr);
                manifest.appendChild (sample, nullptr);
            }
        }
        
        if (samplesDir.exists())
            samplesDir.deleteRecursively();
        captureDir.copyDirectoryTo (samplesDir);
        captureDir.deleteRecursively();
        
        samples = manifest;
        stopped.triggerAsyncUpdate();
    }
    else
    {
        for (auto* detail : old)
            for (auto* const info : detail->samples)
                info->writer.reset();
        captureDir.deleteRecursively();
        samples = ValueTree ("samples");
        cancelled.triggerAsyncUpdate();
    }

    shouldCancel.set (0);
}

void AutoSampler::setContext (const Context& newContext)
{
    if (rendering.get() != 0 || renderingRequest.get() != 0)
        return;
    ScopedLock rsl (lock);
    context = newContext;
}

Result AutoSampler::start (const Context& newContext, int latencySamples)
{
    if (isRendering())
        return Result::fail ("recording already in progress");

    jassert (sampleRate > 0.0);
    jassert (blockSize > 0);

    const auto extension = ".wav"; // FormatType::getFileExtension (FormatType::fromSlug (newContext.format));
    auto* const audioFormat = formats.findFormatForFileExtension (extension);
    if (! audioFormat)
    {
        jassertfalse;
        return Result::fail ("could not create encoder for recording");
    }

    OwnedArray<CaptureDetails> newDetails;
    const File directory = newContext.getCaptureDir();
    if (directory.exists())
        directory.deleteRecursively();
    directory.createDirectory();
    steps.clearQuick();
    totalSteps = 0;
    for (int i = 0; i < newContext.layers.size(); ++i)
    {
        String layerName = "Layer "; layerName << int (i + 1);
        auto* details = newDetails.add (newContext.createLayerRenderDetails (
                                        i, sampleRate, formats, thread));
        for (auto* const sample : details->samples)
        {
            const auto file = sample->file;
            std::unique_ptr<FileOutputStream> stream (file.createOutputStream());
            String step = layerName;
            step << " - " << MidiMessage::getMidiNoteName (sample->note, true, true, 4);
            steps.add (step);

            if (stream)
            {
                if (auto* const writer = audioFormat->createWriterFor (
                        stream.get(),
                        sampleRate,
                        newContext.channels,
                        newContext.bitDepth,
                        StringPairArray(),
                        0
                    ))
                {
                    sample->writer.reset (new AudioFormatWriter::ThreadedWriter (writer, thread, 8192));
                    stream.release();
                    DBG("[VCP] " << file.getFullPathName());
                }
                else
                {
                    jassertfalse;
                    return Result::fail ("couldn't create writer for recording");
                }
            }
            else
            {
                String message = "could not open ";
                message << sample->file.getFileName() << " for recording";
                return Result::fail (message);
            }
        }
    }

    totalSteps = steps.size();
    samples = ValueTree (samplesType);

    {
        ScopedLock sl (getCallbackLock());
        nlayers         = jmax (0, newContext.layers.size());
        writerDelay     = jmax (0, newContext.latency + latencySamples);
        context         = newContext;
        details.swapWith (newDetails);
    }

    if (shouldCancel.compareAndSetBool (0, 1))
    {
        DBG("[VCP] cancel flag reset");
    }

    if (renderingRequest.compareAndSetBool (1, 0))
    {
        DBG("[VCP] render start requested");
        DBG("[VCP] compensate samples: " << writerDelay);
    }

    newDetails.clear();
    return Result::ok();
}

void AutoSampler::cancel()
{
    shouldCancel.set (1);
    if (renderingRequest.compareAndSetBool (0, 1))
    {
        DBG("[VCP] render cancel requested");
    }
}

}
