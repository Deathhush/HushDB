#ifndef HUSH_SQLCOMMAND_H
#define HUSH_SQLCOMMAND_H

#include <vector>
using namespace std;

#include "..\..\Common\BasicType.h"
#include "..\QueryProcessor\Analyzer.h"
#include "..\QueryProcessor\Optimizer.h"

using namespace Hush;

namespace HushDB
{
    class SqlCommand
    {
    public:
        SqlCommand(Catalog::Ptr catalog)
            :catalog(catalog)
        {
        }

        IDataReader::Ptr ExecuteReader()
        {
            SqlToken::List tokens = Tokenizer::Parse(this->CommandText);
            SelectStmt::Ptr selectStmt = SelectStmt::Parse(tokens.begin(), tokens.end());

            Analyzer analyzer(catalog);
            plan = analyzer.Analyze(selectStmt);

            Optimizer o(catalog);
            executionPlan = o.Optimize(plan);

            reader = executionPlan->Execute();
            return reader;
        }

        String CommandText;

    private:
        Catalog::Ptr catalog;
        IExecutionPlanNode::Ptr executionPlan;
        LogicalPlan::Ptr plan;
        IDataReader::Ptr reader;
    };
}

#endif
