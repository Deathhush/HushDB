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

    struct IColumnDef
    {
        typedef shared_ptr<IColumnDef> Ptr;

        virtual String ColumnName() = 0;
        virtual SqlType ColumnType() = 0;
    };

    struct ColumnDef : public IColumnDef
    {
        typedef shared_ptr<ColumnDef> Ptr;

        ColumnDef(const String& name, const SqlType& type)
            : Name(name), Type(type)
        {
        }

        virtual String ColumnName()
        {
            return this->Name;
        }

        virtual SqlType ColumnType()
        {
            return this->Type;
        }

        String Name;
        SqlType Type;        
    };

    struct ITupleDesc
    {
        typedef shared_ptr<ITupleDesc> Ptr;

        virtual int GetOrdinal(const String& columnName) = 0;
        virtual IColumnDef::Ptr GetColumnDesc(const Int32& columnIndex) = 0;

        bool ContainsColumn(const String& columnName)
        {
            return this->GetOrdinal(columnName) != -1;
        }
    };

    class TupleDesc : public ITupleDesc
    {
    public:
        typedef shared_ptr<TupleDesc> Ptr;
        void AddColumn(const ColumnDef& column)
        {
            ColumnDef::Ptr columnPtr = make_shared<ColumnDef>(column);
            this->ColumnList.push_back(columnPtr);
            this->columnMap[column.Name] = this->ColumnList.size() - 1;
        }

        void AddColumn(const String& columnName, const SqlType& columnType)
        {
            this->AddColumn(ColumnDef(columnName, columnType));
        }

        bool ContainsColumn(const String& columnName)
        {
            return this->columnMap.find(columnName) != this->columnMap.end();
        }

        virtual int GetOrdinal(const String& columnName) override
        {
            auto it = this->columnMap.find(columnName);
            if (it != this->columnMap.end())
            {
                return it->second;
            }
            else
            {
                return -1;
            }
        }

        virtual IColumnDef::Ptr GetColumnDesc(const Int32& columnIndex) override
        {
            return ColumnList[columnIndex];
        }

    public:
        vector<ColumnDef::Ptr> ColumnList;

    private:        
        map<String, int> columnMap;
    };
}

#endif
