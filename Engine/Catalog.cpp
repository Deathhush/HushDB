#include "Catalog.h"

namespace HushDB
{
    ColumnDef::Ptr TableDefDesc::ObjectIdDef = make_shared<ColumnDef>(T("ObjectId"), SqlType::Int);
    ColumnDef::Ptr TableDefDesc::ObjectNameDef = make_shared<ColumnDef>(T("ObjectName"), SqlType::String);
    ColumnDef::Ptr TableDefDesc::ObjectTypeDef = make_shared<ColumnDef>(T("ObjectType"), SqlType::Int);
    ColumnDef::Ptr TableDefDesc::HeaderPageIdDef = make_shared<ColumnDef>(T("HeaderPageId"), SqlType::Int);

    TableDefDesc::Ptr TableDefSchema = make_shared<TableDefDesc>();

    ColumnDef::Ptr ColumnDefDesc::ObjectIdDef = make_shared<ColumnDef>(T("ObjectId"), SqlType::Int);
    ColumnDef::Ptr ColumnDefDesc::ColumnNameDef = make_shared<ColumnDef>(T("ColumnName"), SqlType::String);
    ColumnDef::Ptr ColumnDefDesc::ColumnIdDef = make_shared<ColumnDef>(T("ColumnId"), SqlType::Int);
    ColumnDef::Ptr ColumnDefDesc::ColumnTypeDef = make_shared<ColumnDef>(T("ColumnType"), SqlType::Int);

    ColumnDefDesc::Ptr ColumnDefSchema = make_shared<ColumnDefDesc>();
}