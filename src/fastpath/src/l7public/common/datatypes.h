/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename datatypes.h
*
* @purpose The purpose of this file is to have a central location for
*          datatypes to be used by the entire system.
*
* @component sysapi
*
* @comments none
*
* @create 7/26/2000
*
* @author smanders
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_DATATYPES
#define INCLUDE_DATATYPES
#ifdef _L7_OS_ECOS_
#include <stdio.h>
#endif

typedef char            L7_char8;
typedef unsigned char   L7_uchar8;

typedef signed char     L7_int8;
typedef unsigned char   L7_uint8;

typedef short           L7_short16;
typedef unsigned short  L7_ushort16;

typedef long            L7_long32;
typedef unsigned long   L7_ulong32;

typedef unsigned long long L7_uint64;
typedef signed long long   L7_int64;

typedef int             L7_int32;
typedef unsigned int    L7_uint32;

/* PTin added: first changes */
typedef short           L7_int16;
typedef unsigned short  L7_uint16;
/* PTin end */

typedef double          L7_double64;

/* These data types correspond to the natural integer size of the CPU.
 *  - Useful for loop indicies, etc.
 *  - Do not use for variables whose memory footprint size must be tightly controlled.
 */
typedef int             L7_int;
typedef unsigned int    L7_uint;

typedef L7_uint32       L7_netBufHandle;
typedef L7_uint32       L7_netBlockHandle;

/* Represents a 64 bit unsigned integer. */
typedef struct
{
  L7_uint32   high;
  L7_uint32   low;
} L7_ulong64;


typedef enum
{
  L7_FALSE = 0,
  L7_TRUE = 1
} L7_BOOL;

/* RAMA: We need #ifdef's so that we can compare #if XXX == L7_TRUE_DEF */
#define L7_FALSE_DEF 0
#define L7_TRUE_DEF 1

/*--------------------------------------*/
/*  Common Return Codes                 */
/*--------------------------------------*/
typedef enum
{
  L7_SUCCESS = 0,
  L7_FAILURE,
  L7_ERROR,
  L7_NOT_IMPLEMENTED_YET,
  L7_NOT_SUPPORTED,
  L7_NOT_EXIST,
  L7_ALREADY_CONFIGURED,
  L7_TABLE_IS_FULL,
  L7_REQUEST_DENIED,
  L7_ASYNCH_RESPONSE,
  L7_ADDR_INUSE,
  L7_NO_VALUE,
  L7_NO_MEMORY,
  L7_DEPENDENCY_NOT_MET,
  L7_HARDWARE_ERROR,
  L7_IMAGE_IN_USE
} L7_RC_t;


#ifdef __cplusplus
typedef L7_RC_t (*L7_FUNCPTR_t) (...); /* ptr to funct returning L7_RC */
typedef void (*L7_VOIDFUNCPTR_t) (...); /* ptr to fcn rtrning void  */
#else
typedef L7_RC_t (*L7_FUNCPTR_t) (); /* ptr to function returning L7_RC*/
typedef void (*L7_VOIDFUNCPTR_t) (); /* ptr to fcn rtrning void  */
#endif			/* _cplusplus */

#ifdef __cplusplus
  #define EXT_API extern "C"
#else
  #define EXT_API extern
#endif
/* For Linux we need to define FUNCPTR type.
*/
#if defined(_L7_OS_LINUX_) || defined(_L7_OS_ECOS_)
  #ifdef __cplusplus
typedef L7_int32    (*L7_FUNCPTR) (...); 
  #else
typedef L7_int32    (*L7_FUNCPTR) (); 
  #endif
#endif
#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
typedef L7_RC_t (*fNimSetIntfAdminState)(L7_uint32, L7_uint32);
#endif
#endif

#endif

