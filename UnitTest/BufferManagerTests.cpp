#include <iostream>
using namespace std;

#include "Framework\UnitTest.h"
using namespace Hush;
using namespace Hush::UnitTest;

#include "..\Engine\StorageEngine\BufferManager.h"
using namespace HushDB;

TESTCLASS(BufferManagerTests)
{
    TESTMETHOD(AllocatePage)
    {
        BufferManager bufferManager;
        Page* page = bufferManager.AllocatePage();
        DataPage* dataPage = bufferManager.AllocatePage<DataPage>();

        Assert::IsNotNull(page, L"Allocated page is null.");
        Assert::IsNotNull(dataPage, L"Allocated dataPage is null.");

        Assert::AreNotEqual(InvalidPageId, page->GetPageId(), L"The PageId of allocated DataPage is not valid.");
        Assert::AreNotEqual(InvalidPageId, dataPage->GetPageId(), L"The PageId of allocated DataPage is not valid.");

        Assert::AreNotEqual(page->GetPageId(), dataPage->GetPageId(), L"PageId of the allocated two pages are the same.");

        Assert::AreEqual(page, bufferManager.GetPage(page->GetPageId()), L"The page is not correctly retreived.");
        Assert::AreEqual(dataPage, bufferManager.GetPageAs<DataPage>(dataPage->GetPageId()), L"The dataPage is not correctly retreived.");
    }
};