
#ifndef ELEMENT_CORE_H_INCLUDED
#define ELEMENT_CORE_H_INCLUDED

#ifndef JUCE_MODULE_AVAILABLE_element
 /* If you fail to make sure that all your compile units are building JUCE with the same set of
    option flags, then there's a risk that different compile units will treat the classes as having
    different memory layouts, leading to very nasty memory corruption errors when they all get
    linked together. That's why it's best to always include the Introjucer-generated AppConfig.h
    file before any juce headers.

    Note that if you do have an AppConfig.h file and hit this warning, it means that it doesn't
    contain the JUCE_MODULE_AVAILABLE_xxx flags, which are necessary for some inter-module
    functionality to work correctly. In that case, you should either rebuild your AppConfig.h with
    the latest introjucer, or fix it manually to contain these flags.
 */
 #ifdef _MSC_VER
  #pragma message ("Have you included your AppConfig.h file before including the JUCE headers?")
 #else
  #warning "Have you included your AppConfig.h file before including the JUCE headers?"
 #endif
#endif

#include <memory>

#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/juce_core.h"
#include "juce_data_structures/juce_data_structures.h"

namespace juce {
    class AudioIODeviceCallback;
}

using namespace juce;

#include "element/AssetTree.h"
#include "element/Asset.h"
#include "element/AssetType.h"
#include "element/Arc.h"
#include "element/Controller.h"
#include "element/Engine.h"
#include "element/LinkedList.h"
#include "element/Pointer.h"

#include "element/gui/MidiEditorBody.h"

#endif   // ELEMENT_MODELS_H_INCLUDED
