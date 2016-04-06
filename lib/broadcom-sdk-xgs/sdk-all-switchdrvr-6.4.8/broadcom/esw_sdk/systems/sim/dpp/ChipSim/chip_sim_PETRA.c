/* $Id: chip_sim_PETRA.c,v 1.8 Broadcom SDK $
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

/*include SOC_PETRAN and PP*/


#ifdef _MSC_VER
  /*
   * On MS-Windows platform this attribute is not defined.
   */
  #define __ATTRIBUTE_PACKED__
  #pragma pack(push)
  #pragma pack(1)

#elif __GNUC__
    /*
     * GNUC packing attribute
     */
    #define __ATTRIBUTE_PACKED__  __attribute__ ((packed))
#else
    #error  "Add your system support for packed attribute."
#endif

/*************
 * INCLUDES  *
 *************/
/* { */
#include "chip_sim.h"
#include "chip_sim_counter.h"
#include "chip_sim_interrupts.h"
#include "chip_sim_indirect.h"
#include "chip_sim_PETRA.h"
#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>
#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */
CHIP_SIM_INDIRECT_BLOCK
  Soc_petra_indirect_blocks[SOC_PETRA_TBL_ID_LAST + 1];
CHIP_SIM_COUNTER
  Soc_petra_counters[2];
CHIP_SIM_INTERRUPT
  Soc_petra_interrupts[1];
uint8
  Soc_petra_init_device_values = TRUE;
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */
/*****************************************************
*NAME
* soc_petra_indirect_blocks_init
*TYPE:
*  PROC
*DATE:
*  10/24/07
*FUNCTION:
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_petra_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
STATIC void
  soc_petra_indirect_blocks_init(
  )
{
  uint32
    indx;
  SOC_PETRA_REGS
    *regs = NULL;

#ifdef BCM_PETRAB_SUPPORT
  uint32
    err = SOC_SAND_OK;

  err =
    soc_petra_regs_get(
      &(regs)
    );
#endif /* BCM_PETRAB_SUPPORT */

  sal_memset(Soc_petra_indirect_blocks, 0x0, sizeof(CHIP_SIM_INDIRECT_BLOCK) * SOC_PETRA_TBL_ID_LAST);

  Soc_petra_indirect_blocks[SOC_PETRA_OLP_PGE_MEM_TBL_ID].read_result_address =
    regs->a_regs->olp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->olp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_OLP_PGE_MEM_TBL_ID].access_trig_offset =
    regs->a_regs->olp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->olp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_OLP_PGE_MEM_TBL_ID].access_address_offset =
    regs->a_regs->olp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->olp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_OLP_PGE_MEM_TBL_ID].write_val_offset =
    regs->a_regs->olp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->olp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_OLP_PGE_MEM_TBL_ID].start_address =
    ~SOC_PETRA_OLP_MASK & 0x00010000;
  Soc_petra_indirect_blocks[SOC_PETRA_OLP_PGE_MEM_TBL_ID].end_address =
    (~SOC_PETRA_OLP_MASK & 0x00010000) + 0x0020 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_OLP_PGE_MEM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_OLP_PGE_MEM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_ID].read_result_address =
    regs->a_regs->ire.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ire.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_ID].access_trig_offset =
    regs->a_regs->ire.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ire.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_ID].access_address_offset =
    regs->a_regs->ire.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ire.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_ID].write_val_offset =
    regs->a_regs->ire.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ire.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_ID].start_address =
    ~SOC_PETRA_IRE_MASK & 0x00010000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_ID].end_address =
    (~SOC_PETRA_IRE_MASK & 0x00010000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ID].read_result_address =
    regs->a_regs->ire.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ire.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ID].access_trig_offset =
    regs->a_regs->ire.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ire.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ID].access_address_offset =
    regs->a_regs->ire.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ire.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ID].write_val_offset =
    regs->a_regs->ire.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ire.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ID].start_address =
    ~SOC_PETRA_IRE_MASK & 0x00020000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ID].end_address =
    (~SOC_PETRA_IRE_MASK & 0x00020000) + 0x0008 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_ID].read_result_address =
    regs->a_regs->ire.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ire.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_ID].access_trig_offset =
    regs->a_regs->ire.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ire.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_ID].access_address_offset =
    regs->a_regs->ire.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ire.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_ID].write_val_offset =
    regs->a_regs->ire.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ire.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_ID].start_address =
    ~SOC_PETRA_IRE_MASK & 0x00030000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_ID].end_address =
    (~SOC_PETRA_IRE_MASK & 0x00030000) + 0x0040 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRE_CPU_PACKET_COUNTER_TBL_ID].read_result_address =
    regs->a_regs->ire.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ire.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_CPU_PACKET_COUNTER_TBL_ID].access_trig_offset =
    regs->a_regs->ire.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ire.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_CPU_PACKET_COUNTER_TBL_ID].access_address_offset =
    regs->a_regs->ire.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ire.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_CPU_PACKET_COUNTER_TBL_ID].write_val_offset =
    regs->a_regs->ire.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ire.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_CPU_PACKET_COUNTER_TBL_ID].start_address =
    ~SOC_PETRA_IRE_MASK & 0x00040000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_CPU_PACKET_COUNTER_TBL_ID].end_address =
    (~SOC_PETRA_IRE_MASK & 0x00040000) + 0x1 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_CPU_PACKET_COUNTER_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRE_CPU_PACKET_COUNTER_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRE_OLP_PACKET_COUNTER_TBL_ID].read_result_address =
    regs->a_regs->ire.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ire.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_OLP_PACKET_COUNTER_TBL_ID].access_trig_offset =
    regs->a_regs->ire.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ire.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_OLP_PACKET_COUNTER_TBL_ID].access_address_offset =
    regs->a_regs->ire.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ire.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_OLP_PACKET_COUNTER_TBL_ID].write_val_offset =
    regs->a_regs->ire.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ire.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_OLP_PACKET_COUNTER_TBL_ID].start_address =
    ~SOC_PETRA_IRE_MASK & 0x00050000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_OLP_PACKET_COUNTER_TBL_ID].end_address =
    (~SOC_PETRA_IRE_MASK & 0x00050000) + 0x1 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_OLP_PACKET_COUNTER_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRE_OLP_PACKET_COUNTER_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIFA_PACKET_COUNTER_TBL_ID].read_result_address =
    regs->a_regs->ire.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ire.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIFA_PACKET_COUNTER_TBL_ID].access_trig_offset =
    regs->a_regs->ire.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ire.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIFA_PACKET_COUNTER_TBL_ID].access_address_offset =
    regs->a_regs->ire.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ire.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIFA_PACKET_COUNTER_TBL_ID].write_val_offset =
    regs->a_regs->ire.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ire.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIFA_PACKET_COUNTER_TBL_ID].start_address =
    ~SOC_PETRA_IRE_MASK & 0x00060000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIFA_PACKET_COUNTER_TBL_ID].end_address =
    (~SOC_PETRA_IRE_MASK & 0x00060000) + 0x1 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIFA_PACKET_COUNTER_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIFA_PACKET_COUNTER_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIFB_PACKET_COUNTER_TBL_ID].read_result_address =
    regs->a_regs->ire.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ire.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIFB_PACKET_COUNTER_TBL_ID].access_trig_offset =
    regs->a_regs->ire.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ire.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIFB_PACKET_COUNTER_TBL_ID].access_address_offset =
    regs->a_regs->ire.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ire.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIFB_PACKET_COUNTER_TBL_ID].write_val_offset =
    regs->a_regs->ire.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ire.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIFB_PACKET_COUNTER_TBL_ID].start_address =
    ~SOC_PETRA_IRE_MASK & 0x00070000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIFB_PACKET_COUNTER_TBL_ID].end_address =
    (~SOC_PETRA_IRE_MASK & 0x00070000) + 0x1 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIFB_PACKET_COUNTER_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRE_NIFB_PACKET_COUNTER_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRE_RCY_PACKET_COUNTER_TBL_ID].read_result_address =
    regs->a_regs->ire.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ire.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_RCY_PACKET_COUNTER_TBL_ID].access_trig_offset =
    regs->a_regs->ire.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ire.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_RCY_PACKET_COUNTER_TBL_ID].access_address_offset =
    regs->a_regs->ire.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ire.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_RCY_PACKET_COUNTER_TBL_ID].write_val_offset =
    regs->a_regs->ire.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ire.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_RCY_PACKET_COUNTER_TBL_ID].start_address =
    ~SOC_PETRA_IRE_MASK & 0x00080000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_RCY_PACKET_COUNTER_TBL_ID].end_address =
    (~SOC_PETRA_IRE_MASK & 0x00080000) + 0x1 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRE_RCY_PACKET_COUNTER_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRE_RCY_PACKET_COUNTER_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IDR_COMPLETE_PC_TBL_ID].read_result_address =
    regs->a_regs->idr.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->idr.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IDR_COMPLETE_PC_TBL_ID].access_trig_offset =
    regs->a_regs->idr.indirect_command_reg.addr.base +
    0 * (regs->a_regs->idr.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IDR_COMPLETE_PC_TBL_ID].access_address_offset =
    regs->a_regs->idr.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->idr.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IDR_COMPLETE_PC_TBL_ID].write_val_offset =
    regs->a_regs->idr.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->idr.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IDR_COMPLETE_PC_TBL_ID].start_address =
    ~SOC_PETRA_IDR_MASK & 0x00100000;
  Soc_petra_indirect_blocks[SOC_PETRA_IDR_COMPLETE_PC_TBL_ID].end_address =
    (~SOC_PETRA_IDR_MASK & 0x00100000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IDR_COMPLETE_PC_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IDR_COMPLETE_PC_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_ID].read_result_address =
    regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_ID].access_trig_offset =
    regs->a_regs->irr.indirect_command_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_ID].access_address_offset =
    regs->a_regs->irr.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_ID].write_val_offset =
    regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_ID].start_address =
    ~SOC_PETRA_IRR_MASK & 0x00010000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_ID].end_address =
    (~SOC_PETRA_IRR_MASK & 0x00010000) + 0x1000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRR_MIRROR_TABLE_TBL_ID].read_result_address =
    regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_MIRROR_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->irr.indirect_command_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_MIRROR_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->irr.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_MIRROR_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_MIRROR_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IRR_MASK & 0x00030000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_MIRROR_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IRR_MASK & 0x00030000) + 0x0060 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_MIRROR_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRR_MIRROR_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRR_SNOOP_TABLE_TBL_ID].read_result_address =
    regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_SNOOP_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->irr.indirect_command_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_SNOOP_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->irr.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_SNOOP_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_SNOOP_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IRR_MASK & 0x00060000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_SNOOP_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IRR_MASK & 0x00060000) + 0x0010 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_SNOOP_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRR_SNOOP_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_ID].read_result_address =
    regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_ID].access_trig_offset =
    regs->a_regs->irr.indirect_command_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_ID].access_address_offset =
    regs->a_regs->irr.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_ID].write_val_offset =
    regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_ID].start_address =
    ~SOC_PETRA_IRR_MASK & 0x000f0000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_ID].end_address =
    (~SOC_PETRA_IRR_MASK & 0x000f0000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_ID].read_result_address =
    regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_ID].access_trig_offset =
    regs->a_regs->irr.indirect_command_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_ID].access_address_offset =
    regs->a_regs->irr.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_ID].write_val_offset =
    regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_ID].start_address =
    ~SOC_PETRA_IRR_MASK & 0x00100000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_ID].end_address =
    (~SOC_PETRA_IRR_MASK & 0x00100000) + 0x1000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_MAPPING_TBL_ID].read_result_address =
    regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_MAPPING_TBL_ID].access_trig_offset =
    regs->a_regs->irr.indirect_command_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_MAPPING_TBL_ID].access_address_offset =
    regs->a_regs->irr.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_MAPPING_TBL_ID].write_val_offset =
    regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_MAPPING_TBL_ID].start_address =
    ~SOC_PETRA_IRR_MASK & 0x00200000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_MAPPING_TBL_ID].end_address =
    (~SOC_PETRA_IRR_MASK & 0x00200000) + 0x1000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_MAPPING_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_MAPPING_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRR_DESTINATION_TABLE_TBL_ID].read_result_address =
    regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_DESTINATION_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->irr.indirect_command_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_DESTINATION_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->irr.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_DESTINATION_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_DESTINATION_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IRR_MASK & 0x00300000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_DESTINATION_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IRR_MASK & 0x00300000) + 0x1000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_DESTINATION_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRR_DESTINATION_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_ID].read_result_address =
    regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_ID].access_trig_offset =
    regs->a_regs->irr.indirect_command_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_ID].access_address_offset =
    regs->a_regs->irr.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_ID].write_val_offset =
    regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_ID].start_address =
    ~SOC_PETRA_IRR_MASK & 0x00400000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_ID].end_address =
    (~SOC_PETRA_IRR_MASK & 0x00400000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_ID].read_result_address =
    regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_ID].access_trig_offset =
    regs->a_regs->irr.indirect_command_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_ID].access_address_offset =
    regs->a_regs->irr.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_ID].write_val_offset =
    regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_ID].start_address =
    ~SOC_PETRA_IRR_MASK & 0x00500000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_ID].end_address =
    (~SOC_PETRA_IRR_MASK & 0x00500000) + 0x0400 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x000d0000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x000d0000) + 0x0001 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_INFO_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_INFO_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_INFO_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_INFO_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_INFO_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x000e0000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_INFO_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x000e0000) + 0x0050 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_INFO_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_INFO_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_ETH_PORT_INFO1_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_ETH_PORT_INFO1_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_ETH_PORT_INFO1_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_ETH_PORT_INFO1_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_ETH_PORT_INFO1_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00100000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_ETH_PORT_INFO1_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00100000) + 0x0050 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_ETH_PORT_INFO1_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_ETH_PORT_INFO1_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_ETH_PORT_INFO2_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_ETH_PORT_INFO2_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_ETH_PORT_INFO2_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_ETH_PORT_INFO2_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_ETH_PORT_INFO2_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00110000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_ETH_PORT_INFO2_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00110000) + 0x0050 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_ETH_PORT_INFO2_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_ETH_PORT_INFO2_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00120000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00120000) + 0x0050 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_STATIC_HEADER_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_STATIC_HEADER_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_STATIC_HEADER_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_STATIC_HEADER_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_STATIC_HEADER_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00130000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_STATIC_HEADER_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00130000) + 0x0050 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_STATIC_HEADER_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_STATIC_HEADER_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00140000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00140000) + 0x0400 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_INFO_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_INFO_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_INFO_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_INFO_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_INFO_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00160000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_INFO_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00160000) + 0x1000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_INFO_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_INFO_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_TO_TOPOLOGY_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_TO_TOPOLOGY_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_TO_TOPOLOGY_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_TO_TOPOLOGY_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_TO_TOPOLOGY_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00170000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_TO_TOPOLOGY_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00170000) + 0x1000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_TO_TOPOLOGY_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_TO_TOPOLOGY_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_STP_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_STP_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_STP_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_STP_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_STP_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00180000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_STP_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00180000) + 0x2000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_STP_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_STP_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00190000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00190000) + 0x0050 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_INDEX_TO_CID_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_INDEX_TO_CID_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_INDEX_TO_CID_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_INDEX_TO_CID_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_INDEX_TO_CID_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x001a0000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_INDEX_TO_CID_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x001a0000) + 0x0020 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_INDEX_TO_CID_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_INDEX_TO_CID_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x001e0000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x001e0000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00210000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00210000) + 0x0010 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_PORT_MEMBERSHIP_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_PORT_MEMBERSHIP_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_PORT_MEMBERSHIP_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_PORT_MEMBERSHIP_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_PORT_MEMBERSHIP_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00220000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_PORT_MEMBERSHIP_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00220000) + 0x1000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_PORT_MEMBERSHIP_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_PORT_MEMBERSHIP_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IS_CID_SHARED_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IS_CID_SHARED_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IS_CID_SHARED_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IS_CID_SHARED_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IS_CID_SHARED_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00230000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IS_CID_SHARED_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00230000) + 0x0080 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IS_CID_SHARED_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_IS_CID_SHARED_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TOS_TO_TC_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TOS_TO_TC_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TOS_TO_TC_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TOS_TO_TC_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TOS_TO_TC_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00240000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TOS_TO_TC_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00240000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TOS_TO_TC_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_TOS_TO_TC_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_EVENT_FIFO_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_EVENT_FIFO_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_EVENT_FIFO_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_EVENT_FIFO_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_EVENT_FIFO_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00280000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_EVENT_FIFO_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00280000) + 0x0001 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_EVENT_FIFO_TBL_ID].nof_longs_to_move =
    4;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_EVENT_FIFO_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_FEC_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_FEC_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_FEC_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_FEC_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_FEC_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00290000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_FEC_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00290000) + 0x2000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_FEC_TABLE_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_FEC_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_SMOOTH_DIVISION_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_SMOOTH_DIVISION_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_SMOOTH_DIVISION_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_SMOOTH_DIVISION_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_SMOOTH_DIVISION_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x002a0000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_SMOOTH_DIVISION_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x002a0000) + 0x1000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_SMOOTH_DIVISION_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_SMOOTH_DIVISION_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH0_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH0_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH0_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH0_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH0_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x002b0000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH0_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x002b0000) + 0x1000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH0_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH0_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH1_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH1_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH1_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH1_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH1_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x002c0000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH1_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x002c0000) + 0x6000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH1_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH1_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH2_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH2_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH2_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH2_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH2_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x002d0000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH2_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x002d0000) + 0x6000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH2_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_LONGEST_PREFIX_MATCH2_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_A_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_A_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_A_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_A_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_A_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x002e0000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_A_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x002e0000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_A_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_A_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_B_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_B_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_B_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_B_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_B_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x002f0000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_B_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x002f0000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_B_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_B_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_C_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_C_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_C_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_C_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_C_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00300000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_C_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00300000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_C_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_C_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_D_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_D_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_D_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_D_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_D_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00310000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_D_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00310000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_D_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_ACTION_BANK_D_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TOS_TO_COS_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TOS_TO_COS_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TOS_TO_COS_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TOS_TO_COS_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TOS_TO_COS_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00320000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TOS_TO_COS_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00320000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TOS_TO_COS_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_TOS_TO_COS_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_FEC_ACCESSED_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_FEC_ACCESSED_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_FEC_ACCESSED_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_FEC_ACCESSED_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_FEC_ACCESSED_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00330000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_FEC_ACCESSED_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00330000) + 0x0800 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_FEC_ACCESSED_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_FEC_ACCESSED_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IPV4_STAT_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IPV4_STAT_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IPV4_STAT_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IPV4_STAT_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IPV4_STAT_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00340000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IPV4_STAT_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00340000) + 0x0020 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IPV4_STAT_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_IPV4_STAT_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_A_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_A_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_A_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_A_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_A_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00400000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_A_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00400000) + 0x0002 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_A_TBL_ID].nof_longs_to_move =
    10;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_A_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_B_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_B_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_B_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_B_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_B_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00410000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_B_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00410000) + 0x0002 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_B_TBL_ID].nof_longs_to_move =
    10;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_B_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_C_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_C_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_C_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_C_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_C_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00420000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_C_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00420000) + 0x0002 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_C_TBL_ID].nof_longs_to_move =
    10;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_C_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_D_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_D_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_D_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_D_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_D_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00430000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_D_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00430000) + 0x0002 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_D_TBL_ID].nof_longs_to_move =
    10;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_TCAM_BANK_D_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IPV6_TC_TO_COS_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IPV6_TC_TO_COS_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IPV6_TC_TO_COS_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IPV6_TC_TO_COS_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IPV6_TC_TO_COS_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00440000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IPV6_TC_TO_COS_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00440000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_IPV6_TC_TO_COS_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_IPV6_TC_TO_COS_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_COMMANDS1_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_COMMANDS1_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_COMMANDS1_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_COMMANDS1_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_COMMANDS1_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00500000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_COMMANDS1_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00500000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_COMMANDS1_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_COMMANDS1_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_COMMANDS2_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_COMMANDS2_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_COMMANDS2_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_COMMANDS2_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_COMMANDS2_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00510000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_COMMANDS2_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00510000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_COMMANDS2_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_COMMANDS2_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00520000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00520000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM0_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM0_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM0_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM0_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM0_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00530000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM0_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00530000) + 0x0040 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM0_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM0_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM1_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM1_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM1_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM1_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM1_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00540000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM1_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00540000) + 0x0040 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM1_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM1_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM2_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM2_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM2_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM2_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM2_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00550000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM2_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00550000) + 0x0040 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM2_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM2_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM3_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM3_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM3_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM3_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM3_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00560000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM3_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00560000) + 0x0040 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM3_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM3_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM4_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM4_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM4_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM4_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM4_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00570000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM4_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00570000) + 0x0040 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM4_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_KEY_PROGRAM4_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00580000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00580000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_SUBNET_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_SUBNET_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_SUBNET_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_SUBNET_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_SUBNET_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00600000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_SUBNET_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00600000) + 0x0010 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_SUBNET_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_CID_SUBNET_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00610000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00610000) + 0x0010 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_ID].read_result_address =
    regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_ID].access_trig_offset =
    regs->a_regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_ID].access_address_offset =
    regs->a_regs->ihp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_ID].write_val_offset =
    regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ihp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_ID].start_address =
    ~SOC_PETRA_IHP_MASK & 0x00620000;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_ID].end_address =
    (~SOC_PETRA_IHP_MASK & 0x00620000) + 0x0007 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_BDB_LINK_LIST_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_BDB_LINK_LIST_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_BDB_LINK_LIST_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_BDB_LINK_LIST_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_BDB_LINK_LIST_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x00000000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_BDB_LINK_LIST_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x00000000) + 0x10000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_BDB_LINK_LIST_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_BDB_LINK_LIST_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_DYNAMIC_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_DYNAMIC_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_DYNAMIC_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_DYNAMIC_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_DYNAMIC_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x00100000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_DYNAMIC_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x00100000) + 0x8000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_DYNAMIC_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_DYNAMIC_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_STATIC_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_STATIC_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_STATIC_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_STATIC_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_STATIC_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x00200000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_STATIC_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x00200000) + 0x8000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_STATIC_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_STATIC_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x00300000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x00300000) + 0x8000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x00400000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x00400000) + 0x0040 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x00500000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x00500000) + 0x0010 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x00600000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x00600000) + 0x2000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x00700000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x00700000) + 0x0800 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x00800000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x00800000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x01100000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x01100000) + 0x0004 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x01200000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x01200000) + 0x0020 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x01300000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x01300000) + 0x0040 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x01400000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x01400000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x01500000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x01500000) + 0x0004 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x01600000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x01600000) + 0x0020 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x01700000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x01700000) + 0x0040 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x01800000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x01800000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x01900000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x01900000) + 0x0004 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x01a00000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x01a00000) + 0x0020 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x01b00000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x01b00000) + 0x0040 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x01c00000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x01c00000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x01d00000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x01d00000) + 0x0010 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x01e00000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x01e00000) + 0x0010 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x01f00000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x01f00000) + 0x0010 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x02000000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x02000000) + 0x0010 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x02100000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x02100000) + 0x0040 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x02200000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x02200000) + 0x0040 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x02300000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x02300000) + 0x0040 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x02400000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x02400000) + 0x0040 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x02500000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x02500000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x02600000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x02600000) + 0x0010 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_TBL_ID].read_result_address =
    regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_TBL_ID].access_trig_offset =
    regs->a_regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_TBL_ID].access_address_offset =
    regs->a_regs->iqm.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_TBL_ID].write_val_offset =
    regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->iqm.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_TBL_ID].start_address =
    ~SOC_PETRA_IQM_MASK & 0x02700000;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_TBL_ID].end_address =
    (~SOC_PETRA_IQM_MASK & 0x02700000) + 0x0040 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_TBL_ID].nof_longs_to_move =
    6;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IQM_SYSTEM_RED_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ips.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ips.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IPS_MASK & 0x00000000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IPS_MASK & 0x00000000) + 0x2000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ips.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ips.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IPS_MASK & 0x00008000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IPS_MASK & 0x00008000) + 0x1000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ips.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ips.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IPS_MASK & 0x00010000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IPS_MASK & 0x00010000) + 0x2000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ips.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ips.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IPS_MASK & 0x00018000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IPS_MASK & 0x00018000) + 0x8000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID].read_result_address =
    regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID].access_trig_offset =
    regs->a_regs->ips.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID].access_address_offset =
    regs->a_regs->ips.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID].write_val_offset =
    regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID].start_address =
    ~SOC_PETRA_IPS_MASK & 0x00020000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID].end_address =
    (~SOC_PETRA_IPS_MASK & 0x00020000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ips.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ips.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IPS_MASK & 0x00028000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IPS_MASK & 0x00028000) + 0x0004 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ips.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ips.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IPS_MASK & 0x00030000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IPS_MASK & 0x00030000) + 0x0010 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ips.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ips.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IPS_MASK & 0x00038000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IPS_MASK & 0x00038000) + 0x0010 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ips.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ips.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IPS_MASK & 0x00040000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IPS_MASK & 0x00040000) + 0x0010 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ips.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ips.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IPS_MASK & 0x00048000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IPS_MASK & 0x00048000) + 0x0010 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ips.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ips.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IPS_MASK & 0x00050000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IPS_MASK & 0x00050000) + 0x8000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ips.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ips.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IPS_MASK & 0x00058000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IPS_MASK & 0x00058000) + 0x8000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID].read_result_address =
    regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->ips.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->ips.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ips.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ips.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_IPS_MASK & 0x00078000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_IPS_MASK & 0x00078000) + 0x1000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPT_BDQ_TBL_ID].read_result_address =
    regs->a_regs->ipt.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_BDQ_TBL_ID].access_trig_offset =
    regs->a_regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_BDQ_TBL_ID].access_address_offset =
    regs->a_regs->ipt.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_BDQ_TBL_ID].write_val_offset =
    regs->a_regs->ipt.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_BDQ_TBL_ID].start_address =
    ~SOC_PETRA_IPT_MASK & 0x00010000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_BDQ_TBL_ID].end_address =
    (~SOC_PETRA_IPT_MASK & 0x00010000) + 0x1000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_BDQ_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPT_BDQ_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPT_PCQ_TBL_ID].read_result_address =
    regs->a_regs->ipt.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_PCQ_TBL_ID].access_trig_offset =
    regs->a_regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_PCQ_TBL_ID].access_address_offset =
    regs->a_regs->ipt.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_PCQ_TBL_ID].write_val_offset =
    regs->a_regs->ipt.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_PCQ_TBL_ID].start_address =
    ~SOC_PETRA_IPT_MASK & 0x00020000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_PCQ_TBL_ID].end_address =
    (~SOC_PETRA_IPT_MASK & 0x00020000) + 0x1000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_PCQ_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPT_PCQ_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPT_SOP_MMU_TBL_ID].read_result_address =
    regs->a_regs->ipt.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_SOP_MMU_TBL_ID].access_trig_offset =
    regs->a_regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_SOP_MMU_TBL_ID].access_address_offset =
    regs->a_regs->ipt.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_SOP_MMU_TBL_ID].write_val_offset =
    regs->a_regs->ipt.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_SOP_MMU_TBL_ID].start_address =
    ~SOC_PETRA_IPT_MASK & 0x00030000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_SOP_MMU_TBL_ID].end_address =
    (~SOC_PETRA_IPT_MASK & 0x00030000) + 0x0600 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_SOP_MMU_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPT_SOP_MMU_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPT_MOP_MMU_TBL_ID].read_result_address =
    regs->a_regs->ipt.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_MOP_MMU_TBL_ID].access_trig_offset =
    regs->a_regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_MOP_MMU_TBL_ID].access_address_offset =
    regs->a_regs->ipt.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_MOP_MMU_TBL_ID].write_val_offset =
    regs->a_regs->ipt.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_MOP_MMU_TBL_ID].start_address =
    ~SOC_PETRA_IPT_MASK & 0x00040000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_MOP_MMU_TBL_ID].end_address =
    (~SOC_PETRA_IPT_MASK & 0x00040000) + 0x0600 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_MOP_MMU_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPT_MOP_MMU_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPT_FDTCTL_TBL_ID].read_result_address =
    regs->a_regs->ipt.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_FDTCTL_TBL_ID].access_trig_offset =
    regs->a_regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_FDTCTL_TBL_ID].access_address_offset =
    regs->a_regs->ipt.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_FDTCTL_TBL_ID].write_val_offset =
    regs->a_regs->ipt.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_FDTCTL_TBL_ID].start_address =
    ~SOC_PETRA_IPT_MASK & 0x00050000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_FDTCTL_TBL_ID].end_address =
    (~SOC_PETRA_IPT_MASK & 0x00050000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_FDTCTL_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPT_FDTCTL_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPT_FDTDATA_TBL_ID].read_result_address =
    regs->a_regs->ipt.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_FDTDATA_TBL_ID].access_trig_offset =
    regs->a_regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_FDTDATA_TBL_ID].access_address_offset =
    regs->a_regs->ipt.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_FDTDATA_TBL_ID].write_val_offset =
    regs->a_regs->ipt.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_FDTDATA_TBL_ID].start_address =
    ~SOC_PETRA_IPT_MASK & 0x00060000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_FDTDATA_TBL_ID].end_address =
    (~SOC_PETRA_IPT_MASK & 0x00060000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_FDTDATA_TBL_ID].nof_longs_to_move =
    16;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPT_FDTDATA_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPT_EGQCTL_TBL_ID].read_result_address =
    regs->a_regs->ipt.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_EGQCTL_TBL_ID].access_trig_offset =
    regs->a_regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_EGQCTL_TBL_ID].access_address_offset =
    regs->a_regs->ipt.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_EGQCTL_TBL_ID].write_val_offset =
    regs->a_regs->ipt.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_EGQCTL_TBL_ID].start_address =
    ~SOC_PETRA_IPT_MASK & 0x00070000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_EGQCTL_TBL_ID].end_address =
    (~SOC_PETRA_IPT_MASK & 0x00070000) + 0x0020 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_EGQCTL_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPT_EGQCTL_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IPT_EGQDATA_TBL_ID].read_result_address =
    regs->a_regs->ipt.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_EGQDATA_TBL_ID].access_trig_offset =
    regs->a_regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_EGQDATA_TBL_ID].access_address_offset =
    regs->a_regs->ipt.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_EGQDATA_TBL_ID].write_val_offset =
    regs->a_regs->ipt.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->ipt.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_EGQDATA_TBL_ID].start_address =
    ~SOC_PETRA_IPT_MASK & 0x00080000;
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_EGQDATA_TBL_ID].end_address =
    (~SOC_PETRA_IPT_MASK & 0x00080000) + 0x0020 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IPT_EGQDATA_TBL_ID].nof_longs_to_move =
    16;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IPT_EGQDATA_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_A_ID].read_result_address =
    regs->a_regs->dpi.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->dpi.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_A_ID].access_trig_offset =
    regs->a_regs->dpi.indirect_command_reg.addr.base +
    0 * (regs->a_regs->dpi.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_A_ID].access_address_offset =
    regs->a_regs->dpi.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->dpi.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_A_ID].write_val_offset =
    regs->a_regs->dpi.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->dpi.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_A_ID].start_address =
    ~SOC_PETRA_DPI_A_MASK & 0x00010000;
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_A_ID].end_address =
    (~SOC_PETRA_DPI_A_MASK & 0x00010000) + 0x0080 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_A_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_A_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_B_ID].read_result_address =
    regs->a_regs->dpi.indirect_command_rd_data_reg_0.addr.base +
    1 * (regs->a_regs->dpi.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_B_ID].access_trig_offset =
    regs->a_regs->dpi.indirect_command_reg.addr.base +
    1 * (regs->a_regs->dpi.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_B_ID].access_address_offset =
    regs->a_regs->dpi.indirect_command_address_reg.addr.base +
    1 * (regs->a_regs->dpi.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_B_ID].write_val_offset =
    regs->a_regs->dpi.indirect_command_wr_data_reg_0.addr.base +
    1 * (regs->a_regs->dpi.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_B_ID].start_address =
    ~SOC_PETRA_DPI_B_MASK & 0x00010000;
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_B_ID].end_address =
    (~SOC_PETRA_DPI_B_MASK & 0x00010000) + 0x0080 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_B_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_B_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_C_ID].read_result_address =
    regs->a_regs->dpi.indirect_command_rd_data_reg_0.addr.base +
    2 * (regs->a_regs->dpi.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_C_ID].access_trig_offset =
    regs->a_regs->dpi.indirect_command_reg.addr.base +
    2 * (regs->a_regs->dpi.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_C_ID].access_address_offset =
    regs->a_regs->dpi.indirect_command_address_reg.addr.base +
    2 * (regs->a_regs->dpi.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_C_ID].write_val_offset =
    regs->a_regs->dpi.indirect_command_wr_data_reg_0.addr.base +
    2 * (regs->a_regs->dpi.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_C_ID].start_address =
    ~SOC_PETRA_DPI_C_MASK & 0x00010000;
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_C_ID].end_address =
    (~SOC_PETRA_DPI_C_MASK & 0x00010000) + 0x0080 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_C_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_C_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_D_ID].read_result_address =
    regs->a_regs->dpi.indirect_command_rd_data_reg_0.addr.base +
    3 * (regs->a_regs->dpi.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_D_ID].access_trig_offset =
    regs->a_regs->dpi.indirect_command_reg.addr.base +
    3 * (regs->a_regs->dpi.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_D_ID].access_address_offset =
    regs->a_regs->dpi.indirect_command_address_reg.addr.base +
    3 * (regs->a_regs->dpi.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_D_ID].write_val_offset =
    regs->a_regs->dpi.indirect_command_wr_data_reg_0.addr.base +
    3 * (regs->a_regs->dpi.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_D_ID].start_address =
    ~SOC_PETRA_DPI_D_MASK & 0x00010000;
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_D_ID].end_address =
    (~SOC_PETRA_DPI_D_MASK & 0x00010000) + 0x0080 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_D_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_D_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_E_ID].read_result_address =
    regs->a_regs->dpi.indirect_command_rd_data_reg_0.addr.base +
    4 * (regs->a_regs->dpi.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_E_ID].access_trig_offset =
    regs->a_regs->dpi.indirect_command_reg.addr.base +
    4 * (regs->a_regs->dpi.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_E_ID].access_address_offset =
    regs->a_regs->dpi.indirect_command_address_reg.addr.base +
    4 * (regs->a_regs->dpi.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_E_ID].write_val_offset =
    regs->a_regs->dpi.indirect_command_wr_data_reg_0.addr.base +
    4 * (regs->a_regs->dpi.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_E_ID].start_address =
    ~SOC_PETRA_DPI_E_MASK & 0x00010000;
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_E_ID].end_address =
    (~SOC_PETRA_DPI_E_MASK & 0x00010000) + 0x0080 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_E_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_E_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_F_ID].read_result_address =
    regs->a_regs->dpi.indirect_command_rd_data_reg_0.addr.base +
    5 * (regs->a_regs->dpi.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_F_ID].access_trig_offset =
    regs->a_regs->dpi.indirect_command_reg.addr.base +
    5 * (regs->a_regs->dpi.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_F_ID].access_address_offset =
    regs->a_regs->dpi.indirect_command_address_reg.addr.base +
    5 * (regs->a_regs->dpi.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_F_ID].write_val_offset =
    regs->a_regs->dpi.indirect_command_wr_data_reg_0.addr.base +
    5 * (regs->a_regs->dpi.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_F_ID].start_address =
    ~SOC_PETRA_DPI_F_MASK & 0x00010000;
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_F_ID].end_address =
    (~SOC_PETRA_DPI_F_MASK & 0x00010000) + 0x0080 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_F_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_DPI_DLL_RAM_TBL_F_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID].read_result_address =
    regs->a_regs->rtp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->rtp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID].access_trig_offset =
    regs->a_regs->rtp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->rtp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID].access_address_offset =
    regs->a_regs->rtp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->rtp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID].write_val_offset =
    regs->a_regs->rtp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->rtp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID].start_address =
    ~SOC_PETRA_RTP_MASK & 0x00000000;
  Soc_petra_indirect_blocks[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID].end_address =
    (~SOC_PETRA_RTP_MASK & 0x00000000) + 0x0800 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID].read_result_address =
    regs->a_regs->rtp.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->rtp.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID].access_trig_offset =
    regs->a_regs->rtp.indirect_command_reg.addr.base +
    0 * (regs->a_regs->rtp.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID].access_address_offset =
    regs->a_regs->rtp.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->rtp.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID].write_val_offset =
    regs->a_regs->rtp.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->rtp.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID].start_address =
    ~SOC_PETRA_RTP_MASK & 0x00001000;
  Soc_petra_indirect_blocks[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID].end_address =
    (~SOC_PETRA_RTP_MASK & 0x00001000) + 0x0800 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH0_SCM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH0_SCM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH0_SCM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH0_SCM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH0_SCM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00010000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH0_SCM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00010000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH0_SCM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH0_SCM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH1_SCM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH1_SCM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH1_SCM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH1_SCM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH1_SCM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00020000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH1_SCM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00020000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH1_SCM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH1_SCM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH2_SCM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH2_SCM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH2_SCM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH2_SCM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH2_SCM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00030000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH2_SCM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00030000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH2_SCM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH2_SCM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH3_SCM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH3_SCM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH3_SCM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH3_SCM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH3_SCM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00040000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH3_SCM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00040000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH3_SCM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFA_CH3_SCM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH0_SCM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH0_SCM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH0_SCM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH0_SCM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH0_SCM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00050000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH0_SCM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00050000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH0_SCM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH0_SCM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH1_SCM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH1_SCM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH1_SCM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH1_SCM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH1_SCM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00060000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH1_SCM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00060000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH1_SCM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH1_SCM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH2_SCM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH2_SCM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH2_SCM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH2_SCM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH2_SCM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00070000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH2_SCM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00070000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH2_SCM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH2_SCM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH3_SCM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH3_SCM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH3_SCM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH3_SCM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH3_SCM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00080000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH3_SCM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00080000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH3_SCM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFB_CH3_SCM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00090000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00090000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RCY_SCM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RCY_SCM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RCY_SCM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RCY_SCM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RCY_SCM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x000a0000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RCY_SCM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x000a0000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RCY_SCM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RCY_SCM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CPU_SCM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CPU_SCM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CPU_SCM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CPU_SCM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CPU_SCM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x000b0000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CPU_SCM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x000b0000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CPU_SCM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CPU_SCM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CCM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CCM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CCM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CCM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CCM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x000c0000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CCM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x000c0000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CCM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CCM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PMC_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PMC_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PMC_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PMC_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PMC_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x000d0000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PMC_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x000d0000) + 0x00a0 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PMC_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PMC_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CBM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CBM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CBM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CBM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CBM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x000e0000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CBM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x000e0000) + 0x0050 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CBM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_CBM_TBL_ID].base[indx] =
      NULL;
  }
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_FBM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_FBM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_FBM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_FBM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_FBM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x000f0000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_FBM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x000f0000) + 0x0080 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_FBM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_FBM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_FDM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_FDM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_FDM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_FDM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_FDM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00100000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_FDM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00100000) + 0x0080 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_FDM_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_FDM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_DWM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_DWM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_DWM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_DWM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_DWM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00110000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_DWM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00110000) + 0x0050 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_DWM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_DWM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RRDM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RRDM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RRDM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RRDM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RRDM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00120000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RRDM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00120000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RRDM_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RRDM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RPDM_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RPDM_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RPDM_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RPDM_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RPDM_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00130000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RPDM_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00130000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RPDM_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_RPDM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PCT_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PCT_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PCT_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PCT_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PCT_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00140000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PCT_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00140000) + 0x0050 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PCT_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PCT_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_VLAN_TABLE_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_VLAN_TABLE_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_VLAN_TABLE_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_VLAN_TABLE_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_VLAN_TABLE_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00150000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_VLAN_TABLE_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00150000) + 0x1000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_VLAN_TABLE_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_VLAN_TABLE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PPCT_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PPCT_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PPCT_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PPCT_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PPCT_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00160000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PPCT_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00160000) + 0x0050 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PPCT_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_PPCT_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_STP_TBL_ID].read_result_address =
    regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_STP_TBL_ID].access_trig_offset =
    regs->a_regs->egq.indirect_command_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_STP_TBL_ID].access_address_offset =
    regs->a_regs->egq.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->egq.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_STP_TBL_ID].write_val_offset =
    regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->egq.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_STP_TBL_ID].start_address =
    ~SOC_PETRA_EGQ_MASK & 0x00170000;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_STP_TBL_ID].end_address =
    (~SOC_PETRA_EGQ_MASK & 0x00170000) + 0x0050 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EGQ_STP_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EGQ_STP_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_ARP_TBL_ID].read_result_address =
    regs->a_regs->epni.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->epni.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_ARP_TBL_ID].access_trig_offset =
    regs->a_regs->epni.indirect_command_reg.addr.base +
    0 * (regs->a_regs->epni.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_ARP_TBL_ID].access_address_offset =
    regs->a_regs->epni.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->epni.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_ARP_TBL_ID].write_val_offset =
    regs->a_regs->epni.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->epni.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_ARP_TBL_ID].start_address =
    ~SOC_PETRA_EPNI_MASK & 0x00010000;
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_ARP_TBL_ID].end_address =
    (~SOC_PETRA_EPNI_MASK & 0x00010000) + 0x2000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_ARP_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EPNI_ARP_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_PTT_TBL_ID].read_result_address =
    regs->a_regs->epni.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->epni.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_PTT_TBL_ID].access_trig_offset =
    regs->a_regs->epni.indirect_command_reg.addr.base +
    0 * (regs->a_regs->epni.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_PTT_TBL_ID].access_address_offset =
    regs->a_regs->epni.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->epni.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_PTT_TBL_ID].write_val_offset =
    regs->a_regs->epni.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->epni.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_PTT_TBL_ID].start_address =
    ~SOC_PETRA_EPNI_MASK & 0x00020000;
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_PTT_TBL_ID].end_address =
    (~SOC_PETRA_EPNI_MASK & 0x00020000) + 0x1000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_PTT_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EPNI_PTT_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_PCP_ENC_TBL_ID].read_result_address =
    regs->a_regs->epni.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->epni.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_PCP_ENC_TBL_ID].access_trig_offset =
    regs->a_regs->epni.indirect_command_reg.addr.base +
    0 * (regs->a_regs->epni.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_PCP_ENC_TBL_ID].access_address_offset =
    regs->a_regs->epni.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->epni.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_PCP_ENC_TBL_ID].write_val_offset =
    regs->a_regs->epni.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->epni.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_PCP_ENC_TBL_ID].start_address =
    ~SOC_PETRA_EPNI_MASK & 0x00030000;
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_PCP_ENC_TBL_ID].end_address =
    (~SOC_PETRA_EPNI_MASK & 0x00030000) + 0x1 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_PCP_ENC_TBL_ID].nof_longs_to_move =
    6;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EPNI_PCP_ENC_TBL_ID].base[indx] =
      NULL;
  }


  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_EPE_PACKET_COUNTER_TBL_ID].read_result_address =
    regs->a_regs->epni.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->epni.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_EPE_PACKET_COUNTER_TBL_ID].access_trig_offset =
    regs->a_regs->epni.indirect_command_reg.addr.base +
    0 * (regs->a_regs->epni.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_EPE_PACKET_COUNTER_TBL_ID].access_address_offset =
    regs->a_regs->epni.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->epni.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_EPE_PACKET_COUNTER_TBL_ID].write_val_offset =
    regs->a_regs->epni.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->epni.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_EPE_PACKET_COUNTER_TBL_ID].start_address =
    ~SOC_PETRA_EPNI_MASK & 0x00040000;
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_EPE_PACKET_COUNTER_TBL_ID].end_address =
    (~SOC_PETRA_EPNI_MASK & 0x00040000) + 0x1 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_EPE_PACKET_COUNTER_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EPNI_EPE_PACKET_COUNTER_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_EPE_BYTES_COUNTER_TBL_ID].read_result_address =
    regs->a_regs->epni.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->epni.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_EPE_BYTES_COUNTER_TBL_ID].access_trig_offset =
    regs->a_regs->epni.indirect_command_reg.addr.base +
    0 * (regs->a_regs->epni.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_EPE_BYTES_COUNTER_TBL_ID].access_address_offset =
    regs->a_regs->epni.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->epni.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_EPE_BYTES_COUNTER_TBL_ID].write_val_offset =
    regs->a_regs->epni.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->epni.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_EPE_BYTES_COUNTER_TBL_ID].start_address =
    ~SOC_PETRA_EPNI_MASK & 0x00050000;
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_EPE_BYTES_COUNTER_TBL_ID].end_address =
    (~SOC_PETRA_EPNI_MASK & 0x00050000) + 0x1 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_EPNI_EPE_BYTES_COUNTER_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_EPNI_EPE_BYTES_COUNTER_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_ID].read_result_address =
    regs->a_regs->cfc.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_ID].access_trig_offset =
    regs->a_regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_ID].access_address_offset =
    regs->a_regs->cfc.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_ID].write_val_offset =
    regs->a_regs->cfc.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_ID].start_address =
    ~SOC_PETRA_CFC_MASK & 0x00000000;
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_ID].end_address =
    (~SOC_PETRA_CFC_MASK & 0x00000000) + 0x0020 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ID].read_result_address =
    regs->a_regs->cfc.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ID].access_trig_offset =
    regs->a_regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ID].access_address_offset =
    regs->a_regs->cfc.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ID].write_val_offset =
    regs->a_regs->cfc.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ID].start_address =
    ~SOC_PETRA_CFC_MASK & 0x00100000;
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ID].end_address =
    (~SOC_PETRA_CFC_MASK & 0x00100000) + 0x0010 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ID].read_result_address =
    regs->a_regs->cfc.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ID].access_trig_offset =
    regs->a_regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ID].access_address_offset =
    regs->a_regs->cfc.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ID].write_val_offset =
    regs->a_regs->cfc.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ID].start_address =
    ~SOC_PETRA_CFC_MASK & 0x00200000;
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ID].end_address =
    (~SOC_PETRA_CFC_MASK & 0x00200000) + 0x0010 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID].read_result_address =
    regs->a_regs->cfc.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID].access_trig_offset =
    regs->a_regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID].access_address_offset =
    regs->a_regs->cfc.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID].write_val_offset =
    regs->a_regs->cfc.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID].start_address =
    ~SOC_PETRA_CFC_MASK & 0x00400000;
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID].end_address =
    (~SOC_PETRA_CFC_MASK & 0x00400000) + 0x0080 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID].read_result_address =
    regs->a_regs->cfc.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID].access_trig_offset =
    regs->a_regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID].access_address_offset =
    regs->a_regs->cfc.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID].write_val_offset =
    regs->a_regs->cfc.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID].start_address =
    ~SOC_PETRA_CFC_MASK & 0x00500000;
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID].end_address =
    (~SOC_PETRA_CFC_MASK & 0x00500000) + 0x0080 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ID].read_result_address =
    regs->a_regs->cfc.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ID].access_trig_offset =
    regs->a_regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ID].access_address_offset =
    regs->a_regs->cfc.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ID].write_val_offset =
    regs->a_regs->cfc.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ID].start_address =
    ~SOC_PETRA_CFC_MASK & 0x00600000;
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ID].end_address =
    (~SOC_PETRA_CFC_MASK & 0x00600000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ID].read_result_address =
    regs->a_regs->cfc.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ID].access_trig_offset =
    regs->a_regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ID].access_address_offset =
    regs->a_regs->cfc.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ID].write_val_offset =
    regs->a_regs->cfc.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ID].start_address =
    ~SOC_PETRA_CFC_MASK & 0x00700000;
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ID].end_address =
    (~SOC_PETRA_CFC_MASK & 0x00700000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ID].read_result_address =
    regs->a_regs->cfc.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ID].access_trig_offset =
    regs->a_regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ID].access_address_offset =
    regs->a_regs->cfc.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ID].write_val_offset =
    regs->a_regs->cfc.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->cfc.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ID].start_address =
    ~SOC_PETRA_CFC_MASK & 0x00800000;
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ID].end_address =
    (~SOC_PETRA_CFC_MASK & 0x00800000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_CAL_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_CAL_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_CAL_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_CAL_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_CAL_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x40000000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_CAL_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x40000000) + 0xf400 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_CAL_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_CAL_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_DRM_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_DRM_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_DRM_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_DRM_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_DRM_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x40010000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_DRM_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x40010000) + 0x0128 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_DRM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_DRM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_DSM_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_DSM_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_DSM_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_DSM_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_DSM_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x40020000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_DSM_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x40020000) + 0x0200 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_DSM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_DSM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FDMS_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FDMS_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FDMS_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FDMS_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FDMS_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x40030000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FDMS_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x40030000) + 0xe000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FDMS_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_FDMS_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SHDS_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SHDS_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SHDS_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SHDS_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SHDS_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x40040000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SHDS_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x40040000) + 0xe000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SHDS_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_SHDS_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SEM_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SEM_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SEM_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SEM_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SEM_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x40050000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SEM_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x40050000) + 0x0800 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SEM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_SEM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FSF_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FSF_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FSF_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FSF_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FSF_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x40060000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FSF_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x40060000) + 0x0800 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FSF_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_FSF_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FGM_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FGM_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FGM_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FGM_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FGM_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x40070000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FGM_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x40070000) + 0x0800 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FGM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_FGM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SHC_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SHC_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SHC_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SHC_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SHC_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x40080000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SHC_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x40080000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SHC_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_SHC_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCC_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCC_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCC_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCC_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCC_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x40090000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCC_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x40090000) + 0x2000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCC_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCC_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCT_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCT_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCT_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCT_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCT_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x400a0000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCT_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x400a0000) + 0x0100 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCT_TBL_ID].nof_longs_to_move =
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCT_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FQM_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FQM_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FQM_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FQM_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FQM_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x400b0000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FQM_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x400b0000) + 0x4000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FQM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_FQM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FFM_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FFM_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FFM_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FFM_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FFM_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x400c0000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FFM_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x400c0000) + 0x2000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FFM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_FFM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_TMC_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_TMC_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_TMC_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_TMC_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_TMC_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x40100000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_TMC_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x40100000) + 0xe000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_TMC_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_TMC_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_PQS_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_PQS_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_PQS_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_PQS_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_PQS_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x401d0000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_PQS_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x401d0000) + 0x0051 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_PQS_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_PQS_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCHEDULER_INIT_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCHEDULER_INIT_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCHEDULER_INIT_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCHEDULER_INIT_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCHEDULER_INIT_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x41000000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCHEDULER_INIT_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x41000000) + 0x0001 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCHEDULER_INIT_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_SCHEDULER_INIT_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_ID].read_result_address =
    regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_ID].access_trig_offset =
    regs->a_regs->sch.indirect_command_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_ID].access_address_offset =
    regs->a_regs->sch.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->sch.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_ID].write_val_offset =
    regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->sch.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_ID].start_address =
    ~SOC_PETRA_SCH_MASK & 0x42000000;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_ID].end_address =
    (~SOC_PETRA_SCH_MASK & 0x42000000) + 0x0001 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_ID].read_result_address =
    regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_ID].access_trig_offset =
    regs->a_regs->irr.indirect_command_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_ID].access_address_offset =
    regs->a_regs->irr.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_ID].write_val_offset =
    regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_ID].start_address =
    ~SOC_PETRA_IRR_MASK & 0x00020000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_ID].end_address =
    (~SOC_PETRA_IRR_MASK & 0x00020000) + 0x8000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_ID].read_result_address =
    regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_ID].access_trig_offset =
    regs->a_regs->irr.indirect_command_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_ID].access_address_offset =
    regs->a_regs->irr.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->irr.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_ID].write_val_offset =
    regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->irr.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_ID].start_address =
    ~SOC_PETRA_IRR_MASK & 0x00020000;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_ID].end_address =
    (~SOC_PETRA_IRR_MASK & 0x00020000) + 0x8000 - 1;
  Soc_petra_indirect_blocks[SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_ID].nof_longs_to_move =
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_QDR_QDR_DLL_MEM_TBL_ID].read_result_address =
    regs->a_regs->qdr.indirect_command_rd_data_reg_0.addr.base +
    0 * (regs->a_regs->qdr.indirect_command_rd_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_QDR_QDR_DLL_MEM_TBL_ID].access_trig_offset =
    regs->a_regs->qdr.indirect_command_reg.addr.base +
    0 * (regs->a_regs->qdr.indirect_command_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_QDR_QDR_DLL_MEM_TBL_ID].access_address_offset =
    regs->a_regs->qdr.indirect_command_address_reg.addr.base +
    0 * (regs->a_regs->qdr.indirect_command_address_reg.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_QDR_QDR_DLL_MEM_TBL_ID].write_val_offset =
    regs->a_regs->qdr.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->a_regs->qdr.indirect_command_wr_data_reg_0.addr.step);
  Soc_petra_indirect_blocks[SOC_PETRA_QDR_QDR_DLL_MEM_TBL_ID].start_address =
    ~SOC_PETRA_QDR_MASK & 0x20000000;
  Soc_petra_indirect_blocks[SOC_PETRA_QDR_QDR_DLL_MEM_TBL_ID].end_address =
    (~SOC_PETRA_QDR_MASK & 0x20000000) + 0x7f;
  Soc_petra_indirect_blocks[SOC_PETRA_QDR_QDR_DLL_MEM_TBL_ID].nof_longs_to_move =
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_petra_indirect_blocks[SOC_PETRA_QDR_QDR_DLL_MEM_TBL_ID].base[indx] =
      NULL;
  }

  Soc_petra_indirect_blocks[SOC_PETRA_TBL_ID_LAST].read_result_address =
    INVALID_ADDRESS;
}
/*****************************************************
*NAME
* soc_petra_indirect_counter_init
*TYPE:
*  PROC
*DATE:
*  12/23/07
*FUNCTION:
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_petra_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
STATIC void
  soc_petra_indirect_counter_init(
  )
{
  Soc_petra_counters[0].chip_offset = INVALID_ADDRESS; 
  Soc_petra_counters[0].mask = 0xFFFFFFFF;
  Soc_petra_counters[0].shift = 0;
  Soc_petra_counters[0].is_random = FALSE;
  Soc_petra_counters[0].count_per_sec = 1000;
  Soc_petra_counters[0].min = 0;
  Soc_petra_counters[0].max = 0xFFFFFFFF;

  Soc_petra_counters[1].chip_offset = INVALID_ADDRESS;
}

/*****************************************************
*NAME
* soc_petra_indirect_interrupt_init
*TYPE:
*  PROC
*DATE:
*  12/23/07
*FUNCTION:
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_petra_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
STATIC void
  soc_petra_indirect_interrupt_init(
  )

{
    Soc_petra_interrupts[0].int_chip_offset = INVALID_ADDRESS;
}
/*****************************************************
*NAME
* soc_petra_indirect_init
*TYPE:
*  PROC
*DATE:
*  12/23/07
*FUNCTION:
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_petra_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
void
  soc_petra_indirect_init(
  )
{
   soc_petra_indirect_blocks_init();
   soc_petra_indirect_counter_init();
   soc_petra_indirect_interrupt_init();
}

void
  soc_petra_initialize_device_values(
    SOC_SAND_OUT uint32   *base_ptr
  )
{
#if 0 /* Not in use in BCM */
    FILE*
    file_p;
  uint32
    addr, 
    value;

  if (!Soc_petra_init_device_values)
  {
    goto exit;
  }

  if((file_p = fopen("petra_reg_file.txt", "r")) == NULL)
  {
    goto exit;
  }

  while (2 == fscanf(file_p, "0x%04X: 0x%08X\n", &addr, &value))
  {
    base_ptr[addr] = value;
  }

  fclose(file_p);

exit:
  return;
#endif
}

/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

