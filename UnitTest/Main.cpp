#include <iostream>

#include "Framework\UnitTest.h"
using namespace Hush::UnitTest;

int main()
{
    TestRunner::RunTests();
    std::wcin.get();
}