#include "UnitTest.h"

#include <iostream>
using namespace std;

using namespace Hush;
using namespace Hush::UnitTest;

TESTCLASS(UnitTestTests)
{
    TESTMETHOD(TestException)
    EXPECTEXCEPTION(int)
    {
        throw 1;            
    }

    TESTMETHOD(TestException1)
    EXPECTEXCEPTION(int)
    {
        throw 2;
    }

    TESTMETHOD(TestException3)
    EXPECTEXCEPTION(runtime_error)
    {
        throw runtime_error("test");
    }
};
