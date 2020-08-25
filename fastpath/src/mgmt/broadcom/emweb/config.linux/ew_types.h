/*
 * Product: EmWeb/EmStack
 * Release: R6_2_0
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION OF VIRATA CORPORATION
 * THE EMWEB SOFTWARE ARCHITECTURE IS PATENTED (US PATENT #5,973,696)
 * AND OTHER US AND INTERNATIONAL PATENTS PENDING.
 * 'EMWEB' AND 'EMSTACK' ARE TRADEMARKS OF VIRATA CORPORATION
 * 
 * Notice to Users of this Software Product:
 * 
 * This software product of Virata Corporation ("Virata"), 5 Clock Tower
 * Place, Suite 400, Maynard, MA  01754 (e-mail: info@virata.com) in 
 * source and object code format embodies valuable intellectual property 
 * including trade secrets, copyrights and patents which are the exclusive
 * proprietary property of Virata. Access, use, reproduction, modification
 * disclsoure and distribution are expressly prohibited unless authorized
 * in writing by Virata.  Under no circumstances may you copy this
 * software or distribute it to anyone else by any means whatsoever except in
 * strict accordance with a license agreement between Virata and your
 * company.  This software is also protected under patent, trademark and 
 * copyright laws of the United States and foreign countries, as well as 
 * under the license agreement, against any unauthorized copying, modification,
 * or distribution.
 * Please see your supervisor or manager regarding any questions that you may
 * have about your right to use this software product.  Thank you for your
 * cooperation.
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Virata Corporation
 * All Rights Reserved
 * 
 * Virata Corporation
 * 5 Clock Tower Place
 * Suite 400
 * Maynard, MA  01754
 * 
 * Voice: +1 (978) 461-0888
 * Fax:   +1 (978) 461-2080
 * 
 * http://www.emweb.com/
 *   support@agranat.com
 *     sales@agranat.com
 *
 * EmWeb/EmStack common types
 *
 */
#ifndef _EW_TYPES_H
#define _EW_TYPES_H

/*
 * C-TYPES
 * The following basic C-types are defined (see ew_config.h)
 */
#ifndef INT8
#   define INT8 signed char
#endif

#ifndef UINT8
#   define UINT8 unsigned char
#endif

#ifndef INT16
#   define INT16 short int
#endif

#ifndef INT32
#   define INT32 long int
#endif

#ifndef P_INT                   /* an integer the same size as a pointer */
#   define P_INT int
#endif

#ifndef int8
typedef INT8            int8;           /* signed eight bit integer */
#endif /* int8 */

#ifndef int16
typedef INT16           int16;          /* signed sixteen bit integer */
#endif /* int16*/

#ifndef int32
typedef INT32           int32;          /* signed thirty-two bit integer */
#endif /* int32*/

#ifndef uint8
typedef UINT8           uint8;          /* unsigned eight bit integer */
#endif /* unt8 */

#ifndef uint16
typedef unsigned INT16  uint16;         /* unsigned sixteen bit integer */
#endif /* int16*/

#ifndef uint32
typedef unsigned INT32  uint32;         /* unsigned thirty-two bit integer */
#endif /* uint32*/

/*
 * Volatiles for memory shared with devices
 */
#ifndef vint8
typedef volatile int8   vint8;          /* volatile signed eight bits */
#endif /* int8 */

#ifndef vint16
typedef volatile int16  vint16;         /* volatile signed sixteen bits */
#endif /* vint16 */

#ifndef vint32
typedef volatile int32  vint32;         /* volatile signed thirty-two bits */
#endif /* vint32 */

#ifndef vuint8
typedef volatile uint8  vuint8;         /* volatile signed eight bits */
#endif /* vuint8 */

#ifndef vuint16
typedef volatile uint16 vuint16;        /* volatile unsigned sixteen bits */
#endif /* vuint16 */

#ifndef vuint32
typedef volatile uint32 vuint32;        /* volatile unsigned thirty-two bits */
#endif /* vuint32 */


/*
 * FAST TYPES
 * The following types offer the most efficient compilation for optimized speed
 * These are never assumed to be more than 16 bits.
 */
#ifndef sintf
typedef int             sintf;          /* signed integer */
#endif /* sintf */

#ifndef uintf
typedef unsigned        uintf;          /* unsigned integer */
#endif /* uintf */


#if ( ! defined(WIN32) ) || defined(EMWEB_OSE)
/*
 * BOOLEAN
 * The boolean type is used primarily as a function return type.  Note that
 * a default representation of TRUE and FALSE are defined.  However, care
 * should be taken to avoid comparing against these values.  For most compilers,
 * TRUE can be any non-zero integer while FALSE is defined as zero.
 */
#ifndef   TRUE
#  define TRUE            (0 == 0)
#endif
#ifndef   FALSE
#  define FALSE           (!TRUE)
#endif

typedef uintf           boolean;        /* true or false (non-zero or zero) */
#endif /*WIN32 */

/*
 * POINTER INTEGER
 * The pointer integer is an unsigned integer large enough to accommodate a
 * machine pointer.  This is used primarily for generic pointer arithmetic.
 */
typedef P_INT          pint;           /* unsigned integer for pointer */

/*
 * NULL POINTER
 */
#ifndef NULL
#define NULL            (0)
#endif /* NULL */

/*
 * ENDIAN (don't change these)
 */
#define EMWEB_ENDIAN_BIG        1234
#define EMSTACK_ENDIAN_BIG      EMWEB_ENDIAN_BIG
#define EMWEB_ENDIAN_LITTLE     4321
#define EMSTACK_ENDIAN_LITTLE   EMWEB_ENDIAN_LITTLE

typedef enum Endian_e
{
  ewBigEndian    = EMWEB_ENDIAN_BIG,
  ewLittleEndian = EMWEB_ENDIAN_LITTLE
} Endian;

/*
 * Define endian for target
 */
#ifndef WORDS_BIGENDIAN
#  define EMWEB_ENDIAN EMWEB_ENDIAN_LITTLE
#else
#  define EMWEB_ENDIAN EMWEB_ENDIAN_BIG
#endif

/*
 * EMWEB_ARCHIVE_ENDIAN
 * Define endian for target archive as generated by EmWeb/Compiler
 * Recommend that this is the same endian as the target for efficiency.
 * Otherwise, EmWeb/Server must byte-swap words at run-time.
 */
#if defined(WIN32) && ! defined(EMWEB_OSE)
#  define EMWEB_ARCHIVE_ENDIAN    EMWEB_ENDIAN_LITTLE
#else
#  define EMWEB_ARCHIVE_ENDIAN    EMWEB_ENDIAN_BIG
#endif

/*
 * COMPILER COMPATIBILITY
 *
 * __BEGIN_DECLS precedes external definitions, __END_DECLS follows external
 * definitions.  These could be defined as 'extern "C" {' and '}' respectively
s * for GNU C++ compilers, for example.
 */
#ifdef __cplusplus
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }
#else /* __cplusplus */
#define __BEGIN_DECLS
#define __END_DECLS
#endif /* __cplusplus */

/*
 * Prevent type conflict in Windows VC++ V6.0 or greater
 */
#if _MSC_VER >= 1200
#  undef INT32
#endif

/*
 * PROTOTYPES
 */
__BEGIN_DECLS
__END_DECLS


#endif /* _EW_TYPES_H */
