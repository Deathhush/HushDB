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

        Identifier,

        // Keywords
        Select,
        From,

        // Symbols
        Star,
        Comma
    };

    struct SqlToken
    {
        typedef vector<SqlToken> List;
        SqlTokenType TokenType;

        int Row;
        int Column;

        String Value;
    };

    struct TokenizerError
    {
        SqlToken Position;
        String Message;
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
        };

    private:
        
    };

}


#endif

