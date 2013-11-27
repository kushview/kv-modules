#ifndef ELEMENT_MODULE_HPP
#define ELEMENT_MODULE_HPP

    class World;

    /** Abstract base class for all Element modules */
    class Module
    {
    public:

        static inline const
        String& extension()
        {
            #if JUCE_MAC
             static String ext = ".dylib";
            #elif JUCE_WINDOWS
             static String ext = ".dll";
            #else
             static String ext = ".so";
            #endif
            return ext;
        }

        Module() : library (nullptr) { }
        virtual ~Module() { }

        virtual void load (World*) = 0;
        virtual void unload() { }
        virtual void run (World*) { }

        DynamicLibrary* library;
    };


extern "C" {

    /** Entry point for element modules */
    Element::Module* element_module_load();

}

#endif // ELEMENT_MODULE_HPP
