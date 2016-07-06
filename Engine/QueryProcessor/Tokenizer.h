#ifndef HUSH_TOKENIZER_H
#define HUSH_TOKENIZER_H

#include <vector>

#include "..\..\Common\BasicType.h"
using namespace Hush;

namespace HushDB
{
    enum class SqlTokenType
    {
        // For reporting error
        Unknown,

        // Types
        Integer,
        Identifier,

        // Keywords
        Select,
        From,
        Where,

        // Symbols
        Star,
        Comma,        
        GreaterThan,
        LessThan,
        Equal,
        GreaterOrEqual,
        LessOrEqual,
        NotEqual,
    };

    struct SqlToken
    {
        typedef vector<SqlToken> List;
        SqlTokenType TokenType;
        String Value;

        int Row;
        int Column;

        bool operator==(const SqlToken& token) const
        {
            return (this->TokenType == token.TokenType
                && this->Value == token.Value);
        }

        bool operator!=(const SqlToken& token) const
        {
            return !this->operator==(token);
        }
    };

    struct TokenizerException : public Exception
    {
        TokenizerException(const SqlToken& position, const String& message = String())
            : Exception(message)
        {
            this->Position = position;
        }
        SqlToken Position;        
    };

    class Tokenizer
    {
    public:
        static vector<SqlToken> Parse(const String& query);

    public:
        enum class State
        {
            Begin,
            Identifier,
            InInteger
        };

    private:
        
    };

}


#endif

