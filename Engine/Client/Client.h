#ifndef HUSH_CLIENT_H
#define HUSH_CLIENT_H

#include <vector>
#include <memory>
using namespace std;

#include "..\..\Common\BasicType.h"
using namespace Hush;

namespace HushDB
{
    struct DbValue
    {
        DbValue(bool isNull) : IsNull(isNull) {}
        DbValue() : IsNull(true) {}
        bool IsNull;

        virtual ~DbValue() {};
    };

    struct DbInt : public DbValue
    {
        typedef shared_ptr<DbInt> Ptr;

        DbInt() : Value(0), DbValue(true) {}
        DbInt(const Int32& value) : Value(value), DbValue(false) {}
        Int32 Value;
    };

    struct DbFloat : public DbValue
    {
        typedef shared_ptr<DbFloat> Ptr;

        DbFloat() : Value(0.0), DbValue(true) {}
        DbFloat(const float& value) : Value(value), DbValue(false) {}
        float Value;
    };

    struct DbString : public DbValue
    {
        typedef shared_ptr<DbString> Ptr;

        DbString() : DbValue(true) {}
        DbString(const String& value) : Value(value), DbValue(false) {}
        String Value;
    };

    struct IDataRow
    {
        typedef shared_ptr<IDataRow> Ptr;
        virtual shared_ptr<DbValue> GetValue(const String& columnName) = 0;

        template<typename T>
        shared_ptr<T> GetValue(const String& columnName)
        {
            return dynamic_pointer_cast<T>(this->GetValue(columnName));
        }

        virtual ~IDataRow() {}
    };
        
    struct IDataReader
    {
        typedef shared_ptr<IDataReader> Ptr;

        virtual bool MoveNext() = 0;
        virtual IDataRow::Ptr Current() = 0;
        virtual ~IDataReader() {}
    };    
}



#endif