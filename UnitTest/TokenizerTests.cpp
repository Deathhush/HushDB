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
};
