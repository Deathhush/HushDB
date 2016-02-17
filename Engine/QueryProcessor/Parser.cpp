#include "Parser.h"

namespace HushDB
{    
    bool Parser::ConsumeToken(SqlToken::List::iterator & it, SqlToken::List::iterator & end, SqlTokenType type)
    {
        if (it != end && it->TokenType == type)
        {
            it++;
            return true;
        }

        return false;    
    }

    StringPtr Parser::ConsumeIdentifier(SqlToken::List::iterator & it, SqlToken::List::iterator & end)
    {
        if (it->TokenType == SqlTokenType::Identifier)
        {
            StringPtr result = make_shared<String>(it->Value);
            it++;
            return result;
        }

        return nullptr;
    }

    FromClause::Ptr HushDB::FromClause::Parse(SqlToken::List::iterator& it, SqlToken::List::iterator& end)
    {
        SqlToken::List::iterator current = it;

        if (Parser::ConsumeToken(current, end, SqlTokenType::From))
        {
            StringPtr tableName = Parser::ConsumeIdentifier(current, end);

            if (tableName != nullptr)
            {
                FromClause::Ptr result = make_shared<FromClause>();
                result->TableName = tableName;

                it = current;

                return result;
            }            
        }

        return nullptr;
    }

    TargetElement::Ptr TargetElement::Parse(SqlToken::List::iterator & it, SqlToken::List::iterator & end)
    {
        SqlToken::List::iterator current = it;
        
        if (Parser::ConsumeToken(current, end, SqlTokenType::Star))
        {            
            TargetElement::Ptr result = make_shared<TargetElement>();
            result->isStar = true;

            it = current;
            return result;
        }

        StringPtr columnName = Parser::ConsumeIdentifier(current, end);
        if (columnName != nullptr)
        {
            TargetElement::Ptr result = make_shared<TargetElement>();
            result->isStar = false;
            result->ColumnName = columnName;

            it = current;
            return result;
        }

        return nullptr;
    }
    TargetList::Ptr TargetList::Parse(SqlToken::List::iterator & it, SqlToken::List::iterator & end)
    {
        SqlToken::List::iterator current = it;

        TargetElement::Ptr element = TargetElement::Parse(current, end);
        TargetList::Ptr result = make_shared<TargetList>();

        while (element != nullptr)
        {
            it = current;
            result->Elements.push_back(element);
            element = nullptr;

            if (Parser::ConsumeToken(current, end, SqlTokenType::Comma))
            {
                element = TargetElement::Parse(current, end);
            }
        }

        if (result->Elements.size() > 0)
        {
            return result;
        }

        return nullptr;
    }

    SelectStmt::Ptr SelectStmt::Parse(SqlToken::List::iterator & it, SqlToken::List::iterator & end)
    {
        SqlToken::List::iterator current = it;        

        if (Parser::ConsumeToken(current, end, SqlTokenType::Select))
        {
            SelectStmt::Ptr result = make_shared<SelectStmt>();

            result->TargetList = TargetList::Parse(current, end);
            if (result->TargetList != nullptr)
            {
                result->FromClause = FromClause::Parse(current, end);
                
                if (result->FromClause != nullptr)
                {
                    it = current;
                    return result;
                }
            }
        }

        return nullptr;
    }
}
