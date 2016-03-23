#ifndef HUSH_OPTIMIZER_H
#define HUSH_OPTIMIZER_H

#include <vector>
using namespace std;

#include "..\..\Common\BasicType.h"
#include "ExecutionPlan.h"
#include "LogicalPlan.h"

using namespace Hush;

namespace HushDB
{
    class Optimizer
    {
    public:
        Optimizer(Catalog::Ptr catalog)
            :catalog(catalog)
        {}
        IExecutionPlanNode::Ptr Optimize(LogicalPlan::Ptr plan)
        {
            // Only handle a single join today.
            if (plan->Scans.size() > 0)
            {
                LogicalScan& scan = plan->Scans[0];
                IObjectDef::Ptr tableDef = catalog->FindTable(scan.ObjectId);

                if (tableDef->Type() == ObjectType::MemoryTable)
                {
                    return make_shared<MemoryTableScan>(dynamic_pointer_cast<MemoryTableDef>(tableDef));
                }
            }

            throw Exception(T("Invalid operation"));
        }
    private:
        Catalog::Ptr catalog;
    };
}

#endif