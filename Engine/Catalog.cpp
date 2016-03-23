#include "Catalog.h"

namespace HushDB
{
    ColumnDesc::Ptr TableDefDescType::ObjectIdDesc = make_shared<ColumnDesc>(T("ObjectId"), SqlType::Int);
    ColumnDesc::Ptr TableDefDescType::ObjectNameDesc = make_shared<ColumnDesc>(T("ObjectName"), SqlType::String);
    ColumnDesc::Ptr TableDefDescType::TypeDesc = make_shared<ColumnDesc>(T("Type"), SqlType::Int);

    TableDefDescType::Ptr TableDefDesc = make_shared<TableDefDescType>();
}