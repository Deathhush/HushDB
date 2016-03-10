#ifndef HUSH_PARSER_H
#define HUSH_PARSER_H

#include <map>
using namespace std;

#include "..\Client\Client.h"

namespace HushDB
{
    struct MapDataRow : public IDataRow
    {
        typedef shared_ptr<DbValue> ValueType;
        typedef map<String, ValueType> MapType;
        typedef shared_ptr<MapDataRow> Ptr;

        virtual shared_ptr<DbValue> GetValue(const String& columnName) override
        {
            return this->Values.at(columnName);
        }

        MapType Values;
        
    };

    struct VectorTable
    {
        typedef vector<MapDataRow::Ptr> TableType;

        class Enumerator : public IDataReader
        {
        public:
            Enumerator(const VectorTable::TableType::iterator& begin, const VectorTable::TableType::iterator& end)
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
            VectorTable::TableType::iterator current;
            VectorTable::TableType::iterator end;
            bool hasData;
            bool isFirstMove;
        };

        IDataReader::Ptr OpenScan()
        {
            Enumerator::Ptr enumerator = make_shared<Enumerator>(this->Data.begin(), this->Data.end());
            return enumerator;
        }


        vector<MapDataRow::Ptr> Data;
    };
}

#endif