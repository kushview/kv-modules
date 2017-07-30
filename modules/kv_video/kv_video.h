/*
  BEGIN_JUCE_MODULE_DECLARATION
 
  ID:            kv_video
  vendor:        Kushview, LLC.
  version:       0.0.1
  name:          Video wrapping classes for use in JUCE
  description:   Provides classes to read media streams from video files or to
                 mux audio into an existing video
  dependencies:  juce_audio_basics, juce_audio_formats, kv_core
  website:       http://kushview.net
  license:       BSD V2 3-clause

  END_JUCE_MODULE_DECLARATION
*/

#pragma once
#define KV_VIDEO_H_INCLUDED

#include <kv_core/kv_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace kv {
 #include "sources/VideoSource.h"
}
