#include <iostream>
using namespace std;

#include "Framework\UnitTest.h"
using namespace Hush;
using namespace Hush::UnitTest;

#include "..\Engine\QueryProcessor\Tokenizer.h"
#include "..\Engine\StorageEngine\Page.h"
using namespace HushDB;

template <typename T>
inline void AssertRowPtr(const T& value, const RowPtr& rowPtr, const wstring& message)
{
    Assert::AreEqual<Int32>(sizeof(T), rowPtr.length, message + L"(Size)");
    Assert::AreEqual<T>(value, *((T*)(rowPtr.data)), message + L"(Data)");
}

TESTCLASS(PageTests)
{
    TESTMETHOD(TestPageSize)
    {
        Assert::AreEqual(PageSize, sizeof(Page), L"Page size is not equal to PageSize.");
    }

    TESTMETHOD(TestUseDataPageAsPage)
    {
        DataPage dataPage(PageId{ 100 });
        Page* page = (Page*)(&dataPage);

        Assert::AreEqual({ 100 }, page->GetPageId(), L"PageId is not correctly return when accessing DataPage as Page.");
    }
};


TESTCLASS(DataPageTests)
{
public:
    TESTMETHOD(TestSize)
    {
        Assert::AreEqual(PageSize, sizeof(DataPage), L"DataPage size is not equal to PageSize.");
    }
    TESTMETHOD(TestInit)
    {
        DataPage page(PageId{ 1 });
        Assert::AreEqual(0, page.GetRowCount(), L"The row count of new data page is not 0.");
    }

    TESTMETHOD(TestInsertOneRow)
    {
        DataPage page(PageId{ 1 });
        Int32 rowData = 9;
        RowId rid = page.InsertRowValue(rowData);
        Assert::AreEqual(1, page.GetRowCount(), L"Row count of the page is not 1.");

        Int32 returnedData = 0;
        RowPtr rowPtr = page.GetRowPtr(rid);
        AssertRowPtr(rowData, rowPtr, L"The returned row is not the same as the inserted one.");
    }
};