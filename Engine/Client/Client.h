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
        typedef shared_ptr<DbValue> Ptr;
        DbValue(bool isNull) : IsNull(isNull) {}
        DbValue() : IsNull(true) {}
        bool IsNull;

        virtual Int32 Size() = 0;
        virtual const Byte* Data() = 0;

        virtual ~DbValue() {};
    };

    struct DbInt : public DbValue
    {
        typedef shared_ptr<DbInt> Ptr;

        DbInt() : Value(0), DbValue(true) {}
        DbInt(const Int32& value) : Value(value), DbValue(false) {}

        virtual Int32 Size() override
        {
            return sizeof(Int32);
        }

        virtual const Byte* Data() override
        {
            return (Byte*)&(this->Value);
        }

        Int32 Value;
    };

    struct DbFloat : public DbValue
    {
        typedef shared_ptr<DbFloat> Ptr;

        DbFloat() : Value(0.0), DbValue(true) {}
        DbFloat(const float& value) : Value(value), DbValue(false) {}

        virtual Int32 Size() override
        {
            return sizeof(float);
        }

        virtual const Byte* Data() override
        {
            return (Byte*)&(this->Value);
        }

        float Value;
    };

    struct DbString : public DbValue
    {
        typedef shared_ptr<DbString> Ptr;

        DbString() : DbValue(true) {}
        DbString(const String& value) : Value(value), DbValue(false) {}

        virtual Int32 Size() override
        {
            return this->Value.length()*sizeof(Char);
        }

        virtual const Byte* Data() override
        {
            return (Byte*)this->Value.data();
        }

        String Value;
    };

    struct IDataRow
    {
        typedef shared_ptr<IDataRow> Ptr;
        virtual shared_ptr<DbValue> GetValue(const String& columnName) = 0;
        virtual shared_ptr<DbValue> GetValue(const Int32& columnIndex) = 0;

        template<typename T>
        shared_ptr<T> GetValue(const String& columnName)
        {
            return dynamic_pointer_cast<T>(this->GetValue(columnName));
        }

        template<typename T>
        shared_ptr<T> GetValue(const Int32& columnIndex)
        {
            return dynamic_pointer_cast<T>(this->GetValue(columnIndex));
        }

        virtual ~IDataRow() {}
    };
        
    typedef public IEnumerator<IDataRow::Ptr> IDataReader;
}



#endif