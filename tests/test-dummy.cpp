
#include "element/Core.h"
using namespace Element;
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

