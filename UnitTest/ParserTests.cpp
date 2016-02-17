#include <iostream>
using namespace std;

#include "Framework\UnitTest.h"
using namespace Hush;
using namespace Hush::UnitTest;

#include "..\Engine\QueryProcessor\Tokenizer.h"
#include "..\Engine\QueryProcessor\Parser.h"
using namespace HushDB;

TESTCLASS(ParserTests)
{
    TESTMETHOD(TestFromClause)
    {
        String query(T("from t1"));
        SqlToken::List tokens = Tokenizer::Parse(query);

        auto current = tokens.begin();

        FromClause::Ptr result = FromClause::Parse(current, tokens.end());

        Assert::IsNotNullPtr(result);
        Assert::AreEqual(STR("t1"), *(result->TableName), T("Table name not equal."));

        Assert::AreEqual(tokens.end(), current);
    }

    TESTMETHOD(TestInvalidFromClause)
    {
        String query(T("from *"));
        SqlToken::List tokens = Tokenizer::Parse(query);
        auto current = tokens.begin();

        FromClause::Ptr result = FromClause::Parse(current, tokens.end());

        Assert::IsNullPtr(result);

        Assert::AreEqual(tokens.begin(), current);
        
    }

    TESTMETHOD(TestTargetElementStar)
    {
        String query(T("*"));
        SqlToken::List tokens = Tokenizer::Parse(query);
        auto current = tokens.begin();

        TargetElement::Ptr result = TargetElement::Parse(current, tokens.end());

        Assert::IsNotNullPtr(result);
        Assert::IsTrue(result->isStar);

        Assert::AreEqual(tokens.end(), current);
    }

    TESTMETHOD(TestTargetElement)
    {
        String query(T("c1"));
        SqlToken::List tokens = Tokenizer::Parse(query);
        auto current = tokens.begin();

        TargetElement::Ptr result = TargetElement::Parse(current, tokens.end());

        Assert::IsNotNullPtr(result);
        Assert::AreEqual(STR("c1"), *(result->ColumnName));

        Assert::AreEqual(tokens.end(), current);
    }

    TESTMETHOD(TestInvalidTargetElement)
    {
        String query(T("from *"));
        SqlToken::List tokens = Tokenizer::Parse(query);
        auto current = tokens.begin();

        TargetElement::Ptr result = TargetElement::Parse(current, tokens.end());

        Assert::IsNullPtr(result);
        Assert::AreEqual(tokens.begin(), current);

    }

    TESTMETHOD(TestTargetList)
    {
        String query(T("c1, *, c2"));
        SqlToken::List tokens = Tokenizer::Parse(query);
        auto current = tokens.begin();

        TargetList::Ptr result = TargetList::Parse(current, tokens.end());

        Assert::IsNotNullPtr(result);
        Assert::AreEqual(tokens.end(), current);

        Assert::AreEqual<int>(3, result->Elements.size());

        Assert::AreEqual(STR("c1"), *(result->Elements[0]->ColumnName));
        Assert::IsTrue(result->Elements[1]->isStar);
        Assert::AreEqual(STR("c2"), *(result->Elements[2]->ColumnName));
    }

    TESTMETHOD(TestSelectStmt)
    {
        String query(T("select*, c1, c2 from t1"));
        SqlToken::List tokens = Tokenizer::Parse(query);
        auto current = tokens.begin();

        SelectStmt::Ptr result = SelectStmt::Parse(current, tokens.end());

        Assert::IsNotNullPtr(result);
        Assert::AreEqual(tokens.end(), current);

        auto targetList = result->TargetList;
        Assert::IsNotNullPtr(targetList);
        Assert::AreEqual<int>(3, targetList->Elements.size());
        Assert::IsTrue(targetList->Elements[0]->isStar);
        Assert::AreEqual(STR("c1"), *(targetList->Elements[1]->ColumnName));        
        Assert::AreEqual(STR("c2"), *(targetList->Elements[2]->ColumnName));

        auto fromClause = result->FromClause;
        Assert::IsNotNullPtr(fromClause);
        Assert::AreEqual(STR("t1"), *(fromClause->TableName));
    }

    TESTMETHOD(TestSelectStarStmt)
    {
        String query(T("select * from table1"));
        SqlToken::List tokens = Tokenizer::Parse(query);
        auto current = tokens.begin();

        SelectStmt::Ptr result = SelectStmt::Parse(current, tokens.end());

        Assert::IsNotNullPtr(result);
        Assert::AreEqual(tokens.end(), current);

        auto targetList = result->TargetList;
        Assert::IsNotNullPtr(targetList);
        Assert::AreEqual<int>(1, targetList->Elements.size());
        Assert::IsTrue(targetList->Elements[0]->isStar);

        auto fromClause = result->FromClause;
        Assert::IsNotNullPtr(fromClause);
        Assert::AreEqual(STR("table1"), *(fromClause->TableName));
    }
};
