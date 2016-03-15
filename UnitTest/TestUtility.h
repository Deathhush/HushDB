#pragma once

#include <iostream>
#include <map>
using namespace std;

#include "Framework\UnitTest.h"
using namespace Hush;
using namespace Hush::UnitTest;

#include "..\Engine\Client\Client.h"
#include "..\Engine\StorageEngine\MemoryStorage.h"
#include "..\Engine\Client\SqlCommand.h"
using namespace HushDB;

struct TestUtility
{
    static MemoryTable::Ptr CreateMemoryTable()
    {
        MemoryDataRow::Ptr mapRow1 = make_shared<MemoryDataRow>();

        mapRow1->Values.insert(pair<String, MemoryDataRow::ValueType>(STR("id"), make_shared<DbInt>(1)));
        mapRow1->Values.insert(pair<String, MemoryDataRow::ValueType>(STR("data"), make_shared<DbString>(STR("test data 1"))));
        mapRow1->Values.insert(pair<String, MemoryDataRow::ValueType>(STR("region"), make_shared<DbInt>(10)));

        MemoryDataRow::Ptr mapRow2 = make_shared<MemoryDataRow>();

        mapRow2->Values.insert(pair<String, MemoryDataRow::ValueType>(STR("id"), make_shared<DbInt>(2)));
        mapRow2->Values.insert(pair<String, MemoryDataRow::ValueType>(STR("data"), make_shared<DbString>(STR("test data 2"))));
        mapRow2->Values.insert(pair<String, MemoryDataRow::ValueType>(STR("region"), make_shared<DbInt>(20)));

        MemoryTable::Ptr table = make_shared<MemoryTable>();
        table->Rows.push_back(mapRow1);
        table->Rows.push_back(mapRow2);

        return table;
    }
};
