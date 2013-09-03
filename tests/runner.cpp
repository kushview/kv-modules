
#include "element/core.hpp"
#include "element/engine/plugin-manager.hpp"
#include "element/engine/world.hpp"

using namespace element;

int main()
{
    World* world = new World();
    if (AudioPluginFormat* vst = world->plugins().format ("AudioUnit"))
    {
        StringArray plugs = vst->searchPathsForPlugins (FileSearchPath(), true);
        for (const String& str : plugs)
            std::cout << "plug: " << str << std::endl;

        OwnedArray<PluginDescription> descs;
        vst->findAllTypesForFile (descs, "build/Components/Element.component");
        std::cout << "Found plugins : " << plugs.size() << std::endl;
    }
}
