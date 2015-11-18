/* $Id: sand_64cnt.h,v 1.4 Broadcom SDK $
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


#ifndef __SOC_SAND_64CNT_H_INCLUDED__
/* { */
#define __SOC_SAND_64CNT_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>

/* $Id: sand_64cnt.h,v 1.4 Broadcom SDK $
 * 64 bit counter
 */
typedef struct
{
  /*
   * 64 bit number;
   */
  SOC_SAND_U64      u64;

  /*
   * Flag. If non-zero then 'high', specified above,
   * has overflown since it was last cleared.
   * Note:
   * Flag should be cleared after having been reported.
   */
  uint32   overflowed ;

} SOC_SAND_64CNT;


/*****************************************************
*NAME
* soc_sand_64cnt_clear
*TYPE:
*  PROC
*DATE:
*  9-Sep-03
*FUNCTION:
*  Clears all fields in the counter pointed by 'counter'.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_64CNT* counter -
*      Counter to clear.
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
  soc_sand_64cnt_clear(
    SOC_SAND_INOUT SOC_SAND_64CNT* counter
  );

/*****************************************************
*NAME
* soc_sand_64cnt_clear_ov
*TYPE:
*  PROC
*DATE:
*  9-Sep-03
*FUNCTION:
*  Clears Over-Flow field in the counter pointed by 'counter'.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_64CNT* counter -
*      Counter to clear.
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
  soc_sand_64cnt_clear_ov(
    SOC_SAND_INOUT SOC_SAND_64CNT* counter
  );

/*****************************************************
*NAME
* soc_sand_64cnt_add_long
*TYPE:
*  PROC
*DATE:
*  9-Sep-03
*FUNCTION:
*  Add uint32 value to counter.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_64CNT* counter -
*      Counter to add.
*    uint32 value_to_add -
*      32 bit value to add to the counter.
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
  soc_sand_64cnt_add_long(
    SOC_SAND_INOUT SOC_SAND_64CNT*   counter,
    SOC_SAND_IN    uint32 value_to_add
  );

/*****************************************************
*NAME
* soc_sand_64cnt_add_64cnt
*TYPE:
*  PROC
*DATE:
*  9-Sep-03
*FUNCTION:
*  Add long value to counter.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_64CNT* counter -
*      Counter to add.
*    uint32 value_to_add -
*      64 bit counter to add to the counter.
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
  soc_sand_64cnt_add_64cnt(
    SOC_SAND_INOUT SOC_SAND_64CNT* counter,
    SOC_SAND_IN    SOC_SAND_64CNT* value_to_add
  );

#if SOC_SAND_DEBUG
/* { */
/*
 */

/*****************************************************
*NAME
* soc_sand_64cnt_print
*TYPE:
*  PROC
*DATE:
*  9-Sep-03
*FUNCTION:
*  Print service to SOC_SAND_64CNT.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_64CNT* counter -
*      Counter to print.
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
  soc_sand_64cnt_print(
    SOC_SAND_IN SOC_SAND_64CNT*  counter,
    SOC_SAND_IN uint32 short_format
  );

/*****************************************************
*NAME
* soc_sand_64cnt_test
*TYPE:
*  PROC
*DATE:
*  9-Sep-03
*FUNCTION:
*  Test function of SOC_SAND_64CNT.
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
  soc_sand_64cnt_test(uint32 silent);

/*
 * }
 */
#endif


#ifdef  __cplusplus
}
#endif


/* } __SOC_SAND_64CNT_H_INCLUDED__*/
#endif
