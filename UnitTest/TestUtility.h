#pragma once

#include <iostream>
#include <vector>
#include <map>
using namespace std;

#include "Framework\UnitTest.h"
using namespace Hush;
using namespace Hush::UnitTest;

#include "..\Engine\Client\Client.h"
#include "..\Engine\StorageEngine\MemoryStorage.h"
#include "..\Engine\Client\SqlCommand.h"
#include "..\Engine\StorageEngine\Page.h"
using namespace HushDB;

template <typename T>
inline void AssertRowPtr(const T& value, const RowPtr& rowPtr, const wstring& message)
{
    Int32 length = rowPtr.length;
    T testvalue = *((T*)(rowPtr.data));

    Assert::AreEqual<Int32>(sizeof(T), length, message + L"(Size)");
    Assert::AreEqual<T>(value, testvalue, message + L"(Data)");
}

struct TestUtility
{
    static MemoryTable::Ptr CreateMemoryTable()
    {
        MemoryDataRow::Ptr mapRow1 = make_shared<MemoryDataRow>();

        mapRow1->Values.push_back(make_shared<DbInt>(1));
        mapRow1->Values.push_back(make_shared<DbString>(STR("test data 1")));
        mapRow1->Values.push_back(make_shared<DbInt>(10));

        MemoryDataRow::Ptr mapRow2 = make_shared<MemoryDataRow>();
        mapRow2->Values.push_back(make_shared<DbInt>(2));
        mapRow2->Values.push_back(make_shared<DbString>(STR("test data 2")));
        mapRow2->Values.push_back(make_shared<DbInt>(20));

        MemoryTable::Ptr table = make_shared<MemoryTable>();
        table->Rows.push_back(mapRow1);
        table->Rows.push_back(mapRow2);

        table->Schema = make_shared<TupleDesc>();
        table->Schema->AddColumn(T("id"), SqlType::Int);
        table->Schema->AddColumn(T("data"), SqlType::String);
        table->Schema->AddColumn(T("region"), SqlType::Int);

        return table;
    }
};
