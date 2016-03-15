#ifndef HUSH_LOGICALPLAN_H
#define HUSH_LOGICALPLAN_H

#include <vector>
#include <memory>
using namespace std;

#include "Tokenizer.h"
#include "..\Catalog.h"
using namespace Hush;

namespace HushDB
{
    struct LogicalScan
    {
        TableDef::Ptr TableDef;
    };

    struct LogicalSelectItem
    {
        StringPtr Name;
    };

    class LogicalPlan
    {
    public:
        typedef shared_ptr<LogicalPlan> Ptr;

        void AddScan(const LogicalScan& scan)
        {
            this->Scans.push_back(scan);
        }

        void AddSelectItem(const LogicalSelectItem& item)
        {
            this->SelectItems.push_back(item);
        }

    public:
        vector<LogicalScan> Scans;
        vector<LogicalSelectItem> SelectItems;

    };


}

#endif
