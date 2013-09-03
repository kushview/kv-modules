
#include "element/core.hpp"
using namespace element;
#if 1
class DummyTest : public UnitTest
{
public:

    DummyTest() : UnitTest ("dummy") { }
    virtual ~DummyTest() { }

    void runTest()
    {
        expect (true);
    }
};

static DummyTest dummy_test;
#endif

