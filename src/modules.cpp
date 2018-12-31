
#include "AppConfig.h"

#if JUCE_MODULE_AVAILABLE_juce_core
 #include <juce_core/juce_core.cpp>
#endif

// #if JUCE_MODULE_AVAILABLE_kv_core
//  #include <kv_core/kv_core.cpp>
// #endif

// #if JUCE_MODULE_AVAILABLE_kv_edd
//  #include <kv_gui/kv_edd.cpp>
// #endif

// #if JUCE_MODULE_AVAILABLE_kv_gui
//  #include <kv_gui/kv_gui.cpp>
// #endif

#include "kv_core/python/pybind11/include/pybind11/embed.h"

namespace py = pybind11;
using namespace py::literals;

int main()
{
    py::scoped_interpreter guard{};
    juce::String code = R"(
message = "Hello, {name}! The answer is {number}".format(**locals())
)";
    auto locals = py::dict("name"_a="World", "number"_a=42);
    py::exec (code.toStdString(), py::globals(), locals);
    auto message = locals["message"].cast<std::string>();
    std::cout << message << std::endl;
    return 0;
}
