/* $Id: pb_flow_control.c,v 1.19 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/src/soc_pb_flow_control.c
*
* MODULE PREFIX:  soc_pb_flow
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
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_flow_control.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_db.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_pgm_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_nif.h>
#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/Petra/petra_sw_db.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_FC_MODE_MAX                                         (SOC_TMC_FC_NOF_INGR_GEN_GLB_HP_MODES-1)
#define SOC_PB_FC_RCY_VSQ_TRGR_NDX_MAX                             (31)
#define SOC_PB_CPID_TC_NDX_MAX                                     (7)
#define SOC_PB_FC_CLASS_MAX                                        (7)
#define SOC_PB_FC_NOF_FAP_PORTS                                    (80)
#define SOC_PB_CAL_MODE_NDX_MAX                                    (SOC_TMC_FC_NOF_CAL_MODES-1)
#define SOC_PB_IF_NDX_MAX                                          (SOC_TMC_FC_NOF_OOB_IDS-1)
#define SOC_PB_OOB_NDX_MAX                                         (SOC_TMC_FC_NOF_OOB_IDS-1)
#define SOC_PB_ILKN_NDX_MIN                                        (SOC_PB_NIF_ILKN_ID_A)
#define SOC_PB_ILKN_NDX_MAX                                        (SOC_PB_NIF_ILKN_ID_B)
#define SOC_PB_SCHEME_MAX                                          (SOC_PB_FC_NOF_NIF_OVERSUBSCR_SCHEMES-1)
#define SOC_PB_MAL_NDX_MAX                                         (15)
#define SOC_PB_PAUSE_QUANTA_MAX                                    (0xFFFF)
#define SOC_PB_FC_GEN_INBND_CB_INHERIT_MAX                         (SOC_PB_FC_NOF_INBND_CB_INHERITS-1)
#define SOC_PB_FC_GEN_INBND_INFO_MODE_MAX                          (SOC_PB_FC_NOF_INBND_MODES-1)
#define SOC_PB_FC_REC_INBND_CB_INHERIT_MAX                         (SOC_PB_FC_NOF_INBND_CB_INHERITS-1)
#define SOC_PB_FC_REC_INBND_INFO_MODE_MAX                          (SOC_PB_FC_NOF_INBND_MODES-1)
#define SOC_PB_FC_REC_OFP_MAP_INFO_REACT_POINT_MAX                 (SOC_PB_FC_NOF_REC_OFP_RPS-1)
#define SOC_PB_FC_CAL_IF_INFO_CAL_LEN_MAX                          (511)
#define SOC_PB_FC_CAL_IF_INFO_CAL_REPS_MIN                         (1)
#define SOC_PB_FC_CAL_IF_INFO_CAL_REPS_MAX                         (15)
#define SOC_PB_FC_GEN_CALENDAR_SOURCE_MAX                          (SOC_PB_FC_NOF_GEN_CAL_SRCS-1)
#define SOC_PB_FC_REC_CALENDAR_DESTINATION_MAX                     (SOC_PB_FC_NOF_REC_CAL_DESTS-1)
#define SOC_PB_FC_ILKN_LLFC_INFO_CAL_CHANNEL_MAX                   (SOC_TMC_FC_NOF_ILKN_CAL_LLFCS-1)
#define SOC_PB_FLOW_CONTROL_TRGR_NDX_MAX                           (31)
#define SOC_PB_FLOW_CONTROL_REACT_POINT_NDX_MAX                    (SOC_PB_FC_NOF_RCY_OFP_HANDLERS-1)
#define SOC_PB_FLOW_CONTROL_PRIO_NDX_MAX                           (SOC_TMC_FC_NOF_OFP_PRIORITYS)
#define SOC_PB_FC_REC_OOB_SCH_BASED2OFP_INIT_VAL                   (127)
#define SOC_PB_FC_NOF_OFP_HRS                                      (SOC_PETRA_NOF_FAP_PORTS)
#define SOC_PB_FC_OFP_HR2ENTRY_INIT_VAL                            (128)
#define SOC_PB_FC_SCH_BASED_OFP_MAPPING_TABLE_LENGTH               (128)
#define SOC_PB_FC_NOF_FAP_PORTS_UINT32S                               (3)
#define SOC_PB_FC_GEN_CAL_SRC_ARR_SIZE                              5
/* } */
/*************
 * MACROS    *
 *************/
/* { */
#define SOC_PB_FC_CAL_MODE_IS_ILKN(c_mode) \
  SOC_SAND_NUM2BOOL((c_mode == SOC_TMC_FC_CAL_MODE_ILKN_INBND) || (c_mode ==SOC_TMC_FC_CAL_MODE_ILKN_OOB))
/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef uint32 (*SOC_PB_CFC_SCH_MAP_TBL_SET_FUNC_PTR) (SOC_SAND_IN  int, SOC_SAND_IN  SOC_PB_FC_OOB_ID, SOC_SAND_IN  uint32, SOC_SAND_IN  SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA*) ;
typedef uint32 (*SOC_PB_CFC_CALRX_TBL_SET_FUNC_PTR) (SOC_SAND_IN  int, SOC_SAND_IN  SOC_PB_FC_OOB_ID, SOC_SAND_IN  uint32, SOC_SAND_IN  SOC_PB_CFC_CALRX_TBL_DATA*) ;

typedef uint32 (*SOC_PB_CFC_SCH_MAP_TBL_GET_FUNC_PTR) (SOC_SAND_IN  int, SOC_SAND_IN  SOC_PB_FC_OOB_ID, SOC_SAND_IN  uint32, SOC_SAND_OUT SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA*) ;
typedef uint32 (*SOC_PB_CFC_CALRX_TBL_GET_FUNC_PTR) (SOC_SAND_IN  int, SOC_SAND_IN  SOC_PB_FC_OOB_ID, SOC_SAND_IN  uint32, SOC_SAND_OUT SOC_PB_CFC_CALRX_TBL_DATA*) ;

typedef struct
{
  SOC_PETRA_REG_FIELD *enable_reg;
  SOC_PETRA_REG_FIELD *cal_len_reg;
  SOC_PETRA_REG_FIELD *cal_m_reg;
  SOC_PB_CFC_SCH_MAP_TBL_SET_FUNC_PTR sch_map_tbl_set;
  SOC_PB_CFC_CALRX_TBL_SET_FUNC_PTR cal_tbl_set;
  SOC_PB_CFC_SCH_MAP_TBL_GET_FUNC_PTR sch_map_tbl_get;
  SOC_PB_CFC_CALRX_TBL_GET_FUNC_PTR cal_tbl_get;
}SOC_PB_CFC_CAL_RX_SELECT_INFO;

typedef uint32 (*SOC_PB_CFC_CALTX_TBL_SET_FUNC_PTR) (SOC_SAND_IN  int, SOC_SAND_IN  SOC_PB_FC_OOB_ID, SOC_SAND_IN  uint32, SOC_SAND_IN  SOC_PB_CFC_CALTX_TBL_DATA*) ;
typedef uint32 (*SOC_PB_CFC_CALTX_TBL_GET_FUNC_PTR) (SOC_SAND_IN  int, SOC_SAND_IN  SOC_PB_FC_OOB_ID, SOC_SAND_IN  uint32, SOC_SAND_OUT SOC_PB_CFC_CALTX_TBL_DATA*) ;

typedef struct
{
  SOC_PETRA_REG_FIELD *enable_reg;
  SOC_PETRA_REG_FIELD *cal_len_reg;
  SOC_PETRA_REG_FIELD *cal_m_reg;
  SOC_PB_CFC_CALTX_TBL_SET_FUNC_PTR cal_tbl_set;
  SOC_PB_CFC_CALTX_TBL_GET_FUNC_PTR cal_tbl_get;
}SOC_PB_CFC_CAL_TX_SELECT_INFO;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */
static SOC_TMC_FC_GEN_CAL_SRC 
  soc_pb_fc_gen_cal_src_arr[SOC_PB_FC_GEN_CAL_SRC_ARR_SIZE] = 
    { SOC_TMC_FC_GEN_CAL_SRC_STE, 
      SOC_TMC_FC_GEN_CAL_SRC_NIF,
      SOC_TMC_FC_GEN_CAL_SRC_GLB_HP,
      SOC_TMC_FC_GEN_CAL_SRC_GLB_LP,
      SOC_TMC_FC_GEN_CAL_SRC_NONE
    };
/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
* NAME:
*     soc_pb_flow_control_regs_init
* FUNCTION:
*   Initialization of the Soc_petra blocks configured in this module.
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
  soc_pb_flow_control_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK,
  i;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FLOW_CONTROL_REGS_INIT);

  /* Enable Soc_petra device-level flow control */
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.cfc_en, 0x1, 10, exit);

  /* CFC Enablers */
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.cfc_en, 0x1, 20, exit);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.oob_rx_to_sch_hr_en, 0x1, 21, exit);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.oob_rx_to_egq_en, 0x1, 22, exit);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.egq_to_sch_device_en, 0x1, 23, exit);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.egq_to_sch_ofp_lp_en, 0x1, 24, exit);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.egq_to_sch_ofp_hp_en, 0x1, 25, exit);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.egq_to_sch_ch_port_en, 0x1, 26, exit);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.egq_to_sch_erp_en, 0x1, 28, exit);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.egq_to_sch_cpu_port_en, 0x1, 30, exit);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.egq_to_sch_rcl_port_en, 0x1, 32, exit);

  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.glb_rsc_to_rcl_ofp_hp_cfg, 0x1, 34, exit);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.glb_rsc_to_rcl_ofp_lp_cfg, 0x0, 36, exit);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.glb_rsc_to_egq_rcl_ofp_en, FALSE, 38, exit);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.glb_rsc_to_sch_rcl_ofp_en, FALSE, 40, exit);

  SOC_PB_FLD_SET(regs->cfc.oob_pad_configuration_reg.oob_rx_rstn[0], 0x1, 42, exit);
  SOC_PB_FLD_SET(regs->cfc.oob_pad_configuration_reg.oob_rx_rstn[1], 0x1, 44, exit);

  /* FC from CFC High to EGQ */
  for(i=0;i<SOC_PB_EGQ_FC_FROM_CFC_HIGH_ENABLE_REG_MULT_NOF_REGS;i++)
  {
    SOC_PB_REG_SET(regs->egq.fc_from_cfc_high_enable_reg[i], 0xFFFFFFFF, 46, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_flow_control_regs_init()",0,0);
}

/* Wrap soc_pb_cfc_oob_cal_tbl_set_unsafe to be compliant with SOC_PB_CFC_CALTX_TBL_SET_FUNC_PTR */
STATIC uint32
  soc_pb_fc_cfc_cal_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_CALTX_TBL_DATA  *tbl_data
    )
{
  return soc_pb_cfc_oob_caltx_tbl_set_unsafe(unit, entry_offset, tbl_data);
}

/* Wrap soc_pb_cfc_oob_cal_tbl_get_unsafe to be compliant with SOC_PB_CFC_CALTX_TBL_GET_FUNC_PTR */
STATIC uint32
  soc_pb_fc_cfc_cal_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_CALTX_TBL_DATA  *tbl_data
    )
{
  return soc_pb_cfc_oob_caltx_tbl_get_unsafe(unit, entry_offset, tbl_data);
}

STATIC uint32
  soc_pb_fc_cal_rx_select_info_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_PB_CFC_CAL_RX_SELECT_INFO      *cal_select_info
  )
{
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_CAL_SET_UNSAFE);

  switch (cal_mode_ndx)
  {
  case SOC_TMC_FC_CAL_MODE_SPI_OOB:
    cal_select_info->enable_reg       = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? SOC_PB_REG_DB_ACC_REF(regs->cfc.out_of_band_rx_configuration_reg[0].oob_rx_en) : SOC_PB_REG_DB_ACC_REF(regs->cfc.out_of_band_rx_configuration_reg[1].oob_rx_en);
    cal_select_info->cal_len_reg      = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? SOC_PB_REG_DB_ACC_REF(regs->cfc.out_of_band_rx_configuration_reg[0].rx_cal_len) : SOC_PB_REG_DB_ACC_REF(regs->cfc.out_of_band_rx_configuration_reg[1].rx_cal_len);
    cal_select_info->cal_m_reg        = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? SOC_PB_REG_DB_ACC_REF(regs->cfc.out_of_band_rx_configuration_reg[0].rx_cal_m) : SOC_PB_REG_DB_ACC_REF(regs->cfc.out_of_band_rx_configuration_reg[1].rx_cal_m);
    cal_select_info->sch_map_tbl_set  = soc_pb_cfc_oob_sch_map_tbl_set_unsafe;
    cal_select_info->cal_tbl_set      = soc_pb_cfc_oob_calrx_tbl_set_unsafe;
    cal_select_info->sch_map_tbl_get  = soc_pb_cfc_oob_sch_map_tbl_get_unsafe;
    cal_select_info->cal_tbl_get      = soc_pb_cfc_oob_calrx_tbl_get_unsafe;
    break;
  case SOC_TMC_FC_CAL_MODE_ILKN_INBND:
  case SOC_TMC_FC_CAL_MODE_ILKN_OOB:
    cal_select_info->enable_reg       = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? SOC_PB_REG_DB_ACC_REF(regs->cfc.ilkn_configuration_reg[0].ilkn_rx_en) : SOC_PB_REG_DB_ACC_REF(regs->cfc.ilkn_configuration_reg[1].ilkn_rx_en);
    cal_select_info->cal_len_reg      = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? SOC_PB_REG_DB_ACC_REF(regs->cfc.ilkn_configuration_reg[0].ilkn_rx_cal_len) : SOC_PB_REG_DB_ACC_REF(regs->cfc.ilkn_configuration_reg[1].ilkn_rx_cal_len);
    cal_select_info->cal_m_reg        = NULL;
    cal_select_info->sch_map_tbl_set  = soc_pb_cfc_ilkn_sch_map_tbl_set_unsafe;
    cal_select_info->cal_tbl_set      = soc_pb_cfc_ilkn_calrx_tbl_set_unsafe;
    cal_select_info->sch_map_tbl_get  = soc_pb_cfc_ilkn_sch_map_tbl_get_unsafe;
    cal_select_info->cal_tbl_get      = soc_pb_cfc_ilkn_calrx_tbl_get_unsafe;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_FC_CAL_IF_NDX_OUT_OF_RANGE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_fc_cal_rx_select_info_unsafe()",0,0);
}

STATIC uint32
  soc_pb_fc_cal_tx_select_info_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_PB_CFC_CAL_TX_SELECT_INFO      *cal_select_info
  )
{
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_CAL_SET_UNSAFE);

  switch (cal_mode_ndx)
  {
  case SOC_TMC_FC_CAL_MODE_SPI_OOB:
    cal_select_info->enable_reg       = SOC_PB_REG_DB_ACC_REF(regs->cfc.out_of_band_tx_configuration0_reg.oob_tx_en);
    cal_select_info->cal_len_reg      = SOC_PB_REG_DB_ACC_REF(regs->cfc.out_of_band_tx_configuration0_reg.tx_cal_len);
    cal_select_info->cal_m_reg        = SOC_PB_REG_DB_ACC_REF(regs->cfc.out_of_band_tx_configuration0_reg.tx_cal_m);
    cal_select_info->cal_tbl_set      = soc_pb_fc_cfc_cal_tbl_set_unsafe;
    cal_select_info->cal_tbl_get      = soc_pb_fc_cfc_cal_tbl_get_unsafe;
    break;
  case SOC_TMC_FC_CAL_MODE_ILKN_INBND:
  case SOC_TMC_FC_CAL_MODE_ILKN_OOB:
    cal_select_info->enable_reg       = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? SOC_PB_REG_DB_ACC_REF(regs->cfc.ilkn_configuration_reg[0].ilkn_tx_en) : SOC_PB_REG_DB_ACC_REF(regs->cfc.ilkn_configuration_reg[1].ilkn_tx_en);
    cal_select_info->cal_len_reg      = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? SOC_PB_REG_DB_ACC_REF(regs->cfc.ilkn_configuration_reg[0].ilkn_tx_cal_len) : SOC_PB_REG_DB_ACC_REF(regs->cfc.ilkn_configuration_reg[1].ilkn_tx_cal_len);
    cal_select_info->cal_m_reg        = NULL;
    cal_select_info->cal_tbl_set      = soc_pb_cfc_ilkn_caltx_tbl_set_unsafe;
    cal_select_info->cal_tbl_get      = soc_pb_cfc_ilkn_caltx_tbl_get_unsafe;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_FC_CAL_IF_NDX_OUT_OF_RANGE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_fc_cal_rx_select_info_unsafe()",0,0);
}

/*********************************************************************
* NAME:
*     soc_pb_flow_control_init
* FUNCTION:
*     Initialization of the Soc_petra blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_pb_flow_control_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FLOW_CONTROL_INIT);

  res = soc_pb_flow_control_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_flow_control_init()",0,0);
}

STATIC uint32
  soc_pb_fc_inbnd_max_nof_priorities_get(
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  )
{

  switch(soc_pb_nif_id2type(nif_ndx))
  {
  case SOC_PB_NIF_TYPE_QSGMII:
    return 2;
  case SOC_PB_NIF_TYPE_SGMII:
    return 2;
  case SOC_PB_NIF_TYPE_XAUI:
    return 8;
  case SOC_PB_NIF_TYPE_RXAUI:
    return 8;
  default:
    return 0;
  }
}

STATIC uint32
  soc_pb_fc_gen_inbnd_cb_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_CB             *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    offset = soc_pb_nif2intern_id(nif_ndx),
    buffer,
    reg_offset,
    bit_offset;
  uint32
    idx;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    inp_mapping,
    out_mapping;
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_CB_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  for (idx = 0; idx < SOC_PETRA_NOF_FAP_PORTS; ++idx)
  {
    soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&inp_mapping);
    soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&out_mapping);
    res = soc_petra_port_to_interface_map_get_unsafe(
            unit,
            idx,
            &inp_mapping,
            &out_mapping
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (out_mapping.if_id == nif_ndx)
    {
      res = soc_pb_pp_egq_ppct_tbl_get_unsafe(
              unit,
              idx,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      data.cnm_intrcpt_en = SOC_SAND_BOOL2NUM(info->cnm_intercept_enable);
      data.cnm_intrcpt_fc_vec_index_12_6 = offset * 2;
      data.cnm_intrcpt_fc_vec_index_13_13 = SOC_SAND_BOOL2NUM((soc_pb_fc_inbnd_max_nof_priorities_get(nif_ndx) == 8));
      res = soc_pb_pp_egq_ppct_tbl_set_unsafe(
              unit,
              idx,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
  }

  reg_offset = offset / 16;
  bit_offset = 2 * (offset % 16);

  SOC_PB_REG_GET(regs->cfc.lp_glb_rsc_to_nif_clsb_map_reg[reg_offset], buffer, 40, exit);
  buffer &= SOC_SAND_RBITS_MASK(bit_offset + soc_pb_fc_inbnd_max_nof_priorities_get(nif_ndx) - 1, bit_offset);
  buffer |= SOC_SAND_SET_FLD_IN_PLACE(
              info->glbl_rcs_low,
              bit_offset,
              SOC_SAND_BITS_MASK(
                bit_offset + soc_pb_fc_inbnd_max_nof_priorities_get(nif_ndx) - 1,
                bit_offset
              )
            );
  SOC_PB_REG_SET(regs->cfc.lp_glb_rsc_to_nif_clsb_map_reg[reg_offset], buffer, 50, exit);

  buffer = SOC_SAND_BOOL2NUM(info->inherit == SOC_PB_FC_INBND_CB_INHERIT_DOWN);
  SOC_PB_FLD_ISET(regs->nif_mac_lane.flow_control_tx_reg.fc_tx_cbfc_prioritize_dn_en, buffer, offset / 4, 60, exit);

  buffer = SOC_SAND_BOOL2NUM(info->inherit == SOC_PB_FC_INBND_CB_INHERIT_UP);
  SOC_PB_FLD_ISET(regs->nif_mac_lane.flow_control_tx_reg.fc_tx_cbfc_prioritize_up_en, buffer, offset / 4, 60, exit);

  SOC_PB_REG_GET(regs->nbi.fc_cbfc_mapping_reg[reg_offset], buffer, 70, exit);
  buffer &= SOC_SAND_RBITS_MASK(bit_offset + soc_pb_fc_inbnd_max_nof_priorities_get(nif_ndx) - 1, bit_offset);
  buffer |= SOC_SAND_SET_FLD_IN_PLACE(
              info->nif_cls_bitmap,
              bit_offset,
              SOC_SAND_BITS_MASK(
                bit_offset + soc_pb_fc_inbnd_max_nof_priorities_get(nif_ndx) - 1,
                bit_offset
              )
            );
  SOC_PB_REG_SET(regs->nbi.fc_cbfc_mapping_reg[reg_offset], buffer, 80, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_cb_set_unsafe()", 0, 0);
}

STATIC uint32
  soc_pb_fc_gen_inbnd_ll_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_LL             *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset = soc_pb_nif2intern_id(nif_ndx);
  uint32
    idx;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    inp_mapping,
    out_mapping;
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_LL_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  for (idx = 0; idx < SOC_PETRA_NOF_FAP_PORTS; ++idx)
  {
    soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&inp_mapping);
    soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&out_mapping);
    res = soc_petra_port_to_interface_map_get_unsafe(
            unit,
            idx,
            &inp_mapping,
            &out_mapping
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    if (out_mapping.if_id == nif_ndx)
    {
      res = soc_pb_pp_egq_ppct_tbl_get_unsafe(
              unit,
              idx,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      data.cnm_intrcpt_en = SOC_SAND_BOOL2NUM(info->cnm_enable);
      data.cnm_intrcpt_fc_vec_index_5_0 = offset;
      res = soc_pb_pp_egq_ppct_tbl_set_unsafe(
              unit,
              idx,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_ll_set_unsafe()", nif_ndx, 0);
}

/*********************************************************************
*     Inband Flow Control Generation Configuration, based on
 *     Soc_petra Ingress state indications.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    nif_id,
    nif_id_inner,
    mal_id;
  uint32
    val,
    fld_val;
  SOC_PB_FC_GEN_INBND_LL
    ll_disabled;
  SOC_PB_FC_GEN_INBND_CB
    cb_disabled;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_PB_FC_GEN_INBND_LL_clear(&ll_disabled);
  SOC_PB_FC_GEN_INBND_CB_clear(&cb_disabled);
  cb_disabled.inherit = SOC_PB_FC_INBND_CB_INHERIT_DISABLED;

  /*
   *  Indexing
   */
  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  nif_id_inner = nif_id % SOC_PB_MAX_NIFS_PER_MAL;

  SOC_PB_FLD_IGET(regs->nif_mac_lane.flow_control_tx_reg.fc_tx_llfc_en, fld_val, mal_id, 10, exit);
  val = (info->mode == SOC_PB_FC_INBND_MODE_LL)?0x1:0x0;
  SOC_SAND_SET_BIT(fld_val, val, nif_id_inner);
  SOC_PB_FLD_ISET(regs->nif_mac_lane.flow_control_tx_reg.fc_tx_llfc_en, fld_val, mal_id, 12, exit);

  SOC_PB_FLD_IGET(regs->nif_mac_lane.flow_control_tx_reg.fc_tx_cbfc_en, fld_val, mal_id, 20, exit);
  val = (info->mode == SOC_PB_FC_INBND_MODE_CB)?0x1:0x0;
  SOC_SAND_SET_BIT(fld_val, val, nif_id_inner);
  SOC_PB_FLD_ISET(regs->nif_mac_lane.flow_control_tx_reg.fc_tx_cbfc_en, fld_val, mal_id, 22, exit);

  switch (info->mode)
  {
  case SOC_PB_FC_INBND_MODE_LL:
    res = soc_pb_fc_gen_inbnd_cb_set_unsafe(
            unit,
            nif_ndx,
            &cb_disabled
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    res = soc_pb_fc_gen_inbnd_ll_set_unsafe(
            unit,
            nif_ndx,
            &info->ll
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    break;
  case SOC_PB_FC_INBND_MODE_CB:
    res = soc_pb_fc_gen_inbnd_ll_set_unsafe(
            unit,
            nif_ndx,
            &ll_disabled
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    res = soc_pb_fc_gen_inbnd_cb_set_unsafe(
            unit,
            nif_ndx,
            &info->cb
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    break;
    case SOC_PB_FC_INBND_MODE_DISABLED:
    default:
    res = soc_pb_fc_gen_inbnd_ll_set_unsafe(
            unit,
            nif_ndx,
            &ll_disabled
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    res = soc_pb_fc_gen_inbnd_cb_set_unsafe(
            unit,
            nif_ndx,
            &cb_disabled
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_set_unsafe()", nif_ndx, 0);
}

uint32
  soc_pb_fc_gen_inbnd_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_SET_VERIFY);

  res = soc_pb_nif_id_verify(
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (SOC_PB_NIF_IS_TYPE_ID(ILKN, nif_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_API_NOT_APPLICABLE_FOR_ILKN_ERR, 20, exit);
  }

  SOC_PB_STRUCT_VERIFY(SOC_PB_FC_GEN_INBND_INFO, info, 30, exit);

  if (info->cb.nif_cls_bitmap > SOC_SAND_BITS_MASK(soc_pb_fc_inbnd_max_nof_priorities_get(nif_ndx) - 1, 0))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_FC_TC_NOT_SUPPORTED_ON_IF_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_set_verify()", nif_ndx, 0);
}

uint32
  soc_pb_fc_gen_inbnd_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_GET_VERIFY);

  res = soc_pb_nif_id_verify(
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (SOC_PB_NIF_IS_TYPE_ID(ILKN, nif_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_API_NOT_APPLICABLE_FOR_ILKN_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_get_verify()", nif_ndx, 0);
}

STATIC uint32
  soc_pb_fc_gen_inbnd_cb_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_FC_GEN_INBND_CB             *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    offset = soc_pb_nif2intern_id(nif_ndx),
    buffer,
    reg_offset,
    bit_offset;
  uint32
    idx = 0;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    inp_mapping,
    out_mapping;
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_CB_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  info->cnm_intercept_enable = 0;
  do
  {
    soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&inp_mapping);
    soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&out_mapping);
    res = soc_petra_port_to_interface_map_get_unsafe(
            unit,
            idx,
            &inp_mapping,
            &out_mapping
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (out_mapping.if_id == nif_ndx)
    {
      res = soc_pb_pp_egq_ppct_tbl_get_unsafe(
              unit,
              idx,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      info->cnm_intercept_enable = SOC_SAND_NUM2BOOL(data.cnm_intrcpt_en);
    }
  } while ((++idx < SOC_PETRA_NOF_FAP_PORTS) && (out_mapping.if_id != nif_ndx));

  reg_offset = offset / 16;
  bit_offset = 2 * (offset % 16);

  SOC_PB_REG_GET(regs->cfc.lp_glb_rsc_to_nif_clsb_map_reg[reg_offset], buffer, 30, exit);
  info->glbl_rcs_low = SOC_SAND_GET_FLD_FROM_PLACE(
                         buffer,
                         bit_offset,
                         SOC_SAND_BITS_MASK(
                           bit_offset + soc_pb_fc_inbnd_max_nof_priorities_get(nif_ndx) - 1,
                           bit_offset
                         )
                       );

  SOC_PB_FLD_IGET(regs->nif_mac_lane.flow_control_tx_reg.fc_tx_cbfc_prioritize_dn_en, buffer, offset / 4, 40, exit);
  info->inherit = buffer ? SOC_PB_FC_INBND_CB_INHERIT_DOWN : SOC_PB_FC_INBND_CB_INHERIT_UP;

  SOC_PB_REG_GET(regs->nbi.fc_cbfc_mapping_reg[reg_offset], buffer, 50, exit);
  info->nif_cls_bitmap = (uint8) SOC_SAND_GET_FLD_FROM_PLACE(
                                     buffer,
                                     bit_offset,
                                     SOC_SAND_BITS_MASK(
                                       bit_offset + soc_pb_fc_inbnd_max_nof_priorities_get(nif_ndx) - 1,
                                       bit_offset
                                     )
                                   );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_cb_get_unsafe()", 0, 0);
}

STATIC uint32
  soc_pb_fc_gen_inbnd_ll_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_FC_GEN_INBND_LL             *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    idx = 0;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    inp_mapping,
    out_mapping;
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_LL_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  info->cnm_enable = 0;
  do
  {
    soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&inp_mapping);
    soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&out_mapping);
    res = soc_petra_port_to_interface_map_get_unsafe(
            unit,
            idx,
            &inp_mapping,
            &out_mapping
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (out_mapping.if_id == nif_ndx)
    {
      res = soc_pb_pp_egq_ppct_tbl_get_unsafe(
              unit,
              idx,
              &data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      info->cnm_enable = SOC_SAND_NUM2BOOL(data.cnm_intrcpt_en);
    }
  } while ((idx++ < SOC_PETRA_NOF_FAP_PORTS) && (out_mapping.if_id != nif_ndx));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_ll_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Inband Flow Control Generation Configuration, based on
 *     Soc_petra Ingress state indications.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_FC_GEN_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    nif_id,
    nif_id_inner,
    mal_id;
  uint32
    val,
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_PB_FC_GEN_INBND_INFO_clear(info);

  /*
   *  Indexing
   */
  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  nif_id_inner = nif_id % SOC_PB_MAX_NIFS_PER_MAL;

  SOC_PB_FLD_IGET(regs->nif_mac_lane.flow_control_tx_reg.fc_tx_llfc_en, fld_val, mal_id, 10, exit);
  val = SOC_SAND_GET_BIT(fld_val, nif_id_inner);
  if (val)
  {
    info->mode = SOC_PB_FC_INBND_MODE_LL;
  }
  else
  {
    /*
     * LL and CB set simultaneously is not a valid configuration
     */
    SOC_PB_FLD_IGET(regs->nif_mac_lane.flow_control_tx_reg.fc_tx_cbfc_en, fld_val, mal_id, 20, exit);
    val = SOC_SAND_GET_BIT(fld_val, nif_id_inner);
    info->mode = (fld_val)? SOC_PB_FC_INBND_MODE_CB : SOC_PB_FC_INBND_MODE_DISABLED;
  }
  if (info->mode == SOC_PB_FC_INBND_MODE_LL)
  {
    res = soc_pb_fc_gen_inbnd_ll_get_unsafe(
            unit,
            nif_ndx,
            &info->ll
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if (info->mode == SOC_PB_FC_INBND_MODE_CB)
  {
    res = soc_pb_fc_gen_inbnd_cb_get_unsafe(
            unit,
            nif_ndx,
            &info->cb
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_get_unsafe()", nif_ndx, 0);
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
  soc_pb_fc_gen_inbnd_glb_hp_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_INGR_GEN_GLB_HP_MODE     fc_mode
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    ll_fld_val,
    cb_fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_GLB_HP_SET_UNSAFE);

  ll_fld_val = (fc_mode == SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_LL);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.frc_nif_llvl_en, ll_fld_val, 10, exit);

  cb_fld_val = (fc_mode == SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_CB);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.frc_nif_clsb_en, cb_fld_val, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_glb_hp_set_unsafe()", 0, 0);
}

uint32
  soc_pb_fc_gen_inbnd_glb_hp_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_INGR_GEN_GLB_HP_MODE     fc_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_GLB_HP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fc_mode, SOC_PB_FC_MODE_MAX, SOC_PB_FC_MODE_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_glb_hp_set_verify()", 0, 0);
}

uint32
  soc_pb_fc_gen_inbnd_glb_hp_get_verify(
    SOC_SAND_IN  int                      unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_GLB_HP_GET_VERIFY);

  SOC_PB_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_glb_hp_get_verify()", 0, 0);
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
  soc_pb_fc_gen_inbnd_glb_hp_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_FC_INGR_GEN_GLB_HP_MODE     *fc_mode
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    ll_fld_val,
    cb_fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_GLB_HP_GET_UNSAFE);
  
  *fc_mode = SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_NONE;

  SOC_PB_FLD_GET(regs->cfc.cfc_enablers_reg.frc_nif_llvl_en, ll_fld_val, 10, exit);
  *fc_mode = (ll_fld_val ? SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_LL : *fc_mode);

  SOC_PB_FLD_GET(regs->cfc.cfc_enablers_reg.frc_nif_clsb_en, cb_fld_val, 20, exit);
  *fc_mode = (cb_fld_val ? SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_CB : *fc_mode);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_glb_hp_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Map the 3-LSB of the CMN CPID (represent TC), to the FC
 *     indication to generate when using Class Based Flow
 *     Control
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_cnm_map_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_IN  uint8                      enable_ll,
    SOC_SAND_IN  uint32                      fc_class
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    buffer;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_CNM_MAP_SET_UNSAFE);

  buffer = SOC_SAND_BOOL2NUM(enable_ll);
  SOC_PB_FLD_SET(regs->egq.cnm_cpid_to_fc_type_reg[cpid_tc_ndx / SOC_PB_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_REGS].cnm_cpid_to_fc_ll_type[cpid_tc_ndx % SOC_PB_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_FLDS], buffer, 10, exit);
  buffer = fc_class;
  SOC_PB_FLD_SET(regs->egq.cnm_cpid_to_fc_type_reg[cpid_tc_ndx / SOC_PB_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_REGS].cnm_cpid_to_fc_cb_type[cpid_tc_ndx % SOC_PB_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_FLDS], buffer, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_cnm_map_set_unsafe()", cpid_tc_ndx, 0);
}

uint32
  soc_pb_fc_gen_inbnd_cnm_map_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_IN  uint8                      enable_ll,
    SOC_SAND_IN  uint32                      fc_class
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_CNM_MAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cpid_tc_ndx, SOC_PB_CPID_TC_NDX_MAX - 1, SOC_PB_CPID_TC_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fc_class, SOC_PB_FC_CLASS_MAX - 1, SOC_PB_FC_CLASS_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_cnm_map_set_verify()", cpid_tc_ndx, 0);
}

uint32
  soc_pb_fc_gen_inbnd_cnm_map_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_CNM_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cpid_tc_ndx, SOC_PB_CPID_TC_NDX_MAX - 1, SOC_PB_CPID_TC_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_cnm_map_get_verify()", cpid_tc_ndx, 0);
}

/*********************************************************************
*     Map the 3-LSB of the CMN CPID (represent TC), to the FC
 *     indication to generate when using Class Based Flow
 *     Control
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_gen_inbnd_cnm_map_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      cpid_tc_ndx,
    SOC_SAND_OUT uint8                      *enable_ll,
    SOC_SAND_OUT uint32                      *fc_class
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    buffer;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_INBND_CNM_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fc_class);

  SOC_PB_FLD_GET(regs->egq.cnm_cpid_to_fc_type_reg[cpid_tc_ndx / SOC_PB_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_REGS].cnm_cpid_to_fc_ll_type[cpid_tc_ndx % SOC_PB_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_FLDS], buffer, 10, exit);
  *enable_ll = SOC_SAND_NUM2BOOL(buffer);
  SOC_PB_FLD_GET(regs->egq.cnm_cpid_to_fc_type_reg[cpid_tc_ndx / SOC_PB_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_REGS].cnm_cpid_to_fc_cb_type[cpid_tc_ndx % SOC_PB_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_FLDS], buffer, 20, exit);
  *fc_class = buffer;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_inbnd_cnm_map_get_unsafe()", cpid_tc_ndx, 0);
}

/*********************************************************************
*     Inband Flow Control Reception Configuration
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rec_inbnd_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_FC_REC_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    nif_id,
    nif_id_inner,
    mal_id,
    bit_offset;
  uint32
    val,
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_INBND_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *  Indexing
   */
  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  nif_id_inner = nif_id % SOC_PB_MAX_NIFS_PER_MAL;
  bit_offset = 2 * (nif_id % 16);

  SOC_PB_FLD_IGET(regs->nif_mac_lane.flow_control_rx_reg.fc_rx_llfc_en, fld_val, mal_id, 4, exit);
  val = (info->mode == SOC_PB_FC_INBND_MODE_LL)?0x1:0x0;
  SOC_SAND_SET_BIT(fld_val, val, nif_id_inner);
  SOC_PB_FLD_ISET(regs->nif_mac_lane.flow_control_rx_reg.fc_rx_llfc_en, fld_val, mal_id, 6, exit);

  SOC_PB_FLD_IGET(regs->nif_mac_lane.flow_control_rx_reg.fc_rx_cbfc_en, fld_val, mal_id, 8, exit);
  val = (info->mode == SOC_PB_FC_INBND_MODE_CB)?0x1:0x0;
  SOC_SAND_SET_BIT(fld_val, val, nif_id_inner);
  SOC_PB_FLD_ISET(regs->nif_mac_lane.flow_control_rx_reg.fc_rx_cbfc_en, fld_val, mal_id, 10, exit);

  SOC_PB_FLD_IGET(regs->nif_mac_lane.flow_control_rx_reg.fc_rx_cbfc_prioritize_dn_en, fld_val, mal_id, 12, exit);
  fld_val = info->cb.inherit == SOC_PB_FC_INBND_CB_INHERIT_DOWN ? 1 : 0;
  SOC_PB_FLD_ISET(regs->nif_mac_lane.flow_control_rx_reg.fc_rx_cbfc_prioritize_dn_en, fld_val, mal_id, 14, exit);

  SOC_PB_FLD_IGET(regs->nif_mac_lane.flow_control_rx_reg.fc_rx_cbfc_prioritize_up_en, fld_val, mal_id, 16, exit);
  fld_val = info->cb.inherit == SOC_PB_FC_INBND_CB_INHERIT_UP ? 1 : 0;
  SOC_PB_FLD_ISET(regs->nif_mac_lane.flow_control_rx_reg.fc_rx_cbfc_prioritize_up_en, fld_val, mal_id, 18, exit);

  SOC_PB_REG_GET(regs->cfc.nif_clsb_to_sch_hr_mask_reg[nif_id / SOC_PB_NOF_MAC_LANES], fld_val, 20, exit);
  fld_val &= SOC_SAND_RBITS_MASK(bit_offset + soc_pb_fc_inbnd_max_nof_priorities_get(nif_ndx) - 1, bit_offset);
  fld_val |= SOC_SAND_SET_FLD_IN_PLACE(
               info->cb.sch_hr_bitmap,
               bit_offset,
               SOC_SAND_BITS_MASK(
                 bit_offset + soc_pb_fc_inbnd_max_nof_priorities_get(nif_ndx) - 1,
                 bit_offset
               )
             );
  SOC_PB_REG_SET(regs->cfc.nif_clsb_to_sch_hr_mask_reg[nif_id / SOC_PB_NOF_MAC_LANES], fld_val, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rec_inbnd_set_unsafe()", nif_ndx, 0);
}

uint32
  soc_pb_fc_rec_inbnd_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_FC_REC_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_INBND_SET_VERIFY);

  res = soc_pb_nif_id_verify(
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (SOC_PB_NIF_IS_TYPE_ID(ILKN, nif_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_API_NOT_APPLICABLE_FOR_ILKN_ERR, 12, exit);
  }

  SOC_PB_STRUCT_VERIFY(SOC_PB_FC_REC_INBND_INFO, info, 20, exit);

  if (info->cb.sch_hr_bitmap > SOC_SAND_BITS_MASK(soc_pb_fc_inbnd_max_nof_priorities_get(nif_ndx) - 1, 0))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_FC_TC_NOT_SUPPORTED_ON_IF_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rec_inbnd_set_verify()", nif_ndx, 0);
}

uint32
  soc_pb_fc_rec_inbnd_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_INBND_GET_VERIFY);

  res = soc_pb_nif_id_verify(
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (SOC_PB_NIF_IS_TYPE_ID(ILKN, nif_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_API_NOT_APPLICABLE_FOR_ILKN_ERR, 12, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rec_inbnd_get_verify()", nif_ndx, 0);
}

/*********************************************************************
*     Inband Flow Control Reception Configuration
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rec_inbnd_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_FC_REC_INBND_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    nif_id,
    nif_id_inner,
    mal_id,
    bit_offset;
  uint32
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_INBND_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *  Indexing
   */
  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  nif_id_inner = nif_id % SOC_PB_MAX_NIFS_PER_MAL;
  bit_offset = 2 * (nif_id % 16);

  info->cb.inherit = SOC_PB_FC_INBND_CB_INHERIT_DISABLED;
  info->mode = SOC_PB_FC_INBND_MODE_DISABLED;

  SOC_PB_FLD_IGET(regs->nif_mac_lane.flow_control_rx_reg.fc_rx_llfc_en, fld_val, mal_id, 10, exit);
  info->mode = SOC_SAND_GET_FLD_FROM_PLACE(fld_val, nif_id_inner, SOC_SAND_BIT(nif_id_inner)) ? SOC_PB_FC_INBND_MODE_LL : info->mode;

  SOC_PB_FLD_IGET(regs->nif_mac_lane.flow_control_rx_reg.fc_rx_cbfc_en, fld_val, mal_id, 20, exit);
  info->mode = SOC_SAND_GET_FLD_FROM_PLACE(fld_val, nif_id_inner, SOC_SAND_BIT(nif_id_inner)) ? SOC_PB_FC_INBND_MODE_CB : info->mode;

  SOC_PB_FLD_IGET(regs->nif_mac_lane.flow_control_rx_reg.fc_rx_cbfc_prioritize_dn_en, fld_val, mal_id, 30, exit);
  info->cb.inherit = fld_val ? SOC_PB_FC_INBND_CB_INHERIT_DOWN : info->cb.inherit;

  SOC_PB_FLD_IGET(regs->nif_mac_lane.flow_control_rx_reg.fc_rx_cbfc_prioritize_up_en, fld_val, mal_id, 40, exit);
  info->cb.inherit = fld_val ? SOC_PB_FC_INBND_CB_INHERIT_UP   : info->cb.inherit;

  SOC_PB_REG_GET(regs->cfc.nif_clsb_to_sch_hr_mask_reg[nif_id / 16], fld_val, 50, exit);
  info->cb.sch_hr_bitmap = (uint8) SOC_SAND_GET_FLD_FROM_PLACE(
                                       fld_val,
                                       bit_offset,
                                       SOC_SAND_BITS_MASK(
                                         bit_offset + soc_pb_fc_inbnd_max_nof_priorities_get(nif_ndx) - 1,
                                         bit_offset
                                       )
                                     );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rec_inbnd_set_unsafe()", nif_ndx, 0);
}

/*********************************************************************
*     Selects the OFP, priority and Reaction Point for inband
 *     FC reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rec_inbnd_ofp_map_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint32                      fc_cls_ndx,
    SOC_SAND_IN  SOC_PB_FC_REC_OFP_MAP_INFO         *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tbl_offset;
  SOC_PB_CFC_NIFCLSB2_OFP_TBL_DATA
    clsb2ofp_tbl;
  uint32
    nif_id = soc_pb_nif2intern_id(nif_ndx);

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_INBND_OFP_MAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  tbl_offset = (2 * nif_id + fc_cls_ndx) / 4;
  res = soc_pb_cfc_nifclsb2_ofp_tbl_get_unsafe(
          unit,
          tbl_offset,
          &clsb2ofp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  clsb2ofp_tbl.data[(nif_id + fc_cls_ndx) % 4].ofp_num      = info->ofp_ndx;
  clsb2ofp_tbl.data[(nif_id + fc_cls_ndx) % 4].egq.hp_valid = SOC_SAND_NUM2BOOL(((info->priority & SOC_TMC_FC_OFP_PRIORITY_HP) == SOC_TMC_FC_OFP_PRIORITY_HP) && ((info->react_point & SOC_PB_FC_REC_OFP_RP_EGQ) == SOC_PB_FC_REC_OFP_RP_EGQ));
  clsb2ofp_tbl.data[(nif_id + fc_cls_ndx) % 4].egq.lp_valid = SOC_SAND_NUM2BOOL(((info->priority & SOC_TMC_FC_OFP_PRIORITY_LP) == SOC_TMC_FC_OFP_PRIORITY_LP) && ((info->react_point & SOC_PB_FC_REC_OFP_RP_EGQ) == SOC_PB_FC_REC_OFP_RP_EGQ));
  clsb2ofp_tbl.data[(nif_id + fc_cls_ndx) % 4].sch.hp_valid = SOC_SAND_NUM2BOOL(((info->priority & SOC_TMC_FC_OFP_PRIORITY_HP) == SOC_TMC_FC_OFP_PRIORITY_HP) && ((info->react_point & SOC_PB_FC_REC_OFP_RP_SCH) == SOC_PB_FC_REC_OFP_RP_SCH));
  clsb2ofp_tbl.data[(nif_id + fc_cls_ndx) % 4].sch.lp_valid = SOC_SAND_NUM2BOOL(((info->priority & SOC_TMC_FC_OFP_PRIORITY_LP) == SOC_TMC_FC_OFP_PRIORITY_LP) && ((info->react_point & SOC_PB_FC_REC_OFP_RP_SCH) == SOC_PB_FC_REC_OFP_RP_SCH));
  res = soc_pb_cfc_nifclsb2_ofp_tbl_set_unsafe(
          unit,
          tbl_offset,
          &clsb2ofp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rec_inbnd_ofp_map_set_unsafe()", nif_ndx, fc_cls_ndx);
}

uint32
  soc_pb_fc_rec_inbnd_ofp_map_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint32                      fc_cls_ndx,
    SOC_SAND_IN  SOC_PB_FC_REC_OFP_MAP_INFO         *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_INBND_OFP_MAP_SET_VERIFY);

  res = soc_pb_nif_id_verify(
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (SOC_PB_NIF_IS_TYPE_ID(ILKN, nif_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_API_NOT_APPLICABLE_FOR_ILKN_ERR, 12, exit);
  }

  SOC_PB_STRUCT_VERIFY(SOC_PB_FC_REC_OFP_MAP_INFO, info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rec_inbnd_ofp_map_set_verify()", nif_ndx, fc_cls_ndx);
}

uint32
  soc_pb_fc_rec_inbnd_ofp_map_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint32                      fc_cls_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_INBND_OFP_MAP_GET_VERIFY);

  res = soc_pb_nif_id_verify(
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (SOC_PB_NIF_IS_TYPE_ID(ILKN, nif_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_API_NOT_APPLICABLE_FOR_ILKN_ERR, 12, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(fc_cls_ndx, SOC_PB_FC_CLASS_MAX, SOC_PB_FC_CLASS_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rec_inbnd_ofp_map_get_verify()", nif_ndx, fc_cls_ndx);
}

/*********************************************************************
*     Selects the OFP, priority and Reaction Point for inband
 *     FC reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rec_inbnd_ofp_map_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint32                      fc_cls_ndx,
    SOC_SAND_OUT SOC_PB_FC_REC_OFP_MAP_INFO         *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_CFC_NIFCLSB2_OFP_TBL_DATA
    clsb2ofp_tbl;
  uint32
    nif_id = soc_pb_nif2intern_id(nif_ndx);

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_INBND_OFP_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_FC_REC_OFP_MAP_INFO_clear(info);

  info->react_point = 0;
  info->priority = 0;
  res = soc_pb_cfc_nifclsb2_ofp_tbl_get_unsafe(
          unit,
          (nif_id + fc_cls_ndx) / 4,
          &clsb2ofp_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  clsb2ofp_tbl.data[(nif_id + fc_cls_ndx) % 4].ofp_num      = info->ofp_ndx;
  info->priority |= clsb2ofp_tbl.data[(nif_id + fc_cls_ndx) % 4].sch.hp_valid ? SOC_TMC_FC_OFP_PRIORITY_HP : info->priority;
  info->react_point |=  clsb2ofp_tbl.data[(nif_id + fc_cls_ndx) % 4].sch.hp_valid ? SOC_PB_FC_REC_OFP_RP_SCH : info->react_point;
  info->priority |= clsb2ofp_tbl.data[(nif_id + fc_cls_ndx) % 4].sch.lp_valid ? SOC_TMC_FC_OFP_PRIORITY_LP : info->priority;
  info->react_point |=  clsb2ofp_tbl.data[(nif_id + fc_cls_ndx) % 4].sch.lp_valid ? SOC_PB_FC_REC_OFP_RP_SCH : info->react_point;
  info->priority |= clsb2ofp_tbl.data[(nif_id + fc_cls_ndx) % 4].egq.hp_valid ? SOC_TMC_FC_OFP_PRIORITY_HP : info->priority;
  info->react_point |=  clsb2ofp_tbl.data[(nif_id + fc_cls_ndx) % 4].egq.hp_valid ? SOC_PB_FC_REC_OFP_RP_EGQ : info->react_point;
  info->priority |= clsb2ofp_tbl.data[(nif_id + fc_cls_ndx) % 4].egq.lp_valid ? SOC_TMC_FC_OFP_PRIORITY_LP : info->priority;
  info->react_point |=  clsb2ofp_tbl.data[(nif_id + fc_cls_ndx) % 4].egq.lp_valid ? SOC_PB_FC_REC_OFP_RP_EGQ : info->react_point;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rec_inbnd_ofp_map_get_unsafe()", nif_ndx, fc_cls_ndx);
}


/*********************************************************************
*     Set the configuration for handling Flow Control
 *     generated by (and effecting) recycling traffic. Can be
 *     triggered by crossing Virtual Statistics Queues (CT3CC),
 *     or by crossing High or Low Global Resources threshold.
 *     Handled on Outgoing FAP Port level - Scheduler HR or
 *     EGQ.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rcy_ofp_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             trgr_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_OFP_HANDLER react_point_ndx,
    SOC_SAND_IN  SOC_PB_FC_OFP_PRIORITY    prio_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_PORT_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  SOC_PB_CFC_RCL2_OFP_TBL_DATA
    rcy2ofp_tbl_data;
  uint32 ofp_ndx, bitmap_ndx, ofp_exist = FALSE;
  SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO info_incoming;
  SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO info_outgoing;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_RCY_OFP_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);
  sal_memset(&rcy2ofp_tbl_data, 0x0, sizeof(SOC_PB_CFC_RCL2_OFP_TBL_DATA));
  
  if (trgr_ndx <= SOC_PB_FC_RCY_VSQ_TRGR_NDX_MAX) /* VSQ */
  {
    /* translate the bitmap into 1 outgoing FAP port */
    for (ofp_ndx=0; ofp_ndx<SOC_PB_FC_NOF_FAP_PORTS; ofp_ndx++)
    {
      if (soc_sand_bitstream_test_bit(info->ofp_bitmap, ofp_ndx))
      {
        rcy2ofp_tbl_data.ofp_num = ofp_ndx;
        ofp_exist = TRUE;
        break;
      }
    }
    if (ofp_exist == TRUE) /* if the bitmap is not all zeroed */
    {
      res = soc_petra_port_to_interface_map_get(
          unit,
          SOC_CORE_DEFAULT,
          rcy2ofp_tbl_data.ofp_num,
          &info_incoming,
          &info_outgoing
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);
      if (info_outgoing.if_id != SOC_TMC_IF_ID_RCY)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_FLOW_CONTROL_OFP_IS_NOT_DEFINED_AS_RCY_ERR, 4, exit);
      }
      switch(react_point_ndx)
      {
      case SOC_PB_FC_RCY_OFP_HANDLER_EGQ:
        if (prio_ndx == SOC_TMC_FC_OFP_PRIORITY_HP)
        {
          rcy2ofp_tbl_data.egq_hp_valid = TRUE;
        }
        else if (prio_ndx == SOC_TMC_FC_OFP_PRIORITY_LP)
        {
          rcy2ofp_tbl_data.egq_lp_valid = TRUE;
        }
        break;
      case SOC_PB_FC_RCY_OFP_HANDLER_SCH:
        if (prio_ndx == SOC_TMC_FC_OFP_PRIORITY_HP)
        {
          rcy2ofp_tbl_data.sch_hp_valid = TRUE;
        }
        else if (prio_ndx == SOC_TMC_FC_OFP_PRIORITY_LP)
        {
          rcy2ofp_tbl_data.sch_lp_valid = TRUE;
        }
        break;
      default:
        SOC_SAND_SET_ERROR_CODE(SOC_PB_FLOW_CONTROL_REACT_POINT_NDX_OUT_OF_RANGE_ERR, 5, exit);
        break;
      }
    }
    res = soc_pb_cfc_rcl2_ofp_tbl_set_unsafe(
            unit,
            trgr_ndx,
            &rcy2ofp_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  else if (trgr_ndx == SOC_PB_FC_RCY_GLB_RCS_ID_LP || trgr_ndx == SOC_PB_FC_RCY_GLB_RCS_ID_HP) /*Global*/
  {
    /*
     *  verify that all bitmap ports are SOC_TMC_IF_ID_RCY
     */
    for (ofp_ndx=0; ofp_ndx<SOC_PB_FC_NOF_FAP_PORTS; ofp_ndx++)
    {
      if (soc_sand_bitstream_test_bit(info->ofp_bitmap, ofp_ndx))
      {
        res = soc_petra_port_to_interface_map_get(
          unit,
          SOC_CORE_DEFAULT,
          ofp_ndx,
          &info_incoming,
          &info_outgoing
          );
          SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
        if (info_outgoing.if_id != SOC_TMC_IF_ID_RCY)
        {
          SOC_SAND_SET_ERROR_CODE(SOC_PB_FLOW_CONTROL_OFP_IS_NOT_DEFINED_AS_RCY_ERR, 13, exit);
        }
      }
    }
    /*
     *  RMW - global ofp bitmaps
     */
    if (prio_ndx == SOC_TMC_FC_OFP_PRIORITY_HP)
    {
      for (bitmap_ndx=0; bitmap_ndx<SOC_PB_FC_NOF_FAP_PORTS_UINT32S; bitmap_ndx++)
      {
        SOC_PB_FLD_SET(regs->cfc.glb_rsc_to_rcl_ofp_hp_map_reg[bitmap_ndx].glb_rsc_to_rcl_ofp_hp_map, info->ofp_bitmap[bitmap_ndx], 15, exit);
      }
    }
    else if (prio_ndx == SOC_TMC_FC_OFP_PRIORITY_LP)
    {
      for (bitmap_ndx=0; bitmap_ndx<SOC_PB_FC_NOF_FAP_PORTS_UINT32S; bitmap_ndx++)
      {
        SOC_PB_FLD_SET(regs->cfc.glb_rsc_to_rcl_ofp_lp_map_reg[bitmap_ndx].glb_rsc_to_rcl_ofp_lp_map, info->ofp_bitmap[bitmap_ndx], 20, exit);
      }
    }
    else
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_FLOW_CONTROL_PRIO_NDX_OUT_OF_RANGE_ERR, 25, exit);
    }
    /*
     *  enable EGQ/SCH flow
     */
    switch(react_point_ndx)
    {
    case SOC_PB_FC_RCY_OFP_HANDLER_EGQ:
      SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.glb_rsc_to_egq_rcl_ofp_en, TRUE, 35, exit);
      SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.glb_rsc_to_sch_rcl_ofp_en, FALSE, 40, exit);
      break;
    case SOC_PB_FC_RCY_OFP_HANDLER_SCH:
      SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.glb_rsc_to_egq_rcl_ofp_en, FALSE, 45, exit);
      SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.glb_rsc_to_sch_rcl_ofp_en, TRUE, 50, exit);
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_FLOW_CONTROL_REACT_POINT_NDX_OUT_OF_RANGE_ERR, 55, exit);
      break;
    }
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_FLOW_CONTROL_TRGR_NDX_OUT_OF_RANGE_ERR, 60, exit);
  }

 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rcy_ofp_set_unsafe()", trgr_ndx, 65);
}

uint32
  soc_pb_fc_rcy_ofp_set_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             trgr_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_OFP_HANDLER react_point_ndx,
    SOC_SAND_IN  SOC_PB_FC_OFP_PRIORITY    prio_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_PORT_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ofp_ndx, ofp_cntr = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_RCY_OFP_SET_VERIFY);
  /* verify that there is at most one bit up in ofp bitmap for VSQ */
  if (trgr_ndx <= SOC_PB_FC_RCY_VSQ_TRGR_NDX_MAX)
  {
    for (ofp_ndx=0; ofp_ndx<SOC_PB_FC_NOF_FAP_PORTS; ofp_ndx++)
    {
      if (soc_sand_bitstream_test_bit(info->ofp_bitmap, ofp_ndx))
      {
        ofp_cntr++;
      }
    }
  }
  if (ofp_cntr > 1)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_FLOW_CONTROL_VSQ_BITMAP_NOT_VALID_ERR, 1, exit);
  }

  if (trgr_ndx != SOC_PB_FC_RCY_GLB_RCS_ID_LP && trgr_ndx != SOC_PB_FC_RCY_GLB_RCS_ID_HP){
    SOC_SAND_ERR_IF_ABOVE_MAX(trgr_ndx, SOC_PB_FLOW_CONTROL_TRGR_NDX_MAX, SOC_PB_FLOW_CONTROL_TRGR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(react_point_ndx, SOC_PB_FLOW_CONTROL_REACT_POINT_NDX_MAX, SOC_PB_FLOW_CONTROL_REACT_POINT_NDX_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(prio_ndx, SOC_PB_FLOW_CONTROL_PRIO_NDX_MAX, SOC_PB_FLOW_CONTROL_PRIO_NDX_OUT_OF_RANGE_ERR, 30, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_FC_RCY_PORT_INFO, info, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rcy_ofp_set_verify()", trgr_ndx, 0);
}

uint32
  soc_pb_fc_rcy_ofp_get_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             trgr_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_OFP_HANDLER react_point_ndx,
    SOC_SAND_IN  SOC_PB_FC_OFP_PRIORITY    prio_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_RCY_OFP_GET_VERIFY);

  if (trgr_ndx != SOC_PB_FC_RCY_GLB_RCS_ID_LP && trgr_ndx != SOC_PB_FC_RCY_GLB_RCS_ID_HP){
    SOC_SAND_ERR_IF_ABOVE_MAX(trgr_ndx, SOC_PB_FLOW_CONTROL_TRGR_NDX_MAX, SOC_PB_FLOW_CONTROL_TRGR_NDX_OUT_OF_RANGE_ERR, 1, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(react_point_ndx, SOC_PB_FLOW_CONTROL_REACT_POINT_NDX_MAX, SOC_PB_FLOW_CONTROL_REACT_POINT_NDX_OUT_OF_RANGE_ERR, 5, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(prio_ndx, SOC_PB_FLOW_CONTROL_PRIO_NDX_MAX, SOC_PB_FLOW_CONTROL_PRIO_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rcy_ofp_get_verify()", trgr_ndx, 0);
}

/*********************************************************************
*     Set the configuration for handling Flow Control
 *     generated by (and effecting) recycling traffic. Can be
 *     triggered by crossing Virtual Statistics Queues (CT3CC),
 *     or by crossing High or Low Global Resources threshold.
 *     Handled on Outgoing FAP Port level - Scheduler HR or
 *     EGQ.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rcy_ofp_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             trgr_ndx,
    SOC_SAND_IN  SOC_PB_FC_RCY_OFP_HANDLER react_point_ndx,
    SOC_SAND_IN  SOC_PB_FC_OFP_PRIORITY    prio_ndx,
    SOC_SAND_OUT SOC_PB_FC_RCY_PORT_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK,
    egq_val,
    sch_val,
    bitmab_chunk;
  uint32
    bitmap_ndx;
  SOC_PB_CFC_RCL2_OFP_TBL_DATA
    rcy2ofp_tbl_data;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_RCY_OFP_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_PB_FC_RCY_PORT_INFO_clear(info);
  regs = soc_petra_regs();
  
  if (trgr_ndx <= SOC_PB_FC_RCY_VSQ_TRGR_NDX_MAX) /* VSQ */
  {
    res = soc_pb_cfc_rcl2_ofp_tbl_get_unsafe(
        unit,
        trgr_ndx,
        &rcy2ofp_tbl_data
      );
    if ( /* checks if the incoming indexes values match the chip configurations */
      (react_point_ndx == SOC_PB_FC_RCY_OFP_HANDLER_EGQ && prio_ndx == SOC_TMC_FC_OFP_PRIORITY_HP && rcy2ofp_tbl_data.egq_hp_valid == TRUE) ||
      (react_point_ndx == SOC_PB_FC_RCY_OFP_HANDLER_EGQ && prio_ndx == SOC_TMC_FC_OFP_PRIORITY_LP && rcy2ofp_tbl_data.egq_lp_valid == TRUE) ||
      (react_point_ndx == SOC_PB_FC_RCY_OFP_HANDLER_SCH && prio_ndx == SOC_TMC_FC_OFP_PRIORITY_HP && rcy2ofp_tbl_data.sch_hp_valid == TRUE) ||
      (react_point_ndx == SOC_PB_FC_RCY_OFP_HANDLER_SCH && prio_ndx == SOC_TMC_FC_OFP_PRIORITY_LP && rcy2ofp_tbl_data.sch_lp_valid == TRUE)
      )
    { /*if it does return the bitmap of the FAP ports */
      res = soc_sand_bitstream_set_bit(
          info->ofp_bitmap,
          rcy2ofp_tbl_data.ofp_num
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);
    }
  }
  else if (trgr_ndx == SOC_PB_FC_RCY_GLB_RCS_ID_LP || trgr_ndx == SOC_PB_FC_RCY_GLB_RCS_ID_HP)
  {
    SOC_PB_FLD_GET(regs->cfc.cfc_enablers_reg.glb_rsc_to_egq_rcl_ofp_en, egq_val, 5, exit);
    SOC_PB_FLD_GET(regs->cfc.cfc_enablers_reg.glb_rsc_to_sch_rcl_ofp_en, sch_val, 10, exit);
    if ( /* checks if the incoming indexes values match the chip configurations */
      (sch_val == TRUE && react_point_ndx == SOC_PB_FC_RCY_OFP_HANDLER_SCH) ||
      (egq_val == TRUE && react_point_ndx == SOC_PB_FC_RCY_OFP_HANDLER_EGQ)
      )
    { /*if it does return the bitmap of the FAP ports */
      if (prio_ndx == SOC_TMC_FC_OFP_PRIORITY_HP)
      {
        for (bitmap_ndx=0; bitmap_ndx<SOC_PB_FC_NOF_FAP_PORTS_UINT32S; bitmap_ndx++)
        {
          SOC_PB_FLD_GET(regs->cfc.glb_rsc_to_rcl_ofp_hp_map_reg[bitmap_ndx].glb_rsc_to_rcl_ofp_hp_map, bitmab_chunk, 15, exit);
          info->ofp_bitmap[bitmap_ndx] = bitmab_chunk;
        }
      }
      else if (prio_ndx == SOC_TMC_FC_OFP_PRIORITY_LP)
      {
        for (bitmap_ndx=0; bitmap_ndx<SOC_PB_FC_NOF_FAP_PORTS_UINT32S; bitmap_ndx++)
        {
          SOC_PB_FLD_GET(regs->cfc.glb_rsc_to_rcl_ofp_lp_map_reg[bitmap_ndx].glb_rsc_to_rcl_ofp_lp_map, bitmab_chunk, 20, exit);
          info->ofp_bitmap[bitmap_ndx] = bitmab_chunk;
        }
      }
      else
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_FLOW_CONTROL_PRIO_NDX_OUT_OF_RANGE_ERR, 25, exit);
      }
    }
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_FLOW_CONTROL_TRGR_NDX_OUT_OF_RANGE_ERR, 30, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rcy_ofp_get_unsafe()", trgr_ndx, 0);
}

/*********************************************************************
*     Enable/disable reception of Flow Control for Recycling
 *     ports by end to end Scheduler HR-s 192-223, represented
 *     by Category 3 Connection Class in Ingress Statistics
 *     Queues, when the Statistics Queues threshold is crossed.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rcy_hr_enable_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint8                      enable
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_val = 0;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_RCY_HR_ENABLE_SET_UNSAFE);

  fld_val = SOC_SAND_BOOL2NUM(enable);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.rcl_vsq_to_sch_hr_en, fld_val, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_fc_rcy_hr_enable_set_unsafe()",0,0);
}

uint32
  soc_pb_fc_rcy_hr_enable_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint8                      enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_RCY_HR_ENABLE_SET_VERIFY);

  SOC_PB_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rcy_hr_enable_set_verify()", 0, 0);
}

uint32
  soc_pb_fc_rcy_hr_enable_get_verify(
    SOC_SAND_IN  int                      unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_RCY_HR_ENABLE_GET_VERIFY);

  SOC_PB_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rcy_hr_enable_get_verify()", 0, 0);
}

/*********************************************************************
*     Enable/disable reception of Flow Control for Recycling
 *     ports by end to end Scheduler HR-s 192-223, represented
 *     by Category 3 Connection Class in Ingress Statistics
 *     Queues, when the Statistics Queues threshold is crossed.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rcy_hr_enable_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint8                      *enable
  )
{
  uint32
    fld_val = 0,
    res;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_RCY_HR_ENABLE_GET_UNSAFE);

  SOC_PB_FLD_GET(regs->cfc.cfc_enablers_reg.rcl_vsq_to_sch_hr_en, fld_val, 10, exit);
  *enable = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_fc_rcy_hr_enable_get_unsafe()",0,0);
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Generation (OOB/ILKN-Inband TX).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_gen_cal_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_PB_FC_GEN_CALENDAR             *cal_buff
  )
{
  uint32
    fld_val = 0,
    res;
  uint32
    cal_reps,
    rep_idx = 0,      /* Calendar repetition index */
    per_rep_idx = 0,  /* Calendar entry index, per repetition */
    entry_idx = 0;    /* Calendar entry index, global */
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  SOC_PB_CFC_CAL_TX_SELECT_INFO
    cal_select_info;
  SOC_PB_CFC_CALTX_TBL_DATA
    oob_tx_cal;
  SOC_PETRA_REG_FIELD
    *cal_len_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_CAL_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cal_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  res = soc_pb_fc_cal_tx_select_info_unsafe(
          unit,
          cal_mode_ndx,
          if_ndx,
          &cal_select_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  cal_reps = SOC_PB_FC_CAL_MODE_IS_ILKN(cal_mode_ndx)?1:cal_conf->cal_reps;

  /* Enable/disable */
  SOC_PB_FLD_SET(regs->cfc.oob_pad_configuration_reg.oob_mode[if_ndx], SOC_SAND_BOOL2NUM(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB), 10, exit);
  SOC_PB_FLD_SET(regs->cfc.ilkn_configuration_reg[if_ndx].ilkn_tx_en, SOC_SAND_BOOL2NUM(cal_conf->enable), 14, exit);
  SOC_PB_FLD_SET(regs->cfc.ilkn_configuration_reg[if_ndx].ilkn_tx_sel, SOC_SAND_BOOL2NUM(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB), 18, exit);



  /*  If interface B - set for FC Generation */
  if (if_ndx == SOC_TMC_FC_OOB_ID_B)
  {
    SOC_PB_FLD_SET(regs->cfc.out_of_band_tx_configuration0_reg.oob_tx_en, 0x1, 22, exit);
    SOC_PB_FLD_SET(regs->cfc.oob_pad_configuration_reg.oob_intrfb_sel, 0x1, 24, exit);
  }

  /* Calendar length (single repetition) */
  if (cal_select_info.cal_len_reg && cal_conf->cal_len > 0)
  {
    if(SOC_PB_FC_CAL_MODE_IS_ILKN(cal_mode_ndx))
    {
      /* Register value for Calendar length for ILKN should be -1 from actual length */
      fld_val = cal_conf->cal_len - 1;
    }
    else
    {
      fld_val = cal_conf->cal_len;
    }
    res = soc_petra_write_fld_unsafe(unit, cal_select_info.cal_len_reg, SOC_PETRA_DEFAULT_INSTANCE, fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);
  }

  /* For Interlaken inband FC these fields should be configured, too */
  if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND)
  {
    cal_len_fld = SOC_PB_REG_DB_ACC_REF(regs->nbi.tx_ilkn_control1_reg[if_ndx == SOC_TMC_FC_OOB_ID_A ? 0 : 1].tx_fc_cal_len);
    if (cal_conf->cal_len <= 15)
    {
      fld_val = 0x0;
    }
    else if (cal_conf->cal_len <= 31)
    {
      fld_val = 0x1;
    }
    else if (cal_conf->cal_len <= 63)
    {
      fld_val = 0x3;
    }
    else if (cal_conf->cal_len <= 127)
    {
      fld_val = 0x7;
    }
    else
    {
      fld_val = 0xf;
    }
    res = soc_petra_write_fld_unsafe(unit, cal_len_fld, SOC_PETRA_DEFAULT_INSTANCE, fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  /* Calendar parameters { */
  /* Calendar number of repetitions */
  if (cal_select_info.cal_m_reg)
  {
    fld_val = cal_conf->cal_reps;
    res = soc_petra_write_fld_unsafe(unit, cal_select_info.cal_m_reg, SOC_PETRA_DEFAULT_INSTANCE, fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }

  /* Only interface B can be set for FC Generation */
  if (cal_mode_ndx != SOC_TMC_FC_CAL_MODE_ILKN_INBND)
  {
    SOC_PB_FLD_SET(regs->cfc.oob_pad_configuration_reg.oob_intrfb_sel, 0x1, 10, exit);
  } /* Calendar parameters } */

  /* Calendar entries { */
  for (rep_idx = 0; rep_idx < cal_reps; rep_idx++)
  {
    for (per_rep_idx = 0; per_rep_idx < cal_conf->cal_len; per_rep_idx++)
    {
      entry_idx = rep_idx * cal_conf->cal_len + per_rep_idx;

      if (cal_buff[per_rep_idx].source == SOC_PB_FC_GEN_CAL_SRC_NONE)
      {
        oob_tx_cal.fc_source_sel = SOC_PB_FC_OOB_CAL_EMPTY_SEL;
        oob_tx_cal.fc_index = SOC_PB_FC_OOB_CAL_EMPTY_ID;
      }
      else
      {
        oob_tx_cal.fc_source_sel = soc_pb_fc_gen_cal_src_type_to_val_internal(cal_buff[per_rep_idx].source);
        oob_tx_cal.fc_index = cal_buff[per_rep_idx].id;
      }

      res = cal_select_info.cal_tbl_set(
              unit,
              if_ndx,
              entry_idx,
              &oob_tx_cal
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    }
  } /* Calendar entries } */

  /* Take the interface (A/B) out of reset if using ILKN OOB */
  if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB)
  {
    fld_val = SOC_SAND_BOOL2NUM(cal_conf->enable);
    SOC_PB_FLD_SET(regs->cfc.oob_pad_configuration_reg.ilkn_oob_tx_rstn, fld_val, 80, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_cal_set_unsafe()", cal_mode_ndx, if_ndx);
}

uint32
  soc_pb_fc_gen_cal_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_PB_FC_GEN_CALENDAR             *cal_buff
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_CAL_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cal_mode_ndx, SOC_PB_CAL_MODE_NDX_MAX, SOC_PB_FC_CAL_MODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(if_ndx, SOC_PB_IF_NDX_MAX, SOC_PB_FC_CAL_IF_NDX_OUT_OF_RANGE_ERR, 20, exit);
  res = SOC_PB_FC_CAL_IF_INFO_verify(cal_conf, cal_mode_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (idx = 0; idx < cal_conf->cal_len; ++idx)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_FC_GEN_CALENDAR, cal_buff + idx, 40, exit);
  }
  
  if ((if_ndx == SOC_TMC_FC_OOB_ID_A) && (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_FC_FLOW_CONTROL_NOT_SUPPORTED_ON_IF_ERR, 50, exit);
  }
  
  if ((if_ndx == SOC_TMC_FC_OOB_ID_A) && (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_FC_FLOW_CONTROL_NOT_SUPPORTED_ON_IF_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_cal_set_verify()", cal_mode_ndx, if_ndx);
}

uint32
  soc_pb_fc_gen_cal_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_CAL_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cal_mode_ndx, SOC_PB_CAL_MODE_NDX_MAX, SOC_PB_FC_CAL_MODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(if_ndx, SOC_PB_IF_NDX_MAX, SOC_PB_FC_CAL_IF_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_gen_cal_get_verify()", cal_mode_ndx, if_ndx);
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Generation (OOB/ILKN-Inband TX).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_gen_cal_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_PB_FC_GEN_CALENDAR             *cal_buff
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_val = 0;
  uint32
    entry_idx = 0;    /* Calendar entry index, global */
  SOC_PB_CFC_CALTX_TBL_DATA
    oob_tx_cal;
  SOC_PB_CFC_CAL_TX_SELECT_INFO
    cal_select_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_GEN_CAL_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cal_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  SOC_PB_FC_CAL_IF_INFO_clear(cal_conf);
  SOC_PB_FC_GEN_CALENDAR_clear(cal_buff);

  res = soc_pb_fc_cal_tx_select_info_unsafe(
          unit,
          cal_mode_ndx,
          if_ndx,
          &cal_select_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit)

  /* Calendar parameters { */
  /* Enable/disable */
  if (cal_select_info.enable_reg)
  {
    res = soc_petra_read_fld_unsafe(unit, cal_select_info.enable_reg, SOC_PETRA_DEFAULT_INSTANCE, &fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    cal_conf->enable = SOC_SAND_NUM2BOOL(fld_val);
  }

  /* Calendar length (single repetition) */
  if (cal_select_info.cal_len_reg)
  {
    res = soc_petra_read_fld_unsafe(unit, cal_select_info.cal_len_reg, SOC_PETRA_DEFAULT_INSTANCE, &fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    cal_conf->cal_len = fld_val;
  }

  /*  Calendar number of repetitions */
  if (cal_select_info.cal_m_reg)
  {
    res = soc_petra_read_fld_unsafe(unit, cal_select_info.cal_m_reg, SOC_PETRA_DEFAULT_INSTANCE, &fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    cal_conf->cal_reps = fld_val;
  }
  else
  {
    cal_conf->cal_reps = 1;
  }
  /* Calendar parameters } */

  /* Calendar entries { */

  for (entry_idx = 0; entry_idx < cal_conf->cal_len; entry_idx++)
  {
    res = cal_select_info.cal_tbl_get(
            unit,
            if_ndx,
            entry_idx,
            &oob_tx_cal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    if ((oob_tx_cal.fc_source_sel == SOC_PB_FC_OOB_CAL_EMPTY_SEL) &&
        (oob_tx_cal.fc_index == SOC_PB_FC_OOB_CAL_EMPTY_ID))
    {
      cal_buff[entry_idx].source = SOC_PB_FC_GEN_CAL_SRC_NONE;
      cal_buff[entry_idx].id = 0; /* Irrelevant */
    }
    else
    {
      cal_buff[entry_idx].source = oob_tx_cal.fc_source_sel;
      cal_buff[entry_idx].id = oob_tx_cal.fc_index;
    }
  }
  /* Calendar entries } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_fc_gen_cal_get_unsafe()",cal_mode_ndx,0);
}

STATIC uint32
  soc_pb_fc_egr_rec_oob_tables_write(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                           if_ndx,
    SOC_SAND_IN  uint32                              oob_rx_cal_entry_idx,
    SOC_SAND_IN  SOC_PB_CFC_CAL_RX_SELECT_INFO              *cal_select_info,
    SOC_SAND_IN  SOC_PB_CFC_CALRX_TBL_DATA                  *rx_cal,
    SOC_SAND_IN  SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA            *hr_map,
    SOC_SAND_IN  uint8                              is_sch_ofp_hr
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_EGR_REC_OOB_TABLES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rx_cal);

  if (is_sch_ofp_hr)
  {
    SOC_SAND_CHECK_NULL_INPUT(hr_map);
  }

  res = cal_select_info->cal_tbl_set(
          unit,
          if_ndx,
          oob_rx_cal_entry_idx,
          rx_cal
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (is_sch_ofp_hr)
  {
    res = cal_select_info->sch_map_tbl_set(
            unit,
            if_ndx,
            rx_cal->fc_index,
            hr_map
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_fc_egr_rec_oob_tables_write()",0,0);
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rec_cal_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_PB_FC_REC_CALENDAR             *cal_buff
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_val = 0,
    reg_val = 0,
    ofp_mapping_indx;
  uint32
    rep_idx = 0,      /* Calendar repetition index */
    per_rep_idx = 0,  /* Calendar entry index, per repetition */
    entry_idx = 0,    /* Calendar entry index, global */
    reg_idx,
    idx,
    first_entry = 0,
  cal_reps;
  uint8
    is_sch_ofp_hr = FALSE,
    must_stop = FALSE,
    table_entries_to_existing_general_hr_mapping[SOC_PB_FC_SCH_BASED_OFP_MAPPING_TABLE_LENGTH];
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  SOC_PB_CFC_CALRX_TBL_DATA
    rx_cal;
  SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA
    hr_map;
  SOC_PETRA_TBLS
    *tables;
  uint8
    current_entry_for_ofp_hr = 0,
    curr_entry_for_ofp_hr_idx,
    ofp_hr_to_table_entry_mapping[SOC_PB_FC_NOF_OFP_HRS];
  SOC_PB_CFC_CAL_RX_SELECT_INFO
    cal_select_info;
  SOC_PETRA_REG_FIELD
    *cal_len_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_CAL_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cal_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  res = soc_petra_tbls_get(&tables);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PETRA_CLEAR(table_entries_to_existing_general_hr_mapping, uint8, SOC_PB_FC_SCH_BASED_OFP_MAPPING_TABLE_LENGTH);

  res = soc_pb_fc_cal_rx_select_info_unsafe(
          unit,
          cal_mode_ndx,
          if_ndx,
          &cal_select_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  cal_reps = SOC_PB_FC_CAL_MODE_IS_ILKN(cal_mode_ndx) ? 1 : cal_conf->cal_reps;

  /* Disable the FC before configuring it */
  SOC_PB_FLD_SET(regs->cfc.ilkn_configuration_reg[if_ndx].ilkn_rx_en, 0, 82, exit);
  SOC_PB_FLD_SET(regs->cfc.out_of_band_rx_configuration_reg[if_ndx].oob_rx_en, 0, 86, exit);

  /* Clear calendar */
  res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, (if_ndx == SOC_TMC_FC_OOB_ID_A) ? SOC_PB_TBL(tables->cfc.calrxa_tbl).addr.size : SOC_PB_TBL(tables->cfc.calrxb_tbl).addr.size);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  rx_cal.fc_dest_sel = 0;
  rx_cal.fc_index = 0;
  res = cal_select_info.cal_tbl_set(
          unit,
          if_ndx,
          first_entry,
          &rx_cal
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, (if_ndx == SOC_TMC_FC_OOB_ID_A) ? SOC_PB_TBL(tables->cfc.oob0_sch_map_tbl).addr.size : SOC_PB_TBL(tables->cfc.oob1_sch_map_tbl).addr.size);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  hr_map.ofp_hr = SOC_PB_FC_REC_OOB_SCH_BASED2OFP_INIT_VAL;
  hr_map.lp_ofp_valid = 0;
  hr_map.hp_ofp_valid = 0;
  res = cal_select_info.sch_map_tbl_set(
          unit,
          if_ndx,
          first_entry,
          &hr_map
        );

  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  for (idx = 0; idx < SOC_PB_FC_NOF_OFP_HRS; ++idx)
  {
    ofp_hr_to_table_entry_mapping[idx] = SOC_PB_FC_OFP_HR2ENTRY_INIT_VAL;
  }

  /*  Initialization of the oob enable registers for the general SCH HR */
  for (reg_idx = 0; reg_idx < SOC_PB_CFC_OOB_RX_TO_SCH_HR_MASK_REG_MULT_NOF_REGS; reg_idx++)
  {
    if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
    {
      SOC_PB_REG_SET(regs->cfc.oob_rx_to_sch_hr_mask_reg[reg_idx].rx_to_sch_hr_mask, 0x0, 72, exit);
    }
    else
    {
      SOC_PB_REG_SET(regs->cfc.ilkn_rx_to_sch_hr_mask_reg[reg_idx].rx_to_sch_hr_mask, 0x0, 74, exit);
    }
  }

  /* Calendar parameters { */
  /*  Enable/disable */
  SOC_PB_FLD_SET(regs->cfc.oob_pad_configuration_reg.oob_mode[if_ndx], SOC_SAND_BOOL2NUM(SOC_PB_FC_CAL_MODE_IS_ILKN(cal_mode_ndx)), 80, exit);
  SOC_PB_FLD_SET(regs->cfc.ilkn_configuration_reg[if_ndx].ilkn_rx_en, SOC_SAND_BOOL2NUM(SOC_PB_FC_CAL_MODE_IS_ILKN(cal_mode_ndx) && cal_conf->enable), 82, exit);
  SOC_PB_FLD_SET(regs->cfc.ilkn_configuration_reg[if_ndx].ilkn_rx_sel, SOC_SAND_BOOL2NUM(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB), 84, exit);
  SOC_PB_FLD_SET(regs->cfc.out_of_band_rx_configuration_reg[if_ndx].oob_rx_en, SOC_SAND_BOOL2NUM((cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB) && cal_conf->enable), 86, exit);
  
  /* Set CFC Enablers for ILKN */
  fld_val = SOC_PB_FC_CAL_MODE_IS_ILKN(cal_mode_ndx);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.ilkn_rx_to_sch_hr_en,SOC_SAND_BOOL2NUM(fld_val && cal_conf->enable), 86, exit);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.ilkn_rx_to_egq_en,SOC_SAND_BOOL2NUM(fld_val && cal_conf->enable), 86, exit);

  /* Set CFC Enablers for OOB */
  fld_val = SOC_SAND_BOOL2NUM(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB || cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.oob_rx_to_sch_hr_en,SOC_SAND_BOOL2NUM(fld_val && cal_conf->enable), 86, exit);
  SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.oob_rx_to_egq_en,SOC_SAND_BOOL2NUM(fld_val && cal_conf->enable), 86, exit);

  /*  If interface B - set for FC Reception */
  if (if_ndx == SOC_TMC_FC_OOB_ID_B)
  {
    SOC_PB_FLD_SET(regs->cfc.out_of_band_tx_configuration0_reg.oob_tx_en, 0x0, 88, exit);
    SOC_PB_FLD_SET(regs->cfc.oob_pad_configuration_reg.oob_intrfb_sel, 0x0, 90, exit);
  }

  /*  Calendar length (single repetition) - only set if len>0 and enable=1 */
  if (cal_conf->enable && cal_select_info.cal_len_reg && cal_conf->cal_len)
  {
    if(SOC_PB_FC_CAL_MODE_IS_ILKN(cal_mode_ndx))
    {
      /* Register value for Calendar length for ILKN should be -1 from actual length */
      fld_val = cal_conf->cal_len - 1;
    }
    else
    {
      fld_val = cal_conf->cal_len;
    }
    res = soc_petra_write_fld_unsafe(unit, cal_select_info.cal_len_reg, SOC_PETRA_DEFAULT_INSTANCE, fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  }

  /* For Interlaken inband FC these fields should be configured, too */
  if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_INBND)
  {
    cal_len_fld = SOC_PB_REG_DB_ACC_REF(regs->nbi.tx_ilkn_control1_reg[if_ndx == SOC_TMC_FC_OOB_ID_A ? 0 : 1].tx_fc_cal_len);
    if (cal_conf->cal_len <= 15)
    {
      fld_val = 0x0;
    }
    else if (cal_conf->cal_len <= 31)
    {
      fld_val = 0x1;
    }
    else if (cal_conf->cal_len <= 63)
    {
      fld_val = 0x3;
    }
    else if (cal_conf->cal_len <= 127)
    {
      fld_val = 0x7;
    }
    else
    {
      fld_val = 0xf;
    }
    res = soc_petra_write_fld_unsafe(unit, cal_len_fld, SOC_PETRA_DEFAULT_INSTANCE, fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);
  }
  
  /*  Calendar number of repetitions */
  if (cal_select_info.cal_m_reg)
  {
    fld_val = cal_reps;
    res = soc_petra_write_fld_unsafe(unit, cal_select_info.cal_m_reg, SOC_PETRA_DEFAULT_INSTANCE, fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  }
  /* Calendar parameters } */

  /* Calendar entries { */

  /* A first loop on the calendar to insert all the entries which are not sch-based ofp hr */
  for (per_rep_idx = 0; per_rep_idx < cal_conf->cal_len; per_rep_idx++)
  {
    is_sch_ofp_hr = FALSE;

    switch(cal_buff[per_rep_idx].destination)
    {
    case SOC_PB_FC_REC_CAL_DEST_HR:
      /* Handled by HR scheduler - ordinary HR 128-255. */
      rx_cal.fc_dest_sel = 0x0;
      rx_cal.fc_index = cal_buff[per_rep_idx].id - SOC_PB_FC_OOB_CAL_HR_ID_MIN;
      break;
    case SOC_PB_FC_REC_CAL_DEST_OFP_SCH_HR_LP:
    case SOC_PB_FC_REC_CAL_DEST_OFP_SCH_HR_HP:
      /* Is skipped in the first loop */
      is_sch_ofp_hr = TRUE;
      break;
    case SOC_PB_FC_REC_CAL_DEST_OFP_EGQ_HP:
      /*  Handled by OFP-at the EGQ */
      rx_cal.fc_dest_sel = 0x1;
      rx_cal.fc_index = cal_buff[per_rep_idx].id;
      break;
    case SOC_PB_FC_REC_CAL_DEST_OFP_EGQ_LP:
      /*  Handled by OFP-at the EGQ */
      rx_cal.fc_dest_sel = 0x2;
      rx_cal.fc_index = cal_buff[per_rep_idx].id;
      break;
    case SOC_PB_FC_REC_CAL_DEST_NIF:
      rx_cal.fc_dest_sel = 0x3;
      rx_cal.fc_index    = cal_buff[per_rep_idx].id;
      if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
      {
        SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.oob_rx_to_nif_fast_llfc_en, cal_conf->enable, 144, exit);
      }
      if(SOC_PB_FC_CAL_MODE_IS_ILKN(cal_mode_ndx))
      {
        SOC_PB_FLD_SET(regs->cfc.cfc_enablers_reg.ilkn_rx_to_nif_fast_llfc_en, cal_conf->enable, 145, exit);
      }
      break;
    case SOC_PB_FC_REC_CAL_DEST_NONE:
      rx_cal.fc_dest_sel = SOC_PB_FC_OOB_CAL_EMPTY_SEL;
      rx_cal.fc_index = SOC_PB_FC_OOB_CAL_EMPTY_ID;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_FC_OOB_CAL_DEST_TYPE_INVALID_ERR, 150, exit);
    }

    if (!is_sch_ofp_hr)
    {
      /* Scheduler OFP-HR mapping in case of SCH (but not OFP) HR */
      if (cal_buff[per_rep_idx].destination == SOC_PB_FC_REC_CAL_DEST_HR)
      {
        /*
         *  Invalid for OFP - indicates that this entry is not usable for an OFP HR
         *  in the boolean table
         */
        table_entries_to_existing_general_hr_mapping[rx_cal.fc_index] = TRUE;

        /* Enable the oob for this sch general hr */
        reg_idx = SOC_PETRA_REG_IDX_GET(rx_cal.fc_index, SOC_SAND_REG_SIZE_BITS);
        if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
        {
          SOC_PB_REG_GET(regs->cfc.oob_rx_to_sch_hr_mask_reg[reg_idx].rx_to_sch_hr_mask, reg_val, 152, exit);
          SOC_SAND_SET_BIT(reg_val, 0x1, SOC_PETRA_FLD_IDX_GET(rx_cal.fc_index, SOC_SAND_REG_SIZE_BITS));
          SOC_PB_REG_SET(regs->cfc.oob_rx_to_sch_hr_mask_reg[reg_idx].rx_to_sch_hr_mask, reg_val, 153, exit);
        }
        else
        {
          SOC_PB_REG_GET(regs->cfc.ilkn_rx_to_sch_hr_mask_reg[reg_idx].rx_to_sch_hr_mask, reg_val, 154, exit);
          SOC_SAND_SET_BIT(reg_val, 0x1, SOC_PETRA_FLD_IDX_GET(rx_cal.fc_index, SOC_SAND_REG_SIZE_BITS));
          SOC_PB_REG_SET(regs->cfc.ilkn_rx_to_sch_hr_mask_reg[reg_idx].rx_to_sch_hr_mask, reg_val, 53, exit);
        }
      }

      for (rep_idx = 0; rep_idx < cal_reps; rep_idx++)
      {
        entry_idx = rep_idx * cal_conf->cal_len + per_rep_idx;

        res = soc_pb_fc_egr_rec_oob_tables_write(
                unit,
                if_ndx,
                entry_idx,
                &cal_select_info,
                &rx_cal,
                &hr_map,
                FALSE
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
      }
    }
  }

  /*
   *  A second loop on the calendar to insert the sch-based ofp hr entries with the
   *  knowledge of all the existing sch-based gen hr
   */
  for (per_rep_idx = 0; per_rep_idx < cal_conf->cal_len; per_rep_idx++)
  {
    if ((cal_buff[per_rep_idx].destination == SOC_PB_FC_REC_CAL_DEST_OFP_SCH_HR_LP) || (cal_buff[per_rep_idx].destination == SOC_PB_FC_REC_CAL_DEST_OFP_SCH_HR_HP))
    {
      /* Handled by HR scheduler - OFP HR, prioritized (Hi/Low priority) */
      rx_cal.fc_dest_sel = 0x0;

      /*  Search if this Sch OFP HR was inserted */
      ofp_mapping_indx = cal_buff[per_rep_idx].id;
      if (ofp_hr_to_table_entry_mapping[ofp_mapping_indx] != SOC_PB_FC_OFP_HR2ENTRY_INIT_VAL)
      {
        /* The entry was inserted */
        rx_cal.fc_index = ofp_hr_to_table_entry_mapping[ofp_mapping_indx];
      }
      else
      {
        /* Look for the first empty entry in the table */
        must_stop = FALSE;
       for (
              curr_entry_for_ofp_hr_idx = current_entry_for_ofp_hr;
              (curr_entry_for_ofp_hr_idx < SOC_PB_FC_SCH_BASED_OFP_MAPPING_TABLE_LENGTH) && (!must_stop);
              ++curr_entry_for_ofp_hr_idx
           )
        {
          if(table_entries_to_existing_general_hr_mapping[curr_entry_for_ofp_hr_idx] == FALSE)
          {
            must_stop = TRUE;
          }
        }
        if (must_stop == FALSE)
        {
          /* The table is full */
          SOC_SAND_SET_ERROR_CODE(SOC_PB_FC_OOB_CAL_EXCESSIVE_NOF_SCH_OFP_HRS_ERR, 165, exit);
        }
        table_entries_to_existing_general_hr_mapping[current_entry_for_ofp_hr] = TRUE;
        rx_cal.fc_index = current_entry_for_ofp_hr;
        ofp_hr_to_table_entry_mapping[ofp_mapping_indx] = current_entry_for_ofp_hr;
        current_entry_for_ofp_hr = curr_entry_for_ofp_hr_idx;
      } /*  Scheduler OFP-HR High/Low priority mapping */

      /*  Getting the current value of the table entry (to update) */
      res = cal_select_info.sch_map_tbl_get(
              unit,
              if_ndx,
              rx_cal.fc_index,
              &hr_map
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

      hr_map.ofp_hr = cal_buff[per_rep_idx].id;
      if (cal_buff[per_rep_idx].destination == SOC_PB_FC_REC_CAL_DEST_OFP_SCH_HR_LP)
      {
        hr_map.lp_ofp_valid = TRUE;
      }
      else /* SCH OFP HR HP */
      {
        hr_map.hp_ofp_valid = TRUE;
      }

      for (rep_idx = 0; rep_idx < cal_reps; rep_idx++)
      {
        entry_idx = rep_idx * cal_conf->cal_len + per_rep_idx;

        res = soc_pb_fc_egr_rec_oob_tables_write(
                unit,
                if_ndx,
                entry_idx,
                &cal_select_info,
                &rx_cal,
                &hr_map,
                TRUE
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
      }
    }
  }

  /* Take the interface (A/B) out of reset if using ILKN/SPI OOB */
  if(cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB || cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB)
  {
    fld_val = SOC_SAND_BOOL2NUM(cal_conf->enable);    
    SOC_PB_FLD_SET(regs->cfc.oob_pad_configuration_reg.oob_rx_rstn[if_ndx], fld_val, 190, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_egr_rec_oob_set_unsafe()",cal_mode_ndx,if_ndx);
}

uint32
  soc_pb_fc_rec_cal_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_IN  SOC_PB_FC_REC_CALENDAR             *cal_buff
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    idx,
  spi_oob_status,
  ilkn_oob_en,
  ilkn_oob_sel;
  uint8
    ilkn_oob_enabled,
    spi_oob_enabled;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_CAL_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cal_mode_ndx, SOC_PB_CAL_MODE_NDX_MAX, SOC_PB_FC_CAL_MODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(if_ndx, SOC_PB_IF_NDX_MAX, SOC_PB_FC_CAL_IF_NDX_OUT_OF_RANGE_ERR, 20, exit);
  res = SOC_PB_FC_CAL_IF_INFO_verify(cal_conf, cal_mode_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (idx = 0; idx < cal_conf->cal_len; ++idx)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_FC_REC_CALENDAR, cal_buff + idx, 40, exit);
  }

  /* Check the SPI OOB and ILKN OOB status */
  SOC_PB_FLD_GET(regs->cfc.out_of_band_rx_configuration_reg[if_ndx].oob_rx_en, spi_oob_status, 50, exit);
  spi_oob_enabled = SOC_SAND_NUM2BOOL(spi_oob_status);

  SOC_PB_FLD_GET(regs->cfc.ilkn_configuration_reg[if_ndx].ilkn_rx_en, ilkn_oob_en, 60, exit);
  SOC_PB_FLD_GET(regs->cfc.ilkn_configuration_reg[if_ndx].ilkn_rx_sel, ilkn_oob_sel, 70, exit);
  ilkn_oob_enabled = SOC_SAND_NUM2BOOL((ilkn_oob_en) && (ilkn_oob_sel == 0x1));

  /* Check for conflicts between ILKN OOB and SPI OOB on the same interface */
  if(cal_conf->enable && (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_ILKN_OOB) && spi_oob_enabled)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_FC_FLOW_CONTROL_OOB_ILKN_AND_SPI_ERR, 80, exit);
  }

  if(cal_conf->enable && (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB) && ilkn_oob_enabled)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_FC_FLOW_CONTROL_OOB_ILKN_AND_SPI_ERR, 90, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rec_cal_set_verify()", cal_mode_ndx, if_ndx);
}

uint32
  soc_pb_fc_rec_cal_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_CAL_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(cal_mode_ndx, SOC_PB_CAL_MODE_NDX_MAX, SOC_PB_FC_CAL_MODE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(if_ndx, SOC_PB_IF_NDX_MAX, SOC_PB_FC_CAL_IF_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_rec_cal_get_verify()", 0, if_ndx);
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_rec_cal_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE                 cal_mode_ndx,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_PB_FC_CAL_IF_INFO              *cal_conf,
    SOC_SAND_OUT SOC_PB_FC_REC_CALENDAR             *cal_buff
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_val = 0,
    reg_val,
    reg_idx;
  uint32
    entry_idx = 0;    /* Calendar entry index */
  uint8
    lp_and_hp_already_seen[SOC_PB_FC_NOF_OFP_HRS] = {FALSE},
    is_dest_hr = FALSE;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  SOC_PB_CFC_CALRX_TBL_DATA
    rx_cal;
  SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA
    hr_map;
  SOC_PB_CFC_CAL_RX_SELECT_INFO
    cal_select_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_REC_CAL_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cal_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  /*  Initialize to keep the compiler quiet */
  hr_map.ofp_hr = SOC_PETRA_FAP_PORT_ID_INVALID;
  hr_map.lp_ofp_valid = FALSE;
  hr_map.hp_ofp_valid = FALSE;

  res = soc_pb_fc_cal_rx_select_info_unsafe(
          unit,
          cal_mode_ndx,
          if_ndx,
          &cal_select_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Calendar parameters { */

  /*  Enable/disable */
  if (cal_select_info.enable_reg)
  {
    res = soc_petra_read_fld_unsafe(unit, cal_select_info.enable_reg, SOC_PETRA_DEFAULT_INSTANCE, &fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    cal_conf->enable = SOC_SAND_NUM2BOOL(fld_val);
  }

  /* Calendar length (single repetition) */
  if (cal_select_info.cal_len_reg)
  {
    res = soc_petra_read_fld_unsafe(unit, cal_select_info.cal_len_reg, SOC_PETRA_DEFAULT_INSTANCE, &fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    cal_conf->cal_len = fld_val + 1;
  }

  /* Calendar number of repetitions */
  if (cal_select_info.cal_m_reg)
  {
    res = soc_petra_read_fld_unsafe(unit, cal_select_info.cal_m_reg, SOC_PETRA_DEFAULT_INSTANCE, &fld_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    cal_conf->cal_reps = fld_val;
  }
  else
  {
    cal_conf->cal_reps = 1;
  }
  /* Calendar parameters } */

  /* Calendar entries { */

  for (entry_idx = 0; entry_idx < cal_conf->cal_len; entry_idx++)
  {
    res = cal_select_info.cal_tbl_get(
            unit,
            if_ndx,
            entry_idx,
            &rx_cal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    is_dest_hr = SOC_SAND_NUM2BOOL(rx_cal.fc_dest_sel == 0x0);

    if (is_dest_hr)
    {
      res = cal_select_info.sch_map_tbl_get(
                unit,
                if_ndx,
                rx_cal.fc_index,
                &hr_map
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }

    if (is_dest_hr)
    {
      /*
       *  Scheduler HR - check if regular HR 128-255, or Port HR, Prioritized
       *  If in the oob fc to ofp hr mapping table, an entry has both priorities HP and LP
       *  valid, the first occurrence in the calendar points to the
       *  entry with a low priority and then assign high priority for the next times.
       */
      if (hr_map.hp_ofp_valid)
      {
        if (hr_map.lp_ofp_valid)
        {
          if (lp_and_hp_already_seen[hr_map.ofp_hr] == FALSE)
          {
            cal_buff[entry_idx].destination = SOC_PB_FC_REC_CAL_DEST_OFP_SCH_HR_LP;
            cal_buff[entry_idx].id = hr_map.ofp_hr;
            lp_and_hp_already_seen[hr_map.ofp_hr] = TRUE;
          }
          else
          {
            cal_buff[entry_idx].destination = SOC_PB_FC_REC_CAL_DEST_OFP_SCH_HR_HP;
            cal_buff[entry_idx].id = hr_map.ofp_hr;
          }
        }
        else
        {
          cal_buff[entry_idx].destination = SOC_PB_FC_REC_CAL_DEST_OFP_SCH_HR_HP;
          cal_buff[entry_idx].id = hr_map.ofp_hr;
        }
      }
      else
      {
        if (hr_map.lp_ofp_valid)
        {
          cal_buff[entry_idx].destination = SOC_PB_FC_REC_CAL_DEST_OFP_SCH_HR_LP;
          cal_buff[entry_idx].id = hr_map.ofp_hr;
        }
        else
        {
          if (cal_mode_ndx == SOC_TMC_FC_CAL_MODE_SPI_OOB)
          {
            reg_idx = SOC_PETRA_REG_IDX_GET(rx_cal.fc_index, SOC_SAND_REG_SIZE_BITS);
            SOC_PB_REG_GET(regs->cfc.oob_rx_to_sch_hr_mask_reg[reg_idx].rx_to_sch_hr_mask, reg_val, 52, exit);
            fld_val = SOC_SAND_GET_BIT(reg_val, SOC_PETRA_FLD_IDX_GET(rx_cal.fc_index, SOC_SAND_REG_SIZE_BITS));
          }
          else
          {
            reg_idx = SOC_PETRA_REG_IDX_GET(rx_cal.fc_index, SOC_SAND_REG_SIZE_BITS);
            SOC_PB_REG_GET(regs->cfc.ilkn_rx_to_sch_hr_mask_reg[reg_idx].rx_to_sch_hr_mask, reg_val, 52, exit);
            fld_val = SOC_SAND_GET_BIT(reg_val, SOC_PETRA_FLD_IDX_GET(rx_cal.fc_index, SOC_SAND_REG_SIZE_BITS));
          }
          if (fld_val != 0)
          {
            cal_buff[entry_idx].destination = SOC_PB_FC_REC_CAL_DEST_HR;
            cal_buff[entry_idx].id = rx_cal.fc_index + SOC_PB_FC_OOB_CAL_HR_ID_MIN;
          }
          else
          {
            /* Should not get here - mis-configured calendar */
            cal_buff[entry_idx].destination = rx_cal.fc_dest_sel;
            cal_buff[entry_idx].id = rx_cal.fc_index;
          }
        }
      }
    }
    else if (rx_cal.fc_dest_sel == 0x1)
    {
      /* EGQ port */
      cal_buff[entry_idx].destination = SOC_PB_FC_REC_CAL_DEST_OFP_EGQ_HP;
      cal_buff[entry_idx].id = rx_cal.fc_index;
    }
    else if (rx_cal.fc_dest_sel == 0x2)
    {
      /* EGQ port */
      cal_buff[entry_idx].destination = SOC_PB_FC_REC_CAL_DEST_OFP_EGQ_LP;
      cal_buff[entry_idx].id = rx_cal.fc_index;
    }
    else if ((rx_cal.fc_dest_sel == SOC_PB_FC_OOB_CAL_EMPTY_SEL) && (rx_cal.fc_index == SOC_PB_FC_OOB_CAL_EMPTY_ID))
    {
      cal_buff[entry_idx].destination = SOC_PB_FC_REC_CAL_DEST_NONE;
      cal_buff[entry_idx].id = 0; /* Irrelevant */
    }
    else if (rx_cal.fc_dest_sel == 0x3)
    {
      /* NIF */
      cal_buff[entry_idx].destination = SOC_PB_FC_REC_CAL_DEST_NIF;
      cal_buff[entry_idx].id = rx_cal.fc_index;
    }
    else
    {
      /* Shouldn't get here - mis-configured calendar! */
      cal_buff[entry_idx].destination = rx_cal.fc_dest_sel;
      cal_buff[entry_idx].id = rx_cal.fc_index;
    }
  }
  /* Calendar entries } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_fc_rec_cal_get_unsafe()",0,0);
}

/*********************************************************************
*     Configures Calendar-based interface calendar for Flow
 *     Control Reception.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_oob_phy_params_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_PHY_PARAMS_INFO          *phy_params
   )
{
  uint32
    reg_val,
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_OOB_PHY_PARAMS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(phy_params);

  regs = soc_petra_regs();

  SOC_PB_REG_GET(regs->cfc.out_of_band_rx_configuration_reg[if_ndx], reg_val, 10, exit);
  
  fld_val = SOC_SAND_BOOL2NUM(phy_params->is_on_if_oof);
  SOC_PB_FLD_TO_REG(regs->cfc.out_of_band_rx_configuration_reg[if_ndx].oofrm_sts_sel, fld_val, reg_val, 20, exit);

  fld_val = SOC_SAND_BOOL2NUM(phy_params->is_sampled_rising_edge);
  SOC_PB_FLD_TO_REG(regs->cfc.out_of_band_rx_configuration_reg[if_ndx].in_stat_phase_sel , fld_val, reg_val, 30, exit);

  SOC_PB_REG_SET(regs->cfc.out_of_band_rx_configuration_reg[if_ndx], reg_val, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_fc_oob_phy_params_set_unsafe()",0,0);
}

uint32
  soc_pb_fc_oob_phy_params_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  SOC_PB_FC_PHY_PARAMS_INFO          *phy_params
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_OOB_PHY_PARAMS_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(if_ndx, SOC_PB_IF_NDX_MAX, SOC_PB_FC_CAL_IF_NDX_OUT_OF_RANGE_ERR, 20, exit);

  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_fc_oob_phy_params_set_verify()",0,0);
}

uint32
  soc_pb_fc_oob_phy_params_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_OOB_PHY_PARAMS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(if_ndx, SOC_PB_IF_NDX_MAX, SOC_PB_FC_CAL_IF_NDX_OUT_OF_RANGE_ERR, 20, exit);

  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_fc_oob_phy_params_get_verify()",0,0);
}

/*********************************************************************
*     Gets the configuration set by the "soc_pb_fc_oob_phy_params_set"
 *     API.
 *     Refer to "soc_pb_fc_oob_phy_params_set" API for details.
*********************************************************************/
uint32
  soc_pb_fc_oob_phy_params_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT  SOC_PB_FC_PHY_PARAMS_INFO         *phy_params
  )
{
  uint32
    reg_val,
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_OOB_PHY_PARAMS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(phy_params);

  regs = soc_petra_regs();

  SOC_PB_REG_GET(regs->cfc.out_of_band_rx_configuration_reg[if_ndx], reg_val, 10, exit);

  SOC_PB_FLD_FROM_REG(regs->cfc.out_of_band_rx_configuration_reg[if_ndx].oofrm_sts_sel, fld_val, reg_val, 20, exit);
  phy_params->is_on_if_oof = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->cfc.out_of_band_rx_configuration_reg[if_ndx].in_stat_phase_sel , fld_val, reg_val, 30, exit);
  phy_params->is_sampled_rising_edge = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_fc_oob_phy_params_get_unsafe()",0,0);
}

/*********************************************************************
*     Diagnostics - get the Out-Of-Band interface status
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_egr_rec_oob_stat_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx,
    SOC_SAND_OUT SOC_PB_FC_CAL_REC_STAT             *status
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    reg_val = 0,
    fld_val = 0;
  uint32
    obrx_lock_err = 1,
    obrx_out_of_frm = 1,
    obrx_dip2_alarm = 1,
    obrx_frm_err = 1,
    obrx_dip2_err = 1;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_EGR_REC_OOB_STAT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(status);

  SOC_PB_FC_CAL_REC_STAT_clear(status);

  SOC_PB_FLD_SET(regs->cfc.interrupt_reg.obrx_lock_err[if_ndx],   0x1, 2, exit);
  SOC_PB_FLD_SET(regs->cfc.interrupt_reg.obrx_out_of_frm[if_ndx], 0x1, 4, exit);
  SOC_PB_FLD_SET(regs->cfc.interrupt_reg.obrx_dip2_alarm[if_ndx], 0x1, 6, exit);
  SOC_PB_FLD_SET(regs->cfc.interrupt_reg.obrx_frm_err[if_ndx],    0x1, 8, exit);
  SOC_PB_FLD_SET(regs->cfc.interrupt_reg.obrx_dip2_err[if_ndx],   0x1, 10, exit);

  SOC_PB_FLD_GET(regs->cfc.interrupt_reg.obrx_lock_err[if_ndx],   obrx_lock_err,   12, exit);
  SOC_PB_FLD_GET(regs->cfc.interrupt_reg.obrx_out_of_frm[if_ndx], obrx_out_of_frm, 14, exit);
  SOC_PB_FLD_GET(regs->cfc.interrupt_reg.obrx_dip2_alarm[if_ndx], obrx_dip2_alarm, 16, exit);
  SOC_PB_FLD_GET(regs->cfc.interrupt_reg.obrx_frm_err[if_ndx],    obrx_frm_err,    18, exit);
  SOC_PB_FLD_GET(regs->cfc.interrupt_reg.obrx_dip2_err[if_ndx],   obrx_dip2_err,   20, exit);

  status->dip2_alarm       = SOC_SAND_NUM2BOOL(obrx_dip2_alarm);
  status->nof_dip2_errors  = SOC_SAND_NUM2BOOL(obrx_dip2_err);
  status->nof_frame_errors = SOC_SAND_NUM2BOOL(obrx_frm_err);
  status->not_locked       = SOC_SAND_NUM2BOOL(obrx_lock_err);
  status->out_of_frame     = SOC_SAND_NUM2BOOL(obrx_out_of_frm);

  if (
      (status->nof_frame_errors) ||
      (status->nof_dip2_errors)
     )
  {
    SOC_PB_REG_GET(regs->cfc.oob_rx_error_counter_reg[if_ndx], reg_val, 30, exit);

    if (status->nof_frame_errors)
    {
      SOC_PB_FLD_FROM_REG(regs->cfc.oob_rx_error_counter_reg[if_ndx].frm_err_cnt, fld_val, reg_val, 40, exit);
      status->nof_frame_errors = SOC_SAND_MAX(1, fld_val);
    }

    if (status->nof_dip2_errors)
    {
      SOC_PB_FLD_FROM_REG(regs->cfc.oob_rx_error_counter_reg[if_ndx].dip2_err_cnt, fld_val, reg_val, 50, exit);
      status->nof_dip2_errors = SOC_SAND_MAX(1, fld_val);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_fc_egr_rec_oob_stat_get_unsafe()",if_ndx,0);
}

uint32
  soc_pb_fc_egr_rec_oob_stat_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID                   if_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_EGR_REC_OOB_STAT_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(if_ndx, SOC_PB_OOB_NDX_MAX, SOC_PB_FC_OOB_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_egr_rec_oob_stat_get_verify()", if_ndx, 0);
}

/*********************************************************************
*     Defines if and how LLFC can be received/generated using
 *     Interlaken NIF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_ilkn_llfc_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION             direction_ndx,
    SOC_SAND_IN  SOC_PB_FC_ILKN_LLFC_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ilkn_bit_offset;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  SOC_PETRA_REG_FIELD
    *on_ch0,
    *every_16_chs,
    *multiple_use_bits_mask;
  uint32
    buffer;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_ILKN_LLFC_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  if (SOC_PETRA_IS_DIRECTION_REC(direction_ndx))
  {
    on_ch0                 = SOC_PB_REG_DB_ACC_REF(regs->nbi.fc_ilkn_reg.fc_ilkn_rx_llfc_on_ch0);
    every_16_chs           = SOC_PB_REG_DB_ACC_REF(regs->nbi.fc_ilkn_reg.fc_ilkn_rx_llfc_every_16_chs);
    multiple_use_bits_mask = ilkn_ndx == SOC_PB_NIF_ILKN_ID_A ?
                               SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn0_multiple_use_bits_reg.ilkn_rx0_multiple_use_bits_mask_to_llfc)
                                 : SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn1_multiple_use_bits_reg.ilkn_rx1_multiple_use_bits_mask_to_llfc);
  }
  else
  {
    on_ch0                 = SOC_PB_REG_DB_ACC_REF(regs->nbi.fc_ilkn_reg.fc_ilkn_tx_gen_llfc_on_ch0);
    every_16_chs           = SOC_PB_REG_DB_ACC_REF(regs->nbi.fc_ilkn_reg.fc_ilkn_tx_gen_llfc_every_16_chs);
    multiple_use_bits_mask = ilkn_ndx == SOC_PB_NIF_ILKN_ID_B ?
                               SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn0_multiple_use_bits_reg.ilkn_tx0_multiple_use_bits_mask_for_llfc)
                                 : SOC_PB_REG_DB_ACC_REF(regs->nbi.ilkn1_multiple_use_bits_reg.ilkn_tx1_multiple_use_bits_mask_for_llfc);
  }

  ilkn_bit_offset = SOC_PB_NIF_ID_OFFSET(ILKN, ilkn_ndx);
  
  SOC_PB_IMPLICIT_FLD_GET(*on_ch0, buffer, 10, exit);
  buffer &= SOC_SAND_RBIT(ilkn_bit_offset);
  buffer |= SOC_SAND_SET_FLD_IN_PLACE(
              info->cal_channel == SOC_TMC_FC_ILKN_CAL_LLFC_CH_0 ? 1 : 0,
              ilkn_bit_offset,
              SOC_SAND_BIT(ilkn_bit_offset)
            );
  SOC_PB_IMPLICIT_FLD_SET(*on_ch0, buffer, 20, exit);

  SOC_PB_IMPLICIT_FLD_GET(*every_16_chs, buffer, 30, exit);
  buffer &= SOC_SAND_RBIT(ilkn_bit_offset);
  buffer |= SOC_SAND_SET_FLD_IN_PLACE(
              info->cal_channel == SOC_TMC_FC_ILKN_CAL_LLFC_CH_16N ? 1 : 0,
              ilkn_bit_offset,
              SOC_SAND_BIT(ilkn_bit_offset)
            );
  SOC_PB_IMPLICIT_FLD_SET(*every_16_chs, buffer, 40, exit);
  
  SOC_PB_FLD_GET(regs->nbi.fc_ilkn_reg.fc_ilkn_mode, buffer, 50, exit);
  buffer &= SOC_SAND_RBIT(ilkn_bit_offset);
  buffer |= SOC_SAND_SET_FLD_IN_PLACE(
              info->cal_channel != SOC_TMC_FC_ILKN_CAL_LLFC_NONE ? 1 : 0,
              ilkn_bit_offset,
              SOC_SAND_BIT(ilkn_bit_offset)
            );
  SOC_PB_FLD_SET(regs->nbi.fc_ilkn_reg.fc_ilkn_mode, buffer, 60, exit);
  
  buffer = info->multi_use_mask;
  SOC_PB_IMPLICIT_FLD_SET(*multiple_use_bits_mask, buffer, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_ilkn_llfc_set_unsafe()", ilkn_ndx, 0);
}

uint32
  soc_pb_fc_ilkn_llfc_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION             direction_ndx,
    SOC_SAND_IN  SOC_PB_FC_ILKN_LLFC_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_ILKN_LLFC_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(ilkn_ndx, SOC_PB_ILKN_NDX_MIN, SOC_PB_ILKN_NDX_MAX, SOC_PB_ILKN_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_FC_ILKN_LLFC_INFO, info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_ilkn_llfc_set_verify()", ilkn_ndx, 0);
}

uint32
  soc_pb_fc_ilkn_llfc_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_ILKN_LLFC_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(ilkn_ndx, SOC_PB_ILKN_NDX_MIN, SOC_PB_ILKN_NDX_MAX, SOC_PB_ILKN_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_ilkn_llfc_get_verify()", ilkn_ndx, 0);
}

/*********************************************************************
*     Defines if and how LLFC can be received/generated using
 *     Interlaken NIF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_ilkn_llfc_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_OUT SOC_PB_FC_ILKN_LLFC_INFO           *rec_info,
    SOC_SAND_OUT SOC_PB_FC_ILKN_LLFC_INFO           *gen_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ilkn_bit_offset;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    buffer;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_ILKN_LLFC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rec_info);
  SOC_SAND_CHECK_NULL_INPUT(gen_info);

  SOC_PB_FC_ILKN_LLFC_INFO_clear(rec_info);
  SOC_PB_FC_ILKN_LLFC_INFO_clear(gen_info);

  ilkn_bit_offset = SOC_PB_NIF_ID_OFFSET(ILKN, ilkn_ndx);

  /*
   *  Determine Rx type
   */
  SOC_PB_FLD_GET(regs->nbi.fc_ilkn_reg.fc_ilkn_rx_llfc_on_ch0, buffer, 10, exit);
  if (SOC_SAND_GET_FLD_FROM_PLACE(buffer, ilkn_bit_offset, SOC_SAND_BIT(ilkn_bit_offset)) == 1)
  {
    rec_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_CH_0;
  }
  else
  {
    SOC_PB_FLD_GET(regs->nbi.fc_ilkn_reg.fc_ilkn_rx_llfc_every_16_chs, buffer, 20, exit);
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
  SOC_PB_FLD_GET(regs->nbi.fc_ilkn_reg.fc_ilkn_tx_gen_llfc_on_ch0, buffer, 30, exit);
  if (SOC_SAND_GET_FLD_FROM_PLACE(buffer, ilkn_bit_offset, SOC_SAND_BIT(ilkn_bit_offset)) == 1)
  {
    gen_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_CH_0;
  }
  else
  {
    SOC_PB_FLD_GET(regs->nbi.fc_ilkn_reg.fc_ilkn_tx_gen_llfc_every_16_chs, buffer, 40, exit);
    if (SOC_SAND_GET_FLD_FROM_PLACE(buffer, ilkn_bit_offset, SOC_SAND_BIT(ilkn_bit_offset)) == 1)
    {
      gen_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_CH_16N;
    }
    else
    {
      gen_info->cal_channel = SOC_TMC_FC_ILKN_CAL_LLFC_NONE;
    }
  }

  if (ilkn_ndx == SOC_PB_NIF_ILKN_ID_A)
  {
    SOC_PB_FLD_GET(regs->nbi.ilkn0_multiple_use_bits_reg.ilkn_rx0_multiple_use_bits_mask_to_llfc, buffer, 50, exit);
    rec_info->multi_use_mask = (uint8) buffer;
    SOC_PB_FLD_GET(regs->nbi.ilkn0_multiple_use_bits_reg.ilkn_tx0_multiple_use_bits_mask_for_llfc, buffer, 60, exit);
    gen_info->multi_use_mask = (uint8) buffer;
  }
  else
  {
    SOC_PB_FLD_GET(regs->nbi.ilkn1_multiple_use_bits_reg.ilkn_rx1_multiple_use_bits_mask_to_llfc, buffer, 50, exit);
    rec_info->multi_use_mask = (uint8) buffer;
    SOC_PB_FLD_GET(regs->nbi.ilkn1_multiple_use_bits_reg.ilkn_tx1_multiple_use_bits_mask_for_llfc, buffer, 60, exit);
    gen_info->multi_use_mask = (uint8) buffer;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_ilkn_llfc_get_unsafe()", ilkn_ndx, 0);
}

/*********************************************************************
*     Defines FC Oversubscription scheme on the NIFs. For up
 *     to 100GE traffic on the device NIFs should be set to
 *     'NONE'. Otherwise, should be set according to the
 *     oversubscription level. The scheme affects internal NIF
 *     FC thresholds each scheme defines a different thresholds
 *     preset.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_nif_oversubscr_scheme_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME    scheme
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_OVERSUBSCR_SCHEME_SET_UNSAFE);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_oversubscr_scheme_set_unsafe()", 0, 0);
}

uint32
  soc_pb_fc_nif_oversubscr_scheme_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME    scheme
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_OVERSUBSCR_SCHEME_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(scheme, SOC_PB_SCHEME_MAX, SOC_PB_FC_NIF_OVRS_SCHEME_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_oversubscr_scheme_set_verify()", 0, 0);
}

uint32
  soc_pb_fc_nif_oversubscr_scheme_get_verify(
    SOC_SAND_IN  int                      unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_OVERSUBSCR_SCHEME_GET_VERIFY);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_oversubscr_scheme_get_verify()", 0, 0);
}

/*********************************************************************
*     Defines FC Oversubscription scheme on the NIFs. For up
 *     to 100GE traffic on the device NIFs should be set to
 *     'NONE'. Otherwise, should be set according to the
 *     oversubscription level. The scheme affects internal NIF
 *     FC thresholds each scheme defines a different thresholds
 *     preset.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_nif_oversubscr_scheme_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_FC_NIF_OVERSUBSCR_SCHEME    *scheme
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_OVERSUBSCR_SCHEME_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(scheme);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_oversubscr_scheme_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_nif_pause_quanta_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  uint32                       pause_quanta
  )
{
  uint32
   res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    buffer;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_PAUSE_QUANTA_SET_UNSAFE);

  buffer = pause_quanta;
  SOC_PB_FLD_ISET(regs->nif_mac_lane.flow_control_tx_quanta_reg.fc_tx_pause_quanta_xoff, buffer, mal_ndx, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_pause_quanta_set_unsafe()", mal_ndx, 0);
}

uint32
  soc_pb_fc_nif_pause_quanta_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  uint32                       pause_quanta
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_PAUSE_QUANTA_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mal_ndx, SOC_PB_MAL_NDX_MAX, SOC_PB_MAL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(pause_quanta, SOC_PB_PAUSE_QUANTA_MAX, SOC_PB_FC_NIF_PAUSE_QUANTA_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_pause_quanta_set_verify()", mal_ndx, 0);
}

uint32
  soc_pb_fc_nif_pause_quanta_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_PAUSE_QUANTA_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mal_ndx, SOC_PB_MAL_NDX_MAX, SOC_PB_MAL_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_pause_quanta_get_verify()", mal_ndx, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_nif_pause_quanta_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT uint32                       *pause_quanta
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    buffer;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_PAUSE_QUANTA_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pause_quanta);

  SOC_PB_FLD_IGET(regs->nif_mac_lane.flow_control_tx_quanta_reg.fc_tx_pause_quanta_xoff, buffer, mal_ndx, 10, exit);
  *pause_quanta = buffer;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_pause_quanta_get_unsafe()", mal_ndx, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_nif_pause_frame_src_addr_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS            *mac_addr
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    mac_in_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_addr);

  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
          mac_addr,
          mac_in_longs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PB_REG_ISET(regs->nif_mac_lane.flow_control_source_addr_reg[0], mac_in_longs[0], mal_ndx, 10, exit);
  SOC_PB_REG_ISET(regs->nif_mac_lane.flow_control_source_addr_reg[1], mac_in_longs[1], mal_ndx, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_pause_frame_src_addr_set_unsafe()", mal_ndx, 0);
}

uint32
  soc_pb_fc_nif_pause_frame_src_addr_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS            *mac_addr
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mal_ndx, SOC_PB_MAL_NDX_MAX, SOC_PB_MAL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_pause_frame_src_addr_set_verify()", mal_ndx, 0);
}

uint32
  soc_pb_fc_nif_pause_frame_src_addr_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_PAUSE_FRAME_SRC_ADDR_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mal_ndx, SOC_PB_MAL_NDX_MAX, SOC_PB_MAL_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_pause_frame_src_addr_get_verify()", mal_ndx, 0);
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_nif_pause_frame_src_addr_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS            *mac_addr
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    mac_in_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
  SOC_PETRA_REGS
    *regs = soc_petra_regs();

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_NIF_PAUSE_FRAME_SRC_ADDR_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_addr);

  SOC_PB_REG_IGET(regs->nif_mac_lane.flow_control_source_addr_reg[0], mac_in_longs[0], mal_ndx, 10, exit);
  SOC_PB_REG_IGET(regs->nif_mac_lane.flow_control_source_addr_reg[1], mac_in_longs[1], mal_ndx, 10, exit);

  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_long_to_struct(
          mac_in_longs,
          mac_addr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_nif_pause_frame_src_addr_get_unsafe()", mal_ndx, 0);
}

/*********************************************************************
*     Enable/disable the mapping between the VSQ-Pointer and
 *     the Incoming-NIF port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_vsq_by_incoming_nif_set_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint8 enable
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    tm_port_ndx;
  SOC_PB_PMF_DIRECT_TBL_DATA
    dt_data;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    info_incoming,
    info_outgoing;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_VSQ_BY_INCOMING_NIF_SET_UNSAFE);

  /*
   * Set the SW DB
   */
  soc_pb_sw_db_is_vsq_nif_enabled_set(unit, enable);

  /*
   * Fill the Direct Table lines (if disable, remove them)
   * DirectTable[TM-Port] = NIF-Port
   */
  SOC_PB_PMF_DIRECT_TBL_DATA_clear(&dt_data);
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&info_incoming);
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&info_outgoing);
  for (tm_port_ndx = 0; tm_port_ndx < SOC_PETRA_NOF_LOCAL_PORTS; ++tm_port_ndx)
  {
    /*
     *  A. Port to interface
     */
    res = soc_pb_port_to_interface_map_get_unsafe(
            unit,
            tm_port_ndx,
            &info_incoming,
            &info_outgoing
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /*
     *  B. Internal offset
     */
    if (enable == TRUE)
    {
      dt_data.val = soc_pb_nif2intern_id(info_incoming.if_id);
    }
    else
    {
      dt_data.val = 0;
    }

    res = soc_pb_pmf_db_direct_tbl_entry_set_unsafe(
            unit,
            tm_port_ndx,
            &dt_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }


  /*
   * Update all the TM PMF-Programs
   */
  res = soc_pb_pmf_pgm_mgmt_update(
          unit,
          SOC_PB_PMF_PGM_LOOKUP_PROFILE_NDX_FOR_TM
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* 
   * Disable the Raw access to Direct Table 
   */
  res = soc_pb_pmf_low_level_raw_pgm_dt_disable(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_vsq_by_incoming_nif_set_unsafe()", 0, 0);
}

/*********************************************************************
*     Enable/disable the mapping between the VSQ-Pointer and
 *     the Incoming-NIF port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_fc_vsq_by_incoming_nif_get_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint8 *enable
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FC_VSQ_BY_INCOMING_NIF_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(enable);

  /*
   * Get from the SW DB
   */
  *enable = soc_pb_sw_db_is_vsq_nif_enabled_get(unit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fc_vsq_by_incoming_nif_get_unsafe()", 0, 0);
}

uint32
  SOC_PB_FC_GEN_INBND_CB_verify(
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_CB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->inherit, SOC_PB_FC_GEN_INBND_CB_INHERIT_MAX, SOC_PB_FC_INHERIT_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_FC_GEN_INBND_CB_verify()",0,0);
}

uint32
  SOC_PB_FC_GEN_INBND_LL_verify(
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_LL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_FC_GEN_INBND_LL_verify()",0,0);
}

uint32
  SOC_PB_FC_GEN_INBND_INFO_verify(
    SOC_SAND_IN  SOC_PB_FC_GEN_INBND_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mode, SOC_PB_FC_GEN_INBND_INFO_MODE_MAX, SOC_PB_FC_MODE_OUT_OF_RANGE_ERR, 10, exit);
  if (info->mode == SOC_PB_FC_INBND_MODE_CB)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_FC_GEN_INBND_CB, &(info->cb), 11, exit);
  }
  if (info->mode == SOC_PB_FC_INBND_MODE_LL)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_FC_GEN_INBND_LL, &(info->ll), 12, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_FC_GEN_INBND_INFO_verify()",0,0);
}

uint32
  SOC_PB_FC_REC_INBND_CB_verify(
    SOC_SAND_IN  SOC_PB_FC_REC_INBND_CB *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->inherit, SOC_PB_FC_REC_INBND_CB_INHERIT_MAX, SOC_PB_FC_INHERIT_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_FC_REC_INBND_CB_verify()",0,0);
}

uint32
  SOC_PB_FC_REC_INBND_INFO_verify(
    SOC_SAND_IN  SOC_PB_FC_REC_INBND_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mode, SOC_PB_FC_REC_INBND_INFO_MODE_MAX, SOC_PB_FC_MODE_OUT_OF_RANGE_ERR, 10, exit);

  if (info->mode == SOC_PB_FC_INBND_MODE_CB)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_FC_REC_INBND_CB, &(info->cb), 11, exit);
  }

  if (info->mode == SOC_PB_FC_INBND_MODE_LL)
  {
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_FC_REC_INBND_INFO_verify()",0,0);
}

uint32
  SOC_PB_FC_REC_OFP_MAP_INFO_verify(
    SOC_SAND_IN  SOC_PB_FC_REC_OFP_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->react_point, SOC_PB_FC_REC_OFP_MAP_INFO_REACT_POINT_MAX, SOC_PB_FC_REACT_POINT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ofp_ndx, SOC_PETRA_MAX_FAP_PORT_ID, SOC_PETRA_FAP_PORT_ID_INVALID_ERR, 14, exit);

  switch(info->priority)
  {
  case SOC_TMC_FC_OFP_PRIORITY_NONE:
  case SOC_TMC_FC_OFP_PRIORITY_LP:
  case SOC_TMC_FC_OFP_PRIORITY_HP:
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_FC_PRIO_OUT_OF_RANGE_ERR, 20, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_FC_REC_OFP_MAP_INFO_verify()",0,0);
}

uint32
  SOC_PB_FC_RCY_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_FC_RCY_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_FC_RCY_PORT_INFO_verify()",0,0);
}

uint32
  SOC_PB_FC_CAL_IF_INFO_verify(
    SOC_SAND_IN  SOC_PB_FC_CAL_IF_INFO *info,
    SOC_SAND_IN  SOC_PB_FC_CAL_MODE mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cal_len, SOC_PB_FC_CAL_IF_INFO_CAL_LEN_MAX, SOC_PB_FC_CAL_LEN_OUT_OF_RANGE_ERR, 11, exit);
  if(!SOC_PB_FC_CAL_MODE_IS_ILKN(mode))
  {
    SOC_SAND_ERR_IF_BELOW_MIN(info->cal_reps, SOC_PB_FC_CAL_IF_INFO_CAL_REPS_MIN, SOC_PB_FC_CAL_REPS_OUT_OF_RANGE_ERR, 12, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cal_reps, SOC_PB_FC_CAL_IF_INFO_CAL_REPS_MAX, SOC_PB_FC_CAL_REPS_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_FC_CAL_IF_INFO_verify()",0,0);
}

uint32
  SOC_PB_FC_CAL_REC_STAT_verify(
    SOC_SAND_IN  SOC_PB_FC_CAL_REC_STAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_FC_CAL_REC_STAT_verify()",0,0);
}

uint32
  SOC_PB_FC_GEN_CALENDAR_verify(
    SOC_SAND_IN  SOC_PB_FC_GEN_CALENDAR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->source, SOC_PB_FC_GEN_CALENDAR_SOURCE_MAX, SOC_PB_FC_SOURCE_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_FC_GEN_CALENDAR_verify()",0,0);
}

uint32
  SOC_PB_FC_REC_CALENDAR_verify(
    SOC_SAND_IN  SOC_PB_FC_REC_CALENDAR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->destination, SOC_PB_FC_REC_CALENDAR_DESTINATION_MAX, SOC_PB_FC_DESTINATION_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_FC_REC_CALENDAR_verify()",0,0);
}

uint32
  SOC_PB_FC_ILKN_LLFC_INFO_verify(
    SOC_SAND_IN  SOC_PB_FC_ILKN_LLFC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cal_channel, SOC_PB_FC_ILKN_LLFC_INFO_CAL_CHANNEL_MAX, SOC_PB_FC_CAL_CHANNEL_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_FC_ILKN_LLFC_INFO_verify()",0,0);
}

uint32
soc_pb_fc_gen_cal_src_type_to_val_internal(
    SOC_SAND_IN SOC_TMC_FC_GEN_CAL_SRC src_type
  )
{
  uint32
    ret;

  for(ret = 0; ret < SOC_PB_FC_GEN_CAL_SRC_ARR_SIZE; ret++)
  {
    if(soc_pb_fc_gen_cal_src_arr[ret] == src_type)
    {
      return ret;
    }
  }

  return ret;
}

#if SOC_PB_DEBUG_IS_LVL1
#endif /* SOC_PB_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

