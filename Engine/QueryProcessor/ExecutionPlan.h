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
        MemoryTableScan(TableDef::Ptr table)
            :TableDef(table)
        {

        }
        virtual IDataReader::Ptr Execute() override
        {
            return this->TableDef->Table->OpenScan();
        }

        TableDef::Ptr TableDef;
    };
}

#endif