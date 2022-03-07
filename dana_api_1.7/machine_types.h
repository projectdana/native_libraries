#ifndef _MACHINE_TYPES_H
#define _MACHINE_TYPES_H

//http://www.wheresthehashkeyonamac.co.uk
#include <inttypes.h>

#ifdef MACHINE_16
typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;
typedef unsigned long long uint64;
typedef uint16 MemAddr;
#ifndef UINT8_MAX
#define UINT8_MAX 255
#endif
#ifndef UINT16_MAX
#define UINT16_MAX UINT_MAX
#endif
#ifndef UINT32_MAX
#define UINT32_MAX ULONG_MAX
#endif
#ifndef UINT64_MAX
#define UINT64_MAX ULONGLONG_MAX
#endif
#endif

#ifdef MACHINE_32
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef uint32 MemAddr;
#ifndef UINT8_MAX
#define UINT8_MAX 255
#endif
#ifndef UINT16_MAX
#define UINT16_MAX USHRT_MAX
#endif
#ifndef UINT32_MAX
#define UINT32_MAX UINT_MAX
#endif
#ifndef UINT64_MAX
#define UINT64_MAX ULONG_MAX
#endif
#endif

#ifdef MACHINE_64
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef uint64 MemAddr;
#ifndef UINT8_MAX
#define UINT8_MAX 255
#endif
#ifndef UINT16_MAX
#define UINT16_MAX USHRT_MAX
#endif
#ifndef UINT32_MAX
#define UINT32_MAX UINT_MAX
#endif
#ifndef UINT64_MAX
#define UINT64_MAX ULONG_MAX
#endif
#endif

#endif
