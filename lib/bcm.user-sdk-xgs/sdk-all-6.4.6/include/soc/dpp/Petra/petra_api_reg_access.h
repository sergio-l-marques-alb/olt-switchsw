/* $Id: soc_petra_api_reg_access.h,v 1.5 Broadcom SDK $
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


#ifndef __SOC_PETRA_API_REG_ACCESS_H_INCLUDED__
/* { */
#define __SOC_PETRA_API_REG_ACCESS_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/TMC/tmc_api_reg_access.h>
#include <soc/dpp/Petra/petra_chip_regs.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: soc_petra_api_reg_access.h,v 1.5 Broadcom SDK $
 *  NIF Group B range and offset from Group A
 *  Used to calculate the offset to access using instance index
 */
#define SOC_PETRA_NIF_MAL_GRP_B_ADDR_MIN  (SOC_SAND_REG_SIZE_BYTES * 0x5200)
#define SOC_PETRA_NIF_MAL_GRP_B_ADDR_MAX  (SOC_SAND_REG_SIZE_BYTES * 0x5240)

#define SOC_PETRA_NIF_MAL_GRP_SIZE(unit) \
  (uint32)((soc_petra_sw_db_ver_get(unit) == SOC_PETRA_DEV_VER_A)? 4: 8)

#define SOC_PETRA_NIF_MAL_GRP_B_GAP(unit) \
  ((soc_petra_sw_db_ver_get(unit) == SOC_PETRA_DEV_VER_A)? (SOC_SAND_REG_SIZE_BYTES * 0x300): (SOC_SAND_REG_SIZE_BYTES * 0x200))

/*
 * calculate address from block base address, block index
 * and the index of that block (can be non-zero
 * for blocks with multiple instances.
 */
#define SOC_PETRA_GET_ADDR(_base_addr_, _step_, _idx_)  \
  ((_base_addr_) + (_step_)*(_idx_) + \
  ((SOC_SAND_IS_VAL_IN_RANGE(_base_addr_, SOC_PETRA_NIF_MAL_GRP_B_ADDR_MIN, SOC_PETRA_NIF_MAL_GRP_B_ADDR_MAX) \
  && (_idx_ >= SOC_PETRA_NIF_MAL_GRP_SIZE(unit)))?   \
  SOC_PETRA_NIF_MAL_GRP_B_GAP(unit) : 0))

#define SOC_PETRA_DEFAULT_INSTANCE   0xFF

/* } */

/*************
 *  MACROS   *
 *************/
/* { */
#define SOC_PETRA_POLL_DEF(poll_info_, expected_, bw_iters_, timer_iters_, timer_delay_) \
  {                                                                                  \
    soc_petra_PETRA_POLL_INFO_clear(&poll_info_);                                        \
    poll_info_.expected_value = expected_;                                           \
    poll_info_.busy_wait_nof_iters = bw_iters_;                                      \
    poll_info_.timer_nof_iters     = timer_iters_;                                   \
    poll_info_.timer_delay_msec    = timer_delay_;                                   \
  }
/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */
typedef SOC_TMC_POLL_INFO                                      SOC_PETRA_POLL_INFO;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*****************************************************
*NAME
*  soc_petra_read_fld
*TYPE:
*  PROC
*DATE:
*  07/08/2007
*FUNCTION:
*  Read a field from Soc_petra register
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  int      unit - device identifier
*    SOC_SAND_IN  SOC_PETRA_REG_FIELD*  field - pointer to the requested
*                               field descriptor in registers database
*    SOC_SAND_IN  uint32      instance_idx - if a block can have
*                               multiple instances at different offset,
*                               this is the index of the requested instance.
*                               For single instance - this value is 0.
*    SOC_SAND_OUT uint32*         val - return value.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    val - the value of the requested field.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_read_fld(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD*  field,
    SOC_SAND_IN  uint32      instance_idx,
    SOC_SAND_OUT uint32*         val
  );

/*****************************************************
*NAME
*  soc_petra_write_fld
*TYPE:
*  PROC
*DATE:
*  07/08/2007
*FUNCTION:
*  Write to a field of Soc_petra register
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  int      unit - device identifier
*    SOC_SAND_IN  SOC_PETRA_REG_FIELD*  field - pointer to the requested
*                               field descriptor in registers database
*    SOC_SAND_IN  uint32      instance_idx - if a block can have
*                               multiple instances at different offset,
*                               this is the index of the requested instance.
*                               For single instance - this value is 0.
*    SOC_SAND_OUT uint32          val - the value to write.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_write_fld(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD   *field,
    SOC_SAND_IN  uint32          instance_idx,
    SOC_SAND_IN  uint32           val
  );

/*****************************************************
*NAME
*  soc_petra_read_reg
*TYPE:
*  PROC
*DATE:
*  07/08/2007
*FUNCTION:
*  Read Soc_petra register
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  int      unit - device identifier
*    SOC_SAND_IN  SOC_PETRA_REG_ADDR*   reg - pointer to the requested
*                               register descriptor in registers database
*    SOC_SAND_IN  uint32      instance_idx - if a block can have
*                               multiple instances at different offset,
*                               this is the index of the requested instance.
*                               For single instance - this value is 0.
*    SOC_SAND_OUT uint32*         val - return value.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    val - the value of the requested register.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_read_reg(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  SOC_PETRA_REG_ADDR*   reg,
    SOC_SAND_IN  uint32      instance_idx,
    SOC_SAND_OUT uint32*         val
  );

/*****************************************************
*NAME
*  soc_petra_write_reg
*TYPE:
*  PROC
*DATE:
*  07/08/2007
*FUNCTION:
*  Write Soc_petra register
*INPUT:
*    SOC_SAND_IN  int      unit - device identifier
*    SOC_SAND_IN  SOC_PETRA_REG_ADDR*   reg - pointer to the requested
*                               register descriptor in registers database
*    SOC_SAND_IN  uint32      instance_idx - if a block can have
*                               multiple instances at different offset,
*                               this is the index of the requested instance.
*                               For single instance - this value is 0.
*    SOC_SAND_OUT uint32          val - the register value to write.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_write_reg(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  SOC_PETRA_REG_ADDR    *reg,
    SOC_SAND_IN  uint32          instance_idx,
    SOC_SAND_IN  uint32           val
  );

/*********************************************************************
* NAME:
*     soc_petra_status_fld_poll
* TYPE:
*   PROC
* FUNCTION:
*     Preform polling on the value of a given register.
*     According to the poll_info structure, the polling is
*     first performed as busy-wait, repeatedly reading the
*     register. Then, a polling is performed with the
*     requested timer delay between consecutive reads. The
*     'success' parameter returns TRUE if the expected value
*     is read, and FALSE if all the polling iterations do not
*     read the expected value (timeout).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_REG_FIELD           *field -
*     The field to read. The read value is compared to the
*     expected value on each polling iteration.
*   SOC_SAND_IN  uint32        instance_ndx -
*     For an instanced field (e.g. DRAM, MAL) - the instance index.
*     For a non-instanced field (single instance of the BLOCK exist),
*     must be SOC_PETRA_DEFAULT_INSTANCE.
*  SOC_SAND_IN  SOC_PETRA_POLL_INFO           *poll_info -
*     The polling parameters (expected value, number of
*     iterations, delay).
*  SOC_SAND_OUT uint8                 *success -
*     TRUE if the expected value is read. FALSE if timeout
*     occurs, and the expected value is not read.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_status_fld_poll(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD           *field,
    SOC_SAND_IN  uint32                 instance_ndx,
    SOC_SAND_IN  SOC_PETRA_POLL_INFO           *poll_info,
    SOC_SAND_OUT uint8                 *success
  );

void
  soc_petra_PETRA_POLL_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_POLL_INFO *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1
void
  soc_petra_PETRA_POLL_INFO_print(
    SOC_SAND_IN SOC_PETRA_POLL_INFO *info
  );

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_API_REG_ACCESS_H_INCLUDED__*/
#endif
