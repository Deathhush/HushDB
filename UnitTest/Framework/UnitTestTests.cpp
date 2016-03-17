#include "UnitTest.h"

#include <iostream>
#include <memory>
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

        Assert::AreEqual(1, 1);
        Assert::AreNotEqual(1, 2);
    }

    TESTMETHOD(TestPointerAssertion)
    {
        shared_ptr<int> i1 = make_shared<int>(1);
        shared_ptr<int> i2 = make_shared<int>(1);
        shared_ptr<int> i3 = make_shared<int>(2);
        
        Assert::AreEqual(i1.get(), i2.get());
        Assert::AreNotEqual(i2.get(), i3.get());

        Assert::AreEqual(i1, i2);
        Assert::AreNotEqual(i2, i3);
    }
};
