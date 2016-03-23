#include <iostream>
using namespace std;

#include "Framework\UnitTest.h"
#include "TestUtility.h"
using namespace Hush;
using namespace Hush::UnitTest;

#include "..\Engine\Client\Client.h"
#include "..\Engine\StorageEngine\Heap.h"
using namespace HushDB;

TESTCLASS(SimpleHeapTests)
{
public:
    virtual void TestInitialize() override
    {
        remove("test_heap.hdf");
    }

    TESTMETHOD(TestSimpleHeapHeaderPage)
    {     
        Assert::AreEqual(PageSize, sizeof(SimpleHeapHeaderPageData));        
    }

    TESTMETHOD(TestBasicHeapOperation)
    {
        BufferManager bufferManager;
        SimpleHeap heap(&bufferManager);
                       
        Int32 row1 = 1;
        Int32 row2 = 2;
        Int64 row3 = 3;

        RowId rowId1 = heap.InsertRowValue(row1);
        RowId rowId2 = heap.InsertRowValue(row2);
        RowId rowId3 = heap.InsertRowValue(row3);

        IEnumerator<RowPtr>::Ptr enumerator = heap.GetEnumerator();
        Assert::IsTrue(enumerator->MoveNext());

        AssertRowPtr(row1, enumerator->Current(), L"The data is not correctly set for row1.");

        Assert::IsTrue(enumerator->MoveNext());
        AssertRowPtr(row2, enumerator->Current(), L"The data is not correctly set for row2.");

        Assert::IsTrue(enumerator->MoveNext());
        AssertRowPtr(row3, enumerator->Current(), L"The data is not correctly set for row3.");

        Assert::IsFalse(enumerator->MoveNext());
        Assert::IsFalse(enumerator->MoveNext());
    }

    TESTMETHOD(TestPersistHeap)
    {
        Int32 row1 = 1;
        Int32 row2 = 2;
        Int64 row3 = 3;

        PageId headerPageId = InvalidPageId;

        {
            BufferManager bufferManager(T("test_heap.hdf"));
            SimpleHeap heap(&bufferManager);

            headerPageId = heap.GetHeaderPageId();

            heap.InsertRowValue(row1);
            heap.InsertRowValue(row2);
            heap.InsertRowValue(row3);
        }

        BufferManager bufferManager(T("test_heap.hdf"));
        SimpleHeap heap(&bufferManager, headerPageId);

        IEnumerator<RowPtr>::Ptr enumerator = heap.GetEnumerator();
        Assert::IsTrue(enumerator->MoveNext());

        AssertRowPtr(row1, enumerator->Current(), L"The data is not correctly set for row1.");

        Assert::IsTrue(enumerator->MoveNext());
        AssertRowPtr(row2, enumerator->Current(), L"The data is not correctly set for row2.");

        Assert::IsTrue(enumerator->MoveNext());
        AssertRowPtr(row3, enumerator->Current(), L"The data is not correctly set for row3.");

        Assert::IsFalse(enumerator->MoveNext());
        Assert::IsFalse(enumerator->MoveNext());
    }
};