#ifndef HUSH_CATALOG_H
#define HUSH_CATALOG_H

#include <vector>
#include <map>
using namespace std;

#include "..\Common\BasicType.h"
#include "StorageEngine\MemoryStorage.h"
#include "Schema.h"
using namespace Hush;

namespace HushDB
{
    struct TableDef
    {
        typedef shared_ptr<TableDef> Ptr;

        Int32 Id;
        String Name;
        TupleDesc Schema;
        MemoryTable::Ptr Table;
    };

    class Catalog
    {
    public:
        typedef shared_ptr<Catalog> Ptr;

        void AddTable(const TableDef& table)
        {
            TableDef::Ptr tablePtr = make_shared<TableDef>(table);
            tablePtr->Id = this->tableList.size();
            this->tableMap[table.Name] = tablePtr;
            this->tableList.push_back(tablePtr);
        }

        TableDef::Ptr FindTable(const String& tableName)
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

        TableDef::Ptr FindTable(const Int32& tableId)
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
    
    private:
        map<String, TableDef::Ptr> tableMap;
        vector<TableDef::Ptr> tableList;
    };

}

#endif