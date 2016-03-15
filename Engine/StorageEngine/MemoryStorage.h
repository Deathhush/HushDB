#ifndef HUSH_MEMORYSTORAGE_H
#define HUSH_MEMORYSTORAGE_H

#include <map>
using namespace std;

#include "..\Client\Client.h"

namespace HushDB
{
    struct MemoryDataRow : public IDataRow
    {
        typedef shared_ptr<DbValue> ValueType;
        typedef map<String, ValueType> MapType;
        typedef shared_ptr<MemoryDataRow> Ptr;

        virtual shared_ptr<DbValue> GetValue(const String& columnName) override
        {
            return this->Values.at(columnName);
        }

        MapType Values;
        
    };

    struct MemoryTable
    {
        typedef vector<MemoryDataRow::Ptr> TableType;
        typedef shared_ptr<MemoryTable> Ptr;

        class Enumerator : public IDataReader
        {
        public:
            Enumerator(const MemoryTable::TableType::iterator& begin, const MemoryTable::TableType::iterator& end)
            {
                this->current = begin;
                this->end = end;

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
                
            }

            virtual IDataRow::Ptr Current() override
            {
                if (hasData)
                {
                    return *current;
                }

                throw out_of_range("Reached end");
            }
        private:
            MemoryTable::TableType::iterator current;
            MemoryTable::TableType::iterator end;
            bool hasData;
            bool isFirstMove;
        };

        IDataReader::Ptr OpenScan()
        {
            Enumerator::Ptr enumerator = make_shared<Enumerator>(this->Rows.begin(), this->Rows.end());
            return enumerator;
        }


        vector<MemoryDataRow::Ptr> Rows;
    };
}

#endif