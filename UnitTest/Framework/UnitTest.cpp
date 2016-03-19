#include <string>
#include <exception>

using namespace std;

#include "UnitTest.h"

namespace Hush
{
    namespace UnitTest
    {
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
            bool allTestPassed = false;
            try
            {
                TestClassInfo* info = TestRunner::headTestClassInfo;
                while (info)
                {                    
                    info->RunTests();                    
                    info = info->next;
                }
                allTestPassed = true;
            }
            catch (AssertFailedException& e)
            {
                wcout <<  e.GetMessage() << endl;                
            }
            catch (Exception& Ex)
            {
                wcout << L"Unhandled Exception caught. Exception message:" << Ex.GetMessage() << endl;
            }
            catch (exception& ex)
            {
                cout << "std::exception caught. exception message:" << ex.what() << endl;
            }
            catch(...)
            {
                
                wcout << L"Unhandled exception caught. Test failed." << endl;
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

