#include <iostream>
#include <vector>
using namespace std;

#include "Framework\UnitTest.h"
using namespace Hush;
using namespace Hush::UnitTest;

#include "..\Engine\QueryProcessor\Tokenizer.h"
#include "..\Engine\StorageEngine\Page.h"
#include "..\Engine\StorageEngine\DataRow.h"
using namespace HushDB;

TESTCLASS(DataRowTests)
{
    TESTMETHOD(TestInsertDataRow)
    {
        DataPage dataPage(PageId{ 1 });    

        TupleDesc::Ptr schema = make_shared<TupleDesc>();
        schema->AddColumn(T("id"), SqlType::Int);
        schema->AddColumn(T("data"), SqlType::String);
        schema->AddColumn(T("region"), SqlType::Int);

        vector<DbValue::Ptr> fields;
        fields.push_back(make_shared<DbInt>(1)); // size 4
        fields.push_back(make_shared<DbString>(STR("test data 1"))); // size 22
        fields.push_back(make_shared<DbInt>(10)); // size 4

        RowPtr rowPtr = dataPage.InsertDataRow(fields);

        IDataRow::Ptr r = make_shared<DataRow>(schema, rowPtr.data);

        DbInt::Ptr c1 = r->GetValue<DbInt>(STR("id"));
        DbString::Ptr c2 = r->GetValue<DbString>(STR("data"));
        DbInt::Ptr c3 = r->GetValue<DbInt>(STR("region"));
        
        Assert::AreEqual(1, c1->Value);
        Assert::IsFalse(c1->IsNull);
        
        Assert::AreEqual(STR("test data 1"), c2->Value);
        Assert::IsFalse(c2->IsNull);
        
        Assert::AreEqual(10, c3->Value);
        Assert::IsFalse(c3->IsNull);
    }

    TESTMETHOD(TestDataRowGetValueWithColumnIndex)
    {
        DataPage dataPage(PageId{ 1 });

        TupleDesc::Ptr schema = make_shared<TupleDesc>();
        schema->AddColumn(T("id"), SqlType::Int);
        schema->AddColumn(T("data"), SqlType::String);
        schema->AddColumn(T("region"), SqlType::Int);

        vector<DbValue::Ptr> fields;
        fields.push_back(make_shared<DbInt>(1)); // size 4
        fields.push_back(make_shared<DbString>(STR("test data 1"))); // size 22
        fields.push_back(make_shared<DbInt>(10)); // size 4

        RowPtr rowPtr = dataPage.InsertDataRow(fields);

        IDataRow::Ptr r = make_shared<DataRow>(schema, rowPtr.data);

        DbInt::Ptr c1 = r->GetValue<DbInt>(0);
        DbString::Ptr c2 = r->GetValue<DbString>(1);
        DbInt::Ptr c3 = r->GetValue<DbInt>(2);

        Assert::AreEqual(1, c1->Value);
        Assert::IsFalse(c1->IsNull);

        Assert::AreEqual(STR("test data 1"), c2->Value);
        Assert::IsFalse(c2->IsNull);

        Assert::AreEqual(10, c3->Value);
        Assert::IsFalse(c3->IsNull);
    }

    TESTMETHOD(TestNullValue)
    {
        DataPage dataPage(PageId{ 1 });

        TupleDesc::Ptr schema = make_shared<TupleDesc>();
        schema->AddColumn(T("id"), SqlType::Int);
        schema->AddColumn(T("data"), SqlType::String);
        schema->AddColumn(T("region"), SqlType::Int);

        vector<DbValue::Ptr> fields;
        fields.push_back(make_shared<DbInt>(1)); // size 4
        fields.push_back(make_shared<DbString>()); // size 22
        fields.push_back(make_shared<DbInt>()); // size 4

        RowPtr rowPtr = dataPage.InsertDataRow(fields);

        IDataRow::Ptr r = make_shared<DataRow>(schema, rowPtr.data);

        DbInt::Ptr c1 = r->GetValue<DbInt>(STR("id"));
        DbString::Ptr c2 = r->GetValue<DbString>(STR("data"));
        DbInt::Ptr c3 = r->GetValue<DbInt>(STR("region"));

        Assert::AreEqual(1, c1->Value);
        Assert::IsFalse(c1->IsNull);
        
        Assert::IsTrue(c2->IsNull);        
        Assert::IsTrue(c3->IsNull);
    }

};
