#ifndef TESTS_HPP
#define TESTS_HPP

#include "element/engine/block-node.hpp"
#include "element/engine/compiled-graph.hpp"
#include "element/engine/context.hpp"
#include "element/engine/duplex-port.hpp"
#include "element/engine/lv2format.hpp"
#include "element/engine/lv2features.hpp"
#include "element/engine/plugin-manager.hpp"

#include "element/buffer.hpp"
#include "element/core.hpp"
#include "element/world.hpp"

using namespace Element;

static World* __world = nullptr;

static inline
World* test_world()
{
    if (! __world)
        __world = new World();
    return __world;
}

static inline
void test_world_free()
{
    if (! __world)
        return;

    World* d = __world;
    __world = nullptr;
    delete d;
}

static inline
World* test_world_reset()
{
    test_world_free();
    Logger::writeToLog ("World was reset");
    return test_world();
}


class ElementTest : public UnitTest
{
public:

    ElementTest (const String& name) : UnitTest (name) { }
    virtual ~ElementTest() { }
    inline World* getWorld (bool reset = false) { return reset ? test_world_reset() : test_world(); }

};

#endif // TESTS_HPP
