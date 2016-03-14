#ifndef HUSH_SCHEMA_H
#define HUSH_SCHEMA_H

#include <vector>
using namespace std;

#include "..\Common\BasicType.h"
using namespace Hush;

namespace HushDB
{
    enum class SqlType
    {
        Int,
        Float,
        String
    };


    struct ColumnDesc
    {
        typedef shared_ptr<ColumnDesc> Ptr;
        String Name;
        SqlType Type;        
    };

    class TupleDesc
    {
    public:
        void AddColumn(const ColumnDesc& column)
        {
            ColumnDesc::Ptr columnPtr = make_shared<ColumnDesc>(column);
            this->columnList.push_back(columnPtr);
            this->columnMap[column.Name] = columnPtr;
        }

        void AddColumn(const String& columnName, const SqlType& columnType)
        {
            this->AddColumn(ColumnDesc{ columnName, columnType });
        }

        bool ContainsColumn(const String& columnName)
        {
            return this->columnMap.find(columnName) != this->columnMap.end();
        }

    private:
        vector<ColumnDesc::Ptr> columnList;
        map<String, ColumnDesc::Ptr> columnMap;
    };
}

#endif
