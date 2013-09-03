
#include "tests.hpp"

using namespace element;

class JackDeviceTest : public UnitTest
{
public:

    JackDeviceTest() : UnitTest ("JackDeviceDriver") { }
    ~JackDeviceTest() { }

    void runTest()
    {
        World* world = test_world();

        if (world)
         {
            bool loaded = world->loadModule ("jack-device");
            expect (loaded);
         }
    }
};

JackDeviceTest jackTest;

