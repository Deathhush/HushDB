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
        IExecutionPlanNode::Ptr Optimize(LogicalPlan::Ptr plan)
        {
            // Only handle a single join today.
            if (plan->Scans.size() > 0)
            {
                return make_shared<MemoryTableScan>(plan->Scans[0].TableDef);
            }

            throw Exception(T("Invalid operation"));
        }
    };
}

#endif