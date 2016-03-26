#include <iostream>
using namespace std;

#include "Framework\UnitTest.h"
#include "TestUtility.h"
using namespace Hush;
using namespace Hush::UnitTest;

#include "..\Engine\Client\Client.h"
#include "..\Engine\StorageEngine\DataFile.h"
#include "..\Engine\Catalog.h"
#include "TestUtility.h"
using namespace HushDB;

TESTCLASS(CatalogTests)
{
    virtual void TestInitialize() override
    {
        remove("test_catalog.hdf");
    }

    TESTMETHOD(TestLoadCatalogFromDataFile)
    {
        {
            DataFile::CreateEmptyDataFile(T("test_catalog.hdf"));
            BufferManager bufferManager(T("test_catalog.hdf"));

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
        }

        BufferManager bufferManager(T("test_catalog.hdf"));

        Catalog catalog(&bufferManager);
        IObjectDef::Ptr tableDef = catalog.FindTable(T("t1"));
        Assert::IsNotNullPtr(tableDef);
    }
};