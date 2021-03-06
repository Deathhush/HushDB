#ifndef HUSH_ANALYZER_H
#define HUSH_ANALYZER_H

#include <vector>
using namespace std;

#include "..\..\Common\BasicType.h"
using namespace Hush;

#include "..\Catalog.h"
#include "LogicalPlan.h"
#include "Parser.h"

namespace HushDB
{
    struct AnalyzerException : public Exception
    {
        AnalyzerException(const String& message)
            :Exception(message)
        {

        }
    };


    class Analyzer
    {
    public:
        Analyzer(Catalog* catalog)
        {
            this->catalog = catalog;
        }

        LogicalPlan::Ptr Analyze(SelectStmt::Ptr query)
        {
            LogicalPlan::Ptr plan = make_shared<LogicalPlan>();

            // Process FromClause
            FromClause::Ptr fromClause = query->FromClause;
            IObjectDef::Ptr tableDef = this->catalog->FindTable(*(fromClause->TableName));

            if (tableDef == nullptr)
            {
                throw AnalyzerException(STR("Table [") + *(fromClause->TableName) + STR("] does not exist."));
            }

            plan->AddScan(LogicalScan{ tableDef->ObjectId() });

            // Process SelectList
            TargetList::Ptr targetList = query->TargetList;

            if (targetList->Elements.size() == 0)
            {
                throw AnalyzerException(T("Select list cannot be empty."));
            }

            for (TargetElement::Ptr element : targetList->Elements)
            {
                ITupleDesc::Ptr schema = catalog->FindTableSchema(tableDef);
                if (!element->isStar && !schema->ContainsColumn(*(element->ColumnName)))
                {
                    throw AnalyzerException(STR("Column [") + *(element->ColumnName) + STR("] does not exist in table [.") + *(fromClause->TableName) + STR("]."));
                }

                plan->AddSelectItem(LogicalSelectItem{ element->ColumnName });
            }

            return plan;
        }

    private:
        Catalog* catalog;
    };


}

#endif
