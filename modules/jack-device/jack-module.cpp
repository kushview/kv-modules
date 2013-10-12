
#include "element/session.hpp"
#include "jack.hpp"

namespace Element {
namespace jack {

    class JackModule : public Module {
    public:

        JackModule()  { }
        ~JackModule() { }

        void load (World* world)
        {
            DeviceManager& devs (world->devices());
            devs.addAudioDeviceType (Jack::createAudioIODeviceType (nullptr));
        }
    };
}}

extern "C" {
    Element::Module*
    element_module_load()
    {
        return new Element::jack::JackModule();
    }
}
