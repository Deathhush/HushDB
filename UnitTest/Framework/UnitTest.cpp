#include <string>
#include <exception>

using namespace std;

#include "UnitTest.h"

namespace Hush
{
    namespace UnitTest
    {    
        void Assert::IsTrue(bool condition, const wstring& message)
        {
            if (!condition)
            {
                throw message;
            }
        }

        void Assert::IsFalse(bool condition, const wstring& message)
        {
            if (condition)
            {
                throw message;
            }
        }

        void TestRunner::AddTestClass(TestClassInfo* info)
        {
            if (!headTestClassInfo)
            {
                // This is the first node
                TestRunner::headTestClassInfo = info;
                TestRunner::tailTestClassInfo = info;
            }
            else
            {
                // Add the new node to the tail
                tailTestClassInfo->next = info;
                tailTestClassInfo = info;
            }    
        }

        void TestRunner::RunTests()
        {
            bool allTestPassed = true;
            try
            {
                TestClassInfo* info = TestRunner::headTestClassInfo;
                while (info)
                {                    
                    info->RunTests();                    
                    info = info->next;
                }
            }
            catch (wstring& e)
            {
                wcout << e << endl;
                allTestPassed = false;
            }

            if (allTestPassed)
            {
                wcout << L"<> All tests passed." <<endl;
            }
            else
            {
                wcout << L"<> Test failed." <<endl;
            }
        }
        
        TestClassInfo* TestRunner::headTestClassInfo = 0;
        TestClassInfo* TestRunner::tailTestClassInfo = 0;
    }
}

