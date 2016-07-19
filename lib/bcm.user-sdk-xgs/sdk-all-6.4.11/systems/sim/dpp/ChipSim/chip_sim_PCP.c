/* $Id: chip_sim_PCP.c,v 1.5 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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


#include <soc/dpp/SAND/Utils/sand_header.h>

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
#include "chip_sim_PCP.h" 
#include "soc/dpp/PCP/pcp_chip_regs.h" 
#include "soc/dpp/PCP/pcp_chip_tbls.h" 
#include "soc/dpp/PCP/pcp_chip_defines.h" 
#include "soc/dpp/SAND/Management/sand_general_macros.h" 
#include "soc/dpp/SAND/Management/sand_error_code.h" 
#include "soc/dpp/SAND/Utils/sand_os_interface.h"
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
  Pcp_indirect_blocks[PCP_TBL_ID_LAST + 1];
CHIP_SIM_COUNTER
  Pcp_counters[2];
CHIP_SIM_INTERRUPT
  Pcp_interrupts[1];
uint8
  Pcp_init_device_values = FALSE;
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */
/*****************************************************
*NAME
* pcp_indirect_blocks_init
*TYPE:
*  PROC
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
*    *pcp_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
STATIC void
  pcp_indirect_blocks_init(
  )
{
  uint32
    indx;
  uint32
    err = SOC_SAND_OK;
  PCP_REGS
    *regs = NULL;
    
  err = 
    pcp_regs_get(
      &(regs)
    );
  

  Pcp_indirect_blocks[PCP_ECI_QDR_TBL_ID].read_result_address = 
    regs->eci.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->eci.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_ECI_QDR_TBL_ID].access_trig_offset = 
    regs->eci.indirect_command_reg.addr.base +
    0 * (regs->eci.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_ECI_QDR_TBL_ID].access_address_offset = 
    regs->eci.indirect_command_address_reg.addr.base + 
    0 * (regs->eci.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_ECI_QDR_TBL_ID].write_val_offset = 
    regs->eci.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->eci.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_ECI_QDR_TBL_ID].start_address = 
    ~PCP_ECI_MASK & 0x00000000;
  Pcp_indirect_blocks[PCP_ECI_QDR_TBL_ID].end_address = 
    (~PCP_ECI_MASK & 0x00000000) + 0x100000 - 1;
  Pcp_indirect_blocks[PCP_ECI_QDR_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_ECI_QDR_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_ECI_RLD1_TBL_ID].read_result_address = 
    regs->eci.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->eci.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_ECI_RLD1_TBL_ID].access_trig_offset = 
    regs->eci.indirect_command_reg.addr.base +
    0 * (regs->eci.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_ECI_RLD1_TBL_ID].access_address_offset = 
    regs->eci.indirect_command_address_reg.addr.base + 
    0 * (regs->eci.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_ECI_RLD1_TBL_ID].write_val_offset = 
    regs->eci.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->eci.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_ECI_RLD1_TBL_ID].start_address = 
    ~PCP_ECI_MASK & 0x00800000;
  Pcp_indirect_blocks[PCP_ECI_RLD1_TBL_ID].end_address = 
    (~PCP_ECI_MASK & 0x00800000) + 0x800000 - 1;
  Pcp_indirect_blocks[PCP_ECI_RLD1_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_ECI_RLD1_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_ECI_RLD2_TBL_ID].read_result_address = 
    regs->eci.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->eci.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_ECI_RLD2_TBL_ID].access_trig_offset = 
    regs->eci.indirect_command_reg.addr.base +
    0 * (regs->eci.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_ECI_RLD2_TBL_ID].access_address_offset = 
    regs->eci.indirect_command_address_reg.addr.base + 
    0 * (regs->eci.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_ECI_RLD2_TBL_ID].write_val_offset = 
    regs->eci.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->eci.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_ECI_RLD2_TBL_ID].start_address = 
    ~PCP_ECI_MASK & 0x01000000;
  Pcp_indirect_blocks[PCP_ECI_RLD2_TBL_ID].end_address = 
    (~PCP_ECI_MASK & 0x01000000) + 0x800000 - 1;
  Pcp_indirect_blocks[PCP_ECI_RLD2_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_ECI_RLD2_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_ID].read_result_address = 
    regs->elk.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->elk.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_ID].access_trig_offset = 
    regs->elk.indirect_command_reg.addr.base +
    0 * (regs->elk.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_ID].access_address_offset = 
    regs->elk.indirect_command_address_reg.addr.base + 
    0 * (regs->elk.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_ID].write_val_offset = 
    regs->elk.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->elk.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_ID].start_address = 
    ~PCP_ELK_MASK & 0x00000000;
  Pcp_indirect_blocks[PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_ID].end_address = 
    (~PCP_ELK_MASK & 0x00000000) + 0x0008 - 1;
  Pcp_indirect_blocks[PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_ELK_FID_COUNTER_DB_TBL_ID].read_result_address = 
    regs->elk.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->elk.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_ELK_FID_COUNTER_DB_TBL_ID].access_trig_offset = 
    regs->elk.indirect_command_reg.addr.base +
    0 * (regs->elk.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_ELK_FID_COUNTER_DB_TBL_ID].access_address_offset = 
    regs->elk.indirect_command_address_reg.addr.base + 
    0 * (regs->elk.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_ELK_FID_COUNTER_DB_TBL_ID].write_val_offset = 
    regs->elk.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->elk.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_ELK_FID_COUNTER_DB_TBL_ID].start_address = 
    ~PCP_ELK_MASK & 0x00800000;
  Pcp_indirect_blocks[PCP_ELK_FID_COUNTER_DB_TBL_ID].end_address = 
    (~PCP_ELK_MASK & 0x00800000) + 0x4000 - 1;
  Pcp_indirect_blocks[PCP_ELK_FID_COUNTER_DB_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_ELK_FID_COUNTER_DB_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_ELK_SYS_PORT_IS_MINE_TBL_ID].read_result_address = 
    regs->elk.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->elk.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_ELK_SYS_PORT_IS_MINE_TBL_ID].access_trig_offset = 
    regs->elk.indirect_command_reg.addr.base +
    0 * (regs->elk.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_ELK_SYS_PORT_IS_MINE_TBL_ID].access_address_offset = 
    regs->elk.indirect_command_address_reg.addr.base + 
    0 * (regs->elk.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_ELK_SYS_PORT_IS_MINE_TBL_ID].write_val_offset = 
    regs->elk.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->elk.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_ELK_SYS_PORT_IS_MINE_TBL_ID].start_address = 
    ~PCP_ELK_MASK & 0x01000000;
  Pcp_indirect_blocks[PCP_ELK_SYS_PORT_IS_MINE_TBL_ID].end_address = 
    (~PCP_ELK_MASK & 0x01000000) + 0x2000 - 1;
  Pcp_indirect_blocks[PCP_ELK_SYS_PORT_IS_MINE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_ELK_SYS_PORT_IS_MINE_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_ELK_AGING_CFG_TABLE_TBL_ID].read_result_address = 
    regs->elk.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->elk.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_ELK_AGING_CFG_TABLE_TBL_ID].access_trig_offset = 
    regs->elk.indirect_command_reg.addr.base +
    0 * (regs->elk.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_ELK_AGING_CFG_TABLE_TBL_ID].access_address_offset = 
    regs->elk.indirect_command_address_reg.addr.base + 
    0 * (regs->elk.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_ELK_AGING_CFG_TABLE_TBL_ID].write_val_offset = 
    regs->elk.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->elk.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_ELK_AGING_CFG_TABLE_TBL_ID].start_address = 
    ~PCP_ELK_MASK & 0x01800000;
  Pcp_indirect_blocks[PCP_ELK_AGING_CFG_TABLE_TBL_ID].end_address = 
    (~PCP_ELK_MASK & 0x01800000) + 0x0020 - 1;
  Pcp_indirect_blocks[PCP_ELK_AGING_CFG_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_ELK_AGING_CFG_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_ELK_FLUSH_DB_TBL_ID].read_result_address = 
    regs->elk.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->elk.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_ELK_FLUSH_DB_TBL_ID].access_trig_offset = 
    regs->elk.indirect_command_reg.addr.base +
    0 * (regs->elk.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_ELK_FLUSH_DB_TBL_ID].access_address_offset = 
    regs->elk.indirect_command_address_reg.addr.base + 
    0 * (regs->elk.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_ELK_FLUSH_DB_TBL_ID].write_val_offset = 
    regs->elk.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->elk.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_ELK_FLUSH_DB_TBL_ID].start_address = 
    ~PCP_ELK_MASK & 0x02000000;
  Pcp_indirect_blocks[PCP_ELK_FLUSH_DB_TBL_ID].end_address = 
    (~PCP_ELK_MASK & 0x02000000) + 0x0002 - 1;
  Pcp_indirect_blocks[PCP_ELK_FLUSH_DB_TBL_ID].nof_longs_to_move = 
    7;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_ELK_FLUSH_DB_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_OAM_PR2_TCDP_TBL_ID].read_result_address = 
    regs->oam.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->oam.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_PR2_TCDP_TBL_ID].access_trig_offset = 
    regs->oam.indirect_command_reg.addr.base +
    0 * (regs->oam.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_PR2_TCDP_TBL_ID].access_address_offset = 
    regs->oam.indirect_command_address_reg.addr.base + 
    0 * (regs->oam.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_PR2_TCDP_TBL_ID].write_val_offset = 
    regs->oam.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->oam.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_PR2_TCDP_TBL_ID].start_address = 
    ~PCP_OAM_MASK & 0x00000000;
  Pcp_indirect_blocks[PCP_OAM_PR2_TCDP_TBL_ID].end_address = 
    (~PCP_OAM_MASK & 0x00000000) + 0x0020 - 1;
  Pcp_indirect_blocks[PCP_OAM_PR2_TCDP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_OAM_PR2_TCDP_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_OAM_MEP_DB_TBL_ID].read_result_address = 
    regs->oam.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->oam.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_MEP_DB_TBL_ID].access_trig_offset = 
    regs->oam.indirect_command_reg.addr.base +
    0 * (regs->oam.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_MEP_DB_TBL_ID].access_address_offset = 
    regs->oam.indirect_command_address_reg.addr.base + 
    0 * (regs->oam.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_MEP_DB_TBL_ID].write_val_offset = 
    regs->oam.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->oam.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_MEP_DB_TBL_ID].start_address = 
    ~PCP_OAM_MASK & 0x00800000;
  Pcp_indirect_blocks[PCP_OAM_MEP_DB_TBL_ID].end_address = 
    (~PCP_OAM_MASK & 0x00800000) + 0x1000 - 1;
  Pcp_indirect_blocks[PCP_OAM_MEP_DB_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_OAM_MEP_DB_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_OAM_RMEP_DB_TBL_ID].read_result_address = 
    regs->oam.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->oam.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_RMEP_DB_TBL_ID].access_trig_offset = 
    regs->oam.indirect_command_reg.addr.base +
    0 * (regs->oam.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_RMEP_DB_TBL_ID].access_address_offset = 
    regs->oam.indirect_command_address_reg.addr.base + 
    0 * (regs->oam.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_RMEP_DB_TBL_ID].write_val_offset = 
    regs->oam.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->oam.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_RMEP_DB_TBL_ID].start_address = 
    ~PCP_OAM_MASK & 0x01000000;
  Pcp_indirect_blocks[PCP_OAM_RMEP_DB_TBL_ID].end_address = 
    (~PCP_OAM_MASK & 0x01000000) + 0x2000 - 1;
  Pcp_indirect_blocks[PCP_OAM_RMEP_DB_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_OAM_RMEP_DB_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_OAM_RMEP_HASH_0_DB_TBL_ID].read_result_address = 
    regs->oam.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->oam.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_RMEP_HASH_0_DB_TBL_ID].access_trig_offset = 
    regs->oam.indirect_command_reg.addr.base +
    0 * (regs->oam.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_RMEP_HASH_0_DB_TBL_ID].access_address_offset = 
    regs->oam.indirect_command_address_reg.addr.base + 
    0 * (regs->oam.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_RMEP_HASH_0_DB_TBL_ID].write_val_offset = 
    regs->oam.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->oam.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_RMEP_HASH_0_DB_TBL_ID].start_address = 
    ~PCP_OAM_MASK & 0x01800000;
  Pcp_indirect_blocks[PCP_OAM_RMEP_HASH_0_DB_TBL_ID].end_address = 
    (~PCP_OAM_MASK & 0x01800000) + 0x2000 - 1;
  Pcp_indirect_blocks[PCP_OAM_RMEP_HASH_0_DB_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_OAM_RMEP_HASH_0_DB_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_OAM_RMEP_HASH_1_DB_TBL_ID].read_result_address = 
    regs->oam.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->oam.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_RMEP_HASH_1_DB_TBL_ID].access_trig_offset = 
    regs->oam.indirect_command_reg.addr.base +
    0 * (regs->oam.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_RMEP_HASH_1_DB_TBL_ID].access_address_offset = 
    regs->oam.indirect_command_address_reg.addr.base + 
    0 * (regs->oam.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_RMEP_HASH_1_DB_TBL_ID].write_val_offset = 
    regs->oam.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->oam.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_RMEP_HASH_1_DB_TBL_ID].start_address = 
    ~PCP_OAM_MASK & 0x02000000;
  Pcp_indirect_blocks[PCP_OAM_RMEP_HASH_1_DB_TBL_ID].end_address = 
    (~PCP_OAM_MASK & 0x02000000) + 0x2000 - 1;
  Pcp_indirect_blocks[PCP_OAM_RMEP_HASH_1_DB_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_OAM_RMEP_HASH_1_DB_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_OAM_LMDB_CMN_TBL_ID].read_result_address = 
    regs->oam.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->oam.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_LMDB_CMN_TBL_ID].access_trig_offset = 
    regs->oam.indirect_command_reg.addr.base +
    0 * (regs->oam.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_LMDB_CMN_TBL_ID].access_address_offset = 
    regs->oam.indirect_command_address_reg.addr.base + 
    0 * (regs->oam.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_LMDB_CMN_TBL_ID].write_val_offset = 
    regs->oam.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->oam.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_LMDB_CMN_TBL_ID].start_address = 
    ~PCP_OAM_MASK & 0x02800000;
  Pcp_indirect_blocks[PCP_OAM_LMDB_CMN_TBL_ID].end_address = 
    (~PCP_OAM_MASK & 0x02800000) + 0x0080 - 1;
  Pcp_indirect_blocks[PCP_OAM_LMDB_CMN_TBL_ID].nof_longs_to_move = 
    11;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_OAM_LMDB_CMN_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_OAM_LMDB_TX_TBL_ID].read_result_address = 
    regs->oam.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->oam.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_LMDB_TX_TBL_ID].access_trig_offset = 
    regs->oam.indirect_command_reg.addr.base +
    0 * (regs->oam.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_LMDB_TX_TBL_ID].access_address_offset = 
    regs->oam.indirect_command_address_reg.addr.base + 
    0 * (regs->oam.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_LMDB_TX_TBL_ID].write_val_offset = 
    regs->oam.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->oam.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_LMDB_TX_TBL_ID].start_address = 
    ~PCP_OAM_MASK & 0x03000000;
  Pcp_indirect_blocks[PCP_OAM_LMDB_TX_TBL_ID].end_address = 
    (~PCP_OAM_MASK & 0x03000000) + 0x0080 - 1;
  Pcp_indirect_blocks[PCP_OAM_LMDB_TX_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_OAM_LMDB_TX_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_OAM_DMDB_TX_TBL_ID].read_result_address = 
    regs->oam.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->oam.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_DMDB_TX_TBL_ID].access_trig_offset = 
    regs->oam.indirect_command_reg.addr.base +
    0 * (regs->oam.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_DMDB_TX_TBL_ID].access_address_offset = 
    regs->oam.indirect_command_address_reg.addr.base + 
    0 * (regs->oam.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_DMDB_TX_TBL_ID].write_val_offset = 
    regs->oam.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->oam.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_DMDB_TX_TBL_ID].start_address = 
    ~PCP_OAM_MASK & 0x03800000;
  Pcp_indirect_blocks[PCP_OAM_DMDB_TX_TBL_ID].end_address = 
    (~PCP_OAM_MASK & 0x03800000) + 0x0080 - 1;
  Pcp_indirect_blocks[PCP_OAM_DMDB_TX_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_OAM_DMDB_TX_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_OAM_DMDB_RX_TBL_ID].read_result_address = 
    regs->oam.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->oam.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_DMDB_RX_TBL_ID].access_trig_offset = 
    regs->oam.indirect_command_reg.addr.base +
    0 * (regs->oam.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_DMDB_RX_TBL_ID].access_address_offset = 
    regs->oam.indirect_command_address_reg.addr.base + 
    0 * (regs->oam.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_OAM_DMDB_RX_TBL_ID].write_val_offset = 
    regs->oam.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->oam.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_OAM_DMDB_RX_TBL_ID].start_address = 
    ~PCP_OAM_MASK & 0x04000000;
  Pcp_indirect_blocks[PCP_OAM_DMDB_RX_TBL_ID].end_address = 
    (~PCP_OAM_MASK & 0x04000000) + 0x0080 - 1;
  Pcp_indirect_blocks[PCP_OAM_DMDB_RX_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_OAM_DMDB_RX_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_STS_ING_COUNTERS_TBL_ID].read_result_address = 
    regs->sts.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->sts.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_STS_ING_COUNTERS_TBL_ID].access_trig_offset = 
    regs->sts.indirect_command_reg.addr.base +
    0 * (regs->sts.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_STS_ING_COUNTERS_TBL_ID].access_address_offset = 
    regs->sts.indirect_command_address_reg.addr.base + 
    0 * (regs->sts.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_STS_ING_COUNTERS_TBL_ID].write_val_offset = 
    regs->sts.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->sts.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_STS_ING_COUNTERS_TBL_ID].start_address = 
    ~PCP_STS_MASK & 0x00000000;
  Pcp_indirect_blocks[PCP_STS_ING_COUNTERS_TBL_ID].end_address = 
    (~PCP_STS_MASK & 0x00000000) + 0x4000 - 1;
  Pcp_indirect_blocks[PCP_STS_ING_COUNTERS_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_STS_ING_COUNTERS_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_STS_EGR_COUNTERS_TBL_ID].read_result_address = 
    regs->sts.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->sts.indirect_command_rd_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_STS_EGR_COUNTERS_TBL_ID].access_trig_offset = 
    regs->sts.indirect_command_reg.addr.base +
    0 * (regs->sts.indirect_command_reg.addr.step);
  Pcp_indirect_blocks[PCP_STS_EGR_COUNTERS_TBL_ID].access_address_offset = 
    regs->sts.indirect_command_address_reg.addr.base + 
    0 * (regs->sts.indirect_command_address_reg.addr.step);
  Pcp_indirect_blocks[PCP_STS_EGR_COUNTERS_TBL_ID].write_val_offset = 
    regs->sts.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->sts.indirect_command_wr_data_reg_0.addr.step);
  Pcp_indirect_blocks[PCP_STS_EGR_COUNTERS_TBL_ID].start_address = 
    ~PCP_STS_MASK & 0x00004000;
  Pcp_indirect_blocks[PCP_STS_EGR_COUNTERS_TBL_ID].end_address = 
    (~PCP_STS_MASK & 0x00004000) + 0x4000 - 1;
  Pcp_indirect_blocks[PCP_STS_EGR_COUNTERS_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Pcp_indirect_blocks[PCP_STS_EGR_COUNTERS_TBL_ID].base[indx] = 
      NULL;
  }

  Pcp_indirect_blocks[PCP_TBL_ID_LAST].read_result_address = 
    INVALID_ADDRESS;
}
/*****************************************************
*NAME
* pcp_indirect_counter_init
*TYPE:
*  PROC
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
*    *pcp_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
STATIC void
  pcp_indirect_counter_init(
  )
{
  uint32
    err = SOC_SAND_OK;
  PCP_REGS
    *regs = NULL;
    
  err = 
    pcp_regs_get(
      &(regs)
    );
  

  Pcp_counters[0].chip_offset = INVALID_ADDRESS; 
  Pcp_counters[0].mask = 0xFFFFFFFF;
  Pcp_counters[0].shift = 0;
  Pcp_counters[0].is_random = FALSE;
  Pcp_counters[0].count_per_sec = 1000;
  Pcp_counters[0].min = 0;
  Pcp_counters[0].max = 0xFFFFFFFF;

  Pcp_counters[1].chip_offset = INVALID_ADDRESS;
}

/*****************************************************
*NAME
* pcp_indirect_interrupt_init
*TYPE:
*  PROC
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
*    *pcp_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
STATIC void
  pcp_indirect_interrupt_init(
  )

{
    Pcp_interrupts[0].int_chip_offset = INVALID_ADDRESS;
}
/*****************************************************
*NAME
* pcp_indirect_init
*TYPE:
*  PROC
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
*    *pcp_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
void
  pcp_indirect_init(
  )
{
   pcp_indirect_blocks_init();
   pcp_indirect_counter_init();
   pcp_indirect_interrupt_init();
}
  /* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
