#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_flow_control.c,v 1.59 Broadcom SDK $
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
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FLOWCONTROL

/*************
 * INCLUDES  *
 *************/
#include <soc/mem.h>
/* { */

#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_flow_control.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_db.h>
#include <soc/dpp/ARAD/arad_pmf_pgm_mgmt.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/TMC/tmc_api_egr_queuing.h>
#include <soc/dpp/port_sw_db.h>

#include <soc/mcm/allenum.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_ILKN_NDX_MIN                                        (ARAD_NIF_ILKN_ID_A)
#define ARAD_ILKN_NDX_MAX                                        (ARAD_NIF_ILKN_ID_B)
#define ARAD_FC_CLASS_MAX                                        7
#define ARAD_CAL_MODE_NDX_MAX                                    (SOC_TMC_FC_NOF_CAL_MODES-1)

#define ARAD_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_FLDS 3

#define ARAD_FC_REC_CAL_DEST_ARR_SIZE   7
#define ARAD_FC_GEN_CAL_SRC_ARR_SIZE    8

#define ARAD_FC_PFC_GENERIC_BITMAP_SIZE SOC_TMC_FC_PFC_GENERIC_BITMAP_SIZE
#define ARAD_FC_HCFC_BITMAPS 8

/* Retransmit values for RX/TX calendars */
#define ARAD_FC_RETRANSMIT_DEST_ILKN_A_VAL 0x0 
#define ARAD_FC_RETRANSMIT_DEST_ILKN_B_VAL 0x2 

/* ILKN Multi-use TX CAL */
#define ARAD_FC_ILKN_MUB_TX_CAL_SOURCE_ENTRY_OFFSET 9


#if defined(BCM_88650_B0)
#define ARAD_FC_ILKN_RETRANSMIT_CAL_LEN_MIN 1
#define ARAD_FC_ILKN_RETRANSMIT_CAL_LEN_MAX 2
#endif

/* Ardon FCV (flow control vector) input bit length */
#define ARDON_FC_FCV_BIT_LEN 256


#define FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_MAX               SOC_TMC_EGR_NOF_Q_PRIO_ARAD
#define FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD              4

/* } */
/*************
 * MACROS    *
 *************/
/* { */
#define ARAD_FC_CAL_MODE_IS_ILKN(c_mode) \
  SOC_SAND_NUM2BOOL((c_mode == SOC_TMC_FC_CAL_MODE_ILKN_INBND) || (c_mode ==SOC_TMC_FC_CAL_MODE_ILKN_OOB))

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
static SOC_TMC_FC_GEN_CAL_SRC 
  arad_fc_gen_cal_src_arr[ARAD_FC_GEN_CAL_SRC_ARR_SIZE] = 
    { SOC_TMC_FC_GEN_CAL_SRC_STE, 
      SOC_TMC_FC_GEN_CAL_SRC_LLFC_VSQ,
      SOC_TMC_FC_GEN_CAL_SRC_PFC_VSQ,
      SOC_TMC_FC_GEN_CAL_SRC_GLB_RCS,
      SOC_TMC_FC_GEN_CAL_SRC_HCFC,
      SOC_TMC_FC_GEN_CAL_SRC_LLFC,
      SOC_TMC_FC_GEN_CAL_SRC_RETRANSMIT,
      SOC_TMC_FC_GEN_CAL_SRC_CONST
    };
static SOC_TMC_FC_REC_CAL_DEST 
  arad_fc_rec_cal_dest_arr[ARAD_FC_REC_CAL_DEST_ARR_SIZE] = 
    { SOC_TMC_FC_REC_CAL_DEST_PFC, 
      SOC_TMC_FC_REC_CAL_DEST_NIF_LL,
      SOC_TMC_FC_REC_CAL_DEST_PORT_2_PRIORITY,
      SOC_TMC_FC_REC_CAL_DEST_PORT_8_PRIORITY,
      SOC_TMC_FC_REC_CAL_DEST_GENERIC_PFC,
      SOC_TMC_FC_REC_CAL_DEST_RETRANSMIT,
      SOC_TMC_FC_REC_CAL_DEST_NONE
    };

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
* NAME:
*     arad_flow_control_regs_init
* FUNCTION:
*   Initialization of the Arad blocks configured in this module.
*   This function directly accesses registers/tables for
*   initializations that are not covered by API-s
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
STATIC uint32
  arad_flow_control_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_TMC_FC_ILKN_MUB_GEN_CAL
    ilkn_cal;
  uint32
    i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FLOW_CONTROL_REGS_INIT);

  /* CFC Enablers */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, EGQ_TO_SCH_DEVICE_ENf,  0x1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  23,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, EGQ_TO_SCH_IF_ENf,  0x1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  24,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, EGQ_TO_SCH_PFC_ENf,  0x1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  26,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, EGQ_TO_SCH_ERP_ENf,  0x1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  27,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, EGQ_TO_SCH_ERP_TC_ENf,  0x1));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, GLB_RSC_TO_HCFC_HP_CFGf,  0x0));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  31,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, GLB_RSC_TO_HCFC_LP_CFGf,  0x0));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  32,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, GLB_RSC_TO_EGQ_RCL_PFC_ENf,  0x0));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  32,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, GLB_RSC_TO_RCL_PFC_HP_CFGf,  0x0));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  32,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, GLB_RSC_TO_RCL_PFC_LP_CFGf,  0x0));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, OOB_RX_0_RSTNf,  0x1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  41,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, OOB_RX_1_RSTNf,  0x1));

  /* Clear Calendars  - ILKN */
  res = arad_fc_clear_calendar_unsafe(unit, SOC_TMC_FC_CAL_MODE_ILKN_INBND, ARAD_FC_CAL_TYPE_RX, ARAD_FC_ILKN_ID_A);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  res = arad_fc_clear_calendar_unsafe(unit, SOC_TMC_FC_CAL_MODE_ILKN_INBND, ARAD_FC_CAL_TYPE_TX, ARAD_FC_ILKN_ID_A);
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);
  res = arad_fc_clear_calendar_unsafe(unit, SOC_TMC_FC_CAL_MODE_ILKN_INBND, ARAD_FC_CAL_TYPE_RX, ARAD_FC_ILKN_ID_B);
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
  res = arad_fc_clear_calendar_unsafe(unit, SOC_TMC_FC_CAL_MODE_ILKN_INBND, ARAD_FC_CAL_TYPE_TX, ARAD_FC_ILKN_ID_B);
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  /* Clear Calendars  - SPI OOB */
  res = arad_fc_clear_calendar_unsafe(unit, SOC_TMC_FC_CAL_MODE_SPI_OOB, ARAD_FC_CAL_TYPE_RX, SOC_TMC_FC_OOB_ID_A);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  res = arad_fc_clear_calendar_unsafe(unit, SOC_TMC_FC_CAL_MODE_SPI_OOB, ARAD_FC_CAL_TYPE_TX, SOC_TMC_FC_OOB_ID_A);
  SOC_SAND_CHECK_FUNC_RESULT(res, 61, exit);
  res = arad_fc_clear_calendar_unsafe(unit, SOC_TMC_FC_CAL_MODE_SPI_OOB, ARAD_FC_CAL_TYPE_RX, SOC_TMC_FC_OOB_ID_B);
  SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);
  res = arad_fc_clear_calendar_unsafe(unit, SOC_TMC_FC_CAL_MODE_SPI_OOB, ARAD_FC_CAL_TYPE_TX, SOC_TMC_FC_OOB_ID_B);
  SOC_SAND_CHECK_FUNC_RESULT(res, 63, exit);

  /* Init the ILKN MUB TX calendar */
  for(i = 0; i < SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN; i++)
  {
    ilkn_cal.entries[i].source = SOC_TMC_FC_GEN_CAL_SRC_CONST;
    ilkn_cal.entries[i].id = 0;
  }
  res = arad_fc_ilkn_mub_gen_cal_set_unsafe(unit, ARAD_NIF_ILKN_ID_A, &ilkn_cal);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  res = arad_fc_ilkn_mub_gen_cal_set_unsafe(unit, ARAD_NIF_ILKN_ID_B, &ilkn_cal);
  SOC_SAND_CHECK_FUNC_RESULT(res, 71, exit);

  /*IPT FC defaults*/
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, GFMC_FC_MAPf,    0x8));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  81,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_2_FC_MAPf,  0xc));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  82,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_1_FC_MAPf,  0xe));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  83,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_IPT_FLOW_CONTROL_CONFIGURATIONr, SOC_CORE_ALL, 0, BFMC_0_FC_MAPf,  0xf));

#if defined(BCM_88650_B0)
  /* Init the ILKN retransmit calendar                  */
  /* Set the first entry to handle same interface       */
  /* Set the second entry to handle the other interface */
  if(SOC_IS_ARAD_B0_AND_ABOVE(unit))
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_TX_RT_CAL_CFGr, REG_PORT_ANY, 0, ILKN_OOB_TX_0_RT_CAL_INDEX_0f,  0x0));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  91,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_TX_RT_CAL_CFGr, REG_PORT_ANY, 0, ILKN_OOB_TX_0_RT_CAL_INDEX_1f,  0x1));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  92,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_TX_RT_CAL_CFGr, REG_PORT_ANY, 0, ILKN_OOB_TX_1_RT_CAL_INDEX_0f,  0x1));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  93,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_TX_RT_CAL_CFGr, REG_PORT_ANY, 0, ILKN_OOB_TX_1_RT_CAL_INDEX_1f,  0x0));
  }


#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_flow_control_regs_init()",0,0);
}

int arad_fc_shr_mapping(int unit, int fcv_bit, int cl_index, int select, int valid)
{
    int rv = SOC_E_NONE;
    uint32
        entry_offset,
        field_offset;
    uint32
        shr_data[2],
        index_field[4] = {INDEX_0f, INDEX_1f, INDEX_2f, INDEX_3f},
        select_field[4] = {SELECT_0f, SELECT_1f, SELECT_2f, SELECT_3f},
        valid_field[4] = {VALID_0f, VALID_1f, VALID_2f, VALID_3f};
  
    SOCDNX_INIT_FUNC_DEFS;

    if (fcv_bit >= ARDON_FC_FCV_BIT_LEN) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("fcv_bit=%d is out of range [0..%d]"), fcv_bit, ARDON_FC_FCV_BIT_LEN - 1)); 
    }

    entry_offset = fcv_bit / 4;
    field_offset = fcv_bit % 4;

    rv = soc_mem_read(unit, CFC_NIF_SHR_MAPm, MEM_BLOCK_ANY, entry_offset, shr_data);
    SOCDNX_IF_ERR_EXIT(rv);

    soc_mem_field32_set(unit, CFC_NIF_SHR_MAPm, shr_data, index_field[field_offset], cl_index);
    soc_mem_field32_set(unit, CFC_NIF_SHR_MAPm, shr_data, select_field[field_offset], select);
    soc_mem_field32_set(unit, CFC_NIF_SHR_MAPm, shr_data, valid_field[field_offset], valid);

    rv = soc_mem_write(unit, CFC_NIF_SHR_MAPm, MEM_BLOCK_ANY, entry_offset, shr_data);
    SOCDNX_IF_ERR_EXIT(rv);
  
exit:
    SOCDNX_FUNC_RETURN;
}

int arad_fc_init_shr_mapping(int unit)
{
    int rv = SOC_E_NONE;
    uint32
        i,
        entry_offset;
    uint32
        shr_data[2],
        index_field[4] = {INDEX_0f, INDEX_1f, INDEX_2f, INDEX_3f},
        select_field[4] = {SELECT_0f, SELECT_1f, SELECT_2f, SELECT_3f},
        valid_field[4] = {VALID_0f, VALID_1f, VALID_2f, VALID_3f};
  
    SOCDNX_INIT_FUNC_DEFS;

    /* Default FCV/SHR mapping */
    for(entry_offset = 0; entry_offset < ARDON_FC_FCV_BIT_LEN / 4; entry_offset++)   {

        rv = soc_mem_read(unit, CFC_NIF_SHR_MAPm, MEM_BLOCK_ANY, entry_offset, shr_data);
        SOCDNX_IF_ERR_EXIT(rv);

        for(i = 0; i < 4; i++)  {
              soc_mem_field32_set(unit, CFC_NIF_SHR_MAPm, shr_data, index_field[i], (entry_offset * 4) + i);
              soc_mem_field32_set(unit, CFC_NIF_SHR_MAPm, shr_data, select_field[i], 0);
              soc_mem_field32_set(unit, CFC_NIF_SHR_MAPm, shr_data, valid_field[i], 1);
        }

        rv = soc_mem_write(unit, CFC_NIF_SHR_MAPm, MEM_BLOCK_ANY, entry_offset, shr_data);
        SOCDNX_IF_ERR_EXIT(rv);
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_fc_init_pfc_mapping_unsafe(
      SOC_SAND_IN int  unit
    )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    nif_pfc_data[2],
    nif_pfc_data_2[2],
    *data_select,
    entry_offset,
    base_queue_pair,
    nof_priorities,
    i,
    interface_i,
    valid;
  ARAD_PORT2IF_MAPPING_INFO
    interace_info_in;
  uint32
    index_field[4] = {INDEX_0f, INDEX_1f, INDEX_2f, INDEX_3f};
  uint32
    select_field[4] = {SELECT_0f, SELECT_1f, SELECT_2f, SELECT_3f};
  uint32
    valid_field[4] = {VALID_0f, VALID_1f, VALID_2f, VALID_3f};
  soc_pbmp_t
    pbmp;
  soc_port_t
    logical_port;
  uint32
    tm_port;
  int 
    core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_INIT_PFC_MAPPING_UNSAFE);

  /* Default PFC/TC mapping */
  res = soc_port_sw_db_valid_ports_get(unit, 0, &pbmp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_PBMP_ITER(pbmp, logical_port)
  {
    res = soc_port_sw_db_local_to_tm_port_get(unit, logical_port, &tm_port, &core);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10 + logical_port, exit);

    res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, tm_port, core, &base_queue_pair);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1000 + logical_port, exit);

    res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, tm_port, core, &nof_priorities);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2000 + logical_port, exit);

    res = arad_port_to_interface_map_get(unit, core, tm_port, &interace_info_in.if_id, &interace_info_in.channel_id);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3000 + logical_port, exit);

    interface_i = arad_nif2intern_id(unit, interace_info_in.if_id);
    if(interace_info_in.if_id == SOC_TMC_NIF_ID_NONE || interace_info_in.if_id == SOC_TMC_IF_ID_CPU ||
       interace_info_in.if_id == SOC_TMC_IF_ID_OLP || interace_info_in.if_id == SOC_TMC_IF_ID_RCY ||
       interace_info_in.if_id == SOC_TMC_IF_ID_ERP || interace_info_in.if_id == SOC_TMC_IF_ID_OAMP) {
      continue;
    }

    if (SOC_IS_ARDON(unit)) {
        /* If FCV is for CL SCH than disable EGQ PFC (HR) */
        valid = SOC_DPP_CONFIG(unit)->arad->init.fc.cl_sch_enable ? 0x0 : 0x1;
    } else {
        valid = 0x1;
    }


    entry_offset = ((interface_i * SOC_TMC_EGR_NOF_Q_PRIO_ARAD) / 4);
    if(entry_offset > 64) {
      /* Protection against miss-configured interfaces */
      continue;
    }

    res = soc_mem_read(
              unit,
              CFC_NIF_PFC_MAPm,
              MEM_BLOCK_ANY,
              entry_offset,
              nif_pfc_data
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 4000 + logical_port, exit);

    if(nof_priorities > 4)
    {
      res = soc_mem_read(
              unit,
              CFC_NIF_PFC_MAPm,
              MEM_BLOCK_ANY,
              entry_offset + 1,
              nif_pfc_data_2
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 5000 + logical_port, exit);
    }

    for(i = 0; i < nof_priorities; i++)
    {
      data_select = (i < 4) ? nif_pfc_data : nif_pfc_data_2;
      
      soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, data_select, index_field[i % 4], base_queue_pair + i);
      soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, data_select, select_field[i % 4], 0);
      soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, data_select, valid_field[i % 4], valid);
    }

    res = soc_mem_write(
              unit,
              CFC_NIF_PFC_MAPm,
              MEM_BLOCK_ANY,
              entry_offset,
              nif_pfc_data
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 6000 + logical_port, exit);

    if(nof_priorities > 4)
    {
      res = soc_mem_write(
              unit,
              CFC_NIF_PFC_MAPm,
              MEM_BLOCK_ANY,
              entry_offset + 1,
              nif_pfc_data_2
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 7000 + logical_port, exit);
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_flow_control_init_pfc_mapping()",0,0);
}

uint32
  arad_flow_control_init_oob_rx(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_INIT_FC           init_fc
  )
{
  uint32
    res = SOC_SAND_OK,
    interface_number,
    fld_val,
    fld;
  uint32
    per_rep_idx,
    rep_idx,
    entry_idx,
    cal_length = 0,
    cal_reps = 0;
  ARAD_CFC_CALRX_TBL_DATA
    rx_cal;
  uint64
    field64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FLOW_CONTROL_INIT_OOB_RX);

  for(interface_number = 0; interface_number < SOC_TMC_FC_NOF_OOB_IDS; interface_number++)
  {
    if(init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_NONE)
      continue;

    /* Handles RX */
    if(init_fc.fc_directions[interface_number] & SOC_SAND_BIT(SOC_TMC_CONNECTION_DIRECTION_RX))
    {
      /* Put the OOB interfaces into Reset */
      fld_val = 0x0;
      fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_RX_0_RSTNf : OOB_RX_1_RSTNf);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val));

      /* Disable the FC before configuring it */
      fld_val = 0x0;
      fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_RX_0_ENf : ILKN_RX_1_ENf);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val));
      fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_ENf : SPI_OOB_RX_1_ENf);
      COMPILER_64_SET(field64,0,fld_val);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  33,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, fld,  field64));

      /* Configuration start */

      /*Disable flow control on port fault*/
      fld_val = 0x0;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  34,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_OOB_RX_0_ERR_FC_STATUS_SELf,  fld_val));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  35,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_OOB_RX_1_ERR_FC_STATUS_SELf,  fld_val));

      /*  Enable/disable */
      fld_val = (init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN ? 1 : 0);
      if(interface_number == SOC_TMC_FC_OOB_ID_A)
      {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  37,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, OOB_RX_0_MODEf,  fld_val));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  38,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_RX_0_SELf,  fld_val));
        if(init_fc.fc_oob_type[interface_number] != SOC_TMC_FC_CAL_TYPE_ILKN) {
            fld_val = (init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_HCFC ? 1 : 0);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  39,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_CONFIGURATIONr, REG_PORT_ANY, 0, SPI_0_OOB_MODEf,  fld_val));
        }
      }
      else
      {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  43,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, OOB_RX_1_MODEf,  fld_val));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  44,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_RX_1_SELf,  fld_val));
        if(init_fc.fc_oob_type[interface_number] != SOC_TMC_FC_CAL_TYPE_ILKN) {
            fld_val = (init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_HCFC ? 1 : 0);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  46,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_CONFIGURATIONr, REG_PORT_ANY, 0, SPI_1_OOB_MODEf,  fld_val));
        }
      }

      /* Calendar length (single repetition) */
      cal_length = init_fc.fc_oob_calender_length[interface_number][SOC_TMC_CONNECTION_DIRECTION_RX];
      if(init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN)
      {
        fld = (interface_number == ARAD_FC_ILKN_ID_A ? ILKN_OOB_RX_0_CAL_LENf : ILKN_OOB_RX_1_CAL_LENf);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  cal_length - 1));
      }
      else
      {
        fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_CAL_LENf : SPI_OOB_RX_1_CAL_LENf);
        COMPILER_64_SET(field64, 0, cal_length);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, fld,  field64));
      } 

      /*  Calendar number of repetitions - SPI/OOB Only */
      cal_reps = init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN ? 1 : 
        init_fc.fc_oob_calender_rep_count[interface_number][SOC_TMC_CONNECTION_DIRECTION_RX];
      fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_CAL_Mf : SPI_OOB_RX_1_CAL_Mf);
      COMPILER_64_SET(field64,0,cal_reps);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, fld,  field64));

      for (per_rep_idx = 0; per_rep_idx < cal_length; per_rep_idx++)
      {
        rx_cal.fc_index = 0;
        rx_cal.fc_dest_sel = arad_fc_rec_cal_dest_type_to_val_internal(SOC_TMC_FC_REC_CAL_DEST_NONE);       

        for (rep_idx = 0; rep_idx < cal_reps; rep_idx++)
        {
          entry_idx = (rep_idx * cal_length) + per_rep_idx;
          if(init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN)
          {
            res = arad_cfc_ilkn_calrx_tbl_set_unsafe(unit, interface_number, entry_idx, &rx_cal);
          }
          else
          {
            res = arad_cfc_oob_calrx_tbl_set_unsafe(unit, interface_number, entry_idx, &rx_cal);
          }
          SOC_SAND_CHECK_FUNC_RESULT(res, 100 + entry_idx, exit);
        }
      }

      /* Configuration end */

      /* Take the OOB/ILKN interfaces out of Reset */
      fld_val = 0x1;
      fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_RX_0_RSTNf : OOB_RX_1_RSTNf);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val));

      /* Enable the FC after configuring it */
      fld_val = 0x1;
      if(init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN)
      {
        fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_RX_0_ENf : ILKN_RX_1_ENf);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val));
      }
      if(init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_SPI || init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_HCFC)
      {
        fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_ENf : SPI_OOB_RX_1_ENf);
        COMPILER_64_SET(field64,0,fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, fld,  field64));
      }

    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_flow_control_init_oob_rx()",0,0);
}

uint32
  arad_flow_control_init_oob_tx(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_INIT_FC           init_fc
  )
{
  uint32
    res = SOC_SAND_OK,
    interface_number,
    fld_val,
    fld;
  uint32
    per_rep_idx,
    rep_idx,
    entry_idx,
    cal_length = 0,
    cal_reps = 0;
  ARAD_CFC_CALTX_TBL_DATA
    tx_cal;
  uint64
    field64;
  uint32 interfcae_status_oob_ignore;
  soc_pbmp_t phy_ports;
  uint32 clp0_lanes;
  uint32 clp1_lanes;
  uint32 lanes_bitmap;
  int lanes_bitmap_length;
  uint64 reg_val64;
  soc_field_t intf_f[] = {FRC_ILKN_OOB_TX_0_INTFf, FRC_ILKN_OOB_TX_1_INTFf}, 
    lanes_f[] = {FRC_ILKN_OOB_TX_0_LANESf, FRC_ILKN_OOB_TX_1_LANESf};


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FLOW_CONTROL_INIT_OOB_TX);
  
  for(interface_number = 0; interface_number < SOC_TMC_FC_NOF_OOB_IDS; interface_number++)
  {
    interfcae_status_oob_ignore = soc_property_port_get(unit, interface_number, spn_ILKN_INTERFACE_STATUS_OOB_IGNORE, 0);;
    soc_arad_ilkn_phy_ports_btmp_set(unit, interface_number, &phy_ports);

    /*
     *Status msgs through out-of-band interface 
     */
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR, READ_CFC_ILKN_OOB_TX_FRCr(unit, &reg_val64));
    lanes_bitmap = 0;
    lanes_bitmap_length = soc_reg_field_length(unit, CFC_ILKN_OOB_TX_FRCr, FRC_ILKN_OOB_TX_1_LANESf);
    clp0_lanes = clp1_lanes = 0;
    SHR_BITSET_RANGE(&lanes_bitmap, 0, lanes_bitmap_length); /*Default status per lane is forced*/
      
    if(interfcae_status_oob_ignore) {
      /*Do not send error status messages - force port up*/
      soc_reg64_field32_set(unit, CFC_ILKN_OOB_TX_FRCr, &reg_val64, intf_f[interface_number], 1);
      soc_reg64_field32_set(unit, CFC_ILKN_OOB_TX_FRCr, &reg_val64, lanes_f[interface_number], lanes_bitmap /*force all links in the bitmap*/);
    } else {
      /*Send error messgaes if required*/
      SHR_BITCOPY_RANGE(&clp0_lanes, 0, &SOC_PBMP_WORD_GET(phy_ports, 0), ARAD_NIF_CLP0_PORT_MIN+1, ARAD_NIF_CLP_NOF_PORTS);
      SHR_BITCOPY_RANGE(&clp1_lanes, 0, &SOC_PBMP_WORD_GET(phy_ports, 0), ARAD_NIF_CLP1_PORT_MIN+1, ARAD_NIF_CLP_NOF_PORTS);
      lanes_bitmap = lanes_bitmap & ( ~ (clp0_lanes | (clp1_lanes << 12)));/*clear just the links which supports error status messages*/
      soc_reg64_field32_set(unit, CFC_ILKN_OOB_TX_FRCr, &reg_val64, intf_f[interface_number], 0);
      soc_reg64_field32_set(unit, CFC_ILKN_OOB_TX_FRCr, &reg_val64, lanes_f[interface_number], lanes_bitmap);
    }
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR, WRITE_CFC_ILKN_OOB_TX_FRCr(unit, reg_val64));

    if(init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_NONE)
      continue;

    /* Handles TX */
    if(init_fc.fc_directions[interface_number] & SOC_SAND_BIT(SOC_TMC_CONNECTION_DIRECTION_TX))
    {
      /* Put the OOB interfaces into Reset */
      fld_val = 0x0;
      fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_TX_0_RSTNf : OOB_TX_1_RSTNf);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val));

      /* Disable the FC before configuring it */
      fld_val = 0x0;
      fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_TX_0_ENf : ILKN_TX_1_ENf);
      COMPILER_64_SET(field64,0,fld_val);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  field64));
      fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_ENf : SPI_OOB_TX_1_ENf);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val));

      /* Configuration start */
      /* oob type */
      fld_val = (init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN ? 1 : 0);
      COMPILER_64_SET(field64,0,fld_val);
      if(interface_number == SOC_TMC_FC_OOB_ID_A)
      {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  41,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, OOB_TX_0_MODEf,  fld_val));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  42,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_TX_0_SELf,  field64));
      }
      else
      {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  43,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, OOB_TX_1_MODEf,  fld_val));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  44,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_TX_1_SELf,  field64));
      }

      /* Calendar length (single repetition) */
      cal_length = init_fc.fc_oob_calender_length[interface_number][SOC_TMC_CONNECTION_DIRECTION_TX];
      if(init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN)
      {
        fld = (interface_number == ARAD_FC_ILKN_ID_A ? ILKN_TX_0_CAL_LENf : ILKN_TX_1_CAL_LENf);
        COMPILER_64_SET(field64,0,cal_length - 1);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  field64));
      }
      else
      { 
        fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_CAL_LENf : SPI_OOB_TX_1_CAL_LENf);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  cal_length));
      } 

      /*  Calendar number of repetitions - SPI/OOB Only */
      cal_reps = init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN ? 
                            1 : 
                            init_fc.fc_oob_calender_rep_count[interface_number][SOC_TMC_CONNECTION_DIRECTION_TX];
      fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_CAL_Mf : SPI_OOB_TX_1_CAL_Mf);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  cal_reps));

      for (per_rep_idx = 0; per_rep_idx < cal_length; per_rep_idx++)
      {
        tx_cal.fc_index = 0;
        tx_cal.fc_source_sel = arad_fc_gen_cal_src_type_to_val_internal(SOC_TMC_FC_GEN_CAL_SRC_CONST);       

        for (rep_idx = 0; rep_idx < cal_reps; rep_idx++)
        {
          entry_idx = (rep_idx * cal_length) + per_rep_idx;
          if(init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN)
          {
            res = arad_cfc_ilkn_caltx_tbl_set_unsafe(unit, interface_number, entry_idx, &tx_cal);
          }
          else
          {
            res = arad_cfc_oob_caltx_tbl_set_unsafe(unit, interface_number, entry_idx, &tx_cal);
          }
          SOC_SAND_CHECK_FUNC_RESULT(res, 100 + entry_idx, exit);
        }
      }

      /* Configuration end */

      /* Take the OOB interface out of Reset */
      fld_val = 0x1;
      fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? OOB_TX_0_RSTNf : OOB_TX_1_RSTNf);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val));

      /* Enable the FC after configuring it */
      fld_val = 0x1;
      if(init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_ILKN)
      {
        fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? ILKN_TX_0_ENf : ILKN_TX_1_ENf);
        COMPILER_64_SET(field64,0,fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  field64));
      }
      if(init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_SPI || init_fc.fc_oob_type[interface_number] == SOC_TMC_FC_CAL_TYPE_HCFC)
          
      {
        fld = (interface_number == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_ENf : SPI_OOB_TX_1_ENf);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_TX_CONFIGURATIONr, REG_PORT_ANY, 0, fld,  fld_val));
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_flow_control_init_oob_tx()",0,0);
}


int aradon_flow_control_init_cl_sch(int unit, int enable)
{
    uint32
        reg32_val,
        field32_val;
     
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, &reg32_val));
    field32_val = enable;
    soc_reg_field_set(unit, CFC_CFC_ENABLERSr, &reg32_val, NIF_TO_SCH_SHR_ENBf, field32_val);
    field32_val = 0x1; /* Needed for Ardon FC generation - Enable IQM DB to NIF LLFC */
    soc_reg_field_set(unit, CFC_CFC_ENABLERSr, &reg32_val, FRC_NIF_LLVL_ENf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, reg32_val));


    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, SCH_REG_1BCr, REG_PORT_ANY, 0, &reg32_val)); /* OOB-FC Configuration Register */
    field32_val = enable;
    soc_reg_field_set(unit, SCH_REG_1BCr, &reg32_val, FIELD_0_0f, field32_val); /* OobFcEnable */
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, SCH_REG_1BCr, REG_PORT_ANY, 0, reg32_val));

    field32_val = enable ? 255 : 0;
    soc_reg_field_set(unit, SCH_REG_1BCr, &reg32_val, FIELD_4_15f, field32_val); /* OobFcSchedulersRange */

    field32_val = enable ? 0 : 1;
    soc_reg_field_set(unit, SCH_REG_1BCr, &reg32_val, FIELD_28_28f, field32_val); /* OobFcClear */
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, SCH_REG_1BCr, REG_PORT_ANY, 0, reg32_val));

    /* Flow Control generation in Packet Boundry */
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBI_ENABLE_PKT_BOUNDARY_LLFCr, REG_PORT_ANY, 0, &reg32_val));
    field32_val = 0x1;
    soc_reg_field_set(unit, NBI_ENABLE_PKT_BOUNDARY_LLFCr, &reg32_val, ENABLE_PKT_BOUNDARY_LLFCf, field32_val);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBI_ENABLE_PKT_BOUNDARY_LLFCr, REG_PORT_ANY, 0, reg32_val));
  
exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
* NAME:
*     arad_flow_control_init
* FUNCTION:
*     Initialization of the Arad blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  arad_flow_control_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_INIT_FC           init_fc
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FLOW_CONTROL_INIT);

  res = arad_flow_control_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, OOB_TX_0_OUT_SPEEDf,  init_fc.oob_tx_speed[SOC_TMC_FC_OOB_ID_A]));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, OOB_TX_1_OUT_SPEEDf,  init_fc.oob_tx_speed[SOC_TMC_FC_OOB_ID_B]));
  
  res = arad_flow_control_init_oob_rx(unit, init_fc);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_flow_control_init_oob_tx(unit, init_fc);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (SOC_IS_ARDON(unit)) {
        res = aradon_flow_control_init_cl_sch(unit, init_fc.cl_sch_enable);
        if (res != SOC_E_NONE) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 35, exit);
        }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_flow_control_init()",0,0);
}

STATIC uint32
  arad_fc_inbnd_max_nof_priorities_get(
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx
  )
{
  switch(arad_nif_id2type(nif_ndx))
  {
  case ARAD_NIF_TYPE_SGMII:
    return 2;
  case ARAD_NIF_TYPE_XAUI:
    return 8;
  case ARAD_NIF_TYPE_RXAUI:
    return 8;
  default:
    return 0;
  }
}

STATIC uint32
  arad_fc_gen_inbnd_pfc_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_TMC_FC_GEN_INBND_PFC             *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset = arad_nif2intern_id(unit, nif_ndx),
    reg_offset,
    bit_offset;
  uint32
    idx,
    tm_port;
  ARAD_PORT2IF_MAPPING_INFO
    out_mapping;
  ARAD_EGQ_PPCT_TBL_DATA
    data;
  soc_reg_above_64_val_t  
    pfc_map_reg;
  soc_pbmp_t
    ports_bm;
  int
    core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_PFC_SET_UNSAFE);
  SOC_REG_ABOVE_64_CLEAR(pfc_map_reg);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_PBMP_ITER(ports_bm, idx)
  {
    res = soc_port_sw_db_local_to_tm_port_get(unit, idx, &tm_port, &core);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 7, exit);

    res = arad_port_to_interface_map_get(
            unit,
            core,
            tm_port,
            &out_mapping.if_id,
            &out_mapping.channel_id
          );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    if (out_mapping.if_id == nif_ndx)
    {
      res = arad_egq_ppct_tbl_get_unsafe(
              unit,
              core,
              tm_port,
              &data
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

      data.cnm_intrcpt_fc_en = SOC_SAND_BOOL2NUM(info->cnm_intercept_enable);
      data.cnm_intrcpt_fc_vec_pfc = info->cnm_pfc_channel;
      res = arad_egq_ppct_tbl_set_unsafe(
              unit,
              core,
              tm_port,
              &data
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
    }
  }

  /* Set the bit in the LP PFC bitmap that will be sent to the NIF (NIF_PORT x TC) */
  reg_offset = offset / 32;
  bit_offset = offset % 32;

  res = READ_CFC_LP_GLB_RSC_TO_NIF_PFC_MAPr(unit, pfc_map_reg);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  pfc_map_reg[reg_offset] &= SOC_SAND_RBITS_MASK(bit_offset + arad_fc_inbnd_max_nof_priorities_get(nif_ndx) - 1, bit_offset);
  pfc_map_reg[reg_offset] |= SOC_SAND_SET_FLD_IN_PLACE(
                                    info->glbl_rcs_low,
                                    bit_offset,
                                    SOC_SAND_BITS_MASK(
                                      bit_offset + arad_fc_inbnd_max_nof_priorities_get(nif_ndx) - 1,
                                      bit_offset
                                      )
                                    );

  res = WRITE_CFC_LP_GLB_RSC_TO_NIF_PFC_MAPr(unit, pfc_map_reg);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_cb_set_unsafe()", 0, 0);
}

STATIC uint32
  arad_fc_gen_inbnd_ll_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_TMC_FC_GEN_INBND_LL             *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset = arad_nif2intern_id(unit, nif_ndx);
  uint32
    idx,
    tm_port;
  ARAD_PORT2IF_MAPPING_INFO
    out_mapping;
  ARAD_EGQ_PPCT_TBL_DATA
    data;
  soc_pbmp_t
    ports_bm;
  int
    core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_LL_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit);

  SOC_PBMP_ITER(ports_bm, idx)
  {

    res = soc_port_sw_db_local_to_tm_port_get(unit, idx, &tm_port, &core);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 7, exit);

    /* Find the ports that are mapped to the given Interface */
    res = arad_port_to_interface_map_get(
            unit,
            core,
            tm_port,
            &out_mapping.if_id,
            &out_mapping.channel_id
          );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    /* Matching port found */
    if (out_mapping.if_id == nif_ndx)
    {
      /* Get PPCT data and update it */
      res = arad_egq_ppct_tbl_get_unsafe(
              unit,
              core,
              tm_port,
              &data
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

      data.cnm_intrcpt_fc_en = SOC_SAND_BOOL2NUM(info->cnm_enable);
      /* Map OTM-Port to the LLFC Channel (internal nif id) */
      data.cnm_intrcpt_fc_vec_llfc = offset; 

      res = arad_egq_ppct_tbl_set_unsafe(
              unit,
              core,
              tm_port,
              &data
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_ll_set_unsafe()", nif_ndx, 0);
}


uint32
  arad_fc_pfc_timer_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID           nif_ndx,
    SOC_SAND_IN  SOC_TMC_FC_PFC_TIMER_INFO   *info
  )
{
  uint32
      res = SOC_SAND_OK;
  uint32
      wc_ndx,
      clp_port_idx = ARAD_NIF_NOF_CLP_PORTS,
      xlp_port_idx = ARAD_NIF_NOF_XLP_PORTS;
  uint32
    nif_intern_id;
  ARAD_NIF_TYPE
    nif_type;
  uint64
      field64,
      reg_value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /* Indexing */
  nif_intern_id = arad_nif2intern_id(unit, nif_ndx);
  nif_type = arad_nif_id2type(nif_ndx);
  wc_ndx = ARAD_NIF2WC_GLBL_ID(nif_intern_id);
  xlp_port_idx = ARAD_NIF_XLP_NDX(wc_ndx);
  clp_port_idx = ARAD_NIF_CLP_NDX(wc_ndx);


  /*Setting val*/
  if (clp_port_idx < ARAD_NIF_NOF_CLP_PORTS) {
    if (ARAD_NIF_IS_CMAC(nif_type)) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,READ_CLP_CMAC_PFC_CTRLr(unit,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1], &reg_value));
        COMPILER_64_SET(field64,0,info->enable);
        soc_reg64_field_set(unit, CLP_CMAC_PFC_CTRLr, &reg_value, PFC_REFRESH_ENf, field64);
        if (info->refresh_time != -1 && info->refresh_time != 0) {
            COMPILER_64_SET(field64,0,info->refresh_time);
            soc_reg64_field_set(unit, CLP_CMAC_PFC_CTRLr, &reg_value, PFC_REFRESH_TIMERf, field64); 
        }
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,WRITE_CLP_CMAC_PFC_CTRLr(unit,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  reg_value)); 
        
    }
    else {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,READ_CLP_XMAC_PFC_CTRLr(unit,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1], &reg_value));
        COMPILER_64_SET(field64,0,info->enable);
        soc_reg64_field_set(unit, CLP_XMAC_PFC_CTRLr, &reg_value,     PFC_REFRESH_ENf, field64);
        if (info->refresh_time != -1 && info->refresh_time != 0) {
            COMPILER_64_SET(field64,0,info->refresh_time);
            soc_reg64_field_set(unit, CLP_XMAC_PFC_CTRLr, &reg_value,     PFC_REFRESH_TIMERf, field64);
        }
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  25,  exit, ARAD_REG_ACCESS_ERR,WRITE_CLP_XMAC_PFC_CTRLr(unit,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  reg_value)); 
    }
  }
  if (xlp_port_idx < ARAD_NIF_NOF_XLP_PORTS) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,READ_XLP_XMAC_PFC_CTRLr(unit,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1], &reg_value));
        COMPILER_64_SET(field64,0,info->enable);
        soc_reg64_field_set(unit, XLP_XMAC_PFC_CTRLr, &reg_value,     PFC_REFRESH_ENf, field64);
        if (info->refresh_time != -1 && info->refresh_time != 0) {
            COMPILER_64_SET(field64,0,info->refresh_time);
            soc_reg64_field_set(unit, XLP_XMAC_PFC_CTRLr, &reg_value,     PFC_REFRESH_TIMERf, field64);
        }
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  35,  exit, ARAD_REG_ACCESS_ERR,WRITE_XLP_XMAC_PFC_CTRLr(unit,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  reg_value)); 
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(" arad_fc_pfc_timer_set_unsafe()", 0, 0);
}

uint32
  arad_fc_pfc_timer_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID           nif_ndx,
    SOC_SAND_IN  SOC_TMC_FC_PFC_TIMER_INFO     *info
  )
{
    uint32
        res = SOC_SAND_OK;
    SOC_TMC_FC_GEN_INBND_INFO 
        port_fc_info;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_fc_gen_inbnd_get_unsafe(unit, nif_ndx, &port_fc_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (port_fc_info.mode != SOC_TMC_FC_INBND_MODE_PFC) {
      SOC_SAND_SET_ERROR_CODE(ARAD_NIF_API_NOT_APLICABLE_FOR_NON_PFC_PORT, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("arad_fc_pfc_timer_verify()", nif_ndx, 0);
}

uint32
  arad_fc_pfc_timer_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID           nif_ndx,
    SOC_SAND_OUT  SOC_TMC_FC_PFC_TIMER_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    wc_ndx,
    clp_port_idx = ARAD_NIF_NOF_CLP_PORTS,
    xlp_port_idx = ARAD_NIF_NOF_XLP_PORTS;
  uint32
    nif_intern_id;
  ARAD_NIF_TYPE
    nif_type;
  uint64
      reg_value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /* Indexing */
  nif_intern_id = arad_nif2intern_id(unit, nif_ndx);
  nif_type = arad_nif_id2type(nif_ndx);
  wc_ndx = ARAD_NIF2WC_GLBL_ID(nif_intern_id);
  xlp_port_idx = ARAD_NIF_XLP_NDX(wc_ndx);
  clp_port_idx = ARAD_NIF_CLP_NDX(wc_ndx);

  /*Getting val*/
  if (clp_port_idx < ARAD_NIF_NOF_CLP_PORTS) {
    if (ARAD_NIF_IS_CMAC(nif_type)) {
        
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,READ_CLP_CMAC_PFC_CTRLr(unit,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1], &reg_value));
        info->enable = soc_reg64_field32_get(unit, CLP_CMAC_PFC_CTRLr, reg_value, PFC_REFRESH_ENf);
        info->refresh_time = soc_reg64_field32_get(unit, CLP_CMAC_PFC_CTRLr, reg_value, PFC_REFRESH_TIMERf);
    }
    else {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,READ_CLP_XMAC_PFC_CTRLr(unit,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1], &reg_value));
        info->enable = soc_reg64_field32_get(unit, CLP_XMAC_PFC_CTRLr, reg_value, PFC_REFRESH_ENf);
        info->refresh_time = soc_reg64_field32_get(unit, CLP_XMAC_PFC_CTRLr, reg_value, PFC_REFRESH_TIMERf);
    }
  }
  if (xlp_port_idx < ARAD_NIF_NOF_XLP_PORTS) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,READ_XLP_XMAC_PFC_CTRLr(unit,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1], &reg_value));
        info->enable = soc_reg64_field32_get(unit, XLP_XMAC_PFC_CTRLr, reg_value, PFC_REFRESH_ENf);
        info->refresh_time = soc_reg64_field32_get(unit, XLP_XMAC_PFC_CTRLr, reg_value, PFC_REFRESH_TIMERf);
    }
  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(" arad_fc_pfc_timer_get_unsafe()", 0, 0);
}




/*********************************************************************
*     Inband Flow Control Generation Configuration, based on
 *     Arad Ingress state indications.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_gen_inbnd_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_TMC_FC_GEN_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    nif_id,
    nif_intern_id;
  uint32
    wc_ndx,
    clp_port_idx = ARAD_NIF_NOF_CLP_PORTS,
    xlp_port_idx = ARAD_NIF_NOF_XLP_PORTS,
    fld_val;
  ARAD_NIF_TYPE
    nif_type;  
  SOC_TMC_FC_GEN_INBND_LL
    ll_disabled;
  SOC_TMC_FC_GEN_INBND_PFC
    pfc_disabled;
  uint64
    field64;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_TMC_FC_GEN_INBND_LL_clear(&ll_disabled);
  SOC_TMC_FC_GEN_INBND_PFC_clear(&pfc_disabled);
  pfc_disabled.inherit = SOC_TMC_FC_INBND_PFC_INHERIT_DISABLED;

  /* Indexing */
  nif_id = arad_nif2intern_id(unit, nif_ndx);
  nif_type = arad_nif_id2type(nif_ndx);
  nif_intern_id = nif_id;
  wc_ndx = ARAD_NIF2WC_GLBL_ID(nif_intern_id);
  xlp_port_idx = ARAD_NIF_XLP_NDX(wc_ndx);
  clp_port_idx = ARAD_NIF_CLP_NDX(wc_ndx);

  fld_val = 0x1;

  switch (info->mode) {
  case SOC_TMC_FC_INBND_MODE_LL:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  5,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, LLFC_VSQ_TO_NIF_ENf,  fld_val));
      break;
  case SOC_TMC_FC_INBND_MODE_PFC:
  case SOC_TMC_FC_INBND_MODE_SAFC:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  6,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, PFC_VSQ_TO_NIF_ENf,  fld_val));
      break;
  case SOC_TMC_FC_INBND_MODE_DEVICE_DISABLED:
      fld_val = 0x0;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  7,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, LLFC_VSQ_TO_NIF_ENf,  fld_val));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  8,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, PFC_VSQ_TO_NIF_ENf,  fld_val));
      break;
  default:
  case SOC_TMC_FC_INBND_MODE_DISABLED:
      break;
  }
  

  if (clp_port_idx < ARAD_NIF_NOF_CLP_PORTS) {
      if (ARAD_NIF_IS_CMAC(nif_type)) {
        /* Enable / Disable - LL */
        fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_LL);
        COMPILER_64_SET(field64,0,fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CLP_CMAC_PAUSE_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_PAUSE_ENf,  field64));

        /* Enable / Disable - PFC */
        fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_PFC);
        COMPILER_64_SET(field64,0,fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CLP_CMAC_PFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_PFC_ENf,  field64));

        /* Enable / Disable - SAFC */
        fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_SAFC);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CLP_CMAC_LLFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_LLFC_ENf,  fld_val));

      } else {
        /* Enable / Disable - LL */
        fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_LL);
        COMPILER_64_SET(field64,0,fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  14,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CLP_XMAC_PAUSE_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_PAUSE_ENf,  field64));

        /* Enable / Disable - PFC */
        fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_PFC);
        COMPILER_64_SET(field64,0,fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  16,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CLP_XMAC_PFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_PFC_ENf,  field64));

        /* Enable / Disable - SAFC */
        fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_SAFC);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  16,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CLP_XMAC_LLFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_LLFC_ENf,  fld_val));
      }
  }

  if (xlp_port_idx < ARAD_NIF_NOF_XLP_PORTS) {
    /* Enable / Disable - LL */
    fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_LL);
    COMPILER_64_SET(field64,0,fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, XLP_XMAC_PAUSE_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_PAUSE_ENf,  field64));

    /* Enable / Disable - PFC */
    fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_PFC);
    COMPILER_64_SET(field64,0,fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, XLP_XMAC_PFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_PFC_ENf,  field64));

    /* Enable / Disable - SAFC */
    fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_SAFC);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, XLP_XMAC_LLFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_LLFC_ENf,  fld_val));
  }

  switch (info->mode)
  {
  case SOC_TMC_FC_INBND_MODE_LL:
    /* Disable PFC */
    res = arad_fc_gen_inbnd_pfc_set_unsafe(
            unit,
            nif_ndx,
            &pfc_disabled
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    /* Configure LL */
    res = arad_fc_gen_inbnd_ll_set_unsafe(
            unit,
            nif_ndx,
            &info->ll
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    break;
  case SOC_TMC_FC_INBND_MODE_PFC:
  case SOC_TMC_FC_INBND_MODE_SAFC:
    /* Disable LL */
    res = arad_fc_gen_inbnd_ll_set_unsafe(
            unit,
            nif_ndx,
            &ll_disabled
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /* Configure PFC */
    res = arad_fc_gen_inbnd_pfc_set_unsafe(
            unit,
            nif_ndx,
            &info->pfc
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    break;
    case SOC_TMC_FC_INBND_MODE_DISABLED:
    default:
      /* Disable LL */
      res = arad_fc_gen_inbnd_ll_set_unsafe(
            unit,
            nif_ndx,
            &ll_disabled
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

      /* Disable PFC */
      res = arad_fc_gen_inbnd_pfc_set_unsafe(
            unit,
            nif_ndx,
            &pfc_disabled
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_set_unsafe()", nif_ndx, 0);
}

uint32
  arad_fc_gen_inbnd_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_GET_VERIFY);

  res = arad_nif_id_verify(
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (ARAD_NIF_IS_TYPE_ID(ILKN, nif_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_NIF_API_NOT_APPLICABLE_FOR_ILKN_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_verify()", nif_ndx, 0);
}

STATIC uint32
  arad_fc_gen_inbnd_pfc_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_PFC             *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset = arad_nif2intern_id(unit, nif_ndx),
    reg_offset,
    bit_offset;
  uint32
    idx = 0,
    tm_port;
  ARAD_PORT2IF_MAPPING_INFO
    out_mapping;
  ARAD_EGQ_PPCT_TBL_DATA
    data;
  soc_reg_above_64_val_t  
    pfc_map_reg;
  soc_pbmp_t
    ports_bm;
  int
    core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_PFC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit);

  info->cnm_intercept_enable = 0;

  SOC_PBMP_ITER(ports_bm, idx)
  {
    res = soc_port_sw_db_local_to_tm_port_get(unit, idx, &tm_port, &core);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 7, exit);

    res = arad_port_to_interface_map_get(
            unit,
            core,
            tm_port,
            &out_mapping.if_id,
            &out_mapping.channel_id
          );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    if (out_mapping.if_id == nif_ndx)
    {
      res = arad_egq_ppct_tbl_get_unsafe(
              unit,
              core,
              idx,
              &data
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
      info->cnm_intercept_enable = SOC_SAND_NUM2BOOL(data.cnm_intrcpt_fc_en);

      break;
    }
  }

  reg_offset = offset / 32;
  bit_offset = 2 * (offset % 32);

  res = READ_CFC_LP_GLB_RSC_TO_NIF_PFC_MAPr(unit, pfc_map_reg);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  info->glbl_rcs_low = SOC_SAND_GET_FLD_FROM_PLACE(
                         pfc_map_reg[reg_offset],
                         bit_offset,
                         SOC_SAND_BITS_MASK(
                           bit_offset + arad_fc_inbnd_max_nof_priorities_get(nif_ndx) - 1,
                           bit_offset
                         )
                       );

  info->inherit = SOC_TMC_FC_INBND_PFC_INHERIT_DISABLED;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_cb_get_unsafe()", 0, 0);
}

STATIC uint32
  arad_fc_gen_inbnd_ll_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_LL             *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    idx = 0,
    tm_port;
  ARAD_PORT2IF_MAPPING_INFO
    out_mapping;
  ARAD_EGQ_PPCT_TBL_DATA
    data;
  soc_pbmp_t
    ports_bm;
  int
    core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_LL_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit);

  info->cnm_enable = 0;

  SOC_PBMP_ITER(ports_bm, idx)
  {
    res = soc_port_sw_db_local_to_tm_port_get(unit, idx, &tm_port, &core);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 7, exit);

    res = arad_port_to_interface_map_get(
            unit,
            core,
            tm_port,
            &out_mapping.if_id,
            &out_mapping.channel_id
          );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    if (out_mapping.if_id == nif_ndx)
    {
      res = arad_egq_ppct_tbl_get_unsafe(
              unit,
              core,
              idx,
              &data
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
      info->cnm_enable = SOC_SAND_NUM2BOOL(data.cnm_intrcpt_fc_en);

      break;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_ll_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Inband Flow Control Generation Configuration, based on
 *     Arad Ingress state indications.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_gen_inbnd_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    nif_id,
    nif_intern_id;
  ARAD_NIF_TYPE
    nif_type;
  uint32
    wc_ndx,
    clp_port_idx,
    xlp_port_idx,
    ll_enable = 0,
    pfc_enable = 0,
    safc_enable = 0;
  uint64
    field64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_TMC_FC_GEN_INBND_INFO_clear(info);

  /* Indexing */
  nif_id = arad_nif2intern_id(unit, nif_ndx);
  nif_type = arad_nif_id2type(nif_ndx);
  nif_intern_id = nif_id;
  wc_ndx = ARAD_NIF2WC_GLBL_ID(nif_intern_id);
  xlp_port_idx = ARAD_NIF_XLP_NDX(wc_ndx);
  clp_port_idx = ARAD_NIF_CLP_NDX(wc_ndx);


  /* Check which mode is enabled */
  if (clp_port_idx < ARAD_NIF_NOF_CLP_PORTS) {
      if (ARAD_NIF_IS_CMAC(nif_type)) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, CLP_CMAC_PAUSE_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_PAUSE_ENf, &field64));
          ll_enable = COMPILER_64_LO(field64);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, CLP_CMAC_PFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_PFC_ENf, &field64));
          pfc_enable = COMPILER_64_LO(field64);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CLP_CMAC_LLFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_LLFC_ENf, &safc_enable));
      }
      else {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, CLP_XMAC_PAUSE_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_PAUSE_ENf, &field64));
          ll_enable = COMPILER_64_LO(field64);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, CLP_XMAC_PFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_PFC_ENf, &field64));      
          pfc_enable = COMPILER_64_LO(field64);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CLP_XMAC_LLFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_LLFC_ENf, &safc_enable));      
      }
  }

  if (xlp_port_idx < ARAD_NIF_NOF_XLP_PORTS) {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, XLP_XMAC_PAUSE_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_PAUSE_ENf, &field64));
    ll_enable = COMPILER_64_LO(field64);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, XLP_XMAC_PFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_PFC_ENf, &field64));
    pfc_enable = COMPILER_64_LO(field64);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, XLP_XMAC_LLFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, TX_LLFC_ENf, &safc_enable));
  }  
  
  /* set the Inband Mode according to the register values */
  if (ll_enable)
  {
    info->mode = SOC_TMC_FC_INBND_MODE_LL;
  }
  else if(pfc_enable)
  {
    info->mode = SOC_TMC_FC_INBND_MODE_PFC;
  }
  else if(safc_enable)
  {
    info->mode = SOC_TMC_FC_INBND_MODE_SAFC;
  }
  else
  {
    info->mode = SOC_TMC_FC_INBND_MODE_DISABLED;
  }
  
  /* Get the LL configuration */
  if (info->mode == SOC_TMC_FC_INBND_MODE_LL)
  {
    res = arad_fc_gen_inbnd_ll_get_unsafe(
            unit,
            nif_ndx,
            &info->ll
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  /* Get the PFC configuration */
  if (info->mode == SOC_TMC_FC_INBND_MODE_PFC || info->mode == SOC_TMC_FC_INBND_MODE_SAFC)
  {
    res = arad_fc_gen_inbnd_pfc_get_unsafe(
            unit,
            nif_ndx,
            &info->pfc
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_get_unsafe()", nif_ndx, 0);
}

/*********************************************************************
*     Enable/disable Flow Control generation, based on Ingress
 *     Global Resources - high priority, via NIF. Flow Control
 *     generation may be either Link Level or Class Based. For
 *     Link Level - Flow Control will be generated on all
 *     links. For Class Based - Flow Control will be generated
 *     on all Flow Control Classes.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_gen_inbnd_glb_hp_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE     fc_mode
  )
{
  uint32
    res = SOC_SAND_OK;
   
  uint32
    ll_fld_val,
    pfc_fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_GLB_HP_SET_UNSAFE);

  /* Enable/Disable LL */
  ll_fld_val = (fc_mode == SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_LL || fc_mode == SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_ALL);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, FRC_NIF_LLVL_ENf,  ll_fld_val));

  /* Enable/Disable PFC */
  pfc_fld_val = (fc_mode == SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_PFC || fc_mode == SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_ALL);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, FRC_NIF_PFC_ENf,  pfc_fld_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_glb_hp_set_unsafe()", 0, 0);
}

uint32
  arad_fc_gen_inbnd_glb_hp_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE     fc_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_GLB_HP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fc_mode, (SOC_TMC_FC_NOF_INGR_GEN_GLB_HP_MODES-1), ARAD_FC_MODE_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_glb_hp_set_verify()", 0, 0);
}

uint32
  arad_fc_gen_inbnd_glb_hp_get_verify(
    SOC_SAND_IN  int                      unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_GLB_HP_GET_VERIFY);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_glb_hp_get_verify()", 0, 0);
}

/*********************************************************************
*     Enable/disable Flow Control generation, based on Ingress
 *     Global Resources - high priority, via NIF. Flow Control
 *     generation may be either Link Level or Class Based. For
 *     Link Level - Flow Control will be generated on all
 *     links. For Class Based - Flow Control will be generated
 *     on all Flow Control Classes.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_gen_inbnd_glb_hp_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_TMC_FC_INGR_GEN_GLB_HP_MODE     *fc_mode
  )
{
  uint32
    res = SOC_SAND_OK;
   
  uint32
    ll_fld_val,
    pfc_fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_GLB_HP_GET_UNSAFE);
  
  /* Get the register value */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, FRC_NIF_LLVL_ENf, &ll_fld_val));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, FRC_NIF_PFC_ENf, &pfc_fld_val));
  
  /* Set the FC Mode according to the register values */
  if(ll_fld_val && pfc_fld_val)
  {
    *fc_mode = SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_ALL;
  }
  else if (pfc_fld_val)
  {
    *fc_mode = SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_PFC;
  }
  else if (ll_fld_val)
  {
    *fc_mode = SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_LL;
  }
  else
  {
    *fc_mode = SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_NONE;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_glb_hp_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Map the 3-LSB of the CMN CPID (represent TC), to the FC
 *     indication to generate when using Class Based Flow
 *     Control
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_gen_inbnd_cnm_map_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_IN  uint8                      enable_ll,
    SOC_SAND_IN  uint32                      fc_class
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tc_field,
    tc_reg,
    fld_val;
  uint32
    tc_regs[3] = {EGQ_CNM_CPID_TO_FC_TYPE_0r, EGQ_CNM_CPID_TO_FC_TYPE_1r, EGQ_CNM_CPID_TO_FC_TYPE_2r};
  uint32
    tc_fields[8] = {CNM_CPID_0_TO_FC_TYPEf, CNM_CPID_1_TO_FC_TYPEf, CNM_CPID_2_TO_FC_TYPEf, CNM_CPID_3_TO_FC_TYPEf,
                   CNM_CPID_4_TO_FC_TYPEf, CNM_CPID_5_TO_FC_TYPEf, CNM_CPID_6_TO_FC_TYPEf, CNM_CPID_7_TO_FC_TYPEf};
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_CNM_MAP_SET_UNSAFE);

  /* 3 Regs, 8 TC Classes */
  tc_reg = tc_regs[cpid_tc_ndx / ARAD_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_FLDS];
  tc_field = tc_fields[cpid_tc_ndx];
  
  /* Bit 0 is LL */
  /* Bits 1:8 are PFC */
  fld_val = SOC_SAND_BOOL2NUM(enable_ll);
  fld_val = fld_val | (fc_class << 1);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, tc_reg, REG_PORT_ANY, 0, tc_field,  fld_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_cnm_map_set_unsafe()", cpid_tc_ndx, 0);
}

uint32
  arad_fc_gen_inbnd_cnm_map_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_IN  uint8                      enable_ll,
    SOC_SAND_IN  uint32                      fc_class
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_CNM_MAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cpid_tc_ndx, ARAD_FC_CLASS_MAX - 1, ARAD_CPID_TC_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fc_class, ARAD_FC_CLASS_MAX - 1, ARAD_FC_CLASS_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_cnm_map_set_verify()", cpid_tc_ndx, 0);
}

uint32
  arad_fc_gen_inbnd_cnm_map_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_CNM_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cpid_tc_ndx, ARAD_FC_CLASS_MAX - 1, ARAD_CPID_TC_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_cnm_map_get_verify()", cpid_tc_ndx, 0);
}
/*********************************************************************
*     Map the 3-LSB of the CMN CPID (represent TC), to the FC
 *     indication to generate when using Class Based Flow
 *     Control
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_gen_inbnd_cnm_map_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_OUT uint8                      *enable_ll,
    SOC_SAND_OUT uint32                      *fc_class
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tc_reg,
    tc_field,
    fld_val;
  uint32
    tc_regs[3] = {EGQ_CNM_CPID_TO_FC_TYPE_0r, EGQ_CNM_CPID_TO_FC_TYPE_1r, EGQ_CNM_CPID_TO_FC_TYPE_2r};
  uint32
    tc_fields[8] = {CNM_CPID_0_TO_FC_TYPEf, CNM_CPID_1_TO_FC_TYPEf, CNM_CPID_2_TO_FC_TYPEf, CNM_CPID_3_TO_FC_TYPEf,
                    CNM_CPID_4_TO_FC_TYPEf, CNM_CPID_5_TO_FC_TYPEf, CNM_CPID_6_TO_FC_TYPEf, CNM_CPID_7_TO_FC_TYPEf};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_INBND_CNM_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fc_class);
  SOC_SAND_CHECK_NULL_INPUT(enable_ll);

  /* 3 Regs, 8 TC Classes */
  tc_reg = tc_regs[cpid_tc_ndx / ARAD_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_FLDS];
  tc_field = tc_fields[cpid_tc_ndx];

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, tc_reg, REG_PORT_ANY, 0, tc_field, &fld_val));

  /* Bit 0 is LL */
  /* Bits 1:8 are PFC */
  *enable_ll = fld_val & 0x1;
  *fc_class = fld_val >> 1;
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_inbnd_cnm_map_get_unsafe()", cpid_tc_ndx, 0);
}


/*********************************************************************
*     Inband Flow Control Reception Configuration
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_rec_inbnd_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_TMC_FC_REC_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    nif_id,
    nif_intern_id;
  ARAD_NIF_TYPE
    nif_type;
  uint32
    fld_val,
    wc_ndx,
    clp_port_idx,
    xlp_port_idx;
  uint64
      field64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_REC_INBND_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /* Indexing */
  nif_id = arad_nif2intern_id(unit, nif_ndx);
  nif_type = arad_nif_id2type(nif_ndx);
  nif_intern_id = nif_id;
  wc_ndx = ARAD_NIF2WC_GLBL_ID(nif_intern_id);
  xlp_port_idx = ARAD_NIF_XLP_NDX(wc_ndx);
  clp_port_idx = ARAD_NIF_CLP_NDX(wc_ndx);

  if (clp_port_idx < ARAD_NIF_NOF_CLP_PORTS) {
      if (ARAD_NIF_IS_CMAC(nif_type)) {
        /* Enable / Disable - LL */
        fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_LL);
        COMPILER_64_SET(field64,0,fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CLP_CMAC_PAUSE_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_PAUSE_ENf,  field64));

        /* Enable / Disable - PFC */
        fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_PFC);
        COMPILER_64_SET(field64,0,fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CLP_CMAC_PFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_PFC_ENf,  field64));

        /* Enable / Disable - SAFC */
        fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_SAFC);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CLP_CMAC_LLFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_LLFC_ENf,  fld_val));
      } else {
        /* Enable / Disable - LL */
        fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_LL);
        COMPILER_64_SET(field64,0,fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CLP_XMAC_PAUSE_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_PAUSE_ENf,  field64));

        /* Enable / Disable - PFC */
        fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_PFC);
        COMPILER_64_SET(field64,0,fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CLP_XMAC_PFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_PFC_ENf,  field64));

        /* Enable / Disable - SAFC */
        fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_SAFC);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CLP_XMAC_LLFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_LLFC_ENf,  fld_val));
      }
  }

  if (xlp_port_idx < ARAD_NIF_NOF_XLP_PORTS) {
    /* Enable / Disable - LL */
    fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_LL);
    COMPILER_64_SET(field64,0,fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, XLP_XMAC_PAUSE_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_PAUSE_ENf,  field64));

    /* Enable / Disable - PFC */
    fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_PFC);
    COMPILER_64_SET(field64,0,fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, XLP_XMAC_PFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_PFC_ENf,  field64));

    /* Enable / Disable - SAFC */
    fld_val = SOC_SAND_BOOL2NUM(info->mode == SOC_TMC_FC_INBND_MODE_SAFC);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, XLP_XMAC_LLFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_LLFC_ENf,  fld_val));
  }


  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_rec_inbnd_set_unsafe()", nif_ndx, 0);
}

uint32
  arad_fc_rec_inbnd_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_TMC_FC_REC_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_REC_INBND_SET_VERIFY);

  res = arad_nif_id_verify(
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (ARAD_NIF_IS_TYPE_ID(ILKN, nif_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_NIF_API_NOT_APPLICABLE_FOR_ILKN_ERR, 12, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_rec_inbnd_set_verify()", nif_ndx, 0);
}

uint32
  arad_fc_rec_inbnd_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_REC_INBND_GET_VERIFY);

  res = arad_nif_id_verify(
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (ARAD_NIF_IS_TYPE_ID(ILKN, nif_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_NIF_API_NOT_APPLICABLE_FOR_ILKN_ERR, 12, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_rec_inbnd_get_verify()", nif_ndx, 0);
}
/*********************************************************************
*     Inband Flow Control Reception Configuration
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_rec_inbnd_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_TMC_FC_REC_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    nif_id,
    nif_intern_id;
  ARAD_NIF_TYPE
    nif_type;
  uint32
    wc_ndx,
    clp_port_idx,
    xlp_port_idx,
    ll_enable = 0,
    pfc_enable = 0,
    safc_enable = 0;
  uint64
      field64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_REC_INBND_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_TMC_FC_REC_INBND_INFO_clear(info);

  /* Indexing */
  nif_id = arad_nif2intern_id(unit, nif_ndx);
  nif_type = arad_nif_id2type(nif_ndx);
  nif_intern_id = nif_id;
  wc_ndx = ARAD_NIF2WC_GLBL_ID(nif_intern_id);
  xlp_port_idx = ARAD_NIF_XLP_NDX(wc_ndx);
  clp_port_idx = ARAD_NIF_CLP_NDX(wc_ndx);

  /* Check which mode is enabled */
  if (clp_port_idx < ARAD_NIF_NOF_CLP_PORTS) {
      if (ARAD_NIF_IS_CMAC(nif_type) ) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  9,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CLP_CMAC_LLFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_LLFC_ENf, &safc_enable));
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, CLP_CMAC_PAUSE_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_PAUSE_ENf, &field64));
          ll_enable = COMPILER_64_LO(field64);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, CLP_CMAC_PFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_PFC_ENf, &field64));
          pfc_enable = COMPILER_64_LO(field64);
      }
      else {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  9,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CLP_XMAC_LLFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_LLFC_ENf, &safc_enable));
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, CLP_XMAC_PAUSE_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_PAUSE_ENf, &field64));
          ll_enable = COMPILER_64_LO(field64);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, CLP_XMAC_PFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_PFC_ENf, &field64));
          pfc_enable = COMPILER_64_LO(field64);
      }
      
  }
  if (xlp_port_idx < ARAD_NIF_NOF_XLP_PORTS) {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  19,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, XLP_XMAC_LLFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_LLFC_ENf, &safc_enable));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, XLP_XMAC_PAUSE_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_PAUSE_ENf, &field64));
    ll_enable = COMPILER_64_LO(field64);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, XLP_XMAC_PFC_CTRLr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, RX_PFC_ENf, &field64));
    pfc_enable = COMPILER_64_LO(field64);
  }

  /* set the Inband Mode according to the register values */
  if (ll_enable)
  {
    info->mode = SOC_TMC_FC_INBND_MODE_LL;
  }
  else if(pfc_enable)
  {
    info->mode = SOC_TMC_FC_INBND_MODE_PFC;
  }
  else if(safc_enable)
  {
    info->mode = SOC_TMC_FC_INBND_MODE_SAFC;
  }
  else
  {
    info->mode = SOC_TMC_FC_INBND_MODE_DISABLED;
  }

  /* Handle PFC */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_rec_inbnd_get_unsafe()", nif_ndx, 0);
}

uint32
  arad_fc_rcy_hr_enable_get_verify(
    SOC_SAND_IN  int                      unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_RCY_HR_ENABLE_GET_VERIFY);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_rcy_hr_enable_get_verify()", 0, 0);
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Generation (OOB/ILKN-Inband TX).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_gen_cal_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  )
{
  uint32
    /*fld_val = 0,*/
    res = SOC_SAND_OK,
    source_val = 0;
  uint32
    rep_idx = 0,      /* Calendar repetition index */
    per_rep_idx = 0,  /* Calendar entry index, per repetition */
    entry_idx = 0;    /* Calendar entry index, global */
  ARAD_CFC_CALTX_TBL_DATA
    tx_cal;
  uint32
    hcfc_fields[ARAD_FC_HCFC_BITMAPS] = {
      CAT_2_TC_0_MAP_HCFC_ENAf, CAT_2_TC_1_MAP_HCFC_ENAf, CAT_2_TC_2_MAP_HCFC_ENAf, CAT_2_TC_3_MAP_HCFC_ENAf,
      CAT_2_TC_4_MAP_HCFC_ENAf, CAT_2_TC_5_MAP_HCFC_ENAf, CAT_2_TC_6_MAP_HCFC_ENAf, CAT_2_TC_7_MAP_HCFC_ENAf
    };
  /*uint32
    cal_len_reg=0,
    cal_len_fld=0,
    cal_rep_reg=0,
    cal_rep_fld=0;*/
  uint64
      field64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_CAL_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cal_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  /* Disable the FC before configuring it
  if(if_ndx == SOC_TMC_FC_OOB_ID_A || if_ndx == ARAD_FC_ILKN_ID_A)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_RX_0_ENf,  0));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, SPI_OOB_RX_0_ENf,  0));
  }
  if(if_ndx == SOC_TMC_FC_OOB_ID_B || if_ndx == ARAD_FC_ILKN_ID_B)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_RX_1_ENf,  0));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, SPI_OOB_RX_1_ENf,  0));
  } */

  res = arad_fc_clear_calendar_unsafe(
          unit, 
          cal_mode_ndx, 
          ARAD_FC_CAL_TYPE_TX, 
          if_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* Calendar entries { */
  for (per_rep_idx = 0; per_rep_idx < cal_conf->cal_len; per_rep_idx++)
  {
      
      /* Convert source enum to HW value */
      source_val = arad_fc_gen_cal_src_type_to_val_internal(cal_buff[per_rep_idx].source);

      if (source_val == -1)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_FC_INVALID_CALENDAR_ENTRY_ERR, (40 + per_rep_idx), exit);
      }
      else if (cal_buff[per_rep_idx].source == SOC_TMC_FC_GEN_CAL_SRC_RETRANSMIT)
      {
        /* Set the ILKN Retransmit Req bits: 
         * Bit 0: ILKN-0, Bit 2: ILKN-1 */
        tx_cal.fc_source_sel = source_val;
        tx_cal.fc_index = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ARAD_FC_RETRANSMIT_DEST_ILKN_A_VAL : ARAD_FC_RETRANSMIT_DEST_ILKN_B_VAL);
      }
      else if(cal_buff[per_rep_idx].source == SOC_TMC_FC_GEN_CAL_SRC_HCFC)
      {
        tx_cal.fc_source_sel = source_val;
        tx_cal.fc_index = cal_buff[per_rep_idx].id;
        /* Enable the selected HCFC Bitmap */
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CAT_2_TC_MAP_HCFC_ENAr, REG_PORT_ANY, 0, hcfc_fields[cal_buff[per_rep_idx].id],  0x1));
      }
      else
      {
        tx_cal.fc_source_sel = source_val;
        tx_cal.fc_index = cal_buff[per_rep_idx].id;
      }
      for (rep_idx = 0; rep_idx < cal_conf->cal_reps; rep_idx++)
      {
        entry_idx = (rep_idx * cal_conf->cal_len) + per_rep_idx;

        if(ARAD_FC_CAL_MODE_IS_ILKN(cal_mode_ndx))
        {
          res = arad_cfc_ilkn_caltx_tbl_set_unsafe(unit, if_ndx, entry_idx, &tx_cal);
        }
        if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
        {
          res = arad_cfc_oob_caltx_tbl_set_unsafe(unit, if_ndx, entry_idx, &tx_cal);
        }
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      }
  } /* Calendar entries } */

  /* Enable the FC after configuring it */
  if(if_ndx == SOC_TMC_FC_OOB_ID_A)
  {
    COMPILER_64_SET(field64,0,0x1);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  38,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_TX_0_ENf,  field64));
  }
  if(if_ndx == SOC_TMC_FC_OOB_ID_B)
  {
    COMPILER_64_SET(field64,0,0x1);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  39,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_TX_1_ENf,  field64));
  }

  /*  Enable/disable */
  if(if_ndx == SOC_TMC_FC_OOB_ID_A)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, OOB_TX_0_MODEf,  SOC_SAND_BOOL2NUM(ARAD_FC_CAL_MODE_IS_ILKN(cal_mode_ndx))));
    COMPILER_64_SET(field64,0,SOC_SAND_BOOL2NUM(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_TX_0_SELf,  field64));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  43,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_TX_CONFIGURATIONr, REG_PORT_ANY, 0, SPI_OOB_TX_0_ENf,  SOC_SAND_BOOL2NUM((cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB) && cal_conf->enable)));
    COMPILER_64_SET(field64,0,SOC_SAND_BOOL2NUM(ARAD_FC_CAL_MODE_IS_ILKN(cal_mode_ndx) && cal_conf->enable));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  41,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_TX_0_ENf,  field64));
  }
  if(if_ndx == SOC_TMC_FC_OOB_ID_B)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, OOB_TX_1_MODEf,  SOC_SAND_BOOL2NUM(ARAD_FC_CAL_MODE_IS_ILKN(cal_mode_ndx))));
    COMPILER_64_SET(field64,0,SOC_SAND_BOOL2NUM(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  45,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_TX_1_SELf,  field64));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  43,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_SPI_OOB_TX_CONFIGURATIONr, REG_PORT_ANY, 0, SPI_OOB_TX_1_ENf,  SOC_SAND_BOOL2NUM((cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB) && cal_conf->enable)));
    COMPILER_64_SET(field64,0,SOC_SAND_BOOL2NUM(ARAD_FC_CAL_MODE_IS_ILKN(cal_mode_ndx) && cal_conf->enable));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_TX_1_ENf,  field64));
  }

  COMPILER_64_SET(field64,0,0x1);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  200,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, ECI_BLOCKS_SOFT_INITr, REG_PORT_ANY, 0, CFC_INITf,  field64));
  COMPILER_64_SET(field64,0,0x0);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  201,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, ECI_BLOCKS_SOFT_INITr, REG_PORT_ANY, 0, CFC_INITf,  field64));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_cal_set_unsafe()", cal_mode_ndx, if_ndx);
}

uint32
  arad_fc_gen_cal_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_CAL_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cal_mode_ndx, ARAD_CAL_MODE_NDX_MAX, ARAD_FC_CAL_MODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(if_ndx, SOC_TMC_FC_NOF_OOB_IDS, ARAD_FC_CAL_IF_NDX_OUT_OF_RANGE_ERR, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_cal_set_verify()", cal_mode_ndx, if_ndx);
}


uint32
  arad_fc_gen_cal_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_CAL_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cal_mode_ndx, ARAD_CAL_MODE_NDX_MAX, ARAD_FC_CAL_MODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(if_ndx, SOC_TMC_FC_NOF_OOB_IDS, ARAD_FC_CAL_IF_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_gen_cal_get_verify()", cal_mode_ndx, if_ndx);
}


/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Generation (OOB/ILKN-Inband TX).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_gen_cal_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_TMC_FC_GEN_CALENDAR             *cal_buff
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_val = 0,
    cal_len_fld = 0,
    cal_len_reg = 0,
    cal_rep_fld = 0,
    cal_rep_reg = 0;
  uint32
    entry_idx = 0;    /* Calendar entry index, global */
  ARAD_CFC_CALTX_TBL_DATA
    tx_cal;
  uint64
      field64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_GEN_CAL_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cal_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  SOC_TMC_FC_CAL_IF_INFO_clear(cal_conf);
  SOC_TMC_FC_GEN_CALENDAR_clear(cal_buff);

  /* Calendar parameters { */
  /* Enable/disable */
  if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
  {
    if(if_ndx == SOC_TMC_FC_OOB_ID_A) 
    {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_SPI_OOB_TX_CONFIGURATIONr, REG_PORT_ANY, 0, SPI_OOB_TX_0_ENf, &fld_val));
    }
    if(if_ndx == SOC_TMC_FC_OOB_ID_B)
    {
       SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_SPI_OOB_TX_CONFIGURATIONr, REG_PORT_ANY, 0, SPI_OOB_TX_1_ENf, &fld_val));
    }
  }
  else /* ILKN */
  {
    if(if_ndx == SOC_TMC_FC_OOB_ID_A) 
    {  
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_TX_0_ENf, &field64));
        fld_val = COMPILER_64_LO(field64);
    }
    if(if_ndx == SOC_TMC_FC_OOB_ID_B)
    {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  13,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, CFC_ILKN_TX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_TX_1_ENf, &field64));
        fld_val = COMPILER_64_LO(field64);
    }
  }
  cal_conf->enable = SOC_SAND_NUM2BOOL(fld_val);


  /*  Calendar length (single repetition) */
  /* The len register should be set to LEN-1 */
  if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB)
  {
    cal_len_reg = CFC_ILKN_TX_CONFIGURATIONr;
    cal_len_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ILKN_TX_0_CAL_LENf : ILKN_TX_1_CAL_LENf);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, cal_len_reg, REG_PORT_ANY, 0, cal_len_fld, &field64));
    fld_val = COMPILER_64_LO(field64);
    cal_conf->cal_len = fld_val + 1;
  }
  if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
  {
    cal_len_reg = CFC_SPI_OOB_TX_CONFIGURATIONr;
    cal_len_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_CAL_LENf : SPI_OOB_TX_1_CAL_LENf);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, cal_len_reg, REG_PORT_ANY, 0, cal_len_fld, &fld_val));
    cal_conf->cal_len = fld_val;
  }

  /*  Calendar number of repetitions - SPI/OOB Only */
  if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
  {
    cal_rep_reg = CFC_SPI_OOB_TX_CONFIGURATIONr;
    cal_rep_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_TX_0_CAL_Mf : SPI_OOB_TX_1_CAL_Mf);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  31,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, cal_rep_reg, REG_PORT_ANY, 0, cal_rep_fld, &fld_val));
  }
  else /* ILKN */
  {
    fld_val = 1;
  }
  cal_conf->cal_reps = fld_val;

  /* Calendar parameters } */

  /* Calendar entries { */

  for (entry_idx = 0; entry_idx < cal_conf->cal_len; entry_idx++)
  {
    if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB) {
       res = arad_cfc_ilkn_caltx_tbl_get_unsafe(unit, if_ndx, entry_idx, &tx_cal);
      SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);
    } else {
      res = arad_cfc_oob_caltx_tbl_get_unsafe(unit, if_ndx, entry_idx, &tx_cal);
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }

    if(tx_cal.fc_source_sel >= ARAD_FC_GEN_CAL_SRC_ARR_SIZE)
    {
      cal_buff[entry_idx].source = SOC_TMC_FC_GEN_CAL_SRC_NONE;
      cal_buff[entry_idx].id = 0;
    }
    else if(arad_fc_gen_cal_src_arr[tx_cal.fc_source_sel] == SOC_TMC_FC_GEN_CAL_SRC_RETRANSMIT)
    {
      /* Set the ILKN Retransmit Req bits: 
       * Bit 0: ILKN-0, Bit 2: ILKN-1 */
      cal_buff[entry_idx].source = arad_fc_gen_cal_src_arr[tx_cal.fc_source_sel];
      cal_buff[entry_idx].id = (tx_cal.fc_index == ARAD_FC_RETRANSMIT_DEST_ILKN_A_VAL ? ARAD_FC_ILKN_ID_A : ARAD_FC_ILKN_ID_B);
    }
    else
    {
      cal_buff[entry_idx].source = arad_fc_gen_cal_src_arr[tx_cal.fc_source_sel];
      cal_buff[entry_idx].id = tx_cal.fc_index;
    }
  }
  /* Calendar entries } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fc_gen_cal_get_unsafe()",cal_mode_ndx,0);
}

uint32
  arad_fc_clear_calendar_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  ARAD_FC_CAL_TYPE                 cal_type,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32 
    cal_rx_entry[128] = {0};
  uint32 
    cal_tx_entry[128] = {0};
  soc_mem_t
    cal_table = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_CLEAR_CALENDAR_UNSAFE);

  if(cal_type == ARAD_FC_CAL_TYPE_RX)
  {
    soc_mem_field32_set(unit, CFC_ILKN_RX_0_CALm, cal_rx_entry, FC_DST_SELf, arad_fc_rec_cal_dest_type_to_val_internal(SOC_TMC_FC_REC_CAL_DEST_NONE));
    soc_mem_field32_set(unit, CFC_ILKN_RX_0_CALm, cal_rx_entry, FC_INDEXf, 0x0);

    if(ARAD_FC_CAL_MODE_IS_ILKN(cal_mode_ndx))
    {
      cal_table = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_ILKN_RX_0_CALm : CFC_ILKN_RX_1_CALm);
    }
    if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
    {
      cal_table = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_SPI_OOB_RX_0_CALm : CFC_SPI_OOB_RX_1_CALm);
    }

    res = arad_fill_table_with_entry(unit, cal_table, MEM_BLOCK_ANY, cal_rx_entry); 
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }
  if(cal_type == ARAD_FC_CAL_TYPE_TX)
  {
    soc_mem_field32_set(unit, CFC_ILKN_TX_0_CALm, cal_tx_entry, FC_SRC_SELf, arad_fc_gen_cal_src_type_to_val_internal(SOC_TMC_FC_GEN_CAL_SRC_CONST));
    soc_mem_field32_set(unit, CFC_ILKN_TX_0_CALm, cal_tx_entry, FC_INDEXf, 0x0);

    if(ARAD_FC_CAL_MODE_IS_ILKN(cal_mode_ndx))
    {
      cal_table = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_ILKN_TX_0_CALm : CFC_ILKN_TX_1_CALm);
    }
    if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
    {
      cal_table = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_SPI_OOB_TX_0_CALm : CFC_SPI_OOB_TX_1_CALm);
    }

    res = arad_fill_table_with_entry(unit, cal_table, MEM_BLOCK_ANY, cal_tx_entry); 
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fc_clear_calendar_unsafe()",cal_mode_ndx,if_ndx);
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_rec_cal_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_REC_CALENDAR             *cal_buff
  )
{
  uint32
    res = SOC_SAND_OK,
    dest_sel = 0;
  uint32
    fld_val = 0,
    reg_val = 0;
  uint32
    rep_idx = 0,      /* Calendar repetition index */
    per_rep_idx = 0,  /* Calendar entry index, per repetition */
    entry_idx = 0;    /* Calendar entry index, global */
  ARAD_CFC_CALRX_TBL_DATA
    rx_cal;
  uint64
     field64;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_REC_CAL_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cal_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  /* Clear calendar */
  res = arad_fc_clear_calendar_unsafe(
          unit, 
          cal_mode_ndx, 
          ARAD_FC_CAL_TYPE_RX, 
          if_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* Calendar entries { */
  for (per_rep_idx = 0; per_rep_idx < cal_conf->cal_len; per_rep_idx++)
  {
    /* Convert destination enum to HW value */
    dest_sel = arad_fc_rec_cal_dest_type_to_val_internal(cal_buff[per_rep_idx].destination);

    if(dest_sel == -1)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_FC_INVALID_CALENDAR_ENTRY_ERR, (40 + per_rep_idx), exit);
    }
    else if (cal_buff[per_rep_idx].destination == SOC_TMC_FC_REC_CAL_DEST_NONE)
    {
      rx_cal.fc_dest_sel = dest_sel;
      rx_cal.fc_index = 0;
    }
    else if (cal_buff[per_rep_idx].destination == SOC_TMC_FC_REC_CAL_DEST_RETRANSMIT)
    {
      /* Set the ILKN Retransmit Req bits: 
       * Bit 0: ILKN-0, Bit 2: ILKN-1 */
      rx_cal.fc_dest_sel = dest_sel;
      rx_cal.fc_index = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ARAD_FC_RETRANSMIT_DEST_ILKN_A_VAL : ARAD_FC_RETRANSMIT_DEST_ILKN_B_VAL);
    }
    else
    {
      rx_cal.fc_dest_sel = dest_sel;
      rx_cal.fc_index = cal_buff[per_rep_idx].id;
    }

    if(cal_buff[per_rep_idx].destination == SOC_TMC_FC_REC_CAL_DEST_NIF_LL)
    {
      /* The NIF_LL entry need the following CFC EN bit for ILKN */
      if(ARAD_FC_CAL_MODE_IS_ILKN(cal_mode_ndx))
      {
        fld_val = cal_conf->enable;
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, ILKN_RX_TO_NIF_FAST_LLFC_ENf,  fld_val));
      }
      if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
      {
        fld_val = cal_conf->enable;
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, SPI_OOB_RX_TO_NIF_FAST_LLFC_ENf,  fld_val));
      }
      /*Set/Clear port bit in nif mask*/
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  95,  exit, ARAD_REG_ACCESS_ERR,READ_NBI_FC_LLFC_STOP_TX_FROM_CFC_MASKr(unit, &reg_val));
      if (cal_conf->enable) {
          SHR_BITSET(&reg_val,cal_buff[per_rep_idx].id);
      }
      else {
          SHR_BITCLR(&reg_val,cal_buff[per_rep_idx].id);
      }
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,WRITE_NBI_FC_LLFC_STOP_TX_FROM_CFC_MASKr(unit,  reg_val));
    }

    
    
    for (rep_idx = 0; rep_idx < cal_conf->cal_reps; rep_idx++)
    {
      entry_idx = (rep_idx * cal_conf->cal_len) + per_rep_idx;
      if(ARAD_FC_CAL_MODE_IS_ILKN(cal_mode_ndx))
      {
        res = arad_cfc_ilkn_calrx_tbl_set_unsafe(unit, if_ndx, entry_idx, &rx_cal);
      }
      if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
      {
        res = arad_cfc_oob_calrx_tbl_set_unsafe(unit, if_ndx, entry_idx, &rx_cal);
      }
      SOC_SAND_CHECK_FUNC_RESULT(res, 110 + entry_idx, exit);
    }
  }
  
  /*  Enable/disable */
  if(if_ndx == SOC_TMC_FC_OOB_ID_A)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  42,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_RX_0_SELf,  SOC_SAND_BOOL2NUM(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB)));
    COMPILER_64_SET(field64,0,SOC_SAND_BOOL2NUM((cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB) && cal_conf->enable));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  200,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, SPI_OOB_RX_0_ENf,  field64));
  }
  else if(if_ndx == SOC_TMC_FC_OOB_ID_B)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  44,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_RX_1_SELf,  SOC_SAND_BOOL2NUM(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB)));
    COMPILER_64_SET(field64,0,SOC_SAND_BOOL2NUM((cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB) && cal_conf->enable));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  210,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, SPI_OOB_RX_1_ENf,  field64));
  }

  /* Take the interface (A/B) out of reset if using ILKN/SPI OOB 
  if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB || cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB)
  {
    fld_val = SOC_SAND_BOOL2NUM(cal_conf->enable);
    if(if_ndx == SOC_TMC_FC_OOB_ID_A)
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  190,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, OOB_RX_0_RSTNf,  fld_val));
    }
    if(if_ndx == SOC_TMC_FC_OOB_ID_B)
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  191,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_OOB_PAD_CONFIGURATIONr, REG_PORT_ANY, 0, OOB_RX_1_RSTNf,  fld_val));
    }
  }*/

  if(if_ndx == SOC_TMC_FC_OOB_ID_A)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  46,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_RX_0_ENf,  SOC_SAND_BOOL2NUM(ARAD_FC_CAL_MODE_IS_ILKN(cal_mode_ndx) && cal_conf->enable)));
  }
  if(if_ndx == SOC_TMC_FC_OOB_ID_B)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  47,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, ILKN_RX_1_ENf,  SOC_SAND_BOOL2NUM(ARAD_FC_CAL_MODE_IS_ILKN(cal_mode_ndx) && cal_conf->enable)));
  }
  

  /* Set CFC Enablers for ILKN */
  fld_val = ARAD_FC_CAL_MODE_IS_ILKN(cal_mode_ndx);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  250,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, ILKN_RX_TO_EGQ_PFC_ENf,  SOC_SAND_BOOL2NUM(fld_val && cal_conf->enable)));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  251,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, ILKN_RX_TO_EGQ_PORT_ENf,  SOC_SAND_BOOL2NUM(fld_val && cal_conf->enable)));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  252,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, ILKN_RX_TO_GEN_PFC_ENf,  SOC_SAND_BOOL2NUM(fld_val && cal_conf->enable)));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  253,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, ILKN_RX_TO_NIF_FAST_LLFC_ENf,  SOC_SAND_BOOL2NUM(fld_val && cal_conf->enable)));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  254,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, ILKN_RX_TO_RET_REQ_ENf,  SOC_SAND_BOOL2NUM(fld_val && cal_conf->enable)));

  /* Set CFC Enablers for OOB */
  fld_val = SOC_SAND_BOOL2NUM(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  260,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, SPI_OOB_RX_TO_EGQ_PFC_ENf,  SOC_SAND_BOOL2NUM(fld_val && cal_conf->enable)));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  261,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, SPI_OOB_RX_TO_EGQ_PORT_ENf,  SOC_SAND_BOOL2NUM(fld_val && cal_conf->enable)));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  262,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, SPI_OOB_RX_TO_GEN_PFC_ENf,  SOC_SAND_BOOL2NUM(fld_val && cal_conf->enable)));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  263,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, SPI_OOB_RX_TO_NIF_FAST_LLFC_ENf,  SOC_SAND_BOOL2NUM(fld_val && cal_conf->enable)));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  264,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, SPI_OOB_RX_TO_RET_REQ_ENf,  SOC_SAND_BOOL2NUM(fld_val && cal_conf->enable)));

  COMPILER_64_SET(field64,0, 0x1);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  300,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, ECI_BLOCKS_SOFT_INITr, REG_PORT_ANY, 0, CFC_INITf,  field64));
  COMPILER_64_SET(field64,0, 0x0);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  301,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, ECI_BLOCKS_SOFT_INITr, REG_PORT_ANY, 0, CFC_INITf,  field64));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fc_egr_rec_oob_set_unsafe()",cal_mode_ndx,if_ndx);
}

uint32
  arad_fc_rec_cal_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_TMC_FC_REC_CALENDAR             *cal_buff
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_REC_CAL_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cal_mode_ndx, ARAD_CAL_MODE_NDX_MAX, ARAD_FC_CAL_MODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_rec_cal_set_verify()", cal_mode_ndx, if_ndx);
}

uint32
  arad_fc_rec_cal_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_REC_CAL_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cal_mode_ndx, ARAD_CAL_MODE_NDX_MAX, ARAD_FC_CAL_MODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(if_ndx, SOC_TMC_FC_NOF_OOB_IDS, ARAD_FC_CAL_IF_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_rec_cal_get_verify()", 0, if_ndx);
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_rec_cal_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_TMC_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_TMC_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_TMC_FC_REC_CALENDAR             *cal_buff
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_val = 0,
    sel_fld_val = 0,
    en_fld_val = 0;
  uint32
    entry_idx = 0;    /* Calendar entry index, global */
  ARAD_CFC_CALRX_TBL_DATA
    rx_cal;
  uint32
    en_fld = 0,
    sel_fld = 0,
    cal_len_reg=0,
    cal_len_fld=0,
    cal_rep_reg=0,
    cal_rep_fld=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_REC_CAL_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(cal_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  /* Calendar parameters { */

  /*  Enable/disable */
  en_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ILKN_RX_0_ENf : ILKN_RX_1_ENf);
  sel_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ILKN_RX_0_SELf : ILKN_RX_1_SELf);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  41,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, en_fld, &en_fld_val));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  42,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, sel_fld, &sel_fld_val));

  if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND)
  {
    cal_conf->enable = SOC_SAND_BOOL2NUM((en_fld_val == 1) && (sel_fld_val == 0)); /* ILKN Enable && Inband */
  }
  else if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB)
  {
    cal_conf->enable = SOC_SAND_BOOL2NUM((en_fld_val == 1) && (sel_fld_val == 1)); /* ILKN Enable && OOB */
  }
  else /* SOC_TMC_FC_CAL_MODE_SPI_OOB */
  {
    en_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_ENf : SPI_OOB_RX_1_ENf);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_SPI_OOB_RX_CONFIGURATION_0r, REG_PORT_ANY, 0, en_fld, &en_fld_val));
    cal_conf->enable = en_fld_val;
  }

  if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB)
  {
    cal_len_reg = CFC_ILKN_RX_CONFIGURATIONr;
    cal_len_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? ILKN_OOB_RX_0_CAL_LENf : ILKN_OOB_RX_1_CAL_LENf);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, cal_len_reg, REG_PORT_ANY, 0, cal_len_fld, &fld_val));
    cal_conf->cal_len = fld_val + 1;
  }
  if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
  {
    cal_len_reg = CFC_SPI_OOB_RX_CONFIGURATION_0r;
    cal_len_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_CAL_LENf : SPI_OOB_RX_1_CAL_LENf);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  31,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, cal_len_reg, REG_PORT_ANY, 0, cal_len_fld, &fld_val));
    cal_conf->cal_len = fld_val;
  } 
  if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND)
  {
    cal_len_reg = NBI_FC_ILKN_RX_CHFC_CAL_LENr;
    cal_len_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? FC_ILKN_RX_0_CHFC_CAL_LENf : FC_ILKN_RX_1_CHFC_CAL_LENf);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  32,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, cal_len_reg, REG_PORT_ANY, 0, cal_len_fld, &fld_val));
    cal_conf->cal_len = fld_val;
  }

  /*  Calendar number of repetitions - SPI/OOB Only */
  if(ARAD_FC_CAL_MODE_IS_ILKN(cal_mode_ndx))
  {
    cal_conf->cal_reps = 1;
  }
  else
  {
    cal_rep_reg = CFC_SPI_OOB_RX_CONFIGURATION_0r;
    cal_rep_fld = (if_ndx == SOC_TMC_FC_OOB_ID_A ? SPI_OOB_RX_0_CAL_Mf : SPI_OOB_RX_1_CAL_Mf);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  31,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, cal_rep_reg, REG_PORT_ANY, 0, cal_rep_fld, &fld_val));
    cal_conf->cal_reps = fld_val;
  }
  
  /* Calendar parameters } */

  /* Calendar entries { */

  for (entry_idx = 0; entry_idx < cal_conf->cal_len; entry_idx++)
  {
    if(ARAD_FC_CAL_MODE_IS_ILKN(cal_mode_ndx))
    {
      res = arad_cfc_ilkn_calrx_tbl_get_unsafe(unit, if_ndx, entry_idx, &rx_cal);
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    } else if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB) {
      res = arad_cfc_oob_calrx_tbl_get_unsafe(unit, if_ndx, entry_idx, &rx_cal);
      SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);
    } else {
        SOC_SAND_SET_ERROR_CODE(ARAD_FC_INVALID_CALENDAR_ENTRY_ERR, 42, exit);
    }

    if(rx_cal.fc_dest_sel >= ARAD_FC_REC_CAL_DEST_ARR_SIZE)
    {
      cal_buff[entry_idx].destination = SOC_TMC_FC_REC_CAL_DEST_NONE;
      cal_buff[entry_idx].id = 0;
    }
    else if(arad_fc_rec_cal_dest_arr[rx_cal.fc_dest_sel] == SOC_TMC_FC_REC_CAL_DEST_RETRANSMIT)
    {
      /* Set the ILKN Retransmit Req bits: 
       * Bit 0: ILKN-0, Bit 2: ILKN-1 */
      cal_buff[entry_idx].destination = arad_fc_rec_cal_dest_arr[rx_cal.fc_dest_sel];
      cal_buff[entry_idx].id = (rx_cal.fc_index == ARAD_FC_RETRANSMIT_DEST_ILKN_A_VAL ? ARAD_FC_ILKN_ID_A : ARAD_FC_ILKN_ID_B);
    }
    else
    {
      cal_buff[entry_idx].destination = arad_fc_rec_cal_dest_arr[rx_cal.fc_dest_sel];
      cal_buff[entry_idx].id = rx_cal.fc_index;
    }
  }
  /* Calendar entries } */
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fc_rec_cal_get_unsafe()",0,0);
}

/*********************************************************************
*     Defines if and how LLFC can be received/generated using
 *     Interlaken NIF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_ilkn_llfc_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION             direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_LLFC_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ilkn_bit_offset;
  uint32
    on_ch0 =0 ,
    every_16_chs = 0,
    multiple_use_bits_fld = 0,
    multiple_use_bits_reg = 0;
  uint32
    buffer;
  uint64
      field64;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_LLFC_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  if (ARAD_IS_DIRECTION_REC(direction_ndx))
  {
    on_ch0                 = FC_ILKN_RX_LLFC_ON_CH_0f;
    every_16_chs           = FC_ILKN_RX_LLFC_EVERY_16_CHSf;
    multiple_use_bits_fld = (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_RX_0_LLFC_STOP_TX_FROM_MULTIPLE_USE_BITS_MASKf : ILKN_RX_1_LLFC_STOP_TX_FROM_MULTIPLE_USE_BITS_MASKf;
    multiple_use_bits_reg = (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? NBI_ILKN_0_MULTIPLE_USE_BITSr : NBI_ILKN_1_MULTIPLE_USE_BITSr;
  }
  if (ARAD_IS_DIRECTION_GEN(direction_ndx))
  {
    on_ch0                 = FC_ILKN_TX_GEN_LLFC_ON_CH_0f;
    every_16_chs           = FC_ILKN_TX_GEN_LLFC_EVERY_16_CHSf;
    multiple_use_bits_fld = (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_TX_0_MULTIPLE_USE_BITS_FROM_LLFC_MASKf : ILKN_TX_1_MULTIPLE_USE_BITS_FROM_LLFC_MASKf;
    multiple_use_bits_reg = (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? NBI_ILKN_0_MULTIPLE_USE_BITSr : NBI_ILKN_1_MULTIPLE_USE_BITSr;
  }

  ilkn_bit_offset = ARAD_NIF_ID_OFFSET(ILKN, ilkn_ndx);
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, NBI_FC_ILKNr, REG_PORT_ANY, 0, on_ch0, &buffer));
  buffer &= SOC_SAND_RBIT(ilkn_bit_offset);
  buffer |= SOC_SAND_SET_FLD_IN_PLACE(
              info->cal_channel == SOC_TMC_FC_ILKN_CAL_LLFC_CH_0 ? 1 : 0,
              ilkn_bit_offset,
              SOC_SAND_BIT(ilkn_bit_offset)
            );
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, NBI_FC_ILKNr, REG_PORT_ANY, 0, on_ch0,  buffer));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, NBI_FC_ILKNr, REG_PORT_ANY, 0, every_16_chs, &buffer));
  buffer &= SOC_SAND_RBIT(ilkn_bit_offset);
  buffer |= SOC_SAND_SET_FLD_IN_PLACE(
              info->cal_channel == SOC_TMC_FC_ILKN_CAL_LLFC_CH_16N ? 1 : 0,
              ilkn_bit_offset,
              SOC_SAND_BIT(ilkn_bit_offset)
            );
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, NBI_FC_ILKNr, REG_PORT_ANY, 0, every_16_chs,  buffer));
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, NBI_FC_ILKNr, REG_PORT_ANY, 0, FC_ILKN_MODEf, &buffer));
  buffer &= SOC_SAND_RBIT(ilkn_bit_offset);
  buffer |= SOC_SAND_SET_FLD_IN_PLACE(
              info->cal_channel != SOC_TMC_FC_ILKN_CAL_LLFC_NONE ? 1 : 0,
              ilkn_bit_offset,
              SOC_SAND_BIT(ilkn_bit_offset)
            );
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  31,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, NBI_FC_ILKNr, REG_PORT_ANY, 0, FC_ILKN_MODEf,  buffer));
  
  buffer = info->multi_use_mask;
  COMPILER_64_SET(field64,0, buffer);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, multiple_use_bits_reg, REG_PORT_ANY, 0, multiple_use_bits_fld,  field64));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_llfc_set_unsafe()", ilkn_ndx, 0);
}

uint32
  arad_fc_ilkn_llfc_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION             direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_LLFC_INFO           *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_LLFC_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(ilkn_ndx, ARAD_ILKN_NDX_MIN, ARAD_ILKN_NDX_MAX, ARAD_ILKN_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_llfc_set_verify()", ilkn_ndx, 0);
}

uint32
  arad_fc_ilkn_llfc_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_LLFC_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(ilkn_ndx, ARAD_ILKN_NDX_MIN, ARAD_ILKN_NDX_MAX, ARAD_ILKN_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_llfc_get_verify()", ilkn_ndx, 0);
}

/*********************************************************************
*     Defines if and how LLFC can be received/generated using
 *     Interlaken NIF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_ilkn_llfc_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_OUT ARAD_FC_ILKN_LLFC_INFO           *rec_info,
    SOC_SAND_OUT ARAD_FC_ILKN_LLFC_INFO           *gen_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ilkn_bit_offset;
  uint32
    buffer;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_LLFC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rec_info);
  SOC_SAND_CHECK_NULL_INPUT(gen_info);

  SOC_TMC_FC_ILKN_LLFC_INFO_clear(rec_info);
  SOC_TMC_FC_ILKN_LLFC_INFO_clear(gen_info);

  ilkn_bit_offset = ARAD_NIF_ID_OFFSET(ILKN, ilkn_ndx);

  /*
   *  Determine Rx type
   */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, NBI_FC_ILKNr, REG_PORT_ANY, 0, FC_ILKN_RX_LLFC_ON_CH_0f, &buffer));
  if (SOC_SAND_GET_FLD_FROM_PLACE(buffer, ilkn_bit_offset, SOC_SAND_BIT(ilkn_bit_offset)) == 1)
  {
    rec_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_CH_0;
  }
  else
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, NBI_FC_ILKNr, REG_PORT_ANY, 0, FC_ILKN_RX_LLFC_EVERY_16_CHSf, &buffer));
    if (SOC_SAND_GET_FLD_FROM_PLACE(buffer, ilkn_bit_offset, SOC_SAND_BIT(ilkn_bit_offset)) == 1)
    {
      rec_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_CH_16N;
    }
    else
    {
      rec_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_NONE;
    }
  }

  /*
   *  Determine Tx type
   */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, NBI_FC_ILKNr, REG_PORT_ANY, 0, FC_ILKN_TX_GEN_LLFC_ON_CH_0f, &buffer));
  if (SOC_SAND_GET_FLD_FROM_PLACE(buffer, ilkn_bit_offset, SOC_SAND_BIT(ilkn_bit_offset)) == 1)
  {
    gen_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_CH_0;
  }
  else
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, NBI_FC_ILKNr, REG_PORT_ANY, 0, FC_ILKN_TX_GEN_LLFC_EVERY_16_CHSf, &buffer));
    if (SOC_SAND_GET_FLD_FROM_PLACE(buffer, ilkn_bit_offset, SOC_SAND_BIT(ilkn_bit_offset)) == 1)
    {
      gen_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_CH_16N;
    }
    else
    {
      gen_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_NONE;
    }
  }

  if (ilkn_bit_offset == ARAD_FC_ILKN_ID_A)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, NBI_ILKN_0_MULTIPLE_USE_BITSr, REG_PORT_ANY, 0, ILKN_RX_0_MULTIPLE_USE_BITS_VALUEf, &buffer));
    rec_info->multi_use_mask = (uint8) buffer;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, NBI_ILKN_0_MULTIPLE_USE_BITSr, REG_PORT_ANY, 0, ILKN_TX_0_MULTIPLE_USE_BITS_VALUEf, &buffer));
    gen_info->multi_use_mask = (uint8) buffer;
  }
  if (ilkn_bit_offset == ARAD_FC_ILKN_ID_B)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, NBI_ILKN_1_MULTIPLE_USE_BITSr, REG_PORT_ANY, 0, ILKN_RX_1_MULTIPLE_USE_BITS_VALUEf, &buffer));
    rec_info->multi_use_mask = (uint8) buffer;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, NBI_ILKN_1_MULTIPLE_USE_BITSr, REG_PORT_ANY, 0, ILKN_TX_1_MULTIPLE_USE_BITS_VALUEf, &buffer));
    gen_info->multi_use_mask = (uint8) buffer;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_llfc_get_unsafe()", ilkn_ndx, 0);
}

uint32
  arad_fc_ilkn_retransmit_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_RETRANSMIT_INFO     *ret_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    rx_polarity_field[2] = {ILKN_OOB_RX_0_RT_POLARITYf, ILKN_OOB_RX_1_RT_POLARITYf},
    tx_polarity_field[2] = {ILKN_OOB_TX_0_RT_POLARITYf, ILKN_OOB_TX_1_RT_POLARITYf},
    err_indication_field[2] = {ILKN_OOB_RX_0_ERR_RT_STATUS_SELf, ILKN_OOB_RX_1_ERR_RT_STATUS_SELf};
  uint32
    reg_index = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_RETRANSMIT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ret_info);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, ILKN_RX_TO_RET_REQ_ENf,  ret_info->enable));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, SPI_OOB_RX_TO_RET_REQ_ENf,  ret_info->enable));

  if(ilkn_ndx == ARAD_NIF_ILKN_ID_A)
  {
    reg_index = 0;
  }
  if(ilkn_ndx == ARAD_NIF_ILKN_ID_B)
  {
    reg_index = 1;
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_POLARITY_CFGr, REG_PORT_ANY, 0, rx_polarity_field[reg_index],  ret_info->rx_polarity));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_POLARITY_CFGr, REG_PORT_ANY, 0, tx_polarity_field[reg_index],  ret_info->tx_polarity));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, err_indication_field[reg_index],  ret_info->error_indication));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_retransmit_set_unsafe()", ilkn_ndx, 0);
}

uint32
  arad_fc_ilkn_retransmit_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_OUT ARAD_FC_ILKN_RETRANSMIT_INFO     *ret_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    rx_polarity_field[2] = {ILKN_OOB_RX_0_RT_POLARITYf, ILKN_OOB_RX_1_RT_POLARITYf},
    tx_polarity_field[2] = {ILKN_OOB_TX_0_RT_POLARITYf, ILKN_OOB_TX_1_RT_POLARITYf},
    err_indication_field[2] = {ILKN_OOB_RX_0_ERR_RT_STATUS_SELf, ILKN_OOB_RX_1_ERR_RT_STATUS_SELf};
  uint32
    reg_index = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_RETRANSMIT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ret_info);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_CFC_ENABLERSr, REG_PORT_ANY, 0, ILKN_RX_TO_RET_REQ_ENf, &ret_info->enable));

  if(ilkn_ndx == ARAD_NIF_ILKN_ID_A)
  {
    reg_index = 0;
  }
  if(ilkn_ndx == ARAD_NIF_ILKN_ID_B)
  {
    reg_index = 1;
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_OOB_POLARITY_CFGr, REG_PORT_ANY, 0, rx_polarity_field[reg_index], &ret_info->rx_polarity));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_OOB_POLARITY_CFGr, REG_PORT_ANY, 0, tx_polarity_field[reg_index], &ret_info->tx_polarity));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_RX_CONFIGURATIONr, REG_PORT_ANY, 0, err_indication_field[reg_index], &ret_info->error_indication));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_retransmit_set_unsafe()", ilkn_ndx, 0);
}

uint32
  arad_fc_ilkn_mub_rec_set_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  uint8                            bitmap
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_MUB_REC_SET_UNSAFE);
  
  if(ilkn_ndx == ARAD_NIF_ILKN_ID_A)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_MUB_ENABLEr, REG_PORT_ANY, 0, ILKN_0_MUB_RX_ENAf,  bitmap));
  } else {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_MUB_ENABLEr, REG_PORT_ANY, 0, ILKN_1_MUB_RX_ENAf,  bitmap));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_mub_rec_set_unsafe()", ilkn_ndx, 0);
}

uint32
  arad_fc_ilkn_mub_rec_get_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_OUT uint8                            *bitmap
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_val;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_MUB_REC_GET_UNSAFE);

  if(ilkn_ndx == ARAD_NIF_ILKN_ID_A)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_MUB_ENABLEr, REG_PORT_ANY, 0, ILKN_0_MUB_RX_ENAf, &fld_val));
  } else {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_MUB_ENABLEr, REG_PORT_ANY, 0, ILKN_1_MUB_RX_ENAf, &fld_val));
  }


  *bitmap = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_mub_rec_get_unsafe()", ilkn_ndx, 0);
}

uint32
  arad_fc_ilkn_mub_gen_cal_set_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  SOC_TMC_FC_ILKN_MUB_GEN_CAL      *cal_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32 
    calendars[ARAD_NIF_NOF_ILKN_IDS] = 
                 {CFC_ILKN_0_MUB_TX_CALr, CFC_ILKN_1_MUB_TX_CALr};
  uint32
    fields_0[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
                 {ILKN_0_MUB_TX_MAP_0f, ILKN_0_MUB_TX_MAP_1f, ILKN_0_MUB_TX_MAP_2f, ILKN_0_MUB_TX_MAP_3f,
                  ILKN_0_MUB_TX_MAP_4f, ILKN_0_MUB_TX_MAP_5f, ILKN_0_MUB_TX_MAP_6f, ILKN_0_MUB_TX_MAP_7f};
  
  uint32
    fields_1[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
                 {ILKN_1_MUB_TX_MAP_0f, ILKN_1_MUB_TX_MAP_1f, ILKN_1_MUB_TX_MAP_2f, ILKN_1_MUB_TX_MAP_3f,
                  ILKN_1_MUB_TX_MAP_4f, ILKN_1_MUB_TX_MAP_5f, ILKN_1_MUB_TX_MAP_6f, ILKN_1_MUB_TX_MAP_7f};

  uint32
    enabler_field[2] = {ILKN_0_MUB_TX_ENAf, ILKN_1_MUB_TX_ENAf};

  soc_reg_above_64_val_t  
    cal_data, entry_data;
  uint32
    entry_idx, fld_val;
  uint32
    ilkn_id_offset = ARAD_NIF_ID_OFFSET(ILKN, ilkn_ndx);

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_MUB_GEN_CAL_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(cal_info);
  SOC_REG_ABOVE_64_CLEAR(cal_data);

  /* Set calendar data */
  for(entry_idx = 0; entry_idx < SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN; entry_idx++)
  {
    /* init */
    SOC_REG_ABOVE_64_CLEAR(entry_data);
    /* set source (bits 11:9) */
    entry_data[0] = arad_fc_gen_cal_src_type_to_val_internal(cal_info->entries[entry_idx].source) << ARAD_FC_ILKN_MUB_TX_CAL_SOURCE_ENTRY_OFFSET;
    /* set index (bits 8:0) */
    entry_data[0] |= (cal_info->entries[entry_idx].id & SOC_SAND_BITS_MASK(ARAD_FC_ILKN_MUB_TX_CAL_SOURCE_ENTRY_OFFSET-1, 0));

    /* update the calendar data with the entry*/
    if(ilkn_ndx == ARAD_NIF_ILKN_ID_A) {
      soc_reg_above_64_field_set(unit, calendars[ilkn_id_offset], cal_data, fields_0[entry_idx], entry_data);
    } else {
      soc_reg_above_64_field_set(unit, calendars[ilkn_id_offset], cal_data, fields_1[entry_idx], entry_data);
    }

    /* update the multi-use bits enalers/mask */
    /* set to 0 if source is <CONST,0> */
    /* set to 1 otherwise */
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_MUB_ENABLEr, REG_PORT_ANY, 0, enabler_field[ilkn_id_offset], &fld_val)); 
    if(cal_info->entries[entry_idx].source == SOC_TMC_FC_GEN_CAL_SRC_CONST && cal_info->entries[entry_idx].id == 0)
    {
      fld_val &= ~(SOC_SAND_BIT(entry_idx)); /* set bit to 0 */
    }
    else
    {
      fld_val |= SOC_SAND_BIT(entry_idx); /* set bit to 1 */
    }
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_MUB_ENABLEr, REG_PORT_ANY, 0, enabler_field[ilkn_id_offset],  fld_val));
  }
  
  /* Write the calendar */
  res = soc_reg_above_64_set(unit, calendars[ilkn_id_offset], REG_PORT_ANY, 0, entry_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_mub_gen_cal_set_unsafe()", ilkn_ndx, 0);
}

uint32
  arad_fc_ilkn_mub_gen_cal_get_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_OUT SOC_TMC_FC_ILKN_MUB_GEN_CAL      *cal_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32 
    calendars[ARAD_NIF_NOF_ILKN_IDS] = 
                 {CFC_ILKN_0_MUB_TX_CALr, CFC_ILKN_1_MUB_TX_CALr};
  uint32
    fields_0[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
                 {ILKN_0_MUB_TX_MAP_0f, ILKN_0_MUB_TX_MAP_1f, ILKN_0_MUB_TX_MAP_2f, ILKN_0_MUB_TX_MAP_3f,
                  ILKN_0_MUB_TX_MAP_4f, ILKN_0_MUB_TX_MAP_5f, ILKN_0_MUB_TX_MAP_6f, ILKN_0_MUB_TX_MAP_7f};
  
  uint32
    fields_1[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN] = 
                 {ILKN_1_MUB_TX_MAP_0f, ILKN_1_MUB_TX_MAP_1f, ILKN_1_MUB_TX_MAP_2f, ILKN_1_MUB_TX_MAP_3f,
                  ILKN_1_MUB_TX_MAP_4f, ILKN_1_MUB_TX_MAP_5f, ILKN_1_MUB_TX_MAP_6f, ILKN_1_MUB_TX_MAP_7f};
  
  soc_reg_above_64_val_t  
    cal_data, entry_data;
  uint32
    entry_idx;
  uint32
    ilkn_id_offset = ARAD_NIF_ID_OFFSET(ILKN, ilkn_ndx);


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_MUB_GEN_CAL_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(cal_info);
  SOC_REG_ABOVE_64_CLEAR(cal_data);

  /* Read the calendar */
  res = soc_reg_above_64_get(unit, calendars[ilkn_id_offset], REG_PORT_ANY, 0, entry_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Get calendar data */
  for(entry_idx = 0; entry_idx < SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN; entry_idx++)
  {
    if(ilkn_ndx == ARAD_NIF_ILKN_ID_A) {
      soc_reg_above_64_field_get(unit, calendars[ilkn_id_offset], cal_data, fields_0[entry_idx], entry_data);
    } else {
      soc_reg_above_64_field_get(unit, calendars[ilkn_id_offset], cal_data, fields_1[entry_idx], entry_data);
    }

    /* get source (bits 11:9) */
    cal_info->entries[entry_idx].source = arad_fc_gen_cal_src_arr[entry_data[0] >> ARAD_FC_ILKN_MUB_TX_CAL_SOURCE_ENTRY_OFFSET];

    /* get index (bits 8:0) */
    cal_info->entries[entry_idx].id = (entry_data[0] & SOC_SAND_BITS_MASK(ARAD_FC_ILKN_MUB_TX_CAL_SOURCE_ENTRY_OFFSET-1, 0));
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_mub_gen_cal_get_unsafe()", ilkn_ndx, 0);
}

uint32
  arad_fc_pfc_generic_bitmap_verify(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   uint32    pfc_bitmap_index
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_PFC_GENERIC_BITMAP_VERIFY);

  if(pfc_bitmap_index >= SOC_DPP_DEFS_GET(unit, nof_fc_pfc_generic_bitmaps))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_FC_PFC_GENERIC_BITMAP_OUT_OF_RANGE, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_pfc_generic_bitmap_verify()", pfc_bitmap_index, 0);
}

uint32
  arad_fc_pfc_generic_bitmap_set_unsafe(
    SOC_SAND_IN   int                     unit,
    SOC_SAND_IN   uint32                      pfc_bitmap_index,
    SOC_SAND_IN   SOC_TMC_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_offset;
  soc_reg_above_64_val_t  
    pfc_map_reg;
  uint32
    bitmaps[SOC_DPP_DEFS_MAX(NOF_FC_PFC_GENERIC_BITMAPS)] = 
    {
      CFC_PFC_GENERIC_BITMAP_0r, CFC_PFC_GENERIC_BITMAP_1r, CFC_PFC_GENERIC_BITMAP_2r, CFC_PFC_GENERIC_BITMAP_3r,
      CFC_PFC_GENERIC_BITMAP_4r, CFC_PFC_GENERIC_BITMAP_5r, CFC_PFC_GENERIC_BITMAP_6r, CFC_PFC_GENERIC_BITMAP_7r,
      CFC_PFC_GENERIC_BITMAP_8r, CFC_PFC_GENERIC_BITMAP_9r, CFC_PFC_GENERIC_BITMAP_10r, CFC_PFC_GENERIC_BITMAP_11r,
      CFC_PFC_GENERIC_BITMAP_12r, CFC_PFC_GENERIC_BITMAP_13r, CFC_PFC_GENERIC_BITMAP_14r, CFC_PFC_GENERIC_BITMAP_15r
    };

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_PFC_GENERIC_BITMAP_SET_UNSAFE);
  SOC_REG_ABOVE_64_CLEAR(pfc_map_reg);

  /* Read the selected Generic Bitmap */
  res = soc_reg_above_64_get(unit, bitmaps[pfc_bitmap_index], REG_PORT_ANY, 0, pfc_map_reg);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  
  /* Update the Generic Bitmap */
  for(reg_offset = 0; reg_offset < ARAD_FC_PFC_GENERIC_BITMAP_SIZE / 32; reg_offset++)
  {
    pfc_map_reg[reg_offset] = pfc_bitmap->bitmap[reg_offset];
  }

  /* Write the updated Generic Bitmap */
  res = soc_reg_above_64_set(unit, bitmaps[pfc_bitmap_index], REG_PORT_ANY, 0, pfc_map_reg);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_pfc_generic_bitmap_set_unsafe()", pfc_bitmap_index, 0);
}

uint32
  arad_fc_pfc_generic_bitmap_get_unsafe(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   uint32    pfc_bitmap_index,
    SOC_SAND_OUT   SOC_TMC_FC_PFC_GENERIC_BITMAP    *pfc_bitmap
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_offset;
  soc_reg_above_64_val_t  
    pfc_map_reg;
  uint32
    bitmaps[SOC_DPP_DEFS_MAX(NOF_FC_PFC_GENERIC_BITMAPS)] = 
    {
      CFC_PFC_GENERIC_BITMAP_0r, CFC_PFC_GENERIC_BITMAP_1r, CFC_PFC_GENERIC_BITMAP_2r, CFC_PFC_GENERIC_BITMAP_3r,
      CFC_PFC_GENERIC_BITMAP_4r, CFC_PFC_GENERIC_BITMAP_5r, CFC_PFC_GENERIC_BITMAP_6r, CFC_PFC_GENERIC_BITMAP_7r,
      CFC_PFC_GENERIC_BITMAP_8r, CFC_PFC_GENERIC_BITMAP_9r, CFC_PFC_GENERIC_BITMAP_10r, CFC_PFC_GENERIC_BITMAP_11r,
      CFC_PFC_GENERIC_BITMAP_12r, CFC_PFC_GENERIC_BITMAP_13r, CFC_PFC_GENERIC_BITMAP_14r, CFC_PFC_GENERIC_BITMAP_15r
    };

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_PFC_GENERIC_BITMAP_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(pfc_bitmap);
  SOC_REG_ABOVE_64_CLEAR(pfc_map_reg);

  /* Read the selected Generic Bitmap */
  res = soc_reg_above_64_get(unit, bitmaps[pfc_bitmap_index], REG_PORT_ANY, 0, pfc_map_reg);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  /* Get the requested bitmap */
  for(reg_offset = 0; reg_offset < ARAD_FC_PFC_GENERIC_BITMAP_SIZE / 32; reg_offset++)
  {
    pfc_bitmap->bitmap[reg_offset] = pfc_map_reg[reg_offset];
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_pfc_generic_bitmap_get_unsafe()", pfc_bitmap_index, 0);
}

uint32
  arad_fc_hcfc_bitmap_set_unsafe(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   uint32   hcfc_bitmap_index,
    SOC_SAND_IN   SOC_TMC_FC_PFC_GENERIC_BITMAP    *hcfc_bitmap
  )
{
  uint32
    res = SOC_SAND_OK,
    offset;
  ARAD_CFC_HCFC_BITMAP_TBL_DATA  
    hcfc_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_HCFC_BITMAP_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(hcfc_bitmap);
   
  /* Update the Bitmap */
  for(offset = 0; offset < ARAD_CFC_HCFC_BITMAP_TBL_SIZE; offset++)
  {
    hcfc_tbl_data.bitmap[offset] = hcfc_bitmap->bitmap[offset];
  }

  /* Write the updated Bitmap */
  res = arad_cfc_hcfc_bitmap_tbl_set_unsafe(unit, hcfc_bitmap_index, &hcfc_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_hcfc_bitmap_set_unsafe()", hcfc_bitmap_index, 0);
}

uint32
  arad_fc_hcfc_bitmap_get_unsafe(
    SOC_SAND_IN   int   unit,
    SOC_SAND_IN   uint32   hcfc_bitmap_index,
    SOC_SAND_OUT  SOC_TMC_FC_PFC_GENERIC_BITMAP    *hcfc_bitmap
  )
{
  uint32
    res = SOC_SAND_OK,
    offset;
  ARAD_CFC_HCFC_BITMAP_TBL_DATA  
    hcfc_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_HCFC_BITMAP_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(hcfc_bitmap);

  /* Read the selected Bitmap */
  res = arad_cfc_hcfc_bitmap_tbl_get_unsafe(unit, hcfc_bitmap_index, &hcfc_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
  for(offset = 0; offset < ARAD_CFC_HCFC_BITMAP_TBL_SIZE; offset++)
  {
    hcfc_bitmap->bitmap[offset] = hcfc_tbl_data.bitmap[offset];
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_hcfc_bitmap_get_unsafe()", hcfc_bitmap_index, 0);
}



#if defined(BCM_88650_B0)

uint32
  arad_fc_ilkn_retransmit_cal_set_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                   ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION                  direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_RETRANSMIT_CAL_CFG    *cal_cfg
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_RETRANSMIT_CAL_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(cal_cfg);

  if(SOC_IS_ARAD_B0_AND_ABOVE(unit))
  {
    if (ARAD_IS_DIRECTION_REC(direction_ndx))
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_RX_RT_CAL_CFGr, REG_PORT_ANY, 0, (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_OOB_RX_0_RT_CAL_ENf : ILKN_OOB_RX_1_RT_CAL_ENf,  cal_cfg->enable));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_RX_RT_CAL_CFGr, REG_PORT_ANY, 0, (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_OOB_RX_0_RT_CAL_LENf : ILKN_OOB_RX_1_RT_CAL_LENf,  cal_cfg->length - 1));
    }
    if (ARAD_IS_DIRECTION_GEN(direction_ndx))
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_TX_RT_CAL_CFGr, REG_PORT_ANY, 0, (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_OOB_TX_0_RT_CAL_ENf : ILKN_OOB_TX_1_RT_CAL_ENf,  cal_cfg->enable));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CFC_ILKN_OOB_TX_RT_CAL_CFGr, REG_PORT_ANY, 0, (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_OOB_TX_0_RT_CAL_LENf : ILKN_OOB_TX_1_RT_CAL_LENf,  cal_cfg->length - 1));
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_retransmit_cal_set_unsafe()", ilkn_ndx, direction_ndx);
}

uint32
  arad_fc_ilkn_retransmit_cal_get_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                   ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION                  direction_ndx,
    SOC_SAND_OUT ARAD_FC_ILKN_RETRANSMIT_CAL_CFG    *cal_cfg
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_RETRANSMIT_CAL_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(cal_cfg);

  if(SOC_IS_ARAD_B0_AND_ABOVE(unit))
  {
    if (ARAD_IS_DIRECTION_REC(direction_ndx))
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_OOB_RX_RT_CAL_CFGr, REG_PORT_ANY, 0, (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_OOB_RX_0_RT_CAL_ENf : ILKN_OOB_RX_1_RT_CAL_ENf, &cal_cfg->enable));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_OOB_RX_RT_CAL_CFGr, REG_PORT_ANY, 0, (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_OOB_RX_0_RT_CAL_LENf : ILKN_OOB_RX_1_RT_CAL_LENf, &fld_val));
      cal_cfg->length = fld_val + 1;
    }
    if (ARAD_IS_DIRECTION_GEN(direction_ndx))
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_OOB_TX_RT_CAL_CFGr, REG_PORT_ANY, 0, (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_OOB_TX_0_RT_CAL_ENf : ILKN_OOB_TX_1_RT_CAL_ENf, &cal_cfg->enable));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CFC_ILKN_OOB_TX_RT_CAL_CFGr, REG_PORT_ANY, 0, (ilkn_ndx == ARAD_NIF_ILKN_ID_A) ? ILKN_OOB_TX_0_RT_CAL_LENf : ILKN_OOB_TX_1_RT_CAL_LENf, &fld_val));
      cal_cfg->length = fld_val + 1;
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_retransmit_cal_get_unsafe()", ilkn_ndx, direction_ndx);
}

uint32
  arad_fc_ilkn_retransmit_cal_set_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  ARAD_NIF_ILKN_ID                   ilkn_ndx,
    SOC_SAND_IN  ARAD_FC_DIRECTION                  direction_ndx,
    SOC_SAND_IN  ARAD_FC_ILKN_RETRANSMIT_CAL_CFG    *cal_cfg
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_ILKN_RETRANSMIT_CAL_SET_VERIFY);
  SOC_SAND_CHECK_NULL_INPUT(cal_cfg);

  if(SOC_IS_ARAD_B0_AND_ABOVE(unit))
  {
    if(cal_cfg->length < ARAD_FC_ILKN_RETRANSMIT_CAL_LEN_MIN || cal_cfg->length > ARAD_FC_ILKN_RETRANSMIT_CAL_LEN_MAX)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_FC_ILKN_RETRANSMIT_CAL_LEN_RANGE_ERR, 10, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_ilkn_retransmit_cal_set_verify()", ilkn_ndx, direction_ndx);
}

#endif

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_nif_pause_frame_src_addr_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      nif_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS     *mac_addr
  )
{
  uint32
    res = SOC_SAND_OK,
    i = 0;
  uint64
    mac;
  uint32
    nif_id,
    nif_intern_id,
    mac_high = 0,
    mac_low = 0,
    index = 0;
  uint32
    wc_ndx,
    clp_port_idx = ARAD_NIF_NOF_CLP_PORTS,
    xlp_port_idx = ARAD_NIF_NOF_XLP_PORTS;
  ARAD_NIF_TYPE
    nif_type;  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET_UNSAFE);
  COMPILER_64_ZERO(mac);
  SOC_SAND_CHECK_NULL_INPUT(mac_addr);

  /* loop on indexes 0-3 */
  for(i = 0; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - 2; i++)
  {
    /* mac_low will include the first 32 bits of the mac address */
    mac_low |= (mac_addr->address[i] << (i * 8));
  }

  /* loop on indexes 4-5 */
  for(i = SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - 2; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; i++)
  {
    /* mac_high will include the last 16 bits of the mac address */
    index = i - (SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - 2);
    mac_high |= (mac_addr->address[i] << (index * 8));
  }

  /* Create a 64 bit variable with the entire mac address (48 bits) */
  COMPILER_64_SET(mac, mac_high, mac_low);

  /* Indexing */
  nif_id = arad_nif2intern_id(unit, nif_ndx);
  nif_type = arad_nif_id2type(nif_ndx);
  nif_intern_id = nif_id;
  wc_ndx = ARAD_NIF2WC_GLBL_ID(nif_intern_id);
  xlp_port_idx = ARAD_NIF_XLP_NDX(wc_ndx);
  clp_port_idx = ARAD_NIF_CLP_NDX(wc_ndx);

  if (clp_port_idx < ARAD_NIF_NOF_CLP_PORTS && nif_type != ARAD_NIF_TYPE_ILKN) {
      if (ARAD_NIF_IS_CMAC(nif_type)) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CLP_CMAC_TX_MAC_SAr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, CTRL_SAf,  mac));
      } else {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CLP_XMAC_TX_MAC_SAr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, CTRL_SAf,  mac));
      }
  }

  if (xlp_port_idx < ARAD_NIF_NOF_XLP_PORTS) {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, XLP_XMAC_TX_MAC_SAr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, CTRL_SAf,  mac));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_nif_pause_frame_src_addr_set_unsafe()", nif_ndx, 0);
}

uint32
  arad_fc_nif_pause_frame_src_addr_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      nif_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_NIF_PAUSE_FRAME_SRC_ADDR_GET_VERIFY);

  res = arad_nif_id_verify(
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_nif_pause_frame_src_addr_get_verify()", nif_ndx, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_fc_nif_pause_frame_src_addr_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      nif_ndx,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS            *mac_addr
  )
{
  uint32
    res = SOC_SAND_OK,
    i = 0;
  uint64
    mac;
  uint32
    nif_id,
    nif_intern_id,
    mask,
    index = 0;
  uint32
    wc_ndx,
    clp_port_idx = ARAD_NIF_NOF_CLP_PORTS,
    xlp_port_idx = ARAD_NIF_NOF_XLP_PORTS;
  ARAD_NIF_TYPE
    nif_type;  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET_UNSAFE);
  COMPILER_64_ZERO(mac);
  SOC_SAND_CHECK_NULL_INPUT(mac_addr);

  /* Indexing */
  nif_id = arad_nif2intern_id(unit, nif_ndx);
  nif_type = arad_nif_id2type(nif_ndx);
  nif_intern_id = nif_id;
  wc_ndx = ARAD_NIF2WC_GLBL_ID(nif_intern_id);
  xlp_port_idx = ARAD_NIF_XLP_NDX(wc_ndx);
  clp_port_idx = ARAD_NIF_CLP_NDX(wc_ndx);

  if (clp_port_idx < ARAD_NIF_NOF_CLP_PORTS && nif_type != ARAD_NIF_TYPE_ILKN) {
      if (ARAD_NIF_IS_CMAC(nif_type)) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, CLP_CMAC_TX_MAC_SAr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, CTRL_SAf, &mac));
      } else {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, CLP_XMAC_TX_MAC_SAr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, CTRL_SAf, &mac));
      }
  }

  if (xlp_port_idx < ARAD_NIF_NOF_XLP_PORTS) {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, XLP_XMAC_TX_MAC_SAr,  SOC_INFO((unit)).port_p2l_mapping[nif_intern_id+1],  0, CTRL_SAf, &mac));
  }

  for(i = 0; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - 2; i++)
  {
    mask = 0xff << (i * 8);
    mac_addr->address[i] |= (COMPILER_64_LO(mac) & mask) >> (i * 8);
  }

  for(i = SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - 2; i < SOC_SAND_PP_MAC_ADDRESS_NOF_U8; i++)
  {
    index = i - (SOC_SAND_PP_MAC_ADDRESS_NOF_U8 - 2);
    mask = 0xff << (index * 8);
    mac_addr->address[i] |= (COMPILER_64_HI(mac) & mask) >> (index * 8);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fc_nif_pause_frame_src_addr_get_unsafe()", nif_ndx, 0);

}



uint32
arad_fc_rec_cal_dest_type_to_val_internal(
    SOC_SAND_IN SOC_TMC_FC_REC_CAL_DEST dest_type
  )
{
  uint32
    ret;

  for(ret = 0; ret < ARAD_FC_REC_CAL_DEST_ARR_SIZE; ret++)
  {
    if(arad_fc_rec_cal_dest_arr[ret] == dest_type)
    {
      return ret;
    }
  }

  return -1;
}

uint32
arad_fc_gen_cal_src_type_to_val_internal(
    SOC_SAND_IN SOC_TMC_FC_GEN_CAL_SRC src_type
  )
{
  uint32
    ret;

  for(ret = 0; ret < ARAD_FC_GEN_CAL_SRC_ARR_SIZE; ret++)
  {
    if(arad_fc_gen_cal_src_arr[ret] == src_type)
    {
      return ret;
    }
  }

  return -1;
}


uint32
  arad_fc_pfc_mapping_verify(
      SOC_SAND_IN int  unit,
      SOC_SAND_IN uint32                       nif_id,
      SOC_SAND_IN uint32                       src_pfc_ndx,
      SOC_SAND_IN SOC_TMC_FC_PFC_MAP           *pfc_map
  )
{
    SOCDNX_INIT_FUNC_DEFS;


    if (src_pfc_ndx > (SOC_TMC_EGR_NOF_Q_PRIO_ARAD - 1)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("src pfc index is out of range")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_fc_pfc_mapping_set(
      SOC_SAND_IN int  unit,
      SOC_SAND_IN uint32                        nif_id,
      SOC_SAND_IN uint32                        src_pfc_ndx,
      SOC_SAND_IN SOC_TMC_FC_PFC_MAP            *pfc_map
    )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t nif_pfc_data;
    uint32 entry_offset, field_offset;
    uint32 index_field[FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_MAX] = 
        {INDEX_0f, INDEX_1f, INDEX_2f, INDEX_3f, INDEX_4f, INDEX_5f, INDEX_6f, INDEX_7f};
    uint32 select_field[FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_MAX] = 
        {SELECT_0f, SELECT_1f, SELECT_2f, SELECT_3f, SELECT_4f, SELECT_5f, SELECT_6f, SELECT_7f};
    uint32 valid_field[FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_MAX] = 
        {VALID_0f, VALID_1f, VALID_2f, VALID_3f, VALID_4f, VALID_5f, VALID_6f, VALID_7f};
    uint32 select_data = 0, valid = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_fc_pfc_mapping_verify(unit, nif_id, src_pfc_ndx, pfc_map);
    SOCDNX_IF_ERR_EXIT(res);

    if (SOC_IS_JERICHO(unit)) {
        entry_offset = nif_id;
        field_offset = src_pfc_ndx;
    }
    else {
        entry_offset = ((nif_id * SOC_TMC_EGR_NOF_Q_PRIO_ARAD) / FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD) + 
            src_pfc_ndx / FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD;
        field_offset = src_pfc_ndx % FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD;
    }

    if (pfc_map->mode == SOC_TMC_FC_PFC_MAP_EGQ) {
        select_data = 0;
    }
    else {
        select_data = 1;
    }
    valid = pfc_map->valid;

    res = soc_mem_read(unit, CFC_NIF_PFC_MAPm, MEM_BLOCK_ANY, entry_offset, nif_pfc_data);
    SOCDNX_IF_ERR_EXIT(res);

    soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, index_field[field_offset], pfc_map->index);
    soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, select_field[field_offset], select_data);
    soc_mem_field32_set(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, valid_field[field_offset], valid);

    res = soc_mem_write(unit, CFC_NIF_PFC_MAPm, MEM_BLOCK_ANY, entry_offset, nif_pfc_data);
    SOCDNX_IF_ERR_EXIT(res);
  
exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_fc_pfc_mapping_get(
      SOC_SAND_IN int  unit,
      SOC_SAND_IN uint32                        nif_id,
      SOC_SAND_IN uint32                        src_pfc_ndx,
      SOC_SAND_OUT SOC_TMC_FC_PFC_MAP           *pfc_map
    )
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t nif_pfc_data;
    uint32 entry_offset, field_offset;
    uint32 index_field[FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_MAX] = 
        {INDEX_0f, INDEX_1f, INDEX_2f, INDEX_3f, INDEX_4f, INDEX_5f, INDEX_6f, INDEX_7f};
    uint32 select_field[FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_MAX] = 
        {SELECT_0f, SELECT_1f, SELECT_2f, SELECT_3f, SELECT_4f, SELECT_5f, SELECT_6f, SELECT_7f};
    uint32 valid_field[FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_MAX] = 
        {VALID_0f, VALID_1f, VALID_2f, VALID_3f, VALID_4f, VALID_5f, VALID_6f, VALID_7f};
    uint32 select_data = 0;

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_fc_pfc_mapping_verify(unit, nif_id, src_pfc_ndx, pfc_map);
    SOCDNX_IF_ERR_EXIT(res);

    if (SOC_IS_JERICHO(unit)) {
        entry_offset = nif_id;
        field_offset = src_pfc_ndx;
    }
    else {
        entry_offset = ((nif_id * SOC_TMC_EGR_NOF_Q_PRIO_ARAD) / FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD) + 
            src_pfc_ndx / FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD;
        field_offset = src_pfc_ndx % FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_ARAD;
    }

    if (pfc_map->mode == SOC_TMC_FC_PFC_MAP_EGQ) {
        select_data = 0;
    }
    else {
        select_data = 1;
    }

    res = soc_mem_read(unit, CFC_NIF_PFC_MAPm, MEM_BLOCK_ANY, entry_offset, nif_pfc_data);
    SOCDNX_IF_ERR_EXIT(res);

    pfc_map->index = soc_mem_field32_get(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, index_field[field_offset]);
    select_data = soc_mem_field32_get(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, select_field[field_offset]);
    pfc_map->valid = soc_mem_field32_get(unit, CFC_NIF_PFC_MAPm, nif_pfc_data, valid_field[field_offset]);

    if (select_data == 0) {
        pfc_map->mode = SOC_TMC_FC_PFC_MAP_EGQ;
    }
    else {
        pfc_map->mode = SOC_TMC_FC_PFC_MAP_GENERIC_BITMAP;
    }
  
exit:
    SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

