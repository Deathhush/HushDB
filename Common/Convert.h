#ifndef HUSH_CONVERT_H
#define HUSH_CONVERT_H

#include "BasicType.h"

#include <sstream>
using namespace std;

namespace Hush
{
    template <typename T>
    struct Convert
    {
        static String ToString(const T& value) { return String(); }
    };

    template<typename T>
    String ToString(const T& value)
    {
        wstringstream ss;
        ss << value;
        String result;
        ss >> result;

        return result;
    }

    template<typename T>
    T Parse(const String& value)
    {
        wstringstream ss;
        ss << value;
        T result;
        ss >> result;

        return result;
    }


    template <typename T>
    struct ToStringTrait
    {
        enum { Supported = false };
    };

    // String
    template <>
    struct ToStringTrait<String>
    {
        enum { Supported = true };
    };

    template <>
    struct Convert<String>
    {
        static String ToString(const String& value)
        {
            return value;
        }
    };

    // Int
    template <>
    struct ToStringTrait<Int32>
    {
        enum { Supported = true };
    };

    template <>
    struct Convert<Int32>
    {
        static String ToString(const Int32& value)
        {
            return Hush::ToString<Int32>(value);
        }

        static Int32 Parse(const String& value)
        {
            return Hush::Parse<Int32>(value);
        }
    };

    // UInt32
    template <>
    struct ToStringTrait<UInt32>
    {
        enum { Supported = true };
    };

    template <>
    struct Convert<UInt32>
    {
        static String ToString(const Int32& value)
        {
            return Hush::ToString<UInt32>(value);
        }
    };
}

#endif
