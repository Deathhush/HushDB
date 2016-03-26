#include <iostream>
#include <map>
using namespace std;

#include "Framework\UnitTest.h"
using namespace Hush;
using namespace Hush::UnitTest;

#include "..\Engine\Client\Client.h"
#include "..\Engine\StorageEngine\MemoryStorage.h"
#include "..\Engine\Client\SqlCommand.h"
#include "..\Engine\Client\SqlConnection.h"
#include "..\Engine\Catalog.h"
#include "..\Engine\StorageEngine\DataFile.h"
using namespace HushDB;

#include "TestUtility.h"

TESTCLASS(QueryExecutionTests)
{
    virtual void TestInitialize() override
    {
        remove("test_query.hdf");
    }

    TESTMETHOD(TestQueryMemoryTable)
    {
        MemoryTable::Ptr table = TestUtility::CreateMemoryTable();

        Catalog::Ptr catalog = make_shared<Catalog>();

        MemoryTableDef tableDef;
        tableDef.Name = T("t1");
        tableDef.Schema->AddColumn(T("id"), SqlType::Int);
        tableDef.Schema->AddColumn(T("data"), SqlType::String);
        tableDef.Schema->AddColumn(T("region"), SqlType::Int);
        tableDef.Table = table;

        catalog->AddTable(tableDef);

        SqlCommand command(catalog.get());
        command.CommandText = T("select id, data, region from t1");
        IDataReader::Ptr reader = command.ExecuteReader();
        AssertT1Data(reader);
    }

    const Char* TestFileName = T("test_query.hdf");

    TESTMETHOD(TestQueryDataFile)
    {
        PrepareT1DataFile(TestFileName);

        SqlConnection::Ptr connection = make_shared<SqlConnection>();
        connection->Database = TestFileName;
        connection->Open();
        SqlCommand command(connection);
        command.CommandText = T("select id, data, region from t1");
        IDataReader::Ptr reader = command.ExecuteReader();

        AssertT1Data(reader);
    }

    void AssertT1Data(IDataReader::Ptr reader)
    {
        Assert::IsTrue(reader->MoveNext());

        IDataRow::Ptr row1 = reader->Current();

        DbInt::Ptr c1 = row1->GetValue<DbInt>(STR("id"));
        Assert::AreEqual(1, c1->Value);
        Assert::IsFalse(c1->IsNull);

        DbString::Ptr c2 = row1->GetValue<DbString>(STR("data"));
        Assert::AreEqual(STR("test data 1"), c2->Value);
        Assert::IsFalse(c2->IsNull);

        DbInt::Ptr c3 = row1->GetValue<DbInt>(STR("region"));
        Assert::AreEqual(10, c3->Value);
        Assert::IsFalse(c3->IsNull);

        Assert::IsTrue(reader->MoveNext());

        IDataRow::Ptr row2 = reader->Current();

        c1 = row2->GetValue<DbInt>(STR("id"));
        Assert::AreEqual(2, c1->Value);
        Assert::IsFalse(c1->IsNull);

        c2 = row2->GetValue<DbString>(STR("data"));
        Assert::AreEqual(STR("test data 2"), c2->Value);
        Assert::IsFalse(c2->IsNull);

        c3 = row2->GetValue<DbInt>(STR("region"));
        Assert::AreEqual(20, c3->Value);
        Assert::IsFalse(c3->IsNull);

        Assert::IsFalse(reader->MoveNext());
    }      

    void PrepareT1DataFile(const String& fileName)
    {
        DataFile::CreateEmptyDataFile(fileName);
        BufferManager bufferManager(fileName);

        DataFileHeaderPage* headerPage = bufferManager.GetPageAs<DataFileHeaderPage>(0);
        SimpleHeap t1(&bufferManager);

        SimpleHeap objectDefHeap(&bufferManager, headerPage->ObjectDefPageId);
        objectDefHeap.InsertDataRow(TableDefAccessor::CreateTableDefRow(1000, T("t1"), ObjectType::SimpleHeap, t1.GetHeaderPageId()));

        SimpleHeap columnDefHeap(&bufferManager, headerPage->ColumnDefPageId);
        columnDefHeap.InsertDataRow(ColumnDefAccessor::CreateColumnDefRow(1000, STR("id"), 0, SqlType::Int));
        columnDefHeap.InsertDataRow(ColumnDefAccessor::CreateColumnDefRow(1000, STR("data"), 1, SqlType::String));
        columnDefHeap.InsertDataRow(ColumnDefAccessor::CreateColumnDefRow(1000, STR("region"), 2, SqlType::Int));

        t1.InsertDataRow(TestUtility::GetT1Row1());
        t1.InsertDataRow(TestUtility::GetT1Row2());

        bufferManager.ReleasePage(headerPage->GetPageId());
    }
};


