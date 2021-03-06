#include <iostream>
using namespace std;

#include "Framework\UnitTest.h"
using namespace Hush;
using namespace Hush::UnitTest;

#include "..\Engine\QueryProcessor\Tokenizer.h"
using namespace HushDB;


TESTCLASS(TokenizerTests)
{
    TESTMETHOD(TestSelectStar)        
    {
        String query(T("select * from t1"));
        vector<SqlToken> tokens = Tokenizer::Parse(query);

        Assert::AreEqual<int>(4, tokens.size());
        Assert::AreEqual(SqlTokenType::Select, tokens[0].TokenType);
        Assert::AreEqual(SqlTokenType::Star, tokens[1].TokenType);
        Assert::AreEqual(SqlTokenType::From, tokens[2].TokenType);
        Assert::AreEqual(SqlTokenType::Identifier, tokens[3].TokenType);
        Assert::AreEqual(String(T("t1")), tokens[3].Value, T("Identifier value not equal"));
    }

    TESTMETHOD(TestSelectColumns)
    {
        String query(T("select c1, c2 from t1"));
        vector<SqlToken> tokens = Tokenizer::Parse(query);

        Assert::AreEqual<int>(6, tokens.size());
        Assert::AreEqual(SqlTokenType::Select, tokens[0].TokenType);
        Assert::AreEqual(SqlTokenType::Identifier, tokens[1].TokenType);
        Assert::AreEqual(String(T("c1")), tokens[1].Value, T("Identifier value not equal"));
        Assert::AreEqual(SqlTokenType::Comma, tokens[2].TokenType);
        Assert::AreEqual(SqlTokenType::Identifier, tokens[3].TokenType);
        Assert::AreEqual(String(T("c2")), tokens[3].Value, T("Identifier value not equal"));
        Assert::AreEqual(SqlTokenType::From, tokens[4].TokenType);
        Assert::AreEqual(SqlTokenType::Identifier, tokens[5].TokenType);
        Assert::AreEqual(String(T("t1")), tokens[5].Value, T("Identifier value not equal"));
    }

    TESTMETHOD(TestWhere)
    {
        String query(T("select * from $objects where object_id=1"));
        vector<SqlToken> tokens = Tokenizer::Parse(query);

        Assert::AreEqual<int>(8, tokens.size());
        
        auto token_iter = tokens.begin();
        Assert::AreEqual(SqlToken{ SqlTokenType::Select, T("select") }, *token_iter++);
        Assert::AreEqual(SqlToken{ SqlTokenType::Star, T("*") }, *token_iter++);
        Assert::AreEqual(SqlToken{ SqlTokenType::From, T("from") }, *token_iter++);
        Assert::AreEqual(SqlToken{ SqlTokenType::Identifier, T("$objects") }, *token_iter++);
        Assert::AreEqual(SqlToken{ SqlTokenType::Where, T("where") }, *token_iter++);
        Assert::AreEqual(SqlToken{ SqlTokenType::Identifier, T("object_id") }, *token_iter++);
        Assert::AreEqual(SqlToken{ SqlTokenType::Equal, T("=") }, *token_iter++);
        Assert::AreEqual(SqlToken{ SqlTokenType::Integer, T("1") }, *token_iter++);        
    }

    TESTMETHOD(TestWherePredicateOperator)
    {
        String query(T("select * from $objects where object_id>=1"));
        vector<SqlToken> tokens = Tokenizer::Parse(query);

        Assert::AreEqual<int>(8, tokens.size());

        auto token_iter = tokens.begin();
        Assert::AreEqual(SqlToken{ SqlTokenType::Select, T("select") }, *token_iter++);
        Assert::AreEqual(SqlToken{ SqlTokenType::Star, T("*") }, *token_iter++);
        Assert::AreEqual(SqlToken{ SqlTokenType::From, T("from") }, *token_iter++);
        Assert::AreEqual(SqlToken{ SqlTokenType::Identifier, T("$objects") }, *token_iter++);
        Assert::AreEqual(SqlToken{ SqlTokenType::Where, T("where") }, *token_iter++);
        Assert::AreEqual(SqlToken{ SqlTokenType::Identifier, T("object_id") }, *token_iter++);
        Assert::AreEqual(SqlToken{ SqlTokenType::GreaterOrEqual, T(">=") }, *token_iter++);
        Assert::AreEqual(SqlToken{ SqlTokenType::Integer, T("1") }, *token_iter++);
    }

    TESTMETHOD(TestPredicateOperator)
    {        
        TestPredicateOperator(T(">"), SqlTokenType::GreaterThan);
        TestPredicateOperator(T(">="), SqlTokenType::GreaterOrEqual);
        TestPredicateOperator(T("="), SqlTokenType::Equal);
        TestPredicateOperator(T("<"), SqlTokenType::LessThan);
        TestPredicateOperator(T("<="), SqlTokenType::LessOrEqual);
        TestPredicateOperator(T("<>"), SqlTokenType::NotEqual);
    }

    void TestPredicateOperator(String op, SqlTokenType opType)
    {
        String query(STR("object_id") + op + STR("1"));
        vector<SqlToken> tokens = Tokenizer::Parse(query);

        Assert::AreEqual<int>(3, tokens.size());

        auto token_iter = tokens.begin();
        Assert::AreEqual(SqlToken{ SqlTokenType::Identifier, T("object_id") }, *token_iter++);
        Assert::AreEqual(SqlToken{ opType, op }, *token_iter++);
        Assert::AreEqual(SqlToken{ SqlTokenType::Integer, T("1") }, *token_iter++);
    }
};
