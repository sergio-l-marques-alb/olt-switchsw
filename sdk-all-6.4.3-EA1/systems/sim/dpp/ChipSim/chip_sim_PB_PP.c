/* $Id: chip_sim_PB_PP.c,v 1.6 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
/******************************************************************
*
* FILENAME:       chip_sim_PB_PP_PP.c
*
* MODULE PREFIX:  chip_sim
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


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
#include "chip_sim_PB_PP.h" 
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h> 
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_tbls.h> 
#include <soc/dpp/Petra/PB_TM/pb_chip_tbls.h> 
#include <soc/dpp/Petra/petra_chip_defines.h> 
#include <soc/dpp/SAND/Management/sand_general_macros.h> 
#include <soc/dpp/SAND/Management/sand_error_code.h> 
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
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
extern 
  CHIP_SIM_INDIRECT_BLOCK
    Soc_pb_indirect_blocks[];
extern 
  uint8
    Soc_pb_init_device_values;
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */
/*****************************************************
*NAME
* soc_pb_pp_indirect_blocks_init
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
*    *soc_pb_pp_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
void
  soc_pb_pp_indirect_blocks_init(
  )
{
  uint32
    indx;
  uint32
    err = SOC_SAND_OK;
  SOC_PB_REGS
    *regs = NULL;
    
  err = 
    soc_pb_regs_get(
      &(regs)
    );
  

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01300000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01300000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PINFO_LLR_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PINFO_LLR_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PINFO_LLR_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PINFO_LLR_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PINFO_LLR_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00200000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PINFO_LLR_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00200000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PINFO_LLR_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PINFO_LLR_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLR_LLVP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLR_LLVP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLR_LLVP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLR_LLVP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLR_LLVP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00210000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLR_LLVP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00210000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLR_LLVP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLR_LLVP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00220000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00220000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00230000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00230000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00240000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00240000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TOS_2_COS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TOS_2_COS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TOS_2_COS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TOS_2_COS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TOS_2_COS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00250000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TOS_2_COS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00250000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TOS_2_COS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TOS_2_COS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_RESERVED_MC_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_RESERVED_MC_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_RESERVED_MC_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_RESERVED_MC_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_RESERVED_MC_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00260000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_RESERVED_MC_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00260000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_RESERVED_MC_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_RESERVED_MC_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FLUSH_DB_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FLUSH_DB_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FLUSH_DB_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FLUSH_DB_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FLUSH_DB_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00760000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FLUSH_DB_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00760000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FLUSH_DB_TBL_ID].nof_longs_to_move = 
    7;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FLUSH_DB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00780000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00780000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00790000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00790000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x007a0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x007a0000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00000000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00000000) + 0x0050 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00010000) + 0x0050 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00020000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00020000) + 0x0050 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00030000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00040000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00040000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00050000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00050000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00060000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00060000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00070000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00070000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00080000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00080000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00090000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00090000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x000a0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x000a0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].nof_longs_to_move = 
    4;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x000b0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x000b0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x000c0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x000c0000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x000d0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x000d0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x000e0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x000e0000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x000f0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x000f0000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00100000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00100000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00110000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00110000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00120000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00120000) + 0x0007 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00130000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00130000) + 0x0007 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00140000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00140000) + 0x000f - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00900000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00900000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_ISID_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_ISID_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_ISID_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_ISID_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_ISID_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01100000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_ISID_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01100000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_ISID_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_ISID_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01110000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01110000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01120000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01120000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01130000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01130000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID].nof_longs_to_move = 
    4;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  { 
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01140000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01140000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01150000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01150000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01160000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01160000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01170000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01170000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01180000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01180000) + 0x0007 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00300000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00300000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00310000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00310000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00320000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00320000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00330000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00330000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00340000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00340000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00360000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00360000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00370000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00370000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00380000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00380000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_STP_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_STP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_STP_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_STP_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_STP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00390000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_STP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00390000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_STP_TABLE_TBL_ID].nof_longs_to_move = 
    4;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_STP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x003a0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x003a0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x003b0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x003b0000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x003c0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x003c0000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x003d0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x003d0000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_LLVP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_LLVP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_LLVP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_LLVP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_LLVP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x003e0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_LLVP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x003e0000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_LLVP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_LLVP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x003f0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x003f0000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00400000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00400000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00410000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00410000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00420000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00420000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00430000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00430000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00440000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00440000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00450000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00450000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00460000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00460000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00470000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00470000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00480000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00480000) + 0x3c00 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FER_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FER_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FER_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FER_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FER_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b00000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FER_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b00000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FER_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b10000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b10000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b20000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b20000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b30000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b30000) + 0x000f - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b40000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b40000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b50000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b50000) + 0x3fff - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b50000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b50000) + 0x3fff - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b50000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b50000) + 0x3fff - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b50000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b50000) + 0x3fff - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b50000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b50000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b70000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b70000) + 0x03ff - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PATH_SELECT_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PATH_SELECT_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PATH_SELECT_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PATH_SELECT_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PATH_SELECT_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b90000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PATH_SELECT_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b90000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PATH_SELECT_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PATH_SELECT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00ba0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00ba0000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00bb0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00bb0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID].nof_longs_to_move = 
    4;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00bc0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00bc0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FLP_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FLP_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FLP_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FLP_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FLP_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00100000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FLP_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00100000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FLP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FLP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00110000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00110000) + 0x000f - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00120000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00120000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID].nof_longs_to_move = 
    5;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00130000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00130000) + 0x0018 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00140000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00140000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00d10000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00d10000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00d20000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00d20000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00d30000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00d30000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM1_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM1_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM1_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM1_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM1_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00e00000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM1_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00e00000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM1_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM1_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM2_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM2_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM2_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM2_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM2_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00e10000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM2_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00e10000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM2_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM2_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM3_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM3_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM3_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM3_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM3_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00e20000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM3_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00e20000) + 0x3000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM3_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM3_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM4_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM4_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM4_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM4_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM4_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00e30000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM4_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00e30000) + 0x3000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM4_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM4_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM5_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM5_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM5_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM5_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM5_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00e40000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM5_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00e40000) + 0x3000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM5_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM5_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM6_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM6_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM6_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM6_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM6_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00e50000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM6_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00e50000) + 0x3000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM6_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM6_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00320000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00320000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00330000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00330000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00340000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00340000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00350000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00360000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00360000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00370000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00370000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00380000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00380000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00390000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00390000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x003a0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x003a0000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x003b0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x003b0000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x003c0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x003c0000) + 0x001F - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID].nof_longs_to_move = 
    2;

  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1].start_address = 
    ~SOC_PB_IHB_MASK & 0x003c0020;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1].end_address = 
    (~SOC_PB_IHB_MASK & 0x003c0020) + 0x001F - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1].nof_longs_to_move = 
    2;

  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2].start_address = 
    ~SOC_PB_IHB_MASK & 0x003c0040;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2].end_address = 
    (~SOC_PB_IHB_MASK & 0x003c0040) + 0x001F - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2].nof_longs_to_move = 
    2;

  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3].start_address = 
    ~SOC_PB_IHB_MASK & 0x003c0060;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3].end_address = 
    (~SOC_PB_IHB_MASK & 0x003c0060) + 0x001F - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3].nof_longs_to_move = 
    2;

  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00400000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00400000) + 0x001F - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1].start_address = 
    ~SOC_PB_IHB_MASK & 0x00400020;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1].end_address = 
    (~SOC_PB_IHB_MASK & 0x00400020) + 0x001F - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2].start_address = 
    ~SOC_PB_IHB_MASK & 0x00400040;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2].end_address = 
    (~SOC_PB_IHB_MASK & 0x00400040) + 0x001F - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3].start_address = 
    ~SOC_PB_IHB_MASK & 0x00400060;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3].end_address = 
    (~SOC_PB_IHB_MASK & 0x00400060) + 0x001F - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00420000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00420000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00430000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00430000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00460000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00460000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_BANK_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_BANK_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_BANK_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_BANK_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_BANK_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00f00000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_BANK_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00f00000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_BANK_TBL_ID].nof_longs_to_move = 
    10;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_BANK_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00f10000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00f10000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PPCT_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PPCT_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PPCT_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PPCT_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PPCT_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x002b0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PPCT_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x002b0000) + 0x0050 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PPCT_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PPCT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PP_PPCT_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PP_PPCT_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PP_PPCT_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PP_PPCT_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PP_PPCT_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x002c0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PP_PPCT_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x002c0000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PP_PPCT_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PP_PPCT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x004b0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x004b0000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00600000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00600000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00610000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00610000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00620000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00620000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00630000) + 0x0600 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00670000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00670000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00030000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00030000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00030000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00030000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00040000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00040000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00050000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00050000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_STP_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_STP_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_STP_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_STP_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_STP_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00060000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_STP_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00060000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_STP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_STP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00070000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00070000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00080000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00080000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PP_PCT_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PP_PCT_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PP_PCT_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PP_PCT_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PP_PCT_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00090000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PP_PCT_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00090000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PP_PCT_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PP_PCT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x000a0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x000a0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x000b0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x000b0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x000c0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x000c0000) + 0x0c00 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00130000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00130000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00140000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00140000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00150000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00150000) + 0x0018 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01300000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01300000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PINFO_LLR_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PINFO_LLR_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PINFO_LLR_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PINFO_LLR_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PINFO_LLR_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00200000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PINFO_LLR_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00200000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PINFO_LLR_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PINFO_LLR_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLR_LLVP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLR_LLVP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLR_LLVP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLR_LLVP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLR_LLVP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00210000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLR_LLVP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00210000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLR_LLVP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLR_LLVP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00220000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00220000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00230000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00230000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00240000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00240000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TOS_2_COS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TOS_2_COS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TOS_2_COS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TOS_2_COS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TOS_2_COS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00250000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TOS_2_COS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00250000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TOS_2_COS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TOS_2_COS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_RESERVED_MC_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_RESERVED_MC_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_RESERVED_MC_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_RESERVED_MC_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_RESERVED_MC_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00260000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_RESERVED_MC_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00260000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_RESERVED_MC_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_RESERVED_MC_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00630000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FLUSH_DB_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FLUSH_DB_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FLUSH_DB_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FLUSH_DB_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FLUSH_DB_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00760000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FLUSH_DB_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00760000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FLUSH_DB_TBL_ID].nof_longs_to_move = 
    7;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FLUSH_DB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00780000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00780000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00790000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00790000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x007a0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x007a0000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00000000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00000000) + 0x0050 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00010000) + 0x0050 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00020000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00020000) + 0x0050 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00030000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00040000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00040000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00050000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00050000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00060000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00060000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00070000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00070000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00080000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00080000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00090000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00090000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x000a0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x000a0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x000b0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x000b0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x000c0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x000c0000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x000d0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x000d0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x000e0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x000e0000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x000f0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x000f0000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00100000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00100000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00110000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00110000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00120000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00120000) + 0x0007 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00130000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00130000) + 0x0007 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00140000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00140000) + 0x000f - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00900000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00900000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_ISID_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_ISID_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_ISID_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_ISID_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_ISID_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01100000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_ISID_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01100000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_ISID_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_ISID_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01110000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01110000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01120000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01120000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01130000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01130000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID].nof_longs_to_move = 
    4;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01140000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01140000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01150000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01150000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01160000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01160000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01170000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01170000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01180000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01180000) + 0x0007 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00300000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00300000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00310000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00310000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00320000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00320000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00330000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00330000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00340000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00340000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00350000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00360000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00360000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00370000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00370000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00380000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00380000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_STP_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_STP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_STP_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_STP_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_STP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00390000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_STP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00390000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_STP_TABLE_TBL_ID].nof_longs_to_move = 
    4;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_STP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x003a0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x003a0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x003b0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x003b0000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x003c0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x003c0000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x003d0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x003d0000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_LLVP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_LLVP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_LLVP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_LLVP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_LLVP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x003e0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_LLVP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x003e0000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_LLVP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT_LLVP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x003f0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x003f0000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00400000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00400000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00410000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00410000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00420000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00420000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00430000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00430000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00440000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00440000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00450000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00450000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00460000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00460000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00470000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00470000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00480000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00480000) + 0x3c00 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FER_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FER_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FER_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FER_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FER_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b00000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FER_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b00000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FER_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b10000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b10000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b20000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b20000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b30000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b30000) + 0x000f - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b40000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b40000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b50000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b50000) + 0x3fff - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b50000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b50000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b50000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b50000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b50000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b50000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b50000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b50000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b70000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b70000) + 0x07ff - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PATH_SELECT_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PATH_SELECT_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PATH_SELECT_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PATH_SELECT_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PATH_SELECT_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b90000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PATH_SELECT_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b90000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PATH_SELECT_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PATH_SELECT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00ba0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00ba0000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00bb0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00bb0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00bc0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00bc0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FLP_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FLP_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FLP_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FLP_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FLP_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00100000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FLP_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00100000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FLP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_FLP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00110000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00110000) + 0x000f - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00120000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00120000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID].nof_longs_to_move = 
    5;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00130000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00130000) + 0x0018 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00140000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00140000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00d10000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00d10000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00d20000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00d20000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00d30000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00d30000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM1_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM1_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM1_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM1_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM1_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00e00000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM1_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00e00000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM1_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM1_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM2_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM2_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM2_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM2_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM2_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00e10000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM2_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00e10000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM2_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM2_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM3_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM3_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM3_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM3_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM3_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00e20000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM3_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00e20000) + 0x3000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM3_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM3_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM4_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM4_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM4_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM4_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM4_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00e30000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM4_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00e30000) + 0x3000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM4_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM4_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM5_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM5_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM5_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM5_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM5_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00e40000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM5_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00e40000) + 0x3000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM5_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM5_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM6_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM6_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM6_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM6_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM6_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00e50000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM6_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00e50000) + 0x3000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM6_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_LPM6_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00320000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00320000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00330000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00330000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00340000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00340000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00350000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00360000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00360000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00370000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00370000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00380000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00380000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00390000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00390000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x003a0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x003a0000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x003b0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x003b0000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00420000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00420000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00430000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00430000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00460000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00460000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_BANK_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_BANK_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_BANK_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_BANK_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_BANK_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00f00000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_BANK_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00f00000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_BANK_TBL_ID].nof_longs_to_move = 
    10;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_BANK_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00f10000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00f10000) + 0x07fd - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PPCT_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PPCT_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PPCT_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PPCT_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PPCT_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x002b0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PPCT_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x002b0000) + 0x0050 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PPCT_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PPCT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PP_PPCT_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PP_PPCT_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PP_PPCT_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PP_PPCT_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PP_PPCT_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x002c0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PP_PPCT_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x002c0000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PP_PPCT_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_PP_PPCT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x004b0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x004b0000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00600000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00600000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00610000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00610000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00620000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00620000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00630000) + 0x0600 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00670000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00670000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00030000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00030000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00030000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00030000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00040000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00040000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00050000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00050000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_STP_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_STP_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_STP_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_STP_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_STP_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00060000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_STP_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00060000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_STP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_STP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00070000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00070000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00080000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00080000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PP_PCT_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PP_PCT_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PP_PCT_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PP_PCT_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PP_PCT_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00090000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PP_PCT_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00090000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PP_PCT_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PP_PCT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x000a0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x000a0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x000b0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x000b0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x000c0000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x000c0000) + 0x0c00 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00130000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00130000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00140000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00140000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00150000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00150000) + 0x0018 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL2_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL2_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL2_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL2_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL2_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b60000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL2_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b60000) + 0x3fff - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL2_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_TBL2_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL2_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL2_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL2_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL2_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL2_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b60000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL2_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b60000) + 0x3fff - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL2_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL2_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL2_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL2_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL2_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL2_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL2_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b60000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL2_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b60000) + 0x3fff - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL2_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL2_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL2_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL2_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL2_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL2_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL2_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b60000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL2_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b60000) + 0x3fff - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL2_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL2_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL2_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL2_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL2_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL2_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL2_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b60000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL2_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b60000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL2_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL2_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL2_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL2_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL2_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL2_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL2_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00b80000;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL2_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00b80000) + 0x03ff - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL2_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL2_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ESEM_MGMT_REQUEST_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ESEM_MGMT_REQUEST_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ESEM_MGMT_REQUEST_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ESEM_MGMT_REQUEST_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ESEM_MGMT_REQUEST_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00280000;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ESEM_MGMT_REQUEST_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00280000) + 0x001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ESEM_MGMT_REQUEST_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_PP_EPN_ESEM_MGMT_REQUEST_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_PP_TBL_ID_LAST].read_result_address = 
    INVALID_ADDRESS;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
