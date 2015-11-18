/* $Id: sand_u64.h,v 1.5 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/


#ifndef __SOC_SAND_U64_H_INCLUDED__
/* { */
#define __SOC_SAND_U64_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>

#define SOC_SAND_U64_NOF_UINT32S (2)
/* $Id: sand_u64.h,v 1.5 Broadcom SDK $
 * 64 bit unsigned number
 */
typedef struct
{
  /*
   * Low  32 bits - arr[0]
   * High 32 bits - arr[1]
   */
  uint32 arr[SOC_SAND_U64_NOF_UINT32S];

} SOC_SAND_U64;


/*****************************************************
*NAME
* soc_long_to_u64
*TYPE:
*  PROC
*DATE:
*  8-Feb-15
*FUNCTION:
*  Creates a u64 out of a uint32
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32 u32 -
*      Source unit32
*    SOC_SAND_OUT SOC_SAND_U64* u64 -
*      Output structre
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    void
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  soc_sand_long_to_u64(
    SOC_SAND_IN  uint32      		u32,
    SOC_SAND_OUT SOC_SAND_U64* 		u64
  );

/*****************************************************
*NAME
* soc_sand_u64_copy
*TYPE:
*  PROC
*DATE:
*  8-Feb-15
*FUNCTION:
*  Copy one u64 to the other, return FALSE in case of an error
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN SOC_SAND_U64* src -
*      Structure to copy
*    SOC_SAND_OUT SOC_SAND_U64* dst -
*      Output structre
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    void
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  soc_sand_u64_copy(
    SOC_SAND_IN  SOC_SAND_U64*      src,
    SOC_SAND_OUT SOC_SAND_U64* 		dst
  );
/*****************************************************
*NAME
* soc_sand_u64_clear
*TYPE:
*  PROC
*DATE:
*  9-Sep-03
*FUNCTION:
*  Clears all fields in the structure.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_U64* u64 -
*      Structure to clear.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    void
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
void
  soc_sand_u64_clear(
    SOC_SAND_INOUT SOC_SAND_U64* u64
  );

/*****************************************************
*NAME
* soc_sand_u64_to_long
*TYPE:
*  PROC
*DATE:
*  9-Sep-03
*FUNCTION:
*  Convert 'u64' from 64 bit number to 'ulong' 32 bit number.
*  If 'u64' > 0xFFFF_FFFF than 'ulong' is 0xFFFF_FFFF
*  and overflowed indicator is retruned.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  SOC_SAND_U64*      u64 -
*      Structure to convert.
*    SOC_SAND_OUT uint32* ulong -
*      the number after conversion.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32
*      Can overflowed
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  soc_sand_u64_to_long(
    SOC_SAND_IN  SOC_SAND_U64*      u64,
    SOC_SAND_OUT uint32* ulong
  );


/*****************************************************
*NAME
* soc_sand_u64_multiply_longs
*TYPE:
*  PROC
*DATE:
*  03/03/2004
*FUNCTION:
*  Multiply 2 longs and load result into SOC_SAND_U64.
*  result = x * y.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  uint32 x -
*      value to multipy
*    SOC_SAND_IN  uint32 y -
*      value to multipy
*    SOC_SAND_OUT SOC_SAND_U64*     result -
*      Loaded with result
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
void
  soc_sand_u64_multiply_longs(
    SOC_SAND_IN  uint32 x,
    SOC_SAND_IN  uint32 y,
    SOC_SAND_OUT SOC_SAND_U64*     result
  );


/*****************************************************
*NAME
* soc_sand_u64_devide_u64_long
*TYPE:
*  PROC
*DATE:
*  03/03/2004
*FUNCTION:
*  Calculates quotient result = x div v
*  Returns remainder r = x mod v
*  where resultq, u are multiprecision integers of 64 bit each
*  and d, v are long digits.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  SOC_SAND_U64*     x -
*      value to device
*    SOC_SAND_IN  uint32 y  -
*      value to device
*    SOC_SAND_OUT uint32 result -
*      Loaded with result
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32
*      remainder
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  soc_sand_u64_devide_u64_long(
    SOC_SAND_IN  SOC_SAND_U64*     x,
    SOC_SAND_IN  uint32 y,
    SOC_SAND_OUT SOC_SAND_U64*     result
  );

/*****************************************************
*NAME
* soc_sand_u64_add_long
*TYPE:
*  PROC
*DATE:
*  03/03/2004
*FUNCTION:
*  Calculates x = x + y
*  Returns carryif overflowed
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_INOUT  SOC_SAND_U64*     x -
*      value to add
*    SOC_SAND_IN     uint32 y -
*      value to add
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32
*      carry.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  soc_sand_u64_add_long(
    SOC_SAND_INOUT  SOC_SAND_U64*     x,
    SOC_SAND_IN     uint32 y
  );

/*****************************************************
*NAME
* soc_sand_u64_add_u64
*TYPE:
*  PROC
*DATE:
*  03/03/2004
*FUNCTION:
*  Calculates result = x + y
*  Returns carryif overflowed
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  SOC_SAND_U64*     x -
*      value to add
*    SOC_SAND_IN  uint32 y  -
*      value to add
*    SOC_SAND_OUT uint32 result -
*      Loaded with result
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32
*      carry.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  soc_sand_u64_add_u64(
    SOC_SAND_INOUT  SOC_SAND_U64*     x,
    SOC_SAND_IN     SOC_SAND_U64*     y
  );


/*****************************************************
*NAME
* soc_sand_u64_subtract_u64
*TYPE:
*  PROC
*DATE:
*  03/03/2004
*FUNCTION:
*  Calculates x = x - y
*  Returns FALSE if y > x
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_INOUT  SOC_SAND_U64*     x -
*      value to subtract from
*    SOC_SAND_IN     SOC_SAND_U64*     y -
*      value to subtract
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    TRUE x > y.
*    FALSE: x < y.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  If x < y; x<-0, and return FALSE.
*SEE ALSO:
*****************************************************/
uint32
  soc_sand_u64_subtract_u64(
    SOC_SAND_INOUT  SOC_SAND_U64*     x,
    SOC_SAND_IN     SOC_SAND_U64*     y
  );

/*****************************************************
*NAME
* soc_sand_u64_is_bigger
*TYPE:
*  PROC
*DATE:
*  01/07/2004
*FUNCTION:
*  Calculates (x > y)
*  Returns TRUE or FALSE
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  SOC_SAND_U64*     x -
*      value to compare
*    SOC_SAND_IN  SOC_SAND_U64*     y  -
*      value to compare
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32
*      Indicator.
*      TRUE iff (x > y).
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  soc_sand_u64_is_bigger(
    SOC_SAND_IN     SOC_SAND_U64*     x,
    SOC_SAND_IN     SOC_SAND_U64*     y
  );

/*****************************************************
*NAME
* soc_sand_u64_is_zero
*TYPE:
*  PROC
*DATE:
*  01/07/2004
*FUNCTION:
*  Calculates (x == y)
*  Returns TRUE or FALSE
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  SOC_SAND_U64*     x -
*      value to compare
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32
*      Indicator.
*      TRUE iff (x == 0).
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  soc_sand_u64_is_zero(
    SOC_SAND_IN     SOC_SAND_U64*     x
  );

/*****************************************************
*NAME
* soc_sand_u64_are_equal
*TYPE:
*  PROC
*DATE:
*  01/07/2004
*FUNCTION:
*  Calculates (x == y)
*  Returns TRUE or FALSE
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  SOC_SAND_U64*     x -
*      value to compare
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  SOC_SAND_U64*     y -
*      value to compare
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32
*      Indicator.
*      TRUE iff (x == y).
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  soc_sand_u64_are_equal(
    SOC_SAND_IN     SOC_SAND_U64*     x,
    SOC_SAND_IN     SOC_SAND_U64*     y
  );

/*****************************************************
*NAME
*  soc_sand_u64_log2_round_up
*TYPE:
*  PROC
*DATE:
*  16-Jul-07
*FUNCTION:
*  Given number - x.
*  Return the integer round up of log2(x)
*CALLING SEQUENCE:
*  soc_sand_log2_round_up(x)
*INPUT:
*  SOC_SAND_DIRECT:
*    const uint32 x -
*     Number to log.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 -
*       Log with base 2. Round up
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  Examples:
*    soc_sand_log2_round_up(0) = 0 -- definition
*    soc_sand_log2_round_up(1) = 0
*    soc_sand_log2_round_up(2) = 1
*    soc_sand_log2_round_up(3) = 2
*    soc_sand_log2_round_up(4) = 2
*    soc_sand_log2_round_up(5) = 3
*SEE ALSO:
*****************************************************/
uint32
  soc_sand_u64_log2_round_up(
    SOC_SAND_IN SOC_SAND_U64 *x
  );

/*****************************************************
*NAME
* soc_sand_u64_shift_left
*TYPE:
*  PROC
*DATE:
*  03/03/2004
*FUNCTION:
*  Computes a = b << x.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_OUT SOC_SAND_U64*    a -
*      loaded with result
*    SOC_SAND_IN  SOC_SAND_U64*    b -
*      number to shift
*    SOC_SAND_IN  uint32 result -
*      Loaded with result
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 - the carry.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  soc_sand_u64_shift_left(
    SOC_SAND_OUT SOC_SAND_U64*    a,
    SOC_SAND_IN  SOC_SAND_U64*    b,
    SOC_SAND_IN  uint32 x
  );

#if SOC_SAND_DEBUG
/* { */
/*
 */

/*****************************************************
*NAME
* soc_sand_u64_print
*TYPE:
*  PROC
*DATE:
*  9-Sep-03
*FUNCTION:
*  Print service to SOC_SAND_U64.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_U64* u64 -
*      Structure to print.
*    SOC_SAND_IN uint32 print_type -
*      0 - Decimal format.
*      1 - Hexadecimal format.
*    uint32 short_format -
*      Short or long print format.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    void
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
void
  soc_sand_u64_print(
    SOC_SAND_IN SOC_SAND_U64*    u64,
    SOC_SAND_IN uint32 print_type,
    SOC_SAND_IN uint32 short_format
  );


/*****************************************************
*NAME
* soc_sand_u64_test
*TYPE:
*  PROC
*DATE:
*  9-Sep-03
*FUNCTION:
*  Test function of SOC_SAND_U64.
*  View prints and return value to see pass/fail
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32
*       TRUE  - PASS.
*       FALSE - FAIL
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  soc_sand_u64_test(uint32 silent);

/*
 * }
 */
#endif

#ifdef  __cplusplus
}
#endif


/* } __SOC_SAND_U64_H_INCLUDED__*/
#endif
