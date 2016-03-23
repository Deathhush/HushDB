#include<cstdio>

#include <iostream>
using namespace std;

#include "Framework\UnitTest.h"
#include "TestUtility.h"
using namespace Hush;
using namespace Hush::UnitTest;

#include "..\Engine\StorageEngine\PageFile.h"
using namespace HushDB;

TESTCLASS(PageFileTests)
{
public:
    virtual void TestInitialize() override
    {
        remove("test.df");
    }

    TESTMETHOD(TestUpdateFile)
    {
        PageFile file(STR("test.df"));        
        file.Open();

        DataPage page(0);
        Int16 row1 = 1;
        Int32 row2 = 2;        

        RowId rowId1 = page.InsertRowValue(row1);
        RowId rowId2 = page.InsertRowValue(row2); 
                
        file.AllocatePage(&page);
        file.Flush();

        DataPage* pageFromFile = file.ReadPageAs<DataPage>(0);
        Assert::AreEqual(page, *pageFromFile);       
        
        Int64 row3 = 3;
        RowId rowId3 = page.InsertRowValue(row3);

        file.WritePage(&page);
        file.Flush();

        pageFromFile = file.ReadPageAs<DataPage>(0);
        Assert::AreEqual(page, *pageFromFile);
        
        file.Flush();
        file.Close();
    }

    TESTMETHOD(TestAppendFile)
    {
        PageFile file(STR("test.df"));
        file.Open();

        DataPage page(0);
        Int16 row1 = 1;
        Int32 row2 = 2;

        RowId rowId1 = page.InsertRowValue(row1);
        RowId rowId2 = page.InsertRowValue(row2);

        file.AllocatePage(&page);
        file.Flush();
        file.Close();

        file.Open();

        DataPage* pageFromFile = file.ReadPageAs<DataPage>(0);
        Assert::AreEqual(page, *pageFromFile);

        file.Flush();
        file.Close();
    }

    TESTMETHOD(TestFileSize)
    {
        PageFile file(STR("test.df"));
        file.Open();

        DataPage page(0);
        file.AllocatePage(&page);
        file.Flush();
        Assert::AreEqual(PageSize, file.Size());
        DataPage page1(1);
        file.AllocatePage(&page1);
        file.Flush();
        Assert::AreEqual(2*PageSize, file.Size());

        file.Close();        
    }

    TESTMETHOD(TestPageIdGeneration)
    {
        PageFile file(STR("test.df"));
        file.Open();

        DataPage page(0);
        file.AllocatePage(&page);
        file.Flush();
        Assert::AreEqual(PageSize, file.Size());
        Assert::AreEqual(0, page.GetPageId());
        file.Close();

        file.Open();
        DataPage page1(1);
        file.AllocatePage(&page1);
        file.Flush();
        Assert::AreEqual(1, page1.GetPageId());
        Assert::AreEqual(2 * PageSize, file.Size());

        DataPage* pageFromFile = file.ReadPageAs<DataPage>(0);
        Assert::AreEqual(page, *pageFromFile);

        pageFromFile = file.ReadPageAs<DataPage>(1);
        Assert::AreEqual(page1, *pageFromFile);

        file.Close();
    }
};