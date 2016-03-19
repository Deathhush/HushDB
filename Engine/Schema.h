#ifndef HUSH_SCHEMA_H
#define HUSH_SCHEMA_H

#include <vector>
#include <map>
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
        typedef shared_ptr<TupleDesc> Ptr;
        void AddColumn(const ColumnDesc& column)
        {
            ColumnDesc::Ptr columnPtr = make_shared<ColumnDesc>(column);
            this->ColumnList.push_back(columnPtr);
            this->columnMap[column.Name] = this->ColumnList.size() - 1;
        }

        void AddColumn(const String& columnName, const SqlType& columnType)
        {
            this->AddColumn(ColumnDesc{ columnName, columnType });
        }

        bool ContainsColumn(const String& columnName)
        {
            return this->columnMap.find(columnName) != this->columnMap.end();
        }

        int GetOrdinal(const String& columnName)
        {
            return this->columnMap.at(columnName);
        }

    public:
        vector<ColumnDesc::Ptr> ColumnList;

    private:        
        map<String, int> columnMap;
    };
}

#endif
