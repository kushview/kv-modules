/*
    This file is part of the Kushview Modules for JUCE
    Copyright (c) 2014-2019  Kushview, LLC.  All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "../JuceLibraryCode/JuceHeader.h"

namespace kv {

class TestRunner : public UnitTestRunner
{
public:
    TestRunner() { }
    ~TestRunner() { }
    
    /** Runs a single test by name ignoring case */
    void runSingleTest (const String& name)
    {
        auto& allTests (UnitTest::getAllTests());
        Array<UnitTest*> tests;
        
        for (int i = allTests.size(); --i >= 0;)
        {
            auto* test = allTests.getUnchecked(i);
            if (name.equalsIgnoreCase(test->getName()))
                { tests.add (test); break; }
        }
        
        runTests (tests);
    }
};

class DummyTest : public UnitTest
{
public:
    DummyTest() : UnitTest ("dummy") { }
    void runTest() override
    {
        beginTest("true is true");
        expectEquals (true, true);
    }
};

static DummyTest sDummyTest;

}

int main (int argc, char* argv[])
{
    kv::TestRunner runner;
    if (argc > 1)
        runner.runSingleTest (argv [1]);
    else
        runner.runAllTests();
    return 0;
}
