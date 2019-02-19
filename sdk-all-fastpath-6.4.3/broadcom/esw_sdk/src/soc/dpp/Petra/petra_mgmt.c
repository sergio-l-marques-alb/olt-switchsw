/* $Id: petra_mgmt.c,v 1.17 Broadcom SDK $
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


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/SAND_FM/sand_link.h>

#include <soc/dpp/Petra/petra_mgmt.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_fabric.h>
#include <soc/dpp/Petra/petra_interrupt_service.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#ifdef PETRA_PP
#include <soc/dpp/Petra/PB_PP/pb_pp_mgmt.h>
#endif
#include <soc/dpp/SAND/Management/sand_device_management.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_mgmt.h>
#endif
#include <soc/dpp/Petra/petra_ssr.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PETRA_MGMT_NOF_TBL_MAPS                    1
#define SOC_PETRA_MGMT_NOF_MEM                         2

#define SOC_PETRA_MGMT_INIT_CTRL_CELLS_TIMER_ITERATIONS 160
#define SOC_PETRA_MGMT_INIT_CTRL_RCH_STATUS_ITERATIONS  24
#define SOC_PETRA_MGMT_INIT_STANDALONE_ITERATIONS       16
#define SOC_PETRA_MGMT_INIT_CTRL_CELLS_TIMER_DELAY_MSEC 32

/* $Id: petra_mgmt.c,v 1.17 Broadcom SDK $
 * The number of fields being enabled / disabled while enabling / disabling ctrl cells
 */
#define SOC_PETRA_MGMT_CTRL_CELLS_MAX_NOF_CONFS 30

#define SOC_PETRA_MGMT_DBG_ON                   0

#if SOC_PETRA_MGMT_DBG_ON
  #define SOC_PETRA_MGMT_TRACE                                    \
  {                                                           \
    static uint8 p_mgmt_is_traced[SOC_SAND_MAX_DEVICE] = {0}; \
    if (!p_mgmt_is_traced[unit])                         \
    {                                                         \
      soc_sand_os_printf(" %s\r\n", FUNCTION_NAME());         \
      p_mgmt_is_traced[unit] = TRUE;                     \
    }                                                         \
  }
#else
  #define SOC_PETRA_MGMT_TRACE
#endif

/* 
 * SW version identification for internal use of the driver.
 */
#define SOC_PA_MGMT_VER                                            (0x0703)

/*
 *  Chip identification fields (Version Register).
 *  Needed by register_device, before access database is initialized
 */
/* } */

/*************
 *  MACROS   *
 *************/
/* { */
#define SOC_PETRA_MGMT_IS_REVISION_PP(rev) \
  SOC_SAND_NUM2BOOL(rev == SOC_PETRA_REVISION_TYPE_88330)

typedef uint32 (*SOC_PETRA_CTRL_CELL_POLL_FUNC) (SOC_SAND_IN  int unit, SOC_SAND_OUT uint8 *success) ;
typedef uint32 (*SOC_PETRA_CTRL_CELL_FNLY_FUNC) (SOC_SAND_IN  int unit) ;

typedef struct
{
  SOC_PETRA_REG_FIELD             *field;

  uint32                  instance_id;

  SOC_PETRA_CTRL_CELL_POLL_FUNC   polling_func;

  SOC_PETRA_CTRL_CELL_FNLY_FUNC  failure_func;

  uint32                    val;

  /*
   *  In case a polling function is used,
   *  this is the number of iterations (at least one is performed).
   *  Otherwise - this is the delay in milliseconds.
   */
  uint32                    delay_or_polling_iters;

  uint32                    err_on_fail;

}SOC_PETRA_CTRL_CELL_DATA;

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
    Soc_petra_mgmt_indirect_tables_map_init = FALSE;
static
  uint8
    Soc_petra_mgmt_indirect_memory_map_init = FALSE;
static
  uint8
    Soc_petra_mgmt_indirect_module_info_init = FALSE;
static
  SOC_SAND_INDIRECT_TABLES_INFO
    Soc_petra_mgmt_indirect_tables_map[SOC_PETRA_MGMT_NOF_TBL_MAPS];
static
  SOC_SAND_INDIRECT_MEMORY_MAP
    Soc_petra_mgmt_indirect_memory_map[SOC_PETRA_MGMT_NOF_MEM];
static
  SOC_SAND_INDIRECT_MODULE_INFO
    Soc_petra_mgmt_indirect_module_info[SOC_PETRA_NUM_OF_INDIRECT_MODULES];
/* Indirect access } */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_pa_mgmt_ver_set(
    SOC_SAND_IN int unit
  )
{
  uint32  
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_SEQUENCE_FIXES_APPLY);

  regs = soc_petra_regs();

  SOC_PA_FLD_SET(regs->olp.scratch_pad_reg.scratch_pad, SOC_PA_MGMT_VER, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pa_mgmt_ver_set()", 0, 0);
}

uint32
  soc_pa_mgmt_ver_get(
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

  SOC_PA_FLD_GET(regs->olp.scratch_pad_reg.scratch_pad, fld_val, 10, exit);
  *ver = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pa_mgmt_ver_set()", 0, 0);
}

uint32
  soc_petra_mgmt_ver_get(
    SOC_SAND_IN int unit,
    SOC_SAND_OUT uint32  *ver
  )
{
  uint32  
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_SEQUENCE_FIXES_APPLY);
  SOC_SAND_CHECK_NULL_INPUT(ver);

  SOC_PETRA_DIFF_DEVICE_CALL(mgmt_ver_get,(unit, ver));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_ver_get()", 0, 0);
}

uint32
  soc_petra_mgmt_ver_set(
    SOC_SAND_IN int unit
  )
{
  uint32  
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_SEQUENCE_FIXES_APPLY);

  SOC_PETRA_DIFF_DEVICE_CALL(mgmt_ver_set,(unit));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_ver_set()", 0, 0);
}

uint32
  soc_petra_mgmt_fdr_enablers_val_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint8 enable,
    SOC_SAND_OUT uint32  *value
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_val,
    fld_val,
    is_variable_size,
    is_segmentation;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_FDR_ENABLERS_VAL_GET);
  SOC_SAND_CHECK_NULL_INPUT(value);

  regs = soc_petra_regs();

  if(enable)
  {
    is_variable_size = soc_petra_sw_db_is_fabric_variable_cell_size_get(unit);
    is_segmentation  = soc_petra_sw_db_is_fabric_packet_segmentation_get(unit);
    
    reg_val = (is_variable_size)?
      SOC_PETRA_MGMT_FDR_TRFC_ENABLE_VAR_CELL:
      SOC_PETRA_MGMT_FDR_TRFC_ENABLE_FIX_CELL;

    SOC_PETRA_FLD_GET(regs->fdt.fdt_enabler_reg.segment_pkt, fld_val, 10, exit);
    fld_val = is_segmentation ? 0x0 : 0x1;
    SOC_PETRA_FLD_TO_REG(regs->fdr.fdr_enablers_reg.bits_1_0, fld_val, reg_val, 20, exit);
      
    *value = reg_val;
  }
  else
  {
    *value = 0x0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_fdr_enablers_val_get()", 0, 0);
}

uint32
  soc_pa_mgmt_init_sequence_fixes_apply_unsafe(
    SOC_SAND_IN int unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_SEQUENCE_FIXES_APPLY);

  SOC_PETRA_DO_NOTHING_AND_EXIT;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pa_mgmt_init_sequence_fixes_apply_unsafe()", 0, 0);
}

uint32
  soc_petra_mgmt_init_sequence_fixes_apply_unsafe(
    SOC_SAND_IN int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_SEQUENCE_FIXES_APPLY);

  SOC_PETRA_DIFF_DEVICE_CALL(mgmt_init_sequence_fixes_apply_unsafe,(unit));

  res = soc_petra_mgmt_ver_set(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_init_sequence_fixes_apply_unsafe()", 0, 0);
}

/* Indirect modules init { */
uint32
  soc_petra_mgmt_indirect_memory_map_get(
      SOC_SAND_OUT SOC_SAND_INDIRECT_MODULE* info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INDIRECT_MEMORY_MAP_GET);

  SOC_SAND_CHECK_NULL_INPUT(info);

  if (Soc_petra_mgmt_indirect_memory_map_init == FALSE)
  {
     info->memory_map_arr = NULL;
  }
  else
  {
    info->memory_map_arr = Soc_petra_mgmt_indirect_memory_map;
  }

 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_indirect_memory_map_get()",0,0);
}

uint32
  soc_petra_mgmt_indirect_memory_map_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INDIRECT_MEMORY_MAP_INIT);

  if (Soc_petra_mgmt_indirect_memory_map_init == TRUE)
  {
     goto exit;
  }

  Soc_petra_mgmt_indirect_memory_map[0].offset = 0x0;
  Soc_petra_mgmt_indirect_memory_map[0].size = 0xFFFFFFFF;
  Soc_petra_mgmt_indirect_memory_map[1].offset = 0;
  Soc_petra_mgmt_indirect_memory_map[1].size = 0;

  Soc_petra_mgmt_indirect_memory_map_init = TRUE;

 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_indirect_memory_map_init()",0,0);
}
uint32
  soc_petra_mgmt_indirect_table_map_get(
      SOC_SAND_OUT SOC_SAND_INDIRECT_MODULE* info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INDIRECT_TABLE_MAP_GET);

  if (Soc_petra_mgmt_indirect_tables_map_init == FALSE)
  {
    info->tables_info = NULL;
  }
  else
  {
    info->tables_info = Soc_petra_mgmt_indirect_tables_map;
  }

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_indirect_table_map_get()",0,0);
}

uint32
  soc_petra_mgmt_indirect_table_map_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INDIRECT_TABLE_MAP_INIT);

  if (Soc_petra_mgmt_indirect_tables_map_init == TRUE)
  {
     goto exit;
  }

  Soc_petra_mgmt_indirect_tables_map[0].read_result_offset = 0;
  Soc_petra_mgmt_indirect_tables_map[0].tables_prefix = 0;
  Soc_petra_mgmt_indirect_tables_map[0].tables_prefix_nof_bits = 0;
  Soc_petra_mgmt_indirect_tables_map[0].word_size = 0;
  Soc_petra_mgmt_indirect_tables_map[0].write_buffer_offset = 0;

  Soc_petra_mgmt_indirect_tables_map_init = TRUE;

 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_indirect_table_map_init()",0,0);
}

uint32
  soc_petra_mgmt_indirect_module_info_get(
      SOC_SAND_OUT SOC_SAND_INDIRECT_MODULE* info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INDIRECT_MODULE_INFO_GET);

  if (Soc_petra_mgmt_indirect_tables_map_init == FALSE)
  {
    info->info_arr = NULL;
    info->info_arr_max_index = 0;
  }
  else
  {
    info->info_arr = Soc_petra_mgmt_indirect_module_info;
    info->info_arr_max_index = SOC_PETRA_NUM_OF_INDIRECT_MODULES;
  }

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_indirect_module_info_get()",0,0);
}

uint32
  soc_petra_mgmt_indirect_module_info_init(void)
{
  uint32
    err = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = NULL;
  uint32
    dpi_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INDIRECT_MODULE_INFO_INIT);

  err =
    soc_petra_regs_get(
      &(regs)
    );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  SOC_SAND_CHECK_NULL_INPUT(regs);

/* } */

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_OLP_ID].module_index =
    SOC_PETRA_OLP_ID;
  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_OLP_ID].read_result_offset =
    SOC_PA_REG_DB_ACC(regs->olp.indirect_command_rd_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_OLP_ID].word_size =
    sizeof(uint32);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_OLP_ID].access_trigger =
    SOC_PA_REG_DB_ACC(regs->olp.indirect_command_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_OLP_ID].access_address =
    SOC_PA_REG_DB_ACC(regs->olp.indirect_command_address_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_OLP_ID].write_buffer_offset =
    SOC_PA_REG_DB_ACC(regs->olp.indirect_command_wr_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_OLP_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IRE_ID].module_index =
    SOC_PETRA_IRE_ID;
  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IRE_ID].read_result_offset =
    SOC_PA_REG_DB_ACC(regs->ire.indirect_command_rd_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IRE_ID].word_size =
    sizeof(uint32);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IRE_ID].access_trigger =
    SOC_PA_REG_DB_ACC(regs->ire.indirect_command_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IRE_ID].access_address =
    SOC_PA_REG_DB_ACC(regs->ire.indirect_command_address_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IRE_ID].write_buffer_offset =
    SOC_PA_REG_DB_ACC(regs->ire.indirect_command_wr_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IRE_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IDR_ID].module_index =
    SOC_PETRA_IDR_ID;
  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IDR_ID].read_result_offset =
    SOC_PA_REG_DB_ACC(regs->idr.indirect_command_rd_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IDR_ID].word_size =
    sizeof(uint32);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IDR_ID].access_trigger =
    SOC_PA_REG_DB_ACC(regs->idr.indirect_command_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IDR_ID].access_address =
    SOC_PA_REG_DB_ACC(regs->idr.indirect_command_address_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IDR_ID].write_buffer_offset =
    SOC_PA_REG_DB_ACC(regs->idr.indirect_command_wr_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IDR_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IRR_ID].module_index =
    SOC_PETRA_IRR_ID;
  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IRR_ID].read_result_offset =
    SOC_PA_REG_DB_ACC(regs->irr.indirect_command_rd_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IRR_ID].word_size =
    sizeof(uint32);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IRR_ID].access_trigger =
    SOC_PA_REG_DB_ACC(regs->irr.indirect_command_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IRR_ID].access_address =
    SOC_PA_REG_DB_ACC(regs->irr.indirect_command_address_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IRR_ID].write_buffer_offset =
    SOC_PA_REG_DB_ACC(regs->irr.indirect_command_wr_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IRR_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IHP_ID].module_index =
    SOC_PETRA_IHP_ID;
  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IHP_ID].read_result_offset =
    SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_rd_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IHP_ID].word_size =
    sizeof(uint32);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IHP_ID].access_trigger =
    SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IHP_ID].access_address =
    SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_address_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IHP_ID].write_buffer_offset =
    SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_wr_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IHP_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_QDR_ID].module_index =
    SOC_PETRA_QDR_ID;
  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_QDR_ID].read_result_offset =
    SOC_PA_REG_DB_ACC(regs->qdr.indirect_command_rd_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_QDR_ID].word_size =
    sizeof(uint32);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_QDR_ID].access_trigger =
    SOC_PA_REG_DB_ACC(regs->qdr.indirect_command_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_QDR_ID].access_address =
    SOC_PA_REG_DB_ACC(regs->qdr.indirect_command_address_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_QDR_ID].write_buffer_offset =
    SOC_PA_REG_DB_ACC(regs->qdr.indirect_command_wr_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_QDR_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IPS_ID].module_index =
    SOC_PETRA_IPS_ID;
  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IPS_ID].read_result_offset =
    SOC_PA_REG_DB_ACC(regs->ips.indirect_command_rd_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IPS_ID].word_size =
    sizeof(uint32);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IPS_ID].access_trigger =
    SOC_PA_REG_DB_ACC(regs->ips.indirect_command_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IPS_ID].access_address =
    SOC_PA_REG_DB_ACC(regs->ips.indirect_command_address_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IPS_ID].write_buffer_offset =
    SOC_PA_REG_DB_ACC(regs->ips.indirect_command_wr_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IPS_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IPT_ID].module_index =
    SOC_PETRA_IPT_ID;
  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IPT_ID].read_result_offset =
    SOC_PA_REG_DB_ACC(regs->ipt.indirect_command_rd_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IPT_ID].word_size =
    sizeof(uint32);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IPT_ID].access_trigger =
    SOC_PA_REG_DB_ACC(regs->ipt.indirect_command_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IPT_ID].access_address =
    SOC_PA_REG_DB_ACC(regs->ipt.indirect_command_address_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IPT_ID].write_buffer_offset =
    SOC_PA_REG_DB_ACC(regs->ipt.indirect_command_wr_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IPT_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  for (dpi_ndx = 0; dpi_ndx < SOC_PETRA_NOF_DPI_IDS; ++dpi_ndx)
  {
    Soc_petra_mgmt_indirect_module_info[SOC_PETRA_DPI_A_ID + dpi_ndx].module_index =
      SOC_PETRA_DPI_A_ID + dpi_ndx;
    Soc_petra_mgmt_indirect_module_info[SOC_PETRA_DPI_A_ID + dpi_ndx].read_result_offset =
      SOC_PA_REG_DB_ACC(regs->dpi.indirect_command_rd_data_reg_0.addr.base) +
      (dpi_ndx * SOC_PA_REG_DB_ACC(regs->dpi.indirect_command_rd_data_reg_0.addr.step));

    Soc_petra_mgmt_indirect_module_info[SOC_PETRA_DPI_A_ID + dpi_ndx].word_size =
      sizeof(uint32);

    Soc_petra_mgmt_indirect_module_info[SOC_PETRA_DPI_A_ID + dpi_ndx].access_trigger =
      SOC_PA_REG_DB_ACC(regs->dpi.indirect_command_reg.addr.base) +
      (dpi_ndx * SOC_PA_REG_DB_ACC(regs->dpi.indirect_command_reg.addr.step));

    Soc_petra_mgmt_indirect_module_info[SOC_PETRA_DPI_A_ID + dpi_ndx].access_address =
      SOC_PA_REG_DB_ACC(regs->dpi.indirect_command_address_reg.addr.base) +
      (dpi_ndx * SOC_PA_REG_DB_ACC(regs->dpi.indirect_command_address_reg.addr.step));

    Soc_petra_mgmt_indirect_module_info[SOC_PETRA_DPI_A_ID + dpi_ndx].write_buffer_offset =
      SOC_PA_REG_DB_ACC(regs->dpi.indirect_command_wr_data_reg_0.addr.base) +
      (dpi_ndx * SOC_PA_REG_DB_ACC(regs->dpi.indirect_command_wr_data_reg_0.addr.step));

    Soc_petra_mgmt_indirect_module_info[SOC_PETRA_DPI_A_ID + dpi_ndx].module_bits =
      SOC_SAND_INDIRECT_ERASE_MODULE_BITS;
  }

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_RTP_ID].module_index =
    SOC_PETRA_RTP_ID;
  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_RTP_ID].read_result_offset =
    SOC_PA_REG_DB_ACC(regs->rtp.indirect_command_rd_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_RTP_ID].word_size =
    sizeof(uint32);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_RTP_ID].access_trigger =
    SOC_PA_REG_DB_ACC(regs->rtp.indirect_command_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_RTP_ID].access_address =
    SOC_PA_REG_DB_ACC(regs->rtp.indirect_command_address_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_RTP_ID].write_buffer_offset =
    SOC_PA_REG_DB_ACC(regs->rtp.indirect_command_wr_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_RTP_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_EGQ_ID].module_index =
    SOC_PETRA_EGQ_ID;
  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_EGQ_ID].read_result_offset =
    SOC_PA_REG_DB_ACC(regs->egq.indirect_command_rd_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_EGQ_ID].word_size =
    sizeof(uint32);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_EGQ_ID].access_trigger =
    SOC_PA_REG_DB_ACC(regs->egq.indirect_command_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_EGQ_ID].access_address =
    SOC_PA_REG_DB_ACC(regs->egq.indirect_command_address_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_EGQ_ID].write_buffer_offset =
    SOC_PA_REG_DB_ACC(regs->egq.indirect_command_wr_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_EGQ_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;


  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_SCH_ID].module_index =
    SOC_PETRA_SCH_ID;
  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_SCH_ID].read_result_offset =
    SOC_PA_REG_DB_ACC(regs->sch.indirect_command_rd_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_SCH_ID].word_size =
    sizeof(uint32);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_SCH_ID].access_trigger =
    SOC_PA_REG_DB_ACC(regs->sch.indirect_command_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_SCH_ID].access_address =
    SOC_PA_REG_DB_ACC(regs->sch.indirect_command_address_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_SCH_ID].write_buffer_offset =
    SOC_PA_REG_DB_ACC(regs->sch.indirect_command_wr_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_SCH_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_CFC_ID].module_index =
    SOC_PETRA_CFC_ID;
  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_CFC_ID].read_result_offset =
    SOC_PA_REG_DB_ACC(regs->cfc.indirect_command_rd_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_CFC_ID].word_size =
    sizeof(uint32);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_CFC_ID].access_trigger =
    SOC_PA_REG_DB_ACC(regs->cfc.indirect_command_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_CFC_ID].access_address =
    SOC_PA_REG_DB_ACC(regs->cfc.indirect_command_address_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_CFC_ID].write_buffer_offset =
    SOC_PA_REG_DB_ACC(regs->cfc.indirect_command_wr_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_CFC_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_EPNI_ID].module_index =
    SOC_PETRA_EPNI_ID;
  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_EPNI_ID].read_result_offset =
    SOC_PA_REG_DB_ACC(regs->epni.indirect_command_rd_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_EPNI_ID].word_size =
    sizeof(uint32);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_EPNI_ID].access_trigger =
    SOC_PA_REG_DB_ACC(regs->epni.indirect_command_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_EPNI_ID].access_address =
    SOC_PA_REG_DB_ACC(regs->epni.indirect_command_address_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_EPNI_ID].write_buffer_offset =
    SOC_PA_REG_DB_ACC(regs->epni.indirect_command_wr_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_EPNI_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IQM_ID].module_index =
    SOC_PETRA_IQM_ID;
  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IQM_ID].read_result_offset =
    SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_rd_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IQM_ID].word_size =
    sizeof(uint32);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IQM_ID].access_trigger =
    SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IQM_ID].access_address =
    SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_address_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IQM_ID].write_buffer_offset =
    SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_wr_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_IQM_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_MMU_ID].module_index =
    SOC_PETRA_MMU_ID;
  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_MMU_ID].read_result_offset =
    SOC_PA_REG_DB_ACC(regs->mmu.indirect_command_rd_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_MMU_ID].word_size =
    sizeof(uint32);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_MMU_ID].access_trigger =
    SOC_PA_REG_DB_ACC(regs->mmu.indirect_command_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_MMU_ID].access_address =
    SOC_PA_REG_DB_ACC(regs->mmu.indirect_command_address_reg.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_MMU_ID].write_buffer_offset =
    SOC_PA_REG_DB_ACC(regs->mmu.indirect_command_wr_data_reg_0.addr.base);

  Soc_petra_mgmt_indirect_module_info[SOC_PETRA_MMU_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Soc_petra_mgmt_indirect_module_info_init = TRUE;

 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_indirect_module_info()",0,0);
}

uint32
  soc_petra_mgmt_indirect_module_get(
      SOC_SAND_OUT SOC_SAND_INDIRECT_MODULE* info
  )
{
  uint32
    res = SOC_SAND_OK;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INDIRECT_MODULE_INIT);

  res = soc_petra_mgmt_indirect_module_info_get(info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_petra_mgmt_indirect_table_map_get(info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  res = soc_petra_mgmt_indirect_memory_map_get(info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_indirect_module_init()",0,0);
}

STATIC uint32
  soc_petra_mgmt_indirect_module_init(void)
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INDIRECT_MODULE_INIT);

  res = soc_petra_mgmt_indirect_module_info_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_petra_mgmt_indirect_table_map_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  res = soc_petra_mgmt_indirect_memory_map_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:

  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_indirect_module_init()",0,0);
}

STATIC uint32
  soc_petra_access_db_init(void)
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ACCESS_DB_INIT);

  res = soc_petra_regs_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_tbls_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  res = soc_petra_srd_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_access_db_init()",0,0);
}


/* Indirect module init }*/
STATIC uint32
  soc_petra_mgmt_module_init(void)
{
  uint32
    res;

  SOC_SAND_INTERRUPT_INIT_DEFS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_MODULE_INIT);

  /*Stop the interrupts and task switches while accessing global resources*/
  SOC_SAND_INTERRUPTS_STOP;

  res = soc_petra_access_db_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_petra_chip_defines_init(
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_sw_db_init(
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_interrupt_module_initialize(
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_mgmt_indirect_module_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /* Add list of SOC_PETRA errors to all-system errors pool                                                  */
  res = soc_petra_errors_add();
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /* Now add list of SOC_PETRA procedure descriptors to all-system pool.                                     */
  res = soc_petra_procedure_desc_add() ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_module_init()",0,0);
}

uint16
  soc_petra_mgmt_interrupts_unmask(
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

  res = soc_petra_interrupt_unmask_all(
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
  soc_petra_mgmt_is_interrupt_bit_autoclear(
    uint32     bit
  )
{
  return soc_petra_interrupt_is_bit_autoclear(bit);
}

uint16
  soc_petra_mgmt_is_interrupt_masked_get(
    int     unit
  )
{
  return soc_petra_interrupt_is_masked_get(unit);
}

uint16
  soc_petra_mgmt_interrupts_mask_get(
    int     unit,
    uint32      *bit_stream
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;

  res = soc_petra_interrupt_mask_get(
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
  soc_petra_mgmt_interrupt_monitored_cause_mask(
    int     unit,
    uint32     cause
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;

  res = soc_petra_interrupt_monitored_cause_mask_bit_set(
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
  soc_petra_mgmt_device_init(
    SOC_SAND_IN  int  unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_DEVICE_INIT);

  res = soc_petra_sw_db_device_init(
          unit,
          SOC_PETRA_DEV_VER_A
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_device_init()",0,0);
}

STATIC uint32
  soc_petra_mgmt_device_close(
    SOC_SAND_IN  int  unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DEVICE_CLOSE);

  res = soc_petra_sw_db_device_close(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_device_close()",0,0);
}



/*********************************************************************
*     This procedure registers a new device to be taken care
*     of by this device driver. Physical device must be
*     accessible by CPU when this call is made..
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_register_device_unsafe(
             uint32                  *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR reset_device_ptr,
    SOC_SAND_OUT int                 *unit_ptr
  )
{
  uint32
    res;
  int unit = 0;
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

  res = soc_petra_mgmt_module_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  regs = soc_petra_regs();

  /* In order for indirect access to function properly we have to                                         */
  /* point it to the right addresses.                                                                     */
  soc_petra_mgmt_indirect_module_get(&(indirect_module));

  soc_sand_clear_SAND_DEV_VER_INFO(&ver_info);
  chip_type_fld.lsb = SOC_PETRA_MGMT_CHIP_TYPE_FLD_LSB;
  chip_type_fld.msb = SOC_PETRA_MGMT_CHIP_TYPE_FLD_MSB;
  dbg_ver_fld.lsb   = SOC_PETRA_MGMT_DBG_VER_FLD_LSB;
  dbg_ver_fld.msb   = SOC_PETRA_MGMT_DBG_VER_FLD_MSB;
  chip_ver_fld.lsb  = SOC_PETRA_MGMT_CHIP_VER_FLD_LSB;
  chip_ver_fld.msb  = SOC_PETRA_MGMT_CHIP_VER_FLD_MSB;

  ver_info.ver_reg_offset = SOC_PETRA_MGMT_VER_REG_BASE;
  ver_info.logic_chip_type= SOC_SAND_DEV_PETRA;
  ver_info.chip_type      = SOC_PETRA_EXPECTED_CHIP_TYPE;

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
           reg_val, (uint32 *)base_address, SOC_PA_REG_DB_ACC(regs->eci.test_reg.addr.base), SOC_SAND_REG_SIZE_BYTES
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

    res = soc_sand_physical_read_from_chip(
           reg_val, base_address, SOC_PA_REG_DB_ACC(regs->eci.test_reg.addr.base), SOC_SAND_REG_SIZE_BYTES
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
          SOC_PA_TOTAL_SIZE_OF_REGS,
          soc_petra_mgmt_interrupts_unmask,
          soc_petra_mgmt_is_interrupt_bit_autoclear,
          soc_petra_mgmt_is_interrupt_masked_get,
          soc_petra_mgmt_interrupts_mask_get,
          soc_petra_mgmt_interrupt_monitored_cause_mask,
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

  res = soc_petra_mgmt_device_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  soc_sand_indirect_set_nof_repetitions_unsafe(unit, 0);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_register_device_unsafe()",0,0);
}

/*********************************************************************
*     Undo soc_petra_register_device()
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_unregister_device_unsafe(
    SOC_SAND_IN  int        unit
  )
{
  uint32
    res = SOC_SAND_OK;
  int32
    soc_sand_ret;
  SOC_SAND_RET
    ret;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_UNREGISTER_DEVICE_UNSAFE);

#ifdef PETRA_PP
  if (soc_petra_sw_db_pp_enable_get(unit))
  {
    res = soc_petra_pp_mgmt_device_close_unsafe(
            unit
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
#endif
  res = soc_petra_mgmt_device_close(
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_unregister_device_unsafe()",0,0);
}


STATIC uint32
  soc_petra_mgmt_operation_mode_device_type_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_REVISION_SUB_TYPE_ID  device_sub_type
  )
{
  uint32
    reg_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_OPERATION_MODE_DEVICE_TYPE_SET);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->eci.version_reg.chip_type, reg_val, 10, exit);

  switch(reg_val) {
  case SOC_PETRA_EXPECTED_CHIP_TYPE:
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CHIP_TYPE_UNKNOWN_ERR, 20, exit);
  }

  soc_petra_sw_db_revision_subtype_set(
    unit,
    device_sub_type
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_operation_mode_device_type_set()",0,0);
}

STATIC uint32
  soc_petra_mgmt_operation_mode_device_type_get(
    SOC_SAND_IN   int                   unit,
    SOC_SAND_OUT  SOC_PETRA_REVISION_SUB_TYPE_ID  *device_sub_type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_OPERATION_MODE_DEVICE_TYPE_GET);

  *device_sub_type = soc_petra_sw_db_revision_subtype_get(unit);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_operation_mode_device_type_get()",0,0);
}


/*********************************************************************
*     Set soc_petra device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_operation_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_OPERATION_MODE *op_mode
  )
{
  uint32
    res;
  SOC_PETRA_COMBO_QRTT
    combo_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_OPERATION_MODE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  res = soc_petra_mgmt_operation_mode_device_type_set(
          unit,
          op_mode->device_sub_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

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

  if (
      (SOC_PETRA_REV_A1_OR_BELOW) &&
      (op_mode->is_a1_or_below_in_system == FALSE)
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OP_MODE_A1_OR_BELOW_LOCAL_MISMATCH_ERR, 35, exit);
  }

  soc_petra_sw_db_is_a1_or_below_in_system_set(
      unit,
      op_mode->is_a1_or_below_in_system
    );

  soc_petra_sw_db_is_fe200_fabric_set(
      unit,
      op_mode->is_fe200_fabric
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

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_operation_mode_set_unsafe()",0,0);
}

uint32
  soc_petra_mgmt_ref_clk_verify(
    SOC_SAND_IN  uint32 ref_clk
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* The macro SOC_PETRA_MGMT_IS_SRD_REF_CLK_ENUM may be used for signed varible also */
  /* coverity[unsigned_compare : FALSE] */
  if (SOC_PETRA_MGMT_IS_SRD_REF_CLK_ENUM(ref_clk))
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(ref_clk, SOC_PETRA_MGMT_NOF_SRD_REF_CLKS - 1, SOC_PETRA_SRD_REF_CLK_OF_RANGE_ERR, 10, exit);
  }
  else
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(ref_clk, SOC_PETRA_MGMT_SRD_REF_CLK_BY_NUM_KBPS_MIN, SOC_PETRA_MGMT_SRD_REF_CLK_BY_NUM_KBPS_MAX, SOC_PETRA_SRD_CMU_REF_CLK_OUT_OF_RANGE_ERR, 20, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_ref_clk_verify()",0,0);
}

/*********************************************************************
*     Set soc_petra device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_operation_mode_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_OPERATION_MODE      *op_mode
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_OPERATION_MODE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(op_mode);
  SOC_SAND_MAGIC_NUM_VERIFY(op_mode);

  SOC_SAND_ERR_IF_ABOVE_MAX(
      op_mode->device_sub_type, SOC_PETRA_REVISION_NOF_TYPES-1,
      SOC_PETRA_REVISION_SUB_TYPE_OUT_OF_LIMIT_ERR, 10, exit
  );

  if ((!SOC_PETRA_MGMT_IS_REVISION_PP(op_mode->device_sub_type)) &&
       op_mode->pp_enable)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PP_ENABLE_NOT_SUPPORTED_ERR, 20, exit)
  }

  if (op_mode->is_fe200_fabric)
  {
    if (op_mode->fabric_cell_format.variable_cell_size_enable)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CELL_VAR_SIZE_IN_FE200_SYSTEM_ERR, 30, exit);
    }
  }

  if (op_mode->is_fap20_in_system)
  {
    if (op_mode->fabric_cell_format.variable_cell_size_enable)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CELL_VARIABLE_IN_FAP20_SYSTEM_ERR, 40, exit);
    }
  }

  if (op_mode->is_fap21_in_system)
  {
    if (op_mode->fabric_cell_format.variable_cell_size_enable)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CELL_VARIABLE_IN_FAP20_SYSTEM_ERR, 50, exit);
    }
  }

  res = soc_petra_mgmt_ref_clk_verify(op_mode->ref_clocks_conf.nif_ref_clk);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_mgmt_ref_clk_verify(op_mode->ref_clocks_conf.combo_ref_clk);
  SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

  res = soc_petra_mgmt_ref_clk_verify(op_mode->ref_clocks_conf.fabric_ref_clk);
  SOC_SAND_CHECK_FUNC_RESULT(res, 64, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_operation_mode_verify()",0,0);
}

/*********************************************************************
*     Set soc_petra device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_operation_mode_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MGMT_OPERATION_MODE      *op_mode
  )
{
  uint32
    res;
  SOC_PETRA_COMBO_QRTT
    combo_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_OPERATION_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  res = soc_petra_mgmt_operation_mode_device_type_get(
          unit,
          &(op_mode->device_sub_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

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

  op_mode->is_a1_or_below_in_system =
    soc_petra_sw_db_is_a1_or_below_in_system_get(
        unit
      );

  op_mode->is_fe200_fabric =
    soc_petra_sw_db_is_fe200_fabric_get(
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

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_operation_mode_get_unsafe()",0,0);
}

/*********************************************************************
*     Bytes-worth of a single credit. It should be configured
*     the same in all the FAPs in the systems, and should be
*     set before programming the scheduler.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_credit_worth_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  credit_worth
  )
{
  uint32
    fld_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_CREDIT_WORTH_SET_UNSAFE);

  regs = soc_petra_regs();

  fld_val = credit_worth;
  SOC_PETRA_FLD_SET(regs->ips.ips_credit_config_reg.credit_value, fld_val, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_credit_worth_set_unsafe()",0,0);
}

/*********************************************************************
*     Bytes-worth of a single credit. It should be configured
*     the same in all the FAPs in the systems, and should be
*     set before programming the scheduler.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_credit_worth_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  credit_worth
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_CREDIT_WORTH_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    credit_worth, SOC_PETRA_CREDIT_SIZE_BYTES_MIN, SOC_PETRA_CREDIT_SIZE_BYTES_MAX,
    SOC_PETRA_CREDIT_SIZE_OUT_OF_RANGE_ERR, 10, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_credit_worth_verify()",0,0);
}

/*********************************************************************
*     Bytes-worth of a single credit. It should be configured
*     the same in all the FAPs in the systems, and should be
*     set before programming the scheduler.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_credit_worth_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                  *credit_worth
  )
{
  uint32
    fld_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_CREDIT_WORTH_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(credit_worth);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->ips.ips_credit_config_reg.credit_value, fld_val, 10, exit);

  *credit_worth = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_credit_worth_get_unsafe()",0,0);
}

/*********************************************************************
*     Set the fabric system ID of the device. Must be unique
*     in the system.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_system_fap_id_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_fap_id
  )
{
  uint32
    res;
  SOC_PETRA_REGS*
    regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_SYSTEM_FAP_ID_SET_UNSAFE);

  regs = soc_petra_regs();
  SOC_PETRA_FLD_SET(regs->eci.identification_reg.pipe_id, sys_fap_id, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_system_fap_id_set_unsafe()",0,0);
}

/*********************************************************************
*     Set the fabric system ID of the device. Must be unique
*     in the system.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_system_fap_id_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_fap_id
  )
{
  uint8
    is_egr_mc_16k;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_SYSTEM_FAP_ID_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    sys_fap_id, SOC_PETRA_NOF_FAPS_IN_SYSTEM-1,
    SOC_PETRA_FAP_FABRIC_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  is_egr_mc_16k = soc_petra_sw_db_egr_mc_16k_groups_enable_get(unit);
  if (is_egr_mc_16k == TRUE)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
    sys_fap_id, SOC_PETRA_MAX_FAP_ID_IF_MC_16K_EN,
    SOC_PETRA_EGR_MC_16K_ENABLE_INCOMPATIBLE_ERR, 20, exit
  );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_system_fap_id_verify()",0,0);
}

/*********************************************************************
*     Set the fabric system ID of the device. Must be unique
*     in the system.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_system_fap_id_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                 *sys_fap_id
  )
{
  uint32
    res,
    fld_val;
  SOC_PETRA_REGS*
    regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_SYSTEM_FAP_ID_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(sys_fap_id);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->eci.identification_reg.pipe_id, fld_val, 10, exit);

  /* Get device internal field.                                                                           */
  *sys_fap_id = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_system_fap_id_get_unsafe()",0,0);
}

STATIC uint32
  soc_petra_mgmt_all_ctrl_cells_fct_disable_polling(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *all_down
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
     rtp_mask[SOC_PETRA_NOF_BIT_PER_LINK_REGS];
  uint32
    srd_lane_id,
    reg_idx,
    lnk_idx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_ALL_CTRL_CELLS_FCT_DISABLE_POLLING);

  regs = soc_petra_regs();

  *all_down = TRUE;

  res = SOC_SAND_OK; sal_memset(
          rtp_mask,
          0x0,
          sizeof(rtp_mask)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for (reg_idx = 0; reg_idx < SOC_PETRA_NOF_BIT_PER_LINK_REGS; reg_idx++)
  {
    SOC_PETRA_REG_GET(regs->rtp.link_active_mask_reg[reg_idx], rtp_mask[reg_idx], 10, exit);
  }

  for(lnk_idx = 0; lnk_idx < SOC_DPP_DEFS_GET(unit, nof_fabric_links); lnk_idx++)
  {
    srd_lane_id = soc_petra_fbr2srd_lane_id(lnk_idx);

    if (!soc_petra_is_fabric_quartet(unit, SOC_PETRA_SRD_LANE2QRTT_GLBL(srd_lane_id)))
    {
      continue;
    }

    if (!soc_sand_bitstream_test_bit(rtp_mask, lnk_idx))
    {
      *all_down = FALSE;
       /* Exit with SOC_SAND_OK */
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_OK, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_all_ctrl_cells_fct_disable_polling()",0,0);
}

STATIC uint32
  soc_petra_mgmt_all_ctrl_cells_standalone_polling(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    buffer;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_PETRA_MGMT_TRACE;
  SOC_PETRA_FLD_GET(regs->mesh_topology.fap_detect_ctrl_cells_cnt_reg.rcv_ctl1, buffer, 10, exit);

  *success = SOC_SAND_NUM2BOOL(buffer);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mngr_standalone_polling()",0,0);
}

STATIC uint32
  soc_petra_mgmt_all_ctrl_cells_standalone_failure(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_PETRA_MGMT_TRACE;
  res = soc_petra_fabric_stand_alone_fap_mode_set_unsafe(
          unit,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_all_ctrl_cells_standalone_failure()",0,0);
}

STATIC uint32
  soc_petra_mgmt_all_ctrl_cells_status_polling(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    buffer;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint8
    stand_alone;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_PETRA_MGMT_TRACE;
  res = soc_petra_fabric_stand_alone_fap_mode_get_unsafe(
          unit,
          &stand_alone
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PETRA_FLD_GET(regs->mesh_topology.mesh_status_cnt_reg.status, buffer, 10, exit);

  if ((buffer < 0x80) || (stand_alone == TRUE))
  {
    *success = TRUE;
  }
  else
  {
    *success = FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_all_ctrl_cells_status_polling()",0,0);
}

STATIC uint32
  soc_petra_mgmt_all_ctrl_cells_fct_enable_polling(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *any_up
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8
    all_down;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_ALL_CTRL_CELLS_FCT_ENABLE_POLLING);
  SOC_PETRA_MGMT_TRACE;
  res = soc_petra_mgmt_all_ctrl_cells_fct_disable_polling(
          unit,
          &all_down
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *any_up = SOC_SAND_NUM2BOOL_INVERSE(all_down);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_all_ctrl_cells_fct_enable_polling()",0,0);
}

STATIC uint32
  soc_petra_mgmt_ctrl_cells_counter_clear(
    SOC_SAND_IN  int                  unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    reg_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_CTRL_CELLS_COUNTER_CLEAR);

  regs = soc_petra_regs();

  SOC_PETRA_REG_GET(regs->mesh_topology.fap_detect_ctrl_cells_cnt_reg, reg_val, 10, exit);
  SOC_PETRA_REG_GET(regs->mesh_topology.mesh_status_cnt_reg, reg_val, 15, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_ctrl_cells_counter_clear()",0,0);
}
STATIC uint32
  soc_petra_mgmt_all_ctrl_cells_enable_write(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_CTRL_CELL_DATA      *data,
    SOC_SAND_IN  uint8                  silent
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    reg_val;
  uint8
    success = TRUE,
    is_low_sim_active;
  uint32
    wait_iter = 0;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_ALL_CTRL_CELLS_ENABLE_WRITE);

  SOC_SAND_CHECK_NULL_INPUT(data);

  regs = soc_petra_regs();

#ifndef SAND_LOW_LEVEL_SIMULATION
  is_low_sim_active = FALSE;
#else
  is_low_sim_active = soc_sand_low_is_sim_active_get();
#endif

  /* Write the data */
  if (data->field)
  {
    if (data->field->addr.base == (SOC_PETRA_REG_DB_ACC(regs->fdt.fdt_enabler_reg.addr.base)))
    {
      SOC_PETRA_REG_GET(regs->fdt.fdt_enabler_reg, reg_val, 10, exit);

      SOC_PETRA_REGS_FDT_ENABLER_READ_ADJUST(reg_val);

      res = soc_petra_field_from_reg_set(
              &(data->val),
              data->field,
              &(reg_val)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

      SOC_PETRA_REG_SET(regs->fdt.fdt_enabler_reg, reg_val, 20, exit);
    }
    else
    {
      SOC_PETRA_IMPLICIT_FLD_ISET(*(data->field), data->val, data->instance_id, 25, exit);
    }
  }

  if (data->polling_func)
  {
    /* If there polling field exists, refer to delay as the expected value */
#if SOC_PETRA_MGMT_DBG_ON
  if (data->polling_func)
  {
    soc_sand_os_printf("!!DBG: Polling on ");
  }
#endif
    do
    {
      if (is_low_sim_active)
      {
        success = data->failure_func ? FALSE : TRUE;
      }
      else
      {
        res = data->polling_func(
                unit,
                &success
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
      }

      if (success)
      {
        /* Exit with SOC_SAND_OK */
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_OK, 30, exit);
      }
      sal_msleep(SOC_PETRA_MGMT_INIT_CTRL_CELLS_TIMER_DELAY_MSEC);

    } while ((wait_iter++) < data->delay_or_polling_iters);
  }
  else
  {
    /* If there is no polling field, refer to delay as delay */
    if (data->delay_or_polling_iters)
    {
      sal_msleep(data->delay_or_polling_iters);
    }
  }

  if (!success)
  {
    if (data->failure_func)
    {
      res = data->failure_func(
              unit
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

    if (data->err_on_fail)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 50, exit);
    }
  }

exit:
#if SOC_PETRA_MGMT_DBG_ON
  if (data->polling_func)
  {
    soc_sand_os_printf(
      "!!DBG: Polling Time: %u[ms], Success: %s\n\r",
      wait_iter*SOC_PETRA_MGMT_INIT_CTRL_CELLS_TIMER_DELAY_MSEC,
      success?"TRUE":"FALSE"
    );
    if (wait_iter >= data->delay_or_polling_iters)
    {
      soc_sand_os_printf(
      "!!DBG: Exceeded maximal polling time %u[ms] (%u * %u iterations)\n\r",
      data->delay_or_polling_iters*SOC_PETRA_MGMT_INIT_CTRL_CELLS_TIMER_DELAY_MSEC,
      data->delay_or_polling_iters,
      SOC_PETRA_MGMT_INIT_CTRL_CELLS_TIMER_DELAY_MSEC
    );
    }
  }
#endif
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_all_ctrl_cells_enable_write()",0,0);
}

uint32
  soc_petra_mgmt_all_ctrl_cells_enable_get_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_OUT  uint8  *enable
  )
{
  uint32
    res,
    fld_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_ALL_CTRL_CELLS_ENABLE_GET_UNSAFE);

  regs = soc_petra_regs();
  SOC_PETRA_FLD_GET(regs->mesh_topology.mesh_topology_reg.trig, fld_val, 10, exit);
  *enable = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_all_ctrl_cells_enable_get_unsafe()",0,0);
}

/*********************************************************************
*     Enable / Disable the device from receiving and
*     transmitting control cells.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_all_ctrl_cells_enable_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8  enable
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_CTRL_CELL_DATA
    conf[SOC_PETRA_MGMT_CTRL_CELLS_MAX_NOF_CONFS];
  uint32
    conf_idx = 0,
    nof_confs = 0,
    write_idx = 0,
    inst_idx = 0;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_ALL_CTRL_CELLS_ENABLE_SET_UNSAFE);

  regs = soc_petra_regs();

  res = SOC_SAND_OK; sal_memset(
          conf,
          0x0,
          SOC_PETRA_MGMT_CTRL_CELLS_MAX_NOF_CONFS * sizeof(SOC_PETRA_CTRL_CELL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for (inst_idx = 0; inst_idx < SOC_DPP_DEFS_GET(unit, nof_fabric_macs); inst_idx++)
  {
    conf[conf_idx].field = (enable == TRUE ? SOC_PETRA_REG_DB_ACC_REF(regs->fabric_mac.leaky_bucket_control_reg.bkt_fill_rate):NULL);
    conf[conf_idx].val = soc_sand_link_fap_bkt_fill_rate_get();
    conf[conf_idx].instance_id = inst_idx;
    conf[conf_idx].polling_func = NULL;
    conf[conf_idx].failure_func = NULL;
    conf[conf_idx].err_on_fail = FALSE;
    conf[conf_idx++].delay_or_polling_iters = 0;

    conf[conf_idx].field = (enable == TRUE ? SOC_PETRA_REG_DB_ACC_REF(regs->fabric_mac.leaky_bucket_control_reg.bkt_link_dn_th):NULL);
    conf[conf_idx].val = soc_sand_link_fap_dn_link_th_get();
    conf[conf_idx].instance_id = inst_idx;
    conf[conf_idx].polling_func = NULL;
    conf[conf_idx].failure_func = NULL;
    conf[conf_idx].err_on_fail = FALSE;
    conf[conf_idx++].delay_or_polling_iters = 0;

    conf[conf_idx].field = (enable == TRUE ? SOC_PETRA_REG_DB_ACC_REF(regs->fabric_mac.leaky_bucket_control_reg.bkt_link_up_th):NULL);
    conf[conf_idx].val = soc_sand_link_fap_up_link_th_get();
    conf[conf_idx].instance_id = inst_idx;
    conf[conf_idx].polling_func = NULL;
    conf[conf_idx].failure_func = NULL;
    conf[conf_idx].err_on_fail = FALSE;
    conf[conf_idx++].delay_or_polling_iters = 0;
  }

  conf[conf_idx].field = SOC_PETRA_REG_DB_ACC_REF(regs->rtp.rtp_enable_reg.rmgr);
  conf[conf_idx].val = (enable == TRUE ? soc_sand_link_fap_reachability_rate_get(soc_petra_chip_ticks_per_sec_get(unit), SOC_DPP_DEFS_GET(unit, nof_fabric_links), SOC_PETRA_NOF_RMGR_UNITS_PER_LINK) : 0x0);
  conf[conf_idx].instance_id = SOC_PETRA_DEFAULT_INSTANCE;
  conf[conf_idx].polling_func = NULL;
  conf[conf_idx].failure_func = NULL;
  conf[conf_idx].err_on_fail = FALSE;
  conf[conf_idx++].delay_or_polling_iters = 0;

  conf[conf_idx].field = SOC_PETRA_REG_DB_ACC_REF(regs->rtp.rtp_enable_reg.rtpwp);
  conf[conf_idx].val = (enable == TRUE ? soc_sand_link_fap_reachability_watchdog_period_get(soc_petra_chip_ticks_per_sec_get(unit), SOC_DPP_DEFS_GET(unit, nof_fabric_links)) : 0x0);
  conf[conf_idx].instance_id = SOC_PETRA_DEFAULT_INSTANCE;
  conf[conf_idx].polling_func = NULL;
  conf[conf_idx].failure_func = NULL;
  conf[conf_idx].err_on_fail = FALSE;
  conf[conf_idx++].delay_or_polling_iters = 0;

  conf[conf_idx].field = SOC_PETRA_REG_DB_ACC_REF(regs->rtp.rtp_enable_reg.rtp_en_msk);
  conf[conf_idx].val = (enable == TRUE ? 0x1 : 0x0);
  conf[conf_idx].instance_id = SOC_PETRA_DEFAULT_INSTANCE;
  conf[conf_idx].polling_func = NULL;
  conf[conf_idx].failure_func = NULL;
  conf[conf_idx].err_on_fail = FALSE;
  conf[conf_idx++].delay_or_polling_iters = 0;

  conf[conf_idx].field = SOC_PETRA_REG_DB_ACC_REF(regs->rtp.rtp_enable_reg.rtp_up_en);
  conf[conf_idx].val = (enable == TRUE ? 0x1 : 0x0);
  conf[conf_idx].instance_id = SOC_PETRA_DEFAULT_INSTANCE;
  conf[conf_idx].polling_func = NULL;
  conf[conf_idx].failure_func = NULL;
  conf[conf_idx].err_on_fail = FALSE;
  conf[conf_idx++].delay_or_polling_iters = 0;

  conf[conf_idx].field = SOC_PETRA_REG_DB_ACC_REF(regs->fct.fct_enabler_reg.stat_credit_and_rch_ctrl);
  conf[conf_idx].val = (enable == TRUE ? 0x3 : 0x7);
  conf[conf_idx].instance_id = SOC_PETRA_DEFAULT_INSTANCE;
  conf[conf_idx].polling_func = (enable == TRUE ? soc_petra_mgmt_all_ctrl_cells_fct_enable_polling : NULL);
  conf[conf_idx].failure_func = NULL;
  conf[conf_idx].err_on_fail = FALSE; /* Do not throw error in case polling failed */
  conf[conf_idx++].delay_or_polling_iters = (enable == TRUE ? SOC_PETRA_MGMT_INIT_CTRL_RCH_STATUS_ITERATIONS : 0);

  conf[conf_idx].field = SOC_PETRA_REG_DB_ACC_REF(regs->mesh_topology.mesh_topology_reg.trig);
  conf[conf_idx].val = (enable == TRUE ? 0x1 : 0x0);
  conf[conf_idx].instance_id = SOC_PETRA_DEFAULT_INSTANCE;
  conf[conf_idx].polling_func = NULL;
  conf[conf_idx].failure_func = NULL;
  conf[conf_idx].err_on_fail = FALSE;
  conf[conf_idx++].delay_or_polling_iters = 0;

  conf[conf_idx].field = SOC_PETRA_REG_DB_ACC_REF(regs->mesh_topology.init_reg.config1);
  conf[conf_idx].val = (enable == TRUE ? 0xd : 0x0);
  conf[conf_idx].instance_id = SOC_PETRA_DEFAULT_INSTANCE;
  conf[conf_idx].polling_func = (enable == TRUE ? soc_petra_mgmt_all_ctrl_cells_standalone_polling : NULL);
  conf[conf_idx].failure_func = (enable == TRUE ? soc_petra_mgmt_all_ctrl_cells_standalone_failure : NULL);
  conf[conf_idx].err_on_fail = FALSE; /* Do not throw error in case polling failed */
  conf[conf_idx++].delay_or_polling_iters = (enable == TRUE ? SOC_PETRA_MGMT_INIT_STANDALONE_ITERATIONS : 0);

  conf[conf_idx].field = NULL;
  conf[conf_idx].val = 0;
  conf[conf_idx].instance_id = SOC_PETRA_DEFAULT_INSTANCE;
  conf[conf_idx].polling_func = (enable == TRUE ? soc_petra_mgmt_all_ctrl_cells_status_polling : NULL);
  conf[conf_idx].failure_func = NULL;
  conf[conf_idx].err_on_fail = TRUE; /* Throw error in case polling failed */
  conf[conf_idx++].delay_or_polling_iters = (enable == TRUE ? SOC_PETRA_MGMT_INIT_CTRL_CELLS_TIMER_ITERATIONS : 0);

  conf[conf_idx].field = SOC_PETRA_REG_DB_ACC_REF(regs->mesh_topology.mesh_topology_reg.reserved);
  conf[conf_idx].val = (enable == TRUE ? 0x7 : 0x0);
  conf[conf_idx].instance_id = SOC_PETRA_DEFAULT_INSTANCE;
  conf[conf_idx].polling_func = NULL;
  conf[conf_idx].failure_func = NULL;
  conf[conf_idx].err_on_fail = FALSE;
  conf[conf_idx++].delay_or_polling_iters = 0;

  conf[conf_idx].field = SOC_PETRA_REG_DB_ACC_REF(regs->sch.dvs_config1_reg.force_pause);
  conf[conf_idx].val = (enable == TRUE ? 0x0 : 0x1);
  conf[conf_idx].instance_id = SOC_PETRA_DEFAULT_INSTANCE;
  conf[conf_idx].polling_func = NULL;
  conf[conf_idx].failure_func = NULL;
  conf[conf_idx].err_on_fail = FALSE;
  conf[conf_idx++].delay_or_polling_iters = 0;

  conf[conf_idx].field = SOC_PETRA_REG_DB_ACC_REF(regs->fct.fct_enabler_reg.stat_credit_and_rch_ctrl);
  conf[conf_idx].val = (enable == TRUE)?0x0 : 0x7;
  conf[conf_idx].instance_id = SOC_PETRA_DEFAULT_INSTANCE;
  conf[conf_idx].polling_func = (enable == TRUE ? NULL : soc_petra_mgmt_all_ctrl_cells_fct_disable_polling);
  conf[conf_idx].failure_func = NULL;
  conf[conf_idx].err_on_fail = FALSE;
  conf[conf_idx++].delay_or_polling_iters = (enable == TRUE ? 0 : SOC_PETRA_MGMT_INIT_CTRL_CELLS_TIMER_ITERATIONS);

  for (inst_idx = 0; inst_idx < SOC_DPP_DEFS_GET(unit, nof_fabric_macs); inst_idx++)
  {
    conf[conf_idx].field = SOC_PETRA_REG_DB_ACC_REF(regs->fabric_mac.enablers_reg.enable_serial_link);
    conf[conf_idx].val = (enable == TRUE ? 0x0 : 0x1);
    conf[conf_idx].instance_id = inst_idx;
    conf[conf_idx].polling_func = NULL;
    conf[conf_idx].failure_func = NULL;
    conf[conf_idx].err_on_fail = FALSE;
    conf[conf_idx++].delay_or_polling_iters = (((enable == TRUE) && (inst_idx == (SOC_DPP_DEFS_GET(unit, nof_fabric_macs) - 1))) || ((enable == FALSE) && (inst_idx == 0)) ? 16 : 0);
  }

  conf[conf_idx].field = SOC_PETRA_REG_DB_ACC_REF(regs->fdt.fdt_enabler_reg.discard_dll_pkts);
  conf[conf_idx].val = (enable == TRUE ? 0x0 : 0x1);
  conf[conf_idx].instance_id = SOC_PETRA_DEFAULT_INSTANCE;
  conf[conf_idx].polling_func = NULL;
  conf[conf_idx].failure_func = NULL;
  conf[conf_idx].err_on_fail = FALSE;
  conf[conf_idx++].delay_or_polling_iters = 0;

  nof_confs = conf_idx;

  res = soc_petra_mgmt_ctrl_cells_counter_clear(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

#if SOC_PETRA_MGMT_DBG_ON
  soc_sand_os_printf("\n\r!!DBG ON, print write-accesses (Addr,Mask,Value) and function calls\n\r");
  if (enable){soc_sand_set_print_when_writing(1, 1, 0);}
#endif
  for (conf_idx = 0; conf_idx < nof_confs; ++conf_idx)
  {
    write_idx = (enable == TRUE ? conf_idx : nof_confs - conf_idx - 1);

    res = soc_petra_mgmt_all_ctrl_cells_enable_write(
            unit,
            &(conf[write_idx]),
            TRUE
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
#if SOC_PETRA_MGMT_DBG_ON
  if (enable){soc_sand_set_print_when_writing(0, 0, 0);}
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_all_ctrl_cells_enable_set_unsafe()",conf_idx,SOC_SAND_BOOL2NUM(enable));
}

/*********************************************************************
*     Enable / Disable the device from receiving and
*     transmitting control cells.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_all_ctrl_cells_enable_verify(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8  enable
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_ALL_CTRL_CELLS_ENABLE_VERIFY);

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_all_ctrl_cells_enable_verify()",0,0);
}

/*********************************************************************
*     Enable / Disable the device from receiving and
*     transmitting traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_mgmt_enable_traffic_set_unsafe(
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
   *  For enabling the traffic, the action is the opposite of the described
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_mgmt_enable_traffic_set_unsafe()",0,0);
}

/*********************************************************************
*     Enable / Disable the device from receiving and
*     transmitting traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_enable_traffic_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8  enable
  )
{
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_ENABLE_TRAFFIC_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(mgmt_enable_traffic_set_unsafe,(unit, enable));

  /*
   *  Apply initialization sequence fixes at the end of the initialization sequence
   */
  res = soc_petra_mgmt_init_sequence_fixes_apply_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_enable_traffic_set_unsafe()",0,0);
}


/*********************************************************************
*     Enable / Disable the device from receiving and
*     transmitting traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_enable_traffic_verify(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8  enable
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_ENABLE_TRAFFIC_VERIFY);

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_enable_traffic_verify()",0,0);
}

/*********************************************************************
*     Enable / Disable the device from receiving and
*     transmitting traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_enable_traffic_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT uint8  *enable
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    enable_val;
  uint8
    enable_curr = FALSE,
    enable_all = TRUE;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_ENABLE_TRAFFIC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(enable);

  regs = soc_petra_regs();

  /*
   *  Check the ingress data path at the ingress -
   *  I.e., if disabled - no packet will enter the ingress DRAM and fabric.
   *  IDR, IRR, IRE
   */
  SOC_PETRA_FLD_GET(regs->idr.dynamic_configuration_reg_cmn.enable_data_path, enable_val, 2, exit);
  enable_curr = SOC_SAND_NUM2BOOL(enable_val);
  enable_all = (enable_all && enable_curr);

  SOC_PETRA_FLD_GET(regs->irr.dynamic_configuration_reg.enable_data_path_idr, enable_val, 4, exit);
  enable_curr = SOC_SAND_NUM2BOOL(enable_val);
  enable_all = (enable_all && enable_curr);

  SOC_PETRA_FLD_GET( regs->irr.dynamic_configuration_reg.enable_data_path_iqm, enable_val, 6, exit);
  enable_curr = SOC_SAND_NUM2BOOL(enable_val);
  enable_all = (enable_all && enable_curr);

  SOC_PETRA_FLD_GET(regs->ire.dynamic_configuration_reg.enable_data_path, enable_val, 8, exit);
  enable_curr = SOC_SAND_NUM2BOOL(enable_val);
  enable_all = (enable_all && enable_curr);

  /*
   * Check fabric receive.
   */
  SOC_PETRA_REG_GET(regs->fdr.fdr_enablers_reg, enable_val, 20, exit);
  enable_curr = SOC_SAND_NUM2BOOL(enable_val);
  enable_all = (enable_all && enable_curr);

  /*
   * Check SMP messages are enabled (scheduler).
   */
  SOC_PETRA_FLD_GET(regs->sch.scheduler_configuration_reg.smpdisable_fabric, enable_val, 70, exit);
  enable_curr = !(SOC_SAND_NUM2BOOL(enable_val));
  enable_all = (enable_all && enable_curr);

 *enable = enable_all;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_enable_traffic_get_unsafe()",0,0);
}

/*********************************************************************
*     Set the boundaries (minimal/maximal allowed size) for
*     the expected packets. The limitation can be performed
*     based on the packet size before or after the ingress
*     editing (external and internal configuration mode,
*     accordingly). Packets outside the specified range are
*     dropped.
*     Details: in the H file. (search for prototype)
*********************************************************************/

STATIC
  uint32
    soc_pa_mgmt_pckt_size_range_set_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
      SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE      *size_range
    )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_MGMT_PCKT_SIZE_RANGE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(size_range);

  regs = soc_petra_regs();

  switch(conf_mode_ndx) {
  case SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_INTERN:
    SOC_PA_FLD_SET(regs->idr.packet_sizes_reg.min_packet_size, (size_range->min)-SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_INTERNAL, 10, exit);
    /*
     * The actual test is: drop if actual_pckt_size-1 > configured_max_size.
     */
    SOC_PA_FLD_SET(regs->idr.packet_sizes_reg.max_packet_size, (size_range->max)-SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_INTERNAL, 20, exit);
    SOC_PA_FLD_SET(regs->egq.fabric_minimum_packet_size_reg.fabric_min_pkt_size, size_range->min, 30, exit);
    break;
  case SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_EXTERN:
    if (size_range->min == SOC_PETRA_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT)
    {
      SOC_PA_FLD_SET(regs->idr.dynamic_configuration_reg.check_min_org_size, 0x0, 40, exit);
    }
    else
    {
      SOC_PA_FLD_SET(regs->idr.dynamic_configuration_reg.check_min_org_size, 0x1, 45, exit);
      SOC_PA_FLD_SET(regs->idr.original_packet_sizes_reg.min_org_packet_size, (size_range->min)-SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_EXTERNAL, 50, exit);
    }

    if (size_range->max == SOC_PETRA_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT)
    {
      SOC_PA_FLD_SET(regs->idr.dynamic_configuration_reg.check_max_org_size, 0x0, 60, exit);
    }
    else
    {
      SOC_PA_FLD_SET(regs->idr.dynamic_configuration_reg.check_max_org_size, 0x1, 65, exit);
      /*
       * The actual test is: drop if actual_pckt_size-1 > configured_max_size.
       */
      SOC_PA_FLD_SET(regs->idr.original_packet_sizes_reg.max_org_packet_size, (size_range->max)-SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_EXTERNAL, 60, exit);
    }

    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_OUT_OF_RANGE_ERR, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_mgmt_pckt_size_range_set_unsafe()",0,0);
}

uint32
  soc_petra_mgmt_pckt_size_range_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE      *size_range
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_PCKT_SIZE_RANGE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(size_range);

  SOC_PETRA_DIFF_DEVICE_CALL(mgmt_pckt_size_range_set_unsafe,(unit, conf_mode_ndx, size_range));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_pckt_size_range_set_unsafe()",0,0);
}

/*********************************************************************
*     Set the boundaries (minimal/maximal allowed size) for
*     the expected packets. The limitation can be performed
*     based on the packet size before or after the ingress
*     editing (external and internal configuration mode,
*     accordingly). Packets outside the specified range are
*     dropped.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_mgmt_pckt_size_range_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE      *size_range
  )
{
  uint8
    is_var_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_PCKT_SIZE_RANGE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(size_range);
  SOC_SAND_MAGIC_NUM_VERIFY(size_range);

  is_var_size = soc_petra_sw_db_is_fabric_variable_cell_size_get(unit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    conf_mode_ndx, SOC_PETRA_MGMT_NOF_PCKT_SIZE_CONF_MODES,
    SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_OUT_OF_RANGE_ERR, 5, exit
  );

  if (is_var_size == TRUE)
  {
    if (
        !(
          (conf_mode_ndx == SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_EXTERN) &&
          (size_range->min == SOC_PETRA_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT)
         )
       )
    {
      SOC_SAND_ERR_IF_BELOW_MIN(
        size_range->min, SOC_PETRA_MGMT_PCKT_SIZE_BYTES_VSC_MIN,
        SOC_PETRA_PCKT_SIZE_VSC_BELOW_MIN_ERR, 10, exit
      );
    }

    if (
        !(
          (conf_mode_ndx == SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_EXTERN) &&
          (size_range->max == SOC_PETRA_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT)
         )
       )
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(
        size_range->max, SOC_PETRA_MGMT_PCKT_SIZE_BYTES_VSC_MAX,
        SOC_PETRA_PCKT_SIZE_VSC_ABOVE_MAX_ERR, 20, exit
      );
    }
  }
  else /* is_var_size == FALSE */
  {
    if (
        !(
          (conf_mode_ndx == SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_EXTERN) &&
          (size_range->min == SOC_PETRA_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT)
         )
       )
    {
      SOC_SAND_ERR_IF_BELOW_MIN(
        size_range->min, SOC_PETRA_MGMT_PCKT_SIZE_BYTES_FSC_MIN,
        SOC_PETRA_PCKT_SIZE_FSC_BELOW_MIN_ERR, 30, exit
      );
    }

    if (
        !(
          (conf_mode_ndx == SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_EXTERN) &&
          (size_range->max == SOC_PETRA_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT)
         )
       )
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(
        size_range->max, SOC_PETRA_MGMT_PCKT_SIZE_BYTES_FSC_MAX,
        SOC_PETRA_PCKT_SIZE_FSC_ABOVE_MAX_ERR, 40, exit
      );
    }
  }

  if (
      (size_range->max < size_range->min) &&
      !(
          (conf_mode_ndx == SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_EXTERN) &&
          (size_range->max == SOC_PETRA_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT)
       )
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_PCKT_SIZE_MIN_EXCEEDS_MAX_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_pckt_size_range_verify()", size_range->min, size_range->max);


}

/*********************************************************************
*     Set the boundaries (minimal/maximal allowed size) for
*     the expected packets. The limitation can be performed
*     based on the packet size before or after the ingress
*     editing (external and internal configuration mode,
*     accordingly). Packets outside the specified range are
*     dropped.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_pa_mgmt_pckt_size_range_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT SOC_PETRA_MGMT_PCKT_SIZE      *size_range
  )
{
  uint32
    idr_min,
    idr_max,
    egq_min,
    fld_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_MGMT_PCKT_SIZE_RANGE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(size_range);

  regs = soc_petra_regs();

  switch(conf_mode_ndx) {
  case SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_INTERN:
    SOC_PA_FLD_GET(regs->idr.packet_sizes_reg.min_packet_size, idr_min, 10, exit);
    /*
     * The actual test is: drop if actual_pckt_size-1 > configured_max_size.
     */
    SOC_PA_FLD_GET(regs->idr.packet_sizes_reg.max_packet_size, idr_max, 20, exit);

    SOC_PA_FLD_GET(regs->egq.fabric_minimum_packet_size_reg.fabric_min_pkt_size, egq_min, 30, exit);

    if ((idr_min + SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_INTERNAL) != egq_min)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MIN_PCKT_SIZE_INCONSISTENT_ERR, 40, exit);
    }

    size_range->min = idr_min + SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_INTERNAL;
    size_range->max = idr_max + SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_INTERNAL;

    break;
  case SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_EXTERN:

    soc_petra_PETRA_MGMT_PCKT_SIZE_clear(size_range);

    SOC_PA_FLD_GET(regs->idr.dynamic_configuration_reg.check_min_org_size, fld_val, 40, exit);
    if (fld_val == 0x0)
    {
      size_range->min = SOC_PETRA_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT;
    }
    else
    {
      SOC_PA_FLD_GET(regs->idr.original_packet_sizes_reg.min_org_packet_size, idr_min, 50, exit);
       size_range->min = idr_min + SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_EXTERNAL;
    }

    SOC_PA_FLD_GET(regs->idr.dynamic_configuration_reg.check_max_org_size, fld_val, 60, exit);

    if (fld_val == 0x0)
    {
      size_range->max = SOC_PETRA_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT;
    }
    else
    {
      /*
       * The actual test is: drop if actual_pckt_size-1 > configured_max_size.
       */
      SOC_PA_FLD_GET(regs->idr.original_packet_sizes_reg.max_org_packet_size, idr_max, 60, exit);
      size_range->max = idr_max + SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_EXTERNAL;
    }

    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_OUT_OF_RANGE_ERR, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_mgmt_pckt_size_range_get_unsafe()",0,0);
}

uint32
  soc_petra_mgmt_pckt_size_range_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT SOC_PETRA_MGMT_PCKT_SIZE      *size_range
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_PCKT_SIZE_RANGE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(size_range);

  SOC_PETRA_DIFF_DEVICE_CALL(mgmt_pckt_size_range_get_unsafe,(unit, conf_mode_ndx, size_range));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_pckt_size_range_get_unsafe()",0,0);
}

/*********************************************************************
*     Get the core clock frequency of the device.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_core_frequency_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint32                                 *clk_freq
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_CORE_FREQUENCY_GET_UNSAFE);

  *clk_freq = soc_petra_chip_mega_ticks_per_sec_get(unit);

  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_core_frequency_get_unsafe()", 0, 0);
}


uint8
  soc_petra_mgmt_is_pp_enabled(
    SOC_SAND_IN int unit
  )
{
  return soc_petra_sw_db_pp_enable_get(unit);
}
/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>

