#ifndef HUSH_EXECUTIONPLAN_H
#define HUSH_EXECUTIONPLAN_H

#include <vector>
using namespace std;

#include "..\..\Common\BasicType.h"
#include "..\Client\Client.h"
#include "..\StorageEngine\MemoryStorage.h"
#include "..\Catalog.h"
using namespace Hush;

namespace HushDB
{
    class IExecutionPlanNode
    {
    public:
        typedef shared_ptr<IExecutionPlanNode> Ptr;

    public:
        virtual IDataReader::Ptr Execute() = 0;
        virtual ~IExecutionPlanNode() {}
    };

    class MemoryTableScan : public IExecutionPlanNode
    {
    public:
        typedef shared_ptr<MemoryTableScan> Ptr;

    public:
        MemoryTableScan(MemoryTableDef::Ptr table)
            :MemoryTableDef(table)
        {

        }
        virtual IDataReader::Ptr Execute() override
        {
            return this->MemoryTableDef->Table->OpenScan();
        }

        MemoryTableDef::Ptr MemoryTableDef;
    };

    struct DataRowEnumerator : public IDataReader
    {
        typedef shared_ptr<DataRowEnumerator> Ptr;

        DataRowEnumerator(IEnumerator<RowPtr>::Ptr innerEnumerator, ITupleDesc::Ptr schema)
            :InnerEnumerator(innerEnumerator), Schema(schema)
        {}

        virtual bool MoveNext() override
        {
            return this->InnerEnumerator->MoveNext();
        }

        virtual IDataRow::Ptr Current() override
        {
            return make_shared<DataRow>(Schema, this->InnerEnumerator->Current().data);
        }

        IEnumerator<RowPtr>::Ptr InnerEnumerator;
        ITupleDesc::Ptr Schema;
    };


    class SimpleHeapScan : public IExecutionPlanNode
    {
    public:
        typedef shared_ptr<SimpleHeapScan> Ptr;

    public:
        SimpleHeapScan(BufferManager* bufferManager, PageId headerPageId, ITupleDesc::Ptr schema)
            :schema(schema)
        {
            this->heap = make_shared<SimpleHeap>(bufferManager, headerPageId);
        }
        virtual IDataReader::Ptr Execute() override
        {
            return make_shared<DataRowEnumerator>(this->heap->GetEnumerator(), this->schema);
        }

        SimpleHeap::Ptr heap;
        ITupleDesc::Ptr schema;
    };
}

#endif