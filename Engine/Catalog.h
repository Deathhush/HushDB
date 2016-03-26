#ifndef HUSH_CATALOG_H
#define HUSH_CATALOG_H

#include <vector>
#include <memory>
#include <map>
using namespace std;

#include "..\Common\BasicType.h"
#include "StorageEngine\MemoryStorage.h"
#include "StorageEngine\DataRow.h"
#include "Schema.h"
#include "StorageEngine\Page.h"
#include "StorageEngine\DataFile.h"
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

    struct TableDef : public IObjectDef
    {
        typedef shared_ptr<TableDef> Ptr;

        TableDef()
        {            
        }

        virtual Int32 ObjectId() override
        {
            return this->Id;
        }

        virtual String ObjectName() override
        {
            return this->TableName;
        }

        virtual ObjectType Type() override
        {
            return this->ObjectType;
        }

        PageId GetHeaderPageId()
        {
            return this->HeaderPageId;
        }

        Int32 Id;
        String TableName;
        ObjectType ObjectType;
        PageId HeaderPageId;
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

    // The schema for $tables table
    struct TableDefDesc : public ITupleDesc
    {
        typedef shared_ptr<TableDefDesc> Ptr;

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

            if (columnName == T("ObjectType"))
            {
                return 2;
            }

            if (columnName == T("HeaderPageId"))
            {
                return 3;
            }

            return -1;
        }

        virtual IColumnDef::Ptr GetColumnDesc(const Int32& columnIndex) override
        {
            switch (columnIndex)
            {
            case 0:
                return TableDefDesc::ObjectIdDef;
            case 1:
                return TableDefDesc::ObjectNameDef;
            case 2:
                return TableDefDesc::ObjectTypeDef;
            case 3:
                return TableDefDesc::HeaderPageIdDef;
            }

            return nullptr;
        }

    private:
        static ColumnDef::Ptr ObjectIdDef;
        static ColumnDef::Ptr ObjectNameDef;
        static ColumnDef::Ptr ObjectTypeDef;
        static ColumnDef::Ptr HeaderPageIdDef;
    };

    extern TableDefDesc::Ptr TableDefSchema;

    struct TableDefAccessor : public IObjectDef, public DataRow
    {
        typedef shared_ptr<TableDefAccessor> Ptr;

        TableDefAccessor(Byte* data)
            :DataRow(TableDefSchema, data)
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
            return (ObjectType)row->GetValue<DbInt>(T("ObjectType"))->Value;
        }

        PageId GetHeaderPageId()
        {
            IDataRow* row = this;
            return (PageId)row->GetValue<DbInt>(T("HeaderPageId"))->Value;
        }

        TableDef::Ptr ToTableDef()
        {
            TableDef::Ptr result = make_shared<TableDef>();
            result->Id = this->ObjectId();
            result->TableName = this->ObjectName();
            result->ObjectType = this->Type();
            result->HeaderPageId = this->GetHeaderPageId();

            return result;
        }

        static vector<DbValue::Ptr> CreateTableDefRow(Int32 objectId, String objectName, ObjectType type, PageId headerPageId)
        {
            vector<DbValue::Ptr> result;
            result.push_back(make_shared<DbInt>(objectId));
            result.push_back(make_shared<DbString>(objectName));
            result.push_back(make_shared<DbInt>((Int32)type));
            result.push_back(make_shared<DbInt>(headerPageId));
            return result;
        }
    };

    // The schema for $columns table
    struct ColumnDefDesc : public ITupleDesc
    {
        typedef shared_ptr<ColumnDefDesc> Ptr;

        virtual int GetOrdinal(const String& columnName) override
        {
            if (columnName == T("ObjectId"))
            {
                return 0;
            }

            if (columnName == T("ColumnName"))
            {
                return 1;
            }

            if (columnName == T("ColumnId"))
            {
                return 2;
            }

            if (columnName == T("ColumnType"))
            {
                return 3;
            }

            return -1;
        }

        virtual IColumnDef::Ptr GetColumnDesc(const Int32& columnIndex) override
        {
            switch (columnIndex)
            {
            case 0:
                return ColumnDefDesc::ObjectIdDef;
            case 1:
                return ColumnDefDesc::ColumnNameDef;
            case 2:
                return ColumnDefDesc::ColumnIdDef;
            case 3:
                return ColumnDefDesc::ColumnTypeDef;
            }

            return nullptr;
        }

    private:
        static ColumnDef::Ptr ObjectIdDef;
        static ColumnDef::Ptr ColumnNameDef;
        static ColumnDef::Ptr ColumnIdDef;
        static ColumnDef::Ptr ColumnTypeDef;
    };

    extern ColumnDefDesc::Ptr ColumnDefSchema;

    struct ColumnDefAccessor : public IColumnDef, public DataRow
    {
        typedef shared_ptr<ColumnDefAccessor> Ptr;

        ColumnDefAccessor(Byte* data)
            :DataRow(ColumnDefSchema, data)
        {}

        Int32 ObjectId()
        {
            IDataRow* row = this;
            return row->GetValue<DbInt>(T("ObjectId"))->Value;
        }

        virtual String ColumnName() override
        {
            IDataRow* row = this;
            return row->GetValue<DbString>(T("ColumnName"))->Value;
        }

        virtual SqlType ColumnType() override
        {
            IDataRow* row = this;
            return (SqlType)row->GetValue<DbInt>(T("ColumnType"))->Value;
        }

        Int32 ColumnId()
        {
            IDataRow* row = this;
            return row->GetValue<DbInt>(T("ColumnId"))->Value;
        }

        static vector<DbValue::Ptr> CreateColumnDefRow(Int32 objectId, String ColumnName, Int32 ColumnId, SqlType ColumnType)
        {
            vector<DbValue::Ptr> result;
            result.push_back(make_shared<DbInt>(objectId));
            result.push_back(make_shared<DbString>(ColumnName));
            result.push_back(make_shared<DbInt>(ColumnId));
            result.push_back(make_shared<DbInt>((Int32)ColumnType));
            return result;
        }

        ColumnDef::Ptr ToColumnDef()
        {
            return make_shared<ColumnDef>(this->ColumnName(), this->ColumnType());
        }
    };

    class Catalog
    {
    public:
        typedef shared_ptr<Catalog> Ptr;

        Catalog(BufferManager* bufferManager = nullptr)
            :bufferManager(bufferManager)
        {}        

        void AddTable(const MemoryTableDef& table)
        {
            MemoryTableDef::Ptr tablePtr = make_shared<MemoryTableDef>(table);
            tablePtr->Id = this->tableList.size();
            this->tableMap[table.Name] = tablePtr;
            this->tableList.push_back(tablePtr);
        }

        IObjectDef::Ptr FindTable(const String& tableName)
        {
            if (this->bufferManager != nullptr)
            {
                // Try to find from $objects first            
                DataFileHeaderPage* headerPage = bufferManager->GetPageAs<DataFileHeaderPage>(0);

                SimpleHeap heap(bufferManager, headerPage->ObjectDefPageId);
                IEnumerator<RowPtr>::Ptr enumerator = heap.GetEnumerator();

                TableDef::Ptr result = nullptr;
                while (enumerator->MoveNext())
                {
                    TableDefAccessor::Ptr accessor = make_shared<TableDefAccessor>(enumerator->Current().data);
                    if (accessor->ObjectName() == tableName)
                    {
                        result = accessor->ToTableDef();
                        break;
                    }
                }

                bufferManager->ReleasePage(headerPage->GetPageId());
                if (result != nullptr)
                {
                    return result;
                }
            }

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
            // TODO: Rewrite this using filter to eliminate duplication
            if (this->bufferManager != nullptr)
            {
                // Try to find from $objects first            
                DataFileHeaderPage* headerPage = bufferManager->GetPageAs<DataFileHeaderPage>(0);

                SimpleHeap heap(bufferManager, headerPage->ObjectDefPageId);
                IEnumerator<RowPtr>::Ptr enumerator = heap.GetEnumerator();

                TableDef::Ptr result = nullptr;
                while (enumerator->MoveNext())
                {
                    TableDefAccessor::Ptr accessor = make_shared<TableDefAccessor>(enumerator->Current().data);
                    if (accessor->ObjectId() == tableId)
                    {
                        result = accessor->ToTableDef();
                        break;
                    }
                }

                bufferManager->ReleasePage(headerPage->GetPageId());
                if (result != nullptr)
                {
                    return result;
                }
            }

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
            // Try to find from $columns first

            if (this->bufferManager != nullptr)
            {
                DataFileHeaderPage* headerPage = bufferManager->GetPageAs<DataFileHeaderPage>(0);

                SimpleHeap heap(bufferManager, headerPage->ColumnDefPageId);
                IEnumerator<RowPtr>::Ptr enumerator = heap.GetEnumerator();

                TupleDesc::Ptr result = make_shared<TupleDesc>();
                while (enumerator->MoveNext())
                {                    
                    ColumnDefAccessor::Ptr accessor = make_shared<ColumnDefAccessor>(enumerator->Current().data);
                    if (accessor->ObjectId() == tableDef->ObjectId())
                    {
                        // Assumes that the ColumnDef appears in ColumnIndex order
                        result->AddColumn(*accessor->ToColumnDef());
                    }
                }

                bufferManager->ReleasePage(headerPage->GetPageId());
                if (result->ColumnList.size() > 0)
                {
                    return result;
                }
            }

            if (tableDef->Type() == ObjectType::MemoryTable)
            {
                return (dynamic_pointer_cast<MemoryTableDef>(tableDef))->Schema;
            }
        }
        BufferManager* GetBufferManager()
        {
            return this->bufferManager;
        }
    private:
        map<String, MemoryTableDef::Ptr> tableMap;
        vector<MemoryTableDef::Ptr> tableList;
        BufferManager* bufferManager;
    };

}

#endif