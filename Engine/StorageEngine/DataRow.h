#ifndef HUSHDB_DATAROW_H
#define HUSHDB_DATAROW_H

#include <memory.h>
#include <vector>
#include <algorithm>
using namespace std;

#include "..\..\Common\BasicType.h"
#include "..\Client\Client.h"
#include "..\Schema.h"
using namespace Hush;

namespace HushDB
{
    // We use variable size DataRow layout
    // For a row with n field. The DataRow starts with an Int16[n+1] array 
    // in which [i] stores the start offset of the i-th field (starting from 0).
    // offset -1 means the field is null.
    // TODO: Make the layout aligned.
    class DataRow : public IDataRow
    {
    public:
        static const Int16 NullOffset = -1;

        static Int32 CalculateRowSize(const vector<DbValue::Ptr>& fields)
        {
            int totalSize = 2;
            for_each(fields.begin(), fields.end(), [&](DbValue::Ptr v) { totalSize += (v->Size()+2); });
            return totalSize;
        }

        // This method assumes that Byte* has enough space to hold all the values
        static void CopyDataRow(Byte* destination, const vector<DbValue::Ptr>& fields)
        {
            int fieldCount = fields.size();
            Int16 startingOffset = (fieldCount+1) * sizeof(Int16);            
            Int16* offsets = (Int16*)destination;            
            Int16 currentOffset = startingOffset;            
            for (int i = 0; i < fieldCount; i++)
            {
                DbValue::Ptr field = fields[i];
                if (field->IsNull)
                {
                    offsets[i] = DataRow::NullOffset;                    
                }
                else
                {
                    offsets[i] = currentOffset;
                    
                    // Assume the field size won't span multiple pages.
                    // Large record requires special handling.
                    Int16 fieldSize = (Int16)field->Size();

                    Byte* currentData = destination + currentOffset;
                    memcpy(currentData, field->Data(), fieldSize);

                    currentOffset += fieldSize;                    
                }
            }

            offsets[fieldCount] = currentOffset;
        }

    public:
        DataRow(ITupleDesc::Ptr schema, Byte* data)
            : Schema(schema), Data(data)
        {
        }

        virtual shared_ptr<DbValue> GetValue(const String& columnName) override
        {
            int columnIndex = this->Schema->GetOrdinal(columnName);
            return this->GetValue(columnIndex);            
        }

        virtual shared_ptr<DbValue> GetValue(const Int32& columnIndex) override
        {
            Int16* offsets = (Int16*)this->Data;
            Int16 fieldOffset = offsets[columnIndex];
            Int16 fieldSize = offsets[columnIndex + 1] - offsets[columnIndex];
            Byte* fieldData = this->Data + fieldOffset;
            IColumnDef::Ptr columnDesc = this->Schema->GetColumnDesc(columnIndex);

            switch (columnDesc->ColumnType())
            {
            case SqlType::Int:
                if (fieldOffset == DataRow::NullOffset)
                {
                    return make_shared<DbInt>();
                }
                else
                {
                    return make_shared<DbInt>(*(Int32*)fieldData);
                }
                break;
            case SqlType::Float:
                if (fieldOffset == DataRow::NullOffset)
                {
                    return make_shared<DbFloat>();
                }
                else
                {
                    return make_shared<DbFloat>(*(float*)fieldData);
                }
                break;
            case SqlType::String:
                if (fieldOffset == DataRow::NullOffset)
                {
                    return make_shared<DbString>();
                }
                else
                {
                    return make_shared<DbString>(String((Char*)fieldData, fieldSize / sizeof(Char)));
                }
                break;
            }
        }

        ITupleDesc::Ptr Schema;
        Byte* Data;
    };
}

#endif