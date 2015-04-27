/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename     bitset.h
 *
 * @purpose      Internal types for bitset object
 *
 * @component    Routing Utils Component
 *
 * @comments
 *
 * @create       12/25/2000
 *
 * @author       slipov
 *
 * @end
 *
 * ********************************************************************/
#ifndef BIT_SET_H
#define BIT_SET_H

#include "std.h"
#include "xx.ext"
#include "bitset.ext"

#define BYTES_IN_LONG      sizeof(ulng)
#define BITS_IN_BYTE       8
#define BITS_IN_LONG       (BYTES_IN_LONG * BITS_IN_BYTE)
#define ALL_ONES_IN_LONG   0xffffffff
#define ALL_ONES_IN_BYTE   0xff

#define BS_GET_LONG_INDEX(bitIndex)  ((bitIndex)/BITS_IN_LONG)
#define BS_GET_LONG_REM(bitIndex)    ((bitIndex)%BITS_IN_LONG)
#define BS_GET_BYTE_INDEX(bitIndex)  ((bitIndex)/BITS_IN_BYTE)
#define BS_GET_BYTE_REM(bitIndex)    ((bitIndex)%BITS_IN_BYTE)
#define BS_GET_BYTE_SIZE(size)       ((BS_GET_LONG_INDEX((size) - 1) + 1) * BYTES_IN_LONG)

#define BS_INDEX_0_VALID   0x80000000
typedef struct t_BitSet_Tag
{
#ifdef ERRCHK
   XX_Status status;            /* status : valid/invalid   XX_STAT_Valid/XX_STAT_Invalid */
#endif
   ulng      flags;             /* Misc bitset flags, where most significant word contain internal flags */
   ulng      size;              /* Bit set size in bits, unused bits in the last long are set to 0 */
   void      *p_alloc;          /* pointer to the allocated space returned by XX_Malloc */
   ulng      *p_array;          /* Array of longs to store the bitset aligned to ulng */
   ulng      num_0;             /* Number of element having value 0 */
   ulng      index_0;           /* Index of the element set to 0 (caching) */
} t_BitSet;

#endif /* BIT_SET_H */
