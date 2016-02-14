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

namespace Hush
{
    typedef unsigned char Byte;

    typedef __int16 Int16;
    typedef __int32 Int32;
    typedef __int64 Int64;
    typedef unsigned __int16 UInt16;    
    typedef unsigned __int32 UInt32;
    typedef unsigned __int64 UInt64;
    
	typedef bool Bool;

    typedef wchar_t Char;
    typedef std::wstring String;
    #define T(X) L ## X

}
#endif