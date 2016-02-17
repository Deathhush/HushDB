#ifndef HUSH_PARSER_H
#define HUSH_PARSER_H

#include <vector>
using namespace std;

#include "Tokenizer.h"
using namespace Hush;

namespace HushDB
{
    class Parser
    {
    public:
        static bool ConsumeToken(SqlToken::List::iterator& it, SqlToken::List::iterator& end, SqlTokenType type);
        static StringPtr ConsumeIdentifier(SqlToken::List::iterator& it, SqlToken::List::iterator& end);


    };

    struct FromClause
    {
        typedef shared_ptr<FromClause> Ptr;
        static FromClause::Ptr Parse(SqlToken::List::iterator& it, SqlToken::List::iterator& end);

        StringPtr TableName;        
    };

    struct TargetElement
    {
        typedef shared_ptr<TargetElement> Ptr;        
        static TargetElement::Ptr Parse(SqlToken::List::iterator& it, SqlToken::List::iterator& end);

        bool isStar;
        StringPtr ColumnName;
    };

    struct TargetList
    {
        typedef shared_ptr<TargetList> Ptr;
        static TargetList::Ptr Parse(SqlToken::List::iterator& it, SqlToken::List::iterator& end);

        vector<TargetElement::Ptr> Elements;
    };

    struct SelectStmt
    {
        typedef shared_ptr<SelectStmt> Ptr;
        static SelectStmt::Ptr Parse(SqlToken::List::iterator& it, SqlToken::List::iterator& end);

        TargetList::Ptr TargetList;
        FromClause::Ptr FromClause;
    };
}

#endif