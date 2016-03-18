// ****************************************************************************
// Hush
//
// AUTHOR:  Justin Shen
//
// NOTES: 
//     Defines primitive types.
//
// ****************************************************************************

#ifndef HUSH_BASICTYPE_H
#define HUSH_BASICTYPE_H

#include <string>
#include <memory>

using namespace std;

namespace Hush
{
    typedef char Byte;

    typedef __int16 Int16;
    typedef __int32 Int32;
    typedef __int64 Int64;
    typedef unsigned __int16 UInt16;    
    typedef unsigned __int32 UInt32;
    typedef unsigned __int64 UInt64;
    
	typedef bool Bool;

    typedef wchar_t Char;
    typedef wstring String;
    typedef shared_ptr<String> StringPtr;
    #define T(X) L ## X
    #define STR(X) String(L ## X)

    template<typename T>
    struct IEnumerator
    {
        typedef shared_ptr<IEnumerator<T>> Ptr;

        virtual bool MoveNext() = 0;
        virtual const T& Current() = 0;
        virtual ~IEnumerator() {}
    };

    template<typename T>
    struct IEnumerator<T*>
    {
        typedef shared_ptr<IEnumerator<T*>> Ptr;

        virtual bool MoveNext() = 0;
        virtual T* Current() = 0;
        virtual ~IEnumerator() {}
    };

    template<typename T>
    struct IEnumerator<shared_ptr<T>>
    {
        typedef shared_ptr<IEnumerator<shared_ptr<T>>> Ptr;

        virtual bool MoveNext() = 0;
        virtual shared_ptr<T> Current() = 0;
        virtual ~IEnumerator() {}
    };

    class Exception
    {
    protected:
        String message;
    public:
        Exception(const String& message = String())
            :message(message)
        {
        }

        const String& GetMessage() const
        {
            return message;
        }
    };

    class OutOfRangeException : public Exception
    {
    public:
        OutOfRangeException(const String& message = String())
            :Exception(message)
        {
        }
    };

    class InvalidOperationException : public Exception
    {
    public:
        InvalidOperationException(const String& message = String())
            :Exception(message)
        {
        }
    };

}
#endif