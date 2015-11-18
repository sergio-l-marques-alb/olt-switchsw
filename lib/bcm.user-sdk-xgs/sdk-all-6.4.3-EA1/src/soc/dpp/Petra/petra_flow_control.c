/* $Id: petra_flow_control.c,v 1.7 Broadcom SDK $
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

#include <soc/dpp/Petra/petra_flow_control.h>
#include <soc/dpp/Petra/petra_nif.h>
#include <soc/dpp/Petra/petra_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: petra_flow_control.c,v 1.7 Broadcom SDK $
 *  Marks unmodified (not-mapped) entry in the scheduler-based to OFP
 *  mapping table.
 */
#define SOC_PETRA_FC_REC_OOB_SCH_BASED2OFP_INIT_VAL     (127)

/*
 *  Number of OFP HRs
 */
#define SOC_PETRA_FC_NOF_OFP_HRS                        (SOC_PETRA_NOF_FAP_PORTS)

/*
 *  Marks unmodified (not-mapped) entry of the mapping table
 */
#define SOC_PETRA_FC_OFP_HR2ENTRY_INIT_VAL              (128)

/*
 *  Scheduler-based flow control to ofp mapping length
 */
#define SOC_PETRA_FC_SCH_BASED_OFP_MAPPING_TABLE_LENGTH (128)


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

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
* NAME:
*     soc_petra_flow_control_regs_init
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
  soc_petra_flow_control_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    oob_idx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FLOW_CONTROL_REGS_INIT);

  regs = soc_petra_regs();

  /* Enable Soc_petra device-level flow control */
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.cfc_en, 0x1, 10, exit);

  /* CFC Enablers */
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.egq_device_en, 0x1, 20, exit);
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.egq_ofp_lp_en, 0x1, 22, exit);
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.egq_ofp_hp_en, 0x1, 24, exit);
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.egq_ch_port_en, 0x1, 26, exit);
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.egq_erp_en, 0x1, 28, exit);
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.cpu_port_en, 0x1, 30, exit);
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.rcl_port_en, 0x1, 32, exit);
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.oob_sch_en, 0x1, 34, exit);
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.oob_egq_ofp_en, 0x1, 35, exit);
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.oob_intrfb_sel, 0x1, 36, exit);
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.oob_nifa_fast_ll_en, 0x1, 37, exit);
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.oob_nifb_fast_ll_en, 0x1, 38, exit);

  /* OOB-RX */
  for (oob_idx = 0; oob_idx < SOC_PETRA_FC_NOF_OOB_IFS; oob_idx++)
  {
    SOC_PA_FLD_ISET(regs->cfc.out_of_band_rx_configuration0_reg.oofrm_sts_sel, 0x0, oob_idx, 50, exit);
    SOC_PA_FLD_ISET(regs->cfc.out_of_band_rx_configuration0_reg.rx_async_rstn, 0x1, oob_idx, 51, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_flow_control_regs_init()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_flow_control_init
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
  soc_petra_flow_control_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FLOW_CONTROL_INIT);

  res = soc_petra_flow_control_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_flow_control_init()",0,0);
}

/*********************************************************************
*     Configure Flow Control generation, based on Ingress
*     Statistics Queues, via NIF.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_vsq_via_nif_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info
  )
{
  uint32
    val = 0,
    fld_val = 0,
    res;
  uint32
    mal_id = 0,
    lane_id = 0;
  SOC_PETRA_NIF_TYPE
    nif_type;
  uint8
    is_channelized = FALSE;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_VSQ_VIA_NIF_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  mal_id = SOC_PETRA_NIF2MAL_NDX(nif_ndx);
  lane_id = SOC_PETRA_NIF_SGMII_LANE(nif_ndx);

  /*
   * Get NIF Port type
   */
  res = soc_petra_nif_type_get_unsafe(
          unit,
          mal_id,
          &nif_type,
          &is_channelized
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if ((lane_id != 0) && (nif_type != SOC_PETRA_NIF_TYPE_SGMII))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INCOMPATIBLE_TYPE_ERR, 20, exit);
  }

  /*
   *  Set Link Level Flow Control Configuration
   */
  SOC_PA_FLD_IGET(regs->nif_mac_lane.tx_flow_control_config_reg.tx_llfc_on_cfc_req, fld_val, mal_id, 30, exit);

  val = ((info->ll_enable_state == SOC_PETRA_FC_INGR_GEN_VSQ_NIF_LL_STATE_DISABLE)?0x0:0x1);
  if (nif_type == SOC_PETRA_NIF_TYPE_SGMII)
  {
    SOC_SAND_SET_BIT(fld_val, val, lane_id);
  }
  else
  {
    fld_val = val;
  }

  SOC_PA_FLD_ISET(regs->nif_mac_lane.tx_flow_control_config_reg.tx_llfc_on_cfc_req, fld_val, mal_id, 40, exit);

  /*
   *  Set Class Based Flow Control Configuration
   */

  fld_val = (info->cb_enable_state == SOC_PETRA_FC_INGR_GEN_VSQ_NIF_CB_STATE_ENABLE_CC)?0x0:0x1;
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.nif_clb_sel, fld_val, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_vsq_via_nif_set_unsafe()",0,0);
}

/*********************************************************************
*     Configure Flow Control generation, based on Ingress
*     Statistics Queues, via NIF.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_vsq_via_nif_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_VSQ_VIA_NIF_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_interface_id_verify(unit, nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->ll_enable_state, SOC_PETRA_FC_INGR_NOF_GEN_NIF_LL_STATES-1,
    SOC_PETRA_FC_INGR_NOF_GEN_NIF_LL_OUT_OF_RANGE_ERR, 20, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->cb_enable_state, SOC_PETRA_FC_INGR_NOF_GEN_NIF_CB_STATES-1,
    SOC_PETRA_FC_INGR_NOF_GEN_NIF_CB_OUT_OF_RANGE_ERR, 30, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_vsq_via_nif_verify()",0,0);
}

/*********************************************************************
*     Configure Flow Control generation, based on Ingress
*     Statistics Queues, via NIF.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_vsq_via_nif_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_VSQ_NIF_INFO *info
  )
{
  uint32
    val = 0,
    fld_val = 0,
    res;
  uint32
    mal_id = 0,
    lane_id = 0;
  SOC_PETRA_NIF_TYPE
    nif_type;
  uint8
    is_channelized = FALSE;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_VSQ_VIA_NIF_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_interface_id_verify(unit, nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  regs = soc_petra_regs();

  mal_id = SOC_PETRA_NIF2MAL_NDX(nif_ndx);
  lane_id = SOC_PETRA_NIF_SGMII_LANE(nif_ndx);

 /*
  * Get NIF Port type
  */
  res = soc_petra_nif_type_get_unsafe(
          unit,
          mal_id,
          &nif_type,
          &is_channelized
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if ((lane_id != 0) && (nif_type != SOC_PETRA_NIF_TYPE_SGMII))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INCOMPATIBLE_TYPE_ERR, 20, exit);
  }

  /*
  *  Get Link Level Flow Control Configuration
  */
  SOC_PA_FLD_IGET(regs->nif_mac_lane.tx_flow_control_config_reg.tx_llfc_on_cfc_req, fld_val, mal_id, 30, exit);

  if (nif_type == SOC_PETRA_NIF_TYPE_SGMII)
  {
    val = SOC_SAND_GET_BIT(fld_val, lane_id);
  }
  else
  {
    /*
     *  bit 0 is used for non-SGMII NIF Ports
     */
    val = SOC_SAND_GET_BIT(fld_val, 0);
  }
  info->ll_enable_state =
    (val == 0x0)? SOC_PETRA_FC_INGR_GEN_VSQ_NIF_LL_STATE_DISABLE:SOC_PETRA_FC_INGR_GEN_VSQ_NIF_LL_STATE_ENABLE;

  /*
   *  Get Class Based Flow Control Configuration
   */
  SOC_PA_FLD_IGET(regs->nif_mac_lane.tx_flow_control_config2_reg.tx_cbfc, fld_val, mal_id, 50, exit);

  SOC_PA_FLD_GET(regs->cfc.cfc_enablers_reg.nif_clb_sel, fld_val, 70, exit);

  info->cb_enable_state =
    (fld_val == 0x0)?SOC_PETRA_FC_INGR_GEN_VSQ_NIF_CB_STATE_ENABLE_CC:SOC_PETRA_FC_INGR_GEN_VSQ_NIF_CB_STATE_ENABLE_STF;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_vsq_via_nif_get_unsafe()",0,0);
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
  soc_petra_fc_ingr_gen_glb_hp_via_nif_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE fc_mode
  )
{
  uint32
    ll_fld_val = 0,
    cb_fld_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_GLB_HP_VIA_NIF_SET_UNSAFE);

  regs = soc_petra_regs();

  ll_fld_val = (fc_mode == SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE_LL);
  cb_fld_val = (fc_mode == SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE_CB);

  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.frc_llvl_en, ll_fld_val, 10, exit);
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.hp_glb_cls_ovrd_en, cb_fld_val, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_glb_hp_via_nif_set_unsafe()",0,0);
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
  soc_petra_fc_ingr_gen_glb_hp_via_nif_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE fc_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_GLB_HP_VIA_NIF_VERIFY);

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_glb_hp_via_nif_verify()",0,0);
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
  soc_petra_fc_ingr_gen_glb_hp_via_nif_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE *fc_mode
  )
{
  uint32
    ll_fld_val = 0,
    cb_fld_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_GLB_HP_VIA_NIF_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fc_mode);
  regs = soc_petra_regs();

  SOC_PA_FLD_GET(regs->cfc.cfc_enablers_reg.frc_llvl_en, ll_fld_val, 10, exit);
  SOC_PA_FLD_GET(regs->cfc.cfc_enablers_reg.hp_glb_cls_ovrd_en, cb_fld_val, 20, exit);

  if (ll_fld_val && cb_fld_val)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FC_INGR_GEN_GLB_HP_INVALID_ERR, 30, exit);
  }

  if (ll_fld_val)
  {
    *fc_mode = SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE_LL;
  }
  else if (cb_fld_val)
  {
    *fc_mode = SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE_CB;
  }
  else
  {
    *fc_mode = SOC_PETRA_FC_INGR_GEN_GLB_HP_MODE_NONE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_glb_hp_via_nif_get_unsafe()",0,0);
}

/*********************************************************************
*     Configure Flow Control generation, based on Ingress
*     Global Resources - low priority, via NIF, Class Based.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  uint32                 cls_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info
  )
{
  uint32
    fld_val = 0,
    reg_val = 0,
    lowest_class = 0,   /* Lowest Class for which FC will be generated */
    highest_class = 0,  /* Highest Class for which FC will be generated */
    res;
  uint32
    mal_id = 0,
    lane_id = 0,
    nif_id = nif_ndx,
    nof_classes = 0,
    curr_fcc_idx = 0,  /* Flow Control Class iterator */
    reg_idx = 0,
    bit_idx = 0;
  SOC_PETRA_NIF_TYPE
    nif_type;
  uint8
    is_channelized = FALSE;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  mal_id = SOC_PETRA_NIF2MAL_NDX(nif_ndx);
  lane_id = SOC_PETRA_NIF_SGMII_LANE(nif_ndx);

 /*
  * Get NIF Port type
  */
  res = soc_petra_nif_type_get_unsafe(
          unit,
          mal_id,
          &nif_type,
          &is_channelized
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if ((lane_id != 0) && (nif_type != SOC_PETRA_NIF_TYPE_SGMII))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INCOMPATIBLE_TYPE_ERR, 20, exit);
  }

  if (nif_type == SOC_PETRA_NIF_TYPE_SGMII)
  {
    if(info->class_and_below == FALSE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FC_NIF_CB_SGMII_INVALID_CONF_ERR, 30, exit);
    }

    fld_val = cls_ndx;
    SOC_PA_FLD_ISET(regs->nif_mac_lane.tx_flow_control_config2_reg.tx_class_table_l[lane_id], fld_val, mal_id, 40, exit);

    /*
     *  For SGMII - only 2 entries control the Class Based Flow Control.
     *  The effected classes for the lower entry
     *  are defined by the tx_class_table configuration.
     *  The higher entry is only relevant for generating flow control on
     *  all classes.
     */
    lowest_class = 0x0;
    highest_class = 0x0;
    nif_id = nif_ndx;
    nof_classes = SOC_PETRA_NIF_NOF_FC_CLASSES_SGMII;
  }
  else
  {
    highest_class = cls_ndx;
    if(info->class_and_below == FALSE)
    {
      lowest_class = cls_ndx;
    }
    else
    {
      lowest_class = 0x0;
    }
    nif_id = mal_id;
    nof_classes = SOC_PETRA_NIF_NOF_FC_CLASSES;
  }

  /*
   *  Get the offset of the register to set - all FC-s for a single NIF
   *  belong to the same register.
   */
  reg_idx = SOC_PETRA_REG_IDX_GET((nif_id * nof_classes), SOC_SAND_REG_SIZE_BITS);

  SOC_PA_REG_GET(regs->cfc.low_priority_control_reg[reg_idx], reg_val, 50, exit);
  fld_val = SOC_SAND_BOOL2NUM(info->enable);
  for (curr_fcc_idx = lowest_class; curr_fcc_idx <= highest_class; curr_fcc_idx++)
  {
    bit_idx = nif_id * nof_classes + curr_fcc_idx;
    bit_idx = SOC_PETRA_FLD_IDX_GET(bit_idx, SOC_SAND_REG_SIZE_BITS);
    SOC_SAND_SET_BIT(reg_val, fld_val, bit_idx);
  }

  SOC_PA_REG_SET(regs->cfc.low_priority_control_reg[reg_idx], reg_val, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_set_unsafe()",0,0);
}

/*********************************************************************
*     Configure Flow Control generation, based on Ingress
*     Global Resources - low priority, via NIF, Class Based.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  uint32                 cls_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_interface_id_verify(unit, nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    cls_ndx, SOC_PETRA_NIF_NOF_FC_CLASSES-1,
    SOC_PETRA_FC_NIF_CB_CLASSES_OUT_OF_RANGE_ERR , 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_verify()",0,0);
}

/*********************************************************************
*     Configure Flow Control generation, based on Ingress
*     Global Resources - low priority, via NIF, Class Based.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  uint32                 cls_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_INFO *info
  )
{
  uint32
    fld_val = 0,
    reg_val = 0,
    lowest_class = 0,   /* Lowest Class for which FC will be generated */
    highest_class = 0,  /* Highest Class for which FC will be generated */

    res;
  uint32
    mal_id = 0,
    lane_id = 0,
    nif_id = nif_ndx,
    nof_classes = 0,
    curr_fcc_idx = 0,  /* Flow Control Class iterator */
    reg_idx = 0,
    bit_idx = 0,
    sgmii_highest_class;
  SOC_PETRA_NIF_TYPE
    nif_type;
  uint8
    is_channelized = FALSE,
    all_enabled = TRUE,
    all_disabled = TRUE;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_interface_id_verify(unit, nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    cls_ndx, SOC_PETRA_NIF_NOF_FC_CLASSES-1,
    SOC_PETRA_FC_NIF_CB_CLASSES_OUT_OF_RANGE_ERR , 20, exit
  );

  regs = soc_petra_regs();

  mal_id = SOC_PETRA_NIF2MAL_NDX(nif_ndx);
  lane_id = SOC_PETRA_NIF_SGMII_LANE(nif_ndx);

 /*
  * Get NIF Port type
  */
  res = soc_petra_nif_type_get_unsafe(
          unit,
          mal_id,
          &nif_type,
          &is_channelized
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  if ((lane_id != 0) && (nif_type != SOC_PETRA_NIF_TYPE_SGMII))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INCOMPATIBLE_TYPE_ERR, 30, exit);
  }

  if (nif_type == SOC_PETRA_NIF_TYPE_SGMII)
  {
    SOC_PA_FLD_IGET(regs->nif_mac_lane.tx_flow_control_config2_reg.tx_class_table_l[lane_id], fld_val, mal_id, 40, exit);
    sgmii_highest_class = fld_val;

    /*
     *  For SGMII - only 2 entries control the Class Based Flow Control.
     *  The effected classes for the lower entry
     *  are defined by the tx_class_table configuration.
     *  The higher entry is only relevant for generating flow control on
     *  all classes.
     */
    lowest_class = 0x0;
    highest_class = 0x0;
    nif_id = nif_ndx;
    nof_classes = SOC_PETRA_NIF_NOF_FC_CLASSES_SGMII;
  }
  else
  {
    highest_class = cls_ndx;
    lowest_class = 0x0;
    nif_id = mal_id;
    nof_classes = SOC_PETRA_NIF_NOF_FC_CLASSES;
    sgmii_highest_class = 0; /* N/A */
  }

  /*
   *  Get the offset of the register to set - all FC-s for a single NIF
   *  belong to the same register.
   */
  reg_idx = SOC_PETRA_REG_IDX_GET((nif_id * nof_classes), SOC_SAND_REG_SIZE_BITS);

  SOC_PA_REG_GET(regs->cfc.low_priority_control_reg[reg_idx], reg_val, 50, exit);

  /*
   *  Check all classes for current NIF, up to the requested class
   */
  for (curr_fcc_idx = lowest_class; curr_fcc_idx <= highest_class; curr_fcc_idx++)
  {
    bit_idx = nif_id * nof_classes + curr_fcc_idx;
    bit_idx = SOC_PETRA_FLD_IDX_GET(bit_idx, SOC_SAND_REG_SIZE_BITS);
    fld_val = SOC_SAND_GET_BIT(reg_val, bit_idx);
    if (fld_val == 0x0)
    {
      /*
       *  At least one disabled found
       */
      all_enabled = FALSE;
    }
    else
    {
      /*
       *  At least one enabled found
       */
      all_disabled = FALSE;
    }
  }

  if (nif_type == SOC_PETRA_NIF_TYPE_SGMII)
  {
    info->enable = SOC_SAND_NUM2BOOL((fld_val == 0x1) && (cls_ndx <= sgmii_highest_class));
    info->class_and_below = TRUE;
  }
  else
  {
    if (fld_val == 0x0)
    {
      info->enable = FALSE;
      info->class_and_below = SOC_SAND_NUM2BOOL(all_disabled);
    }
    else
    {
      info->enable = TRUE;
      info->class_and_below = SOC_SAND_NUM2BOOL(all_enabled);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_glb_lp_via_nif_cb_get_unsafe()",0,0);
}

/*********************************************************************
*     Configures Out Of Band interface calendar for Flow
*     Control Generation (OOB TX).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_oob_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_IN  SOC_PETRA_FC_GEN_OOB_CAL_INFO *cal_buff
  )
{
  uint32
    fld_val = 0,
    res;
  uint32
    rep_idx = 0,      /* Calendar repetition index */
    per_rep_idx = 0,  /* Calendar entry index, per repetition */
    entry_idx = 0;    /* Calendar entry index, global */
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_DATA
    oob_tx_cal;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_OOB_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(oob_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  regs = soc_petra_regs();

  /* Calendar parameters { */
  /*
   *  Only interface B can be set for FC Generation
   */
  fld_val = 0x1;
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.oob_intrfb_sel, fld_val, 10, exit);

  /*
   *  Enable/disable
   */
  fld_val = SOC_SAND_BOOL2NUM(oob_conf->enable);
  SOC_PA_FLD_SET(regs->cfc.out_of_band_tx_configuration0_reg.oob_tx_en, fld_val, 20, exit);

  /*
   *  Calendar length (single repetition)
   */
  fld_val = oob_conf->cal_len;
  SOC_PA_FLD_SET(regs->cfc.out_of_band_tx_configuration0_reg.tx_cal_len, fld_val, 30, exit);

  /*
   *  Calendar number of repetitions
   */
  fld_val = oob_conf->cal_reps;
  SOC_PA_FLD_SET(regs->cfc.out_of_band_tx_configuration0_reg.tx_cal_m, fld_val, 40, exit);
  /* Calendar parameters } */

  /* Calendar entries { */
  for (rep_idx = 0; rep_idx < oob_conf->cal_reps; rep_idx++)
  {
    for (per_rep_idx = 0; per_rep_idx < oob_conf->cal_len; per_rep_idx++)
    {
      entry_idx = rep_idx * oob_conf->cal_len + per_rep_idx;

      if (cal_buff[per_rep_idx].source == SOC_PETRA_FC_GEN_OOB_SRC_NONE)
      {
        oob_tx_cal.fc_source_sel = SOC_PETRA_FC_OOB_CAL_EMPTY_SEL;
        oob_tx_cal.fc_index = SOC_PETRA_FC_OOB_CAL_EMPTY_ID;
      }
      else
      {
        oob_tx_cal.fc_source_sel = cal_buff[per_rep_idx].source;
        oob_tx_cal.fc_index = cal_buff[per_rep_idx].id;
      }

      res = soc_petra_cfc_out_of_band_tx_calendar_mapping_tbl_set_unsafe(
              unit,
              entry_idx,
              &oob_tx_cal
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  }
  /* Calendar entries } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_oob_set_unsafe()",0,0);
}

/*********************************************************************
*     Configures Out Of Band interface calendar for Flow
*     Control Generation (OOB TX).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_oob_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_IN  SOC_PETRA_FC_GEN_OOB_CAL_INFO *cal_buff
  )
{
  uint32
    entry_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_OOB_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(oob_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  SOC_SAND_MAGIC_NUM_VERIFY(oob_conf);
  SOC_SAND_MAGIC_NUM_VERIFY(cal_buff);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    oob_conf->cal_reps, SOC_PETRA_FC_OOB_CAL_REP_MIN, SOC_PETRA_FC_OOB_CAL_REP_MAX,
    SOC_PETRA_FC_OOB_CAL_REP_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    oob_conf->cal_len * oob_conf->cal_reps, SOC_PETRA_FC_OOB_CAL_LEN_MAX,
    SOC_PETRA_FC_OOB_CAL_LEN_OF_RANGE_ERR, 20, exit
  );

  for (entry_idx = 0; entry_idx < oob_conf->cal_len; entry_idx++)
  {
    switch(cal_buff[entry_idx].source) {
    case SOC_PETRA_FC_GEN_OOB_SRC_STE:
      SOC_SAND_ERR_IF_ABOVE_MAX(
        cal_buff[entry_idx].id, SOC_PETRA_ITM_NOF_VSQS-1,
        SOC_PETRA_FC_OOB_CAL_SRC_ID_OF_RANGE_ERR, 30, exit
      );
      break;
    case SOC_PETRA_FC_GEN_OOB_SRC_NIF:
      SOC_SAND_ERR_IF_ABOVE_MAX(
        cal_buff[entry_idx].id, SOC_PETRA_IF_NOF_NIFS-1,
        SOC_PETRA_FC_OOB_CAL_SRC_ID_OF_RANGE_ERR, 40, exit
      );
      break;
    case SOC_PETRA_FC_GEN_OOB_SRC_GLB_HP:
    case SOC_PETRA_FC_GEN_OOB_SRC_GLB_LP:
      SOC_SAND_ERR_IF_ABOVE_MAX(
        cal_buff[entry_idx].id, SOC_PETRA_FC_NOF_OOB_GLB_RCS_IDS-1,
        SOC_PETRA_FC_OOB_CAL_SRC_ID_OF_RANGE_ERR, 50, exit
      );
      break;
    case SOC_PETRA_FC_GEN_OOB_SRC_NONE:
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FC_OOB_CAL_SRC_TYPE_INVALID_ERR, 30, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_oob_verify()",0,0);
}

/*********************************************************************
*     Configures Out Of Band interface calendar for Flow
*     Control Generation (OOB TX).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_ingr_gen_oob_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_OUT SOC_PETRA_FC_GEN_OOB_CAL_INFO *cal_buff
  )
{
  uint32
    fld_val = 0,
    res;
  uint32
    entry_idx = 0;    /* Calendar entry index, global */
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_DATA
    oob_tx_cal;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_INGR_GEN_OOB_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(oob_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  regs = soc_petra_regs();

  /* Calendar parameters { */

  /*
   *  Enable/disable
   */
  SOC_PA_FLD_GET(regs->cfc.out_of_band_tx_configuration0_reg.oob_tx_en, fld_val, 20, exit);
  oob_conf->enable = SOC_SAND_NUM2BOOL(fld_val);

  /*
   *  Calendar length (single repetition)
   */
  SOC_PA_FLD_GET(regs->cfc.out_of_band_tx_configuration0_reg.tx_cal_len, fld_val, 30, exit);
  oob_conf->cal_len = fld_val;

  /*
   *  Calendar number of repetitions
   */
  SOC_PA_FLD_GET(regs->cfc.out_of_band_tx_configuration0_reg.tx_cal_m, fld_val, 40, exit);
  oob_conf->cal_reps = fld_val;
  /* Calendar parameters } */

  /* Calendar entries { */

  for (entry_idx = 0; entry_idx < oob_conf->cal_len; entry_idx++)
  {
    res = soc_petra_cfc_out_of_band_tx_calendar_mapping_tbl_get_unsafe(
            unit,
            entry_idx,
            &oob_tx_cal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    if ((oob_tx_cal.fc_source_sel == SOC_PETRA_FC_OOB_CAL_EMPTY_SEL) &&
        (oob_tx_cal.fc_index == SOC_PETRA_FC_OOB_CAL_EMPTY_ID))
    {
      cal_buff[entry_idx].source = SOC_PETRA_FC_GEN_OOB_SRC_NONE;
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_ingr_gen_oob_get_unsafe()",0,0);
}

/*********************************************************************
*     Configures the oob rx calendar mapping table and the
*     scheduler-based fc to ofp mapping table during the setting for
*     the oob interface calendar.
*********************************************************************/
STATIC
 uint32
  soc_petra_fc_egr_rec_oob_tables_write(
    SOC_SAND_IN  int                                                        unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_ID                                                  oob_ndx,
    SOC_SAND_IN  uint32                                                        oob_rx_cal_entry_idx,
    SOC_SAND_IN  SOC_PETRA_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA              *oob_rx_cal,
    SOC_SAND_IN  SOC_PETRA_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA *oob_hr_map,
    SOC_SAND_IN  uint8                                                        is_sch_ofp_hr
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_EGR_REC_OOB_TABLES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(oob_rx_cal);

  if (is_sch_ofp_hr)
  {
    SOC_SAND_CHECK_NULL_INPUT(oob_hr_map);
  }

  if (oob_ndx == SOC_PETRA_FC_OOB_ID_A)
  {
    res = soc_petra_cfc_out_of_band_rx_a_calendar_mapping_tbl_set_unsafe(
            unit,
            oob_rx_cal_entry_idx,
            oob_rx_cal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (is_sch_ofp_hr)
    {
      res = soc_petra_cfc_a_schedulers_based_flow_control_to_ofp_mapping_tbl_set_unsafe(
              unit,
              oob_rx_cal->fc_index,
              oob_hr_map
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }
  else
  {
    res = soc_petra_cfc_out_of_band_rx_b_calendar_mapping_tbl_set_unsafe(
            unit,
            oob_rx_cal_entry_idx,
            oob_rx_cal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    if (is_sch_ofp_hr)
    {
      res = soc_petra_cfc_b_schedulers_based_flow_control_to_ofp_mapping_tbl_set_unsafe(
              unit,
              oob_rx_cal->fc_index,
              oob_hr_map
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_egr_rec_oob_tables_write()",0,0);
}

/*********************************************************************
*     Configures Out Of Band interface calendar for Flow
*     Control Reception (OOB RX).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_egr_rec_oob_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_IN  SOC_PETRA_FC_REC_OOB_CAL_INFO *cal_buff
  )
{
  uint32
    fld_val = 0,
    reg_val = 0,
    ofp_mapping_indx,
    nof_lines,
    res;
  uint32
    rep_idx = 0,      /* Calendar repetition index */
    per_rep_idx = 0,  /* Calendar entry index, per repetition */
    entry_idx = 0,    /* Calendar entry index, global */
    reg_idx,
    idx,
    first_entry = 0;
  uint8
    is_sch_ofp_hr = FALSE,
    must_stop=FALSE,
    table_entries_to_existing_general_hr_mapping[SOC_PETRA_FC_SCH_BASED_OFP_MAPPING_TABLE_LENGTH];
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA
    oob_rx_cal;
  SOC_PETRA_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA
    oob_hr_map;
  SOC_PETRA_TBLS
    *tables;
  uint8
    current_entry_for_ofp_hr = 0,
    curr_entry_for_ofp_hr_idx,
    ofp_hr_to_table_entry_mapping[SOC_PETRA_FC_NOF_OFP_HRS];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_EGR_REC_OOB_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(oob_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  regs = soc_petra_regs();

  res = soc_petra_tbls_get(&tables);
  SOC_SAND_CHECK_FUNC_RESULT(res, 9, exit);

  res = SOC_SAND_OK; sal_memset(table_entries_to_existing_general_hr_mapping, 0x0,
          SOC_PETRA_FC_SCH_BASED_OFP_MAPPING_TABLE_LENGTH * sizeof(uint8));
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  oob_rx_cal.fc_dest_sel = 0;
  oob_rx_cal.fc_index = 0;

  oob_hr_map.ofp_hr =
    SOC_PETRA_FC_REC_OOB_SCH_BASED2OFP_INIT_VAL;
  oob_hr_map.lp_ofp_valid = 0;
  oob_hr_map.hp_ofp_valid = 0;

  /*
   *  Initialization of the tables
   */
  if (oob_ndx == SOC_PETRA_FC_OOB_ID_A)
  {
    nof_lines = SOC_PA_TBL(tables->cfc.out_of_band_rx_a_calendar_mapping_tbl.addr.size);

    res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

    res = soc_petra_cfc_out_of_band_rx_a_calendar_mapping_tbl_set_unsafe(
            unit,
            first_entry,
            &oob_rx_cal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    nof_lines = SOC_PA_TBL(tables->cfc.a_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.size);

    res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
    SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

    res = soc_petra_cfc_a_schedulers_based_flow_control_to_ofp_mapping_tbl_set_unsafe(
            unit,
            first_entry,
            &oob_hr_map
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
  }
  else
  {
    nof_lines = SOC_PA_TBL(tables->cfc.out_of_band_rx_b_calendar_mapping_tbl.addr.size);

    res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    res = soc_petra_cfc_out_of_band_rx_b_calendar_mapping_tbl_set_unsafe(
            unit,
            first_entry,
            &oob_rx_cal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

    nof_lines = SOC_PA_TBL(tables->cfc.b_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.size);

    res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
    SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

    res = soc_petra_cfc_b_schedulers_based_flow_control_to_ofp_mapping_tbl_set_unsafe(
            unit,
            first_entry,
            &oob_hr_map
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
  }

  for (idx = 0; idx < SOC_PETRA_FC_NOF_OFP_HRS; ++idx)
  {
    ofp_hr_to_table_entry_mapping[idx] = SOC_PETRA_FC_OFP_HR2ENTRY_INIT_VAL;
  }

  /*
   *  Initialization of the oob enable registers for the general SCH HR
   */
  reg_val = 0x0;
  for (reg_idx = 0; reg_idx < SOC_PETRA_SCH_HR_MASK_NOF_REGS; reg_idx++)
  {
    SOC_PA_REG_SET(regs->cfc.sch_hr_mask_reg[reg_idx].sch_hr_msk, reg_val, 19, exit);
  }

  /* Calendar parameters { */
  /*
   *  If interface B - set for FC Reception
   */
  if (oob_ndx == SOC_PETRA_FC_OOB_ID_B)
  {
    fld_val = 0x0;
    SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.oob_intrfb_sel, fld_val, 20, exit);
  }

  /*
   *  Enable/disable
   */
  fld_val = SOC_SAND_BOOL2NUM(oob_conf->enable);
  SOC_PA_FLD_ISET(regs->cfc.out_of_band_rx_configuration0_reg.oob_rx_en, fld_val, oob_ndx, 25, exit);

  /*
   *  Calendar length (single repetition)
   */
  fld_val = oob_conf->cal_len;
  SOC_PA_FLD_ISET(regs->cfc.out_of_band_rx_configuration0_reg.rx_cal_len, fld_val, oob_ndx, 30, exit);

  /*
   *  Calendar number of repetitions
   */
  fld_val = oob_conf->cal_reps;
  SOC_PA_FLD_ISET(regs->cfc.out_of_band_rx_configuration0_reg.rx_cal_m, fld_val, oob_ndx, 40, exit);
  /* Calendar parameters } */

  /* Calendar entries { */

  /*
   *  A first loop on the calendar to insert all the entries which are not sch-based ofp hr
   */
  for (per_rep_idx = 0; per_rep_idx < oob_conf->cal_len; per_rep_idx++)
  {
    is_sch_ofp_hr = FALSE;

    switch(cal_buff[per_rep_idx].destination) {
    case SOC_PETRA_FC_REC_OOB_DEST_HR:
      /*
       *  Handled by HR scheduler - ordinary HR 128-255.
       */
      oob_rx_cal.fc_dest_sel = 0x0;
      oob_rx_cal.fc_index = cal_buff[per_rep_idx].id - SOC_PETRA_FC_OOB_CAL_HR_ID_MIN;
      break;
    case SOC_PETRA_FC_REC_OOB_DEST_OFP_SCH_HR_LP:
    case SOC_PETRA_FC_REC_OOB_DEST_OFP_SCH_HR_HP:
      /*
       *  Is skipped in the first loop
       */
      is_sch_ofp_hr = TRUE;
      break;
    case SOC_PETRA_FC_REC_OOB_DEST_OFP_EGQ:
      /*
       *  Handled by OFP-at the EGQ
       */
      oob_rx_cal.fc_dest_sel = 0x1;
      oob_rx_cal.fc_index = cal_buff[per_rep_idx].id;
      break;
    case SOC_PETRA_FC_REC_OOB_DEST_NIF:
      oob_rx_cal.fc_dest_sel = 0x2 + SOC_PETRA_REG_IDX_GET(cal_buff[per_rep_idx].id, 16);
      oob_rx_cal.fc_index    = SOC_PETRA_FLD_IDX_GET(cal_buff[per_rep_idx].id, 16);
      break;
    case SOC_PETRA_FC_REC_OOB_DEST_NONE:
      oob_rx_cal.fc_dest_sel = SOC_PETRA_FC_OOB_CAL_EMPTY_SEL;
      oob_rx_cal.fc_index = SOC_PETRA_FC_OOB_CAL_EMPTY_ID;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FC_OOB_CAL_DEST_TYPE_INVALID_ERR, 50, exit);
    }

    if (!is_sch_ofp_hr)
    {
      /*
       *  Scheduler OFP-HR mapping in case of SCH (but not OFP) HR
       */
      if (cal_buff[per_rep_idx].destination == SOC_PETRA_FC_REC_OOB_DEST_HR)
      {
        /*
         *  Invalid for OFP - indicates that this entry is not usable for an OFP HR
         *  in the boolean table
         */
        table_entries_to_existing_general_hr_mapping[oob_rx_cal.fc_index] = TRUE;

        /*
         *  Enable the oob for this sch general hr
         */
        reg_idx = SOC_PETRA_REG_IDX_GET(oob_rx_cal.fc_index, SOC_SAND_REG_SIZE_BITS);
        SOC_PA_REG_GET(regs->cfc.sch_hr_mask_reg[reg_idx].sch_hr_msk, reg_val, 52, exit);
        SOC_SAND_SET_BIT(reg_val, 0x1, SOC_PETRA_FLD_IDX_GET(oob_rx_cal.fc_index, SOC_SAND_REG_SIZE_BITS));
        SOC_PA_REG_SET(regs->cfc.sch_hr_mask_reg[reg_idx].sch_hr_msk, reg_val, 53, exit);
      }

      for (rep_idx = 0; rep_idx < oob_conf->cal_reps; rep_idx++)
      {
        entry_idx = rep_idx * oob_conf->cal_len + per_rep_idx;

        res = soc_petra_fc_egr_rec_oob_tables_write(
                unit,
                oob_ndx,
                entry_idx,
                &oob_rx_cal,
                &oob_hr_map,
                FALSE
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
      }
    }
  }

  /*
   *  A second loop on the calendar to insert the sch-based ofp hr entries with the
   *  knowledge of all the existing sch-based gen hr
   */
  for (per_rep_idx = 0; per_rep_idx < oob_conf->cal_len; per_rep_idx++)
  {
    if (
        (cal_buff[per_rep_idx].destination == SOC_PETRA_FC_REC_OOB_DEST_OFP_SCH_HR_LP)
        || (cal_buff[per_rep_idx].destination == SOC_PETRA_FC_REC_OOB_DEST_OFP_SCH_HR_HP)
       )
    {
      /*
       *  Handled by HR scheduler - OFP HR, prioritized (Hi/Low priority)
       */
      oob_rx_cal.fc_dest_sel = 0x0;

      /*
       *  Search if this Sch OFP HR was inserted
       */
      ofp_mapping_indx = cal_buff[per_rep_idx].id;
      if (ofp_hr_to_table_entry_mapping[ofp_mapping_indx] !=
           SOC_PETRA_FC_OFP_HR2ENTRY_INIT_VAL)
      {
        /*
         *  The entry was inserted
         */
        oob_rx_cal.fc_index = ofp_hr_to_table_entry_mapping[ofp_mapping_indx];
      }
      else
      {
        /*
         *  Look for the first empty entry in the table
         */
        must_stop = FALSE;
       for (
              curr_entry_for_ofp_hr_idx = current_entry_for_ofp_hr;
              (curr_entry_for_ofp_hr_idx < SOC_PETRA_FC_SCH_BASED_OFP_MAPPING_TABLE_LENGTH) && (!must_stop);
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
          /*
           *  The table is full
           */
          SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FC_OOB_CAL_EXCESSIVE_NOF_SCH_OFP_HRS_ERR, 57, exit);
        }
        table_entries_to_existing_general_hr_mapping[current_entry_for_ofp_hr] = TRUE;
        oob_rx_cal.fc_index = current_entry_for_ofp_hr;
        ofp_hr_to_table_entry_mapping[ofp_mapping_indx] = current_entry_for_ofp_hr;
        current_entry_for_ofp_hr = curr_entry_for_ofp_hr_idx;
      }
     /*
       *  Scheduler OFP-HR High/Low priority mapping
       */

      /*
       *  Getting the current value of the table entry (to update)
       */
      if (oob_ndx == SOC_PETRA_FC_OOB_ID_A)
      {
        res = soc_petra_cfc_a_schedulers_based_flow_control_to_ofp_mapping_tbl_get_unsafe(
                unit,
                oob_rx_cal.fc_index,
                &oob_hr_map
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      }
      else
      {
        res = soc_petra_cfc_b_schedulers_based_flow_control_to_ofp_mapping_tbl_get_unsafe(
                unit,
                oob_rx_cal.fc_index,
                &oob_hr_map
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      }

      oob_hr_map.ofp_hr = cal_buff[per_rep_idx].id;
      if (cal_buff[per_rep_idx].destination == SOC_PETRA_FC_REC_OOB_DEST_OFP_SCH_HR_LP)
      {
        oob_hr_map.lp_ofp_valid = TRUE;
      }
      else /* SCH OFP HR HP */
      {
        oob_hr_map.hp_ofp_valid = TRUE;
      }

      for (rep_idx = 0; rep_idx < oob_conf->cal_reps; rep_idx++)
      {
        entry_idx = rep_idx * oob_conf->cal_len + per_rep_idx;

        res = soc_petra_fc_egr_rec_oob_tables_write(
                unit,
                oob_ndx,
                entry_idx,
                &oob_rx_cal,
                &oob_hr_map,
                TRUE
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_egr_rec_oob_set_unsafe()",0,0);
}

/*********************************************************************
*     Configures Out Of Band interface calendar for Flow
*     Control Reception (OOB RX).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_egr_rec_oob_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_IN  SOC_PETRA_FC_REC_OOB_CAL_INFO *cal_buff
  )
{
  uint32
    entry_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_EGR_REC_OOB_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(oob_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  SOC_SAND_MAGIC_NUM_VERIFY(oob_conf);
  SOC_SAND_MAGIC_NUM_VERIFY(cal_buff);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    oob_ndx, SOC_PETRA_FC_NOF_OOB_IFS-1,
    SOC_PETRA_FC_OOB_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    oob_conf->cal_reps, SOC_PETRA_FC_OOB_CAL_REP_MIN, SOC_PETRA_FC_OOB_CAL_REP_MAX,
    SOC_PETRA_FC_OOB_CAL_REP_OF_RANGE_ERR, 20, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    oob_conf->cal_len * oob_conf->cal_reps, SOC_PETRA_FC_OOB_CAL_LEN_MAX,
    SOC_PETRA_FC_OOB_CAL_LEN_OF_RANGE_ERR, 30, exit
  );

  for (entry_idx = 0; entry_idx < oob_conf->cal_len; entry_idx++)
  {
    switch(cal_buff[entry_idx].destination) {
    case SOC_PETRA_FC_REC_OOB_DEST_HR:
      SOC_SAND_ERR_IF_OUT_OF_RANGE(
        cal_buff[entry_idx].id, SOC_PETRA_FC_OOB_CAL_HR_ID_MIN, SOC_PETRA_FC_OOB_CAL_HR_ID_MAX,
        SOC_PETRA_FC_OOB_CAL_DEST_ID_OF_RANGE_ERR, 40, exit
      );
      break;
    case SOC_PETRA_FC_REC_OOB_DEST_OFP_EGQ:
    case SOC_PETRA_FC_REC_OOB_DEST_OFP_SCH_HR_LP:
    case SOC_PETRA_FC_REC_OOB_DEST_OFP_SCH_HR_HP:
      SOC_SAND_ERR_IF_ABOVE_MAX(
        cal_buff[entry_idx].id, SOC_PETRA_MAX_FAP_PORT_ID,
        SOC_PETRA_FC_OOB_CAL_DEST_ID_OF_RANGE_ERR, 50, exit
      );
      break;
    case SOC_PETRA_FC_REC_OOB_DEST_NIF:
      SOC_SAND_ERR_IF_ABOVE_MAX(
        cal_buff[entry_idx].id, SOC_PETRA_IF_NOF_NIFS-1,
        SOC_PETRA_FC_OOB_CAL_DEST_ID_OF_RANGE_ERR, 70, exit
      );
      break;
    case SOC_PETRA_FC_REC_OOB_DEST_NONE:
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FC_OOB_CAL_DEST_TYPE_INVALID_ERR, 80, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_egr_rec_oob_verify()",0,0);
}

/*********************************************************************
*     Configures Out Of Band interface calendar for Flow
*     Control Reception (OOB RX).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_egr_rec_oob_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_OOB_IF_INFO      *oob_conf,
    SOC_SAND_OUT SOC_PETRA_FC_REC_OOB_CAL_INFO *cal_buff
  )
{
  uint32
    fld_val = 0,
    reg_val,
    reg_idx,
    res;
  uint32
    entry_idx = 0;    /* Calendar entry index */
  uint8
    lp_and_hp_already_seen[SOC_PETRA_FC_NOF_OFP_HRS] = {FALSE},
    is_dest_hr = FALSE;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA
    oob_rx_cal;
  SOC_PETRA_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA
    oob_hr_map;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_EGR_REC_OOB_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(oob_conf);
  SOC_SAND_CHECK_NULL_INPUT(cal_buff);

  regs = soc_petra_regs();

  SOC_SAND_ERR_IF_ABOVE_MAX(
    oob_ndx, SOC_PETRA_FC_NOF_OOB_IFS-1,
    SOC_PETRA_FC_OOB_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  /*
   *  Initialize to keep the compiler quiet
   */
  oob_hr_map.ofp_hr = SOC_PETRA_FAP_PORT_ID_INVALID;
  oob_hr_map.lp_ofp_valid = FALSE;
  oob_hr_map.hp_ofp_valid = FALSE;

  /* Calendar parameters { */

  /*
   *  Enable/disable
   */
  SOC_PA_FLD_IGET(regs->cfc.out_of_band_rx_configuration0_reg.oob_rx_en, fld_val, oob_ndx, 20, exit);
  oob_conf->enable = SOC_SAND_NUM2BOOL(fld_val);


  /*
   *  Calendar length (single repetition)
   */
  SOC_PA_FLD_IGET(regs->cfc.out_of_band_rx_configuration0_reg.rx_cal_len, fld_val, oob_ndx, 30, exit);
  oob_conf->cal_len = fld_val;

  /*
   *  Calendar number of repetitions
   */
  SOC_PA_FLD_IGET(regs->cfc.out_of_band_rx_configuration0_reg.rx_cal_m, fld_val, oob_ndx, 40, exit);
  oob_conf->cal_reps = fld_val;
  /* Calendar parameters } */

  /* Calendar entries { */

  for (entry_idx = 0; entry_idx < oob_conf->cal_len; entry_idx++)
  {
    if (oob_ndx == SOC_PETRA_FC_OOB_ID_A)
    {
      res = soc_petra_cfc_out_of_band_rx_a_calendar_mapping_tbl_get_unsafe(
              unit,
              entry_idx,
              &oob_rx_cal
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

      is_dest_hr = SOC_SAND_NUM2BOOL(oob_rx_cal.fc_dest_sel == 0x0);

      if (is_dest_hr)
      {
        res = soc_petra_cfc_a_schedulers_based_flow_control_to_ofp_mapping_tbl_get_unsafe(
                  unit,
                  oob_rx_cal.fc_index,
                  &oob_hr_map
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      }
    }
    else
    {
      res = soc_petra_cfc_out_of_band_rx_b_calendar_mapping_tbl_get_unsafe(
              unit,
              entry_idx,
              &oob_rx_cal
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

      is_dest_hr = SOC_SAND_NUM2BOOL(oob_rx_cal.fc_dest_sel == 0x0);

      if (is_dest_hr)
      {
        res = soc_petra_cfc_b_schedulers_based_flow_control_to_ofp_mapping_tbl_get_unsafe(
                  unit,
                  oob_rx_cal.fc_index,
                  &oob_hr_map
                );
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
      }
    }

    if (is_dest_hr)
    {
      /*
       *  Scheduler HR - check if regular HR 128-255, or Port HR, Prioritized
       *  If in the oob fc to ofp hr mapping table, an entry has both priorities HP and LP
       *  valid, the first occurrence in the calendar points to the
       *  entry with a low priority and then assign high priority for the next times.
       */
      if (oob_hr_map.hp_ofp_valid)
      {
        if (oob_hr_map.lp_ofp_valid)
        {
          if (lp_and_hp_already_seen[oob_hr_map.ofp_hr] == FALSE)
          {
            cal_buff[entry_idx].destination = SOC_PETRA_FC_REC_OOB_DEST_OFP_SCH_HR_LP;
            cal_buff[entry_idx].id = oob_hr_map.ofp_hr;
            lp_and_hp_already_seen[oob_hr_map.ofp_hr] = TRUE;
          }
          else
          {
            cal_buff[entry_idx].destination = SOC_PETRA_FC_REC_OOB_DEST_OFP_SCH_HR_HP;
            cal_buff[entry_idx].id = oob_hr_map.ofp_hr;
          }
        }
        else
        {
          cal_buff[entry_idx].destination = SOC_PETRA_FC_REC_OOB_DEST_OFP_SCH_HR_HP;
          cal_buff[entry_idx].id = oob_hr_map.ofp_hr;
        }
      }
      else
      {
        if (oob_hr_map.lp_ofp_valid)
        {
          cal_buff[entry_idx].destination = SOC_PETRA_FC_REC_OOB_DEST_OFP_SCH_HR_LP;
          cal_buff[entry_idx].id = oob_hr_map.ofp_hr;
        }
        else
        {
          reg_idx = SOC_PETRA_REG_IDX_GET(oob_rx_cal.fc_index, SOC_SAND_REG_SIZE_BITS);
          SOC_PA_REG_GET(regs->cfc.sch_hr_mask_reg[reg_idx].sch_hr_msk, reg_val, 52, exit);
          fld_val = SOC_SAND_GET_BIT(reg_val, SOC_PETRA_FLD_IDX_GET(oob_rx_cal.fc_index, SOC_SAND_REG_SIZE_BITS));
          if (fld_val != 0)
          {
            cal_buff[entry_idx].destination = SOC_PETRA_FC_REC_OOB_DEST_HR;
            cal_buff[entry_idx].id = oob_rx_cal.fc_index + SOC_PETRA_FC_OOB_CAL_HR_ID_MIN;
          }
          else
          {
            /*
             *  Should not get here - mis-configured calendar
             */
            cal_buff[entry_idx].destination = oob_rx_cal.fc_dest_sel;
            cal_buff[entry_idx].id = oob_rx_cal.fc_index;
          }
        }
      }
    }
    else if (oob_rx_cal.fc_dest_sel == 0x1)
    {
      /*
       *  EGQ port
       */
      cal_buff[entry_idx].destination = SOC_PETRA_FC_REC_OOB_DEST_OFP_EGQ;
      cal_buff[entry_idx].id = oob_rx_cal.fc_index;
    }
    else if (oob_rx_cal.fc_dest_sel == 0x2)
    {
      /*
       *  NIF group A
       */
      cal_buff[entry_idx].destination = SOC_PETRA_FC_REC_OOB_DEST_NIF;
      cal_buff[entry_idx].id = oob_rx_cal.fc_index;
    }
    else if (oob_rx_cal.fc_dest_sel == 0x3)
    {
      /*
       *  NIF group B
       */
      cal_buff[entry_idx].destination = SOC_PETRA_FC_REC_OOB_DEST_NIF;
      cal_buff[entry_idx].id = oob_rx_cal.fc_index + 16;
    }
    else if ((oob_rx_cal.fc_dest_sel == SOC_PETRA_FC_OOB_CAL_EMPTY_SEL) &&
             (oob_rx_cal.fc_index == SOC_PETRA_FC_OOB_CAL_EMPTY_ID))
    {
      cal_buff[entry_idx].destination = SOC_PETRA_FC_REC_OOB_DEST_NONE;
      cal_buff[entry_idx].id = 0; /* Irrelevant */
    }
    else
    {
      /*
       *  Shouldn't get here - mis-configured calendar!
       */
      cal_buff[entry_idx].destination = oob_rx_cal.fc_dest_sel;
      cal_buff[entry_idx].id = oob_rx_cal.fc_index;
    }
  }
  /* Calendar entries } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_egr_rec_oob_get_unsafe()",0,0);
}

/*********************************************************************
*     Set the configuration for handling Flow Control
*     generated by (and effecting) recycling traffic, based on
*     Virtual Statistics Queues, handled on Outgoing FAP Port
*     level.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_rcy_vsq_ofp_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 cc_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_RCY_PORT_INFO    *info
  )
{
  uint32
    reg_val = 0,
    sch_l_val = 0,
    sch_h_val = 0,
    egq_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_FC_ON_GLB_RCS_MODE
    glb_mode;
  SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_DATA
    rcy2ofp_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_RCY_VSQ_OFP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  switch(info->handler) {
  case SOC_PETRA_FC_RCY_OFP_HANDLER_SCH_LP:
    sch_l_val = 0x1;
    break;
  case SOC_PETRA_FC_RCY_OFP_HANDLER_SCH_HP:
    sch_h_val = 0x1;
    break;
  case SOC_PETRA_FC_RCY_OFP_HANDLER_EGQ:
    egq_val = 0x1;
    break;
  case SOC_PETRA_FC_RCY_OFP_HANDLER_NONE:
  default:
    /*
     *  Leave all entries zeroed.
     */
   break;
  }

  /*
   *  Writing to recycle_to_out_going_fap_port_mapping_tbl
   *  effects also the HR mapping table for flow control on global
   *  resources.
   *  For this reason, we read and then restore the configuration of
   *  HR mapping table for flow control on global resources.
   *  (recycle_out_going_fap_port_hr_map_reg).
   */

  res = soc_petra_fc_rcy_glb_ofp_hr_get(
          unit,
          info->ofp_id,
          &glb_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  rcy2ofp_tbl_data.egq_ofp_num = info->ofp_id;

  res = soc_petra_cfc_recycle_to_out_going_fap_port_mapping_tbl_set_unsafe(
          unit,
          cc_ndx,
          &rcy2ofp_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_fc_rcy_glb_ofp_hr_set(
          unit,
          info->ofp_id,
          glb_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *  RMW - EGQ
   */
  SOC_PA_REG_GET(regs->cfc.rcycle_out_going_fap_port_map_reg, reg_val, 20, exit);

  SOC_SAND_SET_BIT(reg_val, egq_val, cc_ndx);

  SOC_PA_REG_SET(regs->cfc.rcycle_out_going_fap_port_map_reg, reg_val, 30, exit);

  /*
   *  RWM - SCH-High Priority
   */
  SOC_PA_REG_GET(regs->cfc.high_priority_hr_map_reg, reg_val, 40, exit);

  SOC_SAND_SET_BIT(reg_val, sch_h_val, cc_ndx);

  SOC_PA_REG_SET(regs->cfc.high_priority_hr_map_reg, reg_val, 50, exit);

  /*
   *  RWM - SCH-Low Priority
   */
  SOC_PA_REG_GET(regs->cfc.low_priority_hr_map_reg, reg_val, 60, exit);

  SOC_SAND_SET_BIT(reg_val, sch_l_val, cc_ndx);

  SOC_PA_REG_SET(regs->cfc.low_priority_hr_map_reg, reg_val, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_rcy_vsq_ofp_set_unsafe()",0,0);
}

/*********************************************************************
*     Set the configuration for handling Flow Control
*     generated by (and effecting) recycling traffic, based on
*     Virtual Statistics Queues, handled on Outgoing FAP Port
*     level.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_rcy_vsq_ofp_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 cc_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_RCY_PORT_INFO    *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_RCY_VSQ_OFP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    cc_ndx, SOC_PETRA_ITM_QT_CC_CLS_MAX,
    SOC_PETRA_FC_RCY_CONNECTION_CLS_OUT_OF_RANGE_ERR, 10, exit
  );

  res = soc_petra_fap_port_id_verify(unit, info->ofp_id, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->handler, SOC_PETRA_FC_NOF_RCY_OFP_HANDLERS-1,
    SOC_PETRA_FC_NOF_RCY_OFP_HANDLERS_OUT_OF_RANGE_ERR , 30, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_rcy_vsq_ofp_verify()",0,0);
}

/*********************************************************************
*     Set the configuration for handling Flow Control
*     generated by (and effecting) recycling traffic, based on
*     Virtual Statistics Queues, handled on Outgoing FAP Port
*     level.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_rcy_vsq_ofp_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 cc_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_RCY_PORT_INFO    *info
  )
{
  uint32
    reg_val = 0,
    sch_l_val = 0,
    sch_h_val = 0,
    egq_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_DATA
    rcy2ofp_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_RCY_VSQ_OFP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    cc_ndx, SOC_PETRA_ITM_QT_CC_CLS_MAX,
    SOC_PETRA_FC_RCY_CONNECTION_CLS_OUT_OF_RANGE_ERR, 10, exit
  );

  regs = soc_petra_regs();

  /*
   *  RMW - EGQ
   */
  SOC_PA_REG_GET(regs->cfc.rcycle_out_going_fap_port_map_reg, reg_val, 10, exit);

  egq_val = SOC_SAND_GET_BIT(reg_val, cc_ndx);

  /*
   *  RWM - SCH-High Priority
   */
  SOC_PA_REG_GET(regs->cfc.high_priority_hr_map_reg, reg_val, 20, exit);

  sch_h_val = SOC_SAND_GET_BIT(reg_val, cc_ndx);

  /*
   *  RWM - SCH-Low Priority
   */
  SOC_PA_REG_GET(regs->cfc.low_priority_hr_map_reg, reg_val, 30, exit);

  sch_l_val = SOC_SAND_GET_BIT(reg_val, cc_ndx);

  if (egq_val == 0x1)
  {
    info->handler = SOC_PETRA_FC_RCY_OFP_HANDLER_EGQ;
  }
  else if (sch_h_val == 0x1)
  {
    info->handler = SOC_PETRA_FC_RCY_OFP_HANDLER_SCH_HP;
  }
  else if (sch_l_val == 0x1)
  {
    info->handler = SOC_PETRA_FC_RCY_OFP_HANDLER_SCH_LP;
  }
  else
  {
    info->handler = SOC_PETRA_FC_RCY_OFP_HANDLER_NONE;
  }

  res = soc_petra_cfc_recycle_to_out_going_fap_port_mapping_tbl_get_unsafe(
          unit,
          cc_ndx,
          &rcy2ofp_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->ofp_id = rcy2ofp_tbl_data.egq_ofp_num;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_rcy_vsq_ofp_get_unsafe()",0,0);
}

/*********************************************************************
*     Enable/disable the specified Flow Control mode for the
*     recycling OFP-s. If enabled, Flow Control is received by
*     OFP HR schedulers, based on Global Resources Flow
*     Control indication.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_rcy_glb_ofp_hr_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_ON_GLB_RCS_MODE  fc_mode
  )
{
  uint32
    fld_val = 0,
    reg_val = 0,
    lp_enable = 0,
    hp_enable = 0,
    res;
  uint32
    fld_idx = 0,
    reg_idx = 0;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_RCY_GLB_OFP_HR_SET_UNSAFE);

  regs = soc_petra_regs();

  /*
   *  Set mapping table
   */
  reg_idx = SOC_PETRA_REG_IDX_GET(ofp_ndx, SOC_SAND_REG_SIZE_BITS);
  fld_idx = SOC_PETRA_FLD_IDX_GET(ofp_ndx, SOC_SAND_REG_SIZE_BITS);
  fld_val = SOC_PETRA_FC_ON_GLB_RCS_MODE_DISABLE?0x0:0x1;

  /*
   *  RMW
   */
  SOC_PA_REG_GET(regs->cfc.recycle_out_going_fap_port_hr_map_reg[reg_idx], reg_val, 10, exit);

  SOC_SAND_SET_BIT(reg_val, fld_val, fld_idx);

  SOC_PA_REG_SET(regs->cfc.recycle_out_going_fap_port_hr_map_reg[reg_idx], reg_val, 20, exit);


  /*
   *  Set flow control mode
   */
  switch(fc_mode) {
  case SOC_PETRA_FC_ON_GLB_RCS_MODE_ENABLE_LP:
    lp_enable = 0x1;
    break;
  case SOC_PETRA_FC_ON_GLB_RCS_MODE_ENABLE_HP:
    hp_enable = 0x1;
    break;
  case SOC_PETRA_FC_ON_GLB_RCS_MODE_DISABLE:
  default:
    /*
     *  Leave all disabled
     */
    break;
  }

  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.rcl_ofp_lp_en, lp_enable, 10, exit);

  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.rcl_ofp_hp_en, hp_enable, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_rcy_glb_ofp_hr_set_unsafe()",0,0);
}

/*********************************************************************
*     Enable/disable the specified Flow Control mode for the
*     recycling OFP-s. If enabled, Flow Control is received by
*     OFP HR schedulers, based on Global Resources Flow
*     Control indication.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_rcy_glb_ofp_hr_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_ON_GLB_RCS_MODE  fc_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_RCY_GLB_OFP_HR_VERIFY);

  res = soc_petra_fap_port_id_verify(unit, ofp_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    fc_mode, SOC_PETRA_FC_NOF_ON_GLB_RCS_MODES-1,
    SOC_PETRA_FC_RCY_ON_GLB_RCS_MODE_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_rcy_glb_ofp_hr_verify()",0,0);
}

/*********************************************************************
*     Enable/disable the specified Flow Control mode for the
*     recycling OFP-s. If enabled, Flow Control is received by
*     OFP HR schedulers, based on Global Resources Flow
*     Control indication.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_rcy_glb_ofp_hr_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_ON_GLB_RCS_MODE  *fc_mode
  )
{
  uint32
    lp_enable = 0,
    hp_enable = 0,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_RCY_GLB_OFP_HR_GET_UNSAFE);

  res = soc_petra_fap_port_id_verify(unit, ofp_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  regs = soc_petra_regs();

  SOC_PA_FLD_GET(regs->cfc.cfc_enablers_reg.rcl_ofp_lp_en, lp_enable, 10, exit);

  SOC_PA_FLD_GET(regs->cfc.cfc_enablers_reg.rcl_ofp_hp_en, hp_enable, 20, exit);

  if (hp_enable)
  {
    *fc_mode = SOC_PETRA_FC_ON_GLB_RCS_MODE_ENABLE_HP;
  }
  else if (lp_enable)
  {
    *fc_mode = SOC_PETRA_FC_ON_GLB_RCS_MODE_ENABLE_LP;
  }
  else
  {
    *fc_mode = SOC_PETRA_FC_ON_GLB_RCS_MODE_DISABLE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_rcy_glb_ofp_hr_get_unsafe()",0,0);
}

/*********************************************************************
*     Enable/disable reception of Flow Control for Recycling
*     ports by end to end Scheduler HR-s 192-223, represented
*     by Category 3 Connection Class in Ingress Statistics
*     Queues, when the Statistics Queues threshold is crossed.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_rcy_hr_enable_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  )
{
  uint32
    fld_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_RCY_HR_ENABLE_SET_UNSAFE);

  regs = soc_petra_regs();

  fld_val = SOC_SAND_BOOL2NUM(enable);
  SOC_PA_FLD_SET(regs->cfc.cfc_enablers_reg.rcl_vsq_hr_en, fld_val, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_rcy_hr_enable_set_unsafe()",0,0);
}

/*********************************************************************
*     Enable/disable reception of Flow Control for Recycling
*     ports by end to end Scheduler HR-s 192-223, represented
*     by Category 3 Connection Class in Ingress Statistics
*     Queues, when the Statistics Queues threshold is crossed.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_rcy_hr_enable_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_RCY_HR_ENABLE_VERIFY);


  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_rcy_hr_enable_verify()",0,0);
}

/*********************************************************************
*     Enable/disable reception of Flow Control for Recycling
*     ports by end to end Scheduler HR-s 192-223, represented
*     by Category 3 Connection Class in Ingress Statistics
*     Queues, when the Statistics Queues threshold is crossed.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_fc_rcy_hr_enable_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  )
{
  uint32
    fld_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_RCY_HR_ENABLE_GET_UNSAFE);

  regs = soc_petra_regs();

  SOC_PA_FLD_GET(regs->cfc.cfc_enablers_reg.rcl_vsq_hr_en, fld_val, 10, exit);
  *enable = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_rcy_hr_enable_get_unsafe()",0,0);
}

uint32
  soc_petra_fc_egr_rec_oob_stat_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FC_OOB_ID           oob_ndx,
    SOC_SAND_OUT SOC_PETRA_FC_EGR_REC_OOB_STAT *status
  )
{
  uint32
    reg_val = 0,
    fld_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    obrx_lock_err = 1,
    obrx_out_of_frm = 1,
    obrx_dip2_alarm = 1,
    obrx_frm_err = 1,
    obrx_dip2_err = 1;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_FC_EGR_REC_OOB_STAT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(status);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    oob_ndx, SOC_PETRA_FC_NOF_OOB_IFS-1,
    SOC_PETRA_FC_OOB_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  regs = soc_petra_regs();

  soc_petra_PETRA_FC_EGR_REC_OOB_STAT_clear(status);

  SOC_PA_FLD_SET(regs->cfc.interrupt_reg.obrx_lock_err[oob_ndx],   0x1, 2, exit);
  SOC_PA_FLD_SET(regs->cfc.interrupt_reg.obrx_out_of_frm[oob_ndx], 0x1, 4, exit);
  SOC_PA_FLD_SET(regs->cfc.interrupt_reg.obrx_dip2_alarm[oob_ndx], 0x1, 6, exit);
  SOC_PA_FLD_SET(regs->cfc.interrupt_reg.obrx_frm_err[oob_ndx],    0x1, 8, exit);
  SOC_PA_FLD_SET(regs->cfc.interrupt_reg.obrx_dip2_err[oob_ndx],   0x1, 10, exit);

  SOC_PA_FLD_GET(regs->cfc.interrupt_reg.obrx_lock_err[oob_ndx],   obrx_lock_err,   12, exit);
  SOC_PA_FLD_GET(regs->cfc.interrupt_reg.obrx_out_of_frm[oob_ndx], obrx_out_of_frm, 14, exit);
  SOC_PA_FLD_GET(regs->cfc.interrupt_reg.obrx_dip2_alarm[oob_ndx], obrx_dip2_alarm, 16, exit);
  SOC_PA_FLD_GET(regs->cfc.interrupt_reg.obrx_frm_err[oob_ndx],    obrx_frm_err,    18, exit);
  SOC_PA_FLD_GET(regs->cfc.interrupt_reg.obrx_dip2_err[oob_ndx],   obrx_dip2_err,   20, exit);

  status->dip2_alarm = SOC_SAND_NUM2BOOL(obrx_dip2_alarm);
  status->nof_dip2_errors   = SOC_SAND_NUM2BOOL(obrx_dip2_err);
  status->nof_frame_errors    = SOC_SAND_NUM2BOOL(obrx_frm_err);
  status->not_locked   = SOC_SAND_NUM2BOOL(obrx_lock_err);
  status->out_of_frame = SOC_SAND_NUM2BOOL(obrx_out_of_frm);

  if (
      (status->nof_frame_errors) ||
      (status->nof_dip2_errors)
     )
  {
    SOC_PA_REG_IGET(regs->cfc.out_of_band_rx_error_counter_reg, reg_val, oob_ndx, 30, exit);

    if (status->nof_frame_errors)
    {
      SOC_PA_FLD_FROM_REG(regs->cfc.out_of_band_rx_error_counter_reg.frm_err_cnt, fld_val, reg_val, 40, exit);
      status->nof_frame_errors = SOC_SAND_MAX(1, fld_val);
    }

    if (status->nof_dip2_errors)
    {
      SOC_PA_FLD_FROM_REG(regs->cfc.out_of_band_rx_error_counter_reg.dip2_err_cnt, fld_val, reg_val, 50, exit);
      status->nof_dip2_errors = SOC_SAND_MAX(1, fld_val);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_fc_egr_rec_oob_stat_get_unsafe()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
