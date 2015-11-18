/* $Id: pcp_mgmt.c,v 1.12 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_device_management.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>

#include <soc/dpp/PCP/pcp_general.h>

#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_mgmt.h>
#include <soc/dpp/PCP/pcp_reg_access.h>
#include <soc/dpp/PCP/pcp_tbl_access.h>
#include <soc/dpp/PCP/pcp_chip_regs.h>
#include <soc/dpp/PCP/pcp_chip_tbls.h>
#include <soc/dpp/PCP/pcp_chip_defines.h>
#include <soc/dpp/PCP/pcp_init.h>
#include <soc/dpp/PCP/pcp_sw_db.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define PCP_MGMT_NOF_TBL_MAPS  1
#define PCP_MGMT_NOF_MEM  2

#define PCP_MGMT_MODE_MAX                                      (PCP_MGMT_NOF_ELK_LKP_MODES-1)
#define PCP_MGMT_FC_MAX                                        (SOC_SAND_U32_MAX)
#define PCP_MGMT_DROP_MAX                                      (SOC_SAND_U32_MAX)
#define PCP_MGMT_HEAD_PTR_MAX                                  (SOC_SAND_U32_MAX)
#define PCP_MGMT_BUFF_SIZE_MAX                                 (SOC_SAND_U32_MAX)
#define PCP_MGMT_CAL_LEN_MAX                                   (SOC_SAND_U32_MAX)
#define PCP_MGMT_CAL_M_MAX                                     (SOC_SAND_U32_MAX)
#define PCP_MGMT_CONF_WORD_MAX                                 (SOC_SAND_U32_MAX)
#define PCP_MGMT_SIZE_MBIT_MAX                                 (PCP_MGMT_NOF_HW_IF_DRAM_SIZE_MBITS-1)
#define PCP_MGMT_RX_EQ_CTRL_MAX                                (SOC_SAND_U32_MAX)
#define PCP_MGMT_RX_EQ_DC_GAIN_MAX                             (SOC_SAND_U32_MAX)
#define PCP_MGMT_TX_PREEMP_0T_MAX                              (SOC_SAND_U32_MAX)
#define PCP_MGMT_TX_PREEMP_1T_MAX                              (SOC_SAND_U32_MAX)
#define PCP_MGMT_TX_PREEMP_2T_MAX                              (SOC_SAND_U32_MAX)
#define PCP_MGMT_TX_VODCTRL_MAX                                (SOC_SAND_U32_MAX)
#define PCP_MGMT_BURST_SIZE_MAX                                (SOC_SAND_U32_MAX)

/* } */
/*************
 * MACROS    *
 *************/
/* { */
#define PCP_MGMT_SEM_TAKE                                               \
  if (SOC_SAND_OK != sal_mutex_take(                                      \
                   (sal_mutex_t)INT_TO_PTR(pcp_mgmt_sem_get()),           \
                   SOC_SAND_INFINITE_TIMEOUT                                  \
                 )                                                        \
     )                                                                    \
  {                                                                       \
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_SEM_TAKE_FAIL, PCP_GENERAL_EXIT_PLACE_TAKE_SEMAPHORE, exit); \
  }
                                                                                            
#define PCP_MGMT_SEM_GIVE                                                \
  if (SOC_SAND_OK != sal_mutex_give((sal_mutex_t)(INT_TO_PTR(pcp_mgmt_sem_get()))))\
  {                                                                        \
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_SEM_GIVE_FAIL, PCP_GENERAL_EXIT_PLACE_GIVE_SEMAPHORE, exit); \
  }

#define PCP_MGMT_SEM_IS_INITD  \
  SOC_SAND_NUM2BOOL(pcp_mgmt_sem_get())

/*
 *  Prints phase 1 initialization advance.
 *  Assumes the following variables are defined:
 *   - uint8 silent
 *   - uint32 stage_id
 */
#define PCP_MGMT_PRINT_ADVANCE(str, phase_id)                                \
{                                                                              \
  if (!silent)                                                                 \
  {                                                                            \
    LOG_CLI((BSL_META("    + Phase %u, %.2u: %s\n\r"), phase_id, ++stage_id, str)); \
  }                                                                            \
}

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

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Pcp_procedure_desc_element_mgmt[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_REGISTER_DEVICE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_REGISTER_DEVICE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_REGISTER_DEVICE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_REGISTER_DEVICE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_UNREGISTER_DEVICE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_UNREGISTER_DEVICE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_UNREGISTER_DEVICE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_UNREGISTER_DEVICE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_OP_MODE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_OP_MODE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_OP_MODE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_OP_MODE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_OP_MODE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_OP_MODE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_OP_MODE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_OP_MODE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_INIT_SEQUENCE_PHASE1),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_INIT_SEQUENCE_PHASE1_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_INIT_SEQUENCE_PHASE1_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_INIT_SEQUENCE_PHASE1_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_INIT_SEQUENCE_PHASE2),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_INIT_SEQUENCE_PHASE2_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_INIT_SEQUENCE_PHASE2_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_INIT_SEQUENCE_PHASE2_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_INDIRECT_MEMORY_MAP_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_INDIRECT_TABLE_MAP_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_INDIRECT_MODULE_INFO_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_INDIRECT_MODULE_INFO_INIT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_INDIRECT_MODULE_INIT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_ACCESS_DB_INIT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_MODULE_INIT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_DEVICE_INIT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_DEVICE_CLOSE),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
  SOC_ERROR_DESC_ELEMENT
    Pcp_error_desc_element_mgmt[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    PCP_MGMT_MODE_OUT_OF_RANGE_ERR,
    "PCP_MGMT_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'mode' is out of range. \n\r "
    "The range is: 0 - PCP_MGMT_NOF_ELK_LKP_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MGMT_FC_OUT_OF_RANGE_ERR,
    "PCP_MGMT_FC_OUT_OF_RANGE_ERR",
    "The parameter 'fc' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MGMT_DROP_OUT_OF_RANGE_ERR,
    "PCP_MGMT_DROP_OUT_OF_RANGE_ERR",
    "The parameter 'drop' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MGMT_HEAD_PTR_OUT_OF_RANGE_ERR,
    "PCP_MGMT_HEAD_PTR_OUT_OF_RANGE_ERR",
    "The parameter 'head_ptr' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MGMT_BUFF_SIZE_OUT_OF_RANGE_ERR,
    "PCP_MGMT_BUFF_SIZE_OUT_OF_RANGE_ERR",
    "The parameter 'buff_size' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MGMT_CAL_LEN_OUT_OF_RANGE_ERR,
    "PCP_MGMT_CAL_LEN_OUT_OF_RANGE_ERR",
    "The parameter 'cal_len' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MGMT_CAL_M_OUT_OF_RANGE_ERR,
    "PCP_MGMT_CAL_M_OUT_OF_RANGE_ERR",
    "The parameter 'cal_m' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MGMT_CONF_WORD_OUT_OF_RANGE_ERR,
    "PCP_MGMT_CONF_WORD_OUT_OF_RANGE_ERR",
    "The parameter 'conf_word' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MGMT_SIZE_MBIT_OUT_OF_RANGE_ERR,
    "PCP_MGMT_SIZE_MBIT_OUT_OF_RANGE_ERR",
    "The parameter 'size_mbit' is out of range. \n\r "
    "The range is: 0 - PCP_MGMT_NOF_HW_IF_DRAM_SIZE_MBITS-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MGMT_RX_EQ_CTRL_OUT_OF_RANGE_ERR,
    "PCP_MGMT_RX_EQ_CTRL_OUT_OF_RANGE_ERR",
    "The parameter 'rx_eq_ctrl' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MGMT_RX_EQ_DC_GAIN_OUT_OF_RANGE_ERR,
    "PCP_MGMT_RX_EQ_DC_GAIN_OUT_OF_RANGE_ERR",
    "The parameter 'rx_eq_dc_gain' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MGMT_TX_PREEMP_0T_OUT_OF_RANGE_ERR,
    "PCP_MGMT_TX_PREEMP_0T_OUT_OF_RANGE_ERR",
    "The parameter 'tx_preemp_0t' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MGMT_TX_PREEMP_1T_OUT_OF_RANGE_ERR,
    "PCP_MGMT_TX_PREEMP_1T_OUT_OF_RANGE_ERR",
    "The parameter 'tx_preemp_1t' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MGMT_TX_PREEMP_2T_OUT_OF_RANGE_ERR,
    "PCP_MGMT_TX_PREEMP_2T_OUT_OF_RANGE_ERR",
    "The parameter 'tx_preemp_2t' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MGMT_TX_VODCTRL_OUT_OF_RANGE_ERR,
    "PCP_MGMT_TX_VODCTRL_OUT_OF_RANGE_ERR",
    "The parameter 'tx_vodctrl' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    PCP_MGMT_BURST_SIZE_OUT_OF_RANGE_ERR,
    "PCP_MGMT_BURST_SIZE_OUT_OF_RANGE_ERR",
    "The parameter 'burst_size' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */

  {
    PCP_MGMT_PCP_MEMORY_NOT_READY_ERR,
    "PCP_MGMT_PCP_MEMORY_NOT_READY_ERR",
    "PCP memories  RLDRAM and QDC sram are not ready.\n\r "
    "Unable to set mem init done Bit.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
    {
    PCP_MGMT_PCP_MEMORY_TEST_FAILED_ERR,
    "PCP_MGMT_PCP_MEMORY_TEST_FAILED_ERR",
    "Memory test failed (BIST - running 1).\n\r "
    "Memory preformed on qdr, rld1, rld2.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },

  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

/* Indirect access { */
static
  uint8
    Pcp_mgmt_indirect_module_info_init = FALSE;
static
  SOC_SAND_INDIRECT_MODULE_INFO
    Pcp_mgmt_indirect_module_info[PCP_NUM_OF_INDIRECT_MODULES];

/* Indirect access } */

/*
 *  Global management semaphore (not per-device).
 */
static
  uint32
    Pcp_mgmt_semaphore = 0;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/* general api */

uint32
  pcp_general_puc_enable_set(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint8            enable
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    val;
  PCP_REGS
    *regs = NULL;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = pcp_regs();
  SOC_SAND_CHECK_NULL_INPUT(regs);

  val = SOC_SAND_BOOL2NUM(enable);

  PCP_FLD_SET(regs->csi.puc_reg.pucenable, val , 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_general_puc_enable_set()",0,0);
}


/*
 *  Management semaphore
 */
uint32
  pcp_mgmt_sem_get(void)
{
  return Pcp_mgmt_semaphore;
}

void
  pcp_mgmt_sem_init(void)
{
  Pcp_mgmt_semaphore = PTR_TO_INT(sal_mutex_create("Pcp_mgmt_semaphore"));
}

/* Indirect modules init { */

uint32
  pcp_mgmt_indirect_memory_map_get(
      SOC_SAND_OUT SOC_SAND_INDIRECT_MODULE* info
  )
{
  CONST static
    SOC_SAND_INDIRECT_MEMORY_MAP
      Pcp_mgmt_indirect_memory_map[PCP_MGMT_NOF_MEM] = {
        {0x0, 0xFFFFFFFF},
        {0, 0}
      };

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_INDIRECT_MEMORY_MAP_GET);
  
  SOC_SAND_CHECK_NULL_INPUT(info);
  
  info->memory_map_arr = Pcp_mgmt_indirect_memory_map;
  
 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_indirect_memory_map_get()",0,0);
}

uint32
  pcp_mgmt_indirect_table_map_get(
      SOC_SAND_OUT SOC_SAND_INDIRECT_MODULE* info
  )
{
  CONST static
    SOC_SAND_INDIRECT_TABLES_INFO
      Pcp_mgmt_indirect_tables_map[PCP_MGMT_NOF_TBL_MAPS] = {
        {0, 0, 0, 0, 0}
      };

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_INDIRECT_TABLE_MAP_GET);
  
  info->tables_info = Pcp_mgmt_indirect_tables_map;
  
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_indirect_table_map_get()",0,0);
}

STATIC uint32
  pcp_mgmt_indirect_module_info_init(void)
{
  uint32
    res = SOC_SAND_OK;
  PCP_REGS
    *regs = NULL;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_INDIRECT_MODULE_INFO_INIT);

  if(Pcp_mgmt_indirect_module_info_init == TRUE) {
    PCP_DO_NOTHING_AND_EXIT;
  }

  res = pcp_regs_get(
          &(regs)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_CHECK_NULL_INPUT(regs);

/* } */

  Pcp_mgmt_indirect_module_info[PCP_ECI_ID].module_index =
    PCP_ECI_ID;
  Pcp_mgmt_indirect_module_info[PCP_ECI_ID].read_result_offset =
    regs->eci.indirect_command_rd_data_reg_0.addr.base;

  Pcp_mgmt_indirect_module_info[PCP_ECI_ID].word_size =
    sizeof(uint32);

  Pcp_mgmt_indirect_module_info[PCP_ECI_ID].access_trigger =
    regs->eci.indirect_command_reg.addr.base;

  Pcp_mgmt_indirect_module_info[PCP_ECI_ID].access_address =
    regs->eci.indirect_command_address_reg.addr.base;

  Pcp_mgmt_indirect_module_info[PCP_ECI_ID].write_buffer_offset =
    regs->eci.indirect_command_wr_data_reg_0.addr.base;

  Pcp_mgmt_indirect_module_info[PCP_ECI_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Pcp_mgmt_indirect_module_info[PCP_ELK_ID].module_index =
    PCP_ELK_ID;
  Pcp_mgmt_indirect_module_info[PCP_ELK_ID].read_result_offset =
    regs->elk.indirect_command_rd_data_reg_0.addr.base;

  Pcp_mgmt_indirect_module_info[PCP_ELK_ID].word_size =
    sizeof(uint32);

  Pcp_mgmt_indirect_module_info[PCP_ELK_ID].access_trigger =
    regs->elk.indirect_command_reg.addr.base;

  Pcp_mgmt_indirect_module_info[PCP_ELK_ID].access_address =
    regs->elk.indirect_command_address_reg.addr.base;

  Pcp_mgmt_indirect_module_info[PCP_ELK_ID].write_buffer_offset =
    regs->elk.indirect_command_wr_data_reg_0.addr.base;

  Pcp_mgmt_indirect_module_info[PCP_ELK_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Pcp_mgmt_indirect_module_info[PCP_OAM_ID].module_index =
    PCP_OAM_ID;
  Pcp_mgmt_indirect_module_info[PCP_OAM_ID].read_result_offset =
    regs->oam.indirect_command_rd_data_reg_0.addr.base;

  Pcp_mgmt_indirect_module_info[PCP_OAM_ID].word_size =
    sizeof(uint32);

  Pcp_mgmt_indirect_module_info[PCP_OAM_ID].access_trigger =
    regs->oam.indirect_command_reg.addr.base;

  Pcp_mgmt_indirect_module_info[PCP_OAM_ID].access_address =
    regs->oam.indirect_command_address_reg.addr.base;

  Pcp_mgmt_indirect_module_info[PCP_OAM_ID].write_buffer_offset =
    regs->oam.indirect_command_wr_data_reg_0.addr.base;

  Pcp_mgmt_indirect_module_info[PCP_OAM_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  Pcp_mgmt_indirect_module_info[PCP_STS_ID].module_index =
    PCP_STS_ID;
  Pcp_mgmt_indirect_module_info[PCP_STS_ID].read_result_offset =
    regs->sts.indirect_command_rd_data_reg_0.addr.base;

  Pcp_mgmt_indirect_module_info[PCP_STS_ID].word_size =
    sizeof(uint32);

  Pcp_mgmt_indirect_module_info[PCP_STS_ID].access_trigger =
    regs->sts.indirect_command_reg.addr.base;

  Pcp_mgmt_indirect_module_info[PCP_STS_ID].access_address =
    regs->sts.indirect_command_address_reg.addr.base;

  Pcp_mgmt_indirect_module_info[PCP_STS_ID].write_buffer_offset =
    regs->sts.indirect_command_wr_data_reg_0.addr.base;

  Pcp_mgmt_indirect_module_info[PCP_STS_ID].module_bits =
    SOC_SAND_INDIRECT_ERASE_MODULE_BITS;

  
  Pcp_mgmt_indirect_module_info_init = TRUE;
  
 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_indirect_module_info()",0,0);
}

uint32
  pcp_mgmt_indirect_module_info_get(
      SOC_SAND_OUT SOC_SAND_INDIRECT_MODULE* info
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_INDIRECT_MODULE_INFO_GET);
  
  res = pcp_mgmt_indirect_module_info_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  info->info_arr = Pcp_mgmt_indirect_module_info;
  info->info_arr_max_index = PCP_NUM_OF_INDIRECT_MODULES;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_indirect_module_info_get()",0,0);
}


uint32
  pcp_mgmt_indirect_module_get(
      SOC_SAND_OUT SOC_SAND_INDIRECT_MODULE* info
  )
{
  uint32
    res = SOC_SAND_OK;

  
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_INDIRECT_MODULE_INIT);
  
  res = pcp_mgmt_indirect_module_info_get(info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = pcp_mgmt_indirect_table_map_get(info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  res = pcp_mgmt_indirect_memory_map_get(info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_indirect_module_get()",0,0);
}

STATIC uint32
  pcp_mgmt_indirect_module_init(void)
{
  uint32
    res = SOC_SAND_OK;

  
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_INDIRECT_MODULE_INIT);
  
  res = pcp_mgmt_indirect_module_info_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_indirect_module_init()",0,0);
}
STATIC uint32
  pcp_mgmt_access_db_init(void)
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_ACCESS_DB_INIT);

  res = pcp_regs_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = pcp_tbls_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_mgmt_access_db_init()",0,0);
}


/* Indirect module init }*/
STATIC uint32
  pcp_mgmt_module_init(void)
{
  uint32
    res;

  SOC_SAND_INTERRUPT_INIT_DEFS;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_MODULE_INIT);
  
  if (!PCP_MGMT_SEM_IS_INITD)
  {
    SOC_SAND_INTERRUPTS_STOP;

    if(!pcp_mgmt_sem_get())
    {
      pcp_mgmt_sem_init();
    }

    SOC_SAND_INTERRUPTS_START_IF_STOPPED;
  }

  /* At this point we have a semaphore, so we can take it and allow interrupts */
  PCP_MGMT_SEM_TAKE;

  res = pcp_mgmt_access_db_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = pcp_mgmt_indirect_module_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = pcp_sw_db_initialize();
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /* Add list of PCP errors to all-system errors pool                                                  */
  res = pcp_errors_desc_add();
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  /* Now add list of PCP procedure descriptors to all-system pool.                                     */
  res = pcp_procedure_desc_add() ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  /* We are not going to change macros to avoid such cases */
  /* coverity[dead_error_line] */
  SOC_SAND_INTERRUPTS_START_IF_STOPPED;
  PCP_MGMT_SEM_GIVE;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_mgmt_module_init()",0,0);
}

/*********************************************************************
*     This procedure registers a new device to be taken care
 *     of by this device driver. Physical device must be
 *     accessible by CPU when this call is made
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_register_device_unsafe(
    SOC_SAND_IN  uint32                        *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR      reset_device_ptr,
    SOC_SAND_OUT int                       *unit_ptr
  )
{
  uint32
    res = SOC_SAND_OK;
  int unit = *unit_ptr;
  SOC_SAND_DEV_VER_INFO
    ver_info;
  SOC_SAND_INDIRECT_MODULE
    indirect_module;
  uint32
    *base;
  PCP_REG_FIELD
    chip_type_fld,
    dbg_ver_fld,
    chip_ver_fld;
  PCP_REGS
    *regs = NULL;
  uint8
    is_low_sim_active;
  uint32
    reg_val,
        chip_type,
        dbg_ver,
        chip_ver,
        dbg_subver;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_REGISTER_DEVICE_UNSAFE);

#ifndef SAND_LOW_LEVEL_SIMULATION
  is_low_sim_active = FALSE;
#else
  is_low_sim_active = soc_sand_low_is_sim_active_get();
#endif

  SOC_SAND_CHECK_NULL_INPUT(unit_ptr);
  
  res = pcp_mgmt_module_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  soc_sand_clear_SAND_DEV_VER_INFO(&ver_info);
  chip_type_fld.lsb = PCP_MGMT_CHIP_TYPE_FLD_LSB;
  chip_type_fld.msb = PCP_MGMT_CHIP_TYPE_FLD_MSB;
  dbg_ver_fld.lsb   = PCP_MGMT_DBG_VER_FLD_LSB;
  dbg_ver_fld.msb   = PCP_MGMT_DBG_VER_FLD_MSB;
  chip_ver_fld.lsb  = PCP_MGMT_CHIP_VER_FLD_LSB;
  chip_ver_fld.msb  = PCP_MGMT_CHIP_VER_FLD_MSB;

  ver_info.ver_reg_offset  = PCP_MGMT_VER_REG_BASE;
  ver_info.logic_chip_type = SOC_SAND_DEV_PCP;
  ver_info.chip_type       = PCP_EXPECTED_CHIP_TYPE;

  ver_info.chip_type_shift= PCP_FLD_SHIFT(chip_type_fld);
  ver_info.chip_type_mask = PCP_FLD_MASK(chip_type_fld);
  ver_info.dbg_ver_shift  = PCP_FLD_SHIFT(dbg_ver_fld);
  ver_info.dbg_ver_mask   = PCP_FLD_MASK(dbg_ver_fld);
  ver_info.chip_ver_shift = PCP_FLD_SHIFT(chip_ver_fld);
  ver_info.chip_ver_mask  = PCP_FLD_MASK(chip_ver_fld);

  /* In order for indirect access to function properly we have to                                         */
  /* point it to the right addresses.                                                                     */
  pcp_mgmt_indirect_module_get(&(indirect_module));

  base = (uint32*)base_address;

  /*
   *  Validate low level access to the Pcp
   */
  

  if (!is_low_sim_active) {

      regs = pcp_regs();

      reg_val = 0;

      res = soc_sand_physical_read_from_chip(
                    &reg_val,
                    base_address,
                    regs->eci.rev_reg.chip_type.base,
                    SOC_SAND_REG_SIZE_BYTES
                );
      SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

      PCP_FLD_FROM_REG(regs->eci.rev_reg.chip_type, chip_type, reg_val, 14, exit);
      PCP_FLD_FROM_REG(regs->eci.rev_reg.dbg_ver,   dbg_ver,   reg_val, 15, exit);
      PCP_FLD_FROM_REG(regs->eci.rev_reg.chip_ver,  chip_ver,  reg_val, 16, exit);

      if (chip_type != (PCP_EXPECTED_CHIP_TYPE)) {
          LOG_CLI((BSL_META("ERROR: excpected 0xdc150 recived 0x%x\n"),chip_type));
          SOC_SAND_SET_ERROR_CODE(PCP_LOW_LEVEL_ACCESS_ERR, 17, exit);
      } else {

          res = soc_sand_physical_read_from_chip(
                        &reg_val,
                        base_address,
                        regs->eci.debug_subver_reg.debug_subver.base,
                        SOC_SAND_REG_SIZE_BYTES
                    );
          SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

          PCP_FLD_FROM_REG(regs->eci.debug_subver_reg.debug_subver, dbg_subver, reg_val, 19, exit);
          LOG_CLI((BSL_META("   chip type=0x%x, debug version=0x%x, chip version=0x%x, dbg_subver=0x%x"), chip_type, dbg_ver, chip_ver, dbg_subver));
      }
  } 

   /* Register the device in SOC_SAND. */
  res = soc_sand_device_register(
    base,                           /* uint32                 *base_address, */
    PCP_TOTAL_SIZE_OF_REGS,         /* uint32                 mem_size, */
    NULL,                           /* SOC_SAND_UNMASK_FUNC_PTR               unmask_func_ptr, */
    NULL,                           /* SOC_SAND_IS_BIT_AUTO_CLEAR_FUNC_PTR    is_bit_ac_func_ptr, */
    NULL,                           /* SOC_SAND_IS_DEVICE_INTERRUPTS_MASKED   is_dev_int_mask_func_ptr, */
    NULL,                           /* SOC_SAND_GET_DEVICE_INTERRUPTS_MASK    get_dev_mask_func_ptr, */
    NULL,                           /* SOC_SAND_MASK_SPECIFIC_INTERRUPT_CAUSE mask_specific_interrupt_cause_ptr, */
    reset_device_ptr,               /* SOC_SAND_RESET_DEVICE_FUNC_PTR    reset_device_ptr, */
    NULL,                           /* SOC_SAND_IS_OFFSET_READ_OR_WRITE_PROTECT_FUNC_PTR is_read_write_protect_ptr, */
    &ver_info,                      /* SOC_SAND_DEV_VER_INFO             *ver_info, */
    &indirect_module,               /* SOC_SAND_INDIRECT_MODULE          *indirect_module, */
    0xFFFFFFFF,                     /* uint32                        interrupt_mask_address, */
    &unit                      /* int                  *unit_ptr */
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  *unit_ptr = unit ;


  soc_sand_indirect_set_nof_repetitions_unsafe(unit, 0);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_register_device_unsafe()", 0, 0);
}

uint32
  pcp_register_device_verify(
    SOC_SAND_IN  uint32                        *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR      reset_device_ptr,
    SOC_SAND_IN  int                       *unit_ptr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_REGISTER_DEVICE_VERIFY);

  /* PCP_STRUCT_VERIFY(uint32, base_address, 10, exit); */
  /* PCP_STRUCT_VERIFY(uint32, unit_ptr, 20, exit); */

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_register_device_verify()", 0, 0);
}

/*********************************************************************
*     Undo pcp_register_device
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_unregister_device_unsafe(
    SOC_SAND_IN  int                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_UNREGISTER_DEVICE_UNSAFE);

  res = soc_sand_device_unregister(
    unit
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_unregister_device_unsafe()", 0, 0);
}

uint32
  pcp_unregister_device_verify(
    SOC_SAND_IN  int                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_UNREGISTER_DEVICE_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_unregister_device_verify()", 0, 0);
}

/*********************************************************************
*     Set PCP device operation mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_mgmt_op_mode_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_MGMT_OP_MODE                *info
  )
{
  uint32
    res;
  uint32
    fld_val;
  PCP_REGS
    *regs = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_OP_MODE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = pcp_sw_db_device_initialize(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  soc_petra_sw_db_op_mode_elk_enable_set(unit, info->elk.enable);
  soc_petra_sw_db_op_mode_elk_ilm_key_mask_bitmap_set(unit, info->elk.ilm_key_mask_bitmap);
  soc_petra_sw_db_op_mode_oam_enable_set(unit, info->oam.enable);
  soc_petra_sw_db_op_mode_sts_conf_word_set(unit, info->sts.conf_word);

  /* update hw according to operation mode */
  regs = pcp_regs();
  SOC_SAND_CHECK_NULL_INPUT(regs);

  if (info->elk.mode == PCP_MGMT_ELK_LKP_MODE_BOTH)
  {
    fld_val = 1;
  }
  else
  {
    fld_val = 0;
  }
  PCP_FLD_SET(regs->elk.lpm_lkp_conf_reg.rld_is_shared, fld_val , 20, exit);

    if (info->elk.size_mbit == PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_576)
  {
    fld_val = 1;
  }
  else
  {
    fld_val = 0;
  }
  PCP_FLD_SET(regs->elk.lpm_lkp_conf_reg.use_large_rld, fld_val , 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_op_mode_set_unsafe()", 0, 0);
}

uint32
  pcp_mgmt_op_mode_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_MGMT_OP_MODE                *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_OP_MODE_SET_VERIFY);

  PCP_STRUCT_VERIFY(PCP_MGMT_OP_MODE, info, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_op_mode_set_verify()", 0, 0);
}

uint32
  pcp_mgmt_op_mode_get_verify(
    SOC_SAND_IN  int                       unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_OP_MODE_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_op_mode_get_verify()", 0, 0);
}

/*********************************************************************
*     Set PCP device operation mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_mgmt_op_mode_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT PCP_MGMT_OP_MODE                *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_OP_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_MGMT_OP_MODE_clear(info);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_op_mode_get_unsafe()", 0, 0);
}



/*
 *  Init sequence -
 *  functional init. {
 */
STATIC uint32
  pcp_mgmt_functional_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_OP_MODE_GET_UNSAFE);

  res = pcp_frwrd_ipv4_init_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_functional_init()", 0, 0);
}

/*********************************************************************
*     Initialize the device, including:1. Configuration of
 *     Soc_petra power up configuration word2. Initialize the
 *     device tables and registers to default values 3.
 *     Initialize board-specific hardware interfaces according
 *     to configurable information, as passed in 'hw_adjust' 4.
 *     Perform basic device initialization 5. DMA
 *     initialization
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_mgmt_init_sequence_phase1_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_HW_ADJUSTMENTS              *hw_adjust,
    SOC_SAND_IN  uint8                       silent
  )
{
  uint32
    res = SOC_SAND_OK,
        rld_init_fld,
        qdc_init_fld,
        mem_init_done_fld,
        op_mode_sts_conf_word;
    uint32
        stage_id = 0;
  PCP_REGS
    *regs = NULL;
  uint8
    is_low_sim_active;
   uint8
      op_mode_elk_enable,
      op_mode_oam_enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_INIT_SEQUENCE_PHASE1_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

#ifndef SAND_LOW_LEVEL_SIMULATION
  is_low_sim_active = FALSE;
#else
  is_low_sim_active = soc_sand_low_is_sim_active_get();
#endif

  op_mode_elk_enable   = soc_petra_sw_db_op_mode_elk_enable_get(unit);
  op_mode_oam_enable = soc_petra_sw_db_op_mode_oam_enable_get(unit);

  regs = pcp_regs();
  SOC_SAND_CHECK_NULL_INPUT(regs);

  if (!silent)
  {
    LOG_CLI((BSL_META("\n\r"
                      "   Phase 1 initialization: device %u"
                      "\n\r"),
             unit
             ));
  }

  if ((!is_low_sim_active) && ((op_mode_elk_enable == TRUE) || (op_mode_oam_enable == TRUE)))
  {
    /* memory out of reset */
      PCP_MGMT_PRINT_ADVANCE("Take Memory out-of-reset", 1);
      PCP_FLD_GET(regs->eci.mem_init_reg.rld_init_done, rld_init_fld, 10, exit);
      PCP_FLD_GET(regs->eci.mem_init_reg.qdc_init_done, qdc_init_fld, 11, exit);
      PCP_FLD_GET(regs->eci.mem_init_reg.mem_init_done, mem_init_done_fld, 12, exit);
      if ((rld_init_fld == 0x0) || (qdc_init_fld == 0x0))
      {
          if (mem_init_done_fld == 0x1)
          {
              LOG_CLI((BSL_META("NOTICE: wrong behavior. PCP memories are already ready at init.\n")));
          }
          else
          {
              SOC_SAND_SET_ERROR_CODE(PCP_MGMT_PCP_MEMORY_NOT_READY_ERR, 20, exit);
          }
      }
      PCP_FLD_SET(regs->eci.mem_init_reg.mem_init_done, 0x1 , 30, exit);
  }

  /* Clear puc enable bit */
  PCP_MGMT_PRINT_ADVANCE("Set PUC configuration word for soc_petra, Clear PUC enable bit", 1);
  PCP_FLD_SET(regs->csi.puc_reg.pucenable, 0x0 , 40, exit);
  op_mode_sts_conf_word = soc_petra_sw_db_op_mode_sts_conf_word_get(unit);
  PCP_FLD_SET(regs->csi.puc_reg.pucdata, op_mode_sts_conf_word, 42, exit);


  /* Configure DMA/ELK/OAM straming channels and status frame (flow control) distribution */
  /*
   * DMA Class 1 to streaming channel 0/status frame slot 0 (Soc_petra port  0)
   * DMA Class 2 to streaming channel 1/status frame slot 1 (Soc_petra port 73)
   * DMA Class 3 to streaming channel 2/status frame slot 2 (Soc_petra port 74)
   * DMA Class 4 to streaming channel 3/status frame slot 3 (Soc_petra port 75)
   * DMA Class 5 to streaming channel 4/status frame slot 4 (Soc_petra port 76)
   * ELK Module  to streaming channel 5/status frame slot 5 (Soc_petra port 77)
   * OAM Module  to streaming channel 6/status frame slot 6 (Soc_petra port 78)
   */
  PCP_MGMT_PRINT_ADVANCE("Configure DMA/ELK/OAM straming channels and status frame (flow control) distribution", 1);
  PCP_FLD_SET(regs->csi.rpm_map_class6_reg.rpm_map_class6, 0x3e , 50, exit);                 /* chaneg DMA default to unmapped chanel 62 */
  PCP_FLD_SET(regs->csi.rpm_map_class7_reg.rpm_map_class7, 0x3f , 60, exit);                 /* chaneg DMA default to unmapped chanel 63 */
  PCP_FLD_SET(regs->csi.rpm_map_elk_reg.rpm_map_elk, 0x5 , 70, exit);                                 /* map ELK to class 0x5 */
  PCP_FLD_SET(regs->csi.rpm_map_oam_reg.rpm_map_oam, 0x6 , 80, exit);                           /* map OAM to class 0x5 */

  PCP_FLD_SET(regs->csi.rpm_class6_stat_slot_reg.rpm_class6_stat_slot, 0xf , 90, exit);     /* chaneg DMA default to 0xff - disable */
  PCP_FLD_SET(regs->csi.rpm_class7_stat_slot_reg.rpm_class7_stat_slot, 0xf , 100, exit);     /* chaneg DMA default to 0xff - disable */
  PCP_FLD_SET(regs->csi.rpm_elk_stat_slot_reg.rpm_elk_stat_slot, 0x5 , 110, exit);                  /* map ELK to slot 0x5 */
  PCP_FLD_SET(regs->csi.rpm_oam_stat_slot_reg.rpm_oam_stat_slot, 0x6, 120, exit);           /* map OAM to slot 0x6 */

  /* Configure ELK learn lookup prefix bits */
  PCP_MGMT_PRINT_ADVANCE("Configure ELK learn lookup prefix bits", 1);
  PCP_FLD_SET(regs->elk.lrn_lkp_gen_conf_reg.learn_lookup_elk_bits_63to58, 0x14 , 130, exit);
  PCP_FLD_SET(regs->elk.lrn_lkp_gen_conf_reg.learn_lookup_elk_bits_127to122, 0x10 , 140, exit);

  /* Configure ELK LPM bits */
  PCP_MGMT_PRINT_ADVANCE("Configure ELK LPM bits", 1);
  PCP_FLD_SET(regs->elk.lpm_lkp_conf_reg.lpm_req_bits63to60, 0x5 , 141, exit);

  /* Configure dram sizes */
  PCP_MGMT_PRINT_ADVANCE("Configure dram sizes", 1);
  if (hw_adjust->dram.size_mbit == PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_288)
  {
    PCP_FLD_SET(regs->eci.gen_cfg_reg.rldram_576mb, 0x0 , 142, exit);
    PCP_FLD_SET(regs->elk.lpm_lkp_conf_reg.use_large_rld, 0x0 , 143, exit);
  }
  else
  {
    PCP_FLD_SET(regs->eci.gen_cfg_reg.rldram_576mb, 0x1 , 144, exit);
    PCP_FLD_SET(regs->elk.lpm_lkp_conf_reg.use_large_rld, 0x1 , 144, exit);
  }
  if (hw_adjust->qdr.qdr_size_mbit== PCP_MGMT_HW_IF_QDR_SIZE_MBIT_36)
  {
    PCP_FLD_SET(regs->eci.gen_cfg_reg.qdr_36mb, 0x1 , 145, exit);
  }
  else
  {
    PCP_FLD_SET(regs->eci.gen_cfg_reg.qdr_36mb, 0x0 , 146, exit);
  }

  /* Configure statisrics record AC offset */
  PCP_MGMT_PRINT_ADVANCE("Configure statisrics record AC offset", 1);
  PCP_FLD_SET(regs->sts.ac_offset_reg.in_ac_offset, 0x0 , 147, exit);
  PCP_FLD_SET(regs->sts.ac_offset_reg.out_ac_offset, 0x0 , 148, exit);


  /************************************************************************/
  /* Initialize all tables                                                */
  /*                                                                      */
  /* Most tables are zeroed. Some - initialized to non-zero default       */
  /************************************************************************/
    PCP_MGMT_PRINT_ADVANCE("Tables init", 1);

  res = pcp_mgmt_tbls_init(
          unit,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  PCP_MGMT_PRINT_ADVANCE("Set default configuration", 1);
  res = pcp_mgmt_functional_init(
          unit,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 155, exit);


  /* Enable status channel for out-of-band flow control */
  PCP_MGMT_PRINT_ADVANCE("Enable status channel for out-of-band flow control", 1);
  PCP_FLD_SET(regs->csi.rpm_stat_cfg_reg.rpm_stat_en, 0x1 ,160, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_init_sequence_phase1_unsafe()", 0, 0);
}

uint32
  pcp_mgmt_init_sequence_phase1_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  PCP_HW_ADJUSTMENTS              *hw_adjust,
    SOC_SAND_IN  uint8                       silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_INIT_SEQUENCE_PHASE1_VERIFY);

  PCP_STRUCT_VERIFY(PCP_HW_ADJUSTMENTS, hw_adjust, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_init_sequence_phase1_verify()", 0, 0);
}

/*********************************************************************
*     This API enables all interfaces, as set in the operation
 *     mode.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_mgmt_init_sequence_phase2_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint8                       silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_INIT_SEQUENCE_PHASE2_UNSAFE);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_init_sequence_phase2_unsafe()", 0, 0);
}

uint32
  pcp_mgmt_init_sequence_phase2_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint8                       silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_MGMT_INIT_SEQUENCE_PHASE2_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_init_sequence_phase2_verify()", 0, 0);
}

uint32
  pcp_mgmt_memory_test_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint8                       silent
  )
{
  uint32
    res = SOC_SAND_OK,
        i,
        qdr_tbl_size_bits,
        qdr_tbl_offset,    
        rld1_tbl_size_bits,
        rld1_tbl_offset,
        rld2_tbl_size_bits,
        rld2_tbl_offset;
    PCP_TBLS
    *pcp_tbl = NULL;
    PCP_ECI_QDR_TBL_DATA  
        qdr_tbl_data_set,
        qdr_tbl_data_get;
    PCP_ECI_RLD1_TBL_DATA  
        rld1_tbl_data_set,
        rld1_tbl_data_get;
    PCP_ECI_RLD2_TBL_DATA  
        rld2_tbl_data_set,
        rld2_tbl_data_get;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    sal_memset(&qdr_tbl_data_set, 0x0, sizeof(PCP_ECI_QDR_TBL_DATA));
    sal_memset(&qdr_tbl_data_get, 0x0, sizeof(PCP_ECI_QDR_TBL_DATA));
    sal_memset(&rld1_tbl_data_set, 0x0, sizeof(PCP_ECI_RLD1_TBL_DATA));
    sal_memset(&rld1_tbl_data_get, 0x0, sizeof(PCP_ECI_RLD1_TBL_DATA));
    sal_memset(&rld2_tbl_data_set, 0x0, sizeof(PCP_ECI_RLD2_TBL_DATA));
    sal_memset(&rld2_tbl_data_get, 0x0, sizeof(PCP_ECI_RLD2_TBL_DATA));

    res = pcp_tbls_get(&(pcp_tbl));
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* 
   *    Test QDR 
   */ 

  /* clear offset 0 */
  qdr_tbl_offset = 0;
  qdr_tbl_data_set.qdr_data = 0;

  res = pcp_eci_qdr_tbl_set(
                unit,
                qdr_tbl_offset,
                &qdr_tbl_data_set
            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    /* addr runnig 1 */
    qdr_tbl_size_bits = soc_sand_log2_round_up(pcp_tbl->eci.qdr_tbl.addr.size);
    for (i = 0 ; i < qdr_tbl_size_bits ; i++) {

        qdr_tbl_offset = 1 << i;
        qdr_tbl_data_set.qdr_data = i;
        
        res = pcp_eci_qdr_tbl_set(
                        unit,
                        qdr_tbl_offset,
                        &qdr_tbl_data_set
                    );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

    /* verify that offset 0 is still cleared */
    qdr_tbl_offset = 0;
    res = pcp_eci_qdr_tbl_get(
                unit,
                qdr_tbl_offset,
                &qdr_tbl_data_get
             );
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    if (qdr_tbl_data_get.qdr_data != 0)
    {
        LOG_CLI((BSL_META("ERROR 24: qdr_tbl_data_get.qdr_data=%d\n"),qdr_tbl_data_get.qdr_data));
        SOC_SAND_SET_ERROR_CODE(PCP_MGMT_PCP_MEMORY_TEST_FAILED_ERR, 24, exit);
    }

    for (i = 0 ; i < qdr_tbl_size_bits ; i++) {

        qdr_tbl_offset = 1 << i;
        
        res = pcp_eci_qdr_tbl_get(
                        unit,
                        qdr_tbl_offset,
                        &qdr_tbl_data_get
                    );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        if (qdr_tbl_data_get.qdr_data != i)
        {
            SOC_SAND_SET_ERROR_CODE(PCP_MGMT_PCP_MEMORY_TEST_FAILED_ERR, 40, exit);
        }
    }

    /* test qdr - data runnig 1 */
    for (i = 0 ; i < pcp_tbl->eci.qdr_tbl.addr.width_bits ; i++) {

        qdr_tbl_offset = i;
        qdr_tbl_data_set.qdr_data = 1 << i;
        
        res = pcp_eci_qdr_tbl_set(
                        unit,
                        qdr_tbl_offset,
                        &qdr_tbl_data_set
                    );
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
    for (i = 0 ; i < pcp_tbl->eci.qdr_tbl.addr.width_bits ; i++) {

        qdr_tbl_offset = i;
        qdr_tbl_data_set.qdr_data = 1 << i;

        res = pcp_eci_qdr_tbl_get(
                        unit,
                        qdr_tbl_offset,
                        &qdr_tbl_data_get
                    );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

        if (sal_memcmp(&qdr_tbl_data_set, &qdr_tbl_data_get, sizeof(PCP_ECI_QDR_TBL_DATA)) != 0x0)        {
            SOC_SAND_SET_ERROR_CODE(PCP_MGMT_PCP_MEMORY_TEST_FAILED_ERR, 70, exit);
        }
    }

  /* 
   *    Test RLD1
   */

    /* clear offset 0 */
    rld1_tbl_offset = 0;
    rld1_tbl_data_set.rld1_data[0] = 0;
    
    res = pcp_eci_rld1_tbl_set(
                    unit,
                    rld1_tbl_offset,
                    &rld1_tbl_data_set
                );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    /* test rld1 - addr runnig 1 */
    rld1_tbl_size_bits = soc_sand_log2_round_up(pcp_tbl->eci.rld1_tbl.addr.size);
    for (i = 0 ; i < rld1_tbl_size_bits ; i++) {

        rld1_tbl_offset = 1 << i;
        rld1_tbl_data_set.rld1_data[0] = i;
        
        res = pcp_eci_rld1_tbl_set(
                        unit,
                        rld1_tbl_offset,
                        &rld1_tbl_data_set
                    );
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    }

    /* verify that offset 0 is still cleared */
    rld1_tbl_offset = 0;
    res = pcp_eci_rld1_tbl_get(
                    unit,
                    rld1_tbl_offset,
                    &rld1_tbl_data_get
                );
    SOC_SAND_CHECK_FUNC_RESULT(res, 82, exit);

    if (rld1_tbl_data_get.rld1_data[0] != 0)
    {
        SOC_SAND_SET_ERROR_CODE(PCP_MGMT_PCP_MEMORY_TEST_FAILED_ERR, 84, exit);
    }

    for (i = 0 ; i < rld1_tbl_size_bits ; i++) {

        rld1_tbl_offset = 1 << i;
        
        res = pcp_eci_rld1_tbl_get(
                        unit,
                        rld1_tbl_offset,
                        &rld1_tbl_data_get
                    );
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

        if (rld1_tbl_data_get.rld1_data[0] != i)
        {
            SOC_SAND_SET_ERROR_CODE(PCP_MGMT_PCP_MEMORY_TEST_FAILED_ERR, 100, exit);
        }
    }

    /* test rld1 - data runnig 1 */
    for (i = 0 ; i < pcp_tbl->eci.rld1_tbl.addr.width_bits ; i++) {

        rld1_tbl_offset = i;

        sal_memset(&rld1_tbl_data_set, 0x0, sizeof(PCP_ECI_RLD1_TBL_DATA));
        if (i < 32) {
            rld1_tbl_data_set.rld1_data[0] = 1 << i;
        } else if (i < 64) {
            rld1_tbl_data_set.rld1_data[1] = 1 << (i % 32);
        } else {
            rld1_tbl_data_set.rld1_data[2] = 1 << (i % 32);
        }
        
        res = pcp_eci_rld1_tbl_set(
                        unit,
                        rld1_tbl_offset,
                        &rld1_tbl_data_set
                    );
        SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
    }
    for (i = 0 ; i < pcp_tbl->eci.rld1_tbl.addr.width_bits ; i++) {

        rld1_tbl_offset = i;

        sal_memset(&rld1_tbl_data_set, 0x0, sizeof(PCP_ECI_RLD1_TBL_DATA));
        if (i < 32) {
            rld1_tbl_data_set.rld1_data[0] = 1 << i;
        } else if (i < 64) {
            rld1_tbl_data_set.rld1_data[1] = 1 << (i % 32);
        } else {
            rld1_tbl_data_set.rld1_data[2] = 1 << (i % 32);
        }

        res = pcp_eci_rld1_tbl_get(
                        unit,
                        rld1_tbl_offset,
                        &rld1_tbl_data_get
                    );
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

        if (sal_memcmp(&rld1_tbl_data_set, &rld1_tbl_data_get, sizeof(PCP_ECI_RLD1_TBL_DATA)) != 0x0)        {
            SOC_SAND_SET_ERROR_CODE(PCP_MGMT_PCP_MEMORY_TEST_FAILED_ERR, 130, exit);
        }
    }

  /* 
   *    Test RLD2 
   */ 

    /* clear offset 0 */
    rld2_tbl_offset = 0;
    rld2_tbl_data_set.rld2_data[0] = 0;
    
    res = pcp_eci_rld2_tbl_set(
                    unit,
                    rld2_tbl_offset,
                    &rld2_tbl_data_set
                );
    SOC_SAND_CHECK_FUNC_RESULT(res, 135, exit);

    /* test rld2 - addr runnig 1 */
    rld2_tbl_size_bits = soc_sand_log2_round_up(pcp_tbl->eci.rld2_tbl.addr.size);
    for (i = 0 ; i < rld2_tbl_size_bits ; i++) {

        rld2_tbl_offset = 1 << i;
        rld2_tbl_data_set.rld2_data[0] = i;
        
        res = pcp_eci_rld2_tbl_set(
                        unit,
                        rld2_tbl_offset,
                        &rld2_tbl_data_set
                    );
        SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
    }

    /* verify that offset 0 is still cleared */
    rld2_tbl_offset = 0;
    res = pcp_eci_rld2_tbl_get(
                    unit,
                    rld2_tbl_offset,
                    &rld2_tbl_data_get
                );
    SOC_SAND_CHECK_FUNC_RESULT(res, 142, exit);

    if (rld2_tbl_data_get.rld2_data[0] != 0)
    {
        SOC_SAND_SET_ERROR_CODE(PCP_MGMT_PCP_MEMORY_TEST_FAILED_ERR, 144, exit);
    }

    for (i = 0 ; i < rld2_tbl_size_bits ; i++) {

        rld2_tbl_offset = 1 << i;
        
        res = pcp_eci_rld2_tbl_get(
                        unit,
                        rld2_tbl_offset,
                        &rld2_tbl_data_get
                    );
        SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

        if (rld2_tbl_data_get.rld2_data[0] != i)
        {
            SOC_SAND_SET_ERROR_CODE(PCP_MGMT_PCP_MEMORY_TEST_FAILED_ERR, 160, exit);
        }
    }

    /* test rld2 - data runnig 1 */
    for (i = 0 ; i < pcp_tbl->eci.rld2_tbl.addr.width_bits ; i++) {

        rld2_tbl_offset = i;

        sal_memset(&rld2_tbl_data_set, 0x0, sizeof(PCP_ECI_RLD2_TBL_DATA));
        if (i < 32) {
            rld2_tbl_data_set.rld2_data[0] = 1 << i;
        } else if (i < 64) {
            rld2_tbl_data_set.rld2_data[1] = 1 << (i % 32);
        } else {
            rld2_tbl_data_set.rld2_data[2] = 1 << (i % 32);
        }
        
        res = pcp_eci_rld2_tbl_set(
                        unit,
                        rld2_tbl_offset,
                        &rld2_tbl_data_set
                    );
        SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
    }
    for (i = 0 ; i < pcp_tbl->eci.rld2_tbl.addr.width_bits ; i++) {

        rld2_tbl_offset = i;

        sal_memset(&rld2_tbl_data_set, 0x0, sizeof(PCP_ECI_RLD2_TBL_DATA));
        if (i < 32) {
            rld2_tbl_data_set.rld2_data[0] = 1 << i;
        } else if (i < 64) {
            rld2_tbl_data_set.rld2_data[1] = 1 << (i % 32);
        } else {
            rld2_tbl_data_set.rld2_data[2] = 1 << (i % 32);
        }

        res = pcp_eci_rld2_tbl_get(
                        unit,
                        rld2_tbl_offset,
                        &rld2_tbl_data_get
                    );
        SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

        if (sal_memcmp(&rld2_tbl_data_set, &rld2_tbl_data_get, sizeof(PCP_ECI_RLD2_TBL_DATA)) != 0x0)        {
            SOC_SAND_SET_ERROR_CODE(PCP_MGMT_PCP_MEMORY_TEST_FAILED_ERR, 190, exit);
        }
    }

    /* Clear all pcp tbls after memory test */
    res = pcp_mgmt_tbls_init(
          unit,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_mgmt_memory_test_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     pcp_api_mgmt module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_mgmt_get_procs_ptr(
  )
{
  return Pcp_procedure_desc_element_mgmt;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     pcp_api_mgmt module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  pcp_mgmt_get_errs_ptr(
  )
{
  return Pcp_error_desc_element_mgmt;
}
uint32
  PCP_MGMT_ELK_OP_MODE_verify(
    SOC_SAND_IN  PCP_MGMT_ELK_OP_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mode, PCP_MGMT_MODE_MAX, PCP_MGMT_MODE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_MGMT_ELK_OP_MODE_verify()",0,0);
}

uint32
  PCP_DMA_THRESHOLD_verify(
    SOC_SAND_IN  PCP_DMA_THRESHOLD *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->fc, PCP_MGMT_FC_MAX, PCP_MGMT_FC_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->drop, PCP_MGMT_DROP_MAX, PCP_MGMT_DROP_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_DMA_THRESHOLD_verify()",0,0);
}

uint32
  PCP_DMA_TX_OP_MODE_verify(
    SOC_SAND_IN  PCP_DMA_TX_OP_MODE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->head_ptr, PCP_MGMT_HEAD_PTR_MAX, PCP_MGMT_HEAD_PTR_OUT_OF_RANGE_ERR, 11, exit);
  PCP_STRUCT_VERIFY(PCP_DMA_THRESHOLD, &(info->thresholds), 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_DMA_TX_OP_MODE_verify()",0,0);
}

uint32
  PCP_DMA_RX_OP_MODE_verify(
    SOC_SAND_IN  PCP_DMA_RX_OP_MODE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_SAND_PP_NOF_TC; ++ind)
  {
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->buff_size, PCP_MGMT_BUFF_SIZE_MAX, PCP_MGMT_BUFF_SIZE_OUT_OF_RANGE_ERR, 12, exit);
  PCP_STRUCT_VERIFY(PCP_DMA_THRESHOLD, &(info->thresholds), 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cal_len, PCP_MGMT_CAL_LEN_MAX, PCP_MGMT_CAL_LEN_OUT_OF_RANGE_ERR, 14, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cal_m, PCP_MGMT_CAL_M_MAX, PCP_MGMT_CAL_M_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_DMA_RX_OP_MODE_verify()",0,0);
}

uint32
  PCP_DMA_OP_MODE_verify(
    SOC_SAND_IN  PCP_DMA_OP_MODE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_DMA_TX_OP_MODE, &(info->tx), 11, exit);
  PCP_STRUCT_VERIFY(PCP_DMA_RX_OP_MODE, &(info->rx_mode), 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_DMA_OP_MODE_verify()",0,0);
}

uint32
  PCP_MGMT_OAM_OP_MODE_verify(
    SOC_SAND_IN  PCP_MGMT_OAM_OP_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_MGMT_OAM_OP_MODE_verify()",0,0);
}

uint32
  PCP_MGMT_STS_OP_MODE_verify(
    SOC_SAND_IN  PCP_MGMT_STS_OP_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->conf_word, PCP_MGMT_CONF_WORD_MAX, PCP_MGMT_CONF_WORD_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_MGMT_STS_OP_MODE_verify()",0,0);
}

uint32
  PCP_MGMT_OP_MODE_verify(
    SOC_SAND_IN  PCP_MGMT_OP_MODE *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_MGMT_ELK_OP_MODE, &(info->elk), 10, exit);
  PCP_STRUCT_VERIFY(PCP_DMA_OP_MODE, &(info->dma), 11, exit);
  PCP_STRUCT_VERIFY(PCP_MGMT_OAM_OP_MODE, &(info->oam), 12, exit);
  PCP_STRUCT_VERIFY(PCP_MGMT_STS_OP_MODE, &(info->sts), 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_MGMT_OP_MODE_verify()",0,0);
}

uint32
  PCP_MGMT_HW_IF_DRAM_INFO_verify(
    SOC_SAND_IN  PCP_MGMT_HW_IF_DRAM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->size_mbit, PCP_MGMT_SIZE_MBIT_MAX, PCP_MGMT_SIZE_MBIT_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_MGMT_HW_IF_DRAM_INFO_verify()",0,0);
}

uint32
  PCP_MGMT_HW_IF_QDR_INFO_verify(
    SOC_SAND_IN  PCP_MGMT_HW_IF_QDR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_MGMT_HW_IF_QDR_INFO_verify()",0,0);
}

uint32
  PCP_MGMT_HW_IF_SERDES_INFO_verify(
    SOC_SAND_IN  PCP_MGMT_HW_IF_SERDES_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->rx_eq_ctrl, PCP_MGMT_RX_EQ_CTRL_MAX, PCP_MGMT_RX_EQ_CTRL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->rx_eq_dc_gain, PCP_MGMT_RX_EQ_DC_GAIN_MAX, PCP_MGMT_RX_EQ_DC_GAIN_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tx_preemp_0t, PCP_MGMT_TX_PREEMP_0T_MAX, PCP_MGMT_TX_PREEMP_0T_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tx_preemp_1t, PCP_MGMT_TX_PREEMP_1T_MAX, PCP_MGMT_TX_PREEMP_1T_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tx_preemp_2t, PCP_MGMT_TX_PREEMP_2T_MAX, PCP_MGMT_TX_PREEMP_2T_OUT_OF_RANGE_ERR, 14, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->tx_vodctrl, PCP_MGMT_TX_VODCTRL_MAX, PCP_MGMT_TX_VODCTRL_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_MGMT_HW_IF_SERDES_INFO_verify()",0,0);
}

uint32
  PCP_MGMT_HW_IF_ELK_IF_INFO_verify(
    SOC_SAND_IN  PCP_MGMT_HW_IF_ELK_IF_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_MGMT_HW_IF_SERDES_INFO, &(info->serdes_info), 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_MGMT_HW_IF_ELK_IF_INFO_verify()",0,0);
}

uint32
  PCP_MGMT_HW_IF_PCI_INFO_verify(
    SOC_SAND_IN  PCP_MGMT_HW_IF_PCI_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->burst_size, PCP_MGMT_BURST_SIZE_MAX, PCP_MGMT_BURST_SIZE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_MGMT_HW_IF_PCI_INFO_verify()",0,0);
}

uint32
  PCP_HW_ADJUSTMENTS_verify(
    SOC_SAND_IN  PCP_HW_ADJUSTMENTS *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  PCP_STRUCT_VERIFY(PCP_MGMT_HW_IF_PCI_INFO, &(info->pci), 10, exit);
  PCP_STRUCT_VERIFY(PCP_MGMT_HW_IF_DRAM_INFO, &(info->dram), 11, exit);
  PCP_STRUCT_VERIFY(PCP_MGMT_HW_IF_QDR_INFO, &(info->qdr), 12, exit);
  PCP_STRUCT_VERIFY(PCP_MGMT_HW_IF_ELK_IF_INFO, &(info->elk_if), 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in PCP_HW_ADJUSTMENTS_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

