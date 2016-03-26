#include <iostream>
using namespace std;

#include "Framework\UnitTest.h"
#include "TestUtility.h"
using namespace Hush;
using namespace Hush::UnitTest;

#include "..\Engine\Client\Client.h"
#include "..\Engine\StorageEngine\DataFile.h"
using namespace HushDB;

TESTCLASS(DataFileTests)
{
public:
    virtual void TestInitialize() override
    {
        remove("test_data.hdf");
    }

    TESTMETHOD(TestDataFileHeaderPageSize)
    {
        Assert::AreEqual(PageSize, sizeof(DataFileHeaderPage));        
    }

    TESTMETHOD(TestCreateDataFile)
    {
        {
            DataFile::CreateEmptyDataFile(T("test_data.hdf"));
            BufferManager bufferManager(T("test_data.hdf"));

            DataFileHeaderPage* headerPage = bufferManager.GetPageAs<DataFileHeaderPage>(0);
            SimpleHeap objectDefHeap(&bufferManager, headerPage->ObjectDefPageId);
            objectDefHeap.InsertDataRow(TableDefAccessor::CreateTableDefRow(1000, T("t1"), ObjectType::SimpleHeap, InvalidPageId));
            objectDefHeap.InsertDataRow(TableDefAccessor::CreateTableDefRow(1001, T("t2"), ObjectType::SimpleHeap, InvalidPageId));

            SimpleHeap columnDefHeap(&bufferManager, headerPage->ColumnDefPageId);
            columnDefHeap.InsertDataRow(ColumnDefAccessor::CreateColumnDefRow(1000, STR("id"), 0, SqlType::Int));
            columnDefHeap.InsertDataRow(ColumnDefAccessor::CreateColumnDefRow(1000, STR("data"), 1, SqlType::String));
            columnDefHeap.InsertDataRow(ColumnDefAccessor::CreateColumnDefRow(1000, STR("region"), 2, SqlType::Int));

            columnDefHeap.InsertDataRow(ColumnDefAccessor::CreateColumnDefRow(1001, STR("id"), 0, SqlType::Int));
            columnDefHeap.InsertDataRow(ColumnDefAccessor::CreateColumnDefRow(1001, STR("description"), 1, SqlType::String));
            columnDefHeap.InsertDataRow(ColumnDefAccessor::CreateColumnDefRow(1001, STR("price"), 2, SqlType::Float));
            bufferManager.ReleasePage(headerPage->GetPageId());
        }

        BufferManager bufferManager(T("test_data.hdf"));
        DataFileHeaderPage* headerPage = bufferManager.GetPageAs<DataFileHeaderPage>(0);

        SimpleHeap heap(&bufferManager, headerPage->ObjectDefPageId);

        IEnumerator<RowPtr>::Ptr enumerator = heap.GetEnumerator();
        Assert::IsTrue(enumerator->MoveNext());
        
        IObjectDef::Ptr tableDef = make_shared<TableDefAccessor>(enumerator->Current().data);
        Assert::AreEqual(1000, tableDef->ObjectId());
        Assert::AreEqual(STR("t1"), tableDef->ObjectName());
        Assert::AreEqual(ObjectType::SimpleHeap, tableDef->Type());

        Assert::IsTrue(enumerator->MoveNext());
        tableDef = make_shared<TableDefAccessor>(enumerator->Current().data);
        Assert::AreEqual(1001, tableDef->ObjectId());
        Assert::AreEqual(STR("t2"), tableDef->ObjectName());
        Assert::AreEqual(ObjectType::SimpleHeap, tableDef->Type());

        Assert::IsFalse(enumerator->MoveNext());

        SimpleHeap columnDefHeap(&bufferManager, headerPage->ColumnDefPageId);

        enumerator = columnDefHeap.GetEnumerator();
        Assert::IsTrue(enumerator->MoveNext());

        ColumnDefAccessor::Ptr columnDef = make_shared<ColumnDefAccessor>(enumerator->Current().data);
        Assert::AreEqual(1000, columnDef->ObjectId());
        Assert::AreEqual(STR("id"), columnDef->ColumnName());
        Assert::AreEqual(0, columnDef->ColumnId());
        Assert::AreEqual(SqlType::Int, columnDef->ColumnType());

        Assert::IsTrue(enumerator->MoveNext());
        columnDef = make_shared<ColumnDefAccessor>(enumerator->Current().data);
        Assert::AreEqual(1000, columnDef->ObjectId());
        Assert::AreEqual(STR("data"), columnDef->ColumnName());
        Assert::AreEqual(1, columnDef->ColumnId());
        Assert::AreEqual(SqlType::String, columnDef->ColumnType());

        Assert::IsTrue(enumerator->MoveNext());
        columnDef = make_shared<ColumnDefAccessor>(enumerator->Current().data);
        Assert::AreEqual(1000, columnDef->ObjectId());
        Assert::AreEqual(STR("region"), columnDef->ColumnName());
        Assert::AreEqual(2, columnDef->ColumnId());
        Assert::AreEqual(SqlType::Int, columnDef->ColumnType());

        Assert::IsTrue(enumerator->MoveNext());
        columnDef = make_shared<ColumnDefAccessor>(enumerator->Current().data);
        Assert::AreEqual(1001, columnDef->ObjectId());
        Assert::AreEqual(STR("id"), columnDef->ColumnName());
        Assert::AreEqual(0, columnDef->ColumnId());
        Assert::AreEqual(SqlType::Int, columnDef->ColumnType());

        Assert::IsTrue(enumerator->MoveNext());
        columnDef = make_shared<ColumnDefAccessor>(enumerator->Current().data);
        Assert::AreEqual(1001, columnDef->ObjectId());
        Assert::AreEqual(STR("description"), columnDef->ColumnName());
        Assert::AreEqual(1, columnDef->ColumnId());
        Assert::AreEqual(SqlType::String, columnDef->ColumnType());

        Assert::IsTrue(enumerator->MoveNext());
        columnDef = make_shared<ColumnDefAccessor>(enumerator->Current().data);
        Assert::AreEqual(1001, columnDef->ObjectId());
        Assert::AreEqual(STR("price"), columnDef->ColumnName());
        Assert::AreEqual(2, columnDef->ColumnId());
        Assert::AreEqual(SqlType::Float, columnDef->ColumnType());

    }
};