// Copyright (C) 2018 ZCaliptium.

#ifndef ZKTTYPES_H
#define ZKTTYPES_H

//! 8 bit unsigned integer variable.
typedef unsigned char u8;

//! 16 bit unsigned integer variable.
typedef unsigned short u16;

//! 32 bit unsigned integer variable.
typedef unsigned int u32;

//! 64 bit unsigned integer variable.
#if defined(_MSC_VER)
typedef unsigned __int64 u64;
#else
typedef unsigned long long u64;
#endif

//! 8-bit signed integer variable.
typedef signed char s8;

//! 16-bit signed integer variable.
typedef signed short s16;

//! 32-bit signed integer variable.
typedef signed int s32;

//! 64-bit signed integer variable.
#if defined(_MSC_VER)
typedef __int64 s64;
#else
typedef long long s64;
#endif

//! 8-bit character variable.
typedef char c8;

//! 32-bit (single-precision) floating-point variable.
typedef float f32;

//! 64-bit (double-precision) floating-point variable.
typedef double f64;

//
#define U8_MIN  0x0
#define U16_MIN 0x0
#define U32_MIN 0x0
#define U64_MIN 0x0

//
#define U8_MAX  0xFF
#define U16_MAX 0xFFFF
#define U32_MAX 0xFFFFFFFF
#define U64_MAX 0xFFFFFFFFFFFFFFFF

// -128 // -32768 // etc.
#define S8_MIN  (u8)(0x80)
#define S16_MIN (u16)(0x8000)
#define S32_MIN (s32)(0x80000000)
#define S64_MIN (s64)(0x8000000000000000)

// 127
// 32767
// 2147483647
// 9223372036854775807
#define S8_MAX  0x7F
#define S16_MAX 0x7FFF
#define S32_MAX 0x7FFFFFFF
#define S64_MAX 0x7FFFFFFFFFFFFFFF

#endif /* include-once check. */
