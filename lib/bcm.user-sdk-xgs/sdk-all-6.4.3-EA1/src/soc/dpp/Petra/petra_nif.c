/* $Id: petra_nif.c,v 1.12 Broadcom SDK $
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

#include <soc/dpp/Petra/petra_nif.h>
#include <soc/dpp/Petra/petra_api_fabric.h>
#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/petra_ingress_header_parsing.h>
#include <soc/dpp/Petra/petra_serdes.h>
#include <soc/dpp/Petra/petra_serdes_utils.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_mgmt.h>

#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_fabric.h>
#include <soc/dpp/Petra/petra_bitstream.h>
#include <soc/dpp/port_map.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: petra_nif.c,v 1.12 Broadcom SDK $
 *  MDIO {
 */
/* Maximal allowed delay for MDIO read operation */
#define SOC_PETRA_NIF_MAX_MDIO_BUSY_WAIT_ITERATIONS   50
#define SOC_PETRA_NIF_MAX_MDIO_TIMER_ITERATIONS       10
#define SOC_PETRA_NIF_MAX_MDIO_TIMER_DELAY_MSEC       1

/* Dummy MDIO transaction as a WA to release MDIO bus */
#define SOC_PETRA_NIF_MDIO_DUMMY_OP 0x10020000


/* Maximal allowed delay to wait for the selector */
#define SOC_PETRA_NIF_WAIT_NS  30

/*
 *  MDIO }
 */

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/*
 *  Internally, the 8 MAL-s are
 *  devided in NIF groups A and B
 */
#define SOC_PETRA_NIF_GRP_NDX(mal_ndx) ((mal_ndx) / SOC_PETRA_MALS_IN_NIF_GRP)
#define SOC_PETRA_MAL_IN_NIF_GRP_NDX(mal_ndx) ((mal_ndx) % SOC_PETRA_MALS_IN_NIF_GRP)

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
static const uint8 Soc_petra_nif_counter_nof_bits[] = {34,34,8,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,32,34,40,34,40};

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
* NAME:
*     soc_petra_nif_regs_init
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
uint32
  soc_petra_nif_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    reg_val,
    fld_val,
    res;
  uint32
    mal_idx;
  SOC_PETRA_REGS
    *regs;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_REGS_INIT);

  regs = soc_petra_regs();

  /* Per-port thresholds - set to XAUI-suitable default */
  SOC_PA_REG_SET(regs->epni.nif_ports_fc_thresh_config_reg[0], 0x11113333, 10, exit);
  SOC_PA_REG_SET(regs->epni.nif_ports_fc_thresh_config_reg[1], 0x11113333, 30, exit);

  SOC_PA_REG_SET(regs->epni.nif_ports_fc_thresh_config_const_reg[0], 0x11111111, 12, exit);
  SOC_PA_REG_SET(regs->epni.nif_ports_fc_thresh_config_const_reg[1], 0x11111111, 14, exit);

  /* Set "no fragmentation" as default */
  SOC_PA_REG_SET(regs->epni.egress_interface_no_fragmentation_mode_configuration_reg, 0x000003ff, 50, exit);
  SOC_PA_REG_SET(regs->egq.egress_interface_no_fragmentation_mode_configuration_reg, 0x000003ff, 60, exit);

  /*
   *  SGMII Auto-negotiation defaults
   */
  for (mal_idx = 0; mal_idx < SOC_PETRA_NOF_MAC_LANES; mal_idx++)
  {
    /* idx is MAL-ID */
    SOC_PA_FLD_ISET(regs->nif_mac_lane.auto_neg_config2_reg.num_clocks_in_hundred_us, 12500, mal_idx, 70, exit);
  }

  /*
   *  Init MDIO
   */
  SOC_PA_FLD_SET(regs->eci.mdio_cfg_reg.cfg_tick_cnt, 250, 80, exit);
  SOC_PA_FLD_SET(regs->eci.mdio_cfg_reg.cfg_pd,       0x0,   82, exit);
  SOC_PA_FLD_SET(regs->eci.mdio_cfg_reg.cfg_init,     0x0,   84, exit);

  /*
   * Disable CBFC reception (will be treated as LLFC pause frame)
   * The reason for disabling CBFC-RX is that it is not mapped to different priorities in the CFC.
   * In the NIF, the following would be needed to enable CBFC-RX:
   * CBFC pause frames mode:
   * rx_bct_llfc = 0x0;
   * rx_cbfc     = 0x1;
   * flat_mode   = 0x0;
   * rx_terminate_mac_ctrl = 0x1;
   * rx_terminate_pause    = 0x1;
   * rx_mask_mac_ctrl      = 0x0;
   * rx_mask_fc_burst      = 0x1;
   * cfc_paused_indic_en   = 0xff;  (Mask towards the CFC: 2b per SGMII or 8b per XAUI)
   */
  for (mal_idx = 0; mal_idx < SOC_PETRA_NOF_MAC_LANES; mal_idx++)
  {
    /*
     * Do not forward pause frame indication to the CFC.
     * LLFC is treated locally, and CBFC reception is disabled.
     */
    SOC_PA_FLD_ISET(regs->nif_mac_lane.rx_flow_control_overrides2_reg.cfc_paused_indic_en, 0x0, mal_idx, 88, exit);

    SOC_PA_REG_IGET(regs->nif_mac_lane.rx_flow_control_config_reg, reg_val, mal_idx, 90, exit);
   
    fld_val = 0x0; /* For the following lines */
    SOC_PA_FLD_TO_REG(regs->nif_mac_lane.rx_flow_control_config_reg.rx_cbfc, fld_val, reg_val, 92, exit);
    SOC_PA_FLD_TO_REG(regs->nif_mac_lane.rx_flow_control_config_reg.rx_mask_mac_ctrl, fld_val, reg_val, 94, exit);
    SOC_PA_FLD_TO_REG(regs->nif_mac_lane.rx_flow_control_config_reg.rx_mask_fc_burst, fld_val, reg_val, 96, exit);

    SOC_PA_REG_ISET(regs->nif_mac_lane.rx_flow_control_config_reg, reg_val, mal_idx, 100, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_regs_init()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_nif_init
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
  soc_petra_nif_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    mal_ndx;
  SOC_PETRA_NIF_FC_INFO
    fc_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_INIT);

  /*
   * Note:
   * Register initialization occur in phase1, before interface initialization,
   * since it can be overridden by the NIF configuration
   */

  soc_petra_PETRA_NIF_FC_INFO_clear(&fc_info);

  /*
   *  Set pause quanta default
   */
  for (mal_ndx = 0; mal_ndx < SOC_PETRA_NOF_MAC_LANES; mal_ndx++)
  {
    if (soc_petra_sw_db_pp_enable_get(unit))
    {
      fc_info.rx.ll_mode              = SOC_PETRA_NIF_LL_FC_MODE_ENABLE_PAUSE_FRAME;
      fc_info.rx.treat_ll_class_based = FALSE;
      fc_info.tx.cb_enable            = FALSE;
      fc_info.tx.ll_mode              = SOC_PETRA_NIF_LL_FC_MODE_ENABLE_PAUSE_FRAME;
      fc_info.tx.on_if_cleared        = TRUE;

      res = soc_petra_nif_pause_quanta_set_unsafe(
            unit,
            mal_ndx,
            SOC_PETRA_FC_DIRECTION_GEN,
            0xfff
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_petra_nif_pause_quanta_set_unsafe(
            unit,
            mal_ndx,
            SOC_PETRA_FC_DIRECTION_REC,
            0xfff
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);
    }
    else
    {
      fc_info.rx.ll_mode = SOC_PETRA_NIF_LL_FC_MODE_DISABLE;
      fc_info.tx.ll_mode = SOC_PETRA_NIF_LL_FC_MODE_DISABLE;
    }

    /*
     *	Most of the configuration is per-MAL,
     *  the part that is per-lane has good HW defaults
     */
    res = soc_petra_nif_fc_set_unsafe(
            unit,
            SOC_PETRA_MAL2NIF_NDX(mal_ndx),
            &fc_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_init()",0,0);
}

STATIC void
  soc_petra_nif_srd_lanes_internal(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint32      mal_ndx,
    SOC_SAND_IN SOC_PETRA_NIF_INFO *nif_info,
    SOC_SAND_OUT uint32     *nof_lanes,
    SOC_SAND_OUT uint32     *base_lane
  )
{
  uint32
    nof_srd_lanes,
    base_lane_id,
    srd_qrtt_id;
  SOC_PETRA_INTERFACE_ID
    nif_ndx;
  uint8
    is_combo_qrtt_orphan;

  nif_ndx = SOC_PETRA_MAL2NIF_NDX(mal_ndx);
  base_lane_id = nif_ndx;
  srd_qrtt_id = SOC_PETRA_NIF_MAL2QRTT_ID(mal_ndx);

  if (
      (nif_info == NULL)  ||
      (nof_lanes == NULL) ||
      (base_lane == NULL)
     )
  {
    return;
  }

  is_combo_qrtt_orphan = soc_petra_is_fabric_quartet(
                           unit,
                           srd_qrtt_id
                         );

  switch(nif_info->type) {
  case SOC_PETRA_NIF_TYPE_XAUI:
  case SOC_PETRA_NIF_TYPE_SPAUI:
    if (nif_info->serdes_rate == SOC_PETRA_NIF_SERDES_RATE_SINGLE)
    {
      nof_srd_lanes = SOC_PETRA_MAX_NIFS_PER_MAL;
    }
    else
    {
      if (is_combo_qrtt_orphan)
      {
        nof_srd_lanes = SOC_PETRA_MAX_NIFS_PER_MAL;
        base_lane_id = nif_ndx - SOC_PETRA_MAX_NIFS_PER_MAL;
      }
      else
      {
        nof_srd_lanes = SOC_PETRA_MAX_NIFS_PER_MAL/2;
      }

      if (
          (nif_info->lanes_swap.swap_rx) ||
          (nif_info->lanes_swap.swap_tx)
         )
      {
        base_lane_id += SOC_PETRA_MAX_NIFS_PER_MAL/2;
      }
    }

    break;
  case SOC_PETRA_NIF_TYPE_SGMII:
    nof_srd_lanes  = 1;
    break;
  default:
    nof_srd_lanes = 0;
  }

  *nof_lanes = nof_srd_lanes;
  *base_lane = base_lane_id;
}

STATIC void
  soc_petra_nif_srd_lanes_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID nif_ndx,
    SOC_SAND_OUT uint32          *nof_lanes,
    SOC_SAND_OUT uint32          *base_lane
  )
{
  uint32
    mal_ndx,
    nof_srd_lanes,
    base_lane_id;
  SOC_PETRA_NIF_INFO
    nif_info;
  uint32
    res;

  if (
      (nof_lanes == NULL) ||
      (base_lane == NULL)
     )
  {
    return;
  }

  soc_petra_PETRA_NIF_INFO_clear(&nif_info);
  mal_ndx = SOC_PETRA_NIF2MAL_NDX(nif_ndx);

  base_lane_id = nif_ndx;

  res = soc_petra_nif_mal_basic_conf_get_unsafe(
          unit,
          mal_ndx,
          &nif_info
        );
  if (soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    nof_srd_lanes = 0;
    goto exit;
  }

  soc_petra_nif_srd_lanes_internal(
    unit,
    mal_ndx,
    &nif_info,
    &nof_srd_lanes,
    &base_lane_id
  );

exit:
  *nof_lanes = nof_srd_lanes;
  *base_lane = base_lane_id;
}

/*
 *  Enable/Disable SGMII in the specified direction
 */
STATIC uint32
  soc_petra_nif_sgmii_enable_state_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID          nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION  direction_ndx,
    SOC_SAND_IN  uint8                   is_enabled
  )
{
  uint32
    fld_val,
    reg_val,
    val,
    res;
  SOC_PETRA_REGS
    *regs;
  uint8
    rx_enable = FALSE,
    tx_enable = FALSE;
  uint32
    mal_ndx = 0,
    lane_ndx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SGMII_ENABLE_STATE_SET);

  regs = soc_petra_regs();

  rx_enable = SOC_PETRA_IS_DIRECTION_RX(direction_ndx);
  tx_enable = SOC_PETRA_IS_DIRECTION_TX(direction_ndx);

  mal_ndx = SOC_PETRA_NIF2MAL_NDX(nif_ndx);
  lane_ndx= SOC_PETRA_NIF_SGMII_LANE(nif_ndx);

  val = SOC_SAND_BOOL2NUM(is_enabled);

  SOC_PA_REG_IGET(regs->nif_mac_lane.enable_reg, reg_val, mal_ndx, 10, exit);

  if (rx_enable)
  {
    /* RMW the specified lane of SGMII enable field - rx */
    SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.enable_reg.sgmii_rx_lane_en, fld_val, reg_val, 20, exit);

    SOC_SAND_SET_BIT(fld_val, val, lane_ndx);

    SOC_PA_FLD_TO_REG(regs->nif_mac_lane.enable_reg.sgmii_rx_lane_en, fld_val, reg_val, 30, exit);
  }

  if (tx_enable)
  {
    /* RMW the specified lane of SGMII enable field - tx */
    SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.enable_reg.sgmii_tx_lane_en, fld_val, reg_val, 40, exit);

    SOC_SAND_SET_BIT(fld_val, val, lane_ndx);

    SOC_PA_FLD_TO_REG(regs->nif_mac_lane.enable_reg.sgmii_tx_lane_en, fld_val, reg_val, 50, exit);
  }

  SOC_PA_REG_ISET(regs->nif_mac_lane.enable_reg, reg_val, mal_ndx, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_sgmii_enable_state_set()",nif_ndx,0);
}

STATIC uint32
  soc_petra_nif_sgmii_enable_state_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID          nif_ndx,
    SOC_SAND_OUT  uint8                  *is_enabled_rx,
    SOC_SAND_OUT  uint8                  *is_enabled_tx
  )
{
  uint32
    fld_val,
    reg_val,
    val,
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    mal_ndx = 0,
    lane_ndx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SGMII_ENABLE_STATE_GET);

  regs = soc_petra_regs();

  SOC_SAND_CHECK_NULL_INPUT(is_enabled_rx);
  SOC_SAND_CHECK_NULL_INPUT(is_enabled_tx);

  mal_ndx = SOC_PETRA_NIF2MAL_NDX(nif_ndx);
  lane_ndx= SOC_PETRA_NIF_SGMII_LANE(nif_ndx);

  SOC_PA_REG_IGET(regs->nif_mac_lane.enable_reg, reg_val, mal_ndx, 10, exit);

  /* Read the specified lane of SGMII enable field - rx */
  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.enable_reg.sgmii_rx_lane_en, fld_val, reg_val, 20, exit);
  val = SOC_SAND_GET_BIT(fld_val, lane_ndx);
  *is_enabled_rx = SOC_SAND_NUM2BOOL(val);

  /* Read the specified lane of SGMII enable field - tx */
  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.enable_reg.sgmii_tx_lane_en, fld_val, reg_val, 40, exit);
  val = SOC_SAND_GET_BIT(fld_val, lane_ndx);
  *is_enabled_tx = SOC_SAND_NUM2BOOL(val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_sgmii_enable_state_get()",nif_ndx,0);
}

/*********************************************************************
*     Turns the interface on/off. Optionally, powers up/down
*     the attached SerDes also.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_on_off_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_STATE_INFO      *info
  )
{
  uint32
    res;
  uint32
    srd_lane_id,
    base_lane_id,
    mal_ndx,
    nof_srd_lanes;
  SOC_PETRA_NIF_LINK_STATUS
    link_status;
  uint8
    is_sgmii,
    is_enabled_rx,
    is_enabled_tx;
  SOC_PETRA_INTERFACE_ID
    sgmii_nif_id;
  SOC_PETRA_SRD_POWER_STATE
    srd_pwr_state = SOC_PETRA_SRD_NOF_POWER_STATES;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_ON_OFF_SET_UNSAFE);

  soc_petra_PETRA_NIF_LINK_STATUS_clear(&link_status);

  soc_petra_nif_srd_lanes_get(unit, nif_ndx, &nof_srd_lanes, &base_lane_id);

  if (nof_srd_lanes == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INVALID_TYPE_ERR, 10, exit);
  }

  mal_ndx = SOC_PETRA_NIF2MAL_NDX(nif_ndx);

  res = soc_petra_nif_is_sgmii_get(
          unit,
          mal_ndx,
          &is_sgmii
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /*
   *  Disable the NIF
   */
  if (is_sgmii)
  {
    if (
        (SOC_PETRA_NIF_SGMII_LANE(nif_ndx) == 0)  &&
        (info->on_off == SOC_PETRA_NIF_STATE_OFF) &&
        (info->serdes_also == TRUE)
       )
    {
      /*
       *  It is not allowed to turn off the TX direction of the SerDes of the first
       *  SGMII in MAL, while some other SGMII interfaces are still up in this MAL.
       *  Indicate an error if this is the case
       */
      for (sgmii_nif_id = nif_ndx + 1; sgmii_nif_id < nif_ndx + SOC_PETRA_MAX_NIFS_PER_MAL; sgmii_nif_id++)
      {
        res = soc_petra_nif_sgmii_enable_state_get(
                unit,
                sgmii_nif_id,
                &is_enabled_rx,
                &is_enabled_tx
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

        if (is_enabled_rx || is_enabled_tx)
        {
          SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_SGMII_0_OFF_INVALID_ERR, 28, exit);
        }
      }
    }

    res = soc_petra_nif_sgmii_enable_state_set(
            unit,
            nif_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_BOTH,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }
  else
  {
    res = soc_petra_nif_mal_enable_set_unsafe(
            unit,
            mal_ndx,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if (info->serdes_also)
  {
    srd_pwr_state = (info->on_off == SOC_PETRA_NIF_STATE_ON)?SOC_PETRA_SRD_POWER_STATE_UP_AND_RELOCK:SOC_PETRA_SRD_POWER_STATE_DOWN;

    /*
     *  Set the NIF SerDes power-up/down state
     */
    for (srd_lane_id = base_lane_id; srd_lane_id < (base_lane_id + nof_srd_lanes); srd_lane_id++)
    {
      res = soc_petra_srd_lane_power_state_set_unsafe(
              unit,
              srd_lane_id,
              SOC_PETRA_CONNECTION_DIRECTION_BOTH,
              srd_pwr_state
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
    }
  }

  if (info->on_off == SOC_PETRA_NIF_STATE_ON)
  {
    /*
     *  Enable the NIF if requested
     */
    if (is_sgmii)
    {
      /*
       *  Enable RX/TX as before the API call
       */
      res = soc_petra_nif_sgmii_enable_state_set(
              unit,
              nif_ndx,
              SOC_PETRA_CONNECTION_DIRECTION_BOTH,
              TRUE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
    else
    {
      res = soc_petra_nif_mal_enable_set_unsafe(
              unit,
              mal_ndx,
              TRUE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_on_off_set_unsafe()",nif_ndx,0);
}

/*********************************************************************
*     Turns the interface on/off. Optionally, powers up/down
*     the attached SerDes also.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_on_off_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_STATE_INFO      *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_ON_OFF_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_nif_id_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->on_off, SOC_PETRA_NIF_NOF_STATES,
    SOC_PETRA_NIF_ON_OFF_STATE_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_on_off_verify()",nif_ndx,0);
}

/*********************************************************************
*     Turns the interface on/off. Optionally, powers up/down
*     the attached SerDes also.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_on_off_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_STATE_INFO      *info
  )
{
  uint32
    res;
  uint32
    srd_lane_id,
    base_lane_id,
    mal_ndx,
    nof_srd_lanes;
  uint8
    is_sgmii,
    is_enabled_rx,
    is_enabled_tx;
  uint8
    is_srd_pwr_same = TRUE,
    is_nif_enabled;
  SOC_PETRA_SRD_POWER_STATE
    srd_pwr_state_rx = SOC_PETRA_SRD_NOF_POWER_STATES,
    srd_pwr_state_tx = SOC_PETRA_SRD_NOF_POWER_STATES,
    srd_pwr_state_first= SOC_PETRA_SRD_NOF_POWER_STATES;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_ON_OFF_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_nif_id_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  soc_petra_nif_srd_lanes_get(unit, nif_ndx, &nof_srd_lanes, &base_lane_id);

  if (nof_srd_lanes == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INVALID_TYPE_ERR, 10, exit);
  }

  mal_ndx = SOC_PETRA_NIF2MAL_NDX(nif_ndx);

  res = soc_petra_nif_is_sgmii_get(
          unit,
          mal_ndx,
          &is_sgmii
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  if (is_sgmii)
  {
    res = soc_petra_nif_sgmii_enable_state_get(
            unit,
            nif_ndx,
            &is_enabled_rx,
            &is_enabled_tx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

    is_nif_enabled = (is_enabled_rx && is_enabled_tx);
  }
  else
  {
    res = soc_petra_nif_mal_enable_get_unsafe(
              unit,
              mal_ndx,
              &is_nif_enabled
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }

  for (srd_lane_id = base_lane_id; srd_lane_id < (base_lane_id + nof_srd_lanes); srd_lane_id++)
  {
    res = soc_petra_srd_lane_power_state_get_unsafe(
            unit,
            srd_lane_id,
            &srd_pwr_state_rx,
            &srd_pwr_state_tx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (srd_pwr_state_rx != srd_pwr_state_tx)
    {
      is_srd_pwr_same = FALSE;
      continue;
    }

    if (srd_lane_id == base_lane_id)
    {
      srd_pwr_state_first = srd_pwr_state_rx;
    }
    else
    {
      if (srd_pwr_state_rx != srd_pwr_state_first)
      {
        is_srd_pwr_same = FALSE;
        continue;
      }
    }
  }

  info->on_off = (is_nif_enabled)?SOC_PETRA_NIF_STATE_ON:SOC_PETRA_NIF_STATE_OFF;

  if (!is_srd_pwr_same)
  {
    info->serdes_also = FALSE;
  }
  else
  {
    if(
        (is_nif_enabled && (srd_pwr_state_first == SOC_PETRA_SRD_POWER_STATE_UP)) ||
        (!(is_nif_enabled) && (srd_pwr_state_first == SOC_PETRA_SRD_POWER_STATE_DOWN))
      )
    {
      info->serdes_also = TRUE;
    }
    else
    {
      info->serdes_also = FALSE;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_on_off_get_unsafe()",nif_ndx,0);
}

/*****************************************************
* NAME
*    soc_petra_nif_is_sgmii_get
* TYPE:
*   PROC
* DATE:
*   07/02/2008
* FUNCTION:
*   Identify sgmii interface
* INPUT:
*   SOC_SAND_IN  uint32       mal_ndx -
*      MAC Lane index. Range: 0 - 7.
*   SOC_SAND_OUT uint8       *is_sgmii -
*     TRUE if the interface is sgmii.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   This function is more efficient then
*   nif_type_getm and nif_mal_basic_conf_get
*****************************************************/
uint32
  soc_petra_nif_is_sgmii_get(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32       mal_ndx,
    SOC_SAND_OUT uint8       *is_sgmii
  )
{
  uint32
    fld_val = 0,
    val = 0,
    res;
  SOC_PETRA_REG_FIELD
    *fld_addr;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_TYPE_FROM_EGQ_GET);
  SOC_SAND_CHECK_NULL_INPUT(is_sgmii);

  regs = soc_petra_regs();

  /* Get EGQ configuration { */
  if (mal_ndx < SOC_PETRA_MALS_IN_NIF_GRP)
  {
    fld_addr = SOC_PA_REG_DB_ACC_REF(regs->egq.nifa_sgmii_reg.nifa_sgmii);
  }
  else
  {
    fld_addr = SOC_PA_REG_DB_ACC_REF(regs->egq.nifb_sgmii_reg.nifb_sgmii);
  }

  SOC_PA_IMPLICIT_FLD_GET(*fld_addr, fld_val, 10, exit);
  val = SOC_SAND_GET_BIT(fld_val, (mal_ndx % SOC_PETRA_MALS_IN_NIF_GRP));
  /* Get EGQ configuration } */

  *is_sgmii = (val)?TRUE:FALSE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_type_from_egq_get()",0,0);
}

STATIC uint32
  soc_petra_nif_spaui_defaults_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_NIF_SPAUI_INFO      *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SPAUI_DEFAULTS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_petra_PETRA_NIF_SPAUI_INFO_clear(info);

  info->enable = FALSE;

  info->ch_conf.enable = FALSE;
  info->ch_conf.bct_channel_byte_ndx = 0x1;
  info->ch_conf.bct_size = 0;
  info->ch_conf.is_burst_interleaving = FALSE;

  info->ipg.enable = FALSE;
  info->ipg.size = 0xc;
  info->ipg.dic_mode = SOC_PETRA_NIF_IPG_DIC_MODE_AVERAGE;

  info->preamble.enable = FALSE;
  info->preamble.size = 0x2;
  info->preamble.skip_SOP = FALSE;

  info->crc_mode = SOC_PETRA_NIF_CRC_MODE_32;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_spaui_defaults_get_unsafe()",0,0);
}
/*****************************************************
* NAME
*    soc_petra_nif_spaui_defaults_set_unsafe
* TYPE:
*   PROC
* DATE:
*   30/01/2008
* FUNCTION:
*   Sets default values for configurable parameters.
*   Those parameters can be set as part of SPAUI extensions
*   to XAUI standard.
*   Setting them to defaults effectively reverses the configuration
*   to XAUI-complient.
* INPUT:
*   SOC_SAND_IN  uint32                 mal_ndx -
*     MAL index. Range: 0-7.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
STATIC uint32
  soc_petra_nif_spaui_defaults_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx
  )
{
  uint32
    res;
  SOC_PETRA_NIF_SPAUI_INFO
    default_conf;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SPAUI_DEFAULTS_SET_UNSAFE);

  res = soc_petra_nif_spaui_defaults_get_unsafe(
          unit,
          &default_conf
        ) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  default_conf.enable = TRUE;
  default_conf.preamble.enable = TRUE;
  default_conf.ch_conf.enable = TRUE;
  default_conf.ipg.enable = TRUE;

  res = soc_petra_nif_spaui_conf_set_unsafe(
          unit,
          mal_ndx,
          SOC_PETRA_CONNECTION_DIRECTION_BOTH,
          &default_conf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_spaui_defaults_set_unsafe()",0,0);
}

STATIC uint32
  soc_petra_nif_mal_basic_conf_xaui_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  mal_ndx,
    SOC_SAND_IN  uint32  srd_base_lane
  )
{
  uint32
    fld_val = 0,
    reg_val = 0,
    res;
  uint32
    nif_grp_id,
    mal_in_nif_grp_id;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_ADDR
    *reg_addr;
  SOC_PETRA_SRD_STAR_ID
    star_id;
  uint32
    srd_qrtt_id,
    srd_qrtt_id_glbl,
    srd_lane_id,
    srd_instance_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_BASIC_CONF_XAUI_SET_UNSAFE);

  regs = soc_petra_regs();

  /* Revert SPAUI extensions to default values - XAUI complaint */
  res = soc_petra_nif_spaui_defaults_set_unsafe(
          unit,
          mal_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Set MAL in reset */
  reg_val = 0x0;
  SOC_PA_REG_ISET(regs->nif_mac_lane.enable_reg, reg_val, mal_ndx, 20, exit);

  /* XAUI config register { */
  reg_val = 0;

  fld_val = 0x40;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.rx_min_pkt_length, fld_val, reg_val, 30, exit);

  fld_val = 0x3c;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.tx_min_pkt_length, fld_val, reg_val, 40, exit);

  fld_val = 0xc;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.ipg_dic_threshold, fld_val, reg_val, 50, exit);

  fld_val = 0x1;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.ipg_dic_enable, fld_val, reg_val, 60, exit);

  fld_val = 0x1;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.mask_check_end, fld_val, reg_val, 70, exit);

  fld_val = 0x0;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.idle_rand_polynom, fld_val, reg_val, 80, exit);

  fld_val = 0x1;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.tx_term_pkt_on_err, fld_val, reg_val, 90, exit);

  fld_val = 0x0;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.tx_insert_e_on_err, fld_val, reg_val, 100, exit);

  fld_val = 0x0;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.lfs_ovrd_send_idle, fld_val, reg_val, 110, exit);

  fld_val = 0x0;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.lfs_ovrd_send_q, fld_val, reg_val, 120, exit);

  fld_val = 0x0;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.lfs_ovrd_en, fld_val, reg_val, 130, exit);

  SOC_PA_REG_ISET(regs->nif_mac_lane.xaui_config_reg, reg_val, mal_ndx, 140, exit);

  /* XAUI config register } */

  /* MLF config { */
  SOC_PA_REG_IGET(regs->nif_mac_lane.tx_mlfconfig_status_reg, reg_val, mal_ndx, 142, exit);
  fld_val = 0x18;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.tx_mlfconfig_status_reg.tx_almost_full_threshold, fld_val, reg_val, 143, exit);
  fld_val = 0x17;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.tx_mlfconfig_status_reg.start_tx_threshold, fld_val, reg_val, 144, exit);
  SOC_PA_REG_ISET(regs->nif_mac_lane.tx_mlfconfig_status_reg, reg_val, mal_ndx, 145, exit);

  fld_val = 0x14;
  SOC_PA_FLD_ISET(regs->nif_mac_lane.rx_mlfconfig_status_reg.rx_almost_full_threshold, fld_val, mal_ndx, 146, exit);
  /* MLF config } */

  /* disable SGMII { */
  SOC_PA_FLD_ISET(regs->nif_mac_lane.sgmii_config_reg.sgmii_mode, 0x0, mal_ndx, 148, exit);
  /* } disable SGMII */


  /* EGQ SGMII reg { */

  /* Unset if marked as SGMII */
  if (mal_ndx < SOC_PETRA_MALS_IN_NIF_GRP)
  {
    reg_addr = SOC_PA_REG_DB_ACC_REF(regs->egq.nifa_sgmii_reg.addr);
  }
  else
  {
    reg_addr = SOC_PA_REG_DB_ACC_REF(regs->egq.nifb_sgmii_reg.addr);
  }

  res = soc_petra_read_reg_unsafe(
          unit,
          reg_addr,
          SOC_PETRA_DEFAULT_INSTANCE,
          &reg_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  SOC_SAND_SET_BIT(reg_val, 0x0, (mal_ndx % SOC_PETRA_MALS_IN_NIF_GRP));

  res = soc_petra_write_reg_unsafe(
          unit,
          reg_addr,
          SOC_PETRA_DEFAULT_INSTANCE,
          reg_val
        );
  /* EGQ SGMII reg } */

  /* IRE { */
  SOC_PA_REG_GET(regs->ire.ingress_mac_type_reg, reg_val, 160, exit);
  SOC_SAND_SET_BIT(reg_val, 0x0, mal_ndx);
  SOC_PA_REG_SET(regs->ire.ingress_mac_type_reg, reg_val, 170, exit);
  /* IRE } */

  /* EPNI Flow Control { */
  /* Per-port thresholds - set to XAUI-suitable default */
  nif_grp_id        = SOC_PETRA_NIF_GRP_NDX(mal_ndx);
  mal_in_nif_grp_id = SOC_PETRA_MAL_IN_NIF_GRP_NDX(mal_ndx);
  SOC_PA_FLD_SET(regs->epni.nif_ports_fc_thresh_config_reg[nif_grp_id].nif_per_port_th[mal_in_nif_grp_id], 0x3, 175, exit);
  /* EPNI Flow Control { */

  /* SerDes { */
  star_id = SOC_PETRA_SRD_LANE2STAR(srd_base_lane);
  srd_instance_id = SOC_PETRA_SRD_STAR2INSTANCE(star_id);
  srd_qrtt_id = SOC_PETRA_SRD_LANE2QRTT_INNER(srd_base_lane);
  srd_qrtt_id_glbl = SOC_PETRA_SRD_LANE2QRTT_GLBL(srd_base_lane);

  res = soc_petra_srd_qrtt_sync_fifo_en(
          unit,
          srd_qrtt_id_glbl,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 177, exit);

  for (srd_lane_id = 0; srd_lane_id < SOC_PETRA_SRD_NOF_LANES_PER_QRTT; srd_lane_id++)
  {
    SOC_PA_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_align_en_req, 0x1, srd_instance_id, 180, exit);
    SOC_PA_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].rx_nearest_comma_align, 0x1, srd_instance_id, 182, exit);
    SOC_PA_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_comma_det_mask_lsb, 0x1, srd_instance_id, 184, exit);
    SOC_PA_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_comma_det_mask_msb, 0x1, srd_instance_id, 186, exit);
    SOC_PA_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].tx_use_ext_byte_toggle, 0x1, srd_instance_id, 188, exit);
  }
  /* SerDes { */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_basic_conf_xaui_set_unsafe()",0,0);
}

STATIC uint32
  soc_petra_nif_mal_basic_conf_sgmii_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx
  )
{
  uint32
    fld_val = 0,
    reg_val = 0,
    res;
  uint32
    nif_grp_id,
    mal_in_nif_grp_id,
    srd_base_lane,
    idx;
  SOC_PETRA_REG_FIELD
    *fld_addr;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_SRD_STAR_ID
    star_id;
  uint32
    srd_qrtt_id,
    srd_qrtt_id_glbl,
    srd_lane_id,
    srd_instance_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_BASIC_CONF_SGMII_SET_UNSAFE);

  regs = soc_petra_regs();

  /* Revert SPAUI extensions to default values - XAUI complaint */
  res = soc_petra_nif_spaui_defaults_set_unsafe(
          unit,
          mal_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Zero MAL enable register */
  reg_val = 0;
  SOC_PA_REG_ISET(regs->nif_mac_lane.enable_reg, reg_val, mal_ndx, 20, exit);

  /* Configure EGQ { */
  if (mal_ndx < SOC_PETRA_MALS_IN_NIF_GRP)
  {
    fld_addr = SOC_PA_REG_DB_ACC_REF(regs->egq.nifa_sgmii_reg.nifa_sgmii);
  }
  else
  {
    fld_addr = SOC_PA_REG_DB_ACC_REF(regs->egq.nifb_sgmii_reg.nifb_sgmii);
  }

  SOC_PA_IMPLICIT_FLD_GET(*fld_addr, fld_val, 30, exit);

  SOC_SAND_SET_BIT(fld_val, 0x1, (mal_ndx % SOC_PETRA_MALS_IN_NIF_GRP));

  SOC_PA_IMPLICIT_FLD_SET(*fld_addr, fld_val, 35, exit);
  /* Configure EGQ } */

  /* SGMII defaults { */

  reg_val = 0;

  fld_val = 0x1;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.sgmii_config_reg.sgmii_mode, fld_val, reg_val, 40, exit);

  fld_val = 0x1;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.sgmii_config_reg.sgmii_rx_crc_check_en, fld_val, reg_val, 50, exit);

  fld_val = 0x0;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.sgmii_config_reg.sgmii_c_mode, fld_val, reg_val, 60, exit);

  fld_val = 0x40;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.sgmii_config_reg.sgmii_rx_min_pkt_length, fld_val, reg_val, 70, exit);

  fld_val = 0x3c;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.sgmii_config_reg.sgmii_tx_min_pkt_length, fld_val, reg_val, 80, exit);

  fld_val = 0xc;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.sgmii_config_reg.sgmii_tx_ipg_length, fld_val, reg_val, 90, exit);

  SOC_PA_REG_ISET(regs->nif_mac_lane.sgmii_config_reg, reg_val, mal_ndx, 100, exit);

  reg_val = 0;

  fld_val = 0x2;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.sgmii_config2_reg.prefetch_const, fld_val, reg_val, 110, exit);

  fld_val = 0x0;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.sgmii_config2_reg.early_sop_prefetch_const, fld_val, reg_val, 120, exit);

  fld_val = 0x0;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.sgmii_config2_reg.tx_max_credits, fld_val, reg_val, 130, exit);

  fld_val = 0x1;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.sgmii_config2_reg.tx_early_fc_release, fld_val, reg_val, 140, exit);

  fld_val = 0x0;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.sgmii_config2_reg.fct_large_delay, fld_val, reg_val, 150, exit);

  fld_val = 0x7;
  for (idx = 0; idx < SOC_PETRA_MAX_NIFS_PER_MAL; idx++)
  {
    /* idx is a lane index in MAL */
    SOC_PA_FLD_TO_REG(regs->nif_mac_lane.sgmii_config2_reg.an_xmit_ovrd_l[idx], fld_val, reg_val, 160, exit);
  }

  SOC_PA_REG_ISET(regs->nif_mac_lane.sgmii_config2_reg, reg_val, mal_ndx, 200, exit);

   /* MLF config { */
  SOC_PA_REG_IGET(regs->nif_mac_lane.tx_mlfconfig_status_reg, reg_val, mal_ndx, 142, exit);
  fld_val = 0x4;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.tx_mlfconfig_status_reg.tx_almost_full_threshold, fld_val, reg_val, 143, exit);
  fld_val = 0x3;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.tx_mlfconfig_status_reg.start_tx_threshold, fld_val, reg_val, 144, exit);
  SOC_PA_REG_ISET(regs->nif_mac_lane.tx_mlfconfig_status_reg, reg_val, mal_ndx, 145, exit);

  fld_val = 0x5;
  SOC_PA_FLD_ISET(regs->nif_mac_lane.rx_mlfconfig_status_reg.rx_almost_full_threshold, fld_val, mal_ndx, 146, exit);
  /* MLF config } */

  /* SGMII defaults } */

  /* IRE { */
  SOC_PA_REG_GET(regs->ire.ingress_mac_type_reg, reg_val, 210, exit);
  SOC_SAND_SET_BIT(reg_val, 0x1, mal_ndx);

  SOC_PA_REG_SET(regs->ire.ingress_mac_type_reg, reg_val, 220, exit);
  /* IRE } */

  /* SerDes { */
  /*
   *  SerDes plane index - NIF-A/NIF-B.
   *  This is the same index as NIF Group index
   */
  star_id = SOC_PETRA_NIF_GRP_NDX(mal_ndx);
  srd_instance_id = SOC_PETRA_SRD_STAR2INSTANCE(star_id);
  srd_qrtt_id = SOC_PETRA_SRD_QRTT2INNER_ID(mal_ndx);
  srd_base_lane = SOC_PETRA_SRD_LANE2GLOBAL_ID(star_id, srd_qrtt_id, 0);
  srd_qrtt_id_glbl = SOC_PETRA_SRD_LANE2QRTT_GLBL(srd_base_lane);

  /* EPNI Flow Control { */
  /* Per-port thresholds - set to XAUI-suitable default */
  nif_grp_id        = SOC_PETRA_NIF_GRP_NDX(mal_ndx);
  mal_in_nif_grp_id = SOC_PETRA_MAL_IN_NIF_GRP_NDX(mal_ndx);
  SOC_PA_FLD_SET(regs->epni.nif_ports_fc_thresh_config_reg[nif_grp_id].nif_per_port_th[mal_in_nif_grp_id], 0x1, 175, exit);
  /* EPNI Flow Control { */

  res = soc_petra_srd_qrtt_sync_fifo_en(
          unit,
          srd_qrtt_id_glbl,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  for (srd_lane_id = 0; srd_lane_id < SOC_PETRA_SRD_NOF_LANES_PER_QRTT; srd_lane_id++)
  {
    SOC_PA_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_align_en_req, 0x0, srd_instance_id, 94, exit);
    SOC_PA_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].rx_nearest_comma_align, 0x0, srd_instance_id, 96, exit);
    SOC_PA_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_comma_det_mask_lsb, 0x0, srd_instance_id, 98, exit);
    SOC_PA_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_comma_det_mask_msb, 0x0, srd_instance_id, 100, exit);
    SOC_PA_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].tx_use_ext_byte_toggle, 0x0, srd_instance_id, 110, exit);
  }
  /* SerDes { */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_basic_conf_sgmii_set_unsafe()",0,0);
}


/*********************************************************************
*     Sets basic NIF configuration. After setting basic
*     configuration, per-type configuration must be set before
*     enabling the MAL. Basic NIF configuration includes the
*     following:- MAL to SerDes mapping- Setting type-specific
*     defaults- Setting interface rate
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mal_basic_conf_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_INFO            *nif_info
  )
{
  uint32
    fld_val,
    reg_val,
    res;
  SOC_PETRA_REGS
    *regs;
  uint8
    is_combo_fabric;
  SOC_PETRA_SRD_STAR_ID
    star_id;
  uint32
    srd_qrtt_id,
    srd_instance_id,
    srd_nof_lanes,
    srd_base_lane;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_BASIC_CONF_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(nif_info);

  regs = soc_petra_regs();

  /* SerDes { */
  soc_petra_nif_srd_lanes_internal(
    unit,
    mal_ndx,
    nif_info,
    &srd_nof_lanes,
    &srd_base_lane
  );

  star_id = SOC_PETRA_SRD_LANE2STAR(srd_base_lane);
  srd_instance_id = SOC_PETRA_SRD_STAR2INSTANCE(star_id);
  srd_qrtt_id = SOC_PETRA_SRD_LANE2QRTT_INNER(srd_base_lane);

  is_combo_fabric = soc_petra_is_fabric_quartet(
                       unit,
                       srd_qrtt_id
                     );

  /* SerDes rate (single/double) { */
  if (
      (SOC_PETRA_NIF_IS_LAST_MAL(mal_ndx)) &&
      (is_combo_fabric == TRUE) &&
      (nif_info->serdes_rate == SOC_PETRA_NIF_SERDES_RATE_SINGLE)
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_MAL_IS_FABRIC_NOT_NIF_ERR, 87, exit);
  }

  fld_val = (nif_info->serdes_rate == SOC_PETRA_NIF_SERDES_RATE_SINGLE)?0x0:0x1;

  SOC_PA_FLD_ISET(regs->serdes.srd_cmu_cfga_reg[srd_qrtt_id].ln_20b_mode, fld_val, srd_instance_id, 160, exit);

  /* SerDes rate (single/double) } */
  /* SerDes } */

  switch(nif_info->type)
  {
  case SOC_PETRA_NIF_TYPE_XAUI:
    res = soc_petra_nif_mal_basic_conf_xaui_set_unsafe(
            unit,
            mal_ndx,
            srd_base_lane
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* enable XAUI in both RX/TX { */
    SOC_PA_REG_IGET(regs->nif_mac_lane.enable_reg, reg_val, mal_ndx, 20, exit);

    fld_val = 0x1;
    SOC_PA_FLD_TO_REG(regs->nif_mac_lane.enable_reg.xaui_rx_lane_en, fld_val, reg_val, 30, exit);
    SOC_PA_FLD_TO_REG(regs->nif_mac_lane.enable_reg.xaui_tx_lane_en, fld_val, reg_val, 40, exit);

    SOC_PA_REG_ISET(regs->nif_mac_lane.enable_reg, reg_val, mal_ndx, 50, exit);

    /* enable XAUI in both RX/TX } */
    break;
  case SOC_PETRA_NIF_TYPE_SPAUI:
    res = soc_petra_nif_mal_basic_conf_xaui_set_unsafe(
            unit,
            mal_ndx,
            srd_base_lane
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    break;
  case SOC_PETRA_NIF_TYPE_SGMII:
    res = soc_petra_nif_mal_basic_conf_sgmii_set_unsafe(
            unit,
            mal_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INVALID_TYPE_ERR, 80, exit);
      break;
  }

  /* Disable FC refresh timer */
  SOC_PA_FLD_ISET(regs->nif_mac_lane.tx_flow_control_timer_config_reg.fct_timer_en, 0x0, mal_ndx, 85, exit);

  /* Swap { */
  SOC_PA_REG_IGET(regs->nif_mac_lane.bom_reg, reg_val, mal_ndx, 100, exit);

  fld_val = (nif_info->lanes_swap.swap_rx == TRUE)?0x1:0x0;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.bom_reg.rx_swap_ll, fld_val, reg_val, 110, exit);

  if (nif_info->serdes_rate == SOC_PETRA_NIF_SERDES_RATE_DOUBLE)
  {
    /* For double rate - swap pairs also */
    SOC_PA_FLD_TO_REG(regs->nif_mac_lane.bom_reg.rx_swap_ll_pair, fld_val, reg_val, 120, exit);
  }
  else
  {
    fld_val = 0x0;
    SOC_PA_FLD_TO_REG(regs->nif_mac_lane.bom_reg.rx_swap_ll_pair, fld_val, reg_val, 122, exit);
  }

  fld_val = (nif_info->lanes_swap.swap_tx == TRUE)?0x1:0x0;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.bom_reg.tx_swap_ll, fld_val, reg_val, 124, exit);

  if (nif_info->serdes_rate == SOC_PETRA_NIF_SERDES_RATE_DOUBLE)
  {
    /* For double rate - swap pairs also */
    SOC_PA_FLD_TO_REG(regs->nif_mac_lane.bom_reg.tx_swap_ll_pair, fld_val, reg_val, 126, exit);
  }
  else
  {
    fld_val = 0x0;
    SOC_PA_FLD_TO_REG(regs->nif_mac_lane.bom_reg.tx_swap_ll_pair, fld_val, reg_val, 128, exit);
  }

  SOC_PA_REG_ISET(regs->nif_mac_lane.bom_reg, reg_val, mal_ndx, 150, exit);
  /* Swap } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_basic_conf_set_unsafe()",mal_ndx,0);
}

/*********************************************************************
*     Sets basic NIF configuration. After setting basic
*     configuration, per-type configuration must be set before
*     enabling the MAL. Basic NIF configuration includes the
*     following:- MAL to SerDes mapping- Setting type-specific
*     defaults- Setting interface rate
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mal_basic_conf_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_INFO            *nif_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_BASIC_CONF_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(nif_info);
  SOC_SAND_MAGIC_NUM_VERIFY(nif_info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    mal_ndx, SOC_PETRA_NOF_MAC_LANES-1,
    SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  if ( (nif_info->type == SOC_PETRA_NIF_TYPE_SGMII) &&
       (nif_info->serdes_rate == SOC_PETRA_NIF_SERDES_RATE_DOUBLE) )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_SGMII_INTERFACE_NOF_SINGLE_RATE_ERR, 20, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(
    nif_info->type, SOC_PETRA_NIF_NOF_TYPES-1,
    SOC_PETRA_NIF_INVALID_TYPE_ERR, 30, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    nif_info->serdes_rate, SOC_PETRA_NIF_NOF_SERDES_RATES-1,
    SOC_PETRA_NIF_SRD_RATE_OUT_OF_RANGE_ERR, 40, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_basic_conf_verify()",0,0);
}

/*********************************************************************
*     Sets basic NIF configuration. After setting basic
*     configuration, per-type configuration must be set before
*     enabling the MAL. Basic NIF configuration includes the
*     following:- MAL to SerDes mapping- Setting type-specific
*     defaults- Setting interface rate
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mal_basic_conf_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_INFO            *nif_info
  )
{
  uint32
    fld_val = 0,
    reg_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;
  uint8
    is_xaui_enabled = FALSE,
    is_sgmii_enabled = FALSE,
    is_sgmii_if = FALSE,
    is_type_unknown = FALSE;
  SOC_PETRA_NIF_SPAUI_INFO
    spaui_rx_info,
    spaui_tx_info,
    spaui_defaults;
  SOC_PETRA_SRD_STAR_ID
    star_id;
  uint32
    srd_qrtt_id,
    srd_instance_id,
    srd_nof_lanes,
    srd_base_lane;
  uint8
    is_same = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_BASIC_CONF_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(nif_info);

  regs = soc_petra_regs();

  soc_petra_PETRA_NIF_SPAUI_INFO_clear(&spaui_rx_info);
  soc_petra_PETRA_NIF_SPAUI_INFO_clear(&spaui_tx_info);

  res = soc_petra_nif_is_sgmii_get(
          unit,
          mal_ndx,
          &is_sgmii_if
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Read XAUI enable configuration.
   * If XAUI is enabled at least in one direction, the
   * interface is considered XAUI or SPAUI (not SGMII).
   */
  SOC_PA_REG_IGET(regs->nif_mac_lane.enable_reg, reg_val, mal_ndx, 20, exit);

  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.enable_reg.xaui_rx_lane_en, fld_val, reg_val, 30, exit);

  if (fld_val != 0)
  {
    is_xaui_enabled = TRUE;
  }
  else
  {
    SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.enable_reg.xaui_tx_lane_en, fld_val, reg_val, 40, exit);

    if (fld_val != 0)
    {
      is_xaui_enabled = TRUE;
    }
    else
    {
      is_xaui_enabled = FALSE;
    }
  }

  /*
   * Read SGMII enable configuration.
   * If SGMII is enabled at least in one direction, the
   * interface is considered SGMII.
   */
  fld_val = 0;
  SOC_PA_FLD_IGET(regs->nif_mac_lane.enable_reg.sgmii_rx_lane_en, fld_val, mal_ndx, 50, exit);

  if (fld_val == 0)
  {
     SOC_PA_FLD_IGET(regs->nif_mac_lane.enable_reg.sgmii_tx_lane_en, fld_val, mal_ndx, 60, exit);

     if (fld_val == 0 )
     {
       is_sgmii_enabled = FALSE;
     }
     else
     {
       is_sgmii_enabled = TRUE;
     }
  }
  else
  {
    is_sgmii_enabled = TRUE;
  }

  /* Verify that SGMII and XAUI are not both enabled */
  if ((is_sgmii_enabled) && (is_xaui_enabled))
  {
    is_type_unknown = TRUE;
  }

  if (!is_sgmii_if)
  {
    /* XAUI/SPAUI - rx */
    res = soc_petra_nif_spaui_conf_get_unsafe(
            unit,
            mal_ndx,
            &spaui_rx_info,
            &spaui_tx_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    spaui_rx_info.enable = FALSE;

    res = soc_petra_nif_spaui_defaults_get_unsafe(
            unit,
            &spaui_defaults
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    if (!is_type_unknown)
    {
      SOC_PETRA_COMP(&spaui_defaults, &spaui_rx_info, SOC_PETRA_NIF_SPAUI_INFO, 1, is_same);

      /* SPAUI defaults are XAUI, claim SPAUI if any changes were made */
      nif_info->type = (is_same == TRUE)?SOC_PETRA_NIF_TYPE_XAUI:SOC_PETRA_NIF_TYPE_SPAUI;
    }
    if (is_same)
    {
      /* XAUI/SPAUI - tx */
      spaui_tx_info.enable = FALSE;

      res = soc_petra_nif_spaui_defaults_get_unsafe(
              unit,
              &spaui_defaults
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 82, exit);

      if (!is_type_unknown)
      {
        SOC_PETRA_COMP(&spaui_defaults, &spaui_tx_info, SOC_PETRA_NIF_SPAUI_INFO, 1, is_same);

        /* SPAUI defaults are XAUI, claim SPAUI if any changes were made */
        nif_info->type = (is_same == TRUE)?SOC_PETRA_NIF_TYPE_XAUI:SOC_PETRA_NIF_TYPE_SPAUI;
      }
    }
  }
  else
  {
    nif_info->type = SOC_PETRA_NIF_TYPE_SGMII;
  }

  if (is_type_unknown)
  {
    /* Servers as indication of "unknown" interface type */
    nif_info->type = SOC_PETRA_NIF_NOF_TYPES;
  }

  /* SerDes rate { */
  soc_petra_nif_srd_lanes_internal(
    unit,
    mal_ndx,
    nif_info,
    &srd_nof_lanes,
    &srd_base_lane
  );

  star_id = SOC_PETRA_SRD_LANE2STAR(srd_base_lane);
  srd_instance_id = SOC_PETRA_SRD_STAR2INSTANCE(star_id);
  srd_qrtt_id = SOC_PETRA_SRD_LANE2QRTT_INNER(srd_base_lane);

  SOC_PA_FLD_IGET(regs->serdes.srd_cmu_cfga_reg[srd_qrtt_id].ln_20b_mode, fld_val, srd_instance_id, 90, exit);
  nif_info->serdes_rate = (fld_val == 0x0)?SOC_PETRA_NIF_SERDES_RATE_SINGLE:SOC_PETRA_NIF_SERDES_RATE_DOUBLE;

  /* } */

  /* Swap { */
  SOC_PA_REG_IGET(regs->nif_mac_lane.bom_reg, reg_val, mal_ndx, 100, exit);

  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.bom_reg.rx_swap_ll, fld_val, reg_val, 110, exit);
  nif_info->lanes_swap.swap_rx = (fld_val == 0x1)?TRUE:FALSE;

  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.bom_reg.tx_swap_ll, fld_val, reg_val, 120, exit);
  nif_info->lanes_swap.swap_tx = (fld_val == 0x1)?TRUE:FALSE;
  /* Swap } */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_basic_conf_get_unsafe()",0,0);
}

uint32
  soc_petra_nif_mal_basic_conf_verify_all(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_NIF_INFO            nif_info[SOC_PETRA_NOF_MAC_LANES]
  )
{
  uint32
    res;
  uint32
    mal_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_BASIC_CONF_VERIFY_ALL);

  for (mal_idx = 0; mal_idx < SOC_PETRA_NOF_MAC_LANES; mal_idx++)
  {
    res = soc_petra_nif_mal_basic_conf_set_unsafe(
            unit,
            mal_idx,
            &(nif_info[mal_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_basic_conf_verify_all()",0,0);
}

uint32
  soc_petra_nif_mal_basic_conf_set_all_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_NIF_INFO            nif_info[SOC_PETRA_NOF_MAC_LANES]
  )
{
  uint32
    res;
  uint32
    mal_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_BASIC_CONF_SET_ALL_UNSAFE);

  for (mal_idx = 0; mal_idx < SOC_PETRA_NOF_MAC_LANES; mal_idx++)
  {
    res = soc_petra_nif_mal_basic_conf_set_unsafe(
            unit,
            mal_idx,
            &(nif_info[mal_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_basic_conf_set_all_unsafe()",0,0);
}


uint32
  soc_petra_nif_mal_basic_conf_get_all_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT  SOC_PETRA_NIF_INFO           nif_info[SOC_PETRA_NOF_MAC_LANES]
  )
{
  uint32
    res;
  uint32
    mal_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_BASIC_CONF_GET_ALL_UNSAFE);

  for (mal_idx = 0; mal_idx < SOC_PETRA_NOF_MAC_LANES; mal_idx++)
  {
    res = soc_petra_nif_mal_basic_conf_get_unsafe(
            unit,
            mal_idx,
            &(nif_info[mal_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_basic_conf_get_all_unsafe()",0,0);
}


/*********************************************************************
*     Validates NIF to SerDes mapping topology configuration.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_topology_validate_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
   uint32
    fld_val,
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    mal_c_ndx = SOC_PETRA_MAX_NIFS_PER_MAL - 2,
    mal_d_ndx = SOC_PETRA_MAX_NIFS_PER_MAL - 1,
    nif_grp_ndx;
  uint8
    mal_c_enabled = FALSE,
    mal_d_enabled = FALSE,
    mal_c_double_rate,
    mal_d_double_rate,
    is_combo_nif,
    is_low_sim_active;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_TOPOLOGY_VALIDATE_UNSAFE);

  regs = soc_petra_regs();

#ifndef SAND_LOW_LEVEL_SIMULATION
  is_low_sim_active = FALSE;
#else
  is_low_sim_active = soc_sand_low_is_sim_active_get();
#endif

  for (nif_grp_ndx = 0; nif_grp_ndx < SOC_PETRA_NOF_NIF_GRPS; nif_grp_ndx++)
  {
    SOC_PA_FLD_IGET(regs->nif.nif_config_reg.config_err, fld_val, nif_grp_ndx, 10, exit);

    if (fld_val != 0x0)
    {
      /* Check hardware status for invalid indication */
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_MAL_INVALID_CONFIG_STATUS_ON_ERR, 20, exit);
    }

    /*
     * If MAL-s C and D are enabled, using only 3 SerDes macros -
     * make sure the SerDes-s in use don't overlap
     * }
     */
    res = soc_petra_nif_mal_enable_get_unsafe(
            unit,
            mal_c_ndx,
            &mal_c_enabled
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_petra_nif_mal_enable_get_unsafe(
            unit,
            mal_d_ndx,
            &mal_d_enabled
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    if (mal_c_enabled && mal_d_enabled)
    {
      SOC_PA_FLD_IGET(regs->nif.nif_config_reg.serdes_g4_en, fld_val, nif_grp_ndx, 50, exit);

      is_combo_nif = SOC_SAND_NUM2BOOL(fld_val);

      if (!is_combo_nif)
      {
        /* Is MAL C double rate? */
        SOC_PA_FLD_IGET(regs->nif_mac_lane.bom_reg.srd_dr_mode_ind, fld_val, mal_c_ndx, 60, exit);

        mal_c_double_rate = SOC_SAND_NUM2BOOL(fld_val);

        /* Is MAL D double rate? */
        SOC_PA_FLD_IGET(regs->nif_mac_lane.bom_reg.srd_dr_mode_ind, fld_val, mal_d_ndx, 70, exit);

        mal_d_double_rate = SOC_SAND_NUM2BOOL(fld_val);

        if (!is_low_sim_active)
        {
          if(!(mal_c_double_rate && mal_d_double_rate))
          {
            SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_TWO_LAST_MALS_SERDES_OVERLAP_ERR, 80, exit);
          }
        }
      }
    }
    /* MAL C and D overlap } */
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_topology_validate_unsafe()",0,0);
}

/*********************************************************************
*     Enables/disable specified MAL.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mal_enable_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  uint8                 is_enabled
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_ENABLE_SET_UNSAFE);

  regs = soc_petra_regs();

  SOC_PA_FLD_ISET(regs->nif_mac_lane.enable_reg.mal_en, 0x0, mal_ndx, 10, exit);

  if (is_enabled)
  {
    SOC_PA_FLD_ISET(regs->nif_mac_lane.enable_reg.mal_en, 0x1, mal_ndx, 20, exit);
    SOC_PA_FLD_ISET(regs->nif_mac_lane.statistics_reg.stat_tx_init, 0xf, mal_ndx, 30, exit);
    SOC_PA_FLD_ISET(regs->nif_mac_lane.statistics_reg.stat_rx_init, 0xf, mal_ndx, 40, exit);
    soc_sand_os_nano_sleep(SOC_PETRA_NIF_WAIT_NS,NULL);
    SOC_PA_FLD_ISET(regs->nif_mac_lane.statistics_reg.stat_tx_init, 0x0, mal_ndx, 50, exit);
    SOC_PA_FLD_ISET(regs->nif_mac_lane.statistics_reg.stat_rx_init, 0x0, mal_ndx, 60, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_enable_set_unsafe()",mal_ndx,0);
}

/*********************************************************************
*     Enables/disable specified MAL.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mal_enable_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  uint8                 is_enabled
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_ENABLE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    mal_ndx, SOC_PETRA_NOF_MAC_LANES-1,
    SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_enable_verify()",0,0);
}

/*********************************************************************
*     Enables/disable specified MAL.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mal_enable_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT uint8                 *is_enabled
  )
{
  uint32
    fld_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MAL_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_enabled);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    mal_ndx, SOC_PETRA_NOF_MAC_LANES-1,
    SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  regs = soc_petra_regs();

  SOC_PA_FLD_IGET(regs->nif_mac_lane.enable_reg.mal_en, fld_val, mal_ndx, 10, exit);

  *is_enabled = (fld_val?TRUE:FALSE);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mal_enable_get_unsafe()",0,0);
}


/*********************************************************************
*     Sets SGMII/1000BASE-X interface configuration
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_sgmii_conf_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID          nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION  direction_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SGMII_INFO        *info
  )
{
  uint32
    fld_val,
    reg_val,
    val,
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    mal_ndx = 0,
    lane_ndx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SGMII_CONF_SET_UNSAFE);

  regs = soc_petra_regs();

  mal_ndx = SOC_PETRA_NIF2MAL_NDX(nif_ndx);
  lane_ndx= SOC_PETRA_NIF_SGMII_LANE(nif_ndx);

  /* Disable SGMII lane during configuration change { */
  res = soc_petra_nif_sgmii_enable_state_set(
          unit,
          nif_ndx,
          direction_ndx,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (info->enable)
  {
    /* Set other fields if enabled */

    /* External PHY {*/

    val = (info->mode == SOC_PETRA_NIF_1GE_MODE_SGMII)?0x1:0x0;

    SOC_PA_FLD_IGET(regs->nif_mac_lane.sgmii_config_reg.sgmii_c_mode, fld_val, mal_ndx, 12, exit);

    SOC_SAND_SET_BIT(fld_val, val, lane_ndx);

    SOC_PA_FLD_ISET(regs->nif_mac_lane.sgmii_config_reg.sgmii_c_mode, fld_val, mal_ndx, 14, exit);
    /* External PHY }*/
  }

  /* AutoNeg Enable { */
  SOC_PA_REG_IGET(regs->nif_mac_lane.auto_neg_config_reg, reg_val, mal_ndx, 16, exit);

  val = SOC_SAND_BOOL2NUM(info->is_autoneg);
  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.auto_neg_config_reg.enable_auto_neg, fld_val, reg_val, 20, exit);
  SOC_SAND_SET_BIT(fld_val, val, lane_ndx);
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.auto_neg_config_reg.enable_auto_neg, fld_val, reg_val, 30, exit);

  /* LinkSpeedOvrdEn  - always set*/
  val = 0x1;
  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.auto_neg_config_reg.link_speed_ovrd_en, fld_val, reg_val, 40, exit);
  SOC_SAND_SET_BIT(fld_val, val, lane_ndx);
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.auto_neg_config_reg.link_speed_ovrd_en, fld_val, reg_val, 50, exit);

  SOC_PA_REG_ISET(regs->nif_mac_lane.auto_neg_config_reg, reg_val, mal_ndx, 60, exit);

  /* xmit override */
  fld_val = (info->is_autoneg == TRUE)?0x7:0x4;
  SOC_PA_FLD_ISET(regs->nif_mac_lane.sgmii_config2_reg.an_xmit_ovrd_l[lane_ndx], fld_val, mal_ndx, 70, exit);
  /* AutoNeg Enable } */

  res = soc_petra_nif_sgmii_rate_set_unsafe(
          unit,
          nif_ndx,
          info->rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /* Enable SGMII if requested { */

  if (info->enable)
  {
     res = soc_petra_nif_sgmii_enable_state_set(
            unit,
            nif_ndx,
            direction_ndx,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_sgmii_conf_set_unsafe()",nif_ndx,0);
}

/*********************************************************************
*     Sets SGMII/1000BASE-X interface configuration
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_sgmii_conf_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID           nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION   direction_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SGMII_INFO         *info
  )
{
  uint32
    res;
  uint32
    mal_idx;
  uint8
    is_sgmii_mal;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SGMII_CONF_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  mal_idx = SOC_PETRA_NIF2MAL_NDX(nif_ndx);

  res = soc_petra_nif_is_sgmii_get(
          unit,
          mal_idx,
          &is_sgmii_mal
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (!is_sgmii_mal)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_MAL_SGMII_CONF_MISMATCH_ERR, 7, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(
    nif_ndx, SOC_PETRA_IF_NOF_NIFS-1,
    SOC_PETRA_NIF_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, SOC_PETRA_NOF_CONNECTION_DIRECTIONS-1,
    SOC_PETRA_NIF_DIRECTION_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_sgmii_conf_verify()",nif_ndx,direction_ndx);
}

/*********************************************************************
*     Sets SGMII/1000BASE-X interface configuration
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_sgmii_conf_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID           nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_INFO         *info_rx,
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_INFO         *info_tx
  )
{
  uint32
    reg_val,
    fld_val,
    val,
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    mal_ndx = 0,
    lane_ndx = 0;
  SOC_PETRA_REG_FIELD
    *rx_fld,
    *tx_fld;
  SOC_PETRA_NIF_SGMII_INFO
    info_local_rx,
    info_local_tx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SGMII_CONF_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    nif_ndx, SOC_PETRA_IF_NOF_NIFS-1,
    SOC_PETRA_NIF_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  soc_petra_PETRA_NIF_SGMII_INFO_clear(&info_local_rx);
  soc_petra_PETRA_NIF_SGMII_INFO_clear(&info_local_tx);

  regs = soc_petra_regs();

  mal_ndx = SOC_PETRA_NIF2MAL_NDX(nif_ndx);
  lane_ndx= SOC_PETRA_NIF_SGMII_LANE(nif_ndx);

  /* Enable/disable SGMII for rx */

  rx_fld = SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.enable_reg.sgmii_rx_lane_en);

  SOC_PA_IMPLICIT_FLD_IGET(*rx_fld, fld_val, mal_ndx, 15, exit);

  val = SOC_SAND_GET_BIT(fld_val, lane_ndx);

  info_local_rx.enable = val?TRUE:FALSE;
  /* Enable/disable SGMII } */

  /* Enable/disable SGMII for tx */

  tx_fld = SOC_PA_REG_DB_ACC_REF(regs->nif_mac_lane.enable_reg.sgmii_tx_lane_en);

  SOC_PA_IMPLICIT_FLD_IGET(*tx_fld, fld_val, mal_ndx, 20, exit);

  val = SOC_SAND_GET_BIT(fld_val, lane_ndx);

  info_local_tx.enable = val?TRUE:FALSE;
  /* Enable/disable SGMII } */

  /* External PHY { */
  SOC_PA_FLD_IGET(regs->nif_mac_lane.sgmii_config_reg.sgmii_c_mode, fld_val, mal_ndx, 25, exit);

  val = SOC_SAND_GET_BIT(fld_val, lane_ndx);

  info_local_rx.mode = (val == 0x1)?SOC_PETRA_NIF_1GE_MODE_SGMII:SOC_PETRA_NIF_1GE_MODE_1000BASE_X;
  info_local_tx.mode = info_local_rx.mode;
  /* External PHY } */

  res = soc_petra_nif_sgmii_rate_get_unsafe(
          unit,
          nif_ndx,
          &(info_local_rx.rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  info_local_tx.rate = info_local_rx.rate;

  /* AutoNeg Enable { */
  SOC_PA_REG_IGET(regs->nif_mac_lane.auto_neg_config_reg, reg_val, mal_ndx, 35, exit);
  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.auto_neg_config_reg.enable_auto_neg, fld_val, reg_val, 40, exit);
  val = SOC_SAND_GET_BIT(fld_val, lane_ndx);
  info_local_rx.is_autoneg = SOC_SAND_NUM2BOOL(val);
  info_local_tx.is_autoneg = info_local_rx.is_autoneg;

  SOC_PETRA_COPY(info_rx, &info_local_rx, SOC_PETRA_NIF_SGMII_INFO, 1);
  SOC_PETRA_COPY(info_tx, &info_local_tx, SOC_PETRA_NIF_SGMII_INFO, 1);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_sgmii_conf_get_unsafe()",nif_ndx,0);
}

/*********************************************************************
*     Gets SGMII/1000BASE-X interface diagnostics status
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_sgmii_status_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_STAT      *status
  )
{
  uint32
    fld_val,
    reg_val,
    res;
  uint32
    mal_ndx = 0,
    lane_ndx = 0;
  uint8
    is_autoneg;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SGMII_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(status);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    nif_ndx, SOC_PETRA_IF_NOF_NIFS-1,
    SOC_PETRA_NIF_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  regs = soc_petra_regs();

  mal_ndx = SOC_PETRA_NIF2MAL_NDX(nif_ndx);
  lane_ndx= SOC_PETRA_NIF_SGMII_LANE(nif_ndx);

  soc_petra_PETRA_NIF_SGMII_STAT_clear(status);

  SOC_PA_REG_IGET(regs->nif_mac_lane.auto_neg_status_reg, reg_val, mal_ndx, 10, exit);

  SOC_PA_FLD_IGET(regs->nif_mac_lane.auto_neg_config_reg.enable_auto_neg, fld_val, mal_ndx, 5, exit);
  is_autoneg = SOC_SAND_NUM2BOOL(fld_val);

  if (is_autoneg)
  {
    SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.auto_neg_status_reg.auto_neg_complete, fld_val, reg_val, 20, exit);
    status->autoneg_not_complete = SOC_SAND_BOOL2NUM_INVERSE(fld_val);
  }


  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.auto_neg_status_reg.got_remote_fault, fld_val, reg_val, 30, exit);
  status->remote_fault = SOC_SAND_NUM2BOOL(fld_val);

  if (status->remote_fault)
  {
    SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.auto_neg_status_reg.remote_fault_type_l[lane_ndx], fld_val, reg_val, 40, exit);
    status->fault_type = (SOC_PETRA_NIF_SGMII_RFLT)fld_val;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_sgmii_status_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets SGMII/1000BASE-X interface rate - autonegotiation
*     mode, or explicit.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_sgmii_rate_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SGMII_RATE      rate
  )
{
  uint32
    fld_val,
    res;
  uint32
    mal_ndx = 0,
    lane_ndx = 0;
  SOC_PETRA_REGS
    *regs;
  uint8
    is_enabled_tx,
    is_enabled_rx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SGMII_RATE_SET_UNSAFE);

  regs = soc_petra_regs();

  mal_ndx = SOC_PETRA_NIF2MAL_NDX(nif_ndx);
  lane_ndx= SOC_PETRA_NIF_SGMII_LANE(nif_ndx);

  res = soc_petra_nif_sgmii_enable_state_get(
          unit,
          nif_ndx,
          &is_enabled_rx,
          &is_enabled_tx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  res = soc_petra_nif_sgmii_enable_state_set(
          unit,
          nif_ndx,
          SOC_PETRA_CONNECTION_DIRECTION_BOTH,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

  /*
   *  Configure explicit rate - also if AutoNeg is enabled
   */

  /*  0x0 is SOC_PETRA_NIF_SGMII_RATE_10Mbps */
  fld_val = rate;
  SOC_PA_FLD_ISET(regs->nif_mac_lane.auto_neg_config_reg.link_speed_l[lane_ndx], fld_val, mal_ndx, 60, exit);

  if (is_enabled_rx == is_enabled_tx)
  {
    /* Revert SGMII enable field - both */
    res = soc_petra_nif_sgmii_enable_state_set(
            unit,
            nif_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_BOTH,
            is_enabled_rx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }
  else
  {
    /* Revert SGMII enable field - rx */
    res = soc_petra_nif_sgmii_enable_state_set(
            unit,
            nif_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_RX,
            is_enabled_rx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    /* Revert SGMII enable field - tx */
    res = soc_petra_nif_sgmii_enable_state_set(
            unit,
            nif_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_TX,
            is_enabled_tx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_sgmii_rate_set_unsafe()",nif_ndx,0);
}

/*********************************************************************
*     Sets SGMII/1000BASE-X interface rate - autonegotiation
*     mode, or explicit.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_sgmii_rate_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SGMII_RATE      rate
  )
{
  uint32
    val,
    fld_val,
    res;
  uint32
    lane_ndx,
    mal_ndx;
  SOC_PETRA_NIF_1GE_MODE
    sgmii_mode;
  uint8
    is_sgmii_mal;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SGMII_RATE_VERIFY);

  regs = soc_petra_regs();

  mal_ndx = SOC_PETRA_NIF2MAL_NDX(nif_ndx);
  lane_ndx= SOC_PETRA_NIF_SGMII_LANE(nif_ndx);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    nif_ndx, SOC_PETRA_IF_NOF_NIFS-1,
    SOC_PETRA_NIF_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rate, SOC_PETRA_NIF_NOF_SGMII_RATES,
    SOC_PETRA_NIF_SGMII_RATE_OUT_OF_RANGE_ERR, 20, exit
  );

  res = soc_petra_nif_is_sgmii_get(
          unit,
          mal_ndx,
          &is_sgmii_mal
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  if (!is_sgmii_mal)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_MAL_SGMII_CONF_MISMATCH_ERR, 27, exit);
  }

  SOC_PA_FLD_IGET(regs->nif_mac_lane.sgmii_config_reg.sgmii_c_mode, fld_val, mal_ndx, 30, exit);

  val = SOC_SAND_GET_BIT(fld_val, lane_ndx);

  sgmii_mode = (val == 0x1)?SOC_PETRA_NIF_1GE_MODE_SGMII:SOC_PETRA_NIF_1GE_MODE_1000BASE_X;

  if (
      (sgmii_mode == SOC_PETRA_NIF_1GE_MODE_1000BASE_X) &&
      (rate != SOC_PETRA_NIF_SGMII_RATE_1000Mbps)
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_1000BASE_X_INVALID_RATE_ERR, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_sgmii_rate_verify()",0,0);
}

/*********************************************************************
*     Sets SGMII/1000BASE-X interface rate - autonegotiation
*     mode, or explicit.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_sgmii_rate_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_SGMII_RATE      *rate
  )
{
  uint32
    fld_val,
    reg_val,
    res;
  uint32
    mal_ndx = 0,
    lane_ndx = 0;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SGMII_RATE_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    nif_ndx, SOC_PETRA_IF_NOF_NIFS-1,
    SOC_PETRA_NIF_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  regs = soc_petra_regs();

  mal_ndx = SOC_PETRA_NIF2MAL_NDX(nif_ndx);
  lane_ndx= SOC_PETRA_NIF_SGMII_LANE(nif_ndx);

  SOC_PA_REG_IGET(regs->nif_mac_lane.auto_neg_config_reg, reg_val, mal_ndx, 10, exit);

  /*
   *  Get explicit rate
   */

  /*  0x0 is SOC_PETRA_NIF_SGMII_RATE_10Mbps */
  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.auto_neg_config_reg.link_speed_l[lane_ndx], fld_val, reg_val, 60, exit);
  *rate = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_sgmii_rate_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets SPAUI configuration - configure SPAUI extensions
*     for XAUI interface.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_spaui_conf_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION   direction_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SPAUI_INFO         *info
  )
{
  uint32
    fld_val = 0,
    fld_val2 = 0,
    reg_val = 0,
    val,
    res;
  SOC_PETRA_REG_FIELD
    *fld_addr, *fld_addr2;
  SOC_PETRA_REGS
    *regs;
  uint8
    rx_enable = FALSE,
    tx_enable = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SPAUI_CONF_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  regs = soc_petra_regs();

  rx_enable = SOC_PETRA_IS_DIRECTION_RX(direction_ndx);
  tx_enable = SOC_PETRA_IS_DIRECTION_TX(direction_ndx);

  fld_val = info->link_partner_double_size_bus ? 0x1 : 0x0;
  SOC_PA_FLD_ISET(regs->nif_mac_lane.spaui_extensions_tx_reg.tx_sop_on_even_only, fld_val, mal_ndx, 2, exit);

  fld_val = info->link_partner_double_size_bus ? 0x1 : 0x0;
  SOC_PA_FLD_ISET(regs->nif_mac_lane.spaui_extensions_tx_reg.tx_a_on_even_only, fld_val, mal_ndx, 3, exit);

  /* Enable/disable SPAUI { */
  SOC_PA_REG_IGET(regs->nif_mac_lane.enable_reg, reg_val, mal_ndx, 10, exit);

  fld_val = (info->enable?0x1:0x0);

  if (rx_enable)
  {
    SOC_PA_FLD_TO_REG(regs->nif_mac_lane.enable_reg.xaui_rx_lane_en, fld_val, reg_val, 20, exit);
  }

  if (tx_enable)
  {
    SOC_PA_FLD_TO_REG(regs->nif_mac_lane.enable_reg.xaui_tx_lane_en, fld_val, reg_val, 30, exit);
  }

  SOC_PA_REG_ISET(regs->nif_mac_lane.enable_reg, reg_val, mal_ndx, 40, exit);
  /* Enable/disable SPAUI } */

  if (info->enable)
  {
    if (rx_enable)
    {
      SOC_PA_REG_IGET(regs->nif_mac_lane.spaui_extensions_reg, reg_val, mal_ndx, 50, exit);

      fld_val = info->preamble.enable ? info->preamble.size : 0x0;
      SOC_PA_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions_reg.rx_preamble_size, fld_val, reg_val, 60, exit);

      fld_val = ((info->preamble.skip_SOP) || (!info->preamble.enable)) ? 0x0 : 0x1;
      SOC_PA_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions_reg.rx_preamble_sop, fld_val, reg_val, 70, exit);

      fld_val = info->ch_conf.enable ? info->ch_conf.bct_size : 0x0;
      SOC_PA_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions_reg.rx_bct_size, fld_val, reg_val, 10, exit);

      fld_val = info->ch_conf.enable ? info->ch_conf.bct_channel_byte_ndx : 0x0;
      SOC_PA_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions_reg.rx_bct_ch_position, fld_val, reg_val, 80, exit);

      if (info->crc_mode == SOC_PETRA_NIF_CRC_MODE_NONE)
      {
        fld_val = 0x0;
        SOC_PA_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions_reg.rx_crc_check_en,     fld_val, reg_val, 82, exit);
        SOC_PA_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions_reg.rx_crc_terminate_en, fld_val, reg_val, 84, exit);
      }
      else
      {
        fld_val = 0x1;
        SOC_PA_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions_reg.rx_crc_check_en,     fld_val, reg_val, 86, exit);
        SOC_PA_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions_reg.rx_crc_terminate_en, fld_val, reg_val, 88, exit);

        fld_val = (info->crc_mode == SOC_PETRA_NIF_CRC_MODE_32)?0x0:0x1;
        SOC_PA_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions_reg.rx_crc24_mode, fld_val, reg_val, 90, exit);
      }

      SOC_PA_REG_ISET(regs->nif_mac_lane.spaui_extensions_reg, reg_val, mal_ndx, 100, exit);
    }

    if (tx_enable)
    {
      SOC_PA_REG_IGET(regs->nif_mac_lane.spaui_extensions_tx_reg, reg_val, mal_ndx, 110, exit);

      fld_val = info->preamble.enable ? info->preamble.size : 0x0;
      SOC_PA_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions_tx_reg.tx_preamble_size, fld_val, reg_val, 120, exit);

      fld_val = ((info->preamble.skip_SOP) || (!info->preamble.enable)) ? 0x0 : 0x1;
      SOC_PA_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions_tx_reg.tx_preamble_sop, fld_val, reg_val, 130, exit);

      fld_val = info->ch_conf.enable ? info->ch_conf.bct_size : 0x0;
      SOC_PA_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions_tx_reg.tx_bct_size, fld_val, reg_val, 140, exit);

      fld_val = info->ch_conf.enable ? info->ch_conf.bct_channel_byte_ndx : 0x0;
      SOC_PA_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions_tx_reg.tx_bct_ch_position, fld_val, reg_val, 150, exit);

      if (info->crc_mode == SOC_PETRA_NIF_CRC_MODE_NONE)
      {
        fld_val = 0x0;
        SOC_PA_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions_tx_reg.tx_crc_insert_en, fld_val, reg_val, 154, exit);
      }
      else
      {
        fld_val = 0x1;
        SOC_PA_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions_tx_reg.tx_crc_insert_en, fld_val, reg_val, 154, exit);

        fld_val = (info->crc_mode == SOC_PETRA_NIF_CRC_MODE_32) ? 0x0 : 0x1;
        SOC_PA_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions_tx_reg.tx_crc24_mode, fld_val, reg_val, 170, exit);
      }

      SOC_PA_REG_ISET(regs->nif_mac_lane.spaui_extensions_tx_reg, reg_val, mal_ndx, 180, exit);

      if (mal_ndx < SOC_PETRA_MALS_IN_NIF_GRP)
      {
        fld_addr = SOC_PA_REG_DB_ACC_REF(regs->epni.egress_interface_no_fragmentation_mode_configuration_reg.nifa_no_frag);
        fld_addr2 = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_interface_no_fragmentation_mode_configuration_reg.nifa_no_frag);
      }
      else
      {
        fld_addr = SOC_PA_REG_DB_ACC_REF(regs->epni.egress_interface_no_fragmentation_mode_configuration_reg.nifb_no_frag);
        fld_addr2 = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_interface_no_fragmentation_mode_configuration_reg.nifb_no_frag);
      }

      SOC_PA_IMPLICIT_FLD_GET(*fld_addr, fld_val, 185, exit);
      SOC_PA_IMPLICIT_FLD_GET(*fld_addr2, fld_val2, 186, exit);
      val = (info->ch_conf.enable && info->ch_conf.is_burst_interleaving) ? 0x0 : 0x1;
      SOC_SAND_SET_BIT(fld_val, val, (mal_ndx % SOC_PETRA_MALS_IN_NIF_GRP));
      SOC_SAND_SET_BIT(fld_val2, val, (mal_ndx % SOC_PETRA_MALS_IN_NIF_GRP));
      SOC_PA_IMPLICIT_FLD_SET(*fld_addr, fld_val, 187, exit);
      SOC_PA_IMPLICIT_FLD_SET(*fld_addr2, fld_val2, 188, exit);

      if (info->ipg.enable == TRUE)
      {
        fld_val = (info->ipg.dic_mode == SOC_PETRA_NIF_IPG_DIC_MODE_AVERAGE)?0x1:0x0;
        SOC_PA_FLD_ISET(regs->nif_mac_lane.xaui_config_reg.ipg_dic_enable, fld_val, mal_ndx, 190, exit);

        fld_val = info->ipg.size;
        SOC_PA_FLD_ISET(regs->nif_mac_lane.xaui_config_reg.ipg_dic_threshold, fld_val, mal_ndx, 200, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_spaui_conf_set_unsafe()",mal_ndx,0);
}

/*********************************************************************
*     Sets SPAUI configuration - configure SPAUI extensions
*     for XAUI interface.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_spaui_conf_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION  direction_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_SPAUI_INFO         *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SPAUI_CONF_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    mal_ndx, SOC_PETRA_NOF_MAC_LANES-1,
    SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, SOC_PETRA_NOF_CONNECTION_DIRECTIONS-1,
    SOC_PETRA_NIF_DIRECTION_OUT_OF_RANGE_ERR, 20, exit
  );

  if (info->enable)
  {
    if (info->ipg.enable)
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(
        info->ipg.size, SOC_PETRA_NIF_IPG_SIZE_MIN, SOC_PETRA_NIF_IPG_SIZE_MAX,
        SOC_PETRA_NIF_IPG_SIZE_OUT_OF_RANGE_ERR, 30, exit
      );
    }

    if ((info->ch_conf.enable)  &&
        (info->preamble.enable) &&
        (info->preamble.size == 0))
    {
      if ((info->preamble.skip_SOP) &&
          (info->ch_conf.bct_channel_byte_ndx != 0x0))
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_PREAMBLE_AND_BCT_CONF_CONFLICT_ERR, 40, exit)
      }

      if ((!(info->preamble.skip_SOP)) &&
          (info->ch_conf.bct_channel_byte_ndx != 0x1))
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_PREAMBLE_AND_BCT_CONF_CONFLICT_ERR, 50, exit)
      }
    }

    if (info->ch_conf.enable)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(
        info->ch_conf.bct_channel_byte_ndx, SOC_PETRA_NIF_BCT_CH_IDX_MAX,
        SOC_PETRA_NIF_BCT_CHANNEL_NDX_OUT_OF_RANGE_ERR ,60, exit
      );

      SOC_SAND_ERR_IF_ABOVE_MAX(
        info->ch_conf.bct_size, SOC_PETRA_NIF_BCT_SIZE_MAX,
        SOC_PETRA_NIF_BCT_SIZE_OUT_OF_RANGE_ERR ,70, exit
      );

      if ((info->preamble.enable) &&
          !(info->preamble.skip_SOP) &&
          (info->ch_conf.bct_channel_byte_ndx == 0)
         )
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_BCT_CHANNEL_NDX_OUT_OF_RANGE_ERR, 80, exit)
      }
      if (
          (info->ch_conf.is_burst_interleaving) &&
          (!(info->ch_conf.bct_size == 2))
          )
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_SPAUI_INTRLV_BCT_SIZE_ERR, 90, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_spaui_conf_verify()",mal_ndx,0);
}

/*********************************************************************
*     Sets SPAUI configuration - configure SPAUI extensions
*     for XAUI interface.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_spaui_conf_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_SPAUI_INFO         *info_rx,
    SOC_SAND_OUT SOC_PETRA_NIF_SPAUI_INFO         *info_tx
  )
{
  uint32
    fld_val_rx = 0,
    reg_val_rx = 0,
    fld_val_tx = 0,
    reg_val_tx = 0,
    val_tx,
    res;
  SOC_PETRA_REG_FIELD
    *fld_addr_tx;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_NIF_SPAUI_INFO
    default_conf;
  uint8
    is_same;
  SOC_PETRA_NIF_SPAUI_INFO
    info_local_rx,
    info_local_tx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_SPAUI_CONF_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info_rx);
  SOC_SAND_CHECK_NULL_INPUT(info_tx);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    mal_ndx, SOC_PETRA_NOF_MAC_LANES-1,
    SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  soc_petra_PETRA_NIF_SPAUI_INFO_clear(info_rx);
  soc_petra_PETRA_NIF_SPAUI_INFO_clear(info_tx);

  regs = soc_petra_regs();

  soc_petra_PETRA_NIF_SPAUI_INFO_clear(&info_local_rx);
  soc_petra_PETRA_NIF_SPAUI_INFO_clear(&info_local_tx);

  res = soc_petra_nif_spaui_defaults_get_unsafe(
          unit,
          &default_conf
        ) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_PA_FLD_IGET(regs->nif_mac_lane.spaui_extensions_tx_reg.tx_sop_on_even_only, fld_val_rx, mal_ndx, 2, exit);

  info_local_rx.link_partner_double_size_bus = SOC_SAND_NUM2BOOL(fld_val_rx);
  info_local_tx.link_partner_double_size_bus = SOC_SAND_NUM2BOOL(fld_val_rx);

  /* Enable/disable SPAUI { */
  SOC_PA_REG_IGET(regs->nif_mac_lane.enable_reg, reg_val_tx, mal_ndx, 40, exit);

  /* For rx */
  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.enable_reg.xaui_rx_lane_en, fld_val_rx, reg_val_rx, 50, exit);

  info_local_rx.enable = (fld_val_rx?TRUE:FALSE);
  /* Enable/disable SPAUI } */

  SOC_PA_REG_IGET(regs->nif_mac_lane.spaui_extensions_reg, reg_val_rx, mal_ndx, 70, exit);

  /* Preamble { */
  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions_reg.rx_preamble_size, fld_val_rx, reg_val_rx, 80, exit);
  info_local_rx.preamble.size = fld_val_rx;

  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions_reg.rx_preamble_sop, fld_val_rx, reg_val_rx, 90, exit);
  info_local_rx.preamble.skip_SOP = fld_val_rx?FALSE:TRUE;
  /* Preamble } */

  /* Channelized { */
  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions_reg.rx_bct_size, fld_val_rx, reg_val_rx, 100, exit);
  info_local_rx.ch_conf.bct_size = fld_val_rx;

  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions_reg.rx_bct_ch_position, fld_val_rx, reg_val_rx, 110, exit);
  info_local_rx.ch_conf.bct_channel_byte_ndx = fld_val_rx;
  /* Channelized } */

  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions_reg.rx_crc_check_en, fld_val_rx, reg_val_rx, 112, exit);
  if (fld_val_rx == 0)
  {
     info_local_rx.crc_mode = SOC_PETRA_NIF_CRC_MODE_NONE;
  }
  else
  {
    SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions_reg.rx_crc24_mode, fld_val_rx, reg_val_rx, 120, exit);
    info_local_rx.crc_mode = (fld_val_rx == 0)? SOC_PETRA_NIF_CRC_MODE_32:SOC_PETRA_NIF_CRC_MODE_24;
  }

  /* IPG { */
  /* IPG is tx-related, but we read it anyway */
  SOC_PA_FLD_IGET(regs->nif_mac_lane.xaui_config_reg.ipg_dic_enable, fld_val_rx, mal_ndx, 125, exit);
  info_local_rx.ipg.dic_mode = fld_val_rx?SOC_PETRA_NIF_IPG_DIC_MODE_AVERAGE:SOC_PETRA_NIF_IPG_DIC_MODE_MINIMUM;

  SOC_PA_FLD_IGET(regs->nif_mac_lane.xaui_config_reg.ipg_dic_threshold, fld_val_rx, mal_ndx, 128, exit);
  info_local_rx.ipg.size = fld_val_rx;
  /* IPG } */

  SOC_PETRA_COMP(&(default_conf.preamble), &(info_local_rx.preamble), SOC_PETRA_NIF_PREAMBLE_COMPRESS, 1, is_same);
  info_local_rx.preamble.enable  = (is_same == FALSE)?TRUE:FALSE;

  SOC_PETRA_COMP(&(default_conf.ch_conf), &(info_local_rx.ch_conf), SOC_PETRA_NIF_CHANNELIZED, 1, is_same);
  info_local_rx.ch_conf.enable  = (is_same == FALSE)?TRUE:FALSE;

  SOC_PETRA_COMP(&(default_conf.ipg), &(info_local_rx.ipg), SOC_PETRA_NIF_IPG_COMPRESS, 1, is_same);
  info_local_rx.ipg.enable  = (is_same == FALSE)?TRUE:FALSE;

  /* For tx */

  SOC_PA_REG_IGET(regs->nif_mac_lane.spaui_extensions_tx_reg, reg_val_tx, mal_ndx, 130, exit);

  /* Preamble { */
  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions_tx_reg.tx_preamble_size, fld_val_tx, reg_val_tx, 140, exit);
  info_local_tx.preamble.size = fld_val_tx;

  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions_tx_reg.tx_preamble_sop, fld_val_tx, reg_val_tx, 150, exit);
  info_local_tx.preamble.skip_SOP = fld_val_tx?FALSE:TRUE;
  /* Preamble } */

  /* Channelized { */
  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions_tx_reg.tx_bct_size, fld_val_tx, reg_val_tx, 160, exit);
  info_local_tx.ch_conf.bct_size = fld_val_tx;

  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions_tx_reg.tx_bct_ch_position, fld_val_tx, reg_val_tx, 170, exit);
  info_local_tx.ch_conf.bct_channel_byte_ndx = fld_val_tx;

  if (mal_ndx < SOC_PETRA_MALS_IN_NIF_GRP)
  {
    fld_addr_tx = SOC_PA_REG_DB_ACC_REF(regs->epni.egress_interface_no_fragmentation_mode_configuration_reg.nifa_no_frag);
  }
  else
  {
    fld_addr_tx = SOC_PA_REG_DB_ACC_REF(regs->epni.egress_interface_no_fragmentation_mode_configuration_reg.nifb_no_frag);
  }

  SOC_PA_IMPLICIT_FLD_GET(*fld_addr_tx, fld_val_tx, 180, exit);

  val_tx = SOC_SAND_GET_BIT(fld_val_tx, (mal_ndx % SOC_PETRA_MALS_IN_NIF_GRP));

  info_local_tx.ch_conf.is_burst_interleaving = val_tx?FALSE:TRUE;
  /* Channelized } */

  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions_tx_reg.tx_crc_insert_en, fld_val_tx, reg_val_tx, 112, exit);
  if (fld_val_tx == 0)
  {
     info_local_tx.crc_mode = SOC_PETRA_NIF_CRC_MODE_NONE;
  }
  else
  {
    SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions_tx_reg.tx_crc24_mode, fld_val_tx, reg_val_tx, 190, exit);
    info_local_tx.crc_mode = (fld_val_tx == 0)? SOC_PETRA_NIF_CRC_MODE_32:SOC_PETRA_NIF_CRC_MODE_24;
  }

  /* IPG { */
  /* IPG is tx-related, but we read it anyway */
  SOC_PA_FLD_IGET(regs->nif_mac_lane.xaui_config_reg.ipg_dic_enable, fld_val_tx, mal_ndx, 200, exit);
  info_local_tx.ipg.dic_mode = fld_val_tx?SOC_PETRA_NIF_IPG_DIC_MODE_AVERAGE:SOC_PETRA_NIF_IPG_DIC_MODE_MINIMUM;

  SOC_PA_FLD_IGET(regs->nif_mac_lane.xaui_config_reg.ipg_dic_threshold, fld_val_tx, mal_ndx, 210, exit);
  info_local_tx.ipg.size = fld_val_tx;
  /* IPG } */

  SOC_PETRA_COMP(&(default_conf.preamble), &(info_local_tx.preamble), SOC_PETRA_NIF_PREAMBLE_COMPRESS, 1, is_same);
  info_local_tx.preamble.enable  = (is_same == FALSE)?TRUE:FALSE;

  SOC_PETRA_COMP(&(default_conf.ch_conf), &(info_local_tx.ch_conf), SOC_PETRA_NIF_CHANNELIZED, 1, is_same);
  info_local_tx.ch_conf.enable  = (is_same == FALSE)?TRUE:FALSE;

  SOC_PETRA_COMP(&(default_conf.ipg), &(info_local_tx.ipg), SOC_PETRA_NIF_IPG_COMPRESS, 1, is_same);
  info_local_tx.ipg.enable  = (is_same == FALSE)?TRUE:FALSE;


  SOC_PETRA_COPY(info_rx, &info_local_rx, SOC_PETRA_NIF_SPAUI_INFO, 1);
  SOC_PETRA_COPY(info_tx, &info_local_tx, SOC_PETRA_NIF_SPAUI_INFO, 1);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_spaui_conf_get_unsafe()",mal_ndx,0);
}

/*********************************************************************
*     Gets interface type and channelized indication
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_type_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_TYPE            *nif_type,
    SOC_SAND_OUT uint8                 *is_channelized
  )
{
  uint32
    res;
  SOC_PETRA_NIF_INFO
    nif_info;
  SOC_PETRA_NIF_SPAUI_INFO
    spaui_rx_info,
    spaui_tx_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_TYPE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(nif_type);
  SOC_SAND_CHECK_NULL_INPUT(is_channelized);

  soc_petra_PETRA_NIF_SPAUI_INFO_clear(&spaui_rx_info);
  soc_petra_PETRA_NIF_SPAUI_INFO_clear(&spaui_tx_info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    mal_ndx, SOC_PETRA_NOF_MAC_LANES-1,
    SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  res = soc_petra_nif_mal_basic_conf_get_unsafe(
          unit,
          mal_ndx,
          &nif_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *nif_type = nif_info.type;

  if (nif_info.type == SOC_PETRA_NIF_TYPE_SPAUI)
  {
    res = soc_petra_nif_spaui_conf_get_unsafe(
            unit,
            mal_ndx,
            &spaui_rx_info,
            &spaui_tx_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);

    *is_channelized = (spaui_rx_info.ch_conf.enable)?TRUE:FALSE;
    if (!(*is_channelized))
    {
      *is_channelized = (spaui_tx_info.ch_conf.enable)?TRUE:FALSE;
    }
  }
  else
  {
    *is_channelized = FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_type_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets minimal packet size.
*     Note: Normally, the packet size is limited
*     using soc_petra_mgmt_pckt_size_range_set(), and not this API.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_min_packet_size_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION   direction_ndx,
    SOC_SAND_IN  uint32                     pckt_size
  )
{
  uint32
    reg_val,
    fld_val,
    res;
  SOC_PETRA_REGS
    *regs;
  uint8
    is_sgmii,
    rx_enable = FALSE,
    tx_enable = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MIN_PACKET_SIZE_SET_UNSAFE);

  regs = soc_petra_regs();

  rx_enable = SOC_PETRA_IS_DIRECTION_RX(direction_ndx);
  tx_enable = SOC_PETRA_IS_DIRECTION_TX(direction_ndx);

  res = soc_petra_nif_is_sgmii_get(
          unit,
          mal_ndx,
          &is_sgmii
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  fld_val = pckt_size;

  if (is_sgmii == TRUE)
  {
    SOC_PA_REG_IGET(regs->nif_mac_lane.sgmii_config_reg, reg_val, mal_ndx, 10, exit);

    if (rx_enable)
    {
      SOC_PA_FLD_TO_REG(regs->nif_mac_lane.sgmii_config_reg.sgmii_rx_min_pkt_length, fld_val, reg_val, 20, exit);
    }

    if(tx_enable)
    {
      SOC_PA_FLD_TO_REG(regs->nif_mac_lane.sgmii_config_reg.sgmii_tx_min_pkt_length, fld_val, reg_val, 30, exit);
    }

    SOC_PA_REG_ISET(regs->nif_mac_lane.sgmii_config_reg, reg_val, mal_ndx, 40, exit);
  }
  else
  {
    SOC_PA_REG_IGET(regs->nif_mac_lane.xaui_config_reg, reg_val, mal_ndx, 110, exit);

    if (rx_enable)
    {
      SOC_PA_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.rx_min_pkt_length, fld_val, reg_val, 120, exit);
    }

    if(tx_enable)
    {
      SOC_PA_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.tx_min_pkt_length, fld_val, reg_val, 130, exit);
    }

    SOC_PA_REG_ISET(regs->nif_mac_lane.xaui_config_reg, reg_val, mal_ndx, 140, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_min_packet_size_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets minimal packet size.
*     Note: Normally, the packet size is limited
*     using soc_petra_mgmt_pckt_size_range_set(), and not this API.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_min_packet_size_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION  direction_ndx,
    SOC_SAND_IN  uint32                     pckt_size
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MIN_PACKET_SIZE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    mal_ndx, SOC_PETRA_NOF_MAC_LANES-1,
    SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, SOC_PETRA_NOF_CONNECTION_DIRECTIONS-1,
    SOC_PETRA_NIF_DIRECTION_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_min_packet_size_verify()",0,0);
}

/*********************************************************************
*     Sets minimal packet size.
*     Note: Normally, the packet size is limited
*     using soc_petra_mgmt_pckt_size_range_set(), and not this API.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_min_packet_size_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_OUT uint32                     *pckt_size_rx,
    SOC_SAND_OUT uint32                     *pckt_size_tx
  )
{
  uint32
    reg_val = 0,
    fld_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    pckt_size_local_rx = 0,
    pckt_size_local_tx = 0;
  uint8
    is_sgmii;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MIN_PACKET_SIZE_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    mal_ndx, SOC_PETRA_NOF_MAC_LANES-1,
    SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  regs = soc_petra_regs();

  res = soc_petra_nif_is_sgmii_get(
          unit,
          mal_ndx,
          &is_sgmii
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (is_sgmii == TRUE)
  {
    SOC_PA_REG_IGET(regs->nif_mac_lane.sgmii_config_reg, reg_val, mal_ndx, 30, exit);

    SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.sgmii_config_reg.sgmii_rx_min_pkt_length, fld_val, reg_val, 40, exit);
    pckt_size_local_rx = fld_val;

    SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.sgmii_config_reg.sgmii_tx_min_pkt_length, fld_val, reg_val, 50, exit);
    pckt_size_local_tx = fld_val;

    *pckt_size_rx = pckt_size_local_rx;
    *pckt_size_tx = pckt_size_local_tx;
  }
  else
  {
    SOC_PA_REG_IGET(regs->nif_mac_lane.xaui_config_reg, reg_val, mal_ndx, 130, exit);

    SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.xaui_config_reg.rx_min_pkt_length, fld_val, reg_val, 140, exit);
    pckt_size_local_rx = fld_val;

    SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.xaui_config_reg.tx_min_pkt_length, fld_val, reg_val, 150, exit);
    pckt_size_local_tx = fld_val;

    *pckt_size_rx = pckt_size_local_rx;
    *pckt_size_tx = pckt_size_local_tx;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_min_packet_size_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets/unsets Local Fault (LF) indication for NIF MAL. If
*     LF is identified (e.g. by external PHY), this API can be
*     called to indicate the LF to the NIF. In this case, the
*     NIF sets Remote Fault indication (LFS Send-Q signal).
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_local_fault_ovrd_set_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint32   mal_ndx,
    SOC_SAND_IN  uint8   is_lf_ovrd
  )
{
  uint32
    fld_val,
    reg_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_LOCAL_FAULT_OVRD_SET_UNSAFE);

  regs = soc_petra_regs();

  SOC_PA_REG_IGET(regs->nif_mac_lane.xaui_config_reg, reg_val, mal_ndx, 10, exit);

  fld_val = 0x1;
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.lfs_ovrd_send_q, fld_val, reg_val, 20, exit);
  fld_val = SOC_SAND_BOOL2NUM(is_lf_ovrd);
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.lfs_ovrd_en, fld_val, reg_val, 30, exit);

  SOC_PA_REG_ISET(regs->nif_mac_lane.xaui_config_reg, reg_val, mal_ndx, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_local_fault_ovrd_set_unsafe()",mal_ndx,0);
}


/*********************************************************************
*     Sets/unsets Local Fault (LF) indication for NIF MAL. If
*     LF is identified (e.g. by external PHY), this API can be
*     called to indicate the LF to the NIF. In this case, the
*     NIF sets Remote Fault indication (LFS Send-Q signal).
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_local_fault_ovrd_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  uint8                 is_lf_ovrd
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_LOCAL_FAULT_OVRD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    mal_ndx, SOC_PETRA_NOF_MAC_LANES-1,
    SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_local_fault_ovrd_verify()",mal_ndx,0);
}

/*********************************************************************
*     Sets/unsets Local Fault (LF) indication for NIF MAL. If
*     LF is identified (e.g. by external PHY), this API can be
*     called to indicate the LF to the NIF. In this case, the
*     NIF sets Remote Fault indication (LFS Send-Q signal).
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_local_fault_ovrd_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT uint8                 *is_lf_ovrd
  )
{
  uint32
    fld_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_LOCAL_FAULT_OVRD_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_lf_ovrd);

  regs = soc_petra_regs();

  SOC_SAND_ERR_IF_ABOVE_MAX(
    mal_ndx, SOC_PETRA_NOF_MAC_LANES-1,
    SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_PA_FLD_IGET(regs->nif_mac_lane.xaui_config_reg.lfs_ovrd_en, fld_val, mal_ndx, 20, exit);

  *is_lf_ovrd = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_local_fault_ovrd_get_unsafe()",mal_ndx,0);
}

/************************************************************************/
/* Checks is FAT pipe interface is currently enabled                    */
/************************************************************************/
uint32
  soc_petra_nif_fat_pipe_enable_get(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT uint8  *is_enabled
  )
{
  uint32
    fld_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_FAT_PIPE_ENABLE_GET);

  regs = soc_petra_regs();

  /* Get FAP port to be part of the Fat Pipe { */
  /* read ingress configuration */
  SOC_PA_FLD_GET(regs->irr.resequencing_port_enable_reg.resequencing_port_enable, fld_val, 10, exit);

  /* TRUE if ports assigned to Fat Pipe exist */
  *is_enabled = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_nif_fat_pipe_enable_get()",0,0);
}

/*********************************************************************
*     Configures Fat Pipe interface.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_fat_pipe_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_NIF_FAT_PIPE_INFO   *info
  )
{
  uint32
    fld_val = 0,
    bit_val = 0,
    res;
  uint32
    port_idx;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    mapping;
  SOC_PETRA_IHP_PORT_INFO
    ihp_port_info;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_FAT_PIPE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&mapping);
  soc_petra_PETRA_IHP_PORT_INFO_clear(&ihp_port_info);

  /* Bypass enable { */
  fld_val =  (info->bypass_enable && info->enable)?0x1:0x0;
  SOC_PA_FLD_SET(regs->irr.resequencing_configuration_reg.bypass_reseq, fld_val, 17, exit);;
  /* Bypass enable } */

  /* set FAP port to be part of the Fat Pipe { */
  fld_val = 0;

  if (info->enable)
  {
    for (port_idx = SOC_PETRA_FAT_PIPE_FAP_PORT_ID; port_idx <= SOC_PETRA_FAT_PIPE_FAP_PORT_ID_LAST; port_idx++)
    {
      bit_val = (port_idx <= info->nof_ports)?0x1:0x0;
      SOC_SAND_SET_BIT(fld_val, bit_val, port_idx - SOC_PETRA_FAT_PIPE_FAP_PORT_ID);
    }
  }

  /* Configure ingress */
  SOC_PA_FLD_SET(regs->irr.resequencing_port_enable_reg.resequencing_port_enable, fld_val, 20, exit);

  /* Configure egress */
  SOC_PA_FLD_SET(regs->egq.oc768_mask_reg.oc768_mask, fld_val, 30, exit);
  /* set FAP port to be part of the Fat Pipe } */

  soc_petra_sw_db_fat_pipe_nof_ports_set(unit, info->nof_ports);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_fat_pipe_set_unsafe()",0,0);
}

uint32
  soc_petra_nif_is_fat_pipe_port(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 port_ndx,
    SOC_SAND_OUT uint8 *is_fatp_port
  )
{
  uint32
    fatp_nof_ports = 0;
  uint8
    is_fatp = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_IS_FAT_PIPE_PORT);

  fatp_nof_ports = soc_petra_sw_db_fat_pipe_nof_ports_get(unit);

  if (SOC_SAND_IS_VAL_IN_RANGE(port_ndx, SOC_PETRA_FAT_PIPE_FAP_PORT_ID, SOC_PETRA_FAT_PIPE_FAP_PORT_ID + fatp_nof_ports - 1))
  {
    is_fatp = TRUE;
  }

  *is_fatp_port = is_fatp;

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_is_fat_pipe_port()",0,0);
}

uint32
  soc_petra_nif_is_fat_pipe_mal(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 mal_ndx,
    SOC_SAND_OUT uint8 *is_fatp_mal
  )
{
  uint32
    res;
  uint32
    port_idx;
  uint8
    is_fatp = FALSE;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    port_map,
    out_port_map;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_IS_FAT_PIPE_MAL);
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&port_map);
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&out_port_map);

  for (port_idx = 0; (port_idx < SOC_PETRA_NOF_FAP_PORTS) && (!is_fatp); port_idx++)
  {
    res = soc_petra_port_to_interface_map_get(
            unit,
            port_idx,
            &port_map,
            &out_port_map
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (SOC_PETRA_IF2MAL_NDX(port_map.if_id) == mal_ndx)
    {
      res = soc_petra_nif_is_fat_pipe_port(
              unit,
              port_idx,
              &is_fatp
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }

  *is_fatp_mal = is_fatp;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_is_fat_pipe_mal()",0,0);
}

/*********************************************************************
*     Configures Fat Pipe interface.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_fat_pipe_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_NIF_FAT_PIPE_INFO   *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_FAT_PIPE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if (info->enable == TRUE)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(
      info->nof_ports, SOC_PETRA_NIF_FAT_PIPE_NOF_PORTS_MIN, SOC_PETRA_NIF_FAT_PIPE_NOF_PORTS_MAX,
      SOC_PETRA_NIF_NOF_PORTS_IN_FAT_PIPE_OUT_OF_RANGE_ERR, 10, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_fat_pipe_verify()",0,0);
}

/*********************************************************************
*     Configures Fat Pipe interface.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_fat_pipe_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_NIF_FAT_PIPE_INFO   *info
  )
{
  uint32
    fld_val = 0,
    bit_val = 0,
    res;
  uint32
    port_idx,
    nof_ports_curr = 0;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_FAT_PIPE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  /* Get FAP port to be part of the Fat Pipe { */
  /* read ingress configuration */
  SOC_PA_FLD_GET(regs->irr.resequencing_port_enable_reg.resequencing_port_enable, fld_val, 20, exit);

  for (port_idx = SOC_PETRA_FAT_PIPE_FAP_PORT_ID; port_idx <= SOC_PETRA_NIF_FAT_PIPE_NOF_PORTS_MAX; port_idx++)
  {
    bit_val = SOC_SAND_GET_BIT(fld_val, port_idx-SOC_PETRA_FAT_PIPE_FAP_PORT_ID);
    nof_ports_curr += bit_val;
  }
  info->nof_ports = nof_ports_curr;
  /* Get FAP port to be part of the Fat Pipe } */

  /* TRUE if ports assigned to Fat Pipe exist */
  info->enable = fld_val?TRUE:FALSE;

  if (info->enable)
  {
    /* Bypass enable { */
    SOC_PA_FLD_GET(regs->irr.resequencing_configuration_reg.bypass_reseq, fld_val, 10, exit);
    info->bypass_enable = fld_val?TRUE:FALSE;
    /* Bypass enable } */
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_nif_fat_pipe_get_unsafe()",0,0);
}

/*********************************************************************
*     MDIO Clause 22 write function
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mdio22_write_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  phy_addr,
    SOC_SAND_IN  uint32                  reg_addr,
    SOC_SAND_IN  uint32                  size_in_words,
    SOC_SAND_IN  uint16                  *data
  )
{
  uint32
    reg_val = 0,
    fld_val = 0,
    res;
  uint32
    wrd_ndx;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    poll_success;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MDIO22_WRITE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(data);

  if (size_in_words < 1)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_MDIO_LESS_THEN_ONE_WORD_DATA_ERR, 10, exit);
  }

  regs = soc_petra_regs();

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  poll_info.expected_value = 0x1;
  poll_info.busy_wait_nof_iters = SOC_PETRA_NIF_MAX_MDIO_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PETRA_NIF_MAX_MDIO_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PETRA_NIF_MAX_MDIO_TIMER_DELAY_MSEC;

  reg_val = 0;

  /* TurnAround */
  fld_val = 0x2;
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_ta, fld_val, reg_val, 30, exit);

  /* Address of a PHY device */
  fld_val = phy_addr;
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_phyad, fld_val, reg_val, 40, exit);

  /* Operation type */
  fld_val = 0x1; /* write */
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_op, fld_val, reg_val, 50, exit);

  /* Start of frame */
  fld_val = 0x1; /* Clause 22 */
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_st, fld_val, reg_val, 60, exit);

  for (wrd_ndx = 0; wrd_ndx < size_in_words; wrd_ndx++)
  {
    /* Register address, incremented at each iteration */
    fld_val = reg_addr + wrd_ndx;
    SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_regad, fld_val, reg_val, 70, exit);

    /* Data */
    fld_val = *(data + wrd_ndx);
    SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_data, fld_val, reg_val, 80, exit);

    /* The actual write to device */
    SOC_PETRA_REG_SET(regs->eci.mdio_op_reg, reg_val, 90, exit);

    res = soc_petra_status_fld_poll_unsafe(
            unit,
            SOC_PETRA_REG_DB_ACC_REF(regs->eci.mdio_stat_reg.stat_valid),
            SOC_PETRA_DEFAULT_INSTANCE,
            &poll_info,
            &poll_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    if (poll_success == FALSE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MDIO_OP_TIMEOUT_ERR, 102, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mdio22_write_unsafe()",0,0);
}

/*********************************************************************
*     MDIO Clause 22 read function
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mdio22_read_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  phy_addr,
    SOC_SAND_IN  uint32                  reg_addr,
    SOC_SAND_IN  uint32                  size_in_words,
    SOC_SAND_OUT uint16                  *data
  )
{
  uint32
    reg_val = 0,
    fld_val = 0,
    res;
  uint32
    wrd_ndx;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    poll_success;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MDIO22_READ_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(data);

  if (size_in_words < 1)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_MDIO_LESS_THEN_ONE_WORD_DATA_ERR, 10, exit);
  }

  regs = soc_petra_regs();

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  poll_info.expected_value = 0x1;
  poll_info.busy_wait_nof_iters = SOC_PETRA_NIF_MAX_MDIO_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PETRA_NIF_MAX_MDIO_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PETRA_NIF_MAX_MDIO_TIMER_DELAY_MSEC;

  reg_val = 0x0;

  /* Clear the data - ignored in clause 22 read operation */
  fld_val = 0x0;
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_data, fld_val, reg_val, 30, exit);

  /* TurnAround */
  fld_val = 0x2;
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_ta, fld_val, reg_val, 40, exit);

  /* Address of a PHY device */
  fld_val = phy_addr;
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_phyad, fld_val, reg_val, 50, exit);

  /* Operation type */
  fld_val = 0x2; /* read */
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_op, fld_val, reg_val, 60, exit);

  /* Start of frame */
  fld_val = 0x1; /* Clause 22 */
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_st, fld_val, reg_val, 70, exit);

  for (wrd_ndx = 0; wrd_ndx < size_in_words; wrd_ndx++)
  {
    /* Send read request { */
    /* Register address is incremented at each iteration */
    fld_val = reg_addr + wrd_ndx;
    SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_regad, fld_val, reg_val, 80, exit);

    /* The actual write to device */
    SOC_PETRA_REG_SET(regs->eci.mdio_op_reg, reg_val, 90, exit);

    res = soc_petra_status_fld_poll_unsafe(
            unit,
            SOC_PETRA_REG_DB_ACC_REF(regs->eci.mdio_stat_reg.stat_valid),
            SOC_PETRA_DEFAULT_INSTANCE,
            &poll_info,
            &poll_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    if (poll_success == FALSE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MDIO_OP_TIMEOUT_ERR, 102, exit);
    }

    SOC_PETRA_FLD_GET(regs->eci.mdio_stat_reg.stat_data, fld_val, 110, exit);
    *(data + wrd_ndx) = (uint16)fld_val;
    /* Read data } */

    SOC_PETRA_REG_SET(regs->eci.mdio_op_reg, SOC_PETRA_NIF_MDIO_DUMMY_OP, 120, exit);

    res = soc_petra_status_fld_poll_unsafe(
            unit,
            SOC_PETRA_REG_DB_ACC_REF(regs->eci.mdio_stat_reg.stat_valid),
            SOC_PETRA_DEFAULT_INSTANCE,
            &poll_info,
            &poll_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

    if (poll_success == FALSE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MDIO_OP_TIMEOUT_ERR, 132, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mdio22_read_unsafe()",0,0);
}

/*********************************************************************
*     MDIO Clause 45 write function
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mdio45_write_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  port_addr,
    SOC_SAND_IN  uint32                  dev_addr,
    SOC_SAND_IN  uint32                  reg_addr,
    SOC_SAND_IN  uint32                  size_in_words,
    SOC_SAND_IN  uint16                  *data
  )
{
  uint32
    reg_val = 0,
    fld_val = 0,
    res;
  uint32
    wrd_ndx;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    poll_success;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MDIO45_WRITE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(data);

  if (size_in_words < 1)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_MDIO_LESS_THEN_ONE_WORD_DATA_ERR, 10, exit);
  }

  regs = soc_petra_regs();

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  poll_info.expected_value = 0x1;
  poll_info.busy_wait_nof_iters = SOC_PETRA_NIF_MAX_MDIO_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PETRA_NIF_MAX_MDIO_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PETRA_NIF_MAX_MDIO_TIMER_DELAY_MSEC;

  reg_val = 0x0;

  /* TurnAround */
  fld_val = 0x2;
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_ta, fld_val, reg_val, 30, exit);

  /* Device address */
  fld_val = dev_addr;
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_regad, fld_val, reg_val, 40, exit);

  /* Port address */
  fld_val = port_addr;
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_phyad, fld_val, reg_val, 50, exit);

  /* Start of frame */
  fld_val = 0x0; /* Clause 45 */
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_st, fld_val, reg_val, 60, exit);

  for (wrd_ndx = 0; wrd_ndx < size_in_words; wrd_ndx++)
  {
    /* write address { */

    /* Register address, incremented at each iteration */
    fld_val = reg_addr + wrd_ndx;
    SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_data, fld_val, reg_val, 70, exit);

    /* Operation type  */
    fld_val = 0x0; /* write address */
    SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_op, fld_val, reg_val, 80, exit);

    /* The actual write to device */
    SOC_PETRA_REG_SET(regs->eci.mdio_op_reg, reg_val, 90, exit);

    res = soc_petra_status_fld_poll_unsafe(
            unit,
            SOC_PETRA_REG_DB_ACC_REF(regs->eci.mdio_stat_reg.stat_valid),
            SOC_PETRA_DEFAULT_INSTANCE,
            &poll_info,
            &poll_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    if (poll_success == FALSE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MDIO_OP_TIMEOUT_ERR, 102, exit);
    }
    /* write address } */

    /* write data { */

    /* Data */
    fld_val = *(data + wrd_ndx);
    SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_data, fld_val, reg_val, 105, exit);

    /* Operation type  */
    fld_val = 0x1; /* write data */
    SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_op, fld_val, reg_val, 110, exit);

    /* The actual write to device */
    SOC_PETRA_REG_SET(regs->eci.mdio_op_reg, reg_val, 120, exit);

    res = soc_petra_status_fld_poll_unsafe(
            unit,
            SOC_PETRA_REG_DB_ACC_REF(regs->eci.mdio_stat_reg.stat_valid),
            SOC_PETRA_DEFAULT_INSTANCE,
            &poll_info,
            &poll_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

    if (poll_success == FALSE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MDIO_OP_TIMEOUT_ERR, 132, exit);
    }
    /* write data } */
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mdio45_write_unsafe()",0,0);
}

/*********************************************************************
*     MDIO Clause 45 read function
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_mdio45_read_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  port_addr,
    SOC_SAND_IN  uint32                  dev_addr,
    SOC_SAND_IN  uint32                  reg_addr,
    SOC_SAND_IN  uint32                  size_in_words,
    SOC_SAND_OUT uint16                  *data
  )
{
  uint32
    reg_val = 0,
    fld_val = 0,
    res;
  uint32
    wrd_ndx;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    poll_success;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_MDIO45_READ_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(data);

  if (size_in_words < 1)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_MDIO_LESS_THEN_ONE_WORD_DATA_ERR, 10, exit);
  }

  regs = soc_petra_regs();

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  poll_info.expected_value = 0x1;
  poll_info.busy_wait_nof_iters = SOC_PETRA_NIF_MAX_MDIO_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PETRA_NIF_MAX_MDIO_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PETRA_NIF_MAX_MDIO_TIMER_DELAY_MSEC;

  reg_val = 0x0;

  /* TurnAround */
  fld_val = 0x2;
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_ta, fld_val, reg_val, 30, exit);

  /* Device address */
  fld_val = dev_addr;
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_regad, fld_val, reg_val, 40, exit);

  /* Port address */
  fld_val = port_addr;
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_phyad, fld_val, reg_val, 50, exit);

  /* Start of frame */
  fld_val = 0x0; /* Clause 45 */
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_st, fld_val, reg_val, 60, exit);

  /* write address { */
  /* Register address */
  fld_val = reg_addr;
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_data, fld_val, reg_val, 70, exit);

  /* Operation type  */
  fld_val = 0x0; /* write address */
  SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_op, fld_val, reg_val, 80, exit);

  /* The actual write to device */
  SOC_PETRA_REG_SET(regs->eci.mdio_op_reg, reg_val, 90, exit);

  res = soc_petra_status_fld_poll_unsafe(
            unit,
            SOC_PETRA_REG_DB_ACC_REF(regs->eci.mdio_stat_reg.stat_valid),
            SOC_PETRA_DEFAULT_INSTANCE,
            &poll_info,
            &poll_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    if (poll_success == FALSE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MDIO_OP_TIMEOUT_ERR, 102, exit);
    }
  /* write address } */

  if (size_in_words == 1)
  {
    /* Send read request { */
    /* Operation type  */
    fld_val = 0x3; /* read */
    SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_op, fld_val, reg_val, 105, exit);

    /* The actual write to device */
    SOC_PETRA_REG_SET(regs->eci.mdio_op_reg, reg_val, 110, exit);

    res = soc_petra_status_fld_poll_unsafe(
            unit,
            SOC_PETRA_REG_DB_ACC_REF(regs->eci.mdio_stat_reg.stat_valid),
            SOC_PETRA_DEFAULT_INSTANCE,
            &poll_info,
            &poll_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

    if (poll_success == FALSE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MDIO_OP_TIMEOUT_ERR, 122, exit);
    }
    /* Send read request } */

    /* Read data { */
    SOC_PETRA_FLD_GET(regs->eci.mdio_stat_reg.stat_data, fld_val, 140, exit);
    *data = (uint16)fld_val;
    /* Read data } */

    SOC_PETRA_REG_SET(regs->eci.mdio_op_reg, SOC_PETRA_NIF_MDIO_DUMMY_OP, 145, exit);

    res = soc_petra_status_fld_poll_unsafe(
            unit,
            SOC_PETRA_REG_DB_ACC_REF(regs->eci.mdio_stat_reg.stat_valid),
            SOC_PETRA_DEFAULT_INSTANCE,
            &poll_info,
            &poll_success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 147, exit);

    if (poll_success == FALSE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MDIO_OP_TIMEOUT_ERR, 152, exit);
    }
  }
  else
  {
    /* post-read-increment-address mode */
    for (wrd_ndx = 0; wrd_ndx < size_in_words; wrd_ndx++)
    {
      /* Send read request { */
      /* Operation type  */
      fld_val = 0x2; /* read, increment register address */
      SOC_PETRA_FLD_TO_REG(regs->eci.mdio_op_reg.op_op, fld_val, reg_val, 150, exit);

      /* The actual write to device */
      SOC_PETRA_REG_SET(regs->eci.mdio_op_reg, reg_val, 160, exit);

      res = soc_petra_status_fld_poll_unsafe(
              unit,
              SOC_PETRA_REG_DB_ACC_REF(regs->eci.mdio_stat_reg.stat_valid),
              SOC_PETRA_DEFAULT_INSTANCE,
              &poll_info,
              &poll_success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 165, exit);

     if (poll_success == FALSE)
     {
       SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MDIO_OP_TIMEOUT_ERR, 167, exit);
     }

      /* Send read request } */

      /* Read data { */
      SOC_PETRA_FLD_GET(regs->eci.mdio_stat_reg.stat_data, fld_val, 190, exit);
      *(data + wrd_ndx) = (uint16)fld_val;
      /* Read data } */

      SOC_PETRA_REG_SET(regs->eci.mdio_op_reg, SOC_PETRA_NIF_MDIO_DUMMY_OP, 200, exit);

      res = soc_petra_status_fld_poll_unsafe(
              unit,
              SOC_PETRA_REG_DB_ACC_REF(regs->eci.mdio_stat_reg.stat_valid),
              SOC_PETRA_DEFAULT_INSTANCE,
              &poll_info,
              &poll_success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

      if (poll_success == FALSE)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MDIO_OP_TIMEOUT_ERR, 212, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_mdio45_read_unsafe()",0,0);
}

/*********************************************************************
*     Function description
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_pause_quanta_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION        direction_ndx,
    SOC_SAND_IN  uint32                  pause_quanta
  )
{
   uint32
    fld_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;
  uint8
    rx_enable = FALSE,
    tx_enable = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_PAUSE_QUANTA_SET_UNSAFE);

  regs = soc_petra_regs();

  rx_enable = SOC_PETRA_IS_DIRECTION_REC(direction_ndx);
  tx_enable = SOC_PETRA_IS_DIRECTION_GEN(direction_ndx);

  fld_val = pause_quanta;
  if (rx_enable)
  {
    SOC_PA_FLD_ISET(regs->nif_mac_lane.llfc_pause_quanta_reg.llfc_pause_quanta, fld_val, mal_ndx, 10, exit);;
  }
  if (tx_enable)
  {
    SOC_PA_FLD_ISET(regs->nif_mac_lane.tx_pause_quanta_reg.tx_pause_quanta, fld_val, mal_ndx, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_pause_quanta_set_unsafe()",0,0);
}


/*********************************************************************
*     Function description
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_pause_quanta_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION        direction_ndx,
    SOC_SAND_IN  uint32                  pause_quanta
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_PAUSE_QUANTA_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    mal_ndx, SOC_PETRA_NOF_MAC_LANES-1,
    SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, SOC_PETRA_NOF_CONNECTION_DIRECTIONS-1,
    SOC_PETRA_NIF_DIRECTION_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_pause_quanta_verify()",0,0);
}

/*********************************************************************
*     Function description
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_pause_quanta_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_FC_DIRECTION        direction_ndx,
    SOC_SAND_OUT uint32                  *pause_quanta
  )
{
  uint32
    fld_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;
  uint8
    rx_enable = FALSE,
    tx_enable = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_PAUSE_QUANTA_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pause_quanta);
  rx_enable = SOC_PETRA_IS_DIRECTION_REC(direction_ndx);
  tx_enable = !(rx_enable);

  regs = soc_petra_regs();

  if (rx_enable)
  {
    SOC_PA_FLD_IGET(regs->nif_mac_lane.llfc_pause_quanta_reg.llfc_pause_quanta, fld_val, mal_ndx, 10, exit);
  }
  if (tx_enable)
  {
    SOC_PA_FLD_IGET(regs->nif_mac_lane.tx_pause_quanta_reg.tx_pause_quanta, fld_val, mal_ndx, 20, exit);
  }

  *pause_quanta = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_pause_quanta_get_unsafe()",0,0);
}

/*********************************************************************
*     Function description
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_pause_frame_src_addr_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS          *mac_addr
  )
{
  uint32
    res;
  uint32
    mac_addr_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_PAUSE_FRAME_SRC_ADDR_SET_UNSAFE);

  regs = soc_petra_regs();

  /* The function soc_sand_pp_mac_address_struct_to_long writes to indecies 0 and 1 of the second parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  res = soc_sand_pp_mac_address_struct_to_long(
          mac_addr,
          mac_addr_longs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PA_REG_ISET(regs->nif_mac_lane.tx_pause_frame_source_addr_reg_0, mac_addr_longs[0], mal_ndx, 20, exit);

  SOC_PA_REG_ISET(regs->nif_mac_lane.tx_pause_frame_source_addr_reg_1, mac_addr_longs[1], mal_ndx, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_pause_frame_src_addr_set_unsafe()",0,0);
}

/*********************************************************************
*     Function description
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_pause_frame_src_addr_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS          *mac_addr
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_PAUSE_FRAME_SRC_ADDR_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
      mal_ndx, SOC_PETRA_NOF_MAC_LANES-1,
      SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR, 10, exit
    );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_pause_frame_src_addr_verify()",0,0);
}

/*********************************************************************
*     Function description
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_pause_frame_src_addr_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS          *mac_addr
  )
{
  uint32
    res;
  uint32
    mac_addr_longs[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S];
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_PAUSE_FRAME_SRC_ADDR_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_addr);

  regs = soc_petra_regs();

  SOC_PA_REG_IGET(regs->nif_mac_lane.tx_pause_frame_source_addr_reg_0, mac_addr_longs[0], mal_ndx, 10, exit);

  SOC_PA_REG_IGET(regs->nif_mac_lane.tx_pause_frame_source_addr_reg_1, mac_addr_longs[1], mal_ndx, 20, exit);

  /* The function soc_sand_pp_mac_address_long_to_struct reads from indecies 0 and 1 of the first parameter only */
  /* coverity[overrun-buffer-val : FALSE] */   
  soc_sand_pp_mac_address_long_to_struct(
        mac_addr_longs,
        mac_addr
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_pause_frame_src_addr_get_unsafe()",0,0);
}

/*********************************************************************
*     Configures the way Networks Interface handles Flow
*     Control generation (TX) and reception (RX), link-level
*     and class-based.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_FC_INFO         *info
  )
{
  uint32
    flat_mode_fld_val = 0x0,
    fld_val,
    pf_fld_val,
    bct_fld_val,
    xon_fld_val,
    cb_fld_val,
    /*
     * RX configuration
     */
     rx_bct_llfc_fld_val = 0x0,
     /* rx_cbfc_fld_val = 0x0, - set upon init, not changed by the driver */
     rx_terminate_mac_ctrl_fld_val = 0x0,
     rx_terminate_pause_fld_val = 0x0,
     rx_fc_reg_val,
    res;
  uint32
    mal_idx,
    lane_idx;
  SOC_PETRA_NIF_TYPE
    nif_type;
  uint8
    is_channelized;
  SOC_PETRA_REGS
    *regs;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_FC_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  mal_idx = SOC_PETRA_NIF2MAL_NDX(nif_ndx);
  lane_idx = SOC_PETRA_NIF_SGMII_LANE(nif_ndx);

  res = soc_petra_nif_type_get_unsafe(
          unit,
          mal_idx,
          &nif_type,
          &is_channelized
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

  /************************************************************************/
  /* TX Flow Control                                                      */
  /************************************************************************/
  /*
   *  TX - Link Level
   *  BCT/Pause Frame
   */
  switch(info->tx.ll_mode) {
  case SOC_PETRA_NIF_LL_FC_MODE_DISABLE:
    pf_fld_val = 0x0;
    bct_fld_val = 0x0;
    break;
  case SOC_PETRA_NIF_LL_FC_MODE_ENABLE_PAUSE_FRAME:
    pf_fld_val = 0x1;
    bct_fld_val = 0x0;
    break;
  case SOC_PETRA_NIF_LL_FC_MODE_ENABLE_BCT:
    pf_fld_val = 0x0;
    bct_fld_val = 0x1;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_FC_LL_MODE_OUT_OF_RANGE_ERR, 5, exit);
    break;
  }

  SOC_PA_FLD_IGET(regs->nif_mac_lane.tx_flow_control_config_reg.tx_pause_frame, fld_val, mal_idx, 10, exit);
  SOC_SAND_SET_BIT(fld_val, pf_fld_val, lane_idx);
  pf_fld_val = fld_val;
  SOC_PA_FLD_ISET(regs->nif_mac_lane.tx_flow_control_config_reg.tx_pause_frame, pf_fld_val, mal_idx, 20, exit);

  SOC_PA_FLD_ISET(regs->nif_mac_lane.tx_flow_control_config_reg.tx_bct_llfc, bct_fld_val, mal_idx, 30, exit);

  /*
   *  TX - Link Level
   *  Clear Enable
   *  (Transmit On message)
   */
  xon_fld_val = SOC_SAND_BOOL2NUM(info->tx.on_if_cleared);
  SOC_PA_FLD_IGET(regs->nif_mac_lane.tx_flow_control_config_reg.xon_on_edge, fld_val, mal_idx, 40, exit);
  SOC_SAND_SET_BIT(fld_val, xon_fld_val, lane_idx);
  xon_fld_val = fld_val;
  SOC_PA_FLD_ISET(regs->nif_mac_lane.tx_flow_control_config_reg.xon_on_edge, xon_fld_val, mal_idx, 50, exit);

  /*
   *  Class Based - TX
   */
  cb_fld_val = SOC_SAND_BOOL2NUM(info->tx.cb_enable);
  SOC_PA_FLD_IGET(regs->nif_mac_lane.tx_flow_control_config2_reg.tx_cbfc, fld_val, mal_idx, 52, exit);
  SOC_SAND_SET_BIT(fld_val, cb_fld_val, lane_idx);
  cb_fld_val = fld_val;
  SOC_PA_FLD_ISET(regs->nif_mac_lane.tx_flow_control_config2_reg.tx_cbfc, cb_fld_val, mal_idx, 54, exit);


  /************************************************************************/
  /* RX Flow Control                                                      */
  /************************************************************************/

  /*
   *  RX - Link Level
   *  BCT/Pause Frame
   */
  switch(info->rx.ll_mode) {
  case SOC_PETRA_NIF_LL_FC_MODE_DISABLE:
    flat_mode_fld_val             = 0x1;
    break;
  case SOC_PETRA_NIF_LL_FC_MODE_ENABLE_PAUSE_FRAME:
    rx_terminate_mac_ctrl_fld_val = 0x1;
    rx_terminate_pause_fld_val    = 0x1;
    break;
  case SOC_PETRA_NIF_LL_FC_MODE_ENABLE_BCT:
    rx_bct_llfc_fld_val           = 0x1;
    break;
  default:
    break;
  }

  SOC_PA_REG_IGET(regs->nif_mac_lane.rx_flow_control_config_reg, rx_fc_reg_val, mal_idx, 58, exit);

  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.rx_flow_control_config_reg.rx_bct_llfc           ,rx_bct_llfc_fld_val 						,rx_fc_reg_val, 60, exit);
  /*
   * Disabled - RX-CBFC
   * SOC_PA_FLD_TO_REG(regs->nif_mac_lane.rx_flow_control_config_reg.rx_cbfc            ,rx_cbfc_fld_val 		 						,rx_fc_reg_val, 61, exit);
   */

  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.rx_flow_control_config_reg.flat_mode           ,fld_val 							          ,rx_fc_reg_val, 62, exit);
  SOC_SAND_SET_BIT(fld_val, flat_mode_fld_val, lane_idx);
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.rx_flow_control_config_reg.flat_mode             ,fld_val 							          ,rx_fc_reg_val, 63, exit);

  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.rx_flow_control_config_reg.rx_terminate_mac_ctrl ,rx_terminate_mac_ctrl_fld_val 	,rx_fc_reg_val, 66, exit);
  SOC_PA_FLD_TO_REG(regs->nif_mac_lane.rx_flow_control_config_reg.rx_terminate_pause    ,rx_terminate_pause_fld_val 		,rx_fc_reg_val, 68, exit);
  
  /*
   * Disabled - RX-CBFC
   * SOC_PA_FLD_TO_REG(regs->nif_mac_lane.rx_flow_control_config_reg.cfc_paused_indic_en ,cfc_paused_indic_en_fld_val 		,rx_fc_reg_val, 74, exit);
   */

  SOC_PA_REG_ISET(regs->nif_mac_lane.rx_flow_control_config_reg, rx_fc_reg_val, mal_idx, 80, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_fc_set_unsafe()",0,0);
}

/*********************************************************************
*     Configures the way Networks Interface handles Flow
*     Control generation (TX) and reception (RX), link-level
*     and class-based.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_FC_INFO         *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_FC_LL_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_nif_id_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->tx.ll_mode, SOC_PETRA_NIF_NOF_LL_FC_MODES-1,
    SOC_PETRA_NIF_FC_LL_MODE_OUT_OF_RANGE_ERR, 20, exit
  );

  SOC_SAND_ERR_IF_BELOW_MIN(
    info->rx.ll_mode, SOC_PETRA_NIF_LL_FC_MODE_DISABLE,
    SOC_PETRA_NIF_FC_RX_DISABLE_ERR, 30, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->rx.ll_mode, SOC_PETRA_NIF_NOF_LL_FC_MODES-1,
    SOC_PETRA_NIF_FC_LL_MODE_OUT_OF_RANGE_ERR, 40, exit
  );

  if (info->rx.treat_ll_class_based)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DEPRICATED_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_fc_verify()",0,0);
}

/*********************************************************************
*     Configures the way Networks Interface handles Flow
*     Control generation (TX) and reception (RX), link-level
*     and class-based.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_FC_INFO         *info
  )
{
  uint32
    fld_val,
    pf_fld_val,
    bct_fld_val,
    xon_fld_val,
    cb_fld_val,
    flat_mode_fld_val,
    res;
  uint32
    mal_idx,
    lane_idx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_FC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  mal_idx = SOC_PETRA_NIF2MAL_NDX(nif_ndx);
  lane_idx = SOC_PETRA_NIF_SGMII_LANE(nif_ndx);

  /*
   *  TX - Link Level
   *  BCT/Pause Frame
   */

  SOC_PA_FLD_IGET(regs->nif_mac_lane.tx_flow_control_config_reg.tx_pause_frame, fld_val, mal_idx, 10, exit);
  pf_fld_val = SOC_SAND_GET_BIT(fld_val, lane_idx);

  SOC_PA_FLD_IGET(regs->nif_mac_lane.tx_flow_control_config_reg.tx_bct_llfc, bct_fld_val, mal_idx, 20, exit);

  if (SOC_SAND_NUM2BOOL(pf_fld_val) == TRUE)
  {
    info->tx.ll_mode = SOC_PETRA_NIF_LL_FC_MODE_ENABLE_PAUSE_FRAME;
  }
  else if (SOC_SAND_NUM2BOOL(bct_fld_val) == TRUE)
  {
    info->tx.ll_mode = SOC_PETRA_NIF_LL_FC_MODE_ENABLE_BCT;
  }
  else
  {
    info->tx.ll_mode = SOC_PETRA_NIF_LL_FC_MODE_DISABLE;
  }

  /*
   *  TX - Link Level
   *  Clear Enable
   *  (Transmit On message)
   */
  SOC_PA_FLD_IGET(regs->nif_mac_lane.tx_flow_control_config_reg.xon_on_edge, fld_val, mal_idx, 30, exit);
  xon_fld_val = SOC_SAND_GET_BIT(fld_val, lane_idx);
  info->tx.on_if_cleared = SOC_SAND_NUM2BOOL(xon_fld_val);

  /*
   *  Class Based - TX
   */
  SOC_PA_FLD_IGET(regs->nif_mac_lane.tx_flow_control_config2_reg.tx_cbfc, fld_val, mal_idx, 34, exit);
  cb_fld_val = SOC_SAND_GET_BIT(fld_val, lane_idx);
  info->tx.cb_enable = SOC_SAND_NUM2BOOL(cb_fld_val);

  /*
   *  RX
   */
  SOC_PA_FLD_IGET(regs->nif_mac_lane.rx_flow_control_config_reg.flat_mode, fld_val, mal_idx, 42, exit);
  flat_mode_fld_val = SOC_SAND_GET_BIT(fld_val, lane_idx);

  if (flat_mode_fld_val)
  {
    info->rx.ll_mode = SOC_PETRA_NIF_LL_FC_MODE_DISABLE;
  }
  else
  {
    SOC_PA_FLD_IGET(regs->nif_mac_lane.rx_flow_control_config_reg.rx_bct_llfc, bct_fld_val, mal_idx, 40, exit);
    info->rx.ll_mode = (bct_fld_val == 0x1)?SOC_PETRA_NIF_LL_FC_MODE_ENABLE_BCT:SOC_PETRA_NIF_LL_FC_MODE_ENABLE_PAUSE_FRAME;
  }

  info->rx.treat_ll_class_based = FALSE; /* Deprecated */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_fc_get_unsafe()", nif_ndx, 0);
}

/*********************************************************************
*     Set all Network Interface SerDes in loopback.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_loopback_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  uint8                 enable
  )
{
  uint32
    res;
  uint32
    srd_lane_id,
    base_lane_id,
    mal_ndx,
    nof_srd_lanes;
  SOC_PETRA_SRD_LANE_LOOPBACK_MODE
    lpbk_mode;
  SOC_PETRA_NIF_LINK_STATUS
    link_status;
  uint8
    is_sgmii,
    is_enabled_rx,
    is_enabled_tx,
    is_enabled_both = FALSE;
  SOC_PETRA_CONNECTION_DIRECTION
    direction_ndx = SOC_PETRA_CONNECTION_DIRECTION_BOTH;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_LOOPBACK_SET_UNSAFE);

  soc_petra_PETRA_NIF_LINK_STATUS_clear(&link_status);

  soc_petra_nif_srd_lanes_get(unit, nif_ndx, &nof_srd_lanes, &base_lane_id);

  if (nof_srd_lanes == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INVALID_TYPE_ERR, 10, exit);
  }

  lpbk_mode = (enable == TRUE)?SOC_PETRA_SRD_LANE_LOOPBACK_NSILB:SOC_PETRA_SRD_LANE_LOOPBACK_NONE;
  mal_ndx = SOC_PETRA_NIF2MAL_NDX(nif_ndx);

  res = soc_petra_nif_is_sgmii_get(
          unit,
          mal_ndx,
          &is_sgmii
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /*
   *  Check if the NIF is currently enabled - store current state.
   *  Disable the NIF
   */
  if (is_sgmii)
  {
    res = soc_petra_nif_sgmii_enable_state_get(
            unit,
            nif_ndx,
            &is_enabled_rx,
            &is_enabled_tx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (is_enabled_rx && is_enabled_tx)
    {
      direction_ndx = SOC_PETRA_CONNECTION_DIRECTION_BOTH;
      is_enabled_both = TRUE;
    }
    else if (is_enabled_rx)
    {
      direction_ndx = SOC_PETRA_CONNECTION_DIRECTION_RX;
    }
    else if (is_enabled_tx)
    {
      direction_ndx = SOC_PETRA_CONNECTION_DIRECTION_TX;
    }
    else
    {
      direction_ndx = SOC_PETRA_NOF_CONNECTION_DIRECTIONS;
    }

    res = soc_petra_nif_sgmii_enable_state_set(
            unit,
            nif_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_BOTH,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }
  else
  {
    res = soc_petra_nif_mal_enable_get_unsafe(
            unit,
            mal_ndx,
            &is_enabled_both
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_petra_nif_mal_enable_set_unsafe(
              unit,
              mal_ndx,
              FALSE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
  }

  /*
   *  Set the NIF SerDes
   */
  for (srd_lane_id = base_lane_id; srd_lane_id < (base_lane_id + nof_srd_lanes); srd_lane_id++)
  {
    res = soc_petra_srd_lane_loopback_mode_verify(
            unit,
            srd_lane_id,
            lpbk_mode
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_petra_srd_lane_loopback_mode_set_unsafe(
            unit,
            srd_lane_id,
            lpbk_mode
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  }

  /*
   *  Revert the NIF enable state
   */

  if (is_sgmii)
  {
    if (direction_ndx != SOC_PETRA_NOF_CONNECTION_DIRECTIONS)
    {
      /*
       *  Enable RX/TX as before the API call
       */
      res = soc_petra_nif_sgmii_enable_state_set(
              unit,
              nif_ndx,
              direction_ndx,
              TRUE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  }
  else
  {
    if (is_enabled_both)
    {
      res = soc_petra_nif_mal_enable_set_unsafe(
              unit,
              mal_ndx,
              TRUE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_loopback_set_unsafe()",0,0);
}

/*********************************************************************
*     Set all Network Interface SerDes in loopback.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_loopback_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_IN  uint8                 enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_LOOPBACK_VERIFY);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_petra_nif_id_verify(
          unit,
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_loopback_verify()",0,0);
}

/*********************************************************************
*     Set all Network Interface SerDes in loopback.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_nif_loopback_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        nif_ndx,
    SOC_SAND_OUT uint8                 *enable
  )
{
  uint32
    res;
  uint32
    srd_lane_id,
    base_lane_id,
    nof_srd_lanes;
  SOC_PETRA_SRD_LANE_LOOPBACK_MODE
    lpbk_mode = SOC_PETRA_SRD_LANE_LOOPBACK_NONE,
    lpbk_mode_first= SOC_PETRA_SRD_LANE_LOOPBACK_NONE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_LOOPBACK_GET_UNSAFE);

  soc_petra_nif_srd_lanes_get(unit, nif_ndx, &nof_srd_lanes, &base_lane_id);

  if (nof_srd_lanes == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_INVALID_TYPE_ERR, 10, exit);
  }

  for (srd_lane_id = base_lane_id; srd_lane_id < (base_lane_id + nof_srd_lanes); srd_lane_id++)
  {
    res = soc_petra_srd_lane_loopback_mode_get_unsafe(
            unit,
            srd_lane_id,
            &lpbk_mode
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (srd_lane_id == base_lane_id)
    {
      lpbk_mode_first = lpbk_mode;
    }
    else
    {
      if (lpbk_mode != lpbk_mode_first)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_SRD_LPBCK_MODE_INCONSISTENT_ERR, 30, exit);
      }
    }
  }

  *enable = (lpbk_mode == SOC_PETRA_SRD_LANE_LOOPBACK_NONE)?FALSE:TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_loopback_get_unsafe()",0,0);
}

/*
 *  Translates from the counter type enumerator index
 *  to the internal HW representation counter index
 */
STATIC
  uint32
  soc_petra_nif_counter_type_to_val(
    SOC_SAND_IN   SOC_PETRA_NIF_COUNTER_TYPE count_type
  )
{
  uint32
    cnt_internal;

  switch(count_type) {
  case SOC_PETRA_NIF_RX_OK_PACKETS:
    cnt_internal = 0x0;
    break;
  case SOC_PETRA_NIF_RX_OK_OCTETS:
    cnt_internal = 0x1;
    break;
  case SOC_PETRA_NIF_TX_OK_NORMAL_PACKETS:
    cnt_internal = 0x24;
    break;
  case SOC_PETRA_NIF_TX_OK_OCTETS:
    cnt_internal = 0x20;
    break;
  default:
    if (count_type <= SOC_PETRA_NIF_RX_PACKETS_WITH_BCT_ERR)
    {
      cnt_internal = count_type + 4;
    }
    else if (count_type <= SOC_PETRA_NIF_RX_BCAST_PACKETS)
    {
      cnt_internal = count_type + 5;
    }
    else if (count_type <= SOC_PETRA_NIF_RX_PACKET_LEN_BINS_TOO_LONG)
    {
      cnt_internal = count_type + 9;
    }
    else
    {
      cnt_internal = count_type + 21;
    }
  }

  return cnt_internal;
}

/*
 * sets the overflow of the counter if counter reached maximum value.
 */
STATIC void
  soc_petra_nif_counter_overflow_set(
    SOC_SAND_IN   SOC_PETRA_NIF_COUNTER_TYPE count_type,
    SOC_SAND_INOUT SOC_SAND_64CNT                   *counter_val
  )
{
  SOC_SAND_U64
    max_val;
  uint8
    nof_bits = Soc_petra_nif_counter_nof_bits[count_type],
    first_word_bits;
  soc_sand_u64_clear(&max_val);

  if (nof_bits > SOC_SAND_NOF_BITS_IN_UINT32)
  {
    first_word_bits = SOC_SAND_NOF_BITS_IN_UINT32;
    nof_bits -= SOC_SAND_NOF_BITS_IN_UINT32;
  }
  else
  {
    first_word_bits = nof_bits;
    nof_bits = 0;
  }
  max_val.arr[0] = SOC_SAND_BITS_MASK(first_word_bits - 1, 0);
  if (nof_bits > 0)
  {
    max_val.arr[1] = SOC_SAND_BITS_MASK(nof_bits-1, 0);
  }
  counter_val->overflowed =
    (
      (soc_sand_u64_are_equal(&(counter_val->u64), &max_val)) ||
      (soc_sand_u64_is_bigger(&(counter_val->u64), &max_val))
    );
}



uint32
  soc_petra_nif_id_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID           nif_ndx
  )
{
  uint32
    mal_ndx;
  uint8
    is_sgmii;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_INTERFACE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    nif_ndx, SOC_PETRA_IF_NOF_NIFS-1,
    SOC_PETRA_NIF_INDEX_OUT_OF_RANGE_ERR, 10, exit
  );

  mal_ndx = SOC_PETRA_NIF2MAL_NDX(nif_ndx);

  res = soc_petra_nif_is_sgmii_get(
          unit,
          mal_ndx,
          &is_sgmii
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  if (is_sgmii == FALSE)
  {
    if (soc_petra_is_channelized_interface_id(nif_ndx) == FALSE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INCOMPATABLE_NIF_ID_ERR, 30, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_id_verify()",0,0);
}

/*********************************************************************
*     Gets counter value of the NIF.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_counter_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID           nif_ndx,
    SOC_SAND_IN  SOC_PETRA_NIF_COUNTER_TYPE       counter_type,
    SOC_SAND_OUT SOC_SAND_64CNT                   *counter_val
  )
{
  SOC_PETRA_REGS
    *regs;
  uint32
    counter_index,
    reg_val,
    fld_val;
  uint32
    lane_ndx,
    mal_ndx;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_COUNTER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(counter_val);

  regs = soc_petra_regs();

  if (counter_type >= SOC_PETRA_NIF_NOF_COUNTERS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_COUNTER_OUT_OF_RANGE_ERR, 10, exit);
  }

  mal_ndx  = SOC_PETRA_NIF2MAL_NDX(nif_ndx);
  lane_ndx = SOC_PETRA_NIF_SGMII_LANE(nif_ndx);

 /*
  * if this is FIFO over flow counter then simply read it,
  */
  if (counter_type == SOC_PETRA_NIF_RX_DISC_FIFO_OVF)
  {
    switch(lane_ndx)
    {
    case 0:
      SOC_PA_REG_IGET(regs->nif_mac_lane.stat_rx_discards_ln0_reg, counter_val->u64.arr[0], mal_ndx, 20, exit);
      break;
    case 1:
      SOC_PA_REG_IGET(regs->nif_mac_lane.stat_rx_discards_ln1_reg, counter_val->u64.arr[0], mal_ndx, 30, exit);
      break;
    case 2:
      SOC_PA_REG_IGET(regs->nif_mac_lane.stat_rx_discards_ln2_reg, counter_val->u64.arr[0], mal_ndx, 40, exit);
      break;
    case 3:
      SOC_PA_REG_IGET(regs->nif_mac_lane.stat_rx_discards_ln3_reg, counter_val->u64.arr[0], mal_ndx, 50, exit);
      break;
    }
  }
  else
  {
   /*
    * for the rest of the counters use the HW counters selector.
    */
    counter_index = soc_petra_nif_counter_type_to_val(counter_type);
    reg_val = 0;

    fld_val = lane_ndx;
    SOC_PA_FLD_TO_REG(regs->nif_mac_lane.statistics_rd_select_reg.stat_rd_port_sel, fld_val, reg_val, 60, exit);

    SOC_PA_FLD_TO_REG(regs->nif_mac_lane.statistics_rd_select_reg.stat_rd_cnt_sel, counter_index, reg_val, 70, exit);

    SOC_PA_REG_ISET(regs->nif_mac_lane.statistics_rd_select_reg, reg_val, mal_ndx, 80, exit);

    soc_sand_os_nano_sleep(SOC_PETRA_NIF_WAIT_NS,NULL);
    SOC_PA_REG_IGET(regs->nif_mac_lane.statistics_rd_msb_reg, counter_val->u64.arr[1], mal_ndx, 90, exit);
    SOC_PA_REG_IGET(regs->nif_mac_lane.statistics_rd_lsb_reg, counter_val->u64.arr[0], mal_ndx, 100, exit);
  }
  /* set over Flow indication */
  soc_petra_nif_counter_overflow_set(counter_type,counter_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_counter_get()",0,0);
}


/*********************************************************************
*     Gets counter value of the NIF.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_all_counters_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID           nif_ndx,
    SOC_SAND_OUT SOC_SAND_64CNT                   counters_val[SOC_PETRA_NIF_NOF_COUNTERS]
  )
{
  uint32
    counter_index;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_COUNTER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(counters_val);

  for (counter_index = 0; counter_index < SOC_PETRA_NIF_NOF_COUNTERS; ++counter_index)
  {
    soc_sand_64cnt_clear(&(counters_val[counter_index]));
    res = soc_petra_nif_counter_get_unsafe(
            unit,
            nif_ndx,
            (SOC_PETRA_NIF_COUNTER_TYPE)counter_index,
            &(counters_val[counter_index])
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_all_counters_get_unsafe()",0,0);
}




/*********************************************************************
*     Gets counter value of the NIF.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_all_nifs_all_counters_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT SOC_SAND_64CNT                   counters_val[SOC_PETRA_IF_NOF_NIFS][SOC_PETRA_NIF_NOF_COUNTERS]
  )
{
  SOC_PETRA_INTERFACE_ID
    if_index;
  SOC_PETRA_NIF_TYPE
    nif_type;
  uint32
    counter_indx;
  uint8
    is_channelized;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_ALL_NIFS_ALL_COUNTERS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(counters_val);

 /*
  * Get NIF Port type
  */

  for (if_index = SOC_PETRA_IF_ID_0; if_index < SOC_PETRA_IF_NOF_NIFS; ++if_index)
  {
    for (counter_indx = 0; counter_indx < SOC_PETRA_NIF_NOF_COUNTERS; ++counter_indx)
    {
      soc_sand_64cnt_clear(&(counters_val[if_index][counter_indx]));
    }
  }


  for (if_index = SOC_PETRA_IF_ID_0; if_index < SOC_PETRA_IF_NOF_NIFS; ++if_index)
  {
    if (!SOC_PETRA_IS_MAL_NIF_ID(if_index))
    {
      res = soc_petra_nif_type_get_unsafe(
              unit,
              SOC_PETRA_NIF2MAL_NDX(if_index),
              &nif_type,
              &is_channelized
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
      if ((nif_type != SOC_PETRA_NIF_TYPE_SGMII))
      {
        continue;
      }
    }
    res = soc_petra_nif_all_counters_get_unsafe(
            unit,
            if_index,
            counters_val[if_index]
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_all_counters_get_unsafe()",0,0);
}
/*********************************************************************
*     Gets link status, and whether was change in the status.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_nif_link_status_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID           nif_ndx,
    SOC_SAND_OUT SOC_PETRA_NIF_LINK_STATUS        *link_status
  )
{

  SOC_PETRA_REGS
    *regs;
  uint32
    fld_val,
    reg_val;
  uint32
    mal_ndx,
    lane_ndx;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_LINK_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(link_status);

  regs = soc_petra_regs();

  mal_ndx  = SOC_PETRA_NIF2MAL_NDX(nif_ndx);
  lane_ndx = SOC_PETRA_NIF_SGMII_LANE(nif_ndx);

 /*
  * Get the link status
  */
  SOC_PA_FLD_IGET(regs->nif_mac_lane.link_status_reg.link_status, fld_val, mal_ndx, 20, exit);
  link_status->link_status = (uint8) (fld_val & SOC_SAND_BIT(lane_ndx));

 /*
  * Get changes in the link status
  */
  SOC_PA_REG_IGET(regs->nif_mac_lane.interrupt_reg, reg_val, mal_ndx, 30, exit);

  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.interrupt_reg.link_status_change_int, fld_val, reg_val, 40, exit);
  link_status->link_status_change = (uint8) (fld_val & SOC_SAND_BIT(lane_ndx));

  SOC_PA_FLD_FROM_REG(regs->nif_mac_lane.interrupt_reg.rx_pcs_ln_sync_stat_change_int, fld_val, reg_val, 50, exit);
  link_status->pl_status_change = (uint8) (fld_val & SOC_SAND_BIT(lane_ndx));
 /*
  * Clear changes in the link status
  */
  SOC_PA_REG_ISET(regs->nif_mac_lane.interrupt_reg, 0xFF, mal_ndx, 60, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_link_status_get_unsafe()",0,0);
}



uint32
  soc_petra_nif_is_channelized_unsafe(
    SOC_SAND_IN   int         unit,
    SOC_SAND_IN   SOC_PETRA_INTERFACE_ID nif_ndx,
    SOC_SAND_OUT  uint8         *is_channelized
  )
{
  uint32
    res;
  uint8
    is_channelized_interface = FALSE;
  SOC_PETRA_NIF_TYPE
    nif_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_NIF_IS_CHANNELIZED_UNSAFE);

  /* NIF */
  res = soc_petra_nif_type_get_unsafe(
          unit,
          SOC_PETRA_NIF2MAL_NDX(nif_ndx),
          &nif_type,
          &is_channelized_interface
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *is_channelized = is_channelized_interface;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_nif_is_channelized_unsafe()",0,0);
}

int
soc_petra_port_init(int unit)
{  
    return 0;  
}

int 
soc_petra_port_post_init(int unit, soc_pbmp_t* ports) 
{
    return 0;
}

int
soc_petra_port_deinit(int unit)
{  
    return 0;  
}

int 
soc_petra_port_enable_set(int unit, soc_port_t port, int enable) 
{
    int                     core;    
    SOC_PETRA_INTERFACE_ID  nif_idx = 0;
    uint32 tm_port, link;
    SOC_PB_NIF_STATE_INFO state_info_pb;
    SOC_TMC_LINK_STATE_INFO link_state;

    if (SOC_PBMP_MEMBER(PBMP_SFI_ALL(unit), port)) {
        link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        SOC_TMC_LINK_STATE_INFO_clear(&link_state);
        link_state.on_off = enable ? SOC_TMC_LINK_STATE_ON : SOC_TMC_LINK_STATE_OFF;
        link_state.serdes_also = TRUE;
        SOC_SAND_IF_ERR_RETURN(soc_petra_link_on_off_set(unit, link, &link_state));
    } else if (SOC_PBMP_MEMBER(PBMP_PORT_ALL(unit), port)) {

        SOC_IF_ERROR_RETURN(petra_soc_dpp_local_to_tm_port_get(unit, port, &tm_port, &core));
        SOC_IF_ERROR_RETURN(soc_petra_port_to_interface_map_get_dispatch(unit, tm_port, core, &nif_idx, NULL));
        
        if (nif_idx == SOC_TMC_IF_ID_NONE) {
            return SOC_E_PARAM;
        }

        SOC_PB_NIF_STATE_INFO_clear(&state_info_pb);
        state_info_pb.is_nif_on = SOC_SAND_NUM2BOOL(enable);
        state_info_pb.serdes_also = FALSE;
      
        SOC_SAND_IF_ERR_RETURN(soc_pb_nif_on_off_set(unit, nif_idx, &state_info_pb)); 
    } else {
        return SOC_E_PORT;
    }

    return SOC_E_NONE;
}

int 
soc_petra_port_enable_get(int unit, soc_port_t port, int* enable) 
{
    int                     core;    
    SOC_PETRA_INTERFACE_ID  nif_idx = 0;
    uint32                  tm_port, link;
    SOC_PB_NIF_STATE_INFO   state_info_pb;
    SOC_PETRA_LINK_STATE_INFO link_state;

    if (SOC_PBMP_MEMBER(PBMP_SFI_ALL(unit), port)) {
        link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        SOC_SAND_IF_ERR_RETURN(soc_petra_link_on_off_get(unit, link, &link_state));
        *enable = (link_state.on_off == SOC_TMC_LINK_STATE_ON ? 1 : 0) && link_state.serdes_also;
    } else if (SOC_PBMP_MEMBER(PBMP_PORT_ALL(unit), port)) {
        SOC_IF_ERROR_RETURN(petra_soc_dpp_local_to_tm_port_get(unit, port, &tm_port, &core));
        SOC_IF_ERROR_RETURN(soc_petra_port_to_interface_map_get_dispatch(unit, tm_port, core, &nif_idx, NULL));
         
        if (SOC_PB_NIF_IS_VALID_ID(nif_idx) == FALSE) {
            return SOC_E_PARAM;
        }

        SOC_PB_NIF_STATE_INFO_clear(&state_info_pb);
        SOC_SAND_IF_ERR_RETURN(soc_pb_nif_on_off_get(unit, nif_idx, &state_info_pb)); 
        *enable = state_info_pb.is_nif_on ? 1 : 0;
    } else {
        return SOC_E_PORT;
    }

    return SOC_E_NONE;
}

int
soc_petra_port_speed_set(int unit, bcm_port_t port, int speed)
{
    SOC_PB_NIF_GMII_RATE    requested_rate;   
    uint32 tm_port;
    int  core;
    SOC_PETRA_INTERFACE_ID nif_id;

    SOC_IF_ERROR_RETURN(petra_soc_dpp_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOC_IF_ERROR_RETURN(soc_petra_port_to_interface_map_get_dispatch(unit, tm_port, core, &nif_id, NULL));

    if (SOC_PB_NIF_IS_TYPE_ID(SGMII, nif_id) || SOC_PB_NIF_IS_TYPE_ID(QSGMII, nif_id)) {
        switch (speed) {
        case 10:
            requested_rate = SOC_PB_NIF_GMII_RATE_10Mbps;
            break;

        case 100:
            requested_rate = SOC_PB_NIF_GMII_RATE_100Mbps;
            break;

        case 1000:
            requested_rate = SOC_PB_NIF_GMII_RATE_1000Mbps;
            break;

        default:
            return SOC_E_CONFIG;
        }
        SOC_SAND_IF_ERR_RETURN(soc_pb_nif_gmii_rate_set(unit, nif_id, requested_rate));
    } 

    return SOC_E_NONE;
}

STATIC int
_bcm_petra_gmii_autoneg_speed_get(int unit, int nif_id, int *speed)
{
    int         soc_sand_rv;
    int         nif_inner_id;
    int         mal_id;
    int         inner_id;
    uint32    autoneg_status;
    uint32    autoneg_complete;
    uint32    link_status;
    uint32    rx_cfg_reg;
    SOC_PB_REGS     *regs;

    regs = soc_pb_regs();

    if (SOC_PB_NIF_IS_TYPE_ID(SGMII, nif_id)) {
        nif_inner_id = SOC_PB_SGMII2GLBL_ID(SOC_PB_NIF_ID_OFFSET(SGMII, nif_id));
    } else if (SOC_PB_NIF_IS_TYPE_ID(QSGMII, nif_id)) {
        nif_inner_id = SOC_PB_NIF_ID_OFFSET(QSGMII, nif_id);
    } else {
        return SOC_E_PARAM;
    }
    mal_id   = SOC_PB_NIF2MAL_GLBL_ID(nif_inner_id);
    inner_id = SOC_PB_NIF_GMII_LANE_INTERN(nif_inner_id);

    soc_sand_rv = soc_petra_read_reg(unit,
                  &regs->nif_mac_lane.auto_neg_status_reg[inner_id].addr,
                  mal_id, &autoneg_status);
    SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

    soc_sand_rv = soc_petra_field_from_reg_get(
                  &autoneg_status,
                  &regs->nif_mac_lane.auto_neg_status_reg[inner_id].an_complete,
                  &autoneg_complete);
    SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

    soc_sand_rv = soc_petra_field_from_reg_get(
                  &autoneg_status,
                  &regs->nif_mac_lane.auto_neg_status_reg[inner_id].an_link_status,
                  &link_status);
    SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

    soc_sand_rv = soc_petra_field_from_reg_get(
                  &autoneg_status,
                  &regs->nif_mac_lane.auto_neg_status_reg[inner_id].an_rx_cfg_reg,
                  &rx_cfg_reg);
    SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

    if (autoneg_complete && link_status == 1) {
        switch ((rx_cfg_reg >> 10) & 0x3) {
        case 0x0:
            *speed = 10;
            break;

        case 0x1:
            *speed = 100;
            break;

        case 0x2:
            *speed = 1000;
            break;

        default:
            return SOC_E_INTERNAL;
        }
    } else {
        *speed = 0;
    }

    return SOC_E_NONE;
}

int 
soc_petra_port_speed_get(int unit, soc_port_t port, int* speed)
{
    uint32 tm_port;
    int  core;
    SOC_PETRA_INTERFACE_ID nif_id;
    SOC_PB_NIF_GMII_RATE reported_rate;

    SOC_IF_ERROR_RETURN(petra_soc_dpp_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOC_IF_ERROR_RETURN(soc_petra_port_to_interface_map_get_dispatch(unit, tm_port, core, &nif_id, NULL));

    /* XAUI and RXAUI are always 10Gbps */
    if (SOC_PB_NIF_IS_TYPE_ID(XAUI, nif_id) || SOC_PB_NIF_IS_TYPE_ID(RXAUI, nif_id)) {
        *speed = 10000;
    } else if (SOC_PB_NIF_IS_TYPE_ID(SGMII, nif_id) || SOC_PB_NIF_IS_TYPE_ID(QSGMII, nif_id)) {
      SOC_IF_ERROR_RETURN(soc_pb_nif_gmii_rate_get(unit, nif_id, &reported_rate));

      switch (reported_rate) {
      case SOC_PB_NIF_GMII_RATE_10Mbps:
          *speed = 10;
          break;

      case SOC_PB_NIF_GMII_RATE_100Mbps:
          *speed = 100;
          break;

      case SOC_PB_NIF_GMII_RATE_1000Mbps:
          *speed = 1000;
          break;

      case SOC_PB_NIF_GMII_RATE_AUTONEG:
          /* If the auto-negotiation is enabled, we should get the actual rate */
          SOC_IF_ERROR_RETURN(_bcm_petra_gmii_autoneg_speed_get(unit, nif_id, speed));
          break;

      default:
          return SOC_E_INTERNAL;
      }
    }

    return SOC_E_NONE;
}

int 
soc_petra_port_interface_get(int unit, soc_port_t port, soc_port_if_t* intf)
{
    uint32 tm_port;
    int  core;
    SOC_PETRA_INTERFACE_ID nif_idx;

    SOC_IF_ERROR_RETURN(petra_soc_dpp_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOC_IF_ERROR_RETURN(soc_petra_port_to_interface_map_get_dispatch(unit, tm_port, core, &nif_idx, NULL));

    if ((nif_idx >= SOC_TMC_NIF_ID_XAUI_0) && (nif_idx <= SOC_TMC_NIF_ID_XAUI_7)) {
        *intf = SOC_PORT_IF_XAUI;
    } else if ((nif_idx >= SOC_TMC_NIF_ID_RXAUI_0) && (nif_idx <= SOC_TMC_NIF_ID_RXAUI_15)) {
        *intf = SOC_PORT_IF_RXAUI;
    } else if ((nif_idx >= SOC_TMC_NIF_ID_SGMII_0) && (nif_idx <= SOC_TMC_NIF_ID_SGMII_31)) {
        *intf = SOC_PORT_IF_SGMII;
    } else if ((nif_idx >= SOC_TMC_NIF_ID_QSGMII_0) && (nif_idx <= SOC_TMC_NIF_ID_QSGMII_63)) {
        *intf = SOC_PORT_IF_QSGMII;
    } else if ((nif_idx >= SOC_TMC_NIF_ID_ILKN_0) && (nif_idx <= SOC_TMC_NIF_ID_ILKN_1)) {
        *intf = SOC_PORT_IF_ILKN;
    } else if ((nif_idx >= SOC_TMC_NIF_ID_CGE_0) && (nif_idx <= SOC_TMC_NIF_ID_CGE_1)) {
        *intf = SOC_PORT_IF_CAUI;
    } else if ((nif_idx >= SOC_TMC_NIF_ID_XLGE_0) && (nif_idx <= SOC_TMC_NIF_ID_XLGE_7)) {
        *intf = SOC_PORT_IF_KR4;
    } else if ((nif_idx >= SOC_TMC_NIF_ID_10GBASE_R_0) && (nif_idx <= SOC_TMC_NIF_ID_10GBASE_R_31)) {
        *intf = SOC_PORT_IF_KR;
    } else {
        *intf = -1;
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;

}

int
soc_petra_port_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence)
{
    int port;

    SOC_PBMP_CLEAR(*okay_pbmp);
    PBMP_ITER(pbmp, port) {
        if (SOC_PBMP_MEMBER(PBMP_XE_ALL(unit),port)) {
            SOC_PBMP_PORT_ADD(*okay_pbmp, port);
        }
    }

    return SOC_E_NONE;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>
