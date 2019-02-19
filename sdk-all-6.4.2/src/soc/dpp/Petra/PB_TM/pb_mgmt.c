/* $Id: pb_mgmt.c,v 1.15 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/src/soc_pb_mgmt.c
*
* MODULE PREFIX:  pb
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_device_management.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_interrupt_service.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_tbls.h>

#include <soc/dpp/Petra/PB_TM/pb_nif.h>
#include <soc/dpp/Petra/PB_TM/pb_stat_if.h>
#include <soc/dpp/Petra/PB_TM/pb_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_flow_control.h>
#include <soc/dpp/Petra/PB_TM/pb_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>


#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>
#include <soc/dpp/Petra/petra_mgmt.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_fabric.h>

#ifdef LINK_PB_PP_LIBRARIES
#include <soc/dpp/Petra/PB_PP/pb_pp_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_framework.h>
#endif

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PORT_NDX_MAX                                        (SOC_PETRA_NOF_FAP_PORTS-1)
#define SOC_PB_CONF_MODE_NDX_MAX                                   (SOC_PETRA_MGMT_NOF_PCKT_SIZE_CONF_MODES - 1)
#define SOC_PB_INIT_PORTS_HDR_TYPE_NOF_ENTRIES_MAX                 (SOC_PETRA_NOF_FAP_PORTS)
#define SOC_PB_INIT_PORTS_IF_MAP_NOF_ENTRIES_MAX                   (SOC_PETRA_NOF_FAP_PORTS)
#define SOC_PB_MGMT_PP_PORT_NOF_ENTRIES_MAX                        (64)
#define SOC_PB_MGMT_TM_PROFILE_MAP_NOF_ENTRIES_MAX                 (SOC_PETRA_NOF_FAP_PORTS)
#define SOC_PB_HW_ADJ_MAL_MAL_NDX_MAX                              (15)
#define SOC_PB_HW_ADJ_NIF_MAL_NOF_ENTRIES_MAX                      (16)
#define SOC_PB_HW_ADJ_NIF_SPAUI_NOF_ENTRIES_MAX                    (16)
#define SOC_PB_HW_ADJ_NIF_GMII_NOF_ENTRIES_MAX                     (64)
#define SOC_PB_HW_ADJ_NIF_ILKN_NOF_ENTRIES_MAX                     (2)
#define SOC_PB_HW_ADJ_NIF_FATP_NOF_ENTRIES_MAX                     (3)
#define SOC_PB_INIT_BASIC_CONF_CREDIT_WORTH_MIN                    (256)
#define SOC_PB_INIT_BASIC_CONF_CREDIT_WORTH_MAX                    (8*1024)
#define SOC_PB_MGMT_TDM_MC_ROUTE_MODE_MAX                          (SOC_PB_INIT_NOF_MC_STATIC_ROUTE_MODES-1)

#define SOC_PB_MGMT_NOF_TBL_MAPS                                   (1)
#define SOC_PB_MGMT_NOF_MEM                                        (2)

#define SOC_PB_MGMT_MC_STATIC_ROUTE_MSB_OVERRIDE_NO                (0)
#define SOC_PB_MGMT_MC_STATIC_ROUTE_MSB_OVERRIDE_1_BIT             (1)
#define SOC_PB_MGMT_MC_STATIC_ROUTE_MSB_OVERRIDE_4_BITS            (3)

#define SOC_PB_MGMT_VER_SCRATCH_PAD                                (0)
#define SOC_PB_MGMT_VER                                            (0x00000703)

/* } */
/*************
 * MACROS    *
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
/* Indirect access { */
static
  uint8
    Soc_pb_mgmt_indirect_tables_map_init = FALSE;
static
  uint8
    Soc_pb_mgmt_indirect_memory_map_init = FALSE;
static
  uint8
    Soc_pb_mgmt_indirect_module_info_init = FALSE;
static
  SOC_SAND_INDIRECT_TABLES_INFO
    Soc_pb_mgmt_indirect_tables_map[SOC_PB_MGMT_NOF_TBL_MAPS];
static
  SOC_SAND_INDIRECT_MEMORY_MAP
    Soc_pb_mgmt_indirect_memory_map[SOC_PB_MGMT_NOF_MEM];
static
  SOC_SAND_INDIRECT_MODULE_INFO
    Soc_pb_mgmt_indirect_module_info[SOC_PB_NUM_OF_INDIRECT_MODULES];

/* Indirect access } */
/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC uint32
  soc_pb_access_db_init(void)
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_ACCESS_DB_INIT);

  res = soc_petra_regs_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_tbls_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_tbls_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_regs_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_srd_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_access_db_init()",0,0);
}

uint32
  soc_pb_mgmt_ver_set(
    SOC_SAND_IN int unit
  )
{
  uint32  
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_SEQUENCE_FIXES_APPLY);

  regs = soc_petra_regs();

  SOC_PB_REG_SET(regs->eci.scratch_pad_reg[SOC_PB_MGMT_VER_SCRATCH_PAD], SOC_PB_MGMT_VER, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pa_mgmt_ver_set()", 0, 0);
}

uint32
  soc_pb_mgmt_ver_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint32  *ver
  )
{
  uint32  
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_SEQUENCE_FIXES_APPLY);

  regs = soc_petra_regs();

  SOC_PB_REG_GET(regs->eci.scratch_pad_reg[SOC_PB_MGMT_VER_SCRATCH_PAD], fld_val, 10, exit);
  *ver = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pa_mgmt_ver_set()", 0, 0);
}
uint32
  soc_pb_mgmt_init_sequence_fixes_apply_unsafe(
    SOC_SAND_IN int unit
  )
{
  SOC_PB_PP_REGS
    *regs;
  uint32
    fld_val;
  SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA
    tbl_data;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INIT_SEQUENCE_FIXES_APPLY);

  regs = soc_pb_pp_regs();

  fld_val = 1;
  SOC_PB_PP_FLD_SET(regs->egq.action_profile1_reg.action_profile_exclude_src, fld_val, 10, exit);

  tbl_data.discard = TRUE;
  tbl_data.otm_valid = 0;
  tbl_data.otm = 0;
  tbl_data.tc = 0;
  tbl_data.tc_valid = 0;
  tbl_data.dp = 0;
  tbl_data.dp_valid = 0;
  tbl_data.cud = 0;
  tbl_data.cud_valid = 0;

  res = soc_pb_pp_egq_action_profile_table_tbl_set_unsafe(
          unit,
          1,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_init_sequence_fixes_apply_unsafe()", 0, 0);
}

/* Indirect modules init { */
uint32
  soc_pb_mgmt_indirect_memory_map_get(
    SOC_SAND_OUT SOC_SAND_INDIRECT_MODULE* info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INDIRECT_MEMORY_MAP_GET);

  SOC_SAND_CHECK_NULL_INPUT(info);

  if (Soc_pb_mgmt_indirect_memory_map_init == FALSE)
  {
    info->memory_map_arr = NULL;
  }
  else
  {
    info->memory_map_arr = Soc_pb_mgmt_indirect_memory_map;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_indirect_memory_map_get()",0,0);
}

uint32
  soc_pb_mgmt_indirect_memory_map_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INDIRECT_MEMORY_MAP_INIT);

  if (Soc_pb_mgmt_indirect_memory_map_init == TRUE)
  {
    goto exit;
  }

  Soc_pb_mgmt_indirect_memory_map[0].offset = 0x0;
  Soc_pb_mgmt_indirect_memory_map[0].size = 0xFFFFFFFF;
  Soc_pb_mgmt_indirect_memory_map[1].offset = 0;
  Soc_pb_mgmt_indirect_memory_map[1].size = 0;

  Soc_pb_mgmt_indirect_memory_map_init = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_indirect_memory_map_init()",0,0);
}
uint32
  soc_pb_mgmt_indirect_table_map_get(
    SOC_SAND_OUT SOC_SAND_INDIRECT_MODULE* info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INDIRECT_TABLE_MAP_GET);

  if (Soc_pb_mgmt_indirect_tables_map_init == FALSE)
  {
    info->tables_info = NULL;
  }
  else
  {
    info->tables_info = Soc_pb_mgmt_indirect_tables_map;
  }

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_indirect_table_map_get()",0,0);
}

uint32
  soc_pb_mgmt_indirect_table_map_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INDIRECT_TABLE_MAP_INIT);

  if (Soc_pb_mgmt_indirect_tables_map_init == TRUE)
  {
    goto exit;
  }

  Soc_pb_mgmt_indirect_tables_map[0].read_result_offset = 0;
  Soc_pb_mgmt_indirect_tables_map[0].tables_prefix = 0;
  Soc_pb_mgmt_indirect_tables_map[0].tables_prefix_nof_bits = 0;
  Soc_pb_mgmt_indirect_tables_map[0].word_size = 0;
  Soc_pb_mgmt_indirect_tables_map[0].write_buffer_offset = 0;

  Soc_pb_mgmt_indirect_tables_map_init = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_indirect_table_map_init()",0,0);
}

uint32
  soc_pb_mgmt_indirect_module_info_get(
    SOC_SAND_OUT SOC_SAND_INDIRECT_MODULE* info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INDIRECT_MODULE_INFO_GET);

  if (Soc_pb_mgmt_indirect_tables_map_init == FALSE)
  {
    info->info_arr = NULL;
    info->info_arr_max_index = 0;
  }
  else
  {
    info->info_arr = Soc_pb_mgmt_indirect_module_info;
    info->info_arr_max_index = SOC_PB_NUM_OF_INDIRECT_MODULES;
  }

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_indirect_module_info_get()",0,0);
}

uint32
  soc_pb_mgmt_indirect_module_info_init(void)
{
  uint32
    err = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = NULL;
  uint32
    dpi_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INDIRECT_MODULE_INFO_INIT);

  err =
    soc_petra_regs_get(
    &(regs)
    );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  SOC_SAND_CHECK_NULL_INPUT(regs);

  /* } */

  Soc_pb_mgmt_indirect_module_info[SOC_PB_OLP_ID].module_index =
    SOC_PB_OLP_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_OLP_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->olp.indirect_command_rd_data_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_OLP_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_OLP_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->olp.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_OLP_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->olp.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_OLP_ID].write_buffer_offset =
    SOC_PB_REG_DB_ACC(regs->olp.indirect_command_wr_data_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_OLP_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IRE_ID].module_index =
    SOC_PB_IRE_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_IRE_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->ire.indirect_command_rd_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IRE_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IRE_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->ire.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IRE_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->ire.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IRE_ID].write_buffer_offset =
    SOC_PB_REG_DB_ACC(regs->ire.indirect_command_wr_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IRE_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IDR_ID].module_index =
    SOC_PB_IDR_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_IDR_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->idr.indirect_command_rd_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IDR_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IDR_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->idr.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IDR_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->idr.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IDR_ID].write_buffer_offset =
    SOC_PB_REG_DB_ACC(regs->idr.indirect_command_wr_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IDR_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IRR_ID].module_index =
    SOC_PB_IRR_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_IRR_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->irr.indirect_command_rd_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IRR_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IRR_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->irr.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IRR_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->irr.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IRR_ID].write_buffer_offset =
    SOC_PB_REG_DB_ACC(regs->irr.indirect_command_wr_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IRR_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IHP_ID].module_index =
    SOC_PB_IHP_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_IHP_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->ihp.indirect_command_rd_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IHP_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IHP_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->ihp.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IHP_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->ihp.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IHP_ID].write_buffer_offset =
    SOC_PB_REG_DB_ACC(regs->ihp.indirect_command_wr_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IHP_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_pb_mgmt_indirect_module_info[SOC_PB_QDR_ID].module_index =
    SOC_PB_QDR_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_QDR_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->qdr.indirect_command_rd_data_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_QDR_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_QDR_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->qdr.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_QDR_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->qdr.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_QDR_ID].write_buffer_offset =
    SOC_PB_REG_DB_ACC(regs->qdr.indirect_command_wr_data_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_QDR_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IPS_ID].module_index =
    SOC_PB_IPS_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_IPS_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->ips.indirect_command_rd_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IPS_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IPS_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->ips.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IPS_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->ips.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IPS_ID].write_buffer_offset =
    SOC_PB_REG_DB_ACC(regs->ips.indirect_command_wr_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IPS_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IPT_ID].module_index =
    SOC_PB_IPT_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_IPT_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->ipt.indirect_command_rd_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IPT_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IPT_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->ipt.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IPT_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->ipt.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IPT_ID].write_buffer_offset =
    SOC_PB_REG_DB_ACC(regs->ipt.indirect_command_wr_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IPT_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  for (dpi_ndx = 0; dpi_ndx < SOC_PETRA_NOF_DPI_IDS; ++dpi_ndx)
  {
    Soc_pb_mgmt_indirect_module_info[SOC_PETRA_DPI_A_ID + dpi_ndx].module_index =
      SOC_PETRA_DPI_A_ID + dpi_ndx;
    Soc_pb_mgmt_indirect_module_info[SOC_PETRA_DPI_A_ID + dpi_ndx].read_result_offset =
      SOC_PB_REG_DB_ACC(regs->dpi.indirect_command_rd_data_reg[0].addr.base) +
      (dpi_ndx * SOC_PB_REG_DB_ACC(regs->dpi.indirect_command_rd_data_reg[0].addr.step));

    Soc_pb_mgmt_indirect_module_info[SOC_PETRA_DPI_A_ID + dpi_ndx].word_size =
      sizeof(uint32);

    Soc_pb_mgmt_indirect_module_info[SOC_PETRA_DPI_A_ID + dpi_ndx].access_trigger =
      SOC_PB_REG_DB_ACC(regs->dpi.indirect_command_reg.addr.base) +
      (dpi_ndx * SOC_PB_REG_DB_ACC(regs->dpi.indirect_command_reg.addr.step));

    Soc_pb_mgmt_indirect_module_info[SOC_PETRA_DPI_A_ID + dpi_ndx].access_address =
      SOC_PB_REG_DB_ACC(regs->dpi.indirect_command_address_reg.addr.base) +
      (dpi_ndx * SOC_PB_REG_DB_ACC(regs->dpi.indirect_command_address_reg.addr.step));

    Soc_pb_mgmt_indirect_module_info[SOC_PETRA_DPI_A_ID + dpi_ndx].write_buffer_offset =
      SOC_PB_REG_DB_ACC(regs->dpi.indirect_command_wr_data_reg[0].addr.base) +
      (dpi_ndx * SOC_PB_REG_DB_ACC(regs->dpi.indirect_command_wr_data_reg[0].addr.step));

    Soc_pb_mgmt_indirect_module_info[SOC_PETRA_DPI_A_ID + dpi_ndx].module_bits =
      SOC_SAND_INDIRECT_ERASE_MODULE_BITS;
  }

  Soc_pb_mgmt_indirect_module_info[SOC_PB_RTP_ID].module_index =
    SOC_PB_RTP_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_RTP_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->rtp.indirect_command_rd_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_RTP_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_RTP_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->rtp.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_RTP_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->rtp.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_RTP_ID].write_buffer_offset =
    SOC_PB_REG_DB_ACC(regs->rtp.indirect_command_wr_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_RTP_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_pb_mgmt_indirect_module_info[SOC_PB_EGQ_ID].module_index =
    SOC_PB_EGQ_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_EGQ_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->egq.indirect_command_rd_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_EGQ_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_EGQ_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->egq.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_EGQ_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->egq.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_EGQ_ID].write_buffer_offset =
    SOC_PB_REG_DB_ACC(regs->egq.indirect_command_wr_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_EGQ_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;


  Soc_pb_mgmt_indirect_module_info[SOC_PB_SCH_ID].module_index =
    SOC_PB_SCH_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_SCH_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->sch.indirect_command_rd_data_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_SCH_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_SCH_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->sch.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_SCH_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->sch.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_SCH_ID].write_buffer_offset =
    SOC_PB_REG_DB_ACC(regs->sch.indirect_command_wr_data_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_SCH_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_pb_mgmt_indirect_module_info[SOC_PB_CFC_ID].module_index =
    SOC_PB_CFC_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_CFC_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->cfc.indirect_command_rd_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_CFC_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_CFC_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->cfc.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_CFC_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->cfc.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_CFC_ID].write_buffer_offset =
    SOC_PB_REG_DB_ACC(regs->cfc.indirect_command_wr_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_CFC_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_pb_mgmt_indirect_module_info[SOC_PB_EPNI_ID].module_index =
    SOC_PB_EPNI_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_EPNI_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->epni.indirect_command_rd_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_EPNI_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_EPNI_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->epni.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_EPNI_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->epni.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_EPNI_ID].write_buffer_offset =
    SOC_PB_REG_DB_ACC(regs->epni.indirect_command_wr_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_EPNI_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IQM_ID].module_index =
    SOC_PB_IQM_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_IQM_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->iqm.indirect_command_rd_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IQM_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IQM_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->iqm.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IQM_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->iqm.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IQM_ID].write_buffer_offset =
    SOC_PB_REG_DB_ACC(regs->iqm.indirect_command_wr_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IQM_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_pb_mgmt_indirect_module_info[SOC_PB_MMU_ID].module_index =
    SOC_PB_MMU_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_MMU_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->mmu.indirect_command_rd_data_reg_0.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_MMU_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_MMU_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->mmu.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_MMU_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->mmu.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_MMU_ID].write_buffer_offset =
    SOC_PB_REG_DB_ACC(regs->mmu.indirect_command_wr_data_reg_0.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_MMU_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IHB_ID].module_index =
    SOC_PB_IHB_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_IHB_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->ihb.indirect_command_rd_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IHB_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IHB_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->ihb.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IHB_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->ihb.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IHB_ID].write_buffer_offset =
    SOC_PB_REG_DB_ACC(regs->ihb.indirect_command_wr_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_IHB_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_pb_mgmt_indirect_module_info[SOC_PB_FDT_ID].module_index =
    SOC_PB_FDT_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_FDT_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->fdt.indirect_command_rd_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_FDT_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_FDT_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->fdt.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_FDT_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->fdt.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_FDT_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_pb_mgmt_indirect_module_info[SOC_PB_NBI_ID].module_index =
    SOC_PB_NBI_ID;
  Soc_pb_mgmt_indirect_module_info[SOC_PB_NBI_ID].read_result_offset =
    SOC_PB_REG_DB_ACC(regs->nbi.indirect_command_rd_data_reg[0].addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_NBI_ID].word_size =
    sizeof(uint32);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_NBI_ID].access_trigger =
    SOC_PB_REG_DB_ACC(regs->nbi.indirect_command_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_NBI_ID].access_address =
    SOC_PB_REG_DB_ACC(regs->nbi.indirect_command_address_reg.addr.base);

  Soc_pb_mgmt_indirect_module_info[SOC_PB_NBI_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;


  Soc_pb_mgmt_indirect_module_info_init = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_indirect_module_info()",0,0);
}

uint32
  soc_pb_mgmt_indirect_module_get(
    SOC_SAND_OUT SOC_SAND_INDIRECT_MODULE* info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INDIRECT_MODULE_INIT);

  res = soc_pb_mgmt_indirect_module_info_get(info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_pb_mgmt_indirect_table_map_get(info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  res = soc_pb_mgmt_indirect_memory_map_get(info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pp_mgmt_indirect_module_init()",0,0);
}

STATIC uint32
  soc_pb_mgmt_indirect_module_init(void)
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INDIRECT_MODULE_INIT);

  res = soc_pb_mgmt_indirect_module_info_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_pb_mgmt_indirect_table_map_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  res = soc_pb_mgmt_indirect_memory_map_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_indirect_module_init()",0,0);
}

STATIC uint32
  soc_pb_mgmt_module_init(void)
{
  uint32
    res;

  SOC_SAND_INTERRUPT_INIT_DEFS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_MODULE_INIT);

  /*Stop the interrupts and task switches while accessing global resources*/
  /*SOC_SAND_INTERRUPTS_STOP; */ 

  res = soc_pb_access_db_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_petra_chip_defines_init(
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_sw_db_init(
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_interrupt_module_initialize(
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_mgmt_indirect_module_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /* Add list of SOC_PETRA errors to all-system errors pool                                                  */
  res = soc_pb_errors_add();
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /* Now add list of SOC_PETRA procedure descriptors to all-system pool.                                     */
  res = soc_pb_procedure_desc_add() ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  /* Add list of SOC_PETRA errors to all-system errors pool                                                  */
  res = soc_petra_errors_add();
  SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);

  /* Now add list of SOC_PETRA procedure descriptors to all-system pool.                                     */
  res = soc_petra_procedure_desc_add() ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_module_init()",0,0);
}

uint16
  soc_pb_mgmt_interrupts_unmask(
    int     unit,
    uint32     is_tcm
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint8
    tcm = (uint8)is_tcm;
  SOC_SAND_TODO_IMPLEMENT_WARNING

  res = soc_pb_interrupt_unmask_all_unsafe(
    unit,
    tcm
    );
  if (SOC_SAND_OK != soc_sand_get_error_code_from_error_word(res))
  {
    ret = SOC_SAND_ERR;
    goto exit;
  }

exit:
  return ret;
}

uint32
  soc_pb_mgmt_is_interrupt_bit_autoclear(
    uint32     bit
  )
{
  SOC_SAND_TODO_IMPLEMENT_WARNING

    return TRUE;
}

uint16
  soc_pb_mgmt_is_interrupt_masked_get(
    int     unit
  )
{
  return soc_pb_interrupt_is_masked_get(unit);
}

uint16
  soc_pb_mgmt_interrupts_mask_get(
    int     unit,
    uint32      *bit_stream
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;

  SOC_SAND_TODO_IMPLEMENT_WARNING

  res = soc_pb_interrupt_mask_get(
          unit,
          bit_stream
        );

  if (SOC_SAND_OK != soc_sand_get_error_code_from_error_word(res))
  {
    ret = SOC_SAND_ERR;
    goto exit;
  }

exit:
  return ret;
}

uint16
  soc_pb_mgmt_interrupt_monitored_cause_mask(
    int     unit,
    uint32     cause
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;

  SOC_SAND_TODO_IMPLEMENT_WARNING

  res = soc_pb_interrupt_monitored_cause_mask_bit_set(
    unit,
    cause,
    TRUE
    );
  if (SOC_SAND_OK != soc_sand_get_error_code_from_error_word(res))
  {
    ret = SOC_SAND_ERR;
    goto exit;
  }

exit:
  return ret;
}

STATIC uint32
  soc_pb_mgmt_device_init(
    SOC_SAND_IN  int  unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_DEVICE_INIT);

  SOC_SAND_TODO_IMPLEMENT_WARNING

  res = soc_petra_sw_db_device_init(
          unit,
          SOC_PETRA_DEV_VER_B
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  soc_petra_sw_db_ver_set(
    unit,
    SOC_PETRA_DEV_VER_B
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_device_init()",0,0);
}

STATIC uint32
  soc_pb_mgmt_device_close(
    SOC_SAND_IN  int  unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_DEVICE_CLOSE);

  SOC_SAND_TODO_IMPLEMENT_WARNING

  res = soc_petra_sw_db_device_close(
          unit
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_device_close()",0,0);
}


/*********************************************************************
*     This procedure registers a new device to be taken care
*     of by this device driver. Physical device must be
*     accessible by CPU when this call is made..
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_register_device_unsafe(
    uint32                           *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR reset_device_ptr,
    SOC_SAND_OUT int                 *unit_ptr
  )
{
  uint32
    res;
  int
    unit = *unit_ptr;
  SOC_SAND_DEV_VER_INFO
    ver_info;
  SOC_SAND_INDIRECT_MODULE
    indirect_module;
  SOC_PETRA_REG_FIELD
    chip_type_fld,
    dbg_ver_fld,
    chip_ver_fld;
  SOC_PETRA_REGS
    *regs;
  uint32
    *base;
  uint8
    is_low_sim_active;
  uint32
    reg_val[1];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_REGISTER_DEVICE_UNSAFE);

#ifndef SAND_LOW_LEVEL_SIMULATION
  is_low_sim_active = FALSE;
#else
  is_low_sim_active = soc_sand_low_is_sim_active_get();
#endif

  res = soc_pb_mgmt_module_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  regs = soc_petra_regs();

  /* In order for indirect access to function properly we have to                                         */
  /* point it to the right addresses.                                                                     */
  soc_pb_mgmt_indirect_module_get(&(indirect_module));

  soc_sand_clear_SAND_DEV_VER_INFO(&ver_info);
  chip_type_fld.lsb = SOC_PETRA_MGMT_CHIP_TYPE_FLD_LSB;
  chip_type_fld.msb = SOC_PETRA_MGMT_CHIP_TYPE_FLD_MSB;
  dbg_ver_fld.lsb   = SOC_PETRA_MGMT_DBG_VER_FLD_LSB;
  dbg_ver_fld.msb   = SOC_PETRA_MGMT_DBG_VER_FLD_MSB;
  chip_ver_fld.lsb  = SOC_PETRA_MGMT_CHIP_VER_FLD_LSB;
  chip_ver_fld.msb  = SOC_PETRA_MGMT_CHIP_VER_FLD_MSB;

  ver_info.ver_reg_offset = SOC_PETRA_MGMT_VER_REG_BASE;
  ver_info.logic_chip_type= SOC_SAND_DEV_PB;
  ver_info.chip_type      = SOC_PB_EXPECTED_CHIP_TYPE;

  ver_info.chip_type_shift= SOC_PETRA_FLD_SHIFT(chip_type_fld);
  ver_info.chip_type_mask = SOC_PETRA_FLD_MASK(chip_type_fld);
  ver_info.dbg_ver_shift  = SOC_PETRA_FLD_SHIFT(dbg_ver_fld);
  ver_info.dbg_ver_mask   = SOC_PETRA_FLD_MASK(dbg_ver_fld);
  ver_info.chip_ver_shift = SOC_PETRA_FLD_SHIFT(chip_ver_fld);
  ver_info.chip_ver_mask  = SOC_PETRA_FLD_MASK(chip_ver_fld);

  base = (uint32*)base_address;

  /*
  *  Validate ECI access to the Soc_petra
  */
  if (!is_low_sim_active)
  {
    *reg_val = 0xaaff5500;
    res = soc_sand_physical_write_to_chip(
      reg_val, (uint32 *)base_address, SOC_PB_REG_DB_ACC(regs->eci.test_reg.addr.base), SOC_SAND_REG_SIZE_BYTES
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

    res = soc_sand_physical_read_from_chip(
      reg_val, base_address, SOC_PB_REG_DB_ACC(regs->eci.test_reg.addr.base), SOC_SAND_REG_SIZE_BYTES
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);
    if (*reg_val != ~(0xaaff5500))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ECI_ACCESS_ERR, 49, exit);
    }
  }
  /* Register the device in SOC_SAND.                                                                         */
  res = soc_sand_device_register(
    base,
    SOC_PB_TOTAL_SIZE_OF_REGS,
    soc_pb_mgmt_interrupts_unmask,
    soc_pb_mgmt_is_interrupt_bit_autoclear,
    soc_pb_mgmt_is_interrupt_masked_get,
    soc_pb_mgmt_interrupts_mask_get,
    soc_pb_mgmt_interrupt_monitored_cause_mask,
    reset_device_ptr,
    NULL,
    &ver_info,
    &indirect_module,
    0xFFFFFFFF,                         /* Given invalid address as the general mask bit.           */
    /* This bit do not exist on the SOC_PETRA                      */
    &unit
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  *unit_ptr = unit ;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

#ifdef LINK_PB_PP_LIBRARIES
  {
    uint32
      pp_enable_fld;

    /* Special case for hot-start:
      Check if pp was marked as enabled. If so, register pp error and procedures
      (usually done in soc_pb_pp_mgmt_operation_mode_set_unsafe, which is not called
      on hot-start) */
    SOC_PB_FLD_GET(regs->eci.spare_flags_reg.pp_enable, pp_enable_fld, 80, exit);

    if (pp_enable_fld)
    {
      /* Add list of SOC_PB_PP errors to all-system errors pool                                                  */
      res = soc_pb_pp_errors_desc_add();
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

      /* Now add list of SOC_PB_PP procedure descriptors to all-system pool.                                     */
      res = soc_pb_pp_procedure_desc_add() ;
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    }
  }
#endif

  res = soc_pb_mgmt_device_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  soc_sand_indirect_set_nof_repetitions_unsafe(unit, 0);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_register_device_unsafe()",0,0);
}

/*********************************************************************
*     Undo soc_pb_register_device()
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_unregister_device_unsafe(
    SOC_SAND_IN  int        unit
  )
{
  uint32
    res = SOC_SAND_OK;
  int32
    soc_sand_ret;
  SOC_SAND_RET
    ret;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_UNREGISTER_DEVICE_UNSAFE);

#ifdef LINK_PB_PP_LIBRARIES
  if (soc_petra_sw_db_pp_enable_get(unit))
  {
    res = soc_pb_pp_mgmt_device_close_unsafe(
            unit
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
#endif
  res = soc_pb_mgmt_device_close(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  /*
  * Take mutexes in the right order -> delta_list, device, rest of them
  */
  if (SOC_SAND_OK != soc_sand_tcm_callback_delta_list_take_mutex())
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_SEM_TAKE_FAIL, 2, exit) ;
  }

  soc_sand_ret = soc_sand_take_chip_descriptor_mutex(unit) ;
  if (SOC_SAND_OK != soc_sand_ret)
  {
    if (SOC_SAND_ERR == soc_sand_ret)
    {
      soc_sand_tcm_callback_delta_list_give_mutex();
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_SEM_TAKE_FAIL, 3, exit);
    }
    if (0 > soc_sand_ret)
    {
      soc_sand_tcm_callback_delta_list_give_mutex();
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit) ;
    }
  }
  /* semaphore taken successfully */

  /*
  * Give device mutex back
  */
  if (SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(unit))
  {
    soc_sand_tcm_callback_delta_list_give_mutex();
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_SEM_GIVE_FAIL, 5, exit) ;
  }
  /*
  * Give list mutex back
  */
  if (SOC_SAND_OK != soc_sand_tcm_callback_delta_list_give_mutex())
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_SEM_GIVE_FAIL, 6, exit) ;
  }

  ret = soc_sand_device_unregister(
    unit
    );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_unregister_device_unsafe()",0,0);
}

/*********************************************************************
*     Set soc_petra device operation mode. This defines
 *     configurations, such as support for certain header
 *     types, etc.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_operation_mode_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_MGMT_OPERATION_MODE         *op_mode
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_COMBO_QRTT
    combo_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_OPERATION_MODE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  soc_petra_sw_db_pp_enable_set(
      unit,
      op_mode->pp_enable
    );

  soc_petra_sw_db_is_fap20_in_system_set(
      unit,
      op_mode->is_fap20_in_system
    );

  soc_petra_sw_db_is_fap21_in_system_set(
      unit,
      op_mode->is_fap21_in_system
    );

  soc_petra_sw_db_is_petra_rev_a_in_system_set(
      unit,
      op_mode->is_petra_rev_a_in_system
    );

  soc_petra_sw_db_is_fe200_fabric_set(
      unit,
      op_mode->is_fe200_fabric
    );

  soc_petra_sw_db_is_fe1600_in_system_set(
    unit,
    op_mode->is_fe1600_in_system
    );

  soc_petra_sw_db_is_fabric_variable_cell_size_set(
      unit,
      op_mode->fabric_cell_format.variable_cell_size_enable
    );

  soc_petra_sw_db_is_fabric_packet_segmentation_set(
      unit,
      op_mode->fabric_cell_format.segmentation_enable
    );

  for (combo_idx = 0; combo_idx < SOC_PETRA_COMBO_NOF_QRTTS; combo_idx++)
  {
    soc_petra_sw_db_combo_nif_not_fabric_set(
      unit,
      combo_idx,
      op_mode->is_combo_nif[combo_idx]
    );
  }

  soc_petra_sw_db_stag_enable_set(
    unit,
    op_mode->stag_enable
  );

  soc_petra_sw_db_egr_mc_16k_groups_enable_set(
    unit,
    op_mode->egr_mc_16k_groups_enable
  );

  res = soc_petra_sw_db_ref_clocks_set(
          unit,
          &(op_mode->ref_clocks_conf)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  soc_petra_sw_db_tdm_mode_set(
          unit,
          op_mode->tdm_mode
        );

  soc_petra_sw_db_ftmh_lb_ext_mode_set(
    unit,
    op_mode->ftmh_lb_ext_mode
  );

  soc_petra_sw_db_add_pph_eep_ext_set(
    unit,
    op_mode->add_pph_eep_ext
  );

  soc_pb_sw_db_core_freq_self_freq_set(
    unit,
    op_mode->core_freq_high_res.self_freq
  );

  soc_pb_sw_db_core_freq_min_fap_freq_in_system_set(
    unit,
    op_mode->core_freq_high_res.min_fap_freq_in_system
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_operation_mode_set_unsafe()", 0, 0);
}

uint32
  soc_pb_mgmt_operation_mode_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_MGMT_OPERATION_MODE         *op_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_OPERATION_MODE_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_MGMT_OPERATION_MODE, op_mode, 10, exit);

  SOC_SAND_CHECK_NULL_INPUT(op_mode);
  SOC_SAND_MAGIC_NUM_VERIFY(op_mode);


  if (op_mode->is_fe200_fabric)
  {
    if (op_mode->fabric_cell_format.variable_cell_size_enable)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_CELL_VAR_SIZE_IN_FE200_SYSTEM_ERR, 10, exit);
    }
  }

  if (op_mode->is_fap20_in_system)
  {
    if (op_mode->fabric_cell_format.variable_cell_size_enable)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_CELL_VARIABLE_IN_FAP20_21_SYSTEM_ERR, 20, exit);
    }
  }

  if (op_mode->is_fap21_in_system)
  {
    if (op_mode->fabric_cell_format.variable_cell_size_enable)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_CELL_VARIABLE_IN_FAP20_21_SYSTEM_ERR, 30, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_operation_mode_set_verify()", 0, 0);
}

/*********************************************************************
*     Set soc_petra device operation mode. This defines
 *     configurations, such as support for certain header
 *     types, etc.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_operation_mode_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_MGMT_OPERATION_MODE         *op_mode
  )
{
  SOC_PETRA_COMBO_QRTT
    combo_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_OPERATION_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  SOC_PB_MGMT_OPERATION_MODE_clear(op_mode);

  op_mode->pp_enable = soc_petra_sw_db_pp_enable_get(
          unit
        );

  op_mode->is_fap20_in_system =
    soc_petra_sw_db_is_fap20_in_system_get(
        unit
      );

  op_mode->is_fap21_in_system =
    soc_petra_sw_db_is_fap21_in_system_get(
        unit
      );

  op_mode->is_petra_rev_a_in_system =
    soc_petra_sw_db_is_petra_rev_a_in_system_get(
        unit
      );

  op_mode->is_fe200_fabric =
    soc_petra_sw_db_is_fe200_fabric_get(
        unit
      );

  op_mode->is_fe1600_in_system = 
      soc_petra_sw_db_is_fe1600_in_system_get(
        unit
      );

  op_mode->fabric_cell_format.variable_cell_size_enable =
    soc_petra_sw_db_is_fabric_variable_cell_size_get(
        unit
      );

  op_mode->fabric_cell_format.segmentation_enable =
    soc_petra_sw_db_is_fabric_packet_segmentation_get(
        unit
      );

  for (combo_idx = 0; combo_idx < SOC_PETRA_COMBO_NOF_QRTTS; combo_idx++)
  {
    op_mode->is_combo_nif[combo_idx] = soc_petra_sw_db_combo_nif_not_fabric_get(
                                          unit,
                                          combo_idx
                                        );
  }

  op_mode->stag_enable =
    soc_petra_sw_db_stag_enable_get(
        unit
    );

  op_mode->ref_clocks_conf.combo_ref_clk =
    soc_petra_sw_db_ref_clocks_combo_get(
      unit
    );

  op_mode->ref_clocks_conf.fabric_ref_clk =
    soc_petra_sw_db_ref_clocks_fabric_get(
      unit
    );

  op_mode->egr_mc_16k_groups_enable =
    soc_petra_sw_db_egr_mc_16k_groups_enable_get(
        unit
      );

  op_mode->ref_clocks_conf.nif_ref_clk =
    soc_petra_sw_db_ref_clocks_nif_get(
      unit
    );

  op_mode->tdm_mode =
    soc_petra_sw_db_tdm_mode_get(
      unit
    );

  op_mode->ftmh_lb_ext_mode =
    soc_petra_sw_db_ftmh_lb_ext_mode_get(
      unit
    );

  op_mode->add_pph_eep_ext =
    soc_petra_sw_db_add_pph_eep_ext_get(
      unit
    );


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_operation_mode_get_unsafe()", 0, 0);
}


/*********************************************************************
*     Set the maximal allowed packet size. The limitation can
 *     be performed based on the packet size before or after
 *     the ingress editing (external and internal configuration
 *     mode, accordingly). Packets above the specified value
 *     are dropped.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_max_pckt_size_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  uint32                       max_size
  )
{
  uint32
    max_size_lcl,
    res = SOC_SAND_OK;
  SOC_PB_IDR_CONTEXT_MRU_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_MAX_PCKT_SIZE_SET_UNSAFE);

  res = soc_pb_idr_context_mru_tbl_get_unsafe(
          unit,
          port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  switch(conf_mode_ndx) {
  case SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_INTERN:
    max_size_lcl = (max_size)-SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_INTERNAL;
    tbl_data.size = max_size_lcl;
    break;

  case SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_EXTERN:
    max_size_lcl = (max_size)-SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_EXTERNAL;
    tbl_data.org_size = max_size_lcl;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_PCKT_SIZE_CONF_MODE_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = soc_pb_idr_context_mru_tbl_set_unsafe(
          unit,
          port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_max_pckt_size_set_unsafe()", port_ndx, 0);
}

uint32
  soc_pb_mgmt_max_pckt_size_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  uint32                       max_size
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_MAX_PCKT_SIZE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_PORT_NDX_MAX, SOC_PB_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(conf_mode_ndx, SOC_PB_CONF_MODE_NDX_MAX, SOC_PB_MGMT_PCKT_SIZE_CONF_MODE_OUT_OF_RANGE_ERR, 20, exit);

  switch(conf_mode_ndx) {
  case SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_INTERN:
    SOC_SAND_ERR_IF_ABOVE_MAX(max_size, SOC_PETRA_MGMT_PCKT_MAX_SIZE_INTERNAL_MAX, SOC_PB_MGMT_PCKT_MAX_SIZE_INTERNAL_OUT_OF_RANGE_ERROR, 30, exit);
    break;
  case SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_EXTERN:
        SOC_SAND_ERR_IF_ABOVE_MAX(max_size, SOC_PETRA_MGMT_PCKT_MAX_SIZE_EXTERNAL_MAX, SOC_PB_MGMT_PCKT_MAX_SIZE_EXTERNAL_OUT_OF_RANGE_ERROR, 40, exit);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_PCKT_SIZE_CONF_MODE_OUT_OF_RANGE_ERR, 50, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_max_pckt_size_set_verify()", port_ndx, 0);
}

uint32
  soc_pb_mgmt_max_pckt_size_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_MAX_PCKT_SIZE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_PORT_NDX_MAX, SOC_PB_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(conf_mode_ndx, SOC_PB_CONF_MODE_NDX_MAX, SOC_PB_MGMT_PCKT_SIZE_CONF_MODE_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_max_pckt_size_get_verify()", port_ndx, 0);
}

/*********************************************************************
*     Set the maximal allowed packet size. The limitation can
 *     be performed based on the packet size before or after
 *     the ingress editing (external and internal configuration
 *     mode, accordingly). Packets above the specified value
 *     are dropped.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_max_pckt_size_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT uint32                       *max_size
  )
{
  uint32
    max_size_lcl,
    res = SOC_SAND_OK;
  SOC_PB_IDR_CONTEXT_MRU_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_MAX_PCKT_SIZE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(max_size);

  res = soc_pb_idr_context_mru_tbl_get_unsafe(
          unit,
          port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  switch(conf_mode_ndx) {
  case SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_INTERN:
    max_size_lcl = tbl_data.size;
    *max_size = max_size_lcl + SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_INTERNAL;
    break;

  case SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_EXTERN:
    max_size_lcl = tbl_data.org_size;
    *max_size = max_size_lcl + SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_EXTERNAL;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_OUT_OF_RANGE_ERR, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_max_pckt_size_get_unsafe()", port_ndx, 0);
}

uint32
  soc_pb_mgmt_pckt_size_range_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE          *size_range
  )
{
  uint32
    max_size,
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    port_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_PCKT_SIZE_RANGE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(size_range);

  regs = soc_petra_regs();

  /*
   *  Minimum size configuration
   */
  switch(conf_mode_ndx) {
  case SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_INTERN:
    SOC_PB_FLD_SET(regs->idr.packet_sizes_reg.min_packet_size, (size_range->min)-SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_INTERNAL, 10, exit);
    SOC_PB_FLD_SET(regs->egq.maximum_fragment_number_reg.fabric_min_pkt_size, size_range->min, 20, exit);
    break;
 
  case SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_EXTERN:
    SOC_PB_FLD_SET(regs->idr.packet_sizes_reg.min_org_packet_size, (size_range->min)-SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_EXTERNAL, 30, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_PCKT_SIZE_CONF_MODE_OUT_OF_RANGE_ERR, 40, exit);
  }

  /*
   *  Maximum size configuration
   */

  max_size = size_range->max;

  for (port_ndx = 0; port_ndx < SOC_PETRA_NOF_FAP_PORTS; ++port_ndx)
  {
    res = soc_pb_mgmt_max_pckt_size_set_unsafe(
            unit,
            port_ndx,
            conf_mode_ndx,
            max_size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_pckt_size_range_set_unsafe()",0,0);
}

uint32
  soc_pb_mgmt_pckt_size_range_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT SOC_PETRA_MGMT_PCKT_SIZE          *size_range
  )
{
  uint32
    port_ndx,
    max_size,
    idr_min,
    egq_min,
    fld_val,
    res;
  SOC_PETRA_REGS
    *regs;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_PCKT_SIZE_RANGE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(size_range);

  regs = soc_petra_regs();

  soc_petra_PETRA_MGMT_PCKT_SIZE_clear(size_range);

  switch(conf_mode_ndx) {
  case SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_INTERN:
    SOC_PB_FLD_GET(regs->idr.packet_sizes_reg.min_packet_size, idr_min, 10, exit);
    SOC_PB_FLD_GET(regs->egq.maximum_fragment_number_reg.fabric_min_pkt_size, egq_min, 20, exit);

    if ((idr_min + SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_INTERNAL) != egq_min)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_MIN_PCKT_SIZE_INCONSISTENT_ERR, 30, exit);
    }

    size_range->min = idr_min + SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_INTERNAL;
    break;

  case SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_EXTERN:

    SOC_PB_FLD_GET(regs->idr.packet_sizes_reg.min_org_packet_size, fld_val, 40, exit);
    if (fld_val == 0x0)
    {
      size_range->min = SOC_PETRA_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT;
    }
    else
    {
      SOC_PB_FLD_GET(regs->idr.packet_sizes_reg.min_org_packet_size, idr_min, 50, exit);
       size_range->min = idr_min + SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_EXTERNAL;
    }
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_OUT_OF_RANGE_ERR, 60, exit);
  }

  /*
   *  Get the max size from port 0
   */
  port_ndx = 0;
  res = soc_pb_mgmt_max_pckt_size_get_unsafe(
          unit,
          port_ndx,
          conf_mode_ndx,
          &max_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  size_range->max = max_size;
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_pckt_size_range_get_unsafe()",0,0);
}

/*********************************************************************
*     Get the core clock frequency of the device.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_core_frequency_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint32                       *clk_freq
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_CORE_FREQUENCY_GET_UNSAFE);

  *clk_freq = soc_petra_chip_mega_ticks_per_sec_get(unit);

  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_core_frequency_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Enable / Disable the device from receiving and
*     transmitting traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_enable_traffic_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8  enable
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    reg_val,
    enable_val,
    disable_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_ENABLE_TRAFFIC_SET_UNSAFE);

  regs = soc_petra_regs();

  enable_val  = SOC_SAND_BOOL2NUM(enable);
  disable_val = SOC_SAND_BOOL2NUM_INVERSE(enable);
  /*
   *  Note: the comments are for disabling the traffic.
   *  For enabling the traffic, the action is the oposite of the described
   */

  /*
   *  Disable data path at the ingress -
   *  I.e., if disabled - no packet will enter the ingress DRAM and fabric.
   *  IDR, IRR, IRE
   */
  if (enable == FALSE)
  {
    /*
     *  Stop traffic from the NIFs
     */
    SOC_PETRA_FLD_SET(regs->ire.dynamic_configuration_reg.enable_data_path, enable_val, 8, exit);
    /*
     * Stop traffic from the fabric.
     */
    SOC_PETRA_REG_SET(regs->fdr.fdr_enablers_reg, SOC_PETRA_MGMT_FDR_TRFC_DISABLE, 20, exit);
  }

  /*
   *  Disable internal data-path - just in case
   */
  SOC_PETRA_FLD_SET(regs->idr.dynamic_configuration_reg_cmn.enable_data_path, enable_val, 2, exit);
  SOC_PETRA_FLD_SET(regs->irr.dynamic_configuration_reg.enable_data_path_idr, enable_val, 4, exit);
  SOC_PETRA_FLD_SET( regs->irr.dynamic_configuration_reg.enable_data_path_iqm, enable_val, 6, exit);
  SOC_PB_FLD_SET( regs->ihp.ihp_enablers_reg.enable_data_path, enable_val, 8, exit);

  /*
   *  Stop credit reception from the fabric
   */
  SOC_PETRA_FLD_SET(regs->sch.scheduler_configuration_reg.smpdisable_fabric, disable_val, 50, exit);

  /*
   *  Stop credit generation
   */
  SOC_PETRA_FLD_SET(regs->sch.dvs_config1_reg.force_pause, disable_val, 60, exit);

  /*
   *  Stop dequeue from all queues
   */
  SOC_PETRA_FLD_SET(regs->ips.ips_general_configurations_reg.dis_deq_cmds, disable_val, 70, exit);

  /*
   *  Discard all credits in the IPS
   */
  SOC_PETRA_FLD_SET(regs->ips.ips_general_configurations_reg.discard_all_crdt, disable_val, 80, exit);

  /*
   *  Discard all packets currently in the queues
   */
  SOC_PETRA_FLD_SET(regs->iqm.iqm_enablers_reg.dscrd_all_pkt, disable_val, 90, exit);

  if (enable == TRUE)
  {
    SOC_PETRA_FLD_SET(regs->ire.dynamic_configuration_reg.enable_data_path, enable_val, 100, exit);
    /*
     * Set the device to receive from the fabric.
     */
    res = soc_petra_mgmt_fdr_enablers_val_get(
            unit,
            enable,
            &reg_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
    SOC_PETRA_REG_SET(regs->fdr.fdr_enablers_reg, reg_val, 120, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_enable_traffic_set_unsafe()",0,0);
}

/*********************************************************************
*     Set the Soc_petra-B B0 revision specific features.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_rev_b0_set_unsafe(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  SOC_PB_MGMT_B0_INFO *info
  )
{
  uint32
    rtp_update_enable,
    fld_val,
    fld_val_0_for_b0,
    fld_val_1_for_b0,
    res = SOC_SAND_OK;
  SOC_PB_REGS
    *tm_regs;
  SOC_PB_PP_REGS
    *regs;
  uint32
    fld_idx;
  uint8
    is_mc_enabled;
  SOC_PB_MGMT_OPERATION_MODE
    op_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_REV_B0_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  tm_regs = soc_pb_regs();
  regs = soc_pb_pp_regs();

  if (info->bugfixes_enabled == TRUE)
  {
    fld_val_0_for_b0 = 0x0;
    fld_val_1_for_b0 = 0x1;
  }
  else
  {
    /* Keep as in A1 */
    fld_val_0_for_b0 = 0x1;
    fld_val_1_for_b0 = 0x0;
  }

  SOC_PB_IMPLICIT_FLD_SET(tm_regs->idr.soc_petra_c_fix.mc_id_use_uc_buffer, fld_val_0_for_b0, 10, exit);
  SOC_PB_PP_FLD_SET(regs->ihp.soc_petra_c_bugfix_reg.llr_authentication_chicken_bit, fld_val_0_for_b0, 11, exit);
  SOC_PB_PP_FLD_SET(regs->ihp.soc_petra_c_bugfix_reg.mact_aging_high_res_chicken_bit, fld_val_0_for_b0, 12, exit);
  SOC_PB_PP_FLD_SET(regs->ihb.soc_petra_c_chicken_bits_reg.chicken_bit_checksum_err, fld_val_0_for_b0, 14, exit);
  SOC_PB_PP_FLD_SET(regs->ihb.soc_petra_c_chicken_bits_reg.chicken_bit_l2_cp, fld_val_0_for_b0, 15, exit);
  SOC_PB_PP_FLD_SET(regs->ihp.soc_petra_c_bugfix_reg_2.disable_llc_fix, fld_val_0_for_b0, 33, exit);

  SOC_PB_PP_FLD_SET(regs->ihb.soc_petra_c_chicken_bits_reg.chicken_bit_mldigmp_encapsulation, fld_val_0_for_b0, 13, exit);
  /*
   * Set the Innner Ethernet type for this bug fix - same encoding
   */
  fld_val = info->inner_eth_encap;
  SOC_PB_PP_FLD_SET(regs->ihp.vtt_general_configs_reg.inner_eth_encapsulation, fld_val, 20, exit);


  /*
   * EGQ EEP Bugfix: enable it only for PP customers with Soc_petra-A system (source of bugfix)
   * Reset by same bit that FTMH-Ext[1]
   * Note: If fix enable, the Egress Editor must be changed for UC TM
   *       packets when FTMH-Ext is Enabled to take the CUD from the Packet,
   *       otherwise its internal CUD = 0
   */
  SOC_PB_PP_FLD_GET(regs->eci.system_headers_configurations0_reg.ftmh_ext, fld_val, 26, exit);
  SOC_PB_IMPLICIT_FLD_SET(tm_regs->egq.egq_chicken_bits.eep_resolution_chicken_bit, (fld_val >> 1), 27, exit);
  if (info->bugfixes_enabled == TRUE)
  {
    /* Verify PP and Soc_petra-A present */
    if (soc_petra_sw_db_pp_enable_get(unit) == TRUE)
    {
      SOC_PB_MGMT_OPERATION_MODE_clear(&op_mode);
      res = soc_pb_mgmt_operation_mode_get_unsafe(
              unit,
              &op_mode
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);

      if (op_mode.is_petra_rev_a_in_system == TRUE)
      {
        SOC_PB_IMPLICIT_FLD_SET(tm_regs->egq.egq_chicken_bits.eep_resolution_chicken_bit, fld_val_0_for_b0, 25, exit);
      }
    }
  }

  SOC_PB_IMPLICIT_FLD_SET(tm_regs->egq.egq_chicken_bits.egress_learning_chicken_bit, fld_val_1_for_b0, 31, exit);
  SOC_PB_IMPLICIT_FLD_SET(tm_regs->ipt.mirror_snp_fix_reg.mirror_snoop_uc_fix_en, fld_val_1_for_b0, 32, exit);
  SOC_PB_IMPLICIT_FLD_SET(tm_regs->fdt.stat_route_config_reg.soc_pb_gc_bugfix, fld_val_1_for_b0, 34, exit);
  SOC_PB_PP_FLD_SET(regs->ihb.soc_petra_c_chicken_bits_reg.chicken_bit_mldigmp_encapsulation, fld_val_1_for_b0, 35, exit);
  SOC_PB_PP_FLD_SET(regs->ihb.soc_petra_c_chicken_bit_reg.chicken_bit_mc_use_sip_ecmp, fld_val_1_for_b0, 45, exit);

  for (fld_idx = 0; fld_idx < SOC_PB_CHIP_REGS_NOF_CNT_PROCESSOR_IDS; fld_idx++)
  {
    SOC_PB_IMPLICIT_FLD_SET(tm_regs->iqm.soc_petra_c_crps_fix_reg.crps_scan_fix_en[fld_idx], fld_val_1_for_b0, 36 + fld_idx, exit);
    SOC_PB_IMPLICIT_FLD_SET(tm_regs->iqm.soc_petra_c_crps_fix_reg.crps_mapping_fix_en[fld_idx], fld_val_1_for_b0, 38 + fld_idx, exit);
    SOC_PB_IMPLICIT_FLD_SET(tm_regs->iqm.soc_petra_c_crps_fix_reg.crps_ovt_clr_fix_en[fld_idx], fld_val_1_for_b0, 40 + fld_idx, exit);
  }
  SOC_PB_IMPLICIT_FLD_SET(tm_regs->cfc.sch_to_egq_priority_fc_fix_en_reg.ilkn_rx_to_egq_port_fc_fix_en, fld_val_1_for_b0, 42, exit);

  SOC_PB_IMPLICIT_FLD_SET(tm_regs->idr.soc_petra_c_fix.meter_mode_1, fld_val_1_for_b0, 44, exit);
  SOC_PB_IMPLICIT_FLD_SET(tm_regs->idr.soc_petra_c_fix.gmeter_mode_1, fld_val_1_for_b0, 46, exit);

  /*
   * Multicast static route mode
   */
  switch(info->tdm_mc_route_mode) {
  case SOC_PB_INIT_MC_STATIC_ROUTE_MODE_PARTIAL:
    is_mc_enabled = TRUE;
    fld_val = SOC_PB_MGMT_MC_STATIC_ROUTE_MSB_OVERRIDE_4_BITS;
    rtp_update_enable = FALSE;
    break;

  case SOC_PB_INIT_MC_STATIC_ROUTE_MODE_PARTIAL_COMMON:
    is_mc_enabled = TRUE;
    fld_val = SOC_PB_MGMT_MC_STATIC_ROUTE_MSB_OVERRIDE_1_BIT;
    rtp_update_enable = FALSE;
    break;

  case SOC_PB_INIT_MC_STATIC_ROUTE_MODE_FULL:
    is_mc_enabled = TRUE;
    fld_val = SOC_PB_MGMT_MC_STATIC_ROUTE_MSB_OVERRIDE_NO;
    rtp_update_enable = FALSE;
    break;

  case SOC_PB_INIT_MC_STATIC_ROUTE_MODE_DIS:
  default:
    is_mc_enabled = FALSE;
    fld_val = SOC_PB_MGMT_MC_STATIC_ROUTE_MSB_OVERRIDE_NO;
    rtp_update_enable = TRUE;
    break;
  }
  SOC_PB_IMPLICIT_FLD_SET(tm_regs->fdt.stat_route_config_reg.soc_petra_c_str_en, SOC_SAND_BOOL2NUM(is_mc_enabled), 50, exit);
  SOC_PB_IMPLICIT_FLD_SET(tm_regs->fdt.stat_route_config_reg.soc_petra_c_str_mode, fld_val, 52, exit);
  SOC_PB_IMPLICIT_FLD_SET(tm_regs->rtp.rtp_enable_reg.rtp_up_en, rtp_update_enable, 53, exit);

  /* Choose the last 16 lines of the RTP table (FE ids 2032 - 2047) */
  SOC_PB_IMPLICIT_FLD_SET(tm_regs->fdt.stat_route_config_reg.soc_petra_c_str_ofst, 0x7F, 54, exit);

  /* CFC HR Flow Control */
  fld_val = info->is_fc_sch_mode_cbfc;
  SOC_PB_IMPLICIT_FLD_SET(tm_regs->cfc.sch_to_egq_priority_fc_fix_en_reg.sch_to_egq_priority_fc_fix_en, fld_val, 56, exit);

  /* Multicast Single Buffer bitmap */
  fld_val = info->single_copy_mc_rng_bmp;
  SOC_PB_IMPLICIT_FLD_SET(tm_regs->idr.soc_petra_c_fix.mc_id_use_uc_buffer, fld_val, 58, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_rev_b0_set_unsafe()", 0, 0);
}

uint32
  soc_pb_mgmt_rev_b0_set_verify(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  SOC_PB_MGMT_B0_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_REV_B0_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_MGMT_B0_INFO, info, 10, exit);

  if (SOC_PB_REV_A1_OR_BELOW == TRUE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_DEVICE_REVISION_REV_A1_OR_BELOW_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_rev_b0_set_verify()", 0, 0);
}

/*********************************************************************
*     Set the Soc_petra-B B0 revision specific features.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_rev_b0_get_unsafe(
    SOC_SAND_IN  int       unit,
    SOC_SAND_OUT SOC_PB_MGMT_B0_INFO *info
  )
{
  uint32
    is_mc_enabled,
    fld_val,
    fld_val_0_for_b0,
    res = SOC_SAND_OK;
  SOC_PB_REGS
    *tm_regs;
  SOC_PB_PP_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_REV_B0_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_MGMT_B0_INFO_clear(info);
  tm_regs = soc_pb_regs();
  regs = soc_pb_pp_regs();

  /* In case of a0 / a1, return the default values */
  if (SOC_PB_REV_A1_OR_BELOW == TRUE)
  {
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  SOC_PB_IMPLICIT_FLD_GET(tm_regs->idr.soc_petra_c_fix.mc_id_use_uc_buffer, fld_val_0_for_b0, 10, exit);

  info->bugfixes_enabled = (fld_val_0_for_b0 == 0)? TRUE : FALSE;

  /*
   * Get the Innner Ethernet type - same encoding
   */
  SOC_PB_PP_FLD_GET(regs->ihp.vtt_general_configs_reg.inner_eth_encapsulation, fld_val, 20, exit);
  info->inner_eth_encap = fld_val;

  /*
   * Multicast static route mode
   */
  SOC_PB_IMPLICIT_FLD_GET(tm_regs->fdt.stat_route_config_reg.soc_petra_c_str_en, is_mc_enabled, 30, exit);

  SOC_PB_IMPLICIT_FLD_GET(tm_regs->fdt.stat_route_config_reg.soc_petra_c_str_mode, fld_val, 32, exit);
  if (is_mc_enabled == FALSE)
  {
    info->tdm_mc_route_mode = SOC_PB_INIT_MC_STATIC_ROUTE_MODE_DIS;
  }
  else if (fld_val == SOC_PB_MGMT_MC_STATIC_ROUTE_MSB_OVERRIDE_NO)
  {
    info->tdm_mc_route_mode = SOC_PB_INIT_MC_STATIC_ROUTE_MODE_FULL;
  }
  else if (fld_val == SOC_PB_MGMT_MC_STATIC_ROUTE_MSB_OVERRIDE_1_BIT)
  {
    info->tdm_mc_route_mode = SOC_PB_INIT_MC_STATIC_ROUTE_MODE_PARTIAL_COMMON;
  }
  else
  {
    info->tdm_mc_route_mode = SOC_PB_INIT_MC_STATIC_ROUTE_MODE_PARTIAL;
  }

  /* CFC HR Flow Control */
  SOC_PB_IMPLICIT_FLD_GET(tm_regs->cfc.sch_to_egq_priority_fc_fix_en_reg.sch_to_egq_priority_fc_fix_en, fld_val, 56, exit);
  info->is_fc_sch_mode_cbfc = SOC_SAND_NUM2BOOL(fld_val);

  /* Multicast Single Buffer bitmap */
  SOC_PB_IMPLICIT_FLD_GET(tm_regs->idr.soc_petra_c_fix.mc_id_use_uc_buffer, fld_val, 58, exit);
  info->single_copy_mc_rng_bmp = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_rev_b0_get_unsafe()", 0, 0);
}

uint32
  SOC_PB_MGMT_OPERATION_MODE_verify(
    SOC_SAND_IN  SOC_PB_MGMT_OPERATION_MODE *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_mgmt_ref_clk_verify(info->ref_clocks_conf.nif_ref_clk);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_mgmt_ref_clk_verify(info->ref_clocks_conf.combo_ref_clk);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = soc_petra_mgmt_ref_clk_verify(info->ref_clocks_conf.fabric_ref_clk);
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tdm_mode, SOC_PETRA_MGMT_NOF_TDM_MODES - 1, SOC_PB_MGMT_TDM_MODE_OF_RANGE_ERR, 15, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ftmh_lb_ext_mode, SOC_PB_MGMT_NOF_FTMH_LB_EXT_MODES - 1, SOC_PB_MGMT_FTMH_LB_EXT_MODE_OF_RANGE_ERR, 16, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_MGMT_OPERATION_MODE_verify()",0,0);
}

uint32
  SOC_PB_INIT_PP_PORT_verify(
    SOC_SAND_IN  SOC_PB_INIT_PP_PORT *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->profile_ndx, (SOC_PB_PORT_NOF_PP_PORTS-1), SOC_PB_MGMT_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_PORT_PP_PORT_INFO, &(info->conf), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_INIT_PP_PORT_verify()",0,0);
}

uint32
  SOC_PB_INIT_PP_PORT_MAP_verify(
    SOC_SAND_IN  SOC_PB_INIT_PP_PORT_MAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->port_ndx, SOC_PB_PORT_NDX_MAX, SOC_PETRA_FAP_PORT_ID_INVALID_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pp_port, SOC_PB_PP_PORT_NDX_MAX, SOC_PB_PORTS_PP_PORT_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_INIT_PP_PORT_MAP_verify()",0,0);
}

uint32
  SOC_PB_INIT_PORTS_verify(
    SOC_SAND_IN  SOC_PB_INIT_PORTS *info
  )
{
  uint32
    ind;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->hdr_type_nof_entries, SOC_PB_INIT_PORTS_HDR_TYPE_NOF_ENTRIES_MAX, SOC_PB_INIT_HDR_TYPE_NOF_ENTRIES_OUT_OF_RANGE_ERR, 10, exit);
  for (ind = 0; ind < SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->hdr_type[ind].port_ndx, SOC_PB_INIT_PORTS_HDR_TYPE_NOF_ENTRIES_MAX - 1, SOC_PB_INIT_HDR_TYPE_NOF_ENTRIES_OUT_OF_RANGE_ERR, 11, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->if_map_nof_entries, SOC_PB_INIT_PORTS_IF_MAP_NOF_ENTRIES_MAX, SOC_PB_INIT_IF_MAP_NOF_ENTRIES_OUT_OF_RANGE_ERR, 12, exit);
  for (ind = 0; ind < SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->if_map[ind].port_ndx, SOC_PB_INIT_PORTS_IF_MAP_NOF_ENTRIES_MAX - 1, SOC_PB_INIT_IF_MAP_NOF_ENTRIES_OUT_OF_RANGE_ERR, 13, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pp_port_nof_entries, SOC_PB_MGMT_PP_PORT_NOF_ENTRIES_MAX, SOC_PB_MGMT_PP_PORT_NOF_ENTRIES_OUT_OF_RANGE_ERR, 14, exit);
  for (ind = 0; ind < SOC_PB_PORT_NOF_PP_PORTS; ++ind)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_INIT_PP_PORT, &(info->pp_port[ind]), 15, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tm2pp_port_map_nof_entries, SOC_PB_MGMT_TM_PROFILE_MAP_NOF_ENTRIES_MAX, SOC_PB_MGMT_TM_PROFILE_MAP_NOF_ENTRIES_OUT_OF_RANGE_ERR, 16, exit);
  for (ind = 0; ind < SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_INIT_PP_PORT_MAP, &(info->tm2pp_port_map[ind]), 17, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_INIT_PORTS_verify()",0,0);
}

uint32
  SOC_PB_HW_ADJ_MAL_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_MAL *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mal_ndx, SOC_PB_HW_ADJ_MAL_MAL_NDX_MAX, SOC_PB_HW_MAL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_MAL_BASIC_INFO, &(info->conf), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_HW_ADJ_MAL_verify()",0,0);
}

uint32
  SOC_PB_HW_ADJ_SPAUI_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_SPAUI *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_interface_id_verify(info->if_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_SPAUI_INFO, &(info->conf), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_HW_ADJ_SPAUI_verify()",0,0);
}

uint32
  SOC_PB_HW_ADJ_GMII_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_GMII *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_GMII_INFO, &(info->conf), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_HW_ADJ_GMII_verify()",0,0);
}

uint32
  SOC_PB_HW_ADJ_FATP_verify(
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE mode,
    SOC_SAND_IN  SOC_PB_HW_ADJ_FATP *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = SOC_PB_HW_ADJ_FATP_verify(
          mode,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_HW_ADJ_FATP_verify()",0,0);
}

uint32
  SOC_PB_HW_ADJ_ILKN_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_ILKN *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_ILKN_INFO, &(info->conf), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_HW_ADJ_ILKN_verify()",0,0);
}

uint32
  SOC_PB_HW_ADJ_ELK_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_ELK *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_ELK_INFO, &(info->conf), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_HW_ADJ_ELK_verify()",0,0);
}

uint32
  SOC_PB_HW_ADJ_SYNCE_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_SYNCE *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ind;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_NIF_NOF_SYNCE_CLK_IDS; ++ind)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_SYNCE_CLK, &(info->conf[ind]), 12, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_HW_ADJ_SYNCE_verify()",0,0);
}

uint32
  SOC_PB_HW_ADJ_NIF_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_NIF *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ind;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mal_nof_entries, SOC_PB_HW_ADJ_NIF_MAL_NOF_ENTRIES_MAX, SOC_PB_HW_MAL_NOF_ENTRIES_OUT_OF_RANGE_ERR, 10, exit);
  for (ind = 0; ind < SOC_PB_NOF_MAC_LANES; ++ind)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_HW_ADJ_MAL, &(info->mal[ind]), 11, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->spaui_nof_entries, SOC_PB_HW_ADJ_NIF_SPAUI_NOF_ENTRIES_MAX, SOC_PB_HW_SPAUI_NOF_ENTRIES_OUT_OF_RANGE_ERR, 12, exit);
  for (ind = 0; ind < SOC_PB_NOF_MAC_LANES; ++ind)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_HW_ADJ_SPAUI, &(info->spaui[ind]), 13, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->gmii_nof_entries, SOC_PB_HW_ADJ_NIF_GMII_NOF_ENTRIES_MAX, SOC_PB_HW_GMII_NOF_ENTRIES_OUT_OF_RANGE_ERR, 14, exit);
  for (ind = 0; ind < SOC_PETRA_IF_NOF_NIFS; ++ind)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_HW_ADJ_GMII, &(info->gmii[ind]), 15, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ilkn_nof_entries, SOC_PB_HW_ADJ_NIF_ILKN_NOF_ENTRIES_MAX, SOC_PB_HW_ILKN_NOF_ENTRIES_OUT_OF_RANGE_ERR, 16, exit);
  for (ind = 0; ind < SOC_PB_NIF_NOF_ILKN_IDS; ++ind)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->ilkn[ind].ilkn_ndx, SOC_PB_NIF_ILKN_ID_B, SOC_PB_ILKN_NDX_OUT_OF_RANGE_ERR, 10, exit);
    SOC_PB_STRUCT_VERIFY(SOC_PB_HW_ADJ_ILKN, &(info->ilkn[ind]), 17, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->fatp_nof_entries, SOC_PB_HW_ADJ_NIF_FATP_NOF_ENTRIES_MAX, SOC_PB_HW_FATP_NOF_ENTRIES_OUT_OF_RANGE_ERR, 18, exit);
  for (ind = 0; ind < SOC_PB_NIF_NOF_FATP_IDS; ++ind)
  {
    res = SOC_PB_HW_ADJ_FATP_verify(
          info->fatp_mode.mode,
          &(info->fatp[ind])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  if (info->mdio_nof_entries > 0)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->mdio.clk_freq_khz, SOC_PB_MGMT_MDIO_CLK_FREQ_MIN, SOC_PB_MGMT_MDIO_CLK_FREQ_MAX, SOC_PB_MDIO_CLK_FREQ_ERR, 21, exit);
  }
    
  SOC_PB_STRUCT_VERIFY(SOC_PB_HW_ADJ_ELK, &(info->elk), 22, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_HW_ADJ_SYNCE, &(info->synce), 23, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_HW_ADJ_NIF_verify()",0,0);
}

uint32
  SOC_PB_HW_ADJ_FC_SCHEME_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_FC_SCHEME *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->conf, SOC_PB_FC_NOF_NIF_OVERSUBSCR_SCHEMES-1, SOC_PB_NIF_OVERSUBSCR_SCHEME_OUT_OF_RANGE_ERR, 14, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_HW_ADJ_FC_SCHEME_verify()",0,0);
}

uint32
  SOC_PB_HW_ADJ_DRAM_PLL_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_DRAM_PLL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->f, SOC_PB_HW_ADJ_DRAM_PLL_F_MIN, SOC_PB_HW_ADJ_DRAM_PLL_F_MAX, SOC_PB_DRAM_PLL_F_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->q, SOC_PB_HW_ADJ_DRAM_PLL_Q_MIN, SOC_PB_HW_ADJ_DRAM_PLL_Q_MAX, SOC_PB_DRAM_PLL_Q_OUT_OF_RANGE_ERR, 12, exit);
  /* SOC_PB_HW_ADJ_DRAM_PLL_R_MIN may be changed and be more thean 0 */
  /* coverity[unsigned_compare : FALSE] */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->r, SOC_PB_HW_ADJ_DRAM_PLL_R_MIN, SOC_PB_HW_ADJ_DRAM_PLL_R_MAX, SOC_PB_DRAM_PLL_R_OUT_OF_RANGE_ERR, 14, exit);
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_HW_ADJ_STAT_IF_verify()",0,0);
}

uint32
  SOC_PB_HW_ADJ_STAT_IF_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_STAT_IF *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_STAT_IF_REPORT_INFO, &(info->rep_conf), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_HW_ADJ_STAT_IF_verify()",0,0);
}

uint32
  SOC_PB_HW_ADJ_STREAMING_IF_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJ_STREAMING_IF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_HW_ADJ_STREAMING_IF_verify()",0,0);
}

uint32
  SOC_PB_HW_ADJUSTMENTS_verify(
    SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_HW_ADJ_DRAM_PLL, &(info->dram_pll), 15, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_HW_ADJ_STAT_IF, &(info->stat_if), 16, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_HW_ADJ_STREAMING_IF, &(info->streaming_if), 16, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_HW_ADJ_FC_SCHEME, &(info->fc_scheme), 17, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_HW_ADJUSTMENTS_verify()",0,0);
}

uint32
  SOC_PB_INIT_STAG_INFO_verify(
    SOC_SAND_IN  SOC_PB_INIT_STAG_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_ITM_STAG_INFO, &(info->encoding), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_INIT_STAG_INFO_verify()",0,0);
}

uint32
  SOC_PB_INIT_BASIC_CONF_verify(
    SOC_SAND_IN  SOC_PB_INIT_BASIC_CONF *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->credit_worth, SOC_PB_INIT_BASIC_CONF_CREDIT_WORTH_MIN, SOC_PB_INIT_BASIC_CONF_CREDIT_WORTH_MAX, SOC_PB_INIT_CREDIT_WORTH_OUT_OF_RANGE_ERR, 11, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_INIT_STAG_INFO, &(info->stag), 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_INIT_BASIC_CONF_verify()",0,0);
}

uint32
  SOC_PB_MGMT_B0_INFO_verify(
    SOC_SAND_IN  SOC_PB_MGMT_B0_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tdm_mc_route_mode, SOC_PB_MGMT_TDM_MC_ROUTE_MODE_MAX, SOC_PB_MGMT_TDM_MC_ROUTE_MODE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_MGMT_B0_INFO_verify()",0,0);
}

#if SOC_PB_DEBUG_IS_LVL1
#endif /* SOC_PB_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


