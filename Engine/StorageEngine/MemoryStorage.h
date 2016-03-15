#ifndef HUSH_MEMORYSTORAGE_H
#define HUSH_MEMORYSTORAGE_H

#include <map>
using namespace std;

#include "..\Client\Client.h"
#include "..\Schema.h"

namespace HushDB
{
    struct MemoryDataRow : public IDataRow
    {
        typedef DbValue::Ptr ValueType;
        typedef map<String, ValueType> MapType;
        typedef shared_ptr<MemoryDataRow> Ptr;

        virtual DbValue::Ptr GetValue(const String& columnName) override
        {
            return this->Values[this->Schema->GetOrdinal(columnName)];
        }

        vector<DbValue::Ptr> Values;
        TupleDesc::Ptr Schema;
    };

    struct MemoryTable
    {
        typedef vector<MemoryDataRow::Ptr> TableType;
        typedef shared_ptr<MemoryTable> Ptr;

        class Enumerator : public IDataReader
        {
        public:
            Enumerator(const MemoryTable::TableType::iterator& begin, const MemoryTable::TableType::iterator& end, TupleDesc::Ptr schema)
            {
                this->current = begin;
                this->end = end;
                this->schema = schema;

                this->isFirstMove = true;
            }
                        
            virtual bool MoveNext() override
            {
                if (this->isFirstMove)
                {
                    this->isFirstMove = false;
                }
                else
                {
                    this->current++;
                }
                                
                if (this->current != end)
                {
                    return true;
                }
                
                return false;
            }

            virtual IDataRow::Ptr Current() override
            {
                if (hasData)
                {
                    auto row = *current;
                    row->Schema = this->schema;
                    return row;
                }

                throw out_of_range("Reached end");
            }
        private:
            MemoryTable::TableType::iterator current;
            MemoryTable::TableType::iterator end;
            TupleDesc::Ptr schema;
            bool hasData;
            bool isFirstMove;
        };

        IDataReader::Ptr OpenScan()
        {
            Enumerator::Ptr enumerator = make_shared<Enumerator>(this->Rows.begin(), this->Rows.end(), this->Schema);
            return enumerator;
        }


        vector<MemoryDataRow::Ptr> Rows;
        TupleDesc::Ptr Schema;
    };
}

#endif