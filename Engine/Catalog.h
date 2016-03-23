#ifndef HUSH_CATALOG_H
#define HUSH_CATALOG_H

#include <vector>
#include <map>
using namespace std;

#include "..\Common\BasicType.h"
#include "StorageEngine\MemoryStorage.h"
#include "StorageEngine\DataRow.h"
#include "Schema.h"
using namespace Hush;

namespace HushDB
{
    enum class ObjectType
    {
        MemoryTable,
        SimpleHeap
    };

    struct IObjectDef
    {
        typedef shared_ptr<IObjectDef> Ptr;

        virtual Int32 ObjectId() = 0;
        virtual String ObjectName() = 0;
        virtual ObjectType Type() = 0;
    };

    struct TableDefDescType : public ITupleDesc
    {
        typedef shared_ptr<TableDefDescType> Ptr;

        virtual int GetOrdinal(const String& columnName) override
        {
            if (columnName == T("ObjectId"))
            {
                return 0;
            }

            if (columnName == T("ObjectName"))
            {
                return 1;
            }

            if (columnName == T("Type"))
            {
                return 2;
            }

            return -1;
        }

        virtual IColumnDesc::Ptr GetColumnDesc(const Int32& columnIndex) override
        {
            switch (columnIndex)
            {
            case 0:
                return TableDefDescType::ObjectIdDesc;
            case 1:
                return TableDefDescType::ObjectNameDesc;
            case 2:
                return TableDefDescType::TypeDesc;
            }

            return nullptr;
        }

    private:
        static ColumnDesc::Ptr ObjectIdDesc;
        static ColumnDesc::Ptr ObjectNameDesc;
        static ColumnDesc::Ptr TypeDesc;
    };

    extern TableDefDescType::Ptr TableDefDesc;

    struct TableDef : public IObjectDef, public DataRow
    {
        TableDef(Byte* data)
            :DataRow(TableDefDesc, data)
        {}

        virtual Int32 ObjectId() override
        {
            IDataRow* row = this;
            return row->GetValue<DbInt>(T("ObjectId"))->Value;
        }

        virtual String ObjectName() override
        {
            IDataRow* row = this;
            return row->GetValue<DbString>(T("ObjectName"))->Value;
        }

        virtual ObjectType Type() override
        {
            IDataRow* row = this;
            return (ObjectType)row->GetValue<DbInt>(T("Type"))->Value;
        }

        static vector<DbValue::Ptr> CreateTableDefRow(Int32 objectId, String objectName, ObjectType type)
        {
            vector<DbValue::Ptr> result;
            result.push_back(make_shared<DbInt>(objectId));
            result.push_back(make_shared<DbString>(objectName));
            result.push_back(make_shared<DbInt>((Int32)type));
            return result;
        }
    };

    struct MemoryTableDef : public IObjectDef
    {
        typedef shared_ptr<MemoryTableDef> Ptr;

        MemoryTableDef()
        {
            this->Schema = make_shared<TupleDesc>();
        }

        virtual Int32 ObjectId() override
        {
            return this->Id;
        }
        virtual ObjectType Type() override
        {
            return ObjectType::MemoryTable;
        }

        virtual String ObjectName() override
        {
            return this->Name;
        }

        Int32 Id;
        String Name;
        TupleDesc::Ptr Schema;
        MemoryTable::Ptr Table;
    };

    class Catalog
    {
    public:
        typedef shared_ptr<Catalog> Ptr;

        void AddTable(const MemoryTableDef& table)
        {
            MemoryTableDef::Ptr tablePtr = make_shared<MemoryTableDef>(table);
            tablePtr->Id = this->tableList.size();
            this->tableMap[table.Name] = tablePtr;
            this->tableList.push_back(tablePtr);
        }

        IObjectDef::Ptr FindTable(const String& tableName)
        {
            auto it = this->tableMap.find(tableName);
            if (it != this->tableMap.end())
            {
                return it->second;
            }
            else
            {
                return nullptr;
            }
        }

        IObjectDef::Ptr FindTable(const Int32& tableId)
        {
            if (tableId < (Int32)this->tableList.size())
            {
                return this->tableList[tableId];
            }
            else
            {
                return nullptr;
            }
        }

        ITupleDesc::Ptr FindTableSchema(IObjectDef::Ptr tableDef)
        {
            if (tableDef->Type() == ObjectType::MemoryTable)
            {
                return (dynamic_pointer_cast<MemoryTableDef>(tableDef))->Schema;
            }
        }
    
    private:
        map<String, MemoryTableDef::Ptr> tableMap;
        vector<MemoryTableDef::Ptr> tableList;
    };

}

#endif