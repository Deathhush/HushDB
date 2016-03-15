#include <iostream>
#include <map>
using namespace std;

#include "Framework\UnitTest.h"
using namespace Hush;
using namespace Hush::UnitTest;

#include "..\Engine\Client\Client.h"
#include "..\Engine\StorageEngine\MemoryStorage.h"
#include "..\Engine\Schema.h"
using namespace HushDB;

#include "TestUtility.h"

TESTCLASS(MapDataRowTests)
{
    TESTMETHOD(TestBasic)
    {   
        MemoryDataRow::Ptr mapRow = make_shared<MemoryDataRow>();

        mapRow->Schema = make_shared<TupleDesc>();
        mapRow->Schema->AddColumn(T("id"), SqlType::Int);
        mapRow->Schema->AddColumn(T("data"), SqlType::String);
        mapRow->Schema->AddColumn(T("region"), SqlType::Int);
        
        mapRow->Values.push_back(make_shared<DbInt>(1));
        mapRow->Values.push_back(make_shared<DbString>(STR("test data 1")));
        mapRow->Values.push_back(make_shared<DbInt>(10));
                
        IDataRow::Ptr row = mapRow;

        DbInt::Ptr c1 = row->GetValue<DbInt>(STR("id"));
        Assert::AreEqual(1, c1->Value);
        Assert::IsFalse(c1->IsNull);

        DbString::Ptr c2 = row->GetValue<DbString>(STR("data"));
        Assert::AreEqual(STR("test data 1"), c2->Value);
        Assert::IsFalse(c2->IsNull);

        DbInt::Ptr c3 = row->GetValue<DbInt>(STR("region"));
        Assert::AreEqual(10, c3->Value);
        Assert::IsFalse(c3->IsNull);        
    }
};

TESTCLASS(MemoryTableTests)
{
    TESTMETHOD(TestEnumerateVectorTable)
    {
        MemoryTable::Ptr table = TestUtility::CreateMemoryTable();


        IDataReader::Ptr reader = table->OpenScan();

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
};