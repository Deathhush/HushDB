#include <iostream>
#include <map>
using namespace std;

#include "Framework\UnitTest.h"
using namespace Hush;
using namespace Hush::UnitTest;

#include "..\Engine\Client\Client.h"
#include "..\Engine\StorageEngine\MemoryStorage.h"
using namespace HushDB;

TESTCLASS(MapDataRowTests)
{
    TESTMETHOD(TestBasic)
    {   
        MapDataRow::Ptr mapRow = make_shared<MapDataRow>();
        
        mapRow->Values.insert(pair<String, MapDataRow::ValueType>(STR("id"), make_shared<DbInt>(1)));
        mapRow->Values.insert(pair<String, MapDataRow::ValueType>(STR("data"), make_shared<DbString>(STR("test data 1"))));
        mapRow->Values.insert(pair<String, MapDataRow::ValueType>(STR("region"), make_shared<DbInt>(1)));
                
        IDataRow::Ptr row = mapRow;

        DbInt::Ptr c1 = row->GetValue<DbInt>(STR("id"));
        Assert::AreEqual(1, c1->Value);
        Assert::IsFalse(c1->IsNull);

        DbString::Ptr c2 = row->GetValue<DbString>(STR("data"));
        Assert::AreEqual(STR("test data 1"), c2->Value);
        Assert::IsFalse(c2->IsNull);

        DbInt::Ptr c3 = row->GetValue<DbInt>(STR("region"));
        Assert::AreEqual(1, c3->Value);
        Assert::IsFalse(c3->IsNull);        
    }
};

TESTCLASS(VectorTableTests)
{
    TESTMETHOD(TestEnumerateVectorTable)
    {
        MapDataRow::Ptr mapRow1 = make_shared<MapDataRow>();

        mapRow1->Values.insert(pair<String, MapDataRow::ValueType>(STR("id"), make_shared<DbInt>(1)));
        mapRow1->Values.insert(pair<String, MapDataRow::ValueType>(STR("data"), make_shared<DbString>(STR("test data 1"))));
        mapRow1->Values.insert(pair<String, MapDataRow::ValueType>(STR("region"), make_shared<DbInt>(10)));

        MapDataRow::Ptr mapRow2 = make_shared<MapDataRow>();

        mapRow2->Values.insert(pair<String, MapDataRow::ValueType>(STR("id"), make_shared<DbInt>(2)));
        mapRow2->Values.insert(pair<String, MapDataRow::ValueType>(STR("data"), make_shared<DbString>(STR("test data 2"))));
        mapRow2->Values.insert(pair<String, MapDataRow::ValueType>(STR("region"), make_shared<DbInt>(20)));

        VectorTable table;
        table.Data.push_back(mapRow1);
        table.Data.push_back(mapRow2);

        IDataReader::Ptr reader = table.OpenScan();

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