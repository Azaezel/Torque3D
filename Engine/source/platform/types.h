//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef _TORQUE_TYPES_H_
#define _TORQUE_TYPES_H_

#include <cstdint>
#include <cstddef>
#include <limits>

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------Basic Types--------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::int8_t        S8;      ///< Compiler independent Signed Char
typedef std::uint8_t       U8;      ///< Compiler independent Unsigned Char

typedef std::int16_t       S16;     ///< Compiler independent Signed 16-bit short
typedef std::uint16_t      U16;     ///< Compiler independent Unsigned 16-bit short

typedef std::int32_t       S32;     ///< Compiler independent Signed 32-bit integer
typedef std::uint32_t      U32;     ///< Compiler independent Unsigned 32-bit integer

typedef float              F32;     ///< Compiler independent 32-bit float
typedef double             F64;     ///< Compiler independent 64-bit float

typedef std::int64_t       S64;
typedef std::uint64_t      U64;

typedef std::size_t        dsize_t;

struct EmptyType {};                ///< "Null" type used by templates

#define TORQUE_UNUSED(var) (void)sizeof(var)

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------String Types--------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef char           UTF8;        ///< Compiler independent 8 bit Unicode encoded character

#if defined(_MSC_VER)
typedef wchar_t        UTF16;
#else
typedef std::uint16_t UTF16;       ///< Compiler independent 16 bit Unicode encoded character
#endif

typedef std::uint32_t   UTF32;       ///< Compiler independent 32 bit Unicode encoded character

typedef const char* StringTableEntry;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------- Type constants-------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define __EQUAL_CONST_F F32(0.000001)                                  ///< Constant float epsilon used for F32 comparisons

constexpr F32 Float_Inf = std::numeric_limits<F32>::infinity();
constexpr F32 Float_One  = F32(1.0);                           ///< Constant float 1.0
constexpr F32 Float_Half = F32(0.5);                           ///< Constant float 0.5
constexpr F32 Float_Zero = F32(0.0);                           ///< Constant float 0.0
constexpr F32 Float_Pi   = F32(3.14159265358979323846);        ///< Constant float PI
constexpr F32 Float_2Pi  = F32(2.0 * 3.14159265358979323846);  ///< Constant float 2*PI
constexpr F32 Float_InversePi = F32(1.0 / 3.14159265358979323846); ///< Constant float 1 / PI
constexpr F32 Float_HalfPi = F32(0.5 * 3.14159265358979323846);    ///< Constant float 1/2 * PI
constexpr F32 Float_2InversePi = F32(2.0 / 3.14159265358979323846);///< Constant float 2 / PI
constexpr F32 Float_Inverse2Pi = F32(0.5 / 3.14159265358979323846);///< Constant float 0.5 / PI

constexpr F32 Float_Sqrt2 = F32(1.41421356237309504880f);          ///< Constant float sqrt(2)
constexpr F32 Float_SqrtHalf = F32(0.7071067811865475244008443f);  ///< Constant float sqrt(0.5)

constexpr S8 S8_MIN  = std::numeric_limits<S8>::min();                              ///< Constant Min Limit S8
constexpr S8 S8_MAX  = std::numeric_limits<S8>::max();                               ///< Constant Max Limit S8
constexpr U8 U8_MAX  = std::numeric_limits<U8>::max();                               ///< Constant Max Limit U8

constexpr S16 S16_MIN = std::numeric_limits<S16>::min();                           ///< Constant Min Limit S16
constexpr S16 S16_MAX = std::numeric_limits<S16>::max();                           ///< Constant Max Limit S16
constexpr U16 U16_MAX = std::numeric_limits<U16>::max();                            ///< Constant Max Limit U16

constexpr S32 S32_MIN = std::numeric_limits<S32>::min();                       ///< Constant Min Limit S32
constexpr S32 S32_MAX = std::numeric_limits<S32>::max();                       ///< Constant Max Limit S32
constexpr U32 U32_MAX = std::numeric_limits<U32>::max();                       ///< Constant Max Limit U32

constexpr S64 S64_MIN = std::numeric_limits<S64>::min();                      ///< Constant Min Limit S64
constexpr S64 S64_MAX = std::numeric_limits<S64>::max();                      ///< Constant Max Limit S64
constexpr U64 U64_MAX = std::numeric_limits<U64>::max();                       ///< Constant Max Limit U64

constexpr F32 F32_MIN = std::numeric_limits<F32>::min();                 ///< Constant Min Limit F32
constexpr F32 F32_MAX = std::numeric_limits<F32>::max();                 ///< Constant Max Limit F32

// define all the variants of Offset that we might use
#define _Offset_Normal(x, cls) ((dsize_t)((const char *)&(((cls *)1)->x)-(const char *)1))
#define _Offset_Variant_1(x, cls) ((int)(&((cls *)1)->x) - 1)
#define _Offset_Variant_2(x, cls) offsetof(cls, x) // also requires #include <stddef.h>

//--------------------------------------
// Identify the compiler being used

// Microsoft Visual C++/Visual.NET
#if defined(_MSC_VER)
#  include "platform/types.visualc.h"
// GNU GCC
#elif defined(__GNUC__)
#  include "platform/types.gcc.h"
#else
#  error "Unknown Compiler"
#endif

/// Integral type matching the host's memory address width.
typedef std::uintptr_t MEM_ADDRESS;

#ifdef _MSC_VER
#define ALIGNAS(x) __declspec( align( x ) ) 
#else
#define ALIGNAS(x)  __attribute__ ((aligned( x )))
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------- GeneralMath Helpers ---------------------------------------- //
//////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Determines if number is a power of two.
/// Zero is not a power of two. So we want take into account that edge case
inline bool isPow2(const U32 num)
{
   return (num != 0) && ((num & (num - 1)) == 0);
}

/// Determines the binary logarithm of the input value rounded down to the nearest power of 2.
inline U32 getBinLog2(U32 value)
{
   F32 floatValue = F32(value);
   return (*((U32 *) &floatValue) >> 23) - 127;
}

/// Determines the binary logarithm of the next greater power of two of the input number.
inline U32 getNextBinLog2(U32 number)
{
   return getBinLog2(number) + (isPow2(number) ? 0 : 1);
}

/// Determines the next greater power of two from the value. If the value is a power of two, it is returned.
inline U32 getNextPow2(U32 value)
{
   return isPow2(value) ? value : (1 << (getBinLog2(value) + 1));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------Many versions of min and max--------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DeclareTemplatizedMinMax(type) \
  inline type getMin(type a, type b) { return a > b ? b : a; } \
  inline type getMax(type a, type b) { return a > b ? a : b; }

DeclareTemplatizedMinMax( U32 )
DeclareTemplatizedMinMax( S32 )
DeclareTemplatizedMinMax( U16 )
DeclareTemplatizedMinMax( S16 )
DeclareTemplatizedMinMax( U8 )
DeclareTemplatizedMinMax( S8 )
DeclareTemplatizedMinMax( F32 )
DeclareTemplatizedMinMax( F64 )
DeclareTemplatizedMinMax( dsize_t )

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------FOURCC------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(TORQUE_BIG_ENDIAN)
#define makeFourCCTag(c0,c1,c2,c3) ((U32) ((((U32)((U8)(c0)))<<24) + (((U32)((U8)(c1)))<<16) + (((U32)((U8)(c2)))<<8) + ((((U32)((U8)(c3))))))
#else
#ifdef TORQUE_LITTLE_ENDIAN
#define makeFourCCTag(c3,c2,c1,c0) ((U32) ((((U32)((U8)(c0)))<<24) + (((U32)((U8)(c1)))<<16) + (((U32)((U8)(c2)))<<8) + (((U32)((U8)(c3))))))
#else
#error BYTE_ORDER not defined
#endif
#endif

#define BIT(x) (1 << (x))                       ///< Returns value with bit x set (2^x)

#if defined(TORQUE_OS_WIN)
#define STDCALL __stdcall
#define FN_CDECL __cdecl
struct FileTime
{
   U32 v1;
   U32 v2;
};
#elif defined(TORQUE_OS_MAC)
#define STDCALL
#define FN_CDECL
typedef U64 FileTime;
#else
#define STDCALL
#define FN_CDECL
typedef S32 FileTime;
#endif

#ifndef NULL
#  define NULL 0
#endif

#endif //_TORQUE_TYPES_H_
