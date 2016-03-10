#include <iostream>
using namespace std;

#include "Framework\UnitTest.h"
using namespace Hush;
using namespace Hush::UnitTest;

#include "..\Engine\Client\Client.h"
using namespace HushDB;

TESTCLASS(ClientTests)
{
    TESTMETHOD(TestDbValues)
    {
        DbInt intValue(10);
        Assert::AreEqual(10, intValue.Value);
        Assert::IsFalse(intValue.IsNull);
    }
};