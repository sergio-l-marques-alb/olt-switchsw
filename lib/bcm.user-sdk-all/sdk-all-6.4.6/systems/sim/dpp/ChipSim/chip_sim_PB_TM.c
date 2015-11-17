/* $Id: chip_sim_PB_TM.c,v 1.6 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       chip_sim_PB_TM.c
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
#include "chip_sim_PB_TM.h" 
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h> 
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
* soc_pb_tm_indirect_blocks_init
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
*    *soc_pb_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
void
  soc_pb_tm_indirect_blocks_init(
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
  

  Soc_pb_indirect_blocks[SOC_PB_OLP_PGE_MEM_TBL_ID].read_result_address = 
    regs->olp.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->olp.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_OLP_PGE_MEM_TBL_ID].access_trig_offset = 
    regs->olp.indirect_command_reg.addr.base +
    0 * (regs->olp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_OLP_PGE_MEM_TBL_ID].access_address_offset = 
    regs->olp.indirect_command_address_reg.addr.base + 
    0 * (regs->olp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_OLP_PGE_MEM_TBL_ID].write_val_offset = 
    regs->olp.indirect_command_wr_data_reg.addr.base +
    0 * (regs->olp.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_OLP_PGE_MEM_TBL_ID].start_address = 
    ~SOC_PB_OLP_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_OLP_PGE_MEM_TBL_ID].end_address = 
    (~SOC_PB_OLP_MASK & 0x00010000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_OLP_PGE_MEM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_OLP_PGE_MEM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IRE_NIF_CTXT_MAP_TBL_ID].read_result_address = 
    regs->ire.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ire.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRE_NIF_CTXT_MAP_TBL_ID].access_trig_offset = 
    regs->ire.indirect_command_reg.addr.base +
    0 * (regs->ire.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRE_NIF_CTXT_MAP_TBL_ID].access_address_offset = 
    regs->ire.indirect_command_address_reg.addr.base + 
    0 * (regs->ire.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRE_NIF_CTXT_MAP_TBL_ID].write_val_offset = 
    regs->ire.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ire.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRE_NIF_CTXT_MAP_TBL_ID].start_address = 
    ~SOC_PB_IRE_MASK & 0x00004000;
  Soc_pb_indirect_blocks[SOC_PB_IRE_NIF_CTXT_MAP_TBL_ID].end_address = 
    (~SOC_PB_IRE_MASK & 0x00004000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IRE_NIF_CTXT_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IRE_NIF_CTXT_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ID].read_result_address = 
    regs->ire.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ire.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ID].access_trig_offset = 
    regs->ire.indirect_command_reg.addr.base +
    0 * (regs->ire.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ID].access_address_offset = 
    regs->ire.indirect_command_address_reg.addr.base + 
    0 * (regs->ire.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ID].write_val_offset = 
    regs->ire.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ire.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ID].start_address = 
    ~SOC_PB_IRE_MASK & 0x00005000;
  Soc_pb_indirect_blocks[SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ID].end_address = 
    (~SOC_PB_IRE_MASK & 0x00005000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IRE_RCY_CTXT_MAP_TBL_ID].read_result_address = 
    regs->ire.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ire.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRE_RCY_CTXT_MAP_TBL_ID].access_trig_offset = 
    regs->ire.indirect_command_reg.addr.base +
    0 * (regs->ire.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRE_RCY_CTXT_MAP_TBL_ID].access_address_offset = 
    regs->ire.indirect_command_address_reg.addr.base + 
    0 * (regs->ire.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRE_RCY_CTXT_MAP_TBL_ID].write_val_offset = 
    regs->ire.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ire.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRE_RCY_CTXT_MAP_TBL_ID].start_address = 
    ~SOC_PB_IRE_MASK & 0x00006000;
  Soc_pb_indirect_blocks[SOC_PB_IRE_RCY_CTXT_MAP_TBL_ID].end_address = 
    (~SOC_PB_IRE_MASK & 0x00006000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IRE_RCY_CTXT_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IRE_RCY_CTXT_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IRE_TDM_CONFIG_TBL_ID].read_result_address = 
    regs->ire.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ire.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRE_TDM_CONFIG_TBL_ID].access_trig_offset = 
    regs->ire.indirect_command_reg.addr.base +
    0 * (regs->ire.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRE_TDM_CONFIG_TBL_ID].access_address_offset = 
    regs->ire.indirect_command_address_reg.addr.base + 
    0 * (regs->ire.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRE_TDM_CONFIG_TBL_ID].write_val_offset = 
    regs->ire.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ire.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRE_TDM_CONFIG_TBL_ID].start_address = 
    ~SOC_PB_IRE_MASK & 0x00007000;
  Soc_pb_indirect_blocks[SOC_PB_IRE_TDM_CONFIG_TBL_ID].end_address = 
    (~SOC_PB_IRE_MASK & 0x00007000) + 0x0050 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IRE_TDM_CONFIG_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IRE_TDM_CONFIG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IDR_CONTEXT_MRU_TBL_ID].read_result_address = 
    regs->idr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->idr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_CONTEXT_MRU_TBL_ID].access_trig_offset = 
    regs->idr.indirect_command_reg.addr.base +
    0 * (regs->idr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_CONTEXT_MRU_TBL_ID].access_address_offset = 
    regs->idr.indirect_command_address_reg.addr.base + 
    0 * (regs->idr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_CONTEXT_MRU_TBL_ID].write_val_offset = 
    regs->idr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->idr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_CONTEXT_MRU_TBL_ID].start_address = 
    ~SOC_PB_IDR_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_IDR_CONTEXT_MRU_TBL_ID].end_address = 
    (~SOC_PB_IDR_MASK & 0x00010000) + 0x0050 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IDR_CONTEXT_MRU_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IDR_CONTEXT_MRU_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IDR_COMPLETE_PC_TBL_ID].read_result_address = 
    regs->idr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->idr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_COMPLETE_PC_TBL_ID].access_trig_offset = 
    regs->idr.indirect_command_reg.addr.base +
    0 * (regs->idr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_COMPLETE_PC_TBL_ID].access_address_offset = 
    regs->idr.indirect_command_address_reg.addr.base + 
    0 * (regs->idr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_COMPLETE_PC_TBL_ID].write_val_offset = 
    regs->idr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->idr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_COMPLETE_PC_TBL_ID].start_address = 
    ~SOC_PB_IDR_MASK & 0x00150000;
  Soc_pb_indirect_blocks[SOC_PB_IDR_COMPLETE_PC_TBL_ID].end_address = 
    (~SOC_PB_IDR_MASK & 0x00150000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IDR_COMPLETE_PC_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IDR_COMPLETE_PC_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_ID].read_result_address = 
    regs->idr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->idr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_ID].access_trig_offset = 
    regs->idr.indirect_command_reg.addr.base +
    0 * (regs->idr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_ID].access_address_offset = 
    regs->idr.indirect_command_address_reg.addr.base + 
    0 * (regs->idr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_ID].write_val_offset = 
    regs->idr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->idr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_ID].start_address = 
    ~SOC_PB_IDR_MASK & 0x001a0000;
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_ID].end_address = 
    (~SOC_PB_IDR_MASK & 0x001a0000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_ID].read_result_address = 
    regs->idr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->idr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_ID].access_trig_offset = 
    regs->idr.indirect_command_reg.addr.base +
    0 * (regs->idr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_ID].access_address_offset = 
    regs->idr.indirect_command_address_reg.addr.base + 
    0 * (regs->idr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_ID].write_val_offset = 
    regs->idr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->idr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_ID].start_address = 
    ~SOC_PB_IDR_MASK & 0x001c0000;
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_ID].end_address = 
    (~SOC_PB_IDR_MASK & 0x001c0000) + 0x0154 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_ID].read_result_address = 
    regs->idr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->idr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_ID].access_trig_offset = 
    regs->idr.indirect_command_reg.addr.base +
    0 * (regs->idr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_ID].access_address_offset = 
    regs->idr.indirect_command_address_reg.addr.base + 
    0 * (regs->idr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_ID].write_val_offset = 
    regs->idr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->idr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_ID].start_address = 
    ~SOC_PB_IDR_MASK & 0x001d0000;
  Soc_pb_indirect_blocks[SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_ID].end_address = 
    (~SOC_PB_IDR_MASK & 0x001d0000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_ID].read_result_address = 
    regs->idr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->idr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_ID].access_trig_offset = 
    regs->idr.indirect_command_reg.addr.base +
    0 * (regs->idr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_ID].access_address_offset = 
    regs->idr.indirect_command_address_reg.addr.base + 
    0 * (regs->idr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_ID].write_val_offset = 
    regs->idr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->idr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_ID].start_address = 
    ~SOC_PB_IDR_MASK & 0x001e0000;
  Soc_pb_indirect_blocks[SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_ID].end_address = 
    (~SOC_PB_IDR_MASK & 0x001e0000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IRR_IRDB_TBL_ID].read_result_address = 
    regs->irr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->irr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_IRDB_TBL_ID].access_trig_offset = 
    regs->irr.indirect_command_reg.addr.base +
    0 * (regs->irr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_IRDB_TBL_ID].access_address_offset = 
    regs->irr.indirect_command_address_reg.addr.base + 
    0 * (regs->irr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_IRDB_TBL_ID].write_val_offset = 
    regs->irr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->irr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_IRDB_TBL_ID].start_address = 
    ~SOC_PB_IRR_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_IRR_IRDB_TBL_ID].end_address = 
    (~SOC_PB_IRR_MASK & 0x00010000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IRR_IRDB_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IRR_IRDB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IRR_MCDB_TBL_ID].read_result_address = 
    regs->irr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->irr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_MCDB_TBL_ID].access_trig_offset = 
    regs->irr.indirect_command_reg.addr.base +
    0 * (regs->irr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_MCDB_TBL_ID].access_address_offset = 
    regs->irr.indirect_command_address_reg.addr.base + 
    0 * (regs->irr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_MCDB_TBL_ID].write_val_offset = 
    regs->irr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->irr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_MCDB_TBL_ID].start_address = 
    ~SOC_PB_IRR_MASK & 0x00020000;
  Soc_pb_indirect_blocks[SOC_PB_IRR_MCDB_TBL_ID].end_address = 
    (~SOC_PB_IRR_MASK & 0x00020000) + 0x8000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IRR_MCDB_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IRR_MCDB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_ID].read_result_address = 
    regs->irr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->irr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_ID].access_trig_offset = 
    regs->irr.indirect_command_reg.addr.base +
    0 * (regs->irr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_ID].access_address_offset = 
    regs->irr.indirect_command_address_reg.addr.base + 
    0 * (regs->irr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_ID].write_val_offset = 
    regs->irr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->irr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_ID].start_address = 
    ~SOC_PB_IRR_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_ID].end_address = 
    (~SOC_PB_IRR_MASK & 0x00030000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_ID].read_result_address = 
    regs->irr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->irr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_ID].access_trig_offset = 
    regs->irr.indirect_command_reg.addr.base +
    0 * (regs->irr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_ID].access_address_offset = 
    regs->irr.indirect_command_address_reg.addr.base + 
    0 * (regs->irr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_ID].write_val_offset = 
    regs->irr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->irr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_ID].start_address = 
    ~SOC_PB_IRR_MASK & 0x00040000;
  Soc_pb_indirect_blocks[SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_ID].end_address = 
    (~SOC_PB_IRR_MASK & 0x00040000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IRR_DESTINATION_TABLE_TBL_ID].read_result_address = 
    regs->irr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->irr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_DESTINATION_TABLE_TBL_ID].access_trig_offset = 
    regs->irr.indirect_command_reg.addr.base +
    0 * (regs->irr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_DESTINATION_TABLE_TBL_ID].access_address_offset = 
    regs->irr.indirect_command_address_reg.addr.base + 
    0 * (regs->irr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_DESTINATION_TABLE_TBL_ID].write_val_offset = 
    regs->irr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->irr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_DESTINATION_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IRR_MASK & 0x001c0000;
  Soc_pb_indirect_blocks[SOC_PB_IRR_DESTINATION_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IRR_MASK & 0x001c0000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IRR_DESTINATION_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IRR_DESTINATION_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_ID].read_result_address = 
    regs->irr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->irr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_ID].access_trig_offset = 
    regs->irr.indirect_command_reg.addr.base +
    0 * (regs->irr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_ID].access_address_offset = 
    regs->irr.indirect_command_address_reg.addr.base + 
    0 * (regs->irr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_ID].write_val_offset = 
    regs->irr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->irr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_ID].start_address = 
    ~SOC_PB_IRR_MASK & 0x00200000;
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_ID].end_address = 
    (~SOC_PB_IRR_MASK & 0x00200000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_MAPPING_TBL_ID].read_result_address = 
    regs->irr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->irr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_MAPPING_TBL_ID].access_trig_offset = 
    regs->irr.indirect_command_reg.addr.base +
    0 * (regs->irr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_MAPPING_TBL_ID].access_address_offset = 
    regs->irr.indirect_command_address_reg.addr.base + 
    0 * (regs->irr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_MAPPING_TBL_ID].write_val_offset = 
    regs->irr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->irr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_MAPPING_TBL_ID].start_address = 
    ~SOC_PB_IRR_MASK & 0x00220000;
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_MAPPING_TBL_ID].end_address = 
    (~SOC_PB_IRR_MASK & 0x00220000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_MAPPING_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_MAPPING_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_ID].read_result_address = 
    regs->irr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->irr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_ID].access_trig_offset = 
    regs->irr.indirect_command_reg.addr.base +
    0 * (regs->irr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_ID].access_address_offset = 
    regs->irr.indirect_command_address_reg.addr.base + 
    0 * (regs->irr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_ID].write_val_offset = 
    regs->irr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->irr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_ID].start_address = 
    ~SOC_PB_IRR_MASK & 0x00230000;
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_ID].end_address = 
    (~SOC_PB_IRR_MASK & 0x00230000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IRR_SMOOTH_DIVISION_TBL_ID].read_result_address = 
    regs->irr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->irr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_SMOOTH_DIVISION_TBL_ID].access_trig_offset = 
    regs->irr.indirect_command_reg.addr.base +
    0 * (regs->irr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_SMOOTH_DIVISION_TBL_ID].access_address_offset = 
    regs->irr.indirect_command_address_reg.addr.base + 
    0 * (regs->irr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_SMOOTH_DIVISION_TBL_ID].write_val_offset = 
    regs->irr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->irr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_SMOOTH_DIVISION_TBL_ID].start_address = 
    ~SOC_PB_IRR_MASK & 0x00240000;
  Soc_pb_indirect_blocks[SOC_PB_IRR_SMOOTH_DIVISION_TBL_ID].end_address = 
    (~SOC_PB_IRR_MASK & 0x00240000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IRR_SMOOTH_DIVISION_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IRR_SMOOTH_DIVISION_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_ID].read_result_address = 
    regs->irr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->irr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_ID].access_trig_offset = 
    regs->irr.indirect_command_reg.addr.base +
    0 * (regs->irr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_ID].access_address_offset = 
    regs->irr.indirect_command_address_reg.addr.base + 
    0 * (regs->irr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_ID].write_val_offset = 
    regs->irr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->irr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_ID].start_address = 
    ~SOC_PB_IRR_MASK & 0x00250000;
  Soc_pb_indirect_blocks[SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_ID].end_address = 
    (~SOC_PB_IRR_MASK & 0x00250000) + 0x000a - 1;
  Soc_pb_indirect_blocks[SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00000000;
  Soc_pb_indirect_blocks[SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00000000) + 0x0050 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00010000) + 0x0050 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHP_PP_PORT_INFO_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_PP_PORT_INFO_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_PP_PORT_INFO_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_PP_PORT_INFO_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_PP_PORT_INFO_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_IHP_PP_PORT_INFO_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00030000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHP_PP_PORT_INFO_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHP_PP_PORT_INFO_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHP_PP_PORT_VALUES_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_PP_PORT_VALUES_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_PP_PORT_VALUES_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_PP_PORT_VALUES_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_PP_PORT_VALUES_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x00040000;
  Soc_pb_indirect_blocks[SOC_PB_IHP_PP_PORT_VALUES_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x00040000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHP_PP_PORT_VALUES_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHP_PP_PORT_VALUES_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_ID].read_result_address = 
    regs->ihp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_ID].access_trig_offset = 
    regs->ihp.indirect_command_reg.addr.base +
    0 * (regs->ihp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_ID].access_address_offset = 
    regs->ihp.indirect_command_address_reg.addr.base + 
    0 * (regs->ihp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_ID].write_val_offset = 
    regs->ihp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_ID].start_address = 
    ~SOC_PB_IHP_MASK & 0x01310000;
  Soc_pb_indirect_blocks[SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_ID].end_address = 
    (~SOC_PB_IHP_MASK & 0x01310000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_PINFO_PMF_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_PINFO_PMF_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_PINFO_PMF_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_PINFO_PMF_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_PINFO_PMF_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00300000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_PINFO_PMF_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00300000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_PINFO_PMF_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_PINFO_PMF_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00310000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00310000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00410000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00410000) + 0x0017 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00440000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00440000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00450000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00450000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00470000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00470000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00480000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00480000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00490000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00490000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x004a0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x004a0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x004b0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x004b0000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x004c0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x004c0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x004d0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x004d0000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x004e0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x004e0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x004f0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x004f0000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00500000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00500000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00510000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00510000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00520000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00520000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    4;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00530000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00530000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00540000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00540000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00550000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00550000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00560000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00560000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00570000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00570000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    4;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00580000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00580000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00590000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00590000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x005a0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x005a0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x005b0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x005b0000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x005c0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x005c0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    4;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x005d0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x005d0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x005e0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x005e0000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x005f0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x005f0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00600000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00600000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00610000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00610000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    5;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00620000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00620000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00630000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00630000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00640000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00640000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00650000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00650000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00660000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00660000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    5;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00670000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00670000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00680000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00680000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00690000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00690000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x006a0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x006a0000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x006b0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x006b0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    5;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x006c0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x006c0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x006d0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x006d0000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x006e0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x006e0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x006f0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x006f0000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00700000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00700000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00710000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00710000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00720000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00720000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00730000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00730000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00740000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00740000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00750000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00750000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00760000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00760000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00770000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00770000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00780000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00780000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    4;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00790000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00790000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x007a0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x007a0000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x007b0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x007b0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x007c0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x007c0000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x007d0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x007d0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    4;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x007e0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x007e0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x007f0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x007f0000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00800000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00800000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00810000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00810000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00820000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00820000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    4;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00830000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00830000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00840000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00840000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00850000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00850000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00860000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00860000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00870000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00870000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    5;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00880000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00880000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00890000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00890000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x008a0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x008a0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x008b0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x008b0000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x008c0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x008c0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    5;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x008d0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x008d0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x008e0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x008e0000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x008f0000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x008f0000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00900000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00900000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00910000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00910000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_ID].nof_longs_to_move = 
    5;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00920000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00920000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_PINFO_LBP_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_PINFO_LBP_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_PINFO_LBP_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_PINFO_LBP_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_PINFO_LBP_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00d00000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_PINFO_LBP_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00d00000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_PINFO_LBP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_PINFO_LBP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IHB_HEADER_PROFILE_TBL_ID].read_result_address = 
    regs->ihb.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ihb.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_HEADER_PROFILE_TBL_ID].access_trig_offset = 
    regs->ihb.indirect_command_reg.addr.base +
    0 * (regs->ihb.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_HEADER_PROFILE_TBL_ID].access_address_offset = 
    regs->ihb.indirect_command_address_reg.addr.base + 
    0 * (regs->ihb.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_HEADER_PROFILE_TBL_ID].write_val_offset = 
    regs->ihb.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ihb.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IHB_HEADER_PROFILE_TBL_ID].start_address = 
    ~SOC_PB_IHB_MASK & 0x00d10000;
  Soc_pb_indirect_blocks[SOC_PB_IHB_HEADER_PROFILE_TBL_ID].end_address = 
    (~SOC_PB_IHB_MASK & 0x00d10000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IHB_HEADER_PROFILE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IHB_HEADER_PROFILE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_BDB_LINK_LIST_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_BDB_LINK_LIST_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_BDB_LINK_LIST_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_BDB_LINK_LIST_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_BDB_LINK_LIST_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x00000000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_BDB_LINK_LIST_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x00000000) + 0x10000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_BDB_LINK_LIST_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_BDB_LINK_LIST_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_DYNAMIC_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_DYNAMIC_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_DYNAMIC_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_DYNAMIC_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_DYNAMIC_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x00100000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_DYNAMIC_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x00100000) + 0x8000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_DYNAMIC_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_DYNAMIC_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_STATIC_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_STATIC_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_STATIC_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_STATIC_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_STATIC_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x00200000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_STATIC_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x00200000) + 0x8000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_STATIC_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_STATIC_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x00300000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x00300000) + 0x8000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x00400000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x00400000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x00500000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x00500000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_CREDIT_DISCOUNT_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_FULL_USER_COUNT_MEMORY_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_FULL_USER_COUNT_MEMORY_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_FULL_USER_COUNT_MEMORY_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_FULL_USER_COUNT_MEMORY_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_FULL_USER_COUNT_MEMORY_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x00600000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_FULL_USER_COUNT_MEMORY_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x00600000) + 0x8000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_FULL_USER_COUNT_MEMORY_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_FULL_USER_COUNT_MEMORY_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x00700000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x00700000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x00800000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x00800000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x01100000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x01100000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x01200000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x01200000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x01300000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x01300000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x01400000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x01400000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x01500000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x01500000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x01600000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x01600000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x01700000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x01700000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x01800000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x01800000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x01900000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x01900000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x01a00000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x01a00000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x01b00000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x01b00000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x01c00000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x01c00000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x01d00000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x01d00000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_A_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x01e00000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x01e00000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_B_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x01f00000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x01f00000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_C_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x02000000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x02000000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_D_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x02100000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x02100000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x02200000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x02200000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_B_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x02300000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x02300000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_C_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x02400000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x02400000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_D_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x02500000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x02500000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x02600000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x02600000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x02700000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x02700000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_TBL_ID].nof_longs_to_move = 
    6;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_SYSTEM_RED_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x02800000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x02800000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x02900000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x02900000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x02a00000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x02a00000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_ID].nof_longs_to_move = 
    5;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x03000000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x03000000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFSELA_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFSELA_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFSELA_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFSELA_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFSELA_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x03100000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFSELA_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x03100000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFSELA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_PRFSELA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFSELB_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFSELB_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFSELB_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFSELB_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFSELB_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x03200000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFSELB_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x03200000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFSELB_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_PRFSELB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFCFGA_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFCFGA_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFCFGA_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFCFGA_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFCFGA_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x03300000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFCFGA_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x03300000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFCFGA_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_PRFCFGA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFCFGB_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFCFGB_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFCFGB_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFCFGB_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFCFGB_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x03400000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFCFGB_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x03400000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_PRFCFGB_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_PRFCFGB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_NORMAL_DYNAMICA_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_NORMAL_DYNAMICA_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_NORMAL_DYNAMICA_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_NORMAL_DYNAMICA_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_NORMAL_DYNAMICA_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x03700000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_NORMAL_DYNAMICA_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x03700000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_NORMAL_DYNAMICA_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_NORMAL_DYNAMICA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_NORMAL_DYNAMICB_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_NORMAL_DYNAMICB_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_NORMAL_DYNAMICB_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_NORMAL_DYNAMICB_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_NORMAL_DYNAMICB_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x03800000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_NORMAL_DYNAMICB_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x03800000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_NORMAL_DYNAMICB_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_NORMAL_DYNAMICB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_HIGH_DYNAMICA_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_HIGH_DYNAMICA_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_HIGH_DYNAMICA_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_HIGH_DYNAMICA_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_HIGH_DYNAMICA_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x03900000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_HIGH_DYNAMICA_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x03900000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_HIGH_DYNAMICA_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_HIGH_DYNAMICA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_HIGH_DYNAMICB_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_HIGH_DYNAMICB_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_HIGH_DYNAMICB_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_HIGH_DYNAMICB_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_HIGH_DYNAMICB_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x03a00000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_HIGH_DYNAMICB_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x03a00000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_HIGH_DYNAMICB_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_HIGH_DYNAMICB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_CNTS_MEMA_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNTS_MEMA_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNTS_MEMA_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNTS_MEMA_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNTS_MEMA_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x03b00000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNTS_MEMA_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x03b00000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNTS_MEMA_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_CNTS_MEMA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_CNTS_MEMB_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNTS_MEMB_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNTS_MEMB_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNTS_MEMB_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNTS_MEMB_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x03c00000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNTS_MEMB_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x03c00000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_CNTS_MEMB_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_CNTS_MEMB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_OVTH_MEMA_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_OVTH_MEMA_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_OVTH_MEMA_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_OVTH_MEMA_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_OVTH_MEMA_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x03d00000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_OVTH_MEMA_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x03d00000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_OVTH_MEMA_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_OVTH_MEMA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IQM_OVTH_MEMB_TBL_ID].read_result_address = 
    regs->iqm.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->iqm.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_OVTH_MEMB_TBL_ID].access_trig_offset = 
    regs->iqm.indirect_command_reg.addr.base +
    0 * (regs->iqm.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_OVTH_MEMB_TBL_ID].access_address_offset = 
    regs->iqm.indirect_command_address_reg.addr.base + 
    0 * (regs->iqm.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_OVTH_MEMB_TBL_ID].write_val_offset = 
    regs->iqm.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->iqm.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IQM_OVTH_MEMB_TBL_ID].start_address = 
    ~SOC_PB_IQM_MASK & 0x03e00000;
  Soc_pb_indirect_blocks[SOC_PB_IQM_OVTH_MEMB_TBL_ID].end_address = 
    (~SOC_PB_IQM_MASK & 0x03e00000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IQM_OVTH_MEMB_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IQM_OVTH_MEMB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_QDR_QDR_MEM_TBL_ID].read_result_address = 
    regs->qdr.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->qdr.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_QDR_QDR_MEM_TBL_ID].access_trig_offset = 
    regs->qdr.indirect_command_reg.addr.base +
    0 * (regs->qdr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_QDR_QDR_MEM_TBL_ID].access_address_offset = 
    regs->qdr.indirect_command_address_reg.addr.base + 
    0 * (regs->qdr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_QDR_QDR_MEM_TBL_ID].write_val_offset = 
    regs->qdr.indirect_command_wr_data_reg.addr.base +
    0 * (regs->qdr.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_QDR_QDR_MEM_TBL_ID].start_address = 
    ~SOC_PB_QDR_MASK & 0x10000000;
  Soc_pb_indirect_blocks[SOC_PB_QDR_QDR_MEM_TBL_ID].end_address = 
    (~SOC_PB_QDR_MASK & 0x10000000) + 0x400000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_QDR_QDR_MEM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_QDR_QDR_MEM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_QDR_QDR_DLL_MEM_TBL_ID].read_result_address = 
    regs->qdr.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->qdr.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_QDR_QDR_DLL_MEM_TBL_ID].access_trig_offset = 
    regs->qdr.indirect_command_reg.addr.base +
    0 * (regs->qdr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_QDR_QDR_DLL_MEM_TBL_ID].access_address_offset = 
    regs->qdr.indirect_command_address_reg.addr.base + 
    0 * (regs->qdr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_QDR_QDR_DLL_MEM_TBL_ID].write_val_offset = 
    regs->qdr.indirect_command_wr_data_reg.addr.base +
    0 * (regs->qdr.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_QDR_QDR_DLL_MEM_TBL_ID].start_address = 
    ~SOC_PB_QDR_MASK & 0x20000000;
  Soc_pb_indirect_blocks[SOC_PB_QDR_QDR_DLL_MEM_TBL_ID].end_address = 
    (~SOC_PB_QDR_MASK & 0x20000000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_QDR_QDR_DLL_MEM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_QDR_QDR_DLL_MEM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID].read_result_address = 
    regs->ips.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ips.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID].access_trig_offset = 
    regs->ips.indirect_command_reg.addr.base +
    0 * (regs->ips.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID].access_address_offset = 
    regs->ips.indirect_command_address_reg.addr.base + 
    0 * (regs->ips.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID].write_val_offset = 
    regs->ips.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ips.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IPS_MASK & 0x00000000;
  Soc_pb_indirect_blocks[SOC_PB_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IPS_MASK & 0x00000000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID].read_result_address = 
    regs->ips.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ips.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID].access_trig_offset = 
    regs->ips.indirect_command_reg.addr.base +
    0 * (regs->ips.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID].access_address_offset = 
    regs->ips.indirect_command_address_reg.addr.base + 
    0 * (regs->ips.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID].write_val_offset = 
    regs->ips.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ips.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IPS_MASK & 0x00008000;
  Soc_pb_indirect_blocks[SOC_PB_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IPS_MASK & 0x00008000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID].read_result_address = 
    regs->ips.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ips.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID].access_trig_offset = 
    regs->ips.indirect_command_reg.addr.base +
    0 * (regs->ips.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID].access_address_offset = 
    regs->ips.indirect_command_address_reg.addr.base + 
    0 * (regs->ips.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID].write_val_offset = 
    regs->ips.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ips.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IPS_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IPS_MASK & 0x00010000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID].read_result_address = 
    regs->ips.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ips.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID].access_trig_offset = 
    regs->ips.indirect_command_reg.addr.base +
    0 * (regs->ips.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID].access_address_offset = 
    regs->ips.indirect_command_address_reg.addr.base + 
    0 * (regs->ips.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID].write_val_offset = 
    regs->ips.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ips.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IPS_MASK & 0x00018000;
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IPS_MASK & 0x00018000) + 0x8000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID].read_result_address = 
    regs->ips.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ips.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID].access_trig_offset = 
    regs->ips.indirect_command_reg.addr.base +
    0 * (regs->ips.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID].access_address_offset = 
    regs->ips.indirect_command_address_reg.addr.base + 
    0 * (regs->ips.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID].write_val_offset = 
    regs->ips.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ips.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID].start_address = 
    ~SOC_PB_IPS_MASK & 0x00020000;
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID].end_address = 
    (~SOC_PB_IPS_MASK & 0x00020000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID].read_result_address = 
    regs->ips.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ips.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID].access_trig_offset = 
    regs->ips.indirect_command_reg.addr.base +
    0 * (regs->ips.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID].access_address_offset = 
    regs->ips.indirect_command_address_reg.addr.base + 
    0 * (regs->ips.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID].write_val_offset = 
    regs->ips.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ips.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IPS_MASK & 0x00028000;
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IPS_MASK & 0x00028000) + 0x0004 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID].read_result_address = 
    regs->ips.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ips.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID].access_trig_offset = 
    regs->ips.indirect_command_reg.addr.base +
    0 * (regs->ips.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID].access_address_offset = 
    regs->ips.indirect_command_address_reg.addr.base + 
    0 * (regs->ips.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID].write_val_offset = 
    regs->ips.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ips.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IPS_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IPS_MASK & 0x00030000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID].read_result_address = 
    regs->ips.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ips.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID].access_trig_offset = 
    regs->ips.indirect_command_reg.addr.base +
    0 * (regs->ips.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID].access_address_offset = 
    regs->ips.indirect_command_address_reg.addr.base + 
    0 * (regs->ips.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID].write_val_offset = 
    regs->ips.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ips.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IPS_MASK & 0x00038000;
  Soc_pb_indirect_blocks[SOC_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IPS_MASK & 0x00038000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID].read_result_address = 
    regs->ips.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ips.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID].access_trig_offset = 
    regs->ips.indirect_command_reg.addr.base +
    0 * (regs->ips.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID].access_address_offset = 
    regs->ips.indirect_command_address_reg.addr.base + 
    0 * (regs->ips.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID].write_val_offset = 
    regs->ips.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ips.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IPS_MASK & 0x00040000;
  Soc_pb_indirect_blocks[SOC_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IPS_MASK & 0x00040000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID].read_result_address = 
    regs->ips.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ips.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID].access_trig_offset = 
    regs->ips.indirect_command_reg.addr.base +
    0 * (regs->ips.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID].access_address_offset = 
    regs->ips.indirect_command_address_reg.addr.base + 
    0 * (regs->ips.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID].write_val_offset = 
    regs->ips.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ips.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IPS_MASK & 0x00048000;
  Soc_pb_indirect_blocks[SOC_PB_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IPS_MASK & 0x00048000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID].read_result_address = 
    regs->ips.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ips.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID].access_trig_offset = 
    regs->ips.indirect_command_reg.addr.base +
    0 * (regs->ips.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID].access_address_offset = 
    regs->ips.indirect_command_address_reg.addr.base + 
    0 * (regs->ips.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID].write_val_offset = 
    regs->ips.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ips.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IPS_MASK & 0x00050000;
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IPS_MASK & 0x00050000) + 0x8000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_SIZE_TABLE_TBL_ID].read_result_address = 
    regs->ips.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ips.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_SIZE_TABLE_TBL_ID].access_trig_offset = 
    regs->ips.indirect_command_reg.addr.base +
    0 * (regs->ips.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_SIZE_TABLE_TBL_ID].access_address_offset = 
    regs->ips.indirect_command_address_reg.addr.base + 
    0 * (regs->ips.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_SIZE_TABLE_TBL_ID].write_val_offset = 
    regs->ips.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ips.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_SIZE_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IPS_MASK & 0x00058000;
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_SIZE_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IPS_MASK & 0x00058000) + 0x8000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_SIZE_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPS_QUEUE_SIZE_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID].read_result_address = 
    regs->ips.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ips.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID].access_trig_offset = 
    regs->ips.indirect_command_reg.addr.base +
    0 * (regs->ips.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID].access_address_offset = 
    regs->ips.indirect_command_address_reg.addr.base + 
    0 * (regs->ips.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID].write_val_offset = 
    regs->ips.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ips.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID].start_address = 
    ~SOC_PB_IPS_MASK & 0x00078000;
  Soc_pb_indirect_blocks[SOC_PB_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID].end_address = 
    (~SOC_PB_IPS_MASK & 0x00078000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPT_BDQ_TBL_ID].read_result_address = 
    regs->ipt.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ipt.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_BDQ_TBL_ID].access_trig_offset = 
    regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->ipt.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_BDQ_TBL_ID].access_address_offset = 
    regs->ipt.indirect_command_address_reg.addr.base + 
    0 * (regs->ipt.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_BDQ_TBL_ID].write_val_offset = 
    regs->ipt.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ipt.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_BDQ_TBL_ID].start_address = 
    ~SOC_PB_IPT_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_IPT_BDQ_TBL_ID].end_address = 
    (~SOC_PB_IPT_MASK & 0x00010000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPT_BDQ_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPT_BDQ_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPT_PCQ_TBL_ID].read_result_address = 
    regs->ipt.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ipt.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_PCQ_TBL_ID].access_trig_offset = 
    regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->ipt.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_PCQ_TBL_ID].access_address_offset = 
    regs->ipt.indirect_command_address_reg.addr.base + 
    0 * (regs->ipt.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_PCQ_TBL_ID].write_val_offset = 
    regs->ipt.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ipt.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_PCQ_TBL_ID].start_address = 
    ~SOC_PB_IPT_MASK & 0x00020000;
  Soc_pb_indirect_blocks[SOC_PB_IPT_PCQ_TBL_ID].end_address = 
    (~SOC_PB_IPT_MASK & 0x00020000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPT_PCQ_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPT_PCQ_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPT_SOP_MMU_TBL_ID].read_result_address = 
    regs->ipt.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ipt.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_SOP_MMU_TBL_ID].access_trig_offset = 
    regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->ipt.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_SOP_MMU_TBL_ID].access_address_offset = 
    regs->ipt.indirect_command_address_reg.addr.base + 
    0 * (regs->ipt.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_SOP_MMU_TBL_ID].write_val_offset = 
    regs->ipt.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ipt.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_SOP_MMU_TBL_ID].start_address = 
    ~SOC_PB_IPT_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_IPT_SOP_MMU_TBL_ID].end_address = 
    (~SOC_PB_IPT_MASK & 0x00030000) + 0x0600 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPT_SOP_MMU_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPT_SOP_MMU_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPT_MOP_MMU_TBL_ID].read_result_address = 
    regs->ipt.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ipt.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_MOP_MMU_TBL_ID].access_trig_offset = 
    regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->ipt.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_MOP_MMU_TBL_ID].access_address_offset = 
    regs->ipt.indirect_command_address_reg.addr.base + 
    0 * (regs->ipt.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_MOP_MMU_TBL_ID].write_val_offset = 
    regs->ipt.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ipt.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_MOP_MMU_TBL_ID].start_address = 
    ~SOC_PB_IPT_MASK & 0x00040000;
  Soc_pb_indirect_blocks[SOC_PB_IPT_MOP_MMU_TBL_ID].end_address = 
    (~SOC_PB_IPT_MASK & 0x00040000) + 0x0600 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPT_MOP_MMU_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPT_MOP_MMU_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPT_FDTCTL_TBL_ID].read_result_address = 
    regs->ipt.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ipt.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_FDTCTL_TBL_ID].access_trig_offset = 
    regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->ipt.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_FDTCTL_TBL_ID].access_address_offset = 
    regs->ipt.indirect_command_address_reg.addr.base + 
    0 * (regs->ipt.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_FDTCTL_TBL_ID].write_val_offset = 
    regs->ipt.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ipt.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_FDTCTL_TBL_ID].start_address = 
    ~SOC_PB_IPT_MASK & 0x00050000;
  Soc_pb_indirect_blocks[SOC_PB_IPT_FDTCTL_TBL_ID].end_address = 
    (~SOC_PB_IPT_MASK & 0x00050000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPT_FDTCTL_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPT_FDTCTL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPT_FDTDATA_TBL_ID].read_result_address = 
    regs->ipt.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ipt.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_FDTDATA_TBL_ID].access_trig_offset = 
    regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->ipt.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_FDTDATA_TBL_ID].access_address_offset = 
    regs->ipt.indirect_command_address_reg.addr.base + 
    0 * (regs->ipt.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_FDTDATA_TBL_ID].write_val_offset = 
    regs->ipt.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ipt.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_FDTDATA_TBL_ID].start_address = 
    ~SOC_PB_IPT_MASK & 0x00060000;
  Soc_pb_indirect_blocks[SOC_PB_IPT_FDTDATA_TBL_ID].end_address = 
    (~SOC_PB_IPT_MASK & 0x00060000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPT_FDTDATA_TBL_ID].nof_longs_to_move = 
    16;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPT_FDTDATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPT_EGQCTL_TBL_ID].read_result_address = 
    regs->ipt.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ipt.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_EGQCTL_TBL_ID].access_trig_offset = 
    regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->ipt.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_EGQCTL_TBL_ID].access_address_offset = 
    regs->ipt.indirect_command_address_reg.addr.base + 
    0 * (regs->ipt.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_EGQCTL_TBL_ID].write_val_offset = 
    regs->ipt.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ipt.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_EGQCTL_TBL_ID].start_address = 
    ~SOC_PB_IPT_MASK & 0x00070000;
  Soc_pb_indirect_blocks[SOC_PB_IPT_EGQCTL_TBL_ID].end_address = 
    (~SOC_PB_IPT_MASK & 0x00070000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPT_EGQCTL_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPT_EGQCTL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPT_EGQDATA_TBL_ID].read_result_address = 
    regs->ipt.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ipt.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_EGQDATA_TBL_ID].access_trig_offset = 
    regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->ipt.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_EGQDATA_TBL_ID].access_address_offset = 
    regs->ipt.indirect_command_address_reg.addr.base + 
    0 * (regs->ipt.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_EGQDATA_TBL_ID].write_val_offset = 
    regs->ipt.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ipt.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_EGQDATA_TBL_ID].start_address = 
    ~SOC_PB_IPT_MASK & 0x00080000;
  Soc_pb_indirect_blocks[SOC_PB_IPT_EGQDATA_TBL_ID].end_address = 
    (~SOC_PB_IPT_MASK & 0x00080000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPT_EGQDATA_TBL_ID].nof_longs_to_move = 
    16;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPT_EGQDATA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPT_SELECT_SOURCE_COUNTER_TBL_ID].read_result_address = 
    regs->ipt.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ipt.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_SELECT_SOURCE_COUNTER_TBL_ID].access_trig_offset = 
    regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->ipt.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_SELECT_SOURCE_COUNTER_TBL_ID].access_address_offset = 
    regs->ipt.indirect_command_address_reg.addr.base + 
    0 * (regs->ipt.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_SELECT_SOURCE_COUNTER_TBL_ID].write_val_offset = 
    regs->ipt.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ipt.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_SELECT_SOURCE_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_IPT_MASK & 0x00090000;
  Soc_pb_indirect_blocks[SOC_PB_IPT_SELECT_SOURCE_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_IPT_MASK & 0x00090000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPT_SELECT_SOURCE_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPT_SELECT_SOURCE_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IPT_SELECT_SOURCE_SUM_TBL_ID].read_result_address = 
    regs->ipt.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->ipt.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_SELECT_SOURCE_SUM_TBL_ID].access_trig_offset = 
    regs->ipt.indirect_command_reg.addr.base +
    0 * (regs->ipt.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_SELECT_SOURCE_SUM_TBL_ID].access_address_offset = 
    regs->ipt.indirect_command_address_reg.addr.base + 
    0 * (regs->ipt.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_SELECT_SOURCE_SUM_TBL_ID].write_val_offset = 
    regs->ipt.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->ipt.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IPT_SELECT_SOURCE_SUM_TBL_ID].start_address = 
    ~SOC_PB_IPT_MASK & 0x000a0000;
  Soc_pb_indirect_blocks[SOC_PB_IPT_SELECT_SOURCE_SUM_TBL_ID].end_address = 
    (~SOC_PB_IPT_MASK & 0x000a0000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IPT_SELECT_SOURCE_SUM_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IPT_SELECT_SOURCE_SUM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_DRAM_ADDRESS_SPACE_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_DRAM_ADDRESS_SPACE_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_DRAM_ADDRESS_SPACE_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_DRAM_ADDRESS_SPACE_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_DRAM_ADDRESS_SPACE_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x00000000;
#if 0
  /* in current implementation, requires allocation of 4GB */
  Soc_pb_indirect_blocks[SOC_PB_MMU_DRAM_ADDRESS_SPACE_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x00000000) + 0x4000000 - 1;
#else
  /* Allocate table of size 1 for now */
  Soc_pb_indirect_blocks[SOC_PB_MMU_DRAM_ADDRESS_SPACE_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x00000000) + 0x2 - 1;
#endif
  Soc_pb_indirect_blocks[SOC_PB_MMU_DRAM_ADDRESS_SPACE_TBL_ID].nof_longs_to_move = 
    16;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_DRAM_ADDRESS_SPACE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_IDF_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_IDF_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_IDF_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_IDF_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_IDF_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x04000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_IDF_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x04000000) + 0x0400 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_IDF_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_IDF_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_FDF_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_FDF_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_FDF_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_FDF_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_FDF_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x08000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_FDF_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x08000000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_FDF_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_FDF_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_WAF_HALFA_WADDR_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAF_HALFA_WADDR_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAF_HALFA_WADDR_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAF_HALFA_WADDR_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAF_HALFA_WADDR_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x0c000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAF_HALFA_WADDR_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x0c000000) + 0x0030 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAF_HALFA_WADDR_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_WAF_HALFA_WADDR_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_WAF_HALFB_WADDR_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAF_HALFB_WADDR_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAF_HALFB_WADDR_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAF_HALFB_WADDR_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAF_HALFB_WADDR_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x10000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAF_HALFB_WADDR_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x10000000) + 0x0030 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAF_HALFB_WADDR_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_WAF_HALFB_WADDR_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFA_HALFA_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFA_HALFA_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFA_HALFA_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFA_HALFA_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFA_HALFA_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x14000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFA_HALFA_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x14000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFA_HALFA_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_WAFA_HALFA_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFB_HALFA_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFB_HALFA_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFB_HALFA_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFB_HALFA_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFB_HALFA_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x18000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFB_HALFA_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x18000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFB_HALFA_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_WAFB_HALFA_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFC_HALFA_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFC_HALFA_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFC_HALFA_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFC_HALFA_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFC_HALFA_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x1c000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFC_HALFA_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x1c000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFC_HALFA_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_WAFC_HALFA_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFD_HALFA_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFD_HALFA_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFD_HALFA_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFD_HALFA_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFD_HALFA_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x20000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFD_HALFA_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x20000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFD_HALFA_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_WAFD_HALFA_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFE_HALFA_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFE_HALFA_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFE_HALFA_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFE_HALFA_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFE_HALFA_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x24000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFE_HALFA_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x24000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFE_HALFA_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_WAFE_HALFA_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFF_HALFA_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFF_HALFA_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFF_HALFA_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFF_HALFA_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFF_HALFA_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x28000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFF_HALFA_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x28000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFF_HALFA_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_WAFF_HALFA_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFA_HALFB_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFA_HALFB_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFA_HALFB_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFA_HALFB_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFA_HALFB_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x2c000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFA_HALFB_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x2c000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFA_HALFB_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_WAFA_HALFB_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFB_HALFB_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFB_HALFB_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFB_HALFB_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFB_HALFB_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFB_HALFB_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x30000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFB_HALFB_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x30000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFB_HALFB_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_WAFB_HALFB_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFC_HALFB_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFC_HALFB_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFC_HALFB_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFC_HALFB_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFC_HALFB_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x34000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFC_HALFB_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x34000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFC_HALFB_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_WAFC_HALFB_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFD_HALFB_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFD_HALFB_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFD_HALFB_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFD_HALFB_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFD_HALFB_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x38000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFD_HALFB_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x38000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFD_HALFB_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_WAFD_HALFB_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFE_HALFB_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFE_HALFB_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFE_HALFB_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFE_HALFB_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFE_HALFB_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x3c000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFE_HALFB_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x3c000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFE_HALFB_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_WAFE_HALFB_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFF_HALFB_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFF_HALFB_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFF_HALFB_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFF_HALFB_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFF_HALFB_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x40000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFF_HALFB_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x40000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_WAFF_HALFB_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_WAFF_HALFB_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_RAF_WADDR_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAF_WADDR_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAF_WADDR_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAF_WADDR_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAF_WADDR_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x44000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAF_WADDR_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x44000000) + 0x0030 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAF_WADDR_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_RAF_WADDR_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFA_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFA_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFA_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFA_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFA_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x48000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFA_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x48000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFA_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_RAFA_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFB_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFB_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFB_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFB_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFB_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x4c000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFB_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x4c000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFB_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_RAFB_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFC_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFC_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFC_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFC_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFC_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x50000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFC_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x50000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFC_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_RAFC_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFD_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFD_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFD_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFD_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFD_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x54000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFD_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x54000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFD_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_RAFD_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFE_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFE_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFE_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFE_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFE_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x58000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFE_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x58000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFE_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_RAFE_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFF_RADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFF_RADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFF_RADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFF_RADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFF_RADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x5c000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFF_RADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x5c000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RAFF_RADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_RAFF_RADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFA_WADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFA_WADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFA_WADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFA_WADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFA_WADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x60000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFA_WADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x60000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFA_WADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_RDFA_WADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFB_WADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFB_WADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFB_WADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFB_WADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFB_WADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x64000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFB_WADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x64000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFB_WADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_RDFB_WADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFC_WADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFC_WADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFC_WADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFC_WADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFC_WADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x68000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFC_WADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x68000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFC_WADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_RDFC_WADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFD_WADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFD_WADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFD_WADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFD_WADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFD_WADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x6c000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFD_WADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x6c000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFD_WADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_RDFD_WADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFE_WADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFE_WADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFE_WADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFE_WADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFE_WADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x70000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFE_WADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x70000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFE_WADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_RDFE_WADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFF_WADDR_STATUS_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFF_WADDR_STATUS_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFF_WADDR_STATUS_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFF_WADDR_STATUS_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFF_WADDR_STATUS_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x74000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFF_WADDR_STATUS_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x74000000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDFF_WADDR_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_RDFF_WADDR_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_MMU_RDF_RADDR_TBL_ID].read_result_address = 
    regs->mmu.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->mmu.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDF_RADDR_TBL_ID].access_trig_offset = 
    regs->mmu.indirect_command_reg.addr.base +
    0 * (regs->mmu.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDF_RADDR_TBL_ID].access_address_offset = 
    regs->mmu.indirect_command_address_reg.addr.base + 
    0 * (regs->mmu.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDF_RADDR_TBL_ID].write_val_offset = 
    regs->mmu.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->mmu.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDF_RADDR_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x78000000;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDF_RADDR_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x78000000) + 0x0030 - 1;
  Soc_pb_indirect_blocks[SOC_PB_MMU_RDF_RADDR_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_MMU_RDF_RADDR_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_A_ID].read_result_address = 
    regs->dpi.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->dpi.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_A_ID].access_trig_offset = 
    regs->dpi.indirect_command_reg.addr.base +
    0 * (regs->dpi.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_A_ID].access_address_offset = 
    regs->dpi.indirect_command_address_reg.addr.base + 
    0 * (regs->dpi.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_A_ID].write_val_offset = 
    regs->dpi.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->dpi.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_A_ID].start_address = 
    ~SOC_PB_DPI_A_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_A_ID].end_address = 
    (~SOC_PB_DPI_A_MASK & 0x00010000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_A_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_A_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_B_ID].read_result_address = 
    regs->dpi.indirect_command_rd_data_reg[0].addr.base + 
    1 * (regs->dpi.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_B_ID].access_trig_offset = 
    regs->dpi.indirect_command_reg.addr.base +
    1 * (regs->dpi.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_B_ID].access_address_offset = 
    regs->dpi.indirect_command_address_reg.addr.base + 
    1 * (regs->dpi.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_B_ID].write_val_offset = 
    regs->dpi.indirect_command_wr_data_reg[0].addr.base +
    1 * (regs->dpi.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_B_ID].start_address = 
    ~SOC_PB_DPI_B_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_B_ID].end_address = 
    (~SOC_PB_DPI_B_MASK & 0x00010000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_B_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_B_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_C_ID].read_result_address = 
    regs->dpi.indirect_command_rd_data_reg[0].addr.base + 
    2 * (regs->dpi.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_C_ID].access_trig_offset = 
    regs->dpi.indirect_command_reg.addr.base +
    2 * (regs->dpi.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_C_ID].access_address_offset = 
    regs->dpi.indirect_command_address_reg.addr.base + 
    2 * (regs->dpi.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_C_ID].write_val_offset = 
    regs->dpi.indirect_command_wr_data_reg[0].addr.base +
    2 * (regs->dpi.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_C_ID].start_address = 
    ~SOC_PB_DPI_C_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_C_ID].end_address = 
    (~SOC_PB_DPI_C_MASK & 0x00010000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_C_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_C_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_D_ID].read_result_address = 
    regs->dpi.indirect_command_rd_data_reg[0].addr.base + 
    3 * (regs->dpi.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_D_ID].access_trig_offset = 
    regs->dpi.indirect_command_reg.addr.base +
    3 * (regs->dpi.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_D_ID].access_address_offset = 
    regs->dpi.indirect_command_address_reg.addr.base + 
    3 * (regs->dpi.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_D_ID].write_val_offset = 
    regs->dpi.indirect_command_wr_data_reg[0].addr.base +
    3 * (regs->dpi.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_D_ID].start_address = 
    ~SOC_PB_DPI_D_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_D_ID].end_address = 
    (~SOC_PB_DPI_D_MASK & 0x00010000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_D_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_D_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_E_ID].read_result_address = 
    regs->dpi.indirect_command_rd_data_reg[0].addr.base + 
    4 * (regs->dpi.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_E_ID].access_trig_offset = 
    regs->dpi.indirect_command_reg.addr.base +
    4 * (regs->dpi.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_E_ID].access_address_offset = 
    regs->dpi.indirect_command_address_reg.addr.base + 
    4 * (regs->dpi.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_E_ID].write_val_offset = 
    regs->dpi.indirect_command_wr_data_reg[0].addr.base +
    4 * (regs->dpi.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_E_ID].start_address = 
    ~SOC_PB_DPI_E_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_E_ID].end_address = 
    (~SOC_PB_DPI_E_MASK & 0x00010000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_E_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_E_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_F_ID].read_result_address = 
    regs->dpi.indirect_command_rd_data_reg[0].addr.base + 
    5 * (regs->dpi.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_F_ID].access_trig_offset = 
    regs->dpi.indirect_command_reg.addr.base +
    5 * (regs->dpi.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_F_ID].access_address_offset = 
    regs->dpi.indirect_command_address_reg.addr.base + 
    5 * (regs->dpi.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_F_ID].write_val_offset = 
    regs->dpi.indirect_command_wr_data_reg[0].addr.base +
    5 * (regs->dpi.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_F_ID].start_address = 
    ~SOC_PB_DPI_F_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_F_ID].end_address = 
    (~SOC_PB_DPI_F_MASK & 0x00010000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_F_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_DPI_DLL_RAM_TBL_F_ID].base[indx] = 
      NULL;
  }

#if 0
  Soc_pb_indirect_blocks[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ID].read_result_address = 
    regs->fdt.indirect_command_rd_data_reg_0.addr.base + 
    0 * (regs->fdt.indirect_command_rd_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ID].access_trig_offset = 
    regs->fdt.indirect_command_reg.addr.base +
    0 * (regs->fdt.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ID].access_address_offset = 
    regs->fdt.indirect_command_address_reg.addr.base + 
    0 * (regs->fdt.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ID].write_val_offset = 
    regs->fdt.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->fdt.indirect_command_wr_data_reg_0.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ID].start_address = 
    ~SOC_PB_FDT_MASK & 0x00000000;
  Soc_pb_indirect_blocks[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ID].end_address = 
    (~SOC_PB_FDT_MASK & 0x00000000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ID].base[indx] = 
      NULL;
  }
#endif

  Soc_pb_indirect_blocks[SOC_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID].read_result_address = 
    regs->rtp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->rtp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID].access_trig_offset = 
    regs->rtp.indirect_command_reg.addr.base +
    0 * (regs->rtp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID].access_address_offset = 
    regs->rtp.indirect_command_address_reg.addr.base + 
    0 * (regs->rtp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID].write_val_offset = 
    regs->rtp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->rtp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID].start_address = 
    ~SOC_PB_RTP_MASK & 0x00000000;
  Soc_pb_indirect_blocks[SOC_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID].end_address = 
    (~SOC_PB_RTP_MASK & 0x00000000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID].read_result_address = 
    regs->rtp.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->rtp.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID].access_trig_offset = 
    regs->rtp.indirect_command_reg.addr.base +
    0 * (regs->rtp.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID].access_address_offset = 
    regs->rtp.indirect_command_address_reg.addr.base + 
    0 * (regs->rtp.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID].write_val_offset = 
    regs->rtp.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->rtp.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID].start_address = 
    ~SOC_PB_RTP_MASK & 0x00001000;
  Soc_pb_indirect_blocks[SOC_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID].end_address = 
    (~SOC_PB_RTP_MASK & 0x00001000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL0_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL0_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL0_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL0_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL0_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL0_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00010000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL0_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL0_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL1_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL1_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL1_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL1_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL1_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00020000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL1_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00020000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL1_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL1_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL2_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL2_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL2_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL2_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL2_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL2_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00030000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL2_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL2_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL3_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL3_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL3_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL3_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL3_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00040000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL3_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00040000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL3_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL3_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL4_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL4_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL4_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL4_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL4_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00050000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL4_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00050000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL4_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL4_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL5_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL5_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL5_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL5_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL5_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00060000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL5_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00060000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL5_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL5_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL6_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL6_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL6_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL6_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL6_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00070000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL6_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00070000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL6_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL6_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL7_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL7_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL7_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL7_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL7_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00080000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL7_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00080000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL7_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL7_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL8_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL8_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL8_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL8_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL8_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00090000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL8_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00090000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL8_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL8_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL9_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL9_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL9_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL9_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL9_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x000a0000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL9_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x000a0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL9_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL9_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL10_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL10_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL10_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL10_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL10_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x000b0000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL10_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x000b0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL10_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL10_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL11_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL11_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL11_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL11_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL11_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x000c0000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL11_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x000c0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL11_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL11_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL12_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL12_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL12_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL12_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL12_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x000d0000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL12_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x000d0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL12_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL12_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL13_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL13_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL13_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL13_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL13_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x000e0000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL13_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x000e0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL13_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL13_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL14_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL14_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL14_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL14_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL14_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x000f0000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL14_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x000f0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL14_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL14_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL15_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL15_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL15_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL15_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL15_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00100000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL15_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00100000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL15_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIF_MAL15_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_RCY_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RCY_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RCY_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RCY_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RCY_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00110000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RCY_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00110000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RCY_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_RCY_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_SCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_SCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_SCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_SCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_SCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00120000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_SCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00120000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_SCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_SCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_CCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00130000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00130000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_CCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_PMC_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PMC_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PMC_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PMC_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PMC_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00140000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PMC_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00140000) + 0x00a0 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PMC_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_PMC_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_CBM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CBM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CBM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CBM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CBM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00150000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CBM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00150000) + 0x0050 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CBM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_CBM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_FBM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FBM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FBM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FBM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FBM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00200000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FBM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00200000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FBM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_FBM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00210000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00210000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDM_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_FDM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_QM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_QM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_QM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_QM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_QM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00220000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_QM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00220000) + 0x0280 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_QM_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_QM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_QSM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_QSM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_QSM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_QSM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_QSM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00230000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_QSM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00230000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_QSM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_QSM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_DCM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DCM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DCM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DCM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DCM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00240000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DCM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00240000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DCM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_DCM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_DWM0_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DWM0_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DWM0_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DWM0_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DWM0_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00250000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DWM0_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00250000) + 0x0050 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DWM0_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_DWM0_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_DWM1_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DWM1_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DWM1_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DWM1_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DWM1_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00260000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DWM1_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00260000) + 0x0050 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_DWM1_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_DWM1_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_RRDM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RRDM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RRDM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RRDM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RRDM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00270000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RRDM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00270000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RRDM_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_RRDM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_RPDM_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RPDM_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RPDM_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RPDM_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RPDM_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00280000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RPDM_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00280000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RPDM_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_RPDM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_PCT_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PCT_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PCT_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PCT_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PCT_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00290000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PCT_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00290000) + 0x0050 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PCT_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_PCT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_VLAN_TABLE_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_VLAN_TABLE_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_VLAN_TABLE_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_VLAN_TABLE_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_VLAN_TABLE_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x002a0000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_VLAN_TABLE_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x002a0000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_VLAN_TABLE_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_VLAN_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_TC_DP_MAP_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_TC_DP_MAP_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_TC_DP_MAP_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_TC_DP_MAP_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_TC_DP_MAP_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x002d0000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_TC_DP_MAP_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x002d0000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_TC_DP_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_TC_DP_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_CFC_FLOW_CONTROL_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CFC_FLOW_CONTROL_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CFC_FLOW_CONTROL_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CFC_FLOW_CONTROL_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CFC_FLOW_CONTROL_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00300000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CFC_FLOW_CONTROL_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00300000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CFC_FLOW_CONTROL_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_CFC_FLOW_CONTROL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIFA_FLOW_CONTROL_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIFA_FLOW_CONTROL_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIFA_FLOW_CONTROL_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIFA_FLOW_CONTROL_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIFA_FLOW_CONTROL_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00310000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIFA_FLOW_CONTROL_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00310000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIFA_FLOW_CONTROL_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIFA_FLOW_CONTROL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIFB_FLOW_CONTROL_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIFB_FLOW_CONTROL_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIFB_FLOW_CONTROL_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIFB_FLOW_CONTROL_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIFB_FLOW_CONTROL_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00320000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIFB_FLOW_CONTROL_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00320000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_NIFB_FLOW_CONTROL_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_NIFB_FLOW_CONTROL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_LAST_HEADER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_LAST_HEADER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_LAST_HEADER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_LAST_HEADER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_LAST_HEADER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00330000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_LAST_HEADER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00330000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_LAST_HEADER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_LAST_HEADER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_IPT_LAST_HEADER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_IPT_LAST_HEADER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_IPT_LAST_HEADER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_IPT_LAST_HEADER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_IPT_LAST_HEADER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00340000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_IPT_LAST_HEADER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00340000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_IPT_LAST_HEADER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_IPT_LAST_HEADER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDR_LAST_HEADER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDR_LAST_HEADER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDR_LAST_HEADER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDR_LAST_HEADER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDR_LAST_HEADER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00350000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDR_LAST_HEADER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00350000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDR_LAST_HEADER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_FDR_LAST_HEADER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00360000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00360000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_CPU_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_IPT_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_IPT_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_IPT_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_IPT_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_IPT_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00370000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_IPT_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00370000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_IPT_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_IPT_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDR_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDR_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDR_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDR_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDR_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00380000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDR_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00380000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FDR_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_FDR_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_RQP_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RQP_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RQP_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RQP_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RQP_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00390000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RQP_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00390000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RQP_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_RQP_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x003a0000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x003a0000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x003b0000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x003b0000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x003c0000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x003c0000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x003d0000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x003d0000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x003e0000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x003e0000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x003f0000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x003f0000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00400000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00400000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00410000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00410000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00420000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00420000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00430000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00430000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00440000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00440000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00450000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00450000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00460000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00460000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00470000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00470000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00480000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00480000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_FQP_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FQP_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FQP_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FQP_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FQP_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00490000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FQP_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00490000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FQP_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_FQP_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x004a0000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x004a0000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_CFM_TRAP_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CFM_TRAP_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CFM_TRAP_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CFM_TRAP_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CFM_TRAP_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00640000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CFM_TRAP_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00640000) + 0x4000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_CFM_TRAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_CFM_TRAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00650000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00650000) + 0x0040 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_ID].read_result_address = 
    regs->egq.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->egq.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_ID].access_trig_offset = 
    regs->egq.indirect_command_reg.addr.base +
    0 * (regs->egq.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_ID].access_address_offset = 
    regs->egq.indirect_command_address_reg.addr.base + 
    0 * (regs->egq.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_ID].write_val_offset = 
    regs->egq.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->egq.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_ID].start_address = 
    ~SOC_PB_EGQ_MASK & 0x00660000;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_ID].end_address = 
    (~SOC_PB_EGQ_MASK & 0x00660000) + 0x0008 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EPNI_EPE_PACKET_COUNTER_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_EPE_PACKET_COUNTER_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_EPE_PACKET_COUNTER_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_EPE_PACKET_COUNTER_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_EPE_PACKET_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_EPE_PACKET_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00010000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_EPE_PACKET_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EPNI_EPE_PACKET_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EPNI_EPE_BYTES_COUNTER_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_EPE_BYTES_COUNTER_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_EPE_BYTES_COUNTER_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_EPE_BYTES_COUNTER_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_EPE_BYTES_COUNTER_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00020000;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_EPE_BYTES_COUNTER_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00020000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_EPE_BYTES_COUNTER_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EPNI_EPE_BYTES_COUNTER_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EPNI_ENCAPSULATION_DB_BANK1_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_ENCAPSULATION_DB_BANK1_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_ENCAPSULATION_DB_BANK1_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_ENCAPSULATION_DB_BANK1_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_ENCAPSULATION_DB_BANK1_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_ENCAPSULATION_DB_BANK1_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00030000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_ENCAPSULATION_DB_BANK1_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EPNI_ENCAPSULATION_DB_BANK1_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EPNI_ENCAPSULATION_DB_BANK2_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_ENCAPSULATION_DB_BANK2_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_ENCAPSULATION_DB_BANK2_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_ENCAPSULATION_DB_BANK2_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_ENCAPSULATION_DB_BANK2_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00040000;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_ENCAPSULATION_DB_BANK2_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00040000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_ENCAPSULATION_DB_BANK2_TBL_ID].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EPNI_ENCAPSULATION_DB_BANK2_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EPNI_TX_TAG_TABLE_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_TX_TAG_TABLE_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_TX_TAG_TABLE_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_TX_TAG_TABLE_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_TX_TAG_TABLE_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00050000;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_TX_TAG_TABLE_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00050000) + 0x1000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_TX_TAG_TABLE_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EPNI_TX_TAG_TABLE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x000d0000;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x000d0000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE1_PROGRAM_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE1_PROGRAM_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE1_PROGRAM_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE1_PROGRAM_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE1_PROGRAM_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x000e0000;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE1_PROGRAM_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x000e0000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE1_PROGRAM_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE1_PROGRAM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE2_PROGRAM_TBL_ID].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE2_PROGRAM_TBL_ID].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE2_PROGRAM_TBL_ID].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE2_PROGRAM_TBL_ID].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE2_PROGRAM_TBL_ID].start_address = 
    ~SOC_PB_EPNI_MASK & 0x000f0000;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE2_PROGRAM_TBL_ID].end_address = 
    (~SOC_PB_EPNI_MASK & 0x000f0000) + 0x0010 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE2_PROGRAM_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EPNI_COPY_ENGINE2_PROGRAM_TBL_ID].base[indx] = 
      NULL;
  }


  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID0].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID0].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID0].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID0].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID0].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00100000;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID0].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00100000) + 0x0003 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID0].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID0].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID1].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID1].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID1].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID1].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID1].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00110000;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID1].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00110000) + 0x0003 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID1].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID1].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID2].read_result_address = 
    regs->epni.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->epni.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID2].access_trig_offset = 
    regs->epni.indirect_command_reg.addr.base +
    0 * (regs->epni.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID2].access_address_offset = 
    regs->epni.indirect_command_address_reg.addr.base + 
    0 * (regs->epni.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID2].write_val_offset = 
    regs->epni.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->epni.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID2].start_address = 
    ~SOC_PB_EPNI_MASK & 0x00120000;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID2].end_address = 
    (~SOC_PB_EPNI_MASK & 0x00120000) + 0x0003 - 1;
  Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID2].nof_longs_to_move = 
    3;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_ID2].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_CFC_RCL2_OFP_TBL_ID].read_result_address = 
    regs->cfc.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->cfc.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_RCL2_OFP_TBL_ID].access_trig_offset = 
    regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->cfc.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_RCL2_OFP_TBL_ID].access_address_offset = 
    regs->cfc.indirect_command_address_reg.addr.base + 
    0 * (regs->cfc.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_RCL2_OFP_TBL_ID].write_val_offset = 
    regs->cfc.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->cfc.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_RCL2_OFP_TBL_ID].start_address = 
    ~SOC_PB_CFC_MASK & 0x00000000;
  Soc_pb_indirect_blocks[SOC_PB_CFC_RCL2_OFP_TBL_ID].end_address = 
    (~SOC_PB_CFC_MASK & 0x00000000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_CFC_RCL2_OFP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_CFC_RCL2_OFP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_CFC_NIFCLSB2_OFP_TBL_ID].read_result_address = 
    regs->cfc.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->cfc.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_NIFCLSB2_OFP_TBL_ID].access_trig_offset = 
    regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->cfc.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_NIFCLSB2_OFP_TBL_ID].access_address_offset = 
    regs->cfc.indirect_command_address_reg.addr.base + 
    0 * (regs->cfc.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_NIFCLSB2_OFP_TBL_ID].write_val_offset = 
    regs->cfc.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->cfc.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_NIFCLSB2_OFP_TBL_ID].start_address = 
    ~SOC_PB_CFC_MASK & 0x00010000;
  Soc_pb_indirect_blocks[SOC_PB_CFC_NIFCLSB2_OFP_TBL_ID].end_address = 
    (~SOC_PB_CFC_MASK & 0x00010000) + 0x0020 - 1;
  Soc_pb_indirect_blocks[SOC_PB_CFC_NIFCLSB2_OFP_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_CFC_NIFCLSB2_OFP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_CFC_CALRXA_TBL_ID].read_result_address = 
    regs->cfc.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->cfc.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALRXA_TBL_ID].access_trig_offset = 
    regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->cfc.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALRXA_TBL_ID].access_address_offset = 
    regs->cfc.indirect_command_address_reg.addr.base + 
    0 * (regs->cfc.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALRXA_TBL_ID].write_val_offset = 
    regs->cfc.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->cfc.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALRXA_TBL_ID].start_address = 
    ~SOC_PB_CFC_MASK & 0x00020000;
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALRXA_TBL_ID].end_address = 
    (~SOC_PB_CFC_MASK & 0x00020000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALRXA_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_CFC_CALRXA_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_CFC_CALRXB_TBL_ID].read_result_address = 
    regs->cfc.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->cfc.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALRXB_TBL_ID].access_trig_offset = 
    regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->cfc.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALRXB_TBL_ID].access_address_offset = 
    regs->cfc.indirect_command_address_reg.addr.base + 
    0 * (regs->cfc.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALRXB_TBL_ID].write_val_offset = 
    regs->cfc.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->cfc.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALRXB_TBL_ID].start_address = 
    ~SOC_PB_CFC_MASK & 0x00030000;
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALRXB_TBL_ID].end_address = 
    (~SOC_PB_CFC_MASK & 0x00030000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALRXB_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_CFC_CALRXB_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_CFC_OOB0_SCH_MAP_TBL_ID].read_result_address = 
    regs->cfc.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->cfc.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_OOB0_SCH_MAP_TBL_ID].access_trig_offset = 
    regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->cfc.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_OOB0_SCH_MAP_TBL_ID].access_address_offset = 
    regs->cfc.indirect_command_address_reg.addr.base + 
    0 * (regs->cfc.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_OOB0_SCH_MAP_TBL_ID].write_val_offset = 
    regs->cfc.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->cfc.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_OOB0_SCH_MAP_TBL_ID].start_address = 
    ~SOC_PB_CFC_MASK & 0x00040000;
  Soc_pb_indirect_blocks[SOC_PB_CFC_OOB0_SCH_MAP_TBL_ID].end_address = 
    (~SOC_PB_CFC_MASK & 0x00040000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_CFC_OOB0_SCH_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_CFC_OOB0_SCH_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_CFC_OOB1_SCH_MAP_TBL_ID].read_result_address = 
    regs->cfc.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->cfc.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_OOB1_SCH_MAP_TBL_ID].access_trig_offset = 
    regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->cfc.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_OOB1_SCH_MAP_TBL_ID].access_address_offset = 
    regs->cfc.indirect_command_address_reg.addr.base + 
    0 * (regs->cfc.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_OOB1_SCH_MAP_TBL_ID].write_val_offset = 
    regs->cfc.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->cfc.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_OOB1_SCH_MAP_TBL_ID].start_address = 
    ~SOC_PB_CFC_MASK & 0x00050000;
  Soc_pb_indirect_blocks[SOC_PB_CFC_OOB1_SCH_MAP_TBL_ID].end_address = 
    (~SOC_PB_CFC_MASK & 0x00050000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_CFC_OOB1_SCH_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_CFC_OOB1_SCH_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_CFC_CALTX_TBL_ID].read_result_address = 
    regs->cfc.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->cfc.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALTX_TBL_ID].access_trig_offset = 
    regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->cfc.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALTX_TBL_ID].access_address_offset = 
    regs->cfc.indirect_command_address_reg.addr.base + 
    0 * (regs->cfc.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALTX_TBL_ID].write_val_offset = 
    regs->cfc.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->cfc.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALTX_TBL_ID].start_address = 
    ~SOC_PB_CFC_MASK & 0x00060000;
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALTX_TBL_ID].end_address = 
    (~SOC_PB_CFC_MASK & 0x00060000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_CFC_CALTX_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_CFC_CALTX_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_CALRX_TBL_ID].read_result_address = 
    regs->cfc.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->cfc.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_CALRX_TBL_ID].access_trig_offset = 
    regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->cfc.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_CALRX_TBL_ID].access_address_offset = 
    regs->cfc.indirect_command_address_reg.addr.base + 
    0 * (regs->cfc.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_CALRX_TBL_ID].write_val_offset = 
    regs->cfc.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->cfc.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_CALRX_TBL_ID].start_address = 
    ~SOC_PB_CFC_MASK & 0x00070000;
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_CALRX_TBL_ID].end_address = 
    (~SOC_PB_CFC_MASK & 0x00070000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_CALRX_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_CALRX_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_CALRX_TBL_ID].read_result_address = 
    regs->cfc.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->cfc.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_CALRX_TBL_ID].access_trig_offset = 
    regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->cfc.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_CALRX_TBL_ID].access_address_offset = 
    regs->cfc.indirect_command_address_reg.addr.base + 
    0 * (regs->cfc.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_CALRX_TBL_ID].write_val_offset = 
    regs->cfc.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->cfc.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_CALRX_TBL_ID].start_address = 
    ~SOC_PB_CFC_MASK & 0x00080000;
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_CALRX_TBL_ID].end_address = 
    (~SOC_PB_CFC_MASK & 0x00080000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_CALRX_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_CALRX_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_SCH_MAP_TBL_ID].read_result_address = 
    regs->cfc.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->cfc.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_SCH_MAP_TBL_ID].access_trig_offset = 
    regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->cfc.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_SCH_MAP_TBL_ID].access_address_offset = 
    regs->cfc.indirect_command_address_reg.addr.base + 
    0 * (regs->cfc.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_SCH_MAP_TBL_ID].write_val_offset = 
    regs->cfc.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->cfc.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_SCH_MAP_TBL_ID].start_address = 
    ~SOC_PB_CFC_MASK & 0x00090000;
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_SCH_MAP_TBL_ID].end_address = 
    (~SOC_PB_CFC_MASK & 0x00090000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_SCH_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_SCH_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_SCH_MAP_TBL_ID].read_result_address = 
    regs->cfc.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->cfc.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_SCH_MAP_TBL_ID].access_trig_offset = 
    regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->cfc.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_SCH_MAP_TBL_ID].access_address_offset = 
    regs->cfc.indirect_command_address_reg.addr.base + 
    0 * (regs->cfc.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_SCH_MAP_TBL_ID].write_val_offset = 
    regs->cfc.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->cfc.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_SCH_MAP_TBL_ID].start_address = 
    ~SOC_PB_CFC_MASK & 0x000a0000;
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_SCH_MAP_TBL_ID].end_address = 
    (~SOC_PB_CFC_MASK & 0x000a0000) + 0x0080 - 1;
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_SCH_MAP_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_SCH_MAP_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_CALTX_TBL_ID].read_result_address = 
    regs->cfc.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->cfc.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_CALTX_TBL_ID].access_trig_offset = 
    regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->cfc.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_CALTX_TBL_ID].access_address_offset = 
    regs->cfc.indirect_command_address_reg.addr.base + 
    0 * (regs->cfc.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_CALTX_TBL_ID].write_val_offset = 
    regs->cfc.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->cfc.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_CALTX_TBL_ID].start_address = 
    ~SOC_PB_CFC_MASK & 0x000b0000;
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_CALTX_TBL_ID].end_address = 
    (~SOC_PB_CFC_MASK & 0x000b0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_CALTX_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN0_CALTX_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_CALTX_TBL_ID].read_result_address = 
    regs->cfc.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->cfc.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_CALTX_TBL_ID].access_trig_offset = 
    regs->cfc.indirect_command_reg.addr.base +
    0 * (regs->cfc.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_CALTX_TBL_ID].access_address_offset = 
    regs->cfc.indirect_command_address_reg.addr.base + 
    0 * (regs->cfc.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_CALTX_TBL_ID].write_val_offset = 
    regs->cfc.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->cfc.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_CALTX_TBL_ID].start_address = 
    ~SOC_PB_CFC_MASK & 0x000c0000;
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_CALTX_TBL_ID].end_address = 
    (~SOC_PB_CFC_MASK & 0x000c0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_CALTX_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_CFC_ILKN1_CALTX_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_CAL_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_CAL_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_CAL_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_CAL_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_CAL_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x40000000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_CAL_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x40000000) + 0xfc00 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_CAL_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_CAL_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_DRM_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_DRM_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_DRM_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_DRM_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_DRM_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x40010000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_DRM_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x40010000) + 0x0128 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_DRM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_DRM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_DSM_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_DSM_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_DSM_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_DSM_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_DSM_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x40020000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_DSM_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x40020000) + 0x0200 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_DSM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_DSM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_FDMS_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FDMS_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FDMS_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FDMS_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FDMS_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x40030000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_FDMS_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x40030000) + 0xe000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_FDMS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_FDMS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_SHDS_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SHDS_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SHDS_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SHDS_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SHDS_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x40040000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_SHDS_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x40040000) + 0xe000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_SHDS_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_SHDS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_SEM_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SEM_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SEM_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SEM_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SEM_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x40050000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_SEM_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x40050000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_SEM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_SEM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_FSF_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FSF_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FSF_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FSF_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FSF_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x40060000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_FSF_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x40060000) + 0x07b4 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_FSF_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_FSF_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_FGM_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FGM_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FGM_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FGM_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FGM_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x40070000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_FGM_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x40070000) + 0x0800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_FGM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_FGM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_SHC_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SHC_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SHC_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SHC_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SHC_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x40080000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_SHC_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x40080000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_SHC_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_SHC_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_SCC_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCC_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCC_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCC_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCC_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x40090000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCC_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x40090000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCC_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_SCC_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_SCT_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCT_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCT_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCT_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCT_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x400a0000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCT_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x400a0000) + 0x0100 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCT_TBL_ID].nof_longs_to_move = 
    2;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_SCT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_FQM_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FQM_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FQM_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FQM_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FQM_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x400b0000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_FQM_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x400b0000) + 0x3800 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_FQM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_FQM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_FFM_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FFM_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FFM_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FFM_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FFM_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x400c0000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_FFM_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x400c0000) + 0x2000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_FFM_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_FFM_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_TMC_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_TMC_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_TMC_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_TMC_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_TMC_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x40100000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_TMC_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x40100000) + 0xe000 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_TMC_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_TMC_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_PQS_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_PQS_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_PQS_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_PQS_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_PQS_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x401d0000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_PQS_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x401d0000) + 0x0051 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_PQS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_PQS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_SCHEDULER_INIT_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCHEDULER_INIT_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCHEDULER_INIT_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCHEDULER_INIT_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCHEDULER_INIT_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x41000000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCHEDULER_INIT_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x41000000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_SCHEDULER_INIT_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_SCHEDULER_INIT_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_SCH_FORCE_STATUS_MESSAGE_TBL_ID].read_result_address = 
    regs->sch.indirect_command_rd_data_reg.addr.base + 
    0 * (regs->sch.indirect_command_rd_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FORCE_STATUS_MESSAGE_TBL_ID].access_trig_offset = 
    regs->sch.indirect_command_reg.addr.base +
    0 * (regs->sch.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FORCE_STATUS_MESSAGE_TBL_ID].access_address_offset = 
    regs->sch.indirect_command_address_reg.addr.base + 
    0 * (regs->sch.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FORCE_STATUS_MESSAGE_TBL_ID].write_val_offset = 
    regs->sch.indirect_command_wr_data_reg.addr.base +
    0 * (regs->sch.indirect_command_wr_data_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_SCH_FORCE_STATUS_MESSAGE_TBL_ID].start_address = 
    ~SOC_PB_SCH_MASK & 0x42000000;
  Soc_pb_indirect_blocks[SOC_PB_SCH_FORCE_STATUS_MESSAGE_TBL_ID].end_address = 
    (~SOC_PB_SCH_MASK & 0x42000000) + 0x0001 - 1;
  Soc_pb_indirect_blocks[SOC_PB_SCH_FORCE_STATUS_MESSAGE_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_SCH_FORCE_STATUS_MESSAGE_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_ID].read_result_address = 
    regs->idr.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->idr.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_ID].access_trig_offset = 
    regs->idr.indirect_command_reg.addr.base +
    0 * (regs->idr.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_ID].access_address_offset = 
    regs->idr.indirect_command_address_reg.addr.base + 
    0 * (regs->idr.indirect_command_address_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_ID].write_val_offset = 
    regs->idr.indirect_command_wr_data_reg[0].addr.base +
    0 * (regs->idr.indirect_command_wr_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_ID].start_address = 
    ~SOC_PB_IDR_MASK & 0x001b0000;
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_ID].end_address = 
    (~SOC_PB_IDR_MASK & 0x001b0000) + 0x0154 - 1;
  Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_ID].base[indx] = 
      NULL;
  }

  Soc_pb_indirect_blocks[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ID].read_result_address = 
    regs->fdt.indirect_command_rd_data_reg[0].addr.base + 
    0 * (regs->fdt.indirect_command_rd_data_reg[0].addr.step);
  Soc_pb_indirect_blocks[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ID].access_trig_offset = 
    regs->fdt.indirect_command_reg.addr.base +
    0 * (regs->fdt.indirect_command_reg.addr.step);
  Soc_pb_indirect_blocks[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ID].access_address_offset = 
    regs->fdt.indirect_command_address_reg.addr.base + 
    0 * (regs->fdt.indirect_command_address_reg.addr.step);
 /* Read-Only table
  Soc_pb_indirect_blocks[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ID].write_val_offset = 
    regs->fdt.indirect_command_wr_data_reg_0.addr.base +
    0 * (regs->fdt.indirect_command_wr_data_reg_0.addr.step);*/
  Soc_pb_indirect_blocks[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ID].start_address = 
    ~SOC_PB_MMU_MASK & 0x00000000;
  Soc_pb_indirect_blocks[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ID].end_address = 
    (~SOC_PB_MMU_MASK & 0x00000000) + 1 - 1;
  Soc_pb_indirect_blocks[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ID].nof_longs_to_move = 
    1;
  for (indx = 0; indx < CHIP_SIM_NOF_CHIPS; ++indx)
  {
    Soc_pb_indirect_blocks[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ID].base[indx] = 
      NULL;
  }

  /* Rest of table is filled in soc_pb_pp_indirect_blocks_init() */
}
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
