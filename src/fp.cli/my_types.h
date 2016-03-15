#ifndef _MY_TYPES_H_
#define _MY_TYPES_H_

typedef unsigned char      UINT8;
typedef unsigned short     UINT16;
typedef unsigned long      UINT32;
typedef unsigned long long UINT64;

typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned long      uint32;
typedef unsigned long long uint64;

typedef unsigned char      L7_uchar;
typedef unsigned char      L7_uint8;
typedef unsigned short     L7_uint16;
typedef unsigned long      L7_uint;
typedef unsigned long      L7_uint32;
typedef unsigned long long L7_uint64;

typedef unsigned char	   u8;
typedef unsigned short	   u16;
typedef unsigned long	   u32;
typedef unsigned long long u64;

typedef signed char        INT8;
typedef signed short       INT16;
typedef signed long        INT32;
typedef signed long long   INT64;

typedef signed char        int8;
typedef signed short       int16;
typedef signed long        int32;
typedef signed long long   int64;

typedef signed char        L7_char;
typedef signed char        L7_int8;
typedef signed short       L7_int16;
typedef signed long        L7_int;
typedef signed long        L7_int32;
typedef signed long long   L7_int64;

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifndef MAKEBOOL01
#define MAKEBOOL01(value) ((value)?1:0)
#endif

#define min(a,b)    ((a) < (b)?     (a):    (b))

#define NLS  "\n\r"

#endif //_MY_TYPES_H_

