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

    TESTMETHOD(TestInsertThreeRows)
    {
        DataPage page(1);
        Int16 row1 = 1;
        Int32 row2 = 2;
        Int64 row3 = 3;

        RowId rowId1 = page.InsertRowValue(row1);
        RowId rowId2 = page.InsertRowValue(row2);
        RowId rowId3 = page.InsertRowValue(row3);

        Assert::AreEqual(3, page.GetRowCount(), L"Row count is not 3 after inserted 3 rows.");

        RowPtr rowPtr1 = page.GetRowPtr(rowId1);
        AssertRowPtr(row1, rowPtr1, L"Row1 is not correct.");

        RowPtr rowPtr2 = page.GetRowPtr(rowId2);
        AssertRowPtr(row2, rowPtr2, L"Row2 is not correct.");

        RowPtr rowPtr3 = page.GetRowPtr(rowId3);
        AssertRowPtr(row3, rowPtr3, L"Row3 is not correct.");
    }

    TESTMETHOD(TestDeleteRow)
    {
        DataPage page(1);
        Int16 row1 = 1; // Offset 0 (before deletion), 0 (after deletion)
        Int32 row2 = 2; // Offset 2, 2
        Int16 rowToBeDeleted = 99; // Offset 6, -
        Int64 row3 = 3; // Offset 8, 6
        Int32 row4 = 4; // Offset 16, 14

        Int16 offset1 = 0;
        Int16 offset2 = offset1 + sizeof(Int16) / sizeof(Byte);
        Int16 offset3 = offset2 + sizeof(Int32) / sizeof(Byte);
        Int16 offset4 = offset3 + sizeof(Int64) / sizeof(Byte);
        Int16 offsetFreeSpace = offset4 + sizeof(Int32) / sizeof(Byte);

        RowId rowId1 = page.InsertRowValue(row1);
        RowId rowId2 = page.InsertRowValue(row2);
        RowId rowIdToBeDeleted = page.InsertRowValue(rowToBeDeleted);
        RowId rowId3 = page.InsertRowValue(row3);
        RowId rowId4 = page.InsertRowValue(row4);

        page.DeleteRow(rowIdToBeDeleted);

        // Check row3 and row4 are correctly moved after deletion
        Assert::AreEqual(4, page.GetRowCount(), L"Row count is not 4 after row deletion.");
        Assert::AreEqual(offset3, page.GetRowOffset(rowId3), L"The starting offset of Row3 is not correct after deletion.");
        Assert::AreEqual(offsetFreeSpace, page.GetFreeSpaceOffset(), L"The starting offset of free space is not correct.");

        // Check the content of row3 and row4
        RowPtr rowPtr3 = page.GetRowPtr(rowId3);
        AssertRowPtr(row3, rowPtr3, L"Row3 is not correct after deletion.");

        RowPtr rowPtr4 = page.GetRowPtr(rowId4);
        AssertRowPtr(row4, rowPtr4, L"Row4 is not correct after deletion.");
    }

    TESTMETHOD(TestInsertAfterDeletion)
    {
        DataPage page(1);
        Int16 row1 = 1;
        Int32 row2 = 2;
        Int64 row3 = 3;

        RowId rowId1 = page.InsertRowValue(row1);
        RowId rowId2 = page.InsertRowValue(row2);
        RowId rowId3 = page.InsertRowValue(row3);

        page.DeleteRow(rowId2);

        Int64 row4 = 4;
        RowId rowId4 = page.InsertRowValue(row4);

        RowPtr rowPtr3 = page.GetRowPtr(rowId3);
        AssertRowPtr(row3, rowPtr3, L"Row3 is not correct after insertion.");

        RowPtr rowPtr4 = page.GetRowPtr(rowId4);
        AssertRowPtr(row4, rowPtr4, L"Row4 is not correct after deletion.");

        Int64 row5 = 5;
        RowId rowId5 = page.InsertRowValue(row5);

        RowPtr rowPtr5 = page.GetRowPtr(rowId5);
        AssertRowPtr(row5, rowPtr5, L"Row4 is not correct after deletion.");
    }

    TESTMETHOD(TestSlotIdReuse)
    {
        DataPage page(1);
        Int16 row1 = 1;
        Int32 row2 = 2;
        Int64 row3 = 3;

        RowId rowId1 = page.InsertRowValue(row1);
        RowId rowId2 = page.InsertRowValue(row2);
        RowId rowId3 = page.InsertRowValue(row3);

        page.DeleteRow(rowId2);

        Int64 row4 = 4;

        RowId rowId4 = page.InsertRowValue(row4);
        RowPtr rowPtr4 = page.GetRowPtr(rowId4);
        AssertRowPtr(row4, rowPtr4, L"Row4 is not correct after deletion.");

        Assert::AreEqual(rowId2.slotId, rowId4.slotId, L"SlotId is not reused after row deletion.");
    }
};