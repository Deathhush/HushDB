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

    TESTMETHOD(TestAssertion)        
    {
        auto s = Convert<int>::ToString(100);
        Assert::AreEqual(STR("100"), s);
        Assert::IsTrue(ToStringTrait<int>::Supported);
        Assert::IsFalse(ToStringTrait<runtime_error>::Supported);        
    }
};
