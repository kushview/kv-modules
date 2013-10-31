
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

/** Config: ELEMENT_PLUGINHOST_LV2

    Enable classes for hosting LV2 plugins
*/
#ifndef ELEMENT_PLUGINHOST_LV2
  // nothing
#endif


/** Config: ELEMENT_DOCKING

    Build the docking widgets classes
*/
#ifndef ELEMENT_DOCKING
  // nothing
#endif

/** Config: ELEMENT_SESSION

    Build the Session/World base classes
*/
#ifndef ELEMENT_DOCKING
  // nothing
#endif


// Header include'ing is handled in implementation files.  This is so the library
// can be built and installed like a normal system library with a pkg-config file.
// The catch is that you MUST include the element source tree in your include path
// e.g. -I/path/to/element

#endif   // ELEMENT_MODELS_H_INCLUDED
