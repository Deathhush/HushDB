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
            objectDefHeap.InsertDataRow(TableDef::CreateTableDefRow(0, T("t1"), ObjectType::SimpleHeap));
            objectDefHeap.InsertDataRow(TableDef::CreateTableDefRow(0, T("t2"), ObjectType::SimpleHeap));
        }

        BufferManager bufferManager(T("test_data.hdf"));
        DataFileHeaderPage* headerPage = bufferManager.GetPageAs<DataFileHeaderPage>(0);


        SimpleHeap heap(&bufferManager, headerPage->ObjectDefPageId);

        IEnumerator<RowPtr>::Ptr enumerator = heap.GetEnumerator();
        Assert::IsTrue(enumerator->MoveNext());
        
        IObjectDef::Ptr tableDef = make_shared<TableDef>(enumerator->Current().data);
        Assert::AreEqual(0, tableDef->ObjectId());
        Assert::AreEqual(STR("t1"), tableDef->ObjectName());
        Assert::AreEqual(ObjectType::SimpleHeap, tableDef->Type());        
    }
};