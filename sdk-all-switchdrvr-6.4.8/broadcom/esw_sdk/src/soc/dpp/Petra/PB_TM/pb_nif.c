/* $Id: pb_nif.c,v 1.13 Broadcom SDK $
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

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_nif.h>
#include <soc/dpp/Petra/PB_TM/pb_flow_control.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_serdes_utils.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_ports.h>

#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PB_NIF_MIN_PCKT_SIZE_MAX                               (255)
#define SOC_PB_NIF_RATE_MAX                                        (SOC_PB_NIF_NOF_GMII_RATES-1)
#define SOC_PB_NIF_CLK_NDX_MAX                                     (SOC_PB_NIF_NOF_SYNCE_CLK_IDS-1)
#define SOC_PB_NIF_SYNCE_MODE_MAX                                  (SOC_PB_NIF_NOF_SYNCE_MODES-1)
#define SOC_PB_FATP_MODE_MAX                                       (SOC_PB_NIF_NOF_FATP_MODES-1)
#define SOC_PB_NIF_FATP_NDX_MAX                                    (SOC_PB_NIF_NOF_FATP_IDS-1)
#define SOC_PB_NIF_COUNTER_TYPE_MAX                                (SOC_PB_NIF_NOF_COUNTERS-1)
#define SOC_PB_NIF_PREAMBLE_COMPRESS_SIZE_MAX                      (SOC_PB_NIF_NOF_PRMBL_SIZES-1)
#define SOC_PB_NIF_IPG_COMPRESS_SIZE_MIN                           (1)
#define SOC_PB_NIF_IPG_COMPRESS_SIZE_MAX                           (255)
#define SOC_PB_NIF_IPG_COMPRESS_DIC_MODE_MAX                       (SOC_PB_NIF_NOF_IPG_DIC_MODES-1)
#define SOC_PB_NIF_CHANNELIZED_BCT_SIZE_MAX                        (2)
#define SOC_PB_NIF_CHANNELIZED_BCT_CHANNEL_BYTE_NDX_MAX            (3)
#define SOC_PB_NIF_FR_INFO_LOCAL_MAX                               (SOC_PB_NIF_NOF_FAULT_RESPONSES-1)
#define SOC_PB_NIF_SPAUI_INFO_CRC_MODE_MAX                         (SOC_PB_NIF_NOF_CRC_MODES-1)
#define SOC_PB_NIF_GMII_INFO_MODE_MAX                              (SOC_PB_NIF_NOF_1GE_MODES-1)
#define SOC_PB_NIF_GMII_INFO_RATE_MAX                              (SOC_PB_NIF_NOF_GMII_RATES-1)
#define SOC_PB_NIF_GMII_STAT_FAULT_TYPE_MAX                        (SOC_PB_NIF_NOF_GMII_RFS-1)
#define SOC_PB_NIF_ILKN_A_NOF_LANES_MIN                            (8)
#define SOC_PB_NIF_ILKN_B_NOF_LANES_MIN                            (4)
#define SOC_PB_NIF_SYNCE_CLK_CLK_DIVIDER_MAX                       (SOC_PB_NIF_NOF_SYNCE_CLK_DIVS-1)
/* 72[hrs] */
/* 72[hrs] */
#define SOC_PB_NIF_WIRE_DELAY_NS_MAX                               (SOC_SAND_U32_MAX)

/* $Id: pb_nif.c,v 1.13 Broadcom SDK $
 * Used in the cases where the actual NIF type is required as a parameter,
 * but in the particular case is not important
 */
#define SOC_PB_NIF_TYPE_DONT_CARE                                  SOC_PB_NIF_TYPE_XAUI

/* Use to define a different exit-place for RX/TX error indication */

#define SOC_PB_NIF_ILKN_BURST_MAX_256B_VAL      0x3
#define SOC_PB_NIF_ILKN_TX_BURST_SHORT_64B_VAL  0x1

/* The index of the QSRD that can be potentially used by either ILKN or ELK */
#define SOC_PB_NIF_ILKN_ELK_QSRD_OVERLAP_ID     6


/*
 *    ILKN
 */
#define SOC_PB_NIF_ILKN0_LANE_MIN   0
#define SOC_PB_NIF_ILKN1_LANE_MIN   16

#define SOC_PB_NIF_ILKN0_MAX_LANES_NO_OVRLP 12

/* Debug flag, enables XMIT override if set */
#define SOC_PB_NIF_GMII_XMIT_OVRD_EN 0

/*
 *    Maximal number of ports according to the basic Fat-Pipe mode
 */
#define  SOC_PB_NIF_FATP_2X6_PORT_MAX (SOC_PB_NIF_FATP_PORT_MAX/2)
#define  SOC_PB_NIF_FATP_3X4_PORT_MAX (SOC_PB_NIF_FATP_PORT_MAX/3)

/* NIF-counter-read delay */
#define SOC_PB_NIF_CNTR_WAIT_NS  30


#define SOC_PB_NIF_MAL_CLK_SEL_VAL_PD     0x0
#define SOC_PB_NIF_MAL_CLK_SEL_VAL_TX_EN  0x2
#define SOC_PB_NIF_MAL_CLK_SEL_VAL_XAUI   0x8
#define SOC_PB_NIF_MAL_CLK_SEL_VAL_SGMII  0x9
#define SOC_PB_NIF_MAL_CLK_SEL_VAL_RXAUI  0xA
#define SOC_PB_NIF_MAL_CLK_SEL_VAL_QSGMII 0xB
#define SOC_PB_NIF_MAL_CLK_SEL_VAL_ILKN   0x4
#define SOC_PB_NIF_MAL_CLK_SEL_VAL_RXAUI_ALT  0x2
#define SOC_PB_NIF_MAL_CLK_SEL_VAL_QSGMII_ALT 0x3

/*
 *    The first lane of MALG-A combo SerDes.
 *  The combo quartet is not used by the ILKN
 */
#define SOC_PB_NIF_ILKN_SRD_COMBO_A_LN_ID     12

/*
 * PTP
 */
/* 3.2ns * 35 (3.125)*/
#define SOC_PB_NIF_PTP_BIAS_TSLC_FACTOR_XAUI       35
/* 1.250 */
#define SOC_PB_NIF_PTP_BIAS_TSLC_FACTOR_SGMII      41
/* 6.250 */
#define SOC_PB_NIF_PTP_BIAS_TSLC_FACTOR_RXAUI      28
/* 5.000 */
#define SOC_PB_NIF_PTP_BIAS_TSLC_FACTOR_QSGMII     70
#define SOC_PB_NIF_PTP_BIAS_TSLC_FACTOR_DFLT       SOC_PB_NIF_PTP_BIAS_TSLC_FACTOR_XAUI

#define SOC_PB_NIF_PTP_BIAS_RESOLUTION_FACTOR      256


#define SOC_PB_NIF_PTP_INTERN_DELAY_FACTOR_XAUI    10
#define SOC_PB_NIF_PTP_INTERN_DELAY_FACTOR_SGMII   10
#define SOC_PB_NIF_PTP_INTERN_DELAY_FACTOR_RXAUI   20
#define SOC_PB_NIF_PTP_INTERN_DELAY_FACTOR_QSGMII  40

/*
 * Fat-Pipe
 */
/* The size (entries) of overall memory shared by the IRR resequencing FIFO-s: 4K */
#define SOC_PB_NIF_FATP_MEMORY_SIZE                (4 * 1024 - 1)
#define SOC_PB_NIF_FATP_FULL_MARK_TH_MIN            80
#define SOC_PB_NIF_FATP_FULL_MARK_TH_MARGINE        20

/* QSGMII Alternative Serdes mapping */
#define SOC_PB_NIF_QSGMII_ALT_CLK_VAL_4BIT 0xF
#define SOC_PB_NIF_QSGMII_ALT_CLK_VAL_2BIT 0x3
/* } */
/*************
 * MACROS    *
 *************/
/* { */
/*
 *    Convert from parameter representation to internal register representation
 */
#define SOC_PETRA_NIF_TO_REG_DIRECTION(drctn) \
  ((drctn == SOC_PETRA_CONNECTION_DIRECTION_RX)?(SOC_PB_REG_NIF_DIRECTION_RX):(SOC_PB_REG_NIF_DIRECTION_TX))

/* First allowed lane. Id is 0-1 */
#define SOC_PB_NIF_ILKN_LANE_FIRST(id) \
  ((id == 0)?SOC_PB_NIF_ILKN0_LANE_MIN:SOC_PB_NIF_ILKN1_LANE_MIN)

#define SOC_PB_NIF_IS_ILKN0_OVER_MALGB(nof_lanes) \
  (nof_lanes > SOC_PB_NIF_ILKN0_MAX_LANES_NO_OVRLP)

#define SOC_PB_NIF_IS_NON_DATA_CNTR(cntr) \
 (\
   ((cntr) == SOC_PB_NIF_RX_OK_PAUSE_FRAMES) || \
   ((cntr) == SOC_PB_NIF_RX_ERR_PAUSE_FRAMES) || \
   ((cntr) == SOC_PB_NIF_RX_PTP_FRAMES) || \
   ((cntr) == SOC_PB_NIF_TX_PAUSE_FRAMES) || \
   ((cntr) == SOC_PB_NIF_TX_PTP_FRAMES) \
 )

/*
 *    Fat Pipe configration: indicates a disabled port
 *  in the IRR (ingress resequencer configuration)
 */
#define SOC_PB_NIF_FATP_IRR_DISABLE_VAL 0xf

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
*     soc_pb_nif_regs_init
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
  soc_pb_nif_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    mal_idx;
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_REGS_INIT);

  regs = soc_petra_regs();

  SOC_PB_FLD_SET(regs->irr.resequencer_configuration_reg.resequencer_drop_out_of_seq, 0x1, 10, exit);

  /*
   *    NIF MALG OOR
   */
  SOC_PB_FLD_ISET(regs->nif.nif_mal_reset_reg.init_mal_hard_reset, 0x0,  SOC_PB_NIF_MALG_ID_A, 20, exit);
  SOC_PB_FLD_ISET(regs->nif.nif_mal_reset_reg.init_mal_hard_reset, 0x0,  SOC_PB_NIF_MALG_ID_B, 22, exit);
  SOC_PB_FLD_ISET(regs->nif.nif_mal_reset_reg.init_mal_soft_reset, 0x0,  SOC_PB_NIF_MALG_ID_A, 24, exit);
  SOC_PB_FLD_ISET(regs->nif.nif_mal_reset_reg.init_mal_soft_reset, 0x0,  SOC_PB_NIF_MALG_ID_B, 26, exit);

  SOC_PB_FLD_SET(regs->nbi.statistics_reg.stat_en, 0x1, 30, exit);
  SOC_PB_FLD_SET(regs->nbi.statistics_reg.stat_reset_start, 0x1, 35, exit);
  SOC_PB_FLD_SET(regs->nbi.statistics_reg.stat_reset_start, 0x0, 36, exit);

  for (mal_idx = 0; mal_idx < SOC_PB_NOF_MAC_LANES; mal_idx++)
  {
    SOC_PB_FLD_ISET(regs->nif_mac_lane.statistics_reg.stat_reset, 0x0, mal_idx, 39, exit);
    /* Disable FC Xon Refresh */
    SOC_PB_FLD_ISET(regs->nif_mac_lane.flow_control_tx_refresh_timer_reg.fc_tx_refresh_timer_xon, 0x0, mal_idx, 40, exit);
  }

  /* ILKN */
  SOC_PB_FLD_SET(regs->nbi.ilkn_rx0_controller_config1_reg.ilkn_rx0_enable_req3, 0x0, 41, exit);
  SOC_PB_FLD_SET(regs->nbi.ilkn_rx1_controller_config1_reg.ilkn_rx1_enable_req3, 0x0, 42, exit);

  /* Count packets instead of bursts in the StatRxBurstsOkCnt and StatTxBurstsOkCnt. */
  SOC_PB_FLD_SET(regs->nbi.statistics_reg.stat_packet_count_en, 0x1, 50, exit);
  /* Make the counters represent all RxOkBursts, and not just the UC */
  SOC_PB_FLD_SET(regs->nbi.statistics_reg.stat_ucast_count_enable, 0x0, 52, exit);

  /* Set "no fragmentation" as default */
  SOC_PB_REG_SET(regs->epni.egress_interface_no_fragmentation_mode_configuration_reg, 0x0000ffff, 55, exit);
  SOC_PB_REG_SET(regs->egq.egress_interface_no_fragmentation_mode_configuration_reg, 0x0000ffff, 60, exit);

  /*
   *    Fat-Pipe - enable all resequencer fifos
   */
  SOC_PB_FLD_SET(regs->irr.resequencer_fifo_valid_reg.resequencer_fifo_valid, 0xfff, 70, exit);

  SOC_PB_FLD_SET(regs->eci.malg_configurations_reg.malga_ptp_clock_pd, 0x0, 72, exit);
  SOC_PB_FLD_SET(regs->eci.malg_configurations_reg.malgb_ptp_clock_pd, 0x0, 73, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_nif_regs_init()",0,0);
}

/*********************************************************************
* NAME:
*     soc_pb_mdio_init
* FUNCTION:
*     Initialization of the mdio configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN uint32                   mdio_clk_freq_khz
*     Identifier of the clock frequency
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence by the function soc_pb_nif_init.
*********************************************************************/
STATIC uint32
  soc_pb_nif_mdio_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  mdio_clk_freq_khz
  )
{
  uint32
    ktics_per_sec,
    fld_val,
    res  = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MDIO_INIT);
  
  regs = soc_petra_regs();
  ktics_per_sec = soc_petra_chip_kilo_ticks_per_sec_get(unit);

  if (mdio_clk_freq_khz == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_DIV_BY_ZERO_ERR, 5, exit);
  }

  fld_val = SOC_SAND_DIV_ROUND_UP(ktics_per_sec, (2*mdio_clk_freq_khz));

  SOC_PB_FLD_SET(regs->eci.mdio_cfg_reg.cfg_tick_cnt, fld_val, 10, exit);
  SOC_PB_FLD_SET(regs->eci.mdio_cfg_reg.cfg_pd, 0x0, 12, exit);
  SOC_PB_FLD_SET(regs->eci.mdio_cfg_reg.cfg_init, 0x0, 14, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_mdio_init()", mdio_clk_freq_khz, 0);
}

/*********************************************************************
* NAME:
*     soc_pb_nif_synce_init
* FUNCTION:
*     Initialization of the SyncE configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN uint32                   is_malgb_enabled
*   If TRUE, the Synchronous Ethernet pins of MALG-B can be
*   used (4 SYNCE signals in total). Otherwise - only 2
*   SYNCE signals can be used. Note: if TRUE, not fully
*   compatible with Soc_petra-A pin-out (override VSS pins).
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence by the function soc_pb_nif_init.
*********************************************************************/
STATIC uint32
  soc_pb_nif_synce_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_malgb_enabled
  )
{
  uint32
    fld_val,
    reg_val,
    res  = SOC_SAND_OK;
  uint32
    malg_idx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SYNCE_INIT);
  
  regs = soc_petra_regs();

  SOC_PB_REG_GET(regs->eci.select_output_of_synchronous_ethernet_pads_reg, reg_val, 50, exit);
  fld_val = 0x0;
  SOC_PB_FLD_TO_REG(regs->eci.select_output_of_synchronous_ethernet_pads_reg.sync_eth_pad0_oe_n, fld_val, reg_val, 52, exit);
  SOC_PB_FLD_TO_REG(regs->eci.select_output_of_synchronous_ethernet_pads_reg.sync_eth_pad1_oe_n, fld_val, reg_val, 54, exit);
  fld_val = SOC_SAND_NUM2BOOL_INVERSE(is_malgb_enabled);
  SOC_PB_FLD_TO_REG(regs->eci.select_output_of_synchronous_ethernet_pads_reg.sync_eth_pad2_oe_n, fld_val, reg_val, 56, exit);
  SOC_PB_FLD_TO_REG(regs->eci.select_output_of_synchronous_ethernet_pads_reg.sync_eth_pad3_oe_n, fld_val, reg_val, 58, exit);
  SOC_PB_REG_SET(regs->eci.select_output_of_synchronous_ethernet_pads_reg, reg_val, 60, exit);

  for (malg_idx = 0; malg_idx < SOC_PB_NIF_NOF_MALG_IDS; malg_idx++)
  {
    SOC_PB_FLD_ISET(regs->nif.nif_sync_eth1_reg.sync_eth_valid_select1, 0x1, malg_idx, 70 + malg_idx, exit);
    SOC_PB_FLD_ISET(regs->nif.nif_sync_eth1_reg.sync_eth_valid_select2, 0x1, malg_idx, 80 + malg_idx, exit);
  }

  SOC_PB_FLD_SET(regs->irr.resequencer_configuration_reg.resequencer_fast_enable, 0x1, 100, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_synce_init()", is_malgb_enabled, 0);
}

/*********************************************************************
* NAME:
*     soc_pb_nif_init
* FUNCTION:
*     Initialization of the Soc_petra blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN uint32                   mdio_clk_freq_khz -
*     Identifier of the clock frequency.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_pb_nif_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  mdio_clk_freq_khz,
    SOC_SAND_IN  uint8                 is_malgb_enabled
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_INIT);
 
  res = soc_pb_nif_regs_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (mdio_clk_freq_khz != SOC_SAND_INTERN_VAL_INVALID_32)
  {
      res = soc_pb_nif_mdio_init(
            unit,
            mdio_clk_freq_khz
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  res = soc_pb_nif_synce_init(
          unit,
          is_malgb_enabled
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_nif_init()", 0, 0);
}

uint32
  soc_pb_nif_mal_type_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  uint32           mal_ndx,
    SOC_SAND_OUT SOC_PB_NIF_TYPE  *nif_type
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;
  uint32
    malg_id,
    mal_inner_id;
  uint8
    alt_srd_mapping = FALSE;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MAL_TYPE_GET);

  SOC_SAND_CHECK_NULL_INPUT(nif_type);

  regs = soc_petra_regs();

  if (mal_ndx >= SOC_PB_NOF_MAC_LANES)
  {
    *nif_type = SOC_PB_NIF_TYPE_NONE;
    SOC_PB_DO_NOTHING_AND_EXIT;
  }

  /*
   *    Indexing
   */
  malg_id = SOC_PB_NIF_MAL2MALG_ID(mal_ndx);
  mal_inner_id = SOC_PB_NIF_MAL2INNER_ID(mal_ndx);

  /* Note: assumes the type is configured in both directions */
  SOC_PB_FLD_IGET(regs->nif.nif_clocks_reg[SOC_PB_REG_NIF_DIRECTION_RX].mal_clock_select[mal_inner_id], fld_val, malg_id, 10, exit);
  
  res = soc_pb_nif_mal_qsgmii_alt_get_unsafe(unit, mal_ndx, &alt_srd_mapping);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  switch(fld_val) {
  case SOC_PB_NIF_MAL_CLK_SEL_VAL_XAUI:
    *nif_type = SOC_PB_NIF_TYPE_XAUI;
      break;
  case SOC_PB_NIF_MAL_CLK_SEL_VAL_SGMII:
    *nif_type = SOC_PB_NIF_TYPE_SGMII;
      break;
  case SOC_PB_NIF_MAL_CLK_SEL_VAL_RXAUI:
  case SOC_PB_NIF_MAL_CLK_SEL_VAL_RXAUI_ALT:
    *nif_type = SOC_PB_NIF_TYPE_RXAUI;
      break;
  case SOC_PB_NIF_MAL_CLK_SEL_VAL_QSGMII:
  case SOC_PB_NIF_MAL_CLK_SEL_VAL_QSGMII_ALT:
    *nif_type = SOC_PB_NIF_TYPE_QSGMII;
      break;
  case SOC_PB_NIF_MAL_CLK_SEL_VAL_ILKN:
    *nif_type = SOC_PB_NIF_TYPE_ILKN;
      break;
  default:
    *nif_type = SOC_PB_NIF_TYPE_NONE;
  }

  /* Because alternative QSGMII mapping changes the */
  /* CLK settings, we need to check it separately   */
  /* Otherwise SOC_PB_NIF_TYPE_NONE will be returned    */
  if(alt_srd_mapping && fld_val == SOC_PB_NIF_QSGMII_ALT_CLK_VAL_4BIT)
  {
    *nif_type = SOC_PB_NIF_TYPE_QSGMII;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_nif_mal_type_get()", 0, 0);
}

uint32
  soc_pb_nif_is_channelized(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID nif_ndx,
    SOC_SAND_OUT uint8 *is_channelized
  )
{
  uint32
    fld_val_rx,
    fld_val_tx,
    res;
  SOC_PB_NIF_TYPE
    nif_type;
  uint32
    mal_id;
  SOC_PETRA_REGS
    *regs;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(is_channelized);
  regs = soc_petra_regs();
  
  nif_type = soc_pb_nif_id2type(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(soc_pb_nif2intern_id(nif_ndx));

  switch(nif_type) {
  case SOC_PB_NIF_TYPE_XAUI:
  case SOC_PB_NIF_TYPE_RXAUI:
    SOC_PB_FLD_IGET(regs->nif_mac_lane.spaui_extensions1_reg.rx_bct_size, fld_val_rx, mal_id, 10, exit);
    if (fld_val_rx == 0)
    {
      SOC_PB_FLD_IGET(regs->nif_mac_lane.spaui_extensions2_reg.tx_bct_size, fld_val_tx, mal_id, 12, exit);
      *is_channelized = (fld_val_tx == 0)?FALSE:TRUE;
    }
    else
    {
      *is_channelized = TRUE;
    }
    break;
  case SOC_PB_NIF_TYPE_SGMII:
  case SOC_PB_NIF_TYPE_QSGMII:
    *is_channelized = FALSE;
  break;
  case SOC_PB_NIF_TYPE_ILKN:
    *is_channelized = TRUE;
  break;
  default:
    *is_channelized = FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_nif_is_channelized()", 0, 0);
}

uint32
  soc_pb_nif_mal_id_verify(
    SOC_SAND_IN  uint32                      mal_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MAL_ID_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mal_ndx, SOC_PB_NOF_MAC_LANES-1, SOC_PB_MAL_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_mal_id_verify()", mal_ndx, 0);
}

uint32
  soc_pb_nif_id_verify(
    SOC_SAND_IN SOC_PETRA_INTERFACE_ID nif_ndx
  )
{
  uint8
    is_valid_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ID_VERIFY);

  is_valid_id = SOC_PB_NIF_IS_VALID_ID(nif_ndx);
  if (!(is_valid_id))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_INVALID_ID_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_id_verify()", nif_ndx, 0);
}

uint32
  soc_pb_nif_type_verify(
    SOC_SAND_IN SOC_PB_NIF_TYPE nif_type
  )
{
  uint8
    is_valid;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_TYPE_VERIFY);

  switch(nif_type) {
  case SOC_PB_NIF_TYPE_XAUI:
  case SOC_PB_NIF_TYPE_RXAUI:
  case SOC_PB_NIF_TYPE_SGMII:
  case SOC_PB_NIF_TYPE_QSGMII:
  case SOC_PB_NIF_TYPE_ILKN:
    is_valid = TRUE;
      break;
  default:
    is_valid = FALSE;
  }
  
  if (!(is_valid))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_UNKNOWN_NIF_TYPE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_type_verify()", nif_type, 0);
}

void
  soc_pb_nif_srd_lanes_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID nif_ndx,
    SOC_SAND_OUT uint32          *nof_lanes,
    SOC_SAND_OUT uint32          *base_lane
  )
{
  uint32
    nif_id,
    mal_id,
    malg_id,
    nof_srd_lanes,
    base_lane_id = 0;
  SOC_PB_NIF_TYPE
    nif_type;
  SOC_PB_NIF_ILKN_INFO
    ilkn_rx,ilkn_tx;
  uint32
    res = SOC_SAND_OK;
  uint8
    alt_srd_mapping = FALSE;

  if (
      (nof_lanes == NULL) ||
      (base_lane == NULL)
     )
  {
    return;
  }

  /*
   *    Indexing
   */
  nif_id = soc_pb_nif2intern_id(nif_ndx);
  if (nif_id == SOC_PETRA_NIF_ID_NONE)
  {
    nof_srd_lanes = 0;
    goto exit;
  }

  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  nif_type = soc_pb_nif_id2type(nif_ndx);
  base_lane_id = mal_id * SOC_PB_SRD_LANES_PER_MAL;
  malg_id = SOC_PB_NIF_MAL2MALG_ID(mal_id);

  switch(nif_type) {
    case SOC_PB_NIF_TYPE_XAUI:
      nof_srd_lanes = SOC_PB_SRD_LANES_PER_DMAL;
  	  break;
    case SOC_PB_NIF_TYPE_RXAUI:
      nof_srd_lanes = SOC_PB_SRD_LANES_PER_MAL;
  	  break;
    case SOC_PB_NIF_TYPE_SGMII:
      nof_srd_lanes = 1;
      base_lane_id += SOC_PB_NIF_SGMII_LANE(nif_ndx);
  	  break;
    case SOC_PB_NIF_TYPE_QSGMII:
      /*
       *	Four QSGMII interfaces on a single SerDes lane
       */
      /* Check if alternative serdes mapping is enabled */   
      res = soc_pb_nif_mal_qsgmii_alt_get_unsafe(unit, mal_id, &alt_srd_mapping);
      if (SOC_PB_IS_ERR_RES(res))
      {
        nof_srd_lanes = 0;
        goto exit;
      }

      if(alt_srd_mapping)
      {
        /* Override the base lane id */
        /* MAL 5  --> LANE 1 */
        /* MAL 6  --> LANE 7 */
        /* MAL 7  --> LANE 3 */
        /* MAL 13 --> LANE 17 */
        /* MAL 14 --> LANE 23 */
        /* MAL 15 --> LANE 19 */
        switch (mal_id) {
          case 5: 
          case 13:
            base_lane_id = 1 + (malg_id * SOC_PB_NIF_MALS_IN_MALG * SOC_PB_SRD_LANES_PER_MAL); 
            break;
          case 6: 
          case 14:
            base_lane_id = 7 + (malg_id * SOC_PB_NIF_MALS_IN_MALG * SOC_PB_SRD_LANES_PER_MAL); 
            break;
          case 7: 
          case 15:
            base_lane_id = 3 + (malg_id * SOC_PB_NIF_MALS_IN_MALG * SOC_PB_SRD_LANES_PER_MAL); 
            break;
          default:
            /* No change */
            break;
        }
      }

      nof_srd_lanes = 1;
  	  break;
    case SOC_PB_NIF_TYPE_ILKN:
      /*
       *	Note: depends on the actual configuration,
       *  so this value is read from the device
       *  and is valid only after the ILKN interface is already
       *  configured.
       */
      SOC_PB_NIF_ILKN_INFO_clear(&ilkn_rx);
      SOC_PB_NIF_ILKN_INFO_clear(&ilkn_tx);

      res = soc_pb_nif_ilkn_get_unsafe(
              unit,
              nif_ndx,
              &ilkn_rx,
              &ilkn_tx
            );
      if (SOC_PB_IS_ERR_RES(res))
      {
        nof_srd_lanes = 0;
        goto exit;
      }

      nof_srd_lanes = ilkn_rx.nof_lanes;

      if (ilkn_rx.is_invalid_lane && 
          !(((nif_ndx == SOC_PETRA_NIF_ID_ILKN_0) && (nof_srd_lanes == SOC_PB_NIF_ILKN_A_NOF_LANES_MIN)) ||
           ((nif_ndx == SOC_PETRA_NIF_ID_ILKN_1) && (nof_srd_lanes == SOC_PB_NIF_ILKN_B_NOF_LANES_MIN))))
      {
        /* One of the lanes is invalid, so the overall range is plus 1 */
        nof_srd_lanes++;
      }
  	  break;
    default:
      nof_srd_lanes = 0;
  }

exit:
  *nof_lanes = nof_srd_lanes;
  *base_lane = base_lane_id;
}

/*
 * Sets to default (XAUI) configuration
 * Currently not used
 */

/*
 *    Reset MAL.
 *  Sets all MAL registers to the device defaults,
 *  typically this corresponds to a XAUI configuration.
 *
 *  is_hard_not_soft_rst - if TRUE, performs Hard Reset, Otherwise - Soft Reset
 *  is_in_not_out_rst - if TRUE, sets the MAL in-reset, otherwise - Out-Of-Reset
 */
uint32
  soc_pb_nif_mal_reset(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  mal_ndx,
    SOC_SAND_IN  uint8  is_hard_not_soft_rst,
    SOC_SAND_IN  uint8  is_in_not_out_rst
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  uint32
    mal_id,
    malg_id;
  SOC_PETRA_REG_FIELD
    *fld_ref;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MAL_RESET);

  regs = soc_petra_regs();

  mal_id = SOC_PB_NIF_MAL2INNER_ID(mal_ndx);
  malg_id = SOC_PB_NIF_MAL2MALG_ID(mal_ndx);

  if (is_hard_not_soft_rst)
  {
    fld_ref = SOC_PB_REG_DB_ACC_REF(regs->nif.nif_mal_reset_reg.init_mal_hard_reset);
  }
  else
  {
    fld_ref = SOC_PB_REG_DB_ACC_REF(regs->nif.nif_mal_reset_reg.init_mal_soft_reset);
  }

  SOC_PB_IMPLICIT_FLD_IGET(*fld_ref, fld_val, malg_id, 10, exit);
  SOC_SAND_SET_BIT(fld_val, is_in_not_out_rst, mal_id);
  SOC_PB_IMPLICIT_FLD_ISET(*fld_ref, fld_val, malg_id, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_mal_reset()", mal_ndx, is_hard_not_soft_rst);
}


/*
 *    Typically, the defaults are suitable for XAUI mode
 */
uint32
  soc_pb_nif_mal_dflts_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx
  )
{
  uint32
    reg_val,
    fld_val,
    bit_i,
    res = SOC_SAND_OK;
  uint32
    mal_id,
    nif_id,
    malg_id = SOC_PB_NIF_MAL2MALG_ID(mal_ndx),
    mal_inner_id = SOC_PB_NIF_MAL2INNER_ID(mal_ndx),
    reg_idx,
    fld_idx;
  SOC_PB_NIF_PTP_INFO
    ptp_info;
  SOC_PB_NIF_ILKN_ID
    ilkn_idx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MAL_DFLTS_SET);

  SOC_PB_NIF_PTP_INFO_clear(&ptp_info);

  regs = soc_petra_regs();
  /*
   *    EGQ
   */
  SOC_PB_FLD_GET(regs->egq.spaui_sel_reg.spaui_sel, fld_val,10 , exit);
  SOC_SAND_SET_BIT(fld_val, 0x1, mal_ndx);
  SOC_PB_FLD_SET(regs->egq.spaui_sel_reg.spaui_sel, fld_val,12 , exit);

  SOC_PB_FLD_GET(regs->egq.nif_mal_enable_reg.nif_cancel_en, fld_val,20 , exit);
  SOC_SAND_SET_BIT(fld_val, 0x1, mal_ndx);
  SOC_PB_FLD_SET(regs->egq.nif_mal_enable_reg.nif_cancel_en, fld_val,22 , exit);

  reg_idx = SOC_PETRA_REG_IDX_GET(mal_inner_id, SOC_PB_REG_NIF_NOF_PAEB_FLDS);
  fld_idx = SOC_PETRA_FLD_IDX_GET(mal_inner_id, SOC_PB_REG_NIF_NOF_PAEB_FLDS);
  SOC_PB_FLD_ISET(regs->nif.nif_paeb_reset_reg[reg_idx].rx_ln0_paeb_rd_ptr_rst[fld_idx], 0x8, malg_id, 30, exit);

  SOC_PB_REG_IGET(regs->nif_mac_lane.bom_reg, reg_val, mal_ndx, 32, exit);
  fld_val = 0x0;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.bom_reg.rx_swap_ll, fld_val, reg_val, 33, exit);
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.bom_reg.rx_swap_ll_pair, fld_val, reg_val, 34, exit);
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.bom_reg.tx_swap_ll, fld_val, reg_val, 35, exit);
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.bom_reg.tx_swap_ll_pair, fld_val, reg_val, 36, exit);
  SOC_PB_REG_ISET(regs->nif_mac_lane.bom_reg, reg_val, mal_ndx, 37, exit);

  /* 
   * Check if the MAL corresponds to an Interlaken interface,
   * If so, unset the "use-all-channels" option. This limits the ports-per-interface to 64.
   */
  for (ilkn_idx = SOC_PB_NIF_ILKN_ID_A; ilkn_idx <= SOC_PB_NIF_ILKN_ID_B; ilkn_idx++)
  {
    nif_id = soc_pb_nif2intern_id(ilkn_idx);
    mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
    if (mal_ndx == mal_id)
    {      
      bit_i = nif_id/16; /* Bit 0 correspond to ILKN-A, Bit 2 corresponds to ILKN-B */
      SOC_PB_FLD_GET(regs->ire.fap_port_configuration_reg.use_all_channels, fld_val, 40, exit);
      SOC_SAND_SET_BIT(fld_val, 0x0, bit_i);
      SOC_PB_FLD_SET(regs->ire.fap_port_configuration_reg.use_all_channels, fld_val, 42, exit);
    }
  }
  
  /*
   * We just disable the PTP here, so, the NIF type is not important -
   */
  res = soc_pb_nif_ptp_conf_mal_config(
          unit,
          mal_ndx,
          SOC_PETRA_CONNECTION_DIRECTION_BOTH,
          SOC_PB_NIF_TYPE_DONT_CARE,
          &ptp_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_mal_dflts_set()", mal_ndx, 0);
}

/*
 *    Sets MAL configuration to default (XAUI) values.
 *  This function is called before setting any NIF type,
 *  to clear any previous configuration of the NIF.
 *  Note 1: an actual XAUI interface can only be set on MALs 0, 2, 4..., 14
 *  Note 2: this function does not enable the MAL, just sets the configuration
 */
uint32
  soc_pb_nif_mal_xaui_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PB_NIF_MAL_TOPOLOGY            *topology
  )
{
  uint32
    reg_val,
    fld_val,
    res = SOC_SAND_OK;
  uint32
    directn_idx,
    nif_id,
    malg_id = SOC_PB_NIF_MAL2MALG_ID(mal_ndx),
    mal_inner_id,
    nif_id_base = SOC_PB_MAL2NIF_INNER_ID(mal_ndx),
    nif_id_end;
  SOC_PETRA_SRD_STAR_ID
    star_id;
  uint32
    srd_base_lane_global,
    srd_qrtt_id_glbl,
    srd_nof_lanes,
    srd_base_lane_inner,
    srd_end_lane_inner,
    srd_qrtt_id,
    srd_lane_id,
    srd_instance_id;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_NIF_STATE_INFO
    on_off;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MAL_XAUI_SET);

  regs = soc_petra_regs();

  mal_inner_id = SOC_PB_NIF_MAL2INNER_ID(mal_ndx);

  /*
   *    Disable fragmentation
   */
  SOC_PB_FLD_SET(regs->egq.egress_interface_no_fragmentation_mode_configuration_reg.nif_no_frag[mal_ndx], 0x1, 170, exit);
  SOC_PB_FLD_SET(regs->epni.egress_interface_no_fragmentation_mode_configuration_reg.nif_no_frag[mal_ndx], 0x1, 172, exit);

  /* Disable the second MAL. Must be handled differently for Alt */
  if (SOC_PB_NIF_IS_BASE_MAL(mal_ndx))
  {
    SOC_PB_NIF_STATE_INFO_clear(&on_off);
    on_off.is_nif_on = FALSE;
    on_off.serdes_also = FALSE;
    res = soc_pb_nif_on_off_set_unsafe(
            unit,
            SOC_PB_NIF_ID(XAUI, mal_ndx/2),
            &on_off
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  /************************************************************************/
  /* MALG                                                                 */
  /************************************************************************/
  /*
   *    Set Clocks (MAL currently in Hard reset)
   */
  SOC_PB_FLD_ISET(regs->nif.nif_clocks_reg[SOC_PB_REG_NIF_DIRECTION_RX].mal_clock_select[mal_inner_id], SOC_PB_NIF_MAL_CLK_SEL_VAL_XAUI, malg_id, 12, exit);
  fld_val = SOC_PB_NIF_IS_BASE_MAL(mal_ndx)?SOC_PB_NIF_MAL_CLK_SEL_VAL_XAUI:SOC_PB_NIF_MAL_CLK_SEL_VAL_TX_EN;
  SOC_PB_FLD_ISET(regs->nif.nif_clocks_reg[SOC_PB_REG_NIF_DIRECTION_TX].mal_clock_select[mal_inner_id], fld_val, malg_id, 14, exit);

  if (!SOC_PB_NIF_IS_BASE_MAL(mal_ndx))
  {
    /*
     *    For XAUI, second MAL must be held in reset - only the clocks
     *  must be configured
     */
    SOC_PB_DO_NOTHING_AND_EXIT;
    /************************************************************************/
    /* Note: the rest of the logic is only performed for base-MAL!          */
    /************************************************************************/
  }
 
  /************************************************************************/
  /* NBI                                                                  */
  /************************************************************************/

  /*
   *    MLF FIFO In-Reset
   *  Note: should not be performed under traffic
   */
  nif_id_end = SOC_SAND_RNG_LAST(nif_id_base, SOC_PB_MAX_NIFS_PER_MAL);
  SOC_PB_NIF_BOTH_DIRECTIONS(directn_idx)
  {
    for (nif_id = nif_id_base; nif_id <= nif_id_end; nif_id++)
    {
      fld_val = 0x0;
      SOC_PB_FLD_SET(regs->nbi.mlf_reset_ports_reg[malg_id][directn_idx].ports_srstn[nif_id], 0x0, 20 + directn_idx, exit);
    }
  }

  /*
   *    MLF FIFO Configuration
   */
  /* XAUI - 1 FIFO */
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_config_reg[mal_ndx].rx_num_logical_fifos_mode, 0x2, 30, exit);
  SOC_PB_FLD_SET(regs->nbi.tx_mlf_config_reg[mal_ndx].tx_num_logical_fifos_mode, 0x2, 32, exit);
  /* Disable discard short packets */
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_config_reg[mal_ndx].rx_discard_too_short_err_pkts, 0x0, 34, exit);
  SOC_PB_FLD_SET(regs->nbi.tx_mlf_config_reg[mal_ndx].tx_start_tx_threshold, 0x8, 36, exit);
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_thresholds_config_reg[mal_ndx].rx_llfc_threshold_set, 64, 38, exit);
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_thresholds_config_reg[mal_ndx].rx_llfc_threshold_clr, 60, 39, exit);
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_thresholds_config_reg[mal_ndx].rx_cbfc_threshold_set, 44, 40, exit);
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_thresholds_config_reg[mal_ndx].rx_cbfc_threshold_clr, 40, 41, exit);
  
  /*
   *    MLF FIFO Out-Of-Reset
   */
  SOC_PB_NIF_BOTH_DIRECTIONS(directn_idx)
  {
    /* Just base nif-port for XAUI */
    SOC_PB_FLD_SET(regs->nbi.mlf_reset_ports_reg[malg_id][directn_idx].ports_srstn[nif_id_base], 0x1, 50 + directn_idx, exit);
  }

  /*
   *    Flow Control - set XAUI mode
   */
  SOC_PB_FLD_SET(regs->nbi.fc_reset_reg.fc_mal_xaui_mode[mal_ndx], 0x1, 55, exit);

  /************************************************************************/
  /* SerDes                                                               */
  /************************************************************************/

  /*
   *    Set only for even XAUI MAL, which controls all the 4 SerDes
   */
  soc_pb_nif_srd_lanes_get(
      unit,
      SOC_PB_NIF_ID(XAUI, mal_ndx/2),
      &srd_nof_lanes,
      &srd_base_lane_global
    );

  if (srd_nof_lanes == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_SRD_LANES_COMP_ERR, 57, exit);
  }

  srd_base_lane_inner = SOC_PETRA_SRD_LANE2INNER_ID(srd_base_lane_global);
  srd_end_lane_inner = SOC_SAND_RNG_LAST(srd_base_lane_inner, srd_nof_lanes);
  star_id = SOC_PETRA_SRD_LANE2STAR(srd_base_lane_global);
  srd_instance_id = SOC_PETRA_SRD_STAR2INSTANCE(star_id);
  srd_qrtt_id = SOC_PETRA_SRD_LANE2QRTT_INNER(srd_base_lane_global);
  srd_qrtt_id_glbl = SOC_PETRA_SRD_LANE2QRTT_GLBL(srd_base_lane_global);

  SOC_PB_FLD_ISET(regs->serdes.srd_cmu_cfga_reg[srd_qrtt_id].ln_20b_mode, 0x0, srd_instance_id, 60, exit);
  SOC_PB_FLD_ISET(regs->serdes.srd_cmu_cfga_reg[srd_qrtt_id].ln_tx_swap_i1_en, 0x0, srd_instance_id, 64, exit);

  res = soc_petra_srd_qrtt_sync_fifo_en(
          unit,
          srd_qrtt_id_glbl,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  for (srd_lane_id = srd_base_lane_inner; srd_lane_id <= srd_end_lane_inner; srd_lane_id++)
  {
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_mask_disparity_errors, 0x0, srd_instance_id, 80, exit);
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_decouple_disparity, 0x0, srd_instance_id, 81, exit);
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].rx_nearest_comma_align, 0x1, srd_instance_id, 82, exit);
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_comma_det_mask_lsb, 0x1, srd_instance_id, 84, exit);
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_comma_det_mask_msb, 0x1, srd_instance_id, 86, exit);
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].tx_use_ext_byte_toggle, 0x1, srd_instance_id, 88, exit);
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_align_en_req, 0x1, srd_instance_id, 89, exit);
  }

  /************************************************************************/
  /* MAL                                                                  */
  /************************************************************************/
  reg_val = 0;
  fld_val = 0x1;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.enable_reg.xaui_sr_mode, fld_val, reg_val, 112, exit);
  fld_val = 0x1;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.enable_reg.xaui_rx_lane_en, fld_val, reg_val, 113, exit);
  fld_val = 0x1;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.enable_reg.xaui_tx_lane_en, fld_val, reg_val, 114, exit);
  SOC_PB_REG_ISET(regs->nif_mac_lane.enable_reg, reg_val, mal_ndx, 120, exit);

  SOC_PB_REG_IGET(regs->nif_mac_lane.async_fifo_config_reg, reg_val, mal_ndx, 130, exit);
  fld_val = 0x1;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.async_fifo_config_reg.tx_mlf_credit_ovrd, fld_val, reg_val, 130, exit);
  fld_val = 0xa;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.async_fifo_config_reg.taf_af_wm, fld_val, reg_val, 132, exit);
  SOC_PB_REG_ISET(regs->nif_mac_lane.async_fifo_config_reg, reg_val, mal_ndx, 134, exit);

  /* 
   * Lanes Swap 
   */
  SOC_PB_REG_IGET(regs->nif_mac_lane.bom_reg, reg_val, mal_ndx, 140, exit);
  fld_val = SOC_SAND_BOOL2NUM(topology->lanes_swap.swap_rx);
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.bom_reg.rx_swap_ll, fld_val, reg_val, 142, exit);
  fld_val = SOC_SAND_BOOL2NUM(topology->lanes_swap.swap_tx);
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.bom_reg.tx_swap_ll, fld_val, reg_val, 146, exit);
  SOC_PB_REG_ISET(regs->nif_mac_lane.bom_reg, reg_val, mal_ndx, 150, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_mal_xaui_set()", mal_ndx, 0);
}

uint32
  soc_pb_nif_mal_rxaui_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PB_NIF_MAL_TOPOLOGY            *topology
  )
{
  uint32
    reg_val,
    fld_val,
    res = SOC_SAND_OK;
  uint32
    directn_idx,
    nif_id,
    malg_id = SOC_PB_NIF_MAL2MALG_ID(mal_ndx),
    mal_inner_id,
    nif_id_base = SOC_PB_MAL2NIF_INNER_ID(mal_ndx),
    nif_id_end;
  SOC_PETRA_SRD_STAR_ID
    star_id;
  uint32
    srd_base_lane_global,
    srd_qrtt_id_glbl,
    srd_nof_lanes,
    srd_base_lane_inner,
    srd_end_lane_inner,
    srd_qrtt_id,
    srd_lane_id,
    srd_instance_id;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MAL_RXAUI_SET);

  regs = soc_petra_regs();

  mal_inner_id = SOC_PB_NIF_MAL2INNER_ID(mal_ndx);

  /*
   *    Disable fragmentation
   */
  SOC_PB_FLD_SET(regs->egq.egress_interface_no_fragmentation_mode_configuration_reg.nif_no_frag[mal_ndx], 0x1, 170, exit);
  SOC_PB_FLD_SET(regs->epni.egress_interface_no_fragmentation_mode_configuration_reg.nif_no_frag[mal_ndx], 0x1, 172, exit);

  /************************************************************************/
  /* MALG                                                                 */
  /************************************************************************/
  /*
   *    Set Clocks (MAL currently in Hard reset)
   */
  SOC_PB_FLD_ISET(regs->nif.nif_clocks_reg[SOC_PB_REG_NIF_DIRECTION_RX].mal_clock_select[mal_inner_id], SOC_PB_NIF_MAL_CLK_SEL_VAL_RXAUI, malg_id, 12, exit);
  fld_val = SOC_PB_NIF_IS_BASE_MAL(mal_ndx)?SOC_PB_NIF_MAL_CLK_SEL_VAL_RXAUI:SOC_PB_NIF_MAL_CLK_SEL_VAL_TX_EN;
  SOC_PB_FLD_ISET(regs->nif.nif_clocks_reg[SOC_PB_REG_NIF_DIRECTION_TX].mal_clock_select[mal_inner_id], fld_val, malg_id, 14, exit);

 /************************************************************************/
  /* NBI                                                                  */
  /************************************************************************/

  /*
   *    MLF FIFO In-Reset
   *  Note: should not be performed under traffic
   */
  nif_id_end = SOC_SAND_RNG_LAST(nif_id_base, SOC_PB_MAX_NIFS_PER_MAL);
  SOC_PB_NIF_BOTH_DIRECTIONS(directn_idx)
  {
    for (nif_id = nif_id_base; nif_id <= nif_id_end; nif_id++)
    {
      fld_val = 0x0;
      SOC_PB_FLD_SET(regs->nbi.mlf_reset_ports_reg[malg_id][directn_idx].ports_srstn[nif_id], 0x0, 20 + directn_idx, exit);
    }
  }

  /*
   *    MLF FIFO Configuration
   */
  /* XAUI - 1 FIFO */
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_config_reg[mal_ndx].rx_num_logical_fifos_mode, 0x2, 30, exit);
  SOC_PB_FLD_SET(regs->nbi.tx_mlf_config_reg[mal_ndx].tx_num_logical_fifos_mode, 0x2, 32, exit);
  /* Disable discard short packets */
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_config_reg[mal_ndx].rx_discard_too_short_err_pkts, 0x0, 34, exit);
  SOC_PB_FLD_SET(regs->nbi.tx_mlf_config_reg[mal_ndx].tx_start_tx_threshold, 0x8, 36, exit);
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_thresholds_config_reg[mal_ndx].rx_llfc_threshold_set, 64, 38, exit);
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_thresholds_config_reg[mal_ndx].rx_llfc_threshold_clr, 60, 39, exit);
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_thresholds_config_reg[mal_ndx].rx_cbfc_threshold_set, 44, 40, exit);
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_thresholds_config_reg[mal_ndx].rx_cbfc_threshold_clr, 40, 41, exit);
  
  /*
   *    MLF FIFO Out-Of-Reset
   */
  SOC_PB_NIF_BOTH_DIRECTIONS(directn_idx)
  {
    /* Just base nif-port for XAUI */
    SOC_PB_FLD_SET(regs->nbi.mlf_reset_ports_reg[malg_id][directn_idx].ports_srstn[nif_id_base], 0x1, 50 + directn_idx, exit);
  }

  /*
   *    Flow Control - set XAUI mode
   */
  SOC_PB_FLD_SET(regs->nbi.fc_reset_reg.fc_mal_xaui_mode[mal_ndx], 0x1, 55, exit);

  /************************************************************************/
  /* SerDes                                                               */
  /************************************************************************/

  soc_pb_nif_srd_lanes_get(
      unit,
      SOC_PB_NIF_ID(RXAUI, mal_ndx),
      &srd_nof_lanes,
      &srd_base_lane_global
    );

  if (srd_nof_lanes == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_SRD_LANES_COMP_ERR, 57, exit);
  }

  srd_base_lane_inner = SOC_PETRA_SRD_LANE2INNER_ID(srd_base_lane_global);
  srd_end_lane_inner = SOC_SAND_RNG_LAST(srd_base_lane_inner, srd_nof_lanes);
  star_id = SOC_PETRA_SRD_LANE2STAR(srd_base_lane_global);
  srd_instance_id = SOC_PETRA_SRD_STAR2INSTANCE(star_id);
  srd_qrtt_id = SOC_PETRA_SRD_LANE2QRTT_INNER(srd_base_lane_global);
  srd_qrtt_id_glbl = SOC_PETRA_SRD_LANE2QRTT_GLBL(srd_base_lane_global);

  SOC_PB_FLD_ISET(regs->serdes.srd_cmu_cfga_reg[srd_qrtt_id].ln_20b_mode, 0x1, srd_instance_id, 60, exit);
  SOC_PB_FLD_ISET(regs->serdes.srd_cmu_cfga_reg[srd_qrtt_id].ln_tx_swap_i1_en, 0x0, srd_instance_id, 64, exit);

  res = soc_petra_srd_qrtt_sync_fifo_en(
          unit,
          srd_qrtt_id_glbl,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  for (srd_lane_id = srd_base_lane_inner; srd_lane_id <= srd_end_lane_inner; srd_lane_id++)
  {
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_mask_disparity_errors, 0x0, srd_instance_id, 80, exit);
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_decouple_disparity, 0x0, srd_instance_id, 81, exit);
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].rx_nearest_comma_align, 0x1, srd_instance_id, 82, exit);
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_comma_det_mask_lsb, 0x1, srd_instance_id, 84, exit);
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_comma_det_mask_msb, 0x1, srd_instance_id, 86, exit);
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].tx_use_ext_byte_toggle, 0x1, srd_instance_id, 88, exit);
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_align_en_req, 0x1, srd_instance_id, 89, exit);
  }

  /************************************************************************/
  /* MAL                                                                  */
  /************************************************************************/
  reg_val = 0;
  fld_val = 0x0;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.enable_reg.xaui_sr_mode, fld_val, reg_val, 112, exit);
  fld_val = 0x1;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.enable_reg.xaui_dr_mode, fld_val, reg_val, 112, exit);
  fld_val = 0x1;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.enable_reg.xaui_rx_lane_en, fld_val, reg_val, 113, exit);
  fld_val = 0x1;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.enable_reg.xaui_tx_lane_en, fld_val, reg_val, 114, exit);
  SOC_PB_REG_ISET(regs->nif_mac_lane.enable_reg, reg_val, mal_ndx, 120, exit);

  SOC_PB_REG_IGET(regs->nif_mac_lane.async_fifo_config_reg, reg_val, mal_ndx, 130, exit);
  fld_val = 0x1;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.async_fifo_config_reg.tx_mlf_credit_ovrd, fld_val, reg_val, 130, exit);
  fld_val = 0xa;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.async_fifo_config_reg.taf_af_wm, fld_val, reg_val, 132, exit);
  SOC_PB_REG_ISET(regs->nif_mac_lane.async_fifo_config_reg, reg_val, mal_ndx, 134, exit);

  /* 
   * Lanes Swap 
   */
  SOC_PB_REG_IGET(regs->nif_mac_lane.bom_reg, reg_val, mal_ndx, 140, exit);
  fld_val = SOC_SAND_BOOL2NUM(topology->lanes_swap.swap_rx);
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.bom_reg.rx_swap_ll, fld_val, reg_val, 142, exit);
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.bom_reg.rx_swap_ll_pair, fld_val, reg_val, 144, exit);
  fld_val = SOC_SAND_BOOL2NUM(topology->lanes_swap.swap_tx);
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.bom_reg.tx_swap_ll, fld_val, reg_val, 146, exit);
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.bom_reg.tx_swap_ll_pair, fld_val, reg_val, 148, exit);
  SOC_PB_REG_ISET(regs->nif_mac_lane.bom_reg, reg_val, mal_ndx, 150, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_mal_rxaui_set()", mal_ndx, 0);
}

uint32
  soc_pb_nif_mal_gmii_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  uint8                      is_sgmii_not_qsgmii
  )
{
  uint32
    fld_val = 0,
    reg_val = 0,
    res = SOC_SAND_OK;
  uint32
    directn_idx,
    nif_port_id,
    nif_port_id_base = SOC_PB_MAL2NIF_INNER_ID(mal_ndx),
    nif_port_id_end,
    malg_id = SOC_PB_NIF_MAL2MALG_ID(mal_ndx),
    mal_inner_id,
    reg_idx,
    fld_idx;
  SOC_PETRA_SRD_STAR_ID
    star_id;
  SOC_PETRA_INTERFACE_ID
    nif_ndx;
  uint32
    srd_base_lane_global,
    srd_qrtt_id_glbl,
    srd_nof_lanes,
    srd_base_lane_inner,
    srd_end_lane_inner,
    srd_qrtt_id,
    srd_instance_id,
    srd_lane_id;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MAL_GMII_SET);

  regs = soc_petra_regs();

  mal_inner_id = SOC_PB_NIF_MAL2INNER_ID(mal_ndx);

  /************************************************************************/
  /* NBI                                                                  */
  /************************************************************************/
  /*
   *    MLF Configuration
   */
  /* SGMII - 2 FIFOs, QSGMII - 4 FIFOs */
  fld_val = (is_sgmii_not_qsgmii)?0x1:0x0;
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_config_reg[mal_ndx].rx_num_logical_fifos_mode, fld_val, 20, exit);
  SOC_PB_FLD_SET(regs->nbi.tx_mlf_config_reg[mal_ndx].tx_num_logical_fifos_mode, fld_val, 22, exit);

  /* Disable discard short packets */
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_config_reg[mal_ndx].rx_discard_too_short_err_pkts, 0, 24, exit);

  SOC_PB_FLD_SET(regs->nbi.tx_mlf_config_reg[mal_ndx].tx_start_tx_threshold, 0x2, 26, exit);
  
  /*
   *    FC thresholds
   */
  fld_val = (is_sgmii_not_qsgmii)?32:16;
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_thresholds_config_reg[mal_ndx].rx_llfc_threshold_set, fld_val, 28, exit);
  fld_val -= 4;
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_thresholds_config_reg[mal_ndx].rx_llfc_threshold_clr, fld_val, 29, exit);

  fld_val = (is_sgmii_not_qsgmii)?32:16;
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_thresholds_config_reg[mal_ndx].rx_cbfc_threshold_set, fld_val, 30, exit);
  fld_val -= 4;
  SOC_PB_FLD_SET(regs->nbi.rx_mlf_thresholds_config_reg[mal_ndx].rx_cbfc_threshold_clr, fld_val, 31, exit);
  
  /*
   *    FIFO Out-Of-Reset
   */
  nif_port_id_end = SOC_SAND_RNG_LAST(nif_port_id_base, SOC_PB_MAX_NIFS_PER_MAL);
  SOC_PB_NIF_BOTH_DIRECTIONS(directn_idx)
  {
    for (nif_port_id = nif_port_id_base; nif_port_id <= nif_port_id_end; nif_port_id++)
    {
      if (is_sgmii_not_qsgmii && (!(SOC_PB_NIF_IS_SGMII_NIF_P_ID(nif_port_id)))
      {
        /*
         *    Skip odd NIF-ports for SGMII
         */
        continue;
      }
      SOC_PB_FLD_SET(regs->nbi.mlf_reset_ports_reg[malg_id][directn_idx].ports_srstn[nif_port_id], 1, 40 + directn_idx, exit);
    }
  }

  /*
   *    Flow Control - set SGMII mode
   */
  SOC_PB_FLD_SET(regs->nbi.fc_reset_reg.fc_mal_xaui_mode[mal_ndx], 0x0, 50, exit);

  
  /************************************************************************/
  /* SerDes                                                               */
  /************************************************************************/
  nif_ndx = (is_sgmii_not_qsgmii)?SOC_PB_NIF_ID(SGMII, mal_ndx*SOC_PB_SRD_LANES_PER_MAL):SOC_PB_NIF_ID(QSGMII, mal_ndx*SOC_PB_SRD_LANES_PER_DMAL);
  soc_pb_nif_srd_lanes_get(
      unit,
      nif_ndx,
      &srd_nof_lanes,
      &srd_base_lane_global
    );

  if (srd_nof_lanes == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_SRD_LANES_COMP_ERR, 57, exit);
  }

  srd_base_lane_inner = SOC_PETRA_SRD_LANE2INNER_ID(srd_base_lane_global);
  if(is_sgmii_not_qsgmii && srd_base_lane_inner > 2) {
    /* In this case the srd_base_lane_inner should be 0 or 2 */
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
  }
  srd_end_lane_inner = (is_sgmii_not_qsgmii)?SOC_SAND_RNG_LAST(srd_base_lane_inner, SOC_PB_SRD_LANES_PER_MAL):srd_base_lane_inner;
  star_id = SOC_PETRA_SRD_LANE2STAR(srd_base_lane_global);
  srd_instance_id = SOC_PETRA_SRD_STAR2INSTANCE(star_id);
  srd_qrtt_id = SOC_PETRA_SRD_LANE2QRTT_INNER(srd_base_lane_global);
  srd_qrtt_id_glbl = SOC_PETRA_SRD_LANE2QRTT_GLBL(srd_base_lane_global);
 
  fld_val = (is_sgmii_not_qsgmii)?0x0:0x1;
  SOC_PB_FLD_ISET(regs->serdes.srd_cmu_cfga_reg[srd_qrtt_id].ln_20b_mode, fld_val, srd_instance_id, 60, exit);
  fld_val = (is_sgmii_not_qsgmii)?0x1:0x0;
  SOC_PB_FLD_ISET(regs->serdes.srd_cmu_cfga_reg[srd_qrtt_id].ln_tx_swap_i1_en, fld_val, srd_instance_id, 64, exit);

  res = soc_petra_srd_qrtt_sync_fifo_en(
          unit,
          srd_qrtt_id_glbl,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  
  for (srd_lane_id = srd_base_lane_inner; srd_lane_id <= srd_end_lane_inner; srd_lane_id++)
  {
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].rx_nearest_comma_align, 0x0, srd_instance_id, 96, exit);
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].tx_use_ext_byte_toggle, 0x0, srd_instance_id, 98, exit);

    fld_val = (is_sgmii_not_qsgmii)?0x0:0x1;
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_align_en_req, 1, srd_instance_id, 100, exit);
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_comma_det_mask_lsb, fld_val, srd_instance_id, 102, exit);
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_rx_comma_det_mask_msb, fld_val, srd_instance_id, 104, exit);
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ln_mask_disparity_errors, fld_val, srd_instance_id, 110, exit);
  }
  
  /************************************************************************/
  /* MALG                                                                 */
  /************************************************************************/
  /*
   *    Clocks
   */
  res = soc_pb_nif_gmii_clk_config(
          unit,
          mal_ndx,
          is_sgmii_not_qsgmii,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
  if (!is_sgmii_not_qsgmii)
  {
      reg_idx = SOC_PETRA_REG_IDX_GET(mal_inner_id, SOC_PB_REG_NIF_NOF_PAEB_FLDS);
      fld_idx = SOC_PETRA_FLD_IDX_GET(mal_inner_id, SOC_PB_REG_NIF_NOF_PAEB_FLDS);
      SOC_PB_FLD_ISET(regs->nif.nif_paeb_reset_reg[reg_idx].rx_ln0_paeb_rd_ptr_rst[fld_idx], 0xc, malg_id, 30, exit);
  }


  /************************************************************************/
  /* MAL                                                                  */
  /************************************************************************/
  reg_val = 0;
  fld_val = (is_sgmii_not_qsgmii)?0x5:0xf;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.enable_reg.gmii_rx_lane_en, fld_val, reg_val, 113, exit);
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.enable_reg.gmii_tx_lane_en, fld_val, reg_val, 114, exit);
  if (is_sgmii_not_qsgmii)
  {
    SOC_PB_FLD_TO_REG(regs->nif_mac_lane.enable_reg.gmii_mode, fld_val, reg_val, 115, exit);
    SOC_PB_FLD_TO_REG(regs->nif_mac_lane.enable_reg.gmii_two_ports_mode, fld_val, reg_val, 116, exit);
  }
  else
  {
    SOC_PB_FLD_TO_REG(regs->nif_mac_lane.enable_reg.qsgmii_mode, fld_val, reg_val, 117, exit);
  }
  SOC_PB_REG_ISET(regs->nif_mac_lane.enable_reg, reg_val, mal_ndx, 120, exit);

  SOC_PB_REG_IGET(regs->nif_mac_lane.async_fifo_config_reg, reg_val, mal_ndx, 130, exit);
  fld_val = 0x0;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.async_fifo_config_reg.tx_mlf_credit_ovrd, fld_val, reg_val, 132, exit);
  fld_val = 0xf;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.async_fifo_config_reg.taf_af_wm, fld_val, reg_val, 134, exit);
  SOC_PB_REG_ISET(regs->nif_mac_lane.async_fifo_config_reg, reg_val, mal_ndx, 136, exit);

  fld_val = (is_sgmii_not_qsgmii)?0x0:0x1;
  SOC_PB_FLD_ISET(regs->nif_mac_lane.gmii_config1_reg.gmii_rx_ignore_comma_position, fld_val, mal_ndx, 138, exit);

  if (!is_sgmii_not_qsgmii)
  {
    SOC_PB_NIF_BOTH_DIRECTIONS(directn_idx)
    {
      SOC_PB_FLD_ISET(regs->nif.nif_qsgmii_reset_reg.mal_qsgmii_phase_rstn[mal_inner_id][directn_idx], fld_val, malg_id, 140, exit);
    }
  }

  /*
   *    Set as "no fragmentation"(full packet mode)
   */
  SOC_PB_FLD_SET(regs->egq.egress_interface_no_fragmentation_mode_configuration_reg.nif_no_frag[mal_ndx], 0x1, 170, exit);
  SOC_PB_FLD_SET(regs->epni.egress_interface_no_fragmentation_mode_configuration_reg.nif_no_frag[mal_ndx], 0x1, 172, exit)
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_mal_gmii_set()", mal_ndx, 0);
}

uint32
  soc_pb_nif_gmii_clk_config(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 mal_ndx,
    SOC_SAND_IN uint8 is_sgmii_not_qsgmii,
    SOC_SAND_IN uint8 alt_srd_mapping /* Used to override register configuration */
  )
{
  uint32
    res = SOC_SAND_OK,
    malg_id = SOC_PB_NIF_MAL2MALG_ID(mal_ndx),
    mal_inner_id = SOC_PB_NIF_MAL2INNER_ID(mal_ndx),
    rx_fld_val,
    tx_fld_val;
  uint8
    alt_srd_mapping_get = FALSE;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

  /* Check register configuration */
  res = soc_pb_nif_mal_qsgmii_alt_get_unsafe(unit, mal_ndx, &alt_srd_mapping_get);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Override register configuration if needed.                      */
  /* This is required because this function can be called before the */
  /* QSGMII alternative mapping is set. (during INIT)                */
  alt_srd_mapping_get = alt_srd_mapping_get | alt_srd_mapping;

  if(alt_srd_mapping_get)
  {
    /* Here we assume that the alternative mapping      */
    /* will only be used when configuring QSGMII        */
    /* so no need to check the is_sgmii_not_qsgmii flag */
    rx_fld_val = SOC_PB_NIF_QSGMII_ALT_CLK_VAL_4BIT;

    /* MAL 6 configuration is different because for TX          */
    /* the register's fields are different for even MAL indexes */
    tx_fld_val = (mal_inner_id == 6) ? SOC_PB_NIF_QSGMII_ALT_CLK_VAL_4BIT : SOC_PB_NIF_QSGMII_ALT_CLK_VAL_2BIT; 
  }
  else
  {
    /* Normal configuration */
    rx_fld_val = (is_sgmii_not_qsgmii) ? SOC_PB_NIF_MAL_CLK_SEL_VAL_SGMII : SOC_PB_NIF_MAL_CLK_SEL_VAL_QSGMII;
    tx_fld_val = SOC_PB_NIF_IS_BASE_MAL(mal_ndx) ? rx_fld_val : SOC_PB_NIF_MAL_CLK_SEL_VAL_TX_EN;
  }
  
  SOC_PB_FLD_ISET(regs->nif.nif_clocks_reg[SOC_PB_REG_NIF_DIRECTION_RX].mal_clock_select[mal_inner_id], rx_fld_val, malg_id, 12, exit);
  SOC_PB_FLD_ISET(regs->nif.nif_clocks_reg[SOC_PB_REG_NIF_DIRECTION_TX].mal_clock_select[mal_inner_id], tx_fld_val, malg_id, 14, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_gmii_clk_config()", 0, 0);
}

uint32
  soc_pb_nif_mal_gmii_get(
    SOC_SAND_IN   int                      unit,
    SOC_SAND_IN   uint32                      mal_ndx,
    SOC_SAND_OUT  uint8                      *is_sgmii_not_qsgmii
  )
{
  uint32
    res;

  SOC_PB_NIF_TYPE
    nif_type;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MAL_GMII_GET);

  res = soc_pb_nif_mal_type_get(
          unit,
          mal_ndx,
          &nif_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  *is_sgmii_not_qsgmii = (nif_type = SOC_PB_NIF_TYPE_SGMII);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_mal_gmii_get()", mal_ndx, 0);
}

STATIC uint32
  soc_pb_nif_on_off_set_intern(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_STATE_INFO              *info,
    SOC_SAND_IN  SOC_PB_NIF_TYPE                    nif_type,
    SOC_SAND_IN  uint8                      is_enabled
  )
{
  uint32
    enable_fld_val,
    res = SOC_SAND_OK;
  uint32
    nif_id,
    mal_id;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ON_OFF_SET_UNSAFE);

  regs = soc_petra_regs();

  /*
   *    Indexing
   */
  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);

  enable_fld_val = SOC_SAND_BOOL2NUM(is_enabled);

  
  if (SOC_PB_NIF_IS_TYPE_GMII(nif_type))
  {
    if (is_enabled)
    {
      /*
       * For GMII - do not power down the MAL (possible for QSGMII, not for SGMII)
       * Just verify it is up if requested.
       */
      SOC_PB_FLD_ISET(regs->nif_mac_lane.enable_reg.mal_en, enable_fld_val, mal_id, 10, exit);
    }

    res = soc_pb_nif_gmii_enable_state_set(
            unit,
            nif_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_BOTH,
            is_enabled
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }
  else if (SOC_PB_NIF_IS_TYPE_XAUI_LIKE(nif_type))
  {
    if ((nif_type) == SOC_PB_NIF_TYPE_XAUI)
    {
      /* Disable the secondary MAL for XAUI */
      SOC_PB_FLD_ISET(regs->nif_mac_lane.enable_reg.mal_en, 0x0, SOC_PB_NIF_SCND_MAL(mal_id), 14, exit);
    }
    SOC_PB_FLD_ISET(regs->nif_mac_lane.enable_reg.mal_en, enable_fld_val, mal_id, 16, exit);
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(ILKN,nif_ndx))
  {
    res = soc_pb_nif_ilkn_reset(
              unit,
              SOC_PB_NIF_ID_OFFSET(ILKN, nif_ndx),
              is_enabled,
              FALSE,
              SOC_PETRA_CONNECTION_DIRECTION_BOTH
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_INVALID_ID_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_on_off_set_intern()", nif_ndx, nif_type);
}

/*********************************************************************
 *     Turns the interface on/off. Optionally, powers up/down
 *     the attached SerDes also.
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_on_off_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_STATE_INFO              *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    base_lane_id,
    srd_lane_id,
    nof_srd_lanes;
  SOC_PB_NIF_TYPE
    nif_type;
  uint8
    is_enabled;
  SOC_PETRA_SRD_POWER_STATE
    srd_pwr_state = SOC_PETRA_SRD_NOF_POWER_STATES;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ON_OFF_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  nif_type = soc_pb_nif_id2type(nif_ndx);
  if (nif_type == SOC_PB_NIF_TYPE_NONE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_UNKNOWN_NIF_TYPE_ERR, 3, exit);
  }

  is_enabled = info->is_nif_on;

  soc_pb_nif_srd_lanes_get(unit, nif_ndx, &nof_srd_lanes, &base_lane_id);

  if (nof_srd_lanes == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_UNKNOWN_NIF_TYPE_ERR, 5, exit);
  }

  if (nif_ndx == SOC_PETRA_NIF_ID_ILKN_0)
  {
    if (SOC_SAND_RNG_LAST(base_lane_id, nof_srd_lanes) >= SOC_PB_NIF_ILKN_SRD_COMBO_A_LN_ID)
    {
      nof_srd_lanes += SOC_PETRA_SRD_NOF_LANES_PER_QRTT;
    }
  }

  res = soc_pb_nif_on_off_set_intern(
          unit,
          nif_ndx,
          info,
          nif_type,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (info->serdes_also)
  {
    srd_pwr_state = (info->is_nif_on)?SOC_PETRA_SRD_POWER_STATE_UP_AND_RELOCK:SOC_PETRA_SRD_POWER_STATE_DOWN;

    /*
     *  Set the NIF SerDes power-up/down state
     */
    for (srd_lane_id = base_lane_id; srd_lane_id < (base_lane_id + nof_srd_lanes); srd_lane_id++)
    {
      if (SOC_PB_NIF_IS_TYPE_ID(ILKN, nif_ndx) && SOC_PETRA_SRD_IS_COMBO_0_LANE(srd_lane_id))
      {
        /* Skip Combo-0 lanes for ILKN */
        continue;
      }

      res = soc_petra_srd_lane_power_state_set_unsafe(
              unit,
              srd_lane_id,
              SOC_PETRA_CONNECTION_DIRECTION_BOTH,
              srd_pwr_state
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
    }
  }

  if (is_enabled)
  {
    res = soc_pb_nif_on_off_set_intern(
            unit,
            nif_ndx,
            info,
            nif_type,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_on_off_set_unsafe()", 0, 0);
}

uint32
  soc_pb_nif_on_off_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_STATE_INFO              *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ON_OFF_SET_VERIFY);

  res = soc_pb_nif_id_verify(nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_STATE_INFO, info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_on_off_set_verify()", 0, 0);
}

uint32
  soc_pb_nif_on_off_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ON_OFF_GET_VERIFY);

  res = soc_pb_nif_id_verify(nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_on_off_get_verify()", 0, 0);
}

/*********************************************************************
*     Turns the interface on/off. Optionally, powers up/down
 *     the attached SerDes also.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_on_off_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_STATE_INFO              *info
  )
{
  uint32
    enable_fld_val,
    enable_fld_val2,
    res = SOC_SAND_OK;
  SOC_PB_NIF_TYPE
    nif_type;
  uint32
    nif_id,
    mal_id,
    base_lane_id,
    srd_lane_id,
    nof_srd_lanes;
  uint8
    rx_enable = FALSE,
    tx_enable = FALSE,
    is_enabled = FALSE;
  uint8
    is_srd_pwr_same = TRUE;
  SOC_PETRA_SRD_POWER_STATE
    srd_pwr_state_rx = SOC_PETRA_SRD_NOF_POWER_STATES,
    srd_pwr_state_tx = SOC_PETRA_SRD_NOF_POWER_STATES,
    srd_pwr_state_first= SOC_PETRA_SRD_NOF_POWER_STATES;

  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ON_OFF_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PB_NIF_STATE_INFO_clear(info);

  nif_type = soc_pb_nif_id2type(nif_ndx);
  if (nif_type == SOC_PB_NIF_TYPE_NONE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_UNKNOWN_NIF_TYPE_ERR, 3, exit);
  }

  soc_pb_nif_srd_lanes_get(unit, nif_ndx, &nof_srd_lanes, &base_lane_id);

  if (nof_srd_lanes == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_UNKNOWN_NIF_TYPE_ERR, 5, exit);
  }

  /*
   *    Indexing
   */
  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);

   if (SOC_PB_NIF_IS_TYPE_GMII(nif_type))
  {
    SOC_PB_FLD_IGET(regs->nif_mac_lane.enable_reg.mal_en, enable_fld_val, mal_id, 10, exit);
 
    if (enable_fld_val)
    {
      res = soc_pb_nif_gmii_enable_state_get(
              unit,
              nif_ndx,
              &rx_enable,
              &tx_enable
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

      is_enabled = ((rx_enable == TRUE) && (tx_enable == TRUE));
    }
  }
  else if (SOC_PB_NIF_IS_TYPE_XAUI_LIKE(nif_type))
  {
    SOC_PB_FLD_IGET(regs->nif_mac_lane.enable_reg.mal_en, enable_fld_val, mal_id, 20, exit);
    is_enabled = SOC_SAND_NUM2BOOL(enable_fld_val);
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(ILKN,nif_ndx))
  {
    SOC_PB_FLD_GET(regs->nbi.ilknreset_reg.ilkn_controller_rstn[SOC_PB_NIF_ID_OFFSET(ILKN, nif_ndx)][SOC_PB_REG_NIF_DIRECTION_RX], enable_fld_val, 25, exit);
    SOC_PB_FLD_GET(regs->nbi.ilknreset_reg.ilkn_controller_rstn[SOC_PB_NIF_ID_OFFSET(ILKN, nif_ndx)][SOC_PB_REG_NIF_DIRECTION_TX], enable_fld_val2, 27, exit);
    is_enabled = SOC_SAND_NUM2BOOL((enable_fld_val == 0x1) && (enable_fld_val2 == 0x1));
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_INVALID_ID_ERR, 30, exit);
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

  info->is_nif_on = is_enabled;

  if (!is_srd_pwr_same)
  {
    info->serdes_also = FALSE;
  }
  else
  {
    if(
        (is_enabled && (srd_pwr_state_first == SOC_PETRA_SRD_POWER_STATE_UP)) ||
        (!(is_enabled) && (srd_pwr_state_first == SOC_PETRA_SRD_POWER_STATE_DOWN))
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
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_on_off_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets the specified Network Interface to loopback, by
 *     setting all its SerDeses in loopback (NSILB)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_loopback_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint8                      enable
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    srd_lane_id,
    nof_srd_lanes,
    base_lane_id;
  SOC_PB_NIF_STATE_INFO
    nif_state,
    nif_state_orig;
  SOC_PETRA_SRD_LANE_LOOPBACK_MODE
    lpbk_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_LOOPBACK_SET_UNSAFE);

  SOC_PB_NIF_STATE_INFO_clear(&nif_state);
  lpbk_mode = (enable == TRUE)?SOC_PETRA_SRD_LANE_LOOPBACK_NSILB:SOC_PETRA_SRD_LANE_LOOPBACK_NONE;
  soc_pb_nif_srd_lanes_get(unit, nif_ndx, &nof_srd_lanes, &base_lane_id);

  if ((nif_ndx == SOC_PETRA_NIF_ID_ILKN_0))
  {
    if (SOC_SAND_RNG_LAST(base_lane_id, nof_srd_lanes) >= SOC_PB_NIF_ILKN_SRD_COMBO_A_LN_ID)
    {
      nof_srd_lanes += SOC_PETRA_SRD_NOF_LANES_PER_QRTT;
    }
  }

  /*
   *    Read the original NIF enable-state
   */
  res = soc_pb_nif_on_off_get_unsafe(
          unit,
          nif_ndx,
          &nif_state_orig
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  nif_state_orig.serdes_also = FALSE;

  if (nif_state_orig.is_nif_on == TRUE)
  {
    nif_state.is_nif_on = FALSE;
    nif_state.serdes_also = FALSE;
    res = soc_pb_nif_on_off_set_unsafe(
            unit,
            nif_ndx,
            &nif_state
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  
  /*
   *  Set the NIF SerDes
   */
  for (srd_lane_id = base_lane_id; srd_lane_id < (base_lane_id + nof_srd_lanes); srd_lane_id++)
  {
    if (SOC_PB_NIF_IS_TYPE_ID(ILKN, nif_ndx) && SOC_PETRA_SRD_IS_COMBO_0_LANE(srd_lane_id))
    {
      /* Skip Combo-0 lanes for ILKN */
      continue;
    }

    res = soc_petra_srd_lane_loopback_mode_verify(
            unit,
            srd_lane_id,
            lpbk_mode
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_petra_srd_lane_loopback_mode_set_unsafe(
            unit,
            srd_lane_id,
            lpbk_mode
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

    res = soc_petra_srd_relock_unsafe(
            unit,
            srd_lane_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40+srd_lane_id, exit);
  }

  /*
   *    Revert the original NIF enable-state
   */
  if (nif_state_orig.is_nif_on == TRUE)
  {
    res = soc_pb_nif_on_off_set_unsafe(
            unit,
            nif_ndx,
            &nif_state_orig
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_loopback_set_unsafe()", nif_ndx, 0);
}

uint32
  soc_pb_nif_loopback_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint8                      enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_LOOPBACK_SET_VERIFY);

  res = soc_pb_nif_id_verify(nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_loopback_set_verify()", 0, 0);
}

uint32
  soc_pb_nif_loopback_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_LOOPBACK_GET_VERIFY);

  res = soc_pb_nif_id_verify(nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_loopback_get_verify()", 0, 0);
}

/*********************************************************************
*     Sets the specified Network Interface to loopback, by
 *     setting all its SerDeses in loopback (NSILB)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_loopback_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT uint8                      *enable
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    srd_lane_id,
    base_lane_id,
    nof_srd_lanes;
  SOC_PETRA_SRD_LANE_LOOPBACK_MODE
    lpbk_mode = SOC_PETRA_SRD_LANE_LOOPBACK_NONE,
    lpbk_mode_first= SOC_PETRA_SRD_LANE_LOOPBACK_NONE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_LOOPBACK_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(enable);

  soc_pb_nif_srd_lanes_get(unit, nif_ndx, &nof_srd_lanes, &base_lane_id);

  if ((nif_ndx == SOC_PETRA_NIF_ID_ILKN_0))
  {
    if (SOC_SAND_RNG_LAST(base_lane_id, nof_srd_lanes) >= SOC_PB_NIF_ILKN_SRD_COMBO_A_LN_ID)
    {
      nof_srd_lanes += SOC_PETRA_SRD_NOF_LANES_PER_QRTT;
    }
  }

  for (srd_lane_id = base_lane_id; srd_lane_id < (base_lane_id + nof_srd_lanes); srd_lane_id++)
  {
    if (SOC_PB_NIF_IS_TYPE_ID(ILKN, nif_ndx) && SOC_PETRA_SRD_IS_COMBO_0_LANE(srd_lane_id))
    {
      /* Skip Combo-0 lanes for ILKN */
      continue;
    }

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
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_loopback_get_unsafe()", nif_ndx, 0);
}

uint32
  soc_pb_nif_mal_qsgmii_alt_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  uint8                      enable
  )
{
  uint32
    res = SOC_SAND_OK,
    malg_id = 0,
    fld_val = 0;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MAL_QSGMII_ALT_SET_UNSAFE);

  regs = soc_petra_regs();

  malg_id = SOC_PB_NIF_MAL2MALG_ID(mal_ndx);
  fld_val = SOC_SAND_BOOL2NUM(enable);

  /* Handles alternative Serdes mapping for MALs 5,6,7 (malg = 0) and 13,14,15 (malg = 1) */
  /* MAL 5  --> LANE 1 */
  /* MAL 6  --> LANE 7 */
  /* MAL 7  --> LANE 3 */
  /* MAL 13 --> LANE 17 */
  /* MAL 14 --> LANE 23 */
  /* MAL 15 --> LANE 19 */
  switch (mal_ndx) {
      case 5: 
      case 13: 
        SOC_PB_FLD_ISET(regs->nif.nif_config_reg.mal5_qsgmii_alt_srd, fld_val, malg_id, 10, exit);
        break;
      case 6: 
      case 14: 
        SOC_PB_FLD_ISET(regs->nif.nif_config_reg.mal6_qsgmii_alt_srd, fld_val, malg_id, 20, exit);
        break;
      case 7: 
      case 15: 
        SOC_PB_FLD_ISET(regs->nif.nif_config_reg.mal7_qsgmii_alt_srd, fld_val, malg_id, 30, exit);
        break;
      default:
        break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_mal_qsgmii_alt_set_unsafe()", mal_ndx, enable);
}

uint32
  soc_pb_nif_mal_qsgmii_alt_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT  uint8                     *enable
  )
{
  uint32
    res = SOC_SAND_OK,
    malg_id = 0,
    fld_val = 0;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MAL_QSGMII_ALT_GET_UNSAFE);

  regs = soc_petra_regs();

  malg_id = SOC_PB_NIF_MAL2MALG_ID(mal_ndx);

  /* Gets the alternative Serdes mapping for MALs 5,6,7 (malg = 0) and 13,14,15 (malg = 1) */
  /* MAL 5  --> LANE 1 */
  /* MAL 6  --> LANE 7 */
  /* MAL 7  --> LANE 3 */
  /* MAL 13 --> LANE 17 */
  /* MAL 14 --> LANE 23 */
  /* MAL 15 --> LANE 19 */
  switch (mal_ndx) {
      case 5: 
      case 13: 
        SOC_PB_FLD_IGET(regs->nif.nif_config_reg.mal5_qsgmii_alt_srd, fld_val, malg_id, 10, exit);
        break;
      case 6: 
      case 14: 
        SOC_PB_FLD_IGET(regs->nif.nif_config_reg.mal6_qsgmii_alt_srd, fld_val, malg_id, 20, exit);
        break;
      case 7: 
      case 15: 
        SOC_PB_FLD_IGET(regs->nif.nif_config_reg.mal7_qsgmii_alt_srd, fld_val, malg_id, 30, exit);
        break;
      default:
        break;
  }

  *enable = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_mal_qsgmii_alt_set_unsafe()", mal_ndx, SOC_SAND_NUM2BOOL(enable));
}

/*********************************************************************
 *     Sets basic MAL configuration. This configuration defines
 *     MAL to SerDes mapping topology. Following setting basic
 *     configuration, per-NIF-type (e.g. SPAUI/SGMII etc.)
 *     configuration must be set before enabling the MAL.
 *     Details: in the H file. (search for prototype)
 *
 *     A typical configuration sequence of a MAL is as follows:
 *     1. MAL in-reset (Hard)  (this also sets mal_enable = FALSE)
 *     2. MLF Clock set (mode+enabled)
 *     3. MAL out-of-reset (Hard)
 *     4. Configuration, according to the NIF type, e.g.:
 *        - MLF (FIFO) in-reset mlf_reset_ports_reg (e.g. all 4 for XAUI)
 *        - MLF (FIFO) configuration
 *        - MLF (FIFO) out-of-reset mlf_reset_ports_reg e.g. all 4 for XAUI)
 *        - FC configuration
 *        - SerDes configuration
 *        - MAL configuration
 *     5. MAL enable
 *        - Note: if disabled, the SerDes ans the MLF clocks can be
 *          powered down
 *********************************************************************/

uint32
  soc_pb_nif_mal_basic_conf_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PB_NIF_MAL_BASIC_INFO          *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MAL_BASIC_CONF_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->topology.is_qsgmii_alt && (info->type == SOC_PB_NIF_TYPE_QSGMII))
  {
    res = soc_pb_nif_mal_qsgmii_alt_set_unsafe(
            unit, 
            mal_ndx, 
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  }


  res = soc_pb_nif_mal_dflts_set(
          unit,
          mal_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  switch(info->type)
  {
  case SOC_PB_NIF_TYPE_XAUI:
    /*
     *    Default SPAUI configuration
     *  is also XAUI
     */
    res = soc_pb_nif_mal_xaui_set(
            unit,
            mal_ndx,
            &(info->topology)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    break;
  case SOC_PB_NIF_TYPE_RXAUI:
    /*
     *    Default SPAUI configuration
     *  is also XAUI
     */
    res = soc_pb_nif_mal_rxaui_set(
            unit,
            mal_ndx,
            &(info->topology)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    break;
  case SOC_PB_NIF_TYPE_SGMII:
    res = soc_pb_nif_mal_gmii_set(
            unit,
            mal_ndx,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    break;
  case SOC_PB_NIF_TYPE_QSGMII:
    res = soc_pb_nif_mal_gmii_set(
            unit,
            mal_ndx,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_UNKNOWN_NIF_TYPE_ERR, 100, exit);
      break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_mal_basic_conf_set_unsafe()", mal_ndx, 0);
}

uint32
  soc_pb_nif_mal_basic_conf_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PB_NIF_MAL_BASIC_INFO          *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MAL_BASIC_CONF_VERIFY);

  res = soc_pb_nif_mal_id_verify(mal_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_MAL_BASIC_INFO, info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_mal_basic_conf_verify()", mal_ndx, 0);
}


/*********************************************************************
 *     Sets basic MAL configuration. This configuration defines
 *     MAL to SerDes mapping topology. Following setting basic
 *     configuration, per-NIF-type (e.g. SPAUI/SGMII etc.)
 *     configuration must be set before enabling the MAL.
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_mal_basic_conf_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT SOC_PB_NIF_MAL_BASIC_INFO          *info
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_val,
    fld_val;
  uint8
    alt_srd_mapping;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MAL_BASIC_CONF_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_PB_NIF_MAL_BASIC_INFO_clear(info);

  regs = soc_petra_regs();
  res = soc_pb_nif_mal_type_get(
          unit,
          mal_ndx,
          &info->type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /* 
   * Lanes Swap 
   */
  SOC_PB_REG_IGET(regs->nif_mac_lane.bom_reg, reg_val, mal_ndx, 10, exit);
  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.bom_reg.rx_swap_ll, fld_val, reg_val, 12, exit);
  info->topology.lanes_swap.swap_rx = SOC_SAND_NUM2BOOL(fld_val);
  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.bom_reg.tx_swap_ll, fld_val, reg_val, 14, exit);
  info->topology.lanes_swap.swap_tx = SOC_SAND_NUM2BOOL(fld_val);
  res = soc_pb_nif_mal_qsgmii_alt_get_unsafe(unit, mal_ndx, &alt_srd_mapping);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->topology.is_qsgmii_alt = alt_srd_mapping;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_mal_basic_conf_get_unsafe()", mal_ndx, 0);
}

/*********************************************************************
 *     Sets minimal packet size as limited by the NIF. Note:
 *     Normally, the packet size is limited using
 *     soc_petra_mgmt_pckt_size_range_set(), and not this API.
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_min_packet_size_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  uint32                       pckt_size
  )
{
  uint32
    reg_val,
    fld_val,
    res = SOC_SAND_OK;
  uint8
    is_gmii,
    rx_enable = FALSE,
    tx_enable = FALSE;
  SOC_PB_NIF_TYPE
    nif_type;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MIN_PACKET_SIZE_SET_UNSAFE);

  regs = soc_petra_regs();
  rx_enable = SOC_PETRA_IS_DIRECTION_RX(direction_ndx);
  tx_enable = SOC_PETRA_IS_DIRECTION_TX(direction_ndx);

  res = soc_pb_nif_mal_type_get(
          unit,
          mal_ndx,
          &nif_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  is_gmii = SOC_PB_NIF_IS_TYPE_GMII(nif_type);
  
  fld_val = pckt_size;

  if (is_gmii == TRUE)
  {
    SOC_PB_REG_IGET(regs->nif_mac_lane.gmii_config1_reg, reg_val, mal_ndx, 10, exit);

    if (rx_enable)
    {
      SOC_PB_FLD_TO_REG(regs->nif_mac_lane.gmii_config1_reg.gmii_rx_min_pkt_length, fld_val, reg_val, 20, exit);
    }

    if(tx_enable)
    {
      SOC_PB_FLD_TO_REG(regs->nif_mac_lane.gmii_config1_reg.gmii_tx_min_pkt_length, fld_val, reg_val, 30, exit);
    }

    SOC_PB_REG_ISET(regs->nif_mac_lane.gmii_config1_reg, reg_val, mal_ndx, 40, exit);
  }
  else
  {
    SOC_PB_REG_IGET(regs->nif_mac_lane.xaui_config_reg, reg_val, mal_ndx, 110, exit);

    if (rx_enable)
    {
      SOC_PB_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.rx_min_pkt_length, fld_val, reg_val, 120, exit);
    }

    if(tx_enable)
    {
      SOC_PB_FLD_TO_REG(regs->nif_mac_lane.xaui_config_reg.tx_min_pkt_length, fld_val, reg_val, 130, exit);
    }

    SOC_PB_REG_ISET(regs->nif_mac_lane.xaui_config_reg, reg_val, mal_ndx, 140, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_min_packet_size_set_unsafe()", mal_ndx, 0);
}

uint32
  soc_pb_nif_min_packet_size_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  uint32                       pckt_size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MIN_PACKET_SIZE_SET_VERIFY);

  res = soc_pb_nif_min_packet_size_ndx_verify(unit, mal_ndx, direction_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(pckt_size, SOC_PB_NIF_MIN_PCKT_SIZE_MAX, SOC_PB_PCKT_SIZE_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_min_packet_size_verify()", mal_ndx, 0);
}

uint32
  soc_pb_nif_min_packet_size_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MIN_PACKET_SIZE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mal_ndx, SOC_PB_NOF_MAC_LANES, SOC_PB_MAL_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(direction_ndx, SOC_PB_CONNECTION_DIRECTION_NDX_MAX, SOC_PETRA_CONNECTION_DIRECTION_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_min_packet_size_ndx_verify()", mal_ndx, 0);
}

/*********************************************************************
 *     Sets minimal packet size as limited by the NIF. Note:
 *     Normally, the packet size is limited using
 *     soc_petra_mgmt_pckt_size_range_set(), and not this API.
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_min_packet_size_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT uint32                       *pckt_size_rx,
    SOC_SAND_OUT uint32                       *pckt_size_tx
  )
{
  uint32
    reg_val,
    fld_val,
    res = SOC_SAND_OK;
  uint8
    is_gmii;
  SOC_PB_NIF_TYPE
    nif_type;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_MIN_PACKET_SIZE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(pckt_size_rx);
  SOC_SAND_CHECK_NULL_INPUT(pckt_size_tx);

  regs = soc_petra_regs();
  
  res = soc_pb_nif_mal_type_get(
          unit,
          mal_ndx,
          &nif_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  is_gmii = SOC_PB_NIF_IS_TYPE_GMII(nif_type);

  if (is_gmii == TRUE)
  {
    SOC_PB_REG_IGET(regs->nif_mac_lane.gmii_config1_reg, reg_val, mal_ndx, 10, exit);
    SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.gmii_config1_reg.gmii_rx_min_pkt_length, fld_val, reg_val, 20, exit);
    *pckt_size_rx = fld_val;
    SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.gmii_config1_reg.gmii_tx_min_pkt_length, fld_val, reg_val, 30, exit);
    *pckt_size_tx = fld_val;
  }
  else
  {
    SOC_PB_REG_IGET(regs->nif_mac_lane.xaui_config_reg, reg_val, mal_ndx, 110, exit);
    SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.xaui_config_reg.rx_min_pkt_length, fld_val, reg_val, 120, exit);
    *pckt_size_rx = fld_val;
    SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.xaui_config_reg.tx_min_pkt_length, fld_val, reg_val, 130, exit);
    *pckt_size_tx = fld_val;
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_min_packet_size_get_unsafe()", mal_ndx, 0);
}

STATIC uint32
  soc_pb_nif_fault2intern(
    SOC_SAND_IN SOC_PB_NIF_FAULT_RESPONSE fault_rspns
  )
{
  uint32
    rspns;

  switch(fault_rspns) {
  case SOC_PB_NIF_FAULT_RESPONSE_DATA_AND_IDLE:
    rspns = 0x1;
      break;
  case SOC_PB_NIF_FAULT_RESPONSE_DATA_AND_RF:
    rspns = 0x2;
      break;
  case SOC_PB_NIF_FAULT_RESPONSE_DATA_AND_LF:
    rspns = 0x3;
      break;
  case SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_IDLE:
    rspns = 0x5;
      break;
  case SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_RF:
    rspns = 0x6;
      break;
  case SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_LF:
    rspns = 0x7;
      break;
  default:
    rspns = SOC_PB_NIF_INVALID_VAL_INTERN;
  }

  return
    rspns;
}

STATIC SOC_PB_NIF_FAULT_RESPONSE
  soc_pb_nif_intern2fault(
    SOC_SAND_IN uint32    rspns
  )
{
  SOC_PB_NIF_FAULT_RESPONSE
    fault_rspns;
  
  switch(rspns) {
  case 0x1:
    fault_rspns = SOC_PB_NIF_FAULT_RESPONSE_DATA_AND_IDLE;
      break;
  case 0x2:
    fault_rspns = SOC_PB_NIF_FAULT_RESPONSE_DATA_AND_RF;
      break;
  case 0x3:
    fault_rspns = SOC_PB_NIF_FAULT_RESPONSE_DATA_AND_LF;
      break;
  case 0x5:
    fault_rspns = SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_IDLE;
      break;
  case 0x6:
    fault_rspns = SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_RF;
      break;
  case 0x7:
    fault_rspns = SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_LF;
      break;
  default:
    fault_rspns = SOC_PB_NIF_NOF_FAULT_RESPONSES;
  }

  return
    fault_rspns;
}

/*********************************************************************
 *     Sets SPAUI configuration - configure SPAUI extensions
 *     for XAUI interface.
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_spaui_conf_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_SPAUI_INFO              *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_val = 0,
    reg_val = 0;
  SOC_PETRA_REGS
    *regs;
  uint32
    nif_id,
    mal_id;
  uint8
    rx_enable = FALSE,
    tx_enable = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SPAUI_CONF_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  
  regs = soc_petra_regs();

  rx_enable = SOC_PETRA_IS_DIRECTION_RX(direction_ndx);
  tx_enable = SOC_PETRA_IS_DIRECTION_TX(direction_ndx);

  /*
   *    Indexing
   */
  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);

  /* Enable/disable SPAUI { */
  SOC_PB_REG_IGET(regs->nif_mac_lane.enable_reg, reg_val, mal_id, 10, exit);

  fld_val = SOC_SAND_NUM2BOOL(info->enable);

  if (rx_enable)
  {
    SOC_PB_FLD_TO_REG(regs->nif_mac_lane.enable_reg.xaui_rx_lane_en, fld_val, reg_val, 20, exit);
  }

  if (tx_enable)
  {
    SOC_PB_FLD_TO_REG(regs->nif_mac_lane.enable_reg.xaui_tx_lane_en, fld_val, reg_val, 30, exit);
  }

  SOC_PB_REG_ISET(regs->nif_mac_lane.enable_reg, reg_val, mal_id, 40, exit);
  /* Enable/disable SPAUI } */

  if (info->enable)
  {
    if (rx_enable)
    {
      SOC_PB_REG_IGET(regs->nif_mac_lane.spaui_extensions1_reg, reg_val, mal_id, 50, exit);

      fld_val = info->preamble.enable ? info->preamble.size : 0x0;
      SOC_PB_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions1_reg.rx_preamble_size, fld_val, reg_val, 60, exit);

      fld_val = ((info->preamble.skip_SOP) || (!info->preamble.enable)) ? 0x0 : 0x1;
      SOC_PB_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions1_reg.rx_preamble_sop, fld_val, reg_val, 70, exit);

      fld_val = info->ch_conf.enable ? info->ch_conf.bct_size : 0x0;
      SOC_PB_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions1_reg.rx_bct_size, fld_val, reg_val, 72, exit);

      fld_val = info->ch_conf.enable ? info->ch_conf.bct_channel_byte_ndx : 0x0;
      SOC_PB_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions1_reg.rx_bct_ch_position, fld_val, reg_val, 80, exit);

      if (info->crc_mode == SOC_PB_NIF_CRC_MODE_NONE)
      {
        fld_val = 0x0;
        SOC_PB_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions1_reg.rx_crc_check_en,     fld_val, reg_val, 82, exit);
        SOC_PB_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions1_reg.rx_crc_terminate_en, fld_val, reg_val, 84, exit);
      }
      else
      {
        fld_val = 0x1;
        SOC_PB_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions1_reg.rx_crc_check_en,     fld_val, reg_val, 86, exit);
        SOC_PB_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions1_reg.rx_crc_terminate_en, fld_val, reg_val, 88, exit);

        fld_val = (info->crc_mode == SOC_PB_NIF_CRC_MODE_32)?0x0:0x1;
        SOC_PB_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions1_reg.rx_crc24_mode, fld_val, reg_val, 90, exit);
      }

      SOC_PB_REG_ISET(regs->nif_mac_lane.spaui_extensions1_reg, reg_val, mal_id, 100, exit);
    }

    if (tx_enable)
    {
      SOC_PB_REG_IGET(regs->nif_mac_lane.spaui_extensions2_reg, reg_val, mal_id, 110, exit);

      fld_val = info->preamble.enable ? info->preamble.size : 0x0;
      SOC_PB_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions2_reg.tx_preamble_size, fld_val, reg_val, 120, exit);

      fld_val = ((info->preamble.skip_SOP) || (!info->preamble.enable)) ? 0x0 : 0x1;
      SOC_PB_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions2_reg.tx_preamble_sop, fld_val, reg_val, 130, exit);

      fld_val = info->ch_conf.enable ? info->ch_conf.bct_size : 0x0;
      SOC_PB_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions2_reg.tx_bct_size, fld_val, reg_val, 140, exit);

      fld_val = info->ch_conf.enable ? info->ch_conf.bct_channel_byte_ndx : 0x0;
      SOC_PB_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions2_reg.tx_bct_ch_position, fld_val, reg_val, 150, exit);

      if (info->crc_mode == SOC_PB_NIF_CRC_MODE_NONE)
      {
        fld_val = 0x0;
        SOC_PB_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions2_reg.tx_crc_insert_en, fld_val, reg_val, 154, exit);
      }
      else
      {
        fld_val = 0x1;
        SOC_PB_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions2_reg.tx_crc_insert_en, fld_val, reg_val, 156, exit);

        fld_val = (info->crc_mode == SOC_PB_NIF_CRC_MODE_32) ? 0x0 : 0x1;
        SOC_PB_FLD_TO_REG(regs->nif_mac_lane.spaui_extensions2_reg.tx_crc24_mode, fld_val, reg_val, 160, exit);
      }

      SOC_PB_REG_ISET(regs->nif_mac_lane.spaui_extensions2_reg, reg_val, mal_id, 180, exit);
      
      fld_val = (info->ch_conf.enable && info->ch_conf.is_burst_interleaving) ? 0x0 : 0x1;
      SOC_PB_FLD_SET(regs->egq.egress_interface_no_fragmentation_mode_configuration_reg.nif_no_frag[mal_id], fld_val, 185, exit);
      SOC_PB_FLD_SET(regs->epni.egress_interface_no_fragmentation_mode_configuration_reg.nif_no_frag[mal_id], fld_val, 187, exit);

      if (info->ipg.enable == TRUE)
      {
        fld_val = (info->ipg.dic_mode == SOC_PB_NIF_IPG_DIC_MODE_AVERAGE)?0x1:0x0;
        SOC_PB_FLD_ISET(regs->nif_mac_lane.xaui_config_reg.ipg_dic_enable, fld_val, mal_id, 190, exit);

        fld_val = info->ipg.size;
        SOC_PB_FLD_ISET(regs->nif_mac_lane.xaui_config_reg.ipg_dic_threshold, fld_val, mal_id, 200, exit);
      }
    }

    if (info->link_partner_double_size_bus)
    {
      /* S-On-Even/Odd is relevant to ||A|| character, so A-on-even must be set for both */
      fld_val = ((info->is_double_size_sop_even_only) || (info->is_double_size_sop_odd_only))?0x1:0x0;
      SOC_PB_FLD_ISET(regs->nif_mac_lane.spaui_extensions2_reg.tx_a_on_even_only, fld_val, mal_id, 202, exit);

      fld_val = (info->is_double_size_sop_even_only) ? 0x1 : 0x0;
      SOC_PB_FLD_ISET(regs->nif_mac_lane.spaui_extensions2_reg.tx_sop_on_even_only, fld_val, mal_id, 204, exit);

      fld_val = (info->is_double_size_sop_odd_only) ? 0x1 : 0x0;
      SOC_PB_FLD_ISET(regs->nif_mac_lane.spaui_extensions1_reg.tx_sop_on_odd_only, fld_val, mal_id, 206, exit);
    }

    fld_val = soc_pb_nif_fault2intern(info->fault_response.local);
    SOC_PB_FLD_ISET(regs->nif_mac_lane.spaui_extensions1_reg.lfs_response_to_lf, fld_val, mal_id, 210, exit);

    fld_val = soc_pb_nif_fault2intern(info->fault_response.remote);
    SOC_PB_FLD_ISET(regs->nif_mac_lane.spaui_extensions1_reg.lfs_response_to_rf, fld_val, mal_id, 212, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_spaui_conf_set_unsafe()", nif_ndx, 0);
}

uint32
  soc_pb_nif_spaui_conf_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_SPAUI_INFO              *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SPAUI_CONF_SET_VERIFY);

  res = soc_pb_nif_spaui_conf_ndx_verify(unit, nif_ndx, direction_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_SPAUI_INFO, info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_spaui_conf_verify()", nif_ndx, direction_ndx);
}

uint32
  soc_pb_nif_spaui_conf_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SPAUI_CONF_GET_VERIFY);

  res = soc_pb_nif_id_verify(nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(direction_ndx, SOC_PB_CONNECTION_DIRECTION_NDX_MAX, SOC_PETRA_CONNECTION_DIRECTION_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_spaui_conf_ndx_verify()", nif_ndx, direction_ndx);
}

/*********************************************************************
 *     Sets SPAUI configuration - configure SPAUI extensions
 *     for XAUI interface.
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_spaui_conf_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_SPAUI_INFO              *info_rx,
    SOC_SAND_OUT SOC_PB_NIF_SPAUI_INFO              *info_tx
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_val = 0,
    reg_val = 0;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_NIF_SPAUI_INFO
    default_conf;
  uint8
    is_same;
  uint32
    nif_id,
    mal_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SPAUI_CONF_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info_rx);
  SOC_SAND_CHECK_NULL_INPUT(info_tx);
  SOC_PB_NIF_SPAUI_INFO_clear(info_rx);
  SOC_PB_NIF_SPAUI_INFO_clear(info_tx);

  SOC_PB_NIF_SPAUI_INFO_clear(&default_conf);

  regs = soc_petra_regs();

  /*
   *    Indexing
   */
  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);

  /* Enable/disable SPAUI { */
  SOC_PB_REG_IGET(regs->nif_mac_lane.enable_reg, reg_val, mal_id, 10, exit);

  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.enable_reg.xaui_rx_lane_en, fld_val, reg_val, 20, exit);
  info_rx->enable = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.enable_reg.xaui_tx_lane_en, fld_val, reg_val, 30, exit);
  info_tx->enable = SOC_SAND_NUM2BOOL(fld_val);
  /* Enable/disable SPAUI } */

  /************************************************************************/
  /* RX                                                                   */
  /************************************************************************/
  SOC_PB_REG_IGET(regs->nif_mac_lane.spaui_extensions1_reg, reg_val, mal_id, 50, exit);

  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions1_reg.rx_preamble_size, fld_val, reg_val, 60, exit);
  info_rx->preamble.size = fld_val;

  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions1_reg.rx_preamble_sop, fld_val, reg_val, 70, exit);
  info_rx->preamble.skip_SOP = SOC_SAND_NUM2BOOL_INVERSE(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions1_reg.rx_bct_size, fld_val, reg_val, 72, exit);
  info_rx->ch_conf.bct_size = fld_val;

  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions1_reg.rx_bct_ch_position, fld_val, reg_val, 80, exit);
  info_rx->ch_conf.bct_channel_byte_ndx = fld_val;

  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions1_reg.rx_crc_check_en,     fld_val, reg_val, 82, exit);
  if (fld_val == 0x0)
  {
    info_rx->crc_mode = SOC_PB_NIF_CRC_MODE_NONE;
  }
  else
  {
    SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions1_reg.rx_crc24_mode, fld_val, reg_val, 90, exit);
    info_rx->crc_mode =(fld_val == 0x0)? SOC_PB_NIF_CRC_MODE_32:SOC_PB_NIF_CRC_MODE_24;
  }

  SOC_PETRA_COMP(&(default_conf.preamble), &(info_rx->preamble), SOC_PB_NIF_PREAMBLE_COMPRESS, 1, is_same);
  info_rx->preamble.enable = SOC_SAND_NUM2BOOL_INVERSE(is_same);

  SOC_PETRA_COMP(&(default_conf.ch_conf), &(info_rx->ch_conf), SOC_PB_NIF_CHANNELIZED, 1, is_same);
  info_rx->ch_conf.enable = SOC_SAND_NUM2BOOL_INVERSE(is_same);

  /************************************************************************/
  /* TX                                                                   */
  /************************************************************************/
  
  SOC_PB_REG_IGET(regs->nif_mac_lane.spaui_extensions2_reg, reg_val, mal_id, 110, exit);

  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions2_reg.tx_preamble_size, fld_val, reg_val, 120, exit);
  info_tx->preamble.size = fld_val;

  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions2_reg.tx_preamble_sop, fld_val, reg_val, 130, exit);
  info_tx->preamble.skip_SOP = SOC_SAND_NUM2BOOL_INVERSE(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions2_reg.tx_bct_size, fld_val, reg_val, 140, exit);
  info_tx->ch_conf.bct_size = fld_val;

  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions2_reg.tx_bct_ch_position, fld_val, reg_val, 150, exit);
  info_tx->ch_conf.bct_channel_byte_ndx = fld_val;

  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions2_reg.tx_crc_insert_en,   fld_val, reg_val, 152, exit);
  if (fld_val == 0x0)
  {
    info_tx->crc_mode = SOC_PB_NIF_CRC_MODE_NONE;
  }
  else
  {
    SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions2_reg.tx_crc24_mode, fld_val, reg_val, 154, exit);
    info_tx->crc_mode =(fld_val == 0x0)? SOC_PB_NIF_CRC_MODE_32:SOC_PB_NIF_CRC_MODE_24;
  }

  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.spaui_extensions2_reg.tx_sop_on_even_only, fld_val, reg_val, 164, exit);
  info_rx->is_double_size_sop_even_only = info_tx->is_double_size_sop_even_only = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_IGET(regs->nif_mac_lane.spaui_extensions1_reg.tx_sop_on_odd_only, fld_val, mal_id, 170, exit);
  info_rx->is_double_size_sop_odd_only = info_tx->is_double_size_sop_odd_only = SOC_SAND_NUM2BOOL(fld_val);

  info_rx->link_partner_double_size_bus = info_tx->link_partner_double_size_bus =
    SOC_SAND_NUM2BOOL(info_tx->is_double_size_sop_even_only || info_tx->is_double_size_sop_odd_only);

  SOC_PB_FLD_GET(regs->egq.egress_interface_no_fragmentation_mode_configuration_reg.nif_no_frag[mal_id], fld_val, 185, exit);
  info_rx->ch_conf.is_burst_interleaving = info_tx->ch_conf.is_burst_interleaving = SOC_SAND_NUM2BOOL_INVERSE(fld_val);
 
  /*
   *    IPG
   */
  SOC_PB_FLD_IGET(regs->nif_mac_lane.xaui_config_reg.ipg_dic_enable, fld_val, mal_id, 190, exit);
  info_rx->ipg.dic_mode = info_tx->ipg.dic_mode = (fld_val == 0x1)?SOC_PB_NIF_IPG_DIC_MODE_AVERAGE:SOC_PB_NIF_IPG_DIC_MODE_MINIMUM;

  SOC_PB_FLD_IGET(regs->nif_mac_lane.xaui_config_reg.ipg_dic_threshold, fld_val, mal_id, 200, exit);
  info_rx->ipg.size = info_tx->ipg.size = fld_val;
  
  SOC_PB_FLD_IGET(regs->nif_mac_lane.spaui_extensions1_reg.lfs_response_to_lf, fld_val, mal_id, 210, exit);
  info_rx->fault_response.local = info_tx->fault_response.local = soc_pb_nif_intern2fault(fld_val);

  SOC_PB_FLD_IGET(regs->nif_mac_lane.spaui_extensions1_reg.lfs_response_to_rf, fld_val, mal_id, 212, exit);
  info_rx->fault_response.remote = info_tx->fault_response.remote = soc_pb_nif_intern2fault(fld_val);

  SOC_PETRA_COMP(&(default_conf.preamble), &(info_tx->preamble), SOC_PB_NIF_PREAMBLE_COMPRESS, 1, is_same);
  info_tx->preamble.enable = SOC_SAND_NUM2BOOL_INVERSE(is_same);

  SOC_PETRA_COMP(&(default_conf.ch_conf), &(info_tx->ch_conf), SOC_PB_NIF_CHANNELIZED, 1, is_same);
  info_tx->ch_conf.enable = SOC_SAND_NUM2BOOL_INVERSE(is_same);

  SOC_PETRA_COMP(&(default_conf.ipg), &(info_tx->ipg), SOC_PB_NIF_IPG_COMPRESS, 1, is_same);
  info_rx->ipg.enable = info_tx->ipg.enable = SOC_SAND_NUM2BOOL_INVERSE(is_same);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_spaui_conf_get_unsafe()", nif_ndx, 0);
}

/*
 *  Enable/Disable SGMII in the specified direction
 */
uint32
  soc_pb_nif_gmii_enable_state_set(
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
    nif_id,
    mal_id = 0,
    lane_id = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_GMII_ENABLE_STATE_SET);

  regs = soc_petra_regs();

  rx_enable = SOC_PETRA_IS_DIRECTION_RX(direction_ndx);
  tx_enable = SOC_PETRA_IS_DIRECTION_TX(direction_ndx);

  nif_id  = soc_pb_nif2intern_id(nif_ndx);
  mal_id  = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  lane_id = SOC_PB_NIF_GMII_LANE_INTERN(nif_id);

  val = SOC_SAND_BOOL2NUM(is_enabled);

  SOC_PB_REG_IGET(regs->nif_mac_lane.enable_reg, reg_val, mal_id, 10, exit);

  if (rx_enable)
  {
    /* RMW the specified lane of GMII enable field - rx */
    SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.enable_reg.gmii_rx_lane_en, fld_val, reg_val, 20, exit);

    SOC_SAND_SET_BIT(fld_val, val, lane_id);

    SOC_PB_FLD_TO_REG(regs->nif_mac_lane.enable_reg.gmii_rx_lane_en, fld_val, reg_val, 30, exit);
  }

  if (tx_enable)
  {
    /* RMW the specified lane of GMII enable field - tx */
    SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.enable_reg.gmii_tx_lane_en, fld_val, reg_val, 40, exit);

    SOC_SAND_SET_BIT(fld_val, val, lane_id);

    SOC_PB_FLD_TO_REG(regs->nif_mac_lane.enable_reg.gmii_tx_lane_en, fld_val, reg_val, 50, exit);
  }

  SOC_PB_REG_ISET(regs->nif_mac_lane.enable_reg, reg_val, mal_id, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_nif_gmii_enable_state_set()",nif_ndx,0);
}

uint32
  soc_pb_nif_gmii_enable_state_get(
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
    nif_id,
    mal_id = 0,
    lane_id = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_GMII_ENABLE_STATE_GET);

  regs = soc_petra_regs();

  SOC_SAND_CHECK_NULL_INPUT(is_enabled_rx);
  SOC_SAND_CHECK_NULL_INPUT(is_enabled_tx);

  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  lane_id= SOC_PB_NIF_GMII_LANE_INTERN(nif_id);

  SOC_PB_REG_IGET(regs->nif_mac_lane.enable_reg, reg_val, mal_id, 10, exit);

  /* Read the specified lane of GMII enable field - rx */
  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.enable_reg.gmii_rx_lane_en, fld_val, reg_val, 20, exit);
  val = SOC_SAND_GET_BIT(fld_val, lane_id);
  *is_enabled_rx = SOC_SAND_NUM2BOOL(val);

  /* Read the specified lane of GMII enable field - tx */
  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.enable_reg.gmii_tx_lane_en, fld_val, reg_val, 40, exit);
  val = SOC_SAND_GET_BIT(fld_val, lane_id);
  *is_enabled_tx = SOC_SAND_NUM2BOOL(val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_nif_gmii_enable_state_get()",nif_ndx,0);
}

/*********************************************************************
 *     Sets SGMII/1000BASE-X interface configuration
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_gmii_conf_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_GMII_INFO               *info
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  uint32
    mal_id = 0,
    nif_id = 0,
    nif_id_inner;
  SOC_PETRA_CONNECTION_DIRECTION
    enable_direction_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_GMII_CONF_SET_UNSAFE);

  regs = soc_petra_regs();

  /*
   *    Indexing
   */
  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  nif_id_inner = nif_id % SOC_PB_MAX_NIFS_PER_MAL;

  /* Disable SGMII lane during configuration change { */
  res = soc_pb_nif_gmii_enable_state_set(
          unit,
          nif_ndx,
          SOC_PETRA_CONNECTION_DIRECTION_BOTH,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if ((info->enable_rx) || (info->enable_tx))
  {
    /* Set other fields if enabled */

    /* External PHY */
    fld_val = (info->mode == SOC_PB_NIF_1GE_MODE_SGMII)?0x1:0x0;
    SOC_PB_FLD_ISET(regs->nif_mac_lane.auto_neg_config_reg[nif_id_inner].an_sgmii_phy_mode, fld_val, mal_id, 14, exit);
    
    /* TX-Config */
    /*   15  14  13  12  11  10  9   8   7   6   5   4   3   2   1   0
     * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
     * |NP |ACK|RF2|RF1| - | - | - |PS2|PS1|HD |FD | - | - | - | - | - |
     * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
     */
    fld_val = (info->mode == SOC_PB_NIF_1GE_MODE_SGMII)?0x1:0x1a0;
    SOC_PB_FLD_ISET(regs->nif_mac_lane.auto_neg_config_reg[nif_id_inner].an_tx_cfg_reg, fld_val, mal_id, 14, exit);

    res = soc_pb_nif_gmii_rate_set_unsafe(
            unit,
            nif_ndx,
            info->rate
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    /* Enable SGMII if requested { */
    if ((info->enable_rx) && (info->enable_tx))
    {
      enable_direction_ndx = SOC_PETRA_CONNECTION_DIRECTION_BOTH;
    }
    else
    {
      enable_direction_ndx = (info->enable_rx)?SOC_PETRA_CONNECTION_DIRECTION_RX : SOC_PETRA_CONNECTION_DIRECTION_TX;
    }
    res = soc_pb_nif_gmii_enable_state_set(
            unit,
            nif_ndx,
            enable_direction_ndx,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  /*
   *    EGQ
   */
  SOC_PB_FLD_GET(regs->egq.spaui_sel_reg.spaui_sel, fld_val,160 , exit);
  SOC_SAND_SET_BIT(fld_val, 0x0, mal_id);
  SOC_PB_FLD_SET(regs->egq.spaui_sel_reg.spaui_sel, fld_val,162 , exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_nif_gmii_conf_set_unsafe()",nif_id,0);
}

uint32
  soc_pb_nif_gmii_conf_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_GMII_INFO              *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_GMII_CONF_SET_VERIFY);

  res = soc_pb_nif_id_verify(nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_GMII_INFO, info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_gmii_conf_verify()", 0, 0);
}

/*********************************************************************
 *     Sets SGMII/1000BASE-X interface configuration
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_gmii_conf_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_GMII_INFO              *info
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  uint32
    mal_id = 0,
    nif_id = 0,
    nif_id_inner;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_GMII_CONF_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_NIF_GMII_INFO_clear(info);

  regs = soc_petra_regs();

  /*
   *    Indexing
   */
  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  nif_id_inner = nif_id % SOC_PB_MAX_NIFS_PER_MAL;

  res = soc_pb_nif_gmii_enable_state_get(
          unit,
          nif_ndx,
          &(info->enable_rx),
          &(info->enable_tx)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* External PHY */
  SOC_PB_FLD_IGET(regs->nif_mac_lane.auto_neg_config_reg[nif_id_inner].an_sgmii_phy_mode, fld_val, mal_id, 14, exit);
  info->mode = (fld_val == 0x1)?SOC_PB_NIF_1GE_MODE_SGMII:SOC_PB_NIF_1GE_MODE_1000BASE_X;
    
  res = soc_pb_nif_gmii_rate_get_unsafe(
          unit,
          nif_ndx,
          &info->rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_gmii_conf_get_unsafe()", nif_ndx, 0);
}

/*********************************************************************
 *     Gets SGMII/1000BASE-X interface diagnostics status
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_gmii_status_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_GMII_STAT               *status
  )
{
  uint32
    fld_val,
    reg_val,
    res = SOC_SAND_OK;
   uint32
    nif_id,
    mal_id = 0,
    nif_id_inner = 0;
  uint8
    is_autoneg;
  SOC_PB_NIF_GMII_INFO
    gmii_info;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_GMII_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(status);
  SOC_PB_NIF_GMII_STAT_clear(status);

  SOC_PB_NIF_GMII_INFO_clear(&gmii_info);

  regs = soc_petra_regs();

  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  nif_id_inner = nif_id % SOC_PB_MAX_NIFS_PER_MAL;

  res = soc_pb_nif_gmii_conf_get_unsafe(
          unit,
          nif_ndx,
          &gmii_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_PB_REG_IGET(regs->nif_mac_lane.auto_neg_status_reg[nif_id_inner], reg_val, mal_id, 10, exit);

  SOC_PB_FLD_IGET(regs->nif_mac_lane.auto_neg_config_reg[nif_id_inner].an_enable, fld_val, mal_id, 15, exit);
  is_autoneg = SOC_SAND_NUM2BOOL(fld_val);

  if (is_autoneg)
  {
    SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.auto_neg_status_reg[nif_id_inner].an_complete, fld_val, reg_val, 20, exit);
    status->autoneg_not_complete = SOC_SAND_BOOL2NUM_INVERSE(fld_val);

    SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.auto_neg_status_reg[nif_id_inner].an_error, fld_val, reg_val, 25, exit);
    status->autoneg_not_complete = SOC_SAND_BOOL2NUM(fld_val);
  }

  if (gmii_info.mode == SOC_PB_NIF_1GE_MODE_SGMII)
  {
    SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.auto_neg_status_reg[nif_id_inner].an_rx_cfg_sgmii_link_up, fld_val, reg_val, 40, exit);

    if(fld_val == 0x0)
    {
      status->remote_fault = TRUE;
      status->fault_type = SOC_PB_NIF_GMII_RF_LNK_FAIL;
    }
    else
    {
      status->remote_fault = FALSE;
      status->fault_type = SOC_PB_NIF_GMII_RF_OK;
    }
  }
  else
  {
    /* 1000base-x */
    SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.auto_neg_status_reg[nif_id_inner].an_remote_fault, fld_val, reg_val, 30, exit);
    status->remote_fault = SOC_SAND_NUM2BOOL(fld_val);

    if (status->remote_fault)
    {
      SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.auto_neg_status_reg[nif_id_inner].an_rx_cfg_1000b_x_rf_type, fld_val, reg_val, 40, exit);

      switch (fld_val)
      {
      case 0x0:
        status->fault_type = SOC_PB_NIF_GMII_RF_OK;
          break;
      case 0x1:
        status->fault_type = SOC_PB_NIF_GMII_RF_OFFLINE_REQ;
        break;
      case 0x2:
        status->fault_type = SOC_PB_NIF_GMII_RF_LNK_FAIL;
        break;
      case 0x3:
        status->fault_type = SOC_PB_NIF_GMII_RF_AUTONEG_ERR;
        break;
      default:
        status->fault_type = SOC_PB_NIF_NOF_GMII_RFS;
      }
    }
    else
    {
      status->fault_type = SOC_PB_NIF_GMII_RF_OK;
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_gmii_status_get_unsafe()", nif_ndx, 0);
}

uint32
  soc_pb_nif_gmii_status_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_GMII_STATUS_GET_VERIFY);

  res = soc_pb_nif_id_verify(nif_ndx); SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_gmii_status_ndx_verify()", nif_ndx, 0);
}

/*
 *    Input: SGMII rate enumerator
 *  Output: the internal (field value) representation)
 */
STATIC uint32
  soc_pb_nif_gmii_rate2intern(
    SOC_SAND_IN  SOC_PB_NIF_GMII_RATE              rate
  )
{
  uint32
    fld_val;

  switch(rate) {
  case SOC_PB_NIF_GMII_RATE_10Mbps:
    fld_val = 0x1;
      break;
  case SOC_PB_NIF_GMII_RATE_100Mbps:
    fld_val = 0x2;
      break;
  case SOC_PB_NIF_GMII_RATE_1000Mbps:
    fld_val = 0x4;
      break;
  case SOC_PB_NIF_GMII_RATE_AUTONEG:
  default:
    fld_val = 0x7;
  }

  return fld_val;
}

STATIC SOC_PB_NIF_GMII_RATE
  soc_pb_nif_gmii_intern2rate(
    SOC_SAND_IN  uint32    intern_fld_val
  )
{
  SOC_PB_NIF_GMII_RATE
    rate;

  switch(intern_fld_val) {
  case 0x1:
    rate = SOC_PB_NIF_GMII_RATE_10Mbps;
      break;
  case 0x2:
    rate = SOC_PB_NIF_GMII_RATE_100Mbps;
      break;
  case 0x4:
    rate = SOC_PB_NIF_GMII_RATE_1000Mbps;
      break;
  case 0x7:
  default:
    rate = SOC_PB_NIF_NOF_GMII_RATES;
  }

  return rate;
}

/*********************************************************************
 *     Sets SGMII interface link-rate.
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_gmii_rate_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_GMII_RATE               rate
  )
{
  uint32
    fld_val,
    reg_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  uint8
    is_enabled_tx,
    is_enabled_rx;
  uint32
    mal_id = 0,
    nif_port_id = 0,
    port_id_inner;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_GMII_RATE_SET_UNSAFE);

  regs = soc_petra_regs();

  /*
   *    Indexing
   */
  if (SOC_PB_NIF_IS_TYPE_ID(SGMII, nif_ndx))
  {
    nif_port_id = SOC_PB_SGMII2GLBL_ID(SOC_PB_NIF_ID_OFFSET(SGMII, nif_ndx));
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(QSGMII, nif_ndx))
  {
    nif_port_id = SOC_PB_NIF_ID_OFFSET(QSGMII, nif_ndx);
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_UNKNOWN_NIF_TYPE_ERR, 8, exit);
  }
  
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_port_id);
  port_id_inner = SOC_PB_NIF_GMII_LANE_INTERN(nif_port_id);

  /************************************************************************/
  /* Save enable state, disable during configuraton                       */
  /************************************************************************/
  res = soc_pb_nif_gmii_enable_state_get(
          unit,
          nif_ndx,
          &is_enabled_rx,
          &is_enabled_tx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Disable SGMII lane during configuration change { */
  res = soc_pb_nif_gmii_enable_state_set(
          unit,
          nif_ndx,
          SOC_PETRA_CONNECTION_DIRECTION_BOTH,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  /************************************************************************/
  /* Set the requested configuration (Autoneg/explicit rate)              */
  /************************************************************************/
  /*
   *    Auto-negotiation
   */
  SOC_PB_REG_IGET(regs->nif_mac_lane.auto_neg_config_reg[port_id_inner], reg_val, mal_id, 16, exit);

  fld_val = SOC_SAND_BOOL2NUM(rate == SOC_PB_NIF_GMII_RATE_AUTONEG);
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.auto_neg_config_reg[port_id_inner].an_enable, fld_val, reg_val, 20, exit);
   
#if SOC_PB_NIF_GMII_XMIT_OVRD_EN
  /* XMIT-Override mode, debug */
  /* LinkSpeedOvrdEn  - always set*/
  fld_val = 0x1;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.auto_neg_config_reg[port_id_inner].an_link_speed_ovrd, fld_val, reg_val, 40, exit);

  /* xmit override */
  fld_val = (info->rate == SOC_PB_NIF_GMII_RATE_AUTONEG)?0x7:0x4;
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.auto_neg_config_reg[port_id_inner].an_xmit_ovrd, fld_val, reg_val, 40, exit);
#endif
    
 /*
  *    Explicit rate (or auto)
  */
  fld_val = soc_pb_nif_gmii_rate2intern(rate);
  SOC_PB_FLD_TO_REG(regs->nif_mac_lane.auto_neg_config_reg[port_id_inner].an_link_speed_ovrd, fld_val, reg_val, 40, exit);
  
  SOC_PB_REG_ISET(regs->nif_mac_lane.auto_neg_config_reg[port_id_inner], reg_val, mal_id, 60, exit);

  /************************************************************************/
  /* Revert to previous enable-state                                      */
  /************************************************************************/
  if (is_enabled_rx == is_enabled_tx)
  {
    /* Revert SGMII enable field - both */
    res = soc_pb_nif_gmii_enable_state_set(
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
    res = soc_pb_nif_gmii_enable_state_set(
            unit,
            nif_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_RX,
            is_enabled_rx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    /* Revert SGMII enable field - tx */
    res = soc_pb_nif_gmii_enable_state_set(
            unit,
            nif_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_TX,
            is_enabled_tx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_gmii_rate_set_unsafe()", nif_ndx, 0);
}

uint32
  soc_pb_nif_gmii_rate_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_GMII_RATE              rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_GMII_RATE_SET_VERIFY);

  res = soc_pb_nif_gmii_rate_ndx_verify(unit, nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(rate, SOC_PB_NIF_RATE_MAX, SOC_PB_RATE_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_gmii_rate_verify()", 0, 0);
}

uint32
  soc_pb_nif_gmii_rate_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_GMII_RATE_GET_VERIFY);

  res = soc_pb_nif_id_verify(nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_gmii_rate_ndx_verify()", nif_ndx, 0);
}

/*********************************************************************
 *     Sets SGMII interface link-rate.
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_gmii_rate_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_GMII_RATE               *rate
  )
{
  uint32
    fld_val,
    reg_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  uint32
    mal_id = 0,
    nif_port_id = 0,
    port_id_inner;
  SOC_PB_NIF_GMII_RATE
    gmii_rate;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_GMII_RATE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(rate);

  regs = soc_petra_regs();

  /*
   *    Indexing
   */
  if (SOC_PB_NIF_IS_TYPE_ID(SGMII, nif_ndx))
  {
    nif_port_id = SOC_PB_SGMII2GLBL_ID(SOC_PB_NIF_ID_OFFSET(SGMII, nif_ndx));
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(QSGMII, nif_ndx))
  {
    nif_port_id = SOC_PB_NIF_ID_OFFSET(QSGMII, nif_ndx);
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_UNKNOWN_NIF_TYPE_ERR, 8, exit);
  }
  
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_port_id);
  port_id_inner = SOC_PB_NIF_GMII_LANE_INTERN(nif_port_id);

  /************************************************************************/
  /* Set the requested configuration (Autoneg/explicit gmii_rate)              */
  /************************************************************************/
  /*
   *    Auto-negotiation
   */
  SOC_PB_REG_IGET(regs->nif_mac_lane.auto_neg_config_reg[port_id_inner], reg_val, mal_id, 16, exit);
  SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.auto_neg_config_reg[port_id_inner].an_enable, fld_val, reg_val, 20, exit);
  if (fld_val == 0x1)
  {
    gmii_rate = SOC_PB_NIF_GMII_RATE_AUTONEG;
  }
  else
  {
   /*
    *    Explicit gmii_rate (or auto)
    */
    SOC_PB_REG_IGET(regs->nif_mac_lane.auto_neg_config_reg[port_id_inner], reg_val, mal_id, 60, exit);
    SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.auto_neg_config_reg[port_id_inner].an_link_speed_ovrd, fld_val, reg_val, 40, exit);
    gmii_rate = soc_pb_nif_gmii_intern2rate(fld_val);
  }

  *rate = gmii_rate;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_gmii_rate_get_unsafe()", nif_ndx, 0);
}

/*
 *    Internal functions per direction (RX/TX, not Both)
 *  {
 */
/************************************************************************/
/* Interlaken Reset:                                                    */
/*   Resets the ILKN FIFO, and if requested, ILKN PORT                  */
/*   Note: the ilkn_id is 0/1, and not the logical id (ILKN-A/B)        */
/************************************************************************/
uint32
  soc_pb_nif_ilkn_reset(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      ilkn_id,
    SOC_SAND_IN  uint8                      is_out_of_reset,
    SOC_SAND_IN  uint8                      is_fifo_only,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  uint32
    directn_max = SOC_PETRA_NIF_TO_REG_DIRECTION(direction_ndx),
    directn_min,
    directn;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ILKN_SET_UNSAFE);

  regs = soc_petra_regs();

  fld_val = SOC_SAND_BOOL2NUM(is_out_of_reset);

  directn_min = (direction_ndx == SOC_PETRA_CONNECTION_DIRECTION_BOTH)?SOC_PB_REG_NIF_DIRECTION_RX:directn_max;

  for (directn = directn_min; directn <= directn_max; directn++)
  {
    if (!is_fifo_only)
    {
      SOC_PB_FLD_SET(regs->nbi.ilknreset_reg.ilkn_port_rstn[ilkn_id][directn], fld_val, 2, exit);
    }

    SOC_PB_FLD_SET(regs->nbi.ilknreset_reg.ilkn_controller_rstn[ilkn_id][directn], fld_val, 4, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ilkn_reset()", ilkn_id, direction_ndx);
}

STATIC uint32
  soc_pb_nif_ilkn_over_malgb_clk_select(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      ilkn_id_clk_src,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  uint32
    directn = SOC_PETRA_NIF_TO_REG_DIRECTION(direction_ndx);
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ILKN_SET_UNSAFE);

  regs = soc_petra_regs();
  fld_val = (ilkn_id_clk_src == 0)?0x1:0x0;

  SOC_PB_FLD_SET(regs->nbi.enable_interlaken_reg.ilkn_1_internal_clock_sel[directn], fld_val, 2, exit);
  SOC_PB_FLD_SET(regs->nbi.enable_interlaken_reg.ilkn_1_external_clock_sel[directn], fld_val, 4, exit);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ilkn_over_malgb_clk_select()", ilkn_id_clk_src, direction_ndx);
}

uint32
  soc_pb_nif_ilkn_actual_nof_lanes_get(
    SOC_SAND_IN  uint32                      ilkn_id,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO               *info
  )
{
  uint32
    nof_lanes = info->nof_lanes;

  /*
   *    If ILKN-A with 4 lanes or ILKN-B with 8 lanes, treat as 4/9 lanes ILKN accordingly,
   *  mark the last lane as invalid
   */
  if ((ilkn_id == 0) && (nof_lanes == SOC_PB_NIF_ILKN_A_NOF_LANES_MIN))
  {
    nof_lanes = SOC_PB_NIF_ILKN_A_NOF_LANES_MIN + 1;
  }
  else if ((ilkn_id == 1) && (nof_lanes == SOC_PB_NIF_ILKN_B_NOF_LANES_MIN))
  {
    nof_lanes = SOC_PB_NIF_ILKN_B_NOF_LANES_MIN + 1;
  }

  return nof_lanes;
}

STATIC uint32
  soc_pb_nif_ilkn_modes_conf(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      ilkn_id,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO               *info,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_OUT uint32                      *actual_nof_lanes
  )
{
  uint32
    fld_val,
    burst_max_fld_val,
    metaframe_fld_val,
    last_lane_fld_val,
    has_bad_lane_fld_val = 0x0,
    bad_lane_fld_val = 0x0,
    res = SOC_SAND_OK;
  uint32
    nof_lanes = info->nof_lanes,
    directn = SOC_PETRA_NIF_TO_REG_DIRECTION(direction_ndx);
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ILKN_SET_UNSAFE);

  regs = soc_petra_regs();

  burst_max_fld_val = SOC_PB_NIF_ILKN_BURST_MAX_256B_VAL;
  metaframe_fld_val = (info->metaframe_sync_period == 0)?0x0:(info->metaframe_sync_period - 1);

  /*
   *    If ILKN-A with 4 lanes or ILKN-B with 8 lanes, treat as 4/9 lanes ILKN accordingly,
   *  mark the last lane as invalid
   */
  if ((ilkn_id == 0) && (nof_lanes == SOC_PB_NIF_ILKN_A_NOF_LANES_MIN))
  {
    has_bad_lane_fld_val = 0x1;
    bad_lane_fld_val = SOC_PB_NIF_ILKN_A_NOF_LANES_MIN;
    nof_lanes = SOC_PB_NIF_ILKN_A_NOF_LANES_MIN + 1;
  }
  else if ((ilkn_id == 1) && (nof_lanes == SOC_PB_NIF_ILKN_B_NOF_LANES_MIN))
  {
    has_bad_lane_fld_val = 0x1;
    bad_lane_fld_val = SOC_PB_NIF_ILKN_B_NOF_LANES_MIN;
    nof_lanes = SOC_PB_NIF_ILKN_B_NOF_LANES_MIN + 1;
  }
  else
  {
    has_bad_lane_fld_val = SOC_SAND_BOOL2NUM(info->is_invalid_lane);
    bad_lane_fld_val = info->invalid_lane_id;
  }
  last_lane_fld_val = nof_lanes - 1;

  if (direction_ndx == SOC_PETRA_CONNECTION_DIRECTION_RX)
  {
    /* Full/Burst interleaving mode - RX only */
    fld_val = SOC_SAND_BOOL2NUM_INVERSE(info->is_burst_interleaving);
    SOC_PB_FLD_SET(regs->nbi.rx_ilkn_control_reg[ilkn_id].rx_full_packet_mode, fld_val, 2, exit);

    SOC_PB_FLD_SET(regs->nbi.rx_ilkn_control_reg[ilkn_id].rx_burstmax, burst_max_fld_val, 4, exit);
    SOC_PB_FLD_SET(regs->nbi.rx_ilkn_control_reg[ilkn_id].rx_mframe_len_minus1, metaframe_fld_val, 8, exit);
  }
  else /* TX */
  {
    SOC_PB_FLD_SET(regs->nbi.tx_ilkn_control_reg_0[ilkn_id].tx_burstmax, burst_max_fld_val, 10, exit);
    /* Burst Short - TX only */
    SOC_PB_FLD_SET(regs->nbi.tx_ilkn_control_reg_0[ilkn_id].tx_burstshort, SOC_PB_NIF_ILKN_TX_BURST_SHORT_64B_VAL, 12, exit);
    SOC_PB_FLD_SET(regs->nbi.tx_ilkn_control_reg_0[ilkn_id].tx_mframe_len_minus1, metaframe_fld_val, 14, exit);
  }

  SOC_PB_FLD_SET(regs->nbi.ilkn_num_lanes_config_reg[ilkn_id].last_lane[directn], last_lane_fld_val, 20, exit);
  SOC_PB_FLD_SET(regs->nbi.ilkn_num_lanes_config_reg[ilkn_id].has_bad_lane[directn], has_bad_lane_fld_val, 22, exit);
  SOC_PB_FLD_SET(regs->nbi.ilkn_num_lanes_config_reg[ilkn_id].bad_lane[directn], bad_lane_fld_val, 24, exit);
 
  *actual_nof_lanes = nof_lanes;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ilkn_modes_conf()", ilkn_id, direction_ndx);
}

/*
 *  }
 * (Internal functions per direction (RX/TX, not Both))
 */

/*
 *    Set Interlaken clock configuration
 *  Must be done before SerDes initialization
 */
uint32
  soc_pb_nif_ilkn_clk_config(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO               *info
  )
{
  uint32
    ilkn_id = SOC_PB_NIF_ID_OFFSET(ILKN, ilkn_ndx);
  uint32
    fld_val,
    res = SOC_SAND_OK;
  uint32
    actual_nof_lanes,
    srd_base_lane,
    srd_last_lane,
    malg_id,
    mal_first,
    mal_last,
    mal_id,
    mal_inner_id,
    malg_first,
    malg_last,
    clk_src_ilkn_id;
  uint8
    rx_enable = FALSE,
    tx_enable = FALSE,
    is_ilkn0_over_malgb;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ILKN_CLK_CONFIG);

  SOC_SAND_CHECK_NULL_INPUT(info);
  
  rx_enable = SOC_PETRA_IS_DIRECTION_RX(direction_ndx);
  tx_enable = SOC_PETRA_IS_DIRECTION_TX(direction_ndx);

  regs = soc_petra_regs();

  actual_nof_lanes = soc_pb_nif_ilkn_actual_nof_lanes_get(ilkn_id, info);

  /*
   *    MALG
   */

  is_ilkn0_over_malgb = SOC_SAND_NUM2BOOL((ilkn_ndx == SOC_PB_NIF_ILKN_ID_A) && (SOC_PB_NIF_IS_ILKN0_OVER_MALGB(info->nof_lanes)));

  /*
   *  If this is ILKN-A, and it is above 12 lanes,
   *    select proper clocks for lanes 12-23
   */
  if (is_ilkn0_over_malgb)
  {
    clk_src_ilkn_id = 0;
  }
  else
  {
    clk_src_ilkn_id = 1;
  }

  if (rx_enable)
  {
    res = soc_pb_nif_ilkn_over_malgb_clk_select(unit, clk_src_ilkn_id, SOC_PETRA_CONNECTION_DIRECTION_RX);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  if (tx_enable)
  {
    res = soc_pb_nif_ilkn_over_malgb_clk_select(unit, clk_src_ilkn_id, SOC_PETRA_CONNECTION_DIRECTION_TX);
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
  }

  /*
   *    MALG
   */
  srd_base_lane = SOC_PB_NIF_ILKN_LANE_FIRST(ilkn_id);
  srd_last_lane = SOC_SAND_RNG_LAST(srd_base_lane, actual_nof_lanes);
  if ((ilkn_ndx == SOC_PB_NIF_ILKN_ID_A) && (srd_last_lane >= SOC_PB_NIF_ILKN_SRD_COMBO_A_LN_ID))
  {
    srd_last_lane += SOC_PETRA_SRD_NOF_LANES_PER_QRTT;
  }
  malg_first = ilkn_id;
  malg_last = (is_ilkn0_over_malgb)?1:malg_first;
  
  for (malg_id = malg_first; malg_id <= malg_last; malg_id++)
  {
    /*
     *    Build occupied quartets bitmap
     *  QSRD-3 is not effected (the bitmap is for QSRD 0 - 2 only).
     *  QSRD-2 in MALG-B can be used by the ELK, so mark only if needed
     */
    fld_val = 0x7;
    if ((malg_id == SOC_PB_NIF_MALG_ID_B) && (SOC_PETRA_SRD_LANE2QRTT_GLBL(srd_last_lane) < SOC_PB_NIF_ILKN_ELK_QSRD_OVERLAP_ID))
    {
      fld_val = 0x3;
    }
        
    SOC_PB_FLD_ISET(regs->nif.nif_config_reg.ilkn_mode, fld_val, malg_id, 70, exit);
  }

  /*
   *    MAL
   */
  srd_base_lane = SOC_PB_NIF_ILKN_LANE_FIRST(ilkn_id);
  srd_last_lane = SOC_SAND_RNG_LAST(srd_base_lane, actual_nof_lanes);
  if ((ilkn_ndx == SOC_PB_NIF_ILKN_ID_A) && (srd_last_lane >= SOC_PB_NIF_ILKN_SRD_COMBO_A_LN_ID))
  {
    srd_last_lane += SOC_PETRA_SRD_NOF_LANES_PER_QRTT;
  }

  mal_first = srd_base_lane/SOC_PB_SRD_LANES_PER_MAL;
  mal_last  = srd_last_lane/SOC_PB_SRD_LANES_PER_MAL;

  for (mal_id = mal_first; mal_id <= mal_last; mal_id++)
  {
    if (SOC_PETRA_SRD_IS_COMBO_0_LANE(mal_id*SOC_PB_SRD_LANES_PER_MAL))
    {
      /* Skip Combo-0 MALs */
      continue;
    }

    malg_id = SOC_PB_NIF_MAL2MALG_ID(mal_id);
    mal_inner_id = SOC_PB_NIF_MAL2INNER_ID(mal_id);
    fld_val = SOC_PB_NIF_IS_BASE_MAL(mal_id)?SOC_PB_NIF_MAL_CLK_SEL_VAL_ILKN:SOC_PB_NIF_MAL_CLK_SEL_VAL_PD;
    if (rx_enable)
    {
      SOC_PB_FLD_ISET(regs->nif.nif_clocks_reg[SOC_PB_REG_NIF_DIRECTION_RX].mal_clock_select[mal_inner_id], fld_val, malg_id, 80, exit);
    }
    if (tx_enable)
    {
      SOC_PB_FLD_ISET(regs->nif.nif_clocks_reg[SOC_PB_REG_NIF_DIRECTION_TX].mal_clock_select[mal_inner_id], fld_val, malg_id, 82, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ilkn_clk_config()", ilkn_id, 0);
}

/*********************************************************************
 *     Function description
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_ilkn_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO               *info
  )
{
  uint32
    mal_id,
    ilkn_id = SOC_PB_NIF_ID_OFFSET(ILKN, ilkn_ndx);
  uint32
    bit_i,
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_SRD_STAR_ID
    star_id;
  uint32
    nif_id,
    actual_nof_lanes = info->nof_lanes,
    srd_base_lane,
    srd_last_lane,
    srd_qrtt_id_prev = 0xFF,
    srd_qrtt_id = 0,
    srd_qrtt_id_glbl,
    srd_lane_id,
    srd_instance_id,
    srd_lane_id_global;
  uint8
    rx_enable = FALSE,
    tx_enable = FALSE,
    srd_qrtt_changed = FALSE;
  SOC_PETRA_SRD_QRTT_INFO
    srd_qrtt_info;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ILKN_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  
  regs = soc_petra_regs();
  soc_petra_PETRA_SRD_QRTT_INFO_clear(&srd_qrtt_info);
  
  rx_enable = SOC_PETRA_IS_DIRECTION_RX(direction_ndx);
  tx_enable = SOC_PETRA_IS_DIRECTION_TX(direction_ndx);

  res = soc_pb_nif_ilkn_clk_config(
          unit,
          ilkn_ndx,
          SOC_PETRA_CONNECTION_DIRECTION_BOTH,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  /*
   *    ILKN In-reset
   */
  
  res = soc_pb_nif_ilkn_reset(unit, ilkn_id, FALSE, FALSE, direction_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *    Set NBI ILKN mode
   */
  SOC_PB_FLD_SET(regs->nbi.enable_interlaken_reg.enable_ilkn[ilkn_id], 0x1, 16, exit);
  SOC_PB_FLD_SET(regs->nbi.enable_interlaken_reg.force_egq_segmentation, 0x0, 18, exit);

  /*
   *    Rx/Tx Modes - Burst, Meta-frame, invalid lane
   */
  if (rx_enable)
  {
    soc_pb_nif_ilkn_modes_conf(unit, ilkn_id, info, SOC_PETRA_CONNECTION_DIRECTION_RX, &actual_nof_lanes);
  }
  if (tx_enable)
  {
    soc_pb_nif_ilkn_modes_conf(unit, ilkn_id, info, SOC_PETRA_CONNECTION_DIRECTION_TX, &actual_nof_lanes);
  }

  /*
   *    Enable Statistics Interface
   */
  SOC_PB_FLD_SET(regs->nbi.statistics_reg.stat_ilkn_enable[ilkn_id], 0x1, 30, exit);
  
  /*
   *    SerDes
   */
  srd_base_lane = SOC_PB_NIF_ILKN_LANE_FIRST(ilkn_id);
  srd_last_lane = SOC_SAND_RNG_LAST(srd_base_lane, actual_nof_lanes);
  if ((ilkn_ndx == SOC_PB_NIF_ILKN_ID_A) && (srd_last_lane >= SOC_PB_NIF_ILKN_SRD_COMBO_A_LN_ID))
  {
    srd_last_lane += SOC_PETRA_SRD_NOF_LANES_PER_QRTT;
  }
  
  for (srd_lane_id_global = srd_base_lane; srd_lane_id_global <= srd_last_lane; srd_lane_id_global++)
  {
    if (SOC_PETRA_SRD_IS_COMBO_0_LANE(srd_lane_id_global))
    {
      /* Skip Combo-0 lanes */
      continue;
    }
    
    star_id = SOC_PETRA_SRD_LANE2STAR(srd_lane_id_global);
    srd_instance_id = SOC_PETRA_SRD_STAR2INSTANCE(star_id);
    srd_qrtt_id = SOC_PETRA_SRD_LANE2QRTT_INNER(srd_lane_id_global);
    srd_qrtt_id_glbl = SOC_PETRA_SRD_LANE2QRTT_GLBL(srd_lane_id_global);
    srd_lane_id = SOC_PETRA_SRD_LANE2INNER_ID(srd_lane_id_global);

    if (srd_qrtt_id != srd_qrtt_id_prev)
    {
      srd_qrtt_id_prev = srd_qrtt_id;
      srd_qrtt_changed = TRUE;
    }
    else
    {
      srd_qrtt_changed = FALSE;
    }

    if (srd_qrtt_changed)
    {
      /*
       * Per-quartet configurations
       */

      /* Use External data for ILKN (bypass 8/10) */
      SOC_PB_FLD_ISET(regs->serdes.srd_cmu_cfga_reg[srd_qrtt_id].ln_20b_mode, 0x1, srd_instance_id, 50, exit);

      res = soc_petra_srd_qrtt_sync_fifo_en(
              unit,
              srd_qrtt_id_glbl,
              FALSE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
    }

    /*
     *    Per-lane configurations
     */
    SOC_PB_FLD_ISET(regs->serdes.srd_ln_cfga_reg[srd_qrtt_id][srd_lane_id].ext_tx_data_ovrd_en, 0x1, srd_instance_id, 62, exit);
  }

  /*
   *    EGQ
   */
  nif_id = soc_pb_nif2intern_id(ilkn_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  SOC_PB_FLD_GET(regs->egq.spaui_sel_reg.spaui_sel, fld_val,100 , exit);
  SOC_SAND_SET_BIT(fld_val, 0x1, mal_id);
  SOC_PB_FLD_SET(regs->egq.spaui_sel_reg.spaui_sel, fld_val,110 , exit);

  SOC_PB_FLD_GET(regs->egq.nif_mal_enable_reg.nif_cancel_en, fld_val,120 , exit);
  SOC_SAND_SET_BIT(fld_val, 0x0, mal_id);
  SOC_PB_FLD_SET(regs->egq.nif_mal_enable_reg.nif_cancel_en, fld_val,122 , exit);

  fld_val = SOC_SAND_BOOL2NUM_INVERSE(info->is_burst_interleaving);
  SOC_PB_FLD_SET(regs->egq.egress_interface_no_fragmentation_mode_configuration_reg.nif_no_frag[mal_id], fld_val, 130, exit);
  SOC_PB_FLD_SET(regs->epni.egress_interface_no_fragmentation_mode_configuration_reg.nif_no_frag[mal_id], fld_val, 132, exit)
  SOC_PB_FLD_SET(regs->egq.nif_interlaken_mode_reg.cfg_ilaken[ilkn_id], fld_val, 140, exit);

  /* 
   * Enable mapping up to 80 TM-ports 
   */    
  bit_i = nif_id/16; /* Bit 0 correspond to ILKN-A, Bit 2 corresponds to ILKN-B */
  SOC_PB_FLD_GET(regs->ire.fap_port_configuration_reg.use_all_channels, fld_val, 40, exit);
  SOC_SAND_SET_BIT(fld_val, 0x1, bit_i);
  SOC_PB_FLD_SET(regs->ire.fap_port_configuration_reg.use_all_channels, fld_val, 42, exit);
 
  /*
   *    ILKN Out-Of-reset
   */
  res = soc_pb_nif_ilkn_reset(unit, ilkn_id, TRUE, FALSE, direction_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ilkn_set_unsafe()", ilkn_id, 0);
}

uint32
  soc_pb_nif_ilkn_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO               *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ILKN_SET_VERIFY);

  res = soc_pb_nif_ilkn_ndx_verify(unit, ilkn_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(direction_ndx, SOC_PB_CONNECTION_DIRECTION_NDX_MAX, SOC_PETRA_CONNECTION_DIRECTION_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_ILKN_INFO, info, 30, exit);

  if (info->is_invalid_lane)
  {
    if(
      ((ilkn_ndx == SOC_PB_NIF_ILKN_ID_A) && (info->nof_lanes == SOC_PB_NIF_ILKN_A_NOF_LANES_MIN)) ||
      ((ilkn_ndx == SOC_PB_NIF_ILKN_ID_B) && (info->nof_lanes == SOC_PB_NIF_ILKN_B_NOF_LANES_MIN))
    )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_ILKN_INVALID_LANE_FOR_MIN_SIZE_ERR, 40, exit);
    }

    if ((info->invalid_lane_id != 0) && !(info->is_invalid_lane))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_ILKN_INVALID_LANE_SPECIFIED_BUT_INACTIVE_ERR, 50, exit);
    }
  }

  if(ilkn_ndx == SOC_PB_NIF_ILKN_ID_A)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_lanes, SOC_PB_NIF_ILKN_A_NOF_LANES_MIN, SOC_PB_NIF_ILKN_A_NOF_LANES_MAX, SOC_PB_ILKN_NOF_LANES_OUT_OF_RANGE_ERR, 60, exit);
  }

  if(ilkn_ndx == SOC_PB_NIF_ILKN_ID_B)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_lanes, SOC_PB_NIF_ILKN_B_NOF_LANES_MIN, SOC_PB_NIF_ILKN_B_NOF_LANES_MAX, SOC_PB_ILKN_NOF_LANES_OUT_OF_RANGE_ERR, 62, exit);
  }

  if (info->nof_lanes > SOC_PB_NIF_ILKN_NOF_LANES_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_ILKN_NOF_LANES_OUT_OF_RANGE_ERR, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ilkn_verify()", ilkn_ndx, direction_ndx);
}

uint32
  soc_pb_nif_ilkn_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ILKN_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(ilkn_ndx, SOC_PB_NIF_ILKN_ID_A, SOC_PB_NIF_ILKN_ID_B, SOC_PB_ILKN_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ilkn_ndx_verify()", 0, 0);
}

STATIC uint32
  soc_pb_nif_ilkn_modes_retrieve(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      ilkn_id,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_OUT  SOC_PB_NIF_ILKN_INFO              *info
  )
{
  uint32
    fld_val,
    last_lane_fld_val,
    has_bad_lane_fld_val = 0x0,
    bad_lane_fld_val = 0x0,
    res = SOC_SAND_OK;
  uint32
    directn = SOC_PETRA_NIF_TO_REG_DIRECTION(direction_ndx);
  uint8
    min_lanes_set = FALSE;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ILKN_SET_UNSAFE);

  regs = soc_petra_regs();

  /*
   *    Same for RX/TX
   */
  SOC_PB_FLD_GET(regs->nbi.rx_ilkn_control_reg[ilkn_id].rx_full_packet_mode, fld_val, 2, exit);
  info->is_burst_interleaving = SOC_SAND_NUM2BOOL_INVERSE(fld_val);

  /*
   *    Meta-frame
   */
  if (direction_ndx == SOC_PETRA_CONNECTION_DIRECTION_RX)
  {
    SOC_PB_FLD_GET(regs->nbi.rx_ilkn_control_reg[ilkn_id].rx_mframe_len_minus1, fld_val, 8, exit);
  }
  else
  {
    SOC_PB_FLD_GET(regs->nbi.tx_ilkn_control_reg_0[ilkn_id].tx_mframe_len_minus1, fld_val, 10, exit);
  }
  info->metaframe_sync_period = fld_val + 1;

  SOC_PB_FLD_GET(regs->nbi.ilkn_num_lanes_config_reg[ilkn_id].last_lane[directn], last_lane_fld_val, 20, exit);
  SOC_PB_FLD_GET(regs->nbi.ilkn_num_lanes_config_reg[ilkn_id].has_bad_lane[directn], has_bad_lane_fld_val, 22, exit);
  SOC_PB_FLD_GET(regs->nbi.ilkn_num_lanes_config_reg[ilkn_id].bad_lane[directn], bad_lane_fld_val, 24, exit);

  /*
   *    Number of lanes and invalid lane mark
   */
  if(ilkn_id == 0)
  {
    if (
         (has_bad_lane_fld_val == 0x1) &&
         (bad_lane_fld_val == SOC_PB_NIF_ILKN_A_NOF_LANES_MIN-1) &&
         (last_lane_fld_val == SOC_PB_NIF_ILKN_A_NOF_LANES_MIN)
       )
    {
      info->nof_lanes = SOC_PB_NIF_ILKN_A_NOF_LANES_MIN;
      info->is_invalid_lane = FALSE;
      info->invalid_lane_id = 0;
      min_lanes_set = TRUE;
    }
  }
  else /* (ilkn_id == 1) */
  {
    if (
         (has_bad_lane_fld_val == 0x1) &&
         (bad_lane_fld_val == SOC_PB_NIF_ILKN_B_NOF_LANES_MIN-1) &&
         (last_lane_fld_val == SOC_PB_NIF_ILKN_B_NOF_LANES_MIN )
       )
    {
      info->nof_lanes = SOC_PB_NIF_ILKN_B_NOF_LANES_MIN;
      info->is_invalid_lane = FALSE;
      info->invalid_lane_id = 0;
      min_lanes_set = TRUE;
    }
  }

  if (!min_lanes_set)
  {
    info->nof_lanes = last_lane_fld_val + 1;
    info->is_invalid_lane = SOC_SAND_NUM2BOOL(has_bad_lane_fld_val);
    info->invalid_lane_id = bad_lane_fld_val;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ilkn_modes_retrieve()", ilkn_id, direction_ndx);
}
/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_ilkn_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_OUT SOC_PB_NIF_ILKN_INFO               *info_rx,
    SOC_SAND_OUT SOC_PB_NIF_ILKN_INFO               *info_tx
  )
{
  uint32
    ilkn_id = SOC_PB_NIF_ID_OFFSET(ILKN, ilkn_ndx);
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ILKN_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info_rx);
  SOC_SAND_CHECK_NULL_INPUT(info_tx);

  SOC_PB_NIF_ILKN_INFO_clear(info_rx);
  SOC_PB_NIF_ILKN_INFO_clear(info_tx);

  res = soc_pb_nif_ilkn_modes_retrieve(
          unit,
          ilkn_id,
          SOC_PETRA_CONNECTION_DIRECTION_RX,
          info_rx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_nif_ilkn_modes_retrieve(
          unit,
          ilkn_id,
          SOC_PETRA_CONNECTION_DIRECTION_TX,
          info_tx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ilkn_get_unsafe()", ilkn_id, 0);
}

/*********************************************************************
*     Reads the Interlaken status from the ILKN interrupt
 *     register and from other status registers. Clears the
 *     interrupts only if requested.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_ilkn_diag_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  uint8                      clear_interrupts,
    SOC_SAND_OUT SOC_PB_NIF_ILKN_DIAG_INFO          *info
  )
{
  uint32
    reg_val,
    fld_val,
    res = SOC_SAND_OK;
  uint32
    ilkn_id = SOC_PB_NIF_ID_OFFSET(ILKN, ilkn_ndx),
    actual_nof_lanes,
    base_lane,
    last_lane;
  SOC_PB_NIF_ILKN_INFO
    ilkn_info_rx,
    ilkn_info_tx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ILKN_DIAG_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  res = soc_pb_nif_ilkn_get_unsafe(
          unit,
          ilkn_ndx,
          &ilkn_info_rx,
          &ilkn_info_tx
        );
  if (SOC_PB_IS_ERR_RES(res))
  {
    actual_nof_lanes = SOC_PB_NIF_ILKN_NOF_LANES_MAX;
  }
  else
  {
    actual_nof_lanes = soc_pb_nif_ilkn_actual_nof_lanes_get(ilkn_id, &ilkn_info_rx);
  }

  /*
   *    Internal register representation, 0..23
   */
  base_lane = (ilkn_id == 0)?0:12;
  last_lane = SOC_SAND_RNG_LAST(base_lane, actual_nof_lanes);

  /************************************************************************/
  /* Read the interrupts                                                  */
  /************************************************************************/
  SOC_PB_REG_GET(regs->nbi.ilkn_interrupt_reg, reg_val, 10, exit);
  
  SOC_PB_FLD_FROM_REG(regs->nbi.ilkn_interrupt_reg.ilkn_rx_port_status_change_int[ilkn_id], fld_val, reg_val, 20, exit);
  info->interrupt.ilkn_rx_port_status_change_int = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.ilkn_interrupt_reg.ilkn_link_partner_status_change_lanes_int[ilkn_id], fld_val, reg_val, 22, exit);
  info->interrupt.ilkn_link_partner_status_change_lanes_int = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.ilkn_interrupt_reg.ilkn_rx_port_align_err_int[ilkn_id], fld_val, reg_val, 24, exit);
  info->interrupt.ilkn_rx_port_align_err_int = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.ilkn_interrupt_reg.ilkn_crc32_lane_err_indication_lanes_int[ilkn_id], fld_val, reg_val, 26, exit);
  info->interrupt.ilkn_crc32_lane_err_indication_lanes_int = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.ilkn_interrupt_reg.ilkn_lane_synch_err_lanes_int[ilkn_id], fld_val, reg_val, 28, exit);
  info->interrupt.ilkn_lane_synch_err_lanes_int = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.ilkn_interrupt_reg.ilkn_lane_framing_err_lanes_int[ilkn_id], fld_val, reg_val, 30, exit);
  info->interrupt.ilkn_lane_framing_err_lanes_int = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.ilkn_interrupt_reg.ilkn_lane_bad_framing_type_err_lanes_int[ilkn_id], fld_val, reg_val, 32, exit);
  info->interrupt.ilkn_lane_bad_framing_type_err_lanes_int = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.ilkn_interrupt_reg.ilkn_lane_meta_frame_sync_word_err_lanes_int[ilkn_id], fld_val, reg_val, 34, exit);
  info->interrupt.ilkn_lane_meta_frame_sync_word_err_lanes_int = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.ilkn_interrupt_reg.ilkn_lane_scrambler_state_err_lanes_int[ilkn_id], fld_val, reg_val, 36, exit);
  info->interrupt.ilkn_lane_scrambler_state_err_lanes_int = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.ilkn_interrupt_reg.ilkn_lane_meta_frame_length_err_lanes_int[ilkn_id], fld_val, reg_val, 38, exit);
  info->interrupt.ilkn_lane_meta_frame_length_err_lanes_int = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.ilkn_interrupt_reg.ilkn_lane_meta_frame_repeat_err_lanes_int[ilkn_id], fld_val, reg_val, 40, exit);
  info->interrupt.ilkn_lane_meta_frame_repeat_err_lanes_int = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.ilkn_interrupt_reg.ilkn_rx_port_status_err_int[ilkn_id], fld_val, reg_val, 42, exit);
  info->interrupt.ilkn_rx_port_status_err_int = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.ilkn_interrupt_reg.ilkn_tx_port_status_err_int[ilkn_id], fld_val, reg_val, 44, exit);
  info->interrupt.ilkn_tx_port_status_err_int = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.ilkn_interrupt_reg.ilkn_rx_parity_err_int, fld_val, reg_val, 46, exit);
  info->interrupt.ilkn_rx_parity_err_int = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.ilkn_interrupt_reg.ilkn_tx_parity_err_int, fld_val, reg_val, 48, exit);
  info->interrupt.ilkn_tx_parity_err_int = SOC_SAND_NUM2BOOL(fld_val);

  /*
   *    If requested, clear the interrupts
   */
  if (clear_interrupts)
  {
    fld_val = 0x1;
    SOC_PB_FLD_TO_REG(regs->nbi.ilkn_interrupt_reg.ilkn_rx_port_status_change_int[ilkn_id], fld_val, reg_val, 120, exit);
    SOC_PB_FLD_TO_REG(regs->nbi.ilkn_interrupt_reg.ilkn_link_partner_status_change_lanes_int[ilkn_id], fld_val, reg_val, 122, exit);
    SOC_PB_FLD_TO_REG(regs->nbi.ilkn_interrupt_reg.ilkn_rx_port_align_err_int[ilkn_id], fld_val, reg_val, 124, exit);
    SOC_PB_FLD_TO_REG(regs->nbi.ilkn_interrupt_reg.ilkn_crc32_lane_err_indication_lanes_int[ilkn_id], fld_val, reg_val, 126, exit);
    SOC_PB_FLD_TO_REG(regs->nbi.ilkn_interrupt_reg.ilkn_lane_synch_err_lanes_int[ilkn_id], fld_val, reg_val, 128, exit);
    SOC_PB_FLD_TO_REG(regs->nbi.ilkn_interrupt_reg.ilkn_lane_framing_err_lanes_int[ilkn_id], fld_val, reg_val, 130, exit);
    SOC_PB_FLD_TO_REG(regs->nbi.ilkn_interrupt_reg.ilkn_lane_bad_framing_type_err_lanes_int[ilkn_id], fld_val, reg_val, 132, exit);
    SOC_PB_FLD_TO_REG(regs->nbi.ilkn_interrupt_reg.ilkn_lane_meta_frame_sync_word_err_lanes_int[ilkn_id], fld_val, reg_val, 134, exit);
    SOC_PB_FLD_TO_REG(regs->nbi.ilkn_interrupt_reg.ilkn_lane_scrambler_state_err_lanes_int[ilkn_id], fld_val, reg_val, 136, exit);
    SOC_PB_FLD_TO_REG(regs->nbi.ilkn_interrupt_reg.ilkn_lane_meta_frame_length_err_lanes_int[ilkn_id], fld_val, reg_val, 138, exit);
    SOC_PB_FLD_TO_REG(regs->nbi.ilkn_interrupt_reg.ilkn_lane_meta_frame_repeat_err_lanes_int[ilkn_id], fld_val, reg_val, 140, exit);
    SOC_PB_FLD_TO_REG(regs->nbi.ilkn_interrupt_reg.ilkn_rx_port_status_err_int[ilkn_id], fld_val, reg_val, 142, exit);
    SOC_PB_FLD_TO_REG(regs->nbi.ilkn_interrupt_reg.ilkn_tx_port_status_err_int[ilkn_id], fld_val, reg_val, 144, exit);
    SOC_PB_FLD_TO_REG(regs->nbi.ilkn_interrupt_reg.ilkn_rx_parity_err_int, fld_val, reg_val, 146, exit);
    SOC_PB_FLD_TO_REG(regs->nbi.ilkn_interrupt_reg.ilkn_tx_parity_err_int, fld_val, reg_val, 148, exit);
    SOC_PB_REG_SET(regs->nbi.ilkn_interrupt_reg, reg_val, 150, exit);
  }

  /************************************************************************/
  /* Other statuses                                                       */
  /************************************************************************/
  
  /*
   *    Rx[0..1]Ilkn Status
   */
  SOC_PB_REG_GET(regs->nbi.rx_ilkn_status_reg[ilkn_id], reg_val, 160, exit);

  SOC_PB_FLD_FROM_REG(regs->nbi.rx_ilkn_status_reg[ilkn_id].rx_stat_aligned_raw,  fld_val, reg_val, 170, exit);
  info->stat.rx_stat_aligned_raw   = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.rx_ilkn_status_reg[ilkn_id].rx_port_active,       fld_val, reg_val, 172, exit);
  info->stat.rx_port_active        = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.rx_ilkn_status_reg[ilkn_id].rx_stat_misaligned,   fld_val, reg_val, 174, exit);
  info->stat.rx_stat_misaligned    = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.rx_ilkn_status_reg[ilkn_id].rx_stat_aligned_err,  fld_val, reg_val, 176, exit);
  info->stat.rx_stat_aligned_err   = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.rx_ilkn_status_reg[ilkn_id].rx_stat_crc24_err,    fld_val, reg_val, 178, exit);
  info->stat.rx_stat_crc24_err     = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.rx_ilkn_status_reg[ilkn_id].rx_stat_miss_sop_err, fld_val, reg_val, 180, exit);
  info->stat.rx_stat_miss_sop_err  = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.rx_ilkn_status_reg[ilkn_id].rx_stat_miss_eop_err, fld_val, reg_val, 182, exit);
  info->stat.rx_stat_miss_eop_err  = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.rx_ilkn_status_reg[ilkn_id].rx_stat_overflow_err, fld_val, reg_val, 184, exit);
  info->stat.rx_stat_overflow_err  = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.rx_ilkn_status_reg[ilkn_id].rx_stat_burstmax_err, fld_val, reg_val, 186, exit);
  info->stat.rx_stat_burstmax_err  = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.rx_ilkn_status_reg[ilkn_id].rx_stat_burst_err,    fld_val, reg_val, 188, exit);
  info->stat.rx_stat_burst_err     = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.rx_ilkn_status_reg[ilkn_id].rx_lp_ifc_status,     fld_val, reg_val, 190, exit);
  info->stat.rx_lp_ifc_status      = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_GET(regs->nbi.rx_ilkn_status_parity_error_reg.ilkn_rx_stat_buff_parity_err[ilkn_id], fld_val, 192, exit);
  info->stat.ilkn_rx_stat_buff_parity_err = SOC_SAND_NUM2BOOL(fld_val);

  /*
   *    Tx[0..1]Ilkn Status
   */
  SOC_PB_REG_GET(regs->nbi.tx_ilkn_status_reg[ilkn_id], reg_val, 200, exit);

  SOC_PB_FLD_FROM_REG(regs->nbi.tx_ilkn_status_reg[ilkn_id].tx_ovfout,  fld_val, reg_val, 210, exit);
  info->stat.tx_ovfout   = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.tx_ilkn_status_reg[ilkn_id].tx_port_active,  fld_val, reg_val, 212, exit);
  info->stat.tx_port_active   = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.tx_ilkn_status_reg[ilkn_id].tx_stat_underflow_err,  fld_val, reg_val, 214, exit);
  info->stat.tx_stat_underflow_err   = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.tx_ilkn_status_reg[ilkn_id].tx_stat_burst_err,  fld_val, reg_val, 216, exit);
  info->stat.tx_stat_burst_err   = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->nbi.tx_ilkn_status_reg[ilkn_id].tx_stat_overflow_err,  fld_val, reg_val, 218, exit);
  info->stat.tx_stat_overflow_err   = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_GET(regs->nbi.tx_ilkn_status_parity_error_reg[ilkn_id].ilkn_tx_stat_buff_parity_err, fld_val, 220, exit);
  info->stat.ilkn_tx_stat_buff_parity_err = SOC_SAND_NUM2BOOL(fld_val);


  /*
   *    Lane Bitmaps
   */
  SOC_PB_FLD_GET(regs->nbi.link_partner_lanes_status_reg.ilkn_rx_stat_diagword_lanes_stat, fld_val, 230, exit);
  info->stat.link_partner_lanes_status_bitmap = SOC_SAND_GET_BITS_RANGE(fld_val, last_lane, base_lane);

  SOC_PB_FLD_GET(regs->nbi.link_partner_interface_status_reg.ilkn_rx_stat_diagword_interface_stat, fld_val, 232, exit);
  info->stat.link_partner_interface_status_bitmap = SOC_SAND_GET_BITS_RANGE(fld_val, last_lane, base_lane);

  SOC_PB_FLD_GET(regs->nbi.crc32_lane_valid_indication_reg.ilkn_rx_stat_crc32_valid, fld_val, 234, exit);
  info->stat.crc32_lane_valid_indication_bitmap = SOC_SAND_GET_BITS_RANGE(fld_val, last_lane, base_lane);

  SOC_PB_FLD_GET(regs->nbi.crc32_lane_err_indication_reg.ilkn_rx_stat_crc32_err, fld_val, 236, exit);
  info->stat.crc32_lane_err_indication_bitmap = SOC_SAND_GET_BITS_RANGE(fld_val, last_lane, base_lane);

  SOC_PB_FLD_GET(regs->nbi.lane_synchronization_achieved_reg.ilkn_rx_stat_lanes_synced, fld_val, 238, exit);
  info->stat.lane_synchronization_achieved_bitmap = SOC_SAND_GET_BITS_RANGE(fld_val, last_lane, base_lane);

  SOC_PB_FLD_GET(regs->nbi.lane_synchronization_err_reg.ilkn_rx_stat_lanes_synced_err, fld_val, 240, exit);
  info->stat.lane_synchronization_err_bitmap = SOC_SAND_GET_BITS_RANGE(fld_val, last_lane, base_lane);

  SOC_PB_FLD_GET(regs->nbi.lane_framing_err_reg.ilkn_rx_stat_lanes_framing_err, fld_val, 242, exit);
  info->stat.lane_framing_err_bitmap = SOC_SAND_GET_BITS_RANGE(fld_val, last_lane, base_lane);

  SOC_PB_FLD_GET(regs->nbi.lane_bad_framing_layer_err_reg.ilkn_rx_stat_lanes_bad_type_err, fld_val, 244, exit);
  info->stat.lane_bad_framing_layer_err_bitmap = SOC_SAND_GET_BITS_RANGE(fld_val, last_lane, base_lane);

  SOC_PB_FLD_GET(regs->nbi.lane_meta_frame_sync_word_err_reg.ilkn_rx_stat_lanes_mf_err, fld_val, 246, exit);
  info->stat.lane_meta_frame_sync_word_err_bitmap = SOC_SAND_GET_BITS_RANGE(fld_val, last_lane, base_lane);

  SOC_PB_FLD_GET(regs->nbi.lane_scrambler_state_err_reg.ilkn_rx_stat_lanes_descram_err, fld_val, 248, exit);
  info->stat.lane_scrambler_state_err_bitmap = SOC_SAND_GET_BITS_RANGE(fld_val, last_lane, base_lane);

  SOC_PB_FLD_GET(regs->nbi.lane_meta_frame_length_err_reg.ilkn_rx_stat_lanes_mf_len_err, fld_val, 250, exit);
  info->stat.lane_meta_frame_length_err_bitmap = SOC_SAND_GET_BITS_RANGE(fld_val, last_lane, base_lane);

  SOC_PB_FLD_GET(regs->nbi.lane_meta_frame_repeat_err_reg.ilkn_rx_stat_lanes_mf_repeat_err, fld_val, 252, exit);
  info->stat.lane_meta_frame_repeat_err_bitmap = SOC_SAND_GET_BITS_RANGE(fld_val, last_lane, base_lane);

  SOC_PB_FLD_GET(regs->nbi.rx_ilkn_status_parity_error_reg.ilkn_rx_stat_lanes_parity_err, fld_val, 254, exit);
  /* Petra b code. Almost not in use. Ignore coverity defects */
  /* coverity[large_shift] */
  info->stat.ilkn_rx_stat_lanes_parity_err_bitmap = SOC_SAND_GET_BITS_RANGE(fld_val, last_lane, base_lane);

  SOC_PB_FLD_GET(regs->nbi.tx_ilkn_status_parity_error_reg[ilkn_id].ilkn_tx_stat_lanes_parity_err, fld_val, 256, exit);
  info->stat.ilkn_tx_stat_lanes_parity_err = fld_val;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ilkn_diag_unsafe()", ilkn_id, 0);
}

uint32
  soc_pb_nif_ilkn_diag_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  uint8                      clear_interrupts
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ILKN_DIAG_VERIFY);

  res = soc_pb_nif_ilkn_ndx_verify(unit, ilkn_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ilkn_diag_verify()", 0, 0);
}

/*********************************************************************
 *     Indicates whether the interface is configured and
 *     powered up
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_stat_activity_get_unsafe(
    SOC_SAND_IN   int                      unit,
    SOC_SAND_IN   SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT  SOC_PB_NIF_ACTIVITY_STATUS         *info
  )
{
  uint32
    fld_val,
    reg_val,
    res = SOC_SAND_OK;
  uint8
    is_nif_on;
  uint32
    nif_id,
    nif_in_malg_id,
    mal_ndx,
    malg_id;
  SOC_PB_NIF_STATE_INFO
    nif_state;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_STAT_ACTIVITY_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_PB_NIF_ACTIVITY_STATUS_clear(info);
  SOC_PB_NIF_STATE_INFO_clear(&nif_state);

  regs = soc_petra_regs();

  /*
   *    Indexing
   */
  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_ndx = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  malg_id = SOC_PB_NIF_MAL2MALG_ID(mal_ndx);
  nif_in_malg_id = SOC_PB_NIF_IN_MALG(nif_id);

  res = soc_pb_nif_on_off_get_unsafe(
          unit,
          nif_ndx,
          &nif_state
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  is_nif_on = nif_state.is_nif_on;
  info->enabled = is_nif_on;

  if (is_nif_on)
  {
    /* Is-Up, read twice since this is a sticky (zero-on-fail) bit */
    SOC_PB_REG_IGET(regs->nif.nif_status_reg, reg_val, malg_id, 14, exit);

    /* Active-TX */
    SOC_PB_REG_IGET(regs->nif.nif_tx_active_reg, reg_val, malg_id, 10, exit);
    fld_val = SOC_SAND_GET_BIT(reg_val, nif_in_malg_id);
    info->status.is_active_tx = SOC_SAND_NUM2BOOL(fld_val);

    /* Active-RX */
    SOC_PB_REG_IGET(regs->nif.nif_rx_active_reg, reg_val, malg_id, 12, exit);
    fld_val = SOC_SAND_GET_BIT(reg_val, nif_in_malg_id);
    info->status.is_active_rx = SOC_SAND_NUM2BOOL(fld_val);

    /* Is-Up, read twice since this is a sticky (zero-on-fail) bit */
    SOC_PB_REG_IGET(regs->nif.nif_status_reg, reg_val, malg_id, 15, exit);
    fld_val = SOC_SAND_GET_BIT(reg_val, nif_in_malg_id);
    info->status.is_up = SOC_SAND_NUM2BOOL(fld_val);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_stat_activity_get_unsafe()", nif_ndx, 0);
}

uint32
  soc_pb_nif_stat_activity_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_STAT_ACTIVITY_GET_VERIFY);

  res = soc_pb_nif_id_verify(nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_stat_activity_ndx_verify()", nif_ndx, 0);
}

/*********************************************************************
 *     Indicates whether the interface is configured and
 *     powered up. All interfaces are read at once.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_stat_activity_all_get_unsafe(
    SOC_SAND_IN   int                      unit,
    SOC_SAND_OUT  SOC_PB_NIF_ACTIVITY_STATUS_ALL     *info
  )
{
  uint32
    reg_val,
    res = SOC_SAND_OK;
  uint32
    malg_id,
    nif_in_malg_id,
    nif_inner_id;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_STAT_ACTIVITY_ALL_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_PB_NIF_ACTIVITY_STATUS_ALL_clear(info);

  regs = soc_petra_regs();

  for (malg_id = 0; malg_id < SOC_PB_NIF_NOF_MALG_IDS; malg_id++)
  {
    /* Is-Up, read twice since this is a sticky (zero-on-fail) bit */
    SOC_PB_REG_IGET(regs->nif.nif_status_reg, reg_val, malg_id, 14, exit);

    /* Active-TX */
    SOC_PB_REG_IGET(regs->nif.nif_tx_active_reg, reg_val, malg_id, 10, exit);
    for (nif_in_malg_id = 0; nif_in_malg_id < SOC_PB_NIFS_IN_MALG; nif_in_malg_id++)
    {
      nif_inner_id = malg_id * SOC_PB_NIFS_IN_MALG + nif_in_malg_id;
      info->activity[nif_inner_id].is_active_tx = SOC_SAND_GET_BIT(reg_val, nif_in_malg_id);;
    }
    
    /* Active-RX */
    SOC_PB_REG_IGET(regs->nif.nif_rx_active_reg, reg_val, malg_id, 12, exit);
    for (nif_in_malg_id = 0; nif_in_malg_id < SOC_PB_NIFS_IN_MALG; nif_in_malg_id++)
    {
      nif_inner_id = malg_id * SOC_PB_NIFS_IN_MALG + nif_in_malg_id;
      info->activity[nif_inner_id].is_active_rx = SOC_SAND_GET_BIT(reg_val, nif_in_malg_id);;
    }
    
    /* Is-Up, read twice since this is a sticky (zero-on-fail) bit */
    SOC_PB_REG_IGET(regs->nif.nif_status_reg, reg_val, malg_id, 15, exit);
    for (nif_in_malg_id = 0; nif_in_malg_id < SOC_PB_NIFS_IN_MALG; nif_in_malg_id++)
    {
      nif_inner_id = malg_id * SOC_PB_NIFS_IN_MALG + nif_in_malg_id;
      info->activity[nif_inner_id].is_up = SOC_SAND_GET_BIT(reg_val, nif_in_malg_id);;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_stat_activity_all_get_unsafe()", 0, 0);
}

uint32
  soc_pb_nif_stat_activity_all_ndx_verify(
    SOC_SAND_IN  int                      unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_STAT_ACTIVITY_ALL_GET_VERIFY);

  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_stat_activity_all_ndx_verify()", 0, 0);
}

/*********************************************************************
 *     Gets link status, and whether there was change in the
 *     status.
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_link_status_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_LINK_STATUS             *link_status
  )
{
  uint32
    fld_val,
    reg_val,
    val,
    res = SOC_SAND_OK;
  uint32
    nif_id,
    lane_id,
    fld_id,
    mal_id,
    ilkn_id;
  SOC_PB_NIF_TYPE
    nif_type;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_LINK_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(link_status);

  regs = soc_petra_regs();

  SOC_PB_NIF_LINK_STATUS_clear(link_status);

  /*
   *    Indexing
   */
  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  lane_id = SOC_PB_NIF_GMII_LANE_INTERN(nif_id);

  nif_type = soc_pb_nif_id2type(nif_ndx);

  if(nif_type == SOC_PB_NIF_TYPE_ILKN)
  {
    ilkn_id = SOC_PB_NIF_ID_OFFSET(ILKN, nif_ndx);
    SOC_PB_FLD_GET(regs->nbi.rx_ilkn_status_reg[ilkn_id].rx_stat_aligned_raw, fld_val, 160, exit);
    link_status->link_status  = SOC_SAND_NUM2BOOL(fld_val);
    link_status->link_status_change = FALSE;
    link_status->pcs_sync = FALSE;
  }
  else
  {
    SOC_PB_REG_IGET(regs->nif_mac_lane.link_status_reg, reg_val, mal_id, 10, exit);
    SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.link_status_reg.link_status, fld_val, reg_val, 12, exit);
    val = SOC_SAND_GET_BIT(fld_val, lane_id);
    link_status->link_status = SOC_SAND_NUM2BOOL(val);

    SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.link_status_reg.link_status_sticky, fld_val, reg_val, 22, exit);
    val = SOC_SAND_GET_BIT(fld_val, lane_id);
    link_status->link_status_change = SOC_SAND_NUM2BOOL(link_status->link_status && (val == 0));

    if(SOC_PB_NIF_IS_TYPE_XAUI_LIKE(nif_type))
    {
      SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.link_status_reg.rx_pcs_sync_stat, fld_val, reg_val, 30, exit);
      link_status->pcs_sync = SOC_SAND_NUM2BOOL(fld_val);
    }
    else
    {
      /* SGMII lane-id 0, 2 correspond to bits 0, 1. QSGMII: same for all lanes */
      fld_id = (nif_type == SOC_PB_NIF_TYPE_SGMII)?lane_id/2: lane_id;
      SOC_PB_FLD_FROM_REG(regs->nif_mac_lane.link_status_reg.rx_pcs_ln_sync_stat, fld_val, reg_val, 40, exit);
      val = SOC_SAND_GET_BIT(fld_val, fld_id);
      link_status->pcs_sync = SOC_SAND_NUM2BOOL(val);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_link_status_get_unsafe()", nif_ndx, 0);
}

uint32
  soc_pb_nif_link_status_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_LINK_STATUS_GET_VERIFY);

  res = soc_pb_nif_id_verify(
          nif_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_link_status_ndx_verify()", 0, 0);
}

STATIC uint32
  soc_pb_nif_synce_clk_div2intern(
    SOC_SAND_IN SOC_PB_NIF_SYNCE_CLK_DIV clk_div
  )
{
  uint32
    clk_div_val;

  switch(clk_div) {
  case SOC_PB_NIF_SYNCE_CLK_DIV_20:
    clk_div_val = 0x1;
      break;
  case SOC_PB_NIF_SYNCE_CLK_DIV_40:
    clk_div_val = 0x2;
      break;
  case SOC_PB_NIF_SYNCE_CLK_DIV_80:
    clk_div_val = 0x3;
      break;
  default:
    clk_div_val = 0x0;
  }

  return clk_div_val;
}

STATIC SOC_PB_NIF_SYNCE_CLK_DIV
  soc_pb_nif_synce_clk_intern2div(
    SOC_SAND_IN uint32 clk_div_val
  )
{
  SOC_PB_NIF_SYNCE_CLK_DIV
    clk_div;

  switch(clk_div_val) {
  case 0x1:
    clk_div = SOC_PB_NIF_SYNCE_CLK_DIV_20;
      break;
  case 0x2:
    clk_div = SOC_PB_NIF_SYNCE_CLK_DIV_40;
      break;
  case 0x3:
    clk_div = SOC_PB_NIF_SYNCE_CLK_DIV_80;
      break;
  default:
    clk_div = SOC_PB_NIF_NOF_SYNCE_CLK_DIVS;
  }

  return clk_div;
}

/*********************************************************************
 *     Function description
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_synce_clk_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_ID            clk_ndx,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK               *clk
  )
{
  uint32
    reg_val,
    div_fld_val,
    squelch_fld_val,
    mal_id_val,
    res = SOC_SAND_OK;
  uint32
    malg_id;
  SOC_PB_NIF_SYNCE_MODE
    mode;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SYNCE_CLK_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(clk);

  regs = soc_petra_regs();
  
  res = soc_pb_nif_synce_mode_get_unsafe(unit, &mode);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!clk->enable)
  {
    div_fld_val = 0x0;
    SOC_PB_DO_NOTHING_AND_EXIT;
  }
  else
  {
    div_fld_val = soc_pb_nif_synce_clk_div2intern(clk->clk_divider);
  }

  squelch_fld_val = SOC_SAND_BOOL2NUM(clk->squelch_enable);
  mal_id_val = SOC_PB_NIF2MAL_GLBL_ID(soc_pb_nif2intern_id(clk->nif_id));

  switch(clk_ndx) {
  case SOC_PB_NIF_SYNCE_CLK_ID_0:
    malg_id = 0;
    SOC_PB_REG_IGET(regs->nif.nif_sync_eth1_reg, reg_val, malg_id, 20, exit);
    SOC_PB_FLD_TO_REG(regs->nif.nif_sync_eth1_reg.sync_eth_clock_sel1, mal_id_val, reg_val, 22, exit);
    SOC_PB_FLD_TO_REG(regs->nif.nif_sync_eth1_reg.sync_eth_clock_div1, div_fld_val, reg_val, 24, exit);
    SOC_PB_FLD_TO_REG(regs->nif.nif_sync_eth1_reg.sync_eth_squelch_en1, squelch_fld_val, reg_val, 26, exit);
    SOC_PB_REG_ISET(regs->nif.nif_sync_eth1_reg, reg_val, malg_id, 28, exit);
      break;
  case SOC_PB_NIF_SYNCE_CLK_ID_2:
    malg_id = 1;
    SOC_PB_REG_IGET(regs->nif.nif_sync_eth1_reg, reg_val, malg_id, 20, exit);
    SOC_PB_FLD_TO_REG(regs->nif.nif_sync_eth1_reg.sync_eth_clock_sel1, mal_id_val, reg_val, 22, exit);
    SOC_PB_FLD_TO_REG(regs->nif.nif_sync_eth1_reg.sync_eth_clock_div1, div_fld_val, reg_val, 24, exit);
    SOC_PB_FLD_TO_REG(regs->nif.nif_sync_eth1_reg.sync_eth_squelch_en1, squelch_fld_val, reg_val, 26, exit);
    SOC_PB_REG_ISET(regs->nif.nif_sync_eth1_reg, reg_val, malg_id, 28, exit);
      break;
  case SOC_PB_NIF_SYNCE_CLK_ID_1_OR_VALID:
    malg_id = 0;
    SOC_PB_REG_IGET(regs->nif.nif_sync_eth1_reg, reg_val, malg_id, 20, exit);
    SOC_PB_FLD_TO_REG(regs->nif.nif_sync_eth1_reg.sync_eth_clock_sel2, mal_id_val, reg_val, 22, exit);
    SOC_PB_FLD_TO_REG(regs->nif.nif_sync_eth1_reg.sync_eth_clock_div2, div_fld_val, reg_val, 24, exit);
    SOC_PB_FLD_TO_REG(regs->nif.nif_sync_eth1_reg.sync_eth_squelch_en2, squelch_fld_val, reg_val, 26, exit);
    SOC_PB_REG_ISET(regs->nif.nif_sync_eth1_reg, reg_val, malg_id, 28, exit);
      break;
  case SOC_PB_NIF_SYNCE_CLK_ID_3_OR_VALID:
    malg_id = 1;
    SOC_PB_REG_IGET(regs->nif.nif_sync_eth1_reg, reg_val, malg_id, 20, exit);
    SOC_PB_FLD_TO_REG(regs->nif.nif_sync_eth1_reg.sync_eth_clock_sel2, mal_id_val, reg_val, 22, exit);
    SOC_PB_FLD_TO_REG(regs->nif.nif_sync_eth1_reg.sync_eth_clock_div2, div_fld_val, reg_val, 24, exit);
    SOC_PB_FLD_TO_REG(regs->nif.nif_sync_eth1_reg.sync_eth_squelch_en2, squelch_fld_val, reg_val, 26, exit);
    SOC_PB_REG_ISET(regs->nif.nif_sync_eth1_reg, reg_val, malg_id, 28, exit);
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_CLK_NDX_OUT_OF_RANGE_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_synce_clk_set_unsafe()", clk_ndx, 0);
}

uint32
  soc_pb_nif_synce_clk_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_ID            clk_ndx,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK               *clk
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    malg_id;
  SOC_PB_NIF_SYNCE_MODE
    mode;
  SOC_PETRA_INTERFACE_ID
    if_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SYNCE_CLK_SET_VERIFY);
  SOC_SAND_CHECK_NULL_INPUT(clk);

  res = soc_pb_nif_synce_clk_ndx_verify(
          unit,
          clk_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_SYNCE_CLK, clk, 20, exit);

  res = soc_pb_nif_synce_mode_get_unsafe(unit, &mode);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (SOC_PB_NIF_SYNCE_IS_TWO_CLK_MODE(mode))
  {
    if (!SOC_PB_NIF_SYNCE_IS_TWO_CLK_ID(clk_ndx))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_SYNCE_INVALID_CLK_ID_FOR_TWO_DIFF_CLK_ERR, 40, exit);
    }
  }

  if (mode == SOC_PB_NIF_SYNCE_MODE_FOUR_CLK)
  {
    SOC_PB_MAL2IF_NDX(clk->nif_id, if_idx);
    malg_id = SOC_PB_NIF_MAL2MALG_ID(if_idx);

    if (malg_id == 0)
    {
      if ((clk_ndx == SOC_PB_NIF_SYNCE_CLK_ID_2) || (clk_ndx == SOC_PB_NIF_SYNCE_CLK_ID_3_OR_VALID))
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_SYNCE_CLK_ID_AND_SRC_MISMATCH_ERR, 50, exit);
      }
    }

    if (malg_id == 1)
    {
      if ((clk_ndx == SOC_PB_NIF_SYNCE_CLK_ID_0) || (clk_ndx == SOC_PB_NIF_SYNCE_CLK_ID_1_OR_VALID))
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_SYNCE_CLK_ID_AND_SRC_MISMATCH_ERR, 52, exit);
      }
    }
  }

  if ((clk->clk_divider == SOC_PB_NIF_SYNCE_CLK_DIV_20) && (clk->squelch_enable == TRUE))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_SYNCE_CLK_SQUELTCH_ON_DIV_20_ERR, 54, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_synce_clk_verify()", clk_ndx, 0);
}

uint32
  soc_pb_nif_synce_clk_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_ID            clk_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SYNCE_CLK_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(clk_ndx, SOC_PB_NIF_CLK_NDX_MAX, SOC_PB_CLK_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_synce_clk_ndx_verify()", 0, 0);
}

/*********************************************************************
 *     Function description
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_synce_clk_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_ID            clk_ndx,
    SOC_SAND_OUT SOC_PB_NIF_SYNCE_CLK               *clk
  )
{
  uint32
    reg_val,
    div_fld_val,
    squelch_fld_val,
    mal_id_val,
    res = SOC_SAND_OK;
  uint32
    malg_id;
  SOC_PB_NIF_SYNCE_MODE
    mode;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SYNCE_CLK_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(clk);
  SOC_PB_NIF_SYNCE_CLK_clear(clk);

  regs = soc_petra_regs();
  res = soc_pb_nif_synce_mode_get_unsafe(unit, &mode);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  switch(clk_ndx) {
  case SOC_PB_NIF_SYNCE_CLK_ID_0:
    malg_id = 0;
    SOC_PB_REG_IGET(regs->nif.nif_sync_eth1_reg, reg_val, malg_id, 20, exit);
    SOC_PB_FLD_FROM_REG(regs->nif.nif_sync_eth1_reg.sync_eth_clock_sel1, mal_id_val, reg_val, 22, exit);
    SOC_PB_FLD_FROM_REG(regs->nif.nif_sync_eth1_reg.sync_eth_clock_div1, div_fld_val, reg_val, 24, exit);
    SOC_PB_FLD_FROM_REG(regs->nif.nif_sync_eth1_reg.sync_eth_squelch_en1, squelch_fld_val, reg_val, 26, exit);
      break;
  case SOC_PB_NIF_SYNCE_CLK_ID_2:
    malg_id = 1;
    SOC_PB_REG_IGET(regs->nif.nif_sync_eth1_reg, reg_val, malg_id, 20, exit);
    SOC_PB_FLD_FROM_REG(regs->nif.nif_sync_eth1_reg.sync_eth_clock_sel1, mal_id_val, reg_val, 22, exit);
    SOC_PB_FLD_FROM_REG(regs->nif.nif_sync_eth1_reg.sync_eth_clock_div1, div_fld_val, reg_val, 24, exit);
    SOC_PB_FLD_FROM_REG(regs->nif.nif_sync_eth1_reg.sync_eth_squelch_en1, squelch_fld_val, reg_val, 26, exit);
      break;
  case SOC_PB_NIF_SYNCE_CLK_ID_1_OR_VALID:
    malg_id = 0;
    SOC_PB_REG_IGET(regs->nif.nif_sync_eth1_reg, reg_val, malg_id, 20, exit);
    SOC_PB_FLD_FROM_REG(regs->nif.nif_sync_eth1_reg.sync_eth_clock_sel2, mal_id_val, reg_val, 22, exit);
    SOC_PB_FLD_FROM_REG(regs->nif.nif_sync_eth1_reg.sync_eth_clock_div2, div_fld_val, reg_val, 24, exit);
    SOC_PB_FLD_FROM_REG(regs->nif.nif_sync_eth1_reg.sync_eth_squelch_en2, squelch_fld_val, reg_val, 26, exit);
      break;
  case SOC_PB_NIF_SYNCE_CLK_ID_3_OR_VALID:
    malg_id = 1;
    SOC_PB_REG_IGET(regs->nif.nif_sync_eth1_reg, reg_val, malg_id, 20, exit);
    SOC_PB_FLD_FROM_REG(regs->nif.nif_sync_eth1_reg.sync_eth_clock_sel2, mal_id_val, reg_val, 22, exit);
    SOC_PB_FLD_FROM_REG(regs->nif.nif_sync_eth1_reg.sync_eth_clock_div2, div_fld_val, reg_val, 24, exit);
    SOC_PB_FLD_FROM_REG(regs->nif.nif_sync_eth1_reg.sync_eth_squelch_en2, squelch_fld_val, reg_val, 26, exit);
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_CLK_NDX_OUT_OF_RANGE_ERR, 50, exit);
  }

  clk->enable = SOC_SAND_NUM2BOOL_INVERSE(div_fld_val == 0x0);
  if (!clk->enable)
  {
    SOC_PB_DO_NOTHING_AND_EXIT;
  }
  else
  {
    clk->clk_divider = soc_pb_nif_synce_clk_intern2div(div_fld_val);
  }
  clk->squelch_enable = SOC_SAND_NUM2BOOL(squelch_fld_val);
  SOC_PB_MAL2IF_NDX(mal_id_val, clk->nif_id);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_synce_clk_get_unsafe()", 0, 0);
}

/*********************************************************************
 *     Define the Recovered Clock signal to be transmitted on
 *     the Soc_petra external pins
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_synce_mode_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_MODE              mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SYNCE_MODE_SET_UNSAFE);

  soc_pb_sw_db_synce_set(unit, mode);
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_synce_mode_set_unsafe()", 0, 0);
}

uint32
  soc_pb_nif_synce_mode_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_MODE              mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SYNCE_MODE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mode, SOC_PB_NIF_SYNCE_MODE_MAX, SOC_PB_MODE_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_synce_mode_verify()", 0, 0);
}

uint32
  soc_pb_nif_synce_mode_ndx_verify(
    SOC_SAND_IN  int                      unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SYNCE_MODE_GET_VERIFY);

  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_synce_mode_ndx_verify()", 0, 0);
}

/*********************************************************************
 *     Define the Recovered Clock signal to be transmitted on
 *     the Soc_petra external pins
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_synce_mode_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_NIF_SYNCE_MODE              *mode
  )
{
  SOC_PB_NIF_SYNCE_MODE
    synce_mode;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_SYNCE_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mode);

  synce_mode = soc_pb_sw_db_synce_get(unit);
  *mode = synce_mode;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_synce_mode_get_unsafe()", 0, 0);
}

/*********************************************************************
 *     Reads the PTP clock current reset time value
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_ptp_clk_reset_value_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT  SOC_PB_NIF_PTP_CLK_RST_VAL         *info
  )
{
  uint32
    malg_id = 0,
    res = SOC_SAND_OK;
  SOC_SAND_U64
    rst_fld_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CLK_RESET_VALUE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_NIF_PTP_CLK_RST_VAL_clear(info);
  regs = soc_petra_regs();

  /*
   *    Read for MALG 0, same for 1
   */
  SOC_PB_FLD_IGET(regs->nif.nif_ptp1588_reset_reg[0].ptp_tslc_reset_value, rst_fld_val.arr[0], malg_id, 10, exit);
  SOC_PB_FLD_IGET(regs->nif.nif_ptp1588_reset_reg[1].ptp_tslc_reset_value, rst_fld_val.arr[1], malg_id, 12, exit);

  SOC_PETRA_COPY(&(info->curr_time), &rst_fld_val, SOC_SAND_U64, 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_clk_reset_value_get_unsafe()", 0, 0);
}

uint32
  soc_pb_nif_ptp_clk_reset_value_ndx_verify(
    SOC_SAND_IN   int                      unit,
    SOC_SAND_OUT  SOC_PB_NIF_PTP_CLK_RST_VAL         *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CLK_RESET_VALUE_GET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_PTP_CLK_RST_VAL, info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_clk_reset_value_ndx_verify()", 0, 0);
}

/*
 * Return the MAL Clock frequency[Hz].
 * This frequency is serdes_rate[bps]/K,
 * Where K is constant per NIF-Type
 */
STATIC uint32
  soc_pb_nif_ptp_mal_clk_freq_get_khz(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint32   srd_rate_kbps,
    SOC_SAND_IN SOC_PB_NIF_TYPE nif_type
  )
{
  uint32
    intern_delay_k,
    mal_clk_khz;

  switch (nif_type)
  {
  case SOC_PB_NIF_TYPE_XAUI:
    intern_delay_k = SOC_PB_NIF_PTP_INTERN_DELAY_FACTOR_XAUI;
    break;
  case SOC_PB_NIF_TYPE_SGMII:
    intern_delay_k = SOC_PB_NIF_PTP_INTERN_DELAY_FACTOR_SGMII;
      break;
  case SOC_PB_NIF_TYPE_RXAUI:
    intern_delay_k = SOC_PB_NIF_PTP_INTERN_DELAY_FACTOR_RXAUI;
    break;
  case SOC_PB_NIF_TYPE_QSGMII:
    intern_delay_k = SOC_PB_NIF_PTP_INTERN_DELAY_FACTOR_QSGMII;
    break;
  default:
    intern_delay_k = SOC_PB_NIF_PTP_INTERN_DELAY_FACTOR_XAUI;
  }

  mal_clk_khz = SOC_SAND_DIV_ROUND_DOWN(srd_rate_kbps, intern_delay_k);

  return mal_clk_khz;
}

STATIC uint32
  soc_pb_nif_tslc_factor_get(
    SOC_SAND_IN SOC_PB_NIF_TYPE nif_type
  )
{
  uint32
    tslc_factor;

  switch (nif_type)
  {
  case SOC_PB_NIF_TYPE_XAUI:
    tslc_factor = SOC_PB_NIF_PTP_BIAS_TSLC_FACTOR_XAUI;
      break;
  case SOC_PB_NIF_TYPE_SGMII:
    tslc_factor = SOC_PB_NIF_PTP_BIAS_TSLC_FACTOR_SGMII;
    break;
  case SOC_PB_NIF_TYPE_RXAUI:
    tslc_factor = SOC_PB_NIF_PTP_BIAS_TSLC_FACTOR_RXAUI;
    break;
  case SOC_PB_NIF_TYPE_QSGMII:
    tslc_factor = SOC_PB_NIF_PTP_BIAS_TSLC_FACTOR_QSGMII;
    break;
  default:
    tslc_factor = SOC_PB_NIF_PTP_BIAS_TSLC_FACTOR_DFLT;
    break;
  }

  return tslc_factor;
}

STATIC uint32
  soc_pb_nif_ptp_intern_bias_get(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint32   mal_ndx,
    SOC_SAND_IN  SOC_PB_NIF_TYPE nif_type,
    SOC_SAND_OUT uint32    *intern_bias,
    SOC_SAND_OUT uint32    *intern_delta_clk
  )
{
  uint32
    srd_rate_kbps,
    mal_clk_freq_khz,
    tslc_factor,
    bias,
    delta_clk;
  uint32
    srd_base_lane_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_INTERN_BIAS_GET);

  srd_base_lane_id = mal_ndx * SOC_PB_SRD_LANES_PER_MAL;

  srd_rate_kbps = soc_petra_srd_rate_calc_kbps(unit, srd_base_lane_id);

  if (SOC_SAND_IS_INTERN_VAL_INVALID(srd_rate_kbps))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SRD_RATE_UNKNOWN_ERR, 3, exit);
  }

  mal_clk_freq_khz = soc_pb_nif_ptp_mal_clk_freq_get_khz(
                           unit,
                           srd_rate_kbps,
                           nif_type
                         );

  if (mal_clk_freq_khz == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_DIV_BY_ZERO_ERR, 10, exit);
  }

  delta_clk = SOC_SAND_DIV_ROUND_DOWN(SOC_PB_NIF_PTP_BIAS_RESOLUTION_FACTOR * 1000 * 1000, mal_clk_freq_khz);

  tslc_factor = soc_pb_nif_tslc_factor_get(nif_type);
  bias = SOC_SAND_DIV_ROUND_DOWN(tslc_factor * 1000 * 1000, mal_clk_freq_khz);

  *intern_bias = bias;
  *intern_delta_clk = delta_clk;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_nif_ptp_intern_bias_get()", mal_ndx, 0);
}


/*********************************************************************
 *     Set the Clock Frequency for the Precision Time Protocol
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_ptp_clk_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_PTP_CLK_INFO            *info
  )
{
  uint32
    delta_ptp_clk_fld_val,
    res = SOC_SAND_OK;
  uint32
    malg_id;
  SOC_SAND_U64
    reset_val_intern,
    autoinc_intern;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CLK_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  delta_ptp_clk_fld_val = info->ptp_clk_delta;

  /* Convert milliseconds to nanoseconds */
  SOC_PETRA_COPY(&autoinc_intern, &info->sync_autoinc_interval, SOC_SAND_U64, 1);
  SOC_PETRA_COPY(&reset_val_intern, &info->sync_reset_val, SOC_SAND_U64, 1);
  for (malg_id = 0; malg_id < SOC_PB_NIF_NOF_MALGS; malg_id++)
  {
    SOC_PB_FLD_ISET(regs->nif.nif_ptp1588_reg.ptp_delta_each_ptp_clock, delta_ptp_clk_fld_val, malg_id, 10, exit);

    SOC_PB_FLD_ISET(regs->nif.nif_ptp1588_reset_reg[0].ptp_tslc_reset_value, reset_val_intern.arr[0], malg_id, 10, exit);
    SOC_PB_FLD_ISET(regs->nif.nif_ptp1588_reset_reg[1].ptp_tslc_reset_value, reset_val_intern.arr[1], malg_id, 12, exit);

    SOC_PB_FLD_ISET(regs->nif.nif_ptp1588_auto_inc_reg[0].ptp_tslc_reset_value_delta, autoinc_intern.arr[0], malg_id, 22, exit);
    SOC_PB_FLD_ISET(regs->nif.nif_ptp1588_auto_inc_reg[1].ptp_tslc_reset_value_delta, autoinc_intern.arr[1], malg_id, 24, exit);
  }
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_clk_set_unsafe()", 0, 0);
}

uint32
  soc_pb_nif_ptp_clk_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_PTP_CLK_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CLK_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_PTP_CLK_INFO, info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_clk_verify()", 0, 0);
}

uint32
  soc_pb_nif_ptp_clk_ndx_verify(
    SOC_SAND_IN  int                      unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CLK_GET_VERIFY);
  SOC_PB_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_clk_ndx_verify()", 0, 0);
}

/*********************************************************************
 *     Set the Clock Frequency for the Precision Time Protocol
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_ptp_clk_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_NIF_PTP_CLK_INFO            *info
  )
{
  uint32
    delta_ptp_clk_fld_val,
    res = SOC_SAND_OK;
  uint32
    malg_id = 0;
  SOC_SAND_U64
    autoinc_intern,
    reset_val_intern;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CLK_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_PB_NIF_PTP_CLK_INFO_clear(info);

  soc_sand_u64_clear(&reset_val_intern);
  soc_sand_u64_clear(&autoinc_intern);
  
  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  /* Read configuration from MALG-0. Set identically for both MALG-s. */
 
  SOC_PB_FLD_IGET(regs->nif.nif_ptp1588_reg.ptp_delta_each_ptp_clock, delta_ptp_clk_fld_val, malg_id, 10, exit);

  SOC_PB_FLD_IGET(regs->nif.nif_ptp1588_reset_reg[0].ptp_tslc_reset_value, reset_val_intern.arr[0], malg_id, 10, exit);
  SOC_PB_FLD_IGET(regs->nif.nif_ptp1588_reset_reg[1].ptp_tslc_reset_value, reset_val_intern.arr[1], malg_id, 12, exit);

  SOC_PB_FLD_IGET(regs->nif.nif_ptp1588_auto_inc_reg[0].ptp_tslc_reset_value_delta, autoinc_intern.arr[0], malg_id, 22, exit);
  SOC_PB_FLD_IGET(regs->nif.nif_ptp1588_auto_inc_reg[1].ptp_tslc_reset_value_delta, autoinc_intern.arr[1], malg_id, 24, exit);

  info->sync_autoinc_interval = autoinc_intern;
  info->sync_reset_val        = reset_val_intern;
  info->ptp_clk_delta         = delta_ptp_clk_fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_clk_get_unsafe()", 0, 0);
}

/*********************************************************************
 *     Enable/disable and configure parameters for the
 *     Precision Time Protocol
 *     Details: in the H file. (search for prototype)
 *********************************************************************/

uint32
  soc_pb_nif_ptp_conf_mal_config(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_TYPE                    nif_type,
    SOC_SAND_IN  SOC_PB_NIF_PTP_INFO                *info
  )
{
  uint32
    fld_val,
    intern_bias,
    intern_delta_clk,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  uint8
    rx_enable,
    tx_enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CONF_SET_UNSAFE);

  regs = soc_petra_regs();

  SOC_SAND_CHECK_NULL_INPUT(info);

  rx_enable = SOC_PETRA_IS_DIRECTION_RX(direction_ndx);
  tx_enable = SOC_PETRA_IS_DIRECTION_TX(direction_ndx);

  fld_val = SOC_SAND_BOOL2NUM_INVERSE(info->enable);
  SOC_PB_FLD_ISET(regs->nif_mac_lane.ieee_1588_reg.ptp_tslc_reset, fld_val, mal_ndx, 5, exit);
  fld_val = info->enable ? 0x0 : 0xf;
  SOC_PB_FLD_ISET(regs->nif_mac_lane.ieee_1588_reg.ptp_stamping_reset, fld_val, mal_ndx, 6, exit);
  if (info->enable)
  {
    res = soc_pb_nif_ptp_intern_bias_get(
            unit,
            mal_ndx,
            nif_type,
            &intern_bias,
            &intern_delta_clk
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);
    
    fld_val = intern_bias + info->wire_delay_ns;
    if (rx_enable)
    {
      SOC_PB_FLD_ISET(regs->nif_mac_lane.ieee_1588_rx_reg.ptp_rx_tslc_bias, fld_val, mal_ndx, 10, exit);
    }

    if (tx_enable)
    {
      SOC_PB_FLD_ISET(regs->nif_mac_lane.ieee_1588_tx_reg.ptp_tx_tslc_bias, fld_val, mal_ndx, 20, exit);
    }

    SOC_PB_FLD_ISET(regs->nif_mac_lane.ieee_1588_timer_reg.ptp_delta_each_mal_clock, intern_delta_clk, mal_ndx, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_conf_mal()", mal_ndx, direction_ndx);
}

uint32
  soc_pb_nif_ptp_conf_mal_retreive(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PB_NIF_TYPE                    nif_type,
    SOC_SAND_OUT SOC_PB_NIF_PTP_INFO                *info_rx,
    SOC_SAND_OUT SOC_PB_NIF_PTP_INFO                *info_tx
  )
{
  uint32
    fld_val,
    intern_bias,
    intern_delta_clk,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CONF_SET_UNSAFE);

  regs = soc_petra_regs();

  SOC_SAND_CHECK_NULL_INPUT(info_rx);
  SOC_SAND_CHECK_NULL_INPUT(info_tx);

  SOC_PB_FLD_IGET(regs->nif_mac_lane.ieee_1588_reg.ptp_tslc_reset, fld_val, mal_ndx, 5, exit);
  info_rx->enable = info_tx->enable = SOC_SAND_NUM2BOOL_INVERSE(fld_val);
  
  res = soc_pb_nif_ptp_intern_bias_get(
          unit,
          mal_ndx,
          nif_type,
          &intern_bias,
          &intern_delta_clk
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);
  
  SOC_PB_FLD_IGET(regs->nif_mac_lane.ieee_1588_rx_reg.ptp_rx_tslc_bias, fld_val, mal_ndx, 10, exit);
  info_rx->wire_delay_ns = SOC_SAND_DELTA(intern_bias, fld_val);
 
  SOC_PB_FLD_IGET(regs->nif_mac_lane.ieee_1588_tx_reg.ptp_tx_tslc_bias, fld_val, mal_ndx, 20, exit);
  info_tx->wire_delay_ns = SOC_SAND_DELTA(intern_bias, fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_conf_mal()", mal_ndx, 0);
}

uint32
  soc_pb_nif_ptp_conf_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_PTP_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    nif_id,
    mal_id;
  SOC_PB_NIF_TYPE
    nif_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CONF_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  nif_type = soc_pb_nif_id2type(nif_ndx);

  res = soc_pb_nif_ptp_conf_mal_config(
          unit,
          mal_id,
          direction_ndx,
          nif_type,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_conf_set_unsafe()", nif_ndx, direction_ndx);
}

uint32
  soc_pb_nif_ptp_conf_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_PTP_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CONF_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_PTP_INFO, info, 10, exit);

  res = soc_pb_nif_ptp_conf_ndx_verify(
          unit,
          nif_ndx,
          direction_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_conf_verify()", nif_ndx, direction_ndx);
}

uint32
  soc_pb_nif_ptp_conf_ndx_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CONF_GET_VERIFY);

  res = soc_pb_nif_id_verify(nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(direction_ndx, SOC_PB_CONNECTION_DIRECTION_NDX_MAX, SOC_PETRA_CONNECTION_DIRECTION_OUT_OF_RANGE_ERR, 10, exit);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_conf_ndx_verify()", nif_ndx, direction_ndx);
}

/*********************************************************************
 *     Enable/disable and configure parameters for the
 *     Precision Time Protocol
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_ptp_conf_get_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_PTP_INFO *info_rx,
    SOC_SAND_OUT SOC_PB_NIF_PTP_INFO *info_tx
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    nif_id,
    mal_id;
  SOC_PB_NIF_TYPE
    nif_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_PTP_CONF_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info_rx);
  SOC_SAND_CHECK_NULL_INPUT(info_tx);

  SOC_PB_NIF_PTP_INFO_clear(info_rx);
  SOC_PB_NIF_PTP_INFO_clear(info_tx);

  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  nif_type = soc_pb_nif_id2type(nif_ndx);

  res = soc_pb_nif_ptp_conf_mal_retreive(
          unit,
          mal_id,
          nif_type,
          info_rx,
          info_tx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_ptp_conf_get_unsafe()", nif_ndx, 0);
}

/*********************************************************************
 *     Function description
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_elk_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ELK_INFO                *info
  )
{
  uint32
    enable_val,
    fld_val,
    reg_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_PP_REGS
    *pp_regs;
  SOC_PB_NIF_PTP_INFO                
    ptp_info;
  SOC_PB_FC_GEN_INBND_INFO
    fc_gen_inbnd_info;
  SOC_PB_FC_REC_INBND_INFO
    fc_rec_inbnd_info;
  SOC_PETRA_INTERFACE_ID
    if_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ELK_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  regs = soc_petra_regs();
  pp_regs = soc_pb_pp_regs();

  enable_val = SOC_SAND_BOOL2NUM(info->enable);
  
  SOC_PB_REG_GET(regs->nbi.elk_cfg_reg, reg_val, 10, exit);

  fld_val = (info->mal_id == 0) ? enable_val:0;
  SOC_PB_FLD_TO_REG(regs->nbi.elk_cfg_reg.elk_enable_mal0, fld_val, reg_val, 12, exit);

  fld_val = (info->mal_id == 12) ? enable_val:0;
  SOC_PB_FLD_TO_REG(regs->nbi.elk_cfg_reg.elk_enable_mal12, fld_val, reg_val, 14, exit);

  fld_val = (info->mal_id == 14) ? enable_val:0;
  SOC_PB_FLD_TO_REG(regs->nbi.elk_cfg_reg.elk_enable_mal14, fld_val, reg_val, 16, exit);

  SOC_PB_REG_SET(regs->nbi.elk_cfg_reg, reg_val, 18, exit);

  /* Nif related configuration */
  fld_val = SOC_SAND_BOOL2NUM_INVERSE(info->enable);
  SOC_PB_FLD_ISET(regs->nif_mac_lane.async_fifo_config_reg.tx_extra_delay_rd, fld_val, info->mal_id, 30, exit); 

  fld_val = SOC_SAND_BOOL2NUM_INVERSE(info->enable);
  SOC_PB_FLD_ISET(regs->nif_mac_lane.async_fifo_config_reg.tx_extra_delay_wr, fld_val, info->mal_id, 40, exit); 

  if (info->enable == 1) {
    /* Disable ptp - 1588 ieee */
    SOC_PB_NIF_PTP_INFO_clear(&ptp_info);
    ptp_info.enable = FALSE;
    res = soc_pb_nif_ptp_conf_mal_config(
                unit,
                info->mal_id,
                SOC_PETRA_CONNECTION_DIRECTION_BOTH,
                SOC_PB_NIF_TYPE_RXAUI,
                &ptp_info
              );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /* Disable Tx flow control */
    SOC_PB_FC_GEN_INBND_INFO_clear(&fc_gen_inbnd_info);
    fc_gen_inbnd_info.mode = SOC_PB_FC_INBND_MODE_DISABLED;
    SOC_PB_MAL2IF_NDX(info->mal_id, if_idx);
    res = soc_pb_fc_gen_inbnd_set_unsafe(
                unit,
                if_idx,
                &fc_gen_inbnd_info
              );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    /* Disable Rx flow control */
    SOC_PB_FC_REC_INBND_INFO_clear(&fc_rec_inbnd_info);
    fc_rec_inbnd_info.mode = SOC_PB_FC_INBND_MODE_DISABLED;
    res = soc_pb_fc_rec_inbnd_set_unsafe(
                unit,
                if_idx,
                &fc_rec_inbnd_info
              );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    /* Set minimum packet size */
    res = soc_pb_nif_min_packet_size_set_unsafe(
                unit,
                info->mal_id,
                SOC_PETRA_CONNECTION_DIRECTION_BOTH,
                0x20
            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }

  /* configure EGW and enable ELK */
  if (info->enable == 1) {
      fld_val = 0xb8;
  } else {
      fld_val = 0xbe;
  }
  SOC_PB_PP_FLD_SET(pp_regs->ihb.flp_general_cfg_reg.egw_full_threshold , fld_val, 100, exit);

   fld_val = SOC_SAND_BOOL2NUM(info->enable);
   SOC_PB_PP_FLD_SET(pp_regs->ihb.lookup_control_reg.enable_elk_lookup,    fld_val, 120, exit);
   SOC_PB_PP_FLD_SET(pp_regs->ihb.flp_general_cfg_reg.elk_enable,                  fld_val, 130, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_elk_set_unsafe()", 0, 0);
}

uint32
  soc_pb_nif_elk_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ELK_INFO                *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ELK_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_ELK_INFO, info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_elk_verify()", 0, 0);
}

/*********************************************************************
 *     Function description
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_elk_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_NIF_ELK_INFO                *info
  )
{
  uint32
    fld_val,
    reg_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ELK_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_NIF_ELK_INFO_clear(info);

  regs = soc_petra_regs();

  SOC_PB_REG_GET(regs->nbi.elk_cfg_reg, reg_val, 10, exit);

  SOC_PB_FLD_FROM_REG(regs->nbi.elk_cfg_reg.elk_enable_mal0, fld_val, reg_val, 12, exit);
   if (fld_val == 0x1) {
       info->mal_id = 0;
       info->enable = TRUE;
   }

   SOC_PB_FLD_FROM_REG(regs->nbi.elk_cfg_reg.elk_enable_mal12, fld_val, reg_val, 12, exit);
   if (fld_val == 0x1) {
       info->mal_id = 12;
       info->enable = TRUE;
   }

   SOC_PB_FLD_FROM_REG(regs->nbi.elk_cfg_reg.elk_enable_mal14, fld_val, reg_val, 12, exit);
   if (fld_val == 0x1) {
       info->mal_id = 14;
       info->enable = TRUE;
   }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_elk_get_unsafe()", 0, 0);
}

STATIC void
  soc_pb_nif_fatp2mode_intern(
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE   fatp_mode,
    SOC_SAND_OUT uint32           *intern_egr,
    SOC_SAND_OUT uint32           *intern_ingr
  )
{
  uint32
    egr_val,
    ingr_val;

  switch(fatp_mode) {
  case SOC_PB_NIF_FATP_MODE_DISABLED:
    ingr_val = 0x3;
    egr_val = 0x0;
      break;
  case SOC_PB_NIF_FATP_MODE_1_X_12:
    ingr_val = 0x2;
    egr_val = 0x1;
      break;
  case SOC_PB_NIF_FATP_MODE_2_X_6:
    ingr_val = 0x1;
    egr_val = 0x2;
      break;
  case SOC_PB_NIF_FATP_MODE_3_X_4:
    ingr_val = 0x0;
    egr_val = 0x3;
      break;
  default:
    egr_val = ingr_val = SOC_PB_NIF_INVALID_VAL_INTERN;
  }

  *intern_egr = egr_val;
  *intern_ingr = ingr_val;
}

STATIC SOC_PB_NIF_FATP_MODE
  soc_pb_nif_fatp_mode_from_egr(
    SOC_SAND_IN   uint32            intern_egr
  )
{
  SOC_PB_NIF_FATP_MODE
    mode;

  switch(intern_egr) {
  case 0x0:
    mode = SOC_PB_NIF_FATP_MODE_DISABLED;
      break;
  case 0x1:
    mode = SOC_PB_NIF_FATP_MODE_1_X_12;
      break;
  case 0x2:
    mode = SOC_PB_NIF_FATP_MODE_2_X_6;
      break;
  case 0x3:
    mode = SOC_PB_NIF_FATP_MODE_3_X_4;
      break;
  default:
    mode = SOC_PB_NIF_FATP_MODE_DISABLED;
  }

  return mode;
}

/*
 *    Per Fat-pipe mode,
 *  return the number of Fat-pipes and
 *  the maximal allowed number of ports per Fat-pipe
 */
void
  soc_pb_nif_fatp_mode2count(
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE   fatp_mode,
    SOC_SAND_OUT uint32          *nof_fatps,
    SOC_SAND_OUT uint32          *nof_ports_per_fatp
  )
{
  if ((nof_fatps == NULL) || (nof_ports_per_fatp == NULL))
  {
    return;
  }

  switch(fatp_mode) {
  case SOC_PB_NIF_FATP_MODE_1_X_12:
    *nof_fatps = 1;
    *nof_ports_per_fatp = 12;
      break;
  case SOC_PB_NIF_FATP_MODE_2_X_6:
    *nof_fatps = 2;
    *nof_ports_per_fatp = 6;
      break;
  case SOC_PB_NIF_FATP_MODE_3_X_4:
    *nof_fatps = 3;
    *nof_ports_per_fatp = 4;
      break;
  case SOC_PB_NIF_FATP_MODE_DISABLED:
  default:
    *nof_fatps = 0;
    *nof_ports_per_fatp = 0;
  }
}

/*********************************************************************
 *     Sets Fat-pipe configuration mode
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_fatp_mode_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE_INFO      *info
  )
{
  uint32
    fld_val,
    egr_fld_val,
    ingr_fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_FATP_MODE_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  soc_pb_nif_fatp2mode_intern(info->mode, &egr_fld_val, &ingr_fld_val);
  SOC_PB_FLD_SET(regs->eci.general_controls_reg.oc768c_mode, egr_fld_val, 10, exit);
  SOC_PB_FLD_SET(regs->irr.resequencer_configuration_reg.resequencer_mode, ingr_fld_val, 12, exit);

  if (info->mode == SOC_PB_NIF_FATP_MODE_DISABLED)
  {
    SOC_PB_REG_SET(regs->irr.resequencer_fifo_index_reg[0], 0xffffffff, 20, exit);
    SOC_PB_REG_SET(regs->irr.resequencer_fifo_index_reg[1], 0xffffffff, 22, exit);
  }
  else
  {
    fld_val = SOC_SAND_BOOL2NUM(info->is_bypass_enable);
    SOC_PB_FLD_SET(regs->irr.resequencer_configuration_reg.resequencer_bypass_enable, fld_val, 30, exit);
    fld_val = SOC_SAND_BOOL2NUM_INVERSE(info->is_bypass_enable);
    SOC_PB_FLD_SET(regs->egq.oc768_configuration_reg.seqnum_msk_dis, fld_val, 32, exit);

    if (info->is_bypass_enable)
    {
      SOC_PB_FLD_SET(regs->epni.fat_pipe_configuration_reg.seqnum_msk_dis, 0x0, 36, exit);
      SOC_PB_FLD_SET(regs->epni.fat_pipe_configuration_reg.seqnum_msk_msb, 0x0, 38, exit);
    }
    else
    {
      /* Bypass disabled */
      SOC_PB_FLD_SET(regs->epni.fat_pipe_configuration_reg.seqnum_msk_dis, 0x1, 40, exit);
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_fatp_mode_set_unsafe()", 0, 0);
}

uint32
  soc_pb_nif_fatp_mode_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE_INFO      *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_FATP_MODE_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_FATP_MODE_INFO, info, 10, exit);
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_fatp_mode_verify()", 0, 0);
}

/*********************************************************************
 *     Sets Fat-pipe configuration mode
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_fatp_mode_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PB_NIF_FATP_MODE_INFO      *info
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_FATP_MODE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PB_NIF_FATP_MODE_INFO_clear(info);

  SOC_PB_FLD_GET(regs->eci.general_controls_reg.oc768c_mode, fld_val, 10, exit);
  info->mode = soc_pb_nif_fatp_mode_from_egr(fld_val);

  SOC_PB_FLD_GET(regs->irr.resequencer_configuration_reg.resequencer_bypass_enable, fld_val, 20, exit);
  info->is_bypass_enable = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_fatp_mode_get_unsafe()", 0, 0);
}

STATIC uint32
  soc_pb_nif_fatp_base_fifo_get(
    SOC_SAND_IN SOC_PB_NIF_FATP_ID    fatp_ndx,
    SOC_SAND_IN SOC_PB_NIF_FATP_MODE  mode
  )
{
  uint32
    fifo_base;

  switch(mode) {
  case SOC_PB_NIF_FATP_MODE_1_X_12:
    fifo_base = 0;
      break;
  case SOC_PB_NIF_FATP_MODE_2_X_6:
    fifo_base = fatp_ndx * SOC_PB_NIF_FATP_2X6_PORT_MAX;
      break;
  case SOC_PB_NIF_FATP_MODE_3_X_4:
    fifo_base = fatp_ndx * SOC_PB_NIF_FATP_3X4_PORT_MAX;
      break;
  default:
    fifo_base = SOC_PB_NIF_INVALID_VAL_INTERN;
  }

  return fifo_base;
}

/*********************************************************************
 *     Sets Fat-pipe configuration mode
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_fatp_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID                 fatp_ndx,
    SOC_SAND_IN  SOC_PB_NIF_FATP_INFO               *info
  )
{
  uint32
    fld_val,
    oc768_qm_mask_fld_val,
    val,
    fifo_full_mark_fld_val,
    res = SOC_SAND_OK;
  uint32
    fld_idx,
    reg_idx,
    port_idx,
    port_last_potential,
    port_last_actual,
    fifo_base,
    fifo_idx;
  SOC_PB_NIF_FATP_MODE_INFO
    basic;
  uint32
    max_nof_fatp,
    max_nof_ports_per_fatp;
  SOC_PETRA_REGS
    *regs;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_FATP_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();
  
  res = soc_pb_nif_fatp_mode_get_unsafe(
          unit,
          &basic
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  soc_pb_nif_fatp_mode2count(basic.mode, &max_nof_fatp, &max_nof_ports_per_fatp);
  port_last_potential = SOC_SAND_RNG_LAST(info->base_port_id, max_nof_ports_per_fatp);
  port_last_actual = SOC_SAND_RNG_LAST(info->base_port_id, info->nof_ports);
  
  fifo_base = soc_pb_nif_fatp_base_fifo_get(fatp_ndx, basic.mode);
  if (fifo_base == SOC_PB_NIF_INVALID_VAL_INTERN)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_FATP_MODE_OUT_OF_RANGE_ERR, 5, exit);
  }
    
  /************************************************************************/
  /* Ingress                                                              */
  /************************************************************************/
  SOC_PB_FLD_SET(regs->irr.resequencer_ports_reg.resequencer_port[fatp_ndx], info->base_port_id, 10, exit);

  if (info->enable)
  {
    /*
     * Calculate the resequencer_fifo_full_mark
     */
    fifo_full_mark_fld_val = SOC_PB_NIF_FATP_MEMORY_SIZE;
    if (max_nof_fatp != 0)
    {
      fifo_full_mark_fld_val /= max_nof_fatp;
    }

    if (info->nof_ports > 1)
    {
      /*
       * Note! assumes symmetrical configuration here (i.e. all FAT-Pipes are set to the same number of ports)
       */
      fifo_full_mark_fld_val /= (info->nof_ports-1);
    }

    /* Leave some extra-margin */
    if(fifo_full_mark_fld_val > (SOC_PB_NIF_FATP_FULL_MARK_TH_MIN + SOC_PB_NIF_FATP_FULL_MARK_TH_MARGINE))
    {
      fifo_full_mark_fld_val -= SOC_PB_NIF_FATP_FULL_MARK_TH_MARGINE;
    }

    SOC_PB_FLD_SET(regs->irr.resequencer_configuration_reg.resequencer_fifo_full_mark, fifo_full_mark_fld_val, 14, exit);
  }

  for (port_idx = info->base_port_id; port_idx <= port_last_potential; port_idx++)
  {
    /* Resequencing FIFO */
    fifo_idx = fifo_base + (port_idx - info->base_port_id);
    reg_idx = SOC_PETRA_REG_IDX_GET(port_idx, SOC_PB_REG_NOF_IRR_RESEQ_PORT_PER_FIFO);
    fld_idx = SOC_PETRA_FLD_IDX_GET(port_idx, SOC_PB_REG_NOF_IRR_RESEQ_PORT_PER_FIFO);
    
    if (!info->enable)
    {
      fld_val = SOC_PB_NIF_FATP_IRR_DISABLE_VAL;
    }
    else
    {
      if (port_idx <= port_last_actual)
      {
        fld_val = fifo_idx;
      }
      else
      {
        /* Not part of the resequencing (disabled) */
        fld_val = SOC_PB_NIF_FATP_IRR_DISABLE_VAL;
      }
    }
    SOC_PB_FLD_SET(regs->irr.resequencer_fifo_index_reg[reg_idx].resequencer_fifo_index[fld_idx], fld_val, 20, exit);
  }

  /************************************************************************/
  /* Egress                                                               */
  /************************************************************************/
  
  /* Clear Port Mask */
  reg_idx = fatp_ndx;
  SOC_PB_FLD_GET(regs->epni.fat_pipe_configuration_reg.fat_pipe_ports, fld_val, 22, exit); /* Common for all fatp-s */
  oc768_qm_mask_fld_val = 0x0; /* Per fatp-ndx */
  
  /* Build the actual mask for the relevant resequencer */
  reg_idx = fatp_ndx;
  for (port_idx = info->base_port_id; port_idx <= port_last_potential; port_idx++)
  {
    /* Resequencing FIFO */
    fifo_idx = fifo_base + (port_idx - info->base_port_id);
    val = (info->enable && (port_idx <= port_last_actual))?0x1:0x0;
    /* Bitmap: bit '0' is for OFP 1 */
    SOC_SAND_SET_BIT(fld_val, val, fifo_idx);
    SOC_SAND_SET_BIT(oc768_qm_mask_fld_val, val, fifo_idx);
  }

  SOC_PB_FLD_SET(regs->egq.oc768_qm_mask_reg[reg_idx].oc768_qm_mask, oc768_qm_mask_fld_val, 35, exit);
  SOC_PB_FLD_SET(regs->epni.fat_pipe_configuration_reg.fat_pipe_ports, fld_val, 40, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_fatp_set_unsafe()", fatp_ndx, 0);
}

uint32
  soc_pb_nif_fatp_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID             fatp_ndx,
    SOC_SAND_IN  SOC_PB_NIF_FATP_INFO           *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_NIF_FATP_MODE_INFO
    fatp_mode_info;
  uint32
    max_nof_fatp = 0,
    max_nof_ports_per_fatp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_FATP_SET_VERIFY);

  res = soc_pb_nif_fatp_mode_get_unsafe(
          unit,
          &fatp_mode_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  soc_pb_nif_fatp_mode2count(fatp_mode_info.mode, &max_nof_fatp, &max_nof_ports_per_fatp);

  SOC_SAND_ERR_IF_ABOVE_MAX(fatp_ndx, SOC_PB_NIF_FATP_NDX_MAX, SOC_PB_FATP_NDX_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX((uint32)fatp_ndx, SOC_SAND_RNG_LAST(SOC_PB_NIF_FATP_ID_A, max_nof_fatp), SOC_PB_FATP_NDX_OUT_OF_RANGE_ERR, 14, exit);

  
  res = SOC_PB_NIF_FATP_INFO_verify(
          fatp_mode_info.mode,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_fatp_verify()", fatp_ndx, max_nof_fatp);
}

uint32
  soc_pb_nif_fatp_ndx_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID             fatp_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_FATP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(fatp_ndx, SOC_PB_NIF_FATP_NDX_MAX, SOC_PB_FATP_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_fatp_ndx_verify()", 0, 0);
}

/************************************************************************/
/* 'is_fatp_port' is set to TRUE if the port is part of a Fat-pipe      */
/************************************************************************/
uint32
  soc_pb_nif_is_fatp_port_get(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint32   port_ndx,
    SOC_SAND_OUT uint8   *is_fatp_port
  )
{
  uint32
    fld_val = 0,
    val = 0,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  uint32
    reg_idx,
    fatp_ndx;
  uint8
    is_fatp = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_IS_FATP_PORT_GET);

  regs = soc_petra_regs();

  for (fatp_ndx = 0; fatp_ndx <= SOC_PB_NIF_FATP_NDX_MAX; fatp_ndx++)
  {
    /* Read the actual mask for the relevant resequencer */
    reg_idx = fatp_ndx;
    SOC_PB_FLD_GET(regs->egq.oc768_qm_mask_reg[reg_idx].oc768_qm_mask, fld_val, 35, exit);

    /* Bitmap: set bit '0' for OFP 1 */
    val = SOC_SAND_GET_BIT(fld_val, (port_ndx - 1));
    if (val != 0)
    {
      is_fatp = TRUE;
      break;
    }
  }

  *is_fatp_port = is_fatp;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_nif_is_fatp_port_get()", port_ndx, 0);
}

/*********************************************************************
 *     Sets Fat-pipe configuration mode
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_pb_nif_fatp_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID             fatp_ndx,
    SOC_SAND_OUT SOC_PB_NIF_FATP_INFO           *info
  )
{
  uint32
    val,
    fld_val,
    res = SOC_SAND_OK;
  uint32
    ports_count = 0,
    port_idx,
    port_last_potential,
    max_nof_fatp,
    max_nof_ports_per_fatp,
    fifo_base,
    reg_idx;
  SOC_PB_NIF_FATP_MODE_INFO
    basic;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_FATP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  regs = soc_petra_regs();
  SOC_PB_NIF_FATP_INFO_clear(info);

   res = soc_pb_nif_fatp_mode_get_unsafe(
          unit,
          &basic
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  SOC_PB_FLD_GET(regs->irr.resequencer_ports_reg.resequencer_port[fatp_ndx], fld_val, 10, exit);
  info->base_port_id = fld_val;
  if (SOC_SAND_IS_VAL_OUT_OF_RANGE(info->base_port_id, SOC_PB_NIF_FATP_PORT_MIN, SOC_PB_NIF_FATP_PORT_MAX))
  {
    info->enable = FALSE;
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  soc_pb_nif_fatp_mode2count(basic.mode, &max_nof_fatp, &max_nof_ports_per_fatp);
  port_last_potential = SOC_SAND_RNG_LAST(info->base_port_id, max_nof_ports_per_fatp);
  
  fifo_base = soc_pb_nif_fatp_base_fifo_get(fatp_ndx, basic.mode);
  if (fifo_base == SOC_PB_NIF_INVALID_VAL_INTERN)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_FATP_MODE_OUT_OF_RANGE_ERR, 5, exit);
  }

  /* Read the actual mask for the relevant resequencer */
  reg_idx = fatp_ndx;
  SOC_PB_FLD_GET(regs->egq.oc768_qm_mask_reg[reg_idx].oc768_qm_mask, fld_val, 35, exit);

  for (port_idx = info->base_port_id; port_idx <= port_last_potential; port_idx++)
  {
    /* Bitmap: set bit '0' for OFP 1 */
    val = SOC_SAND_GET_BIT(fld_val, (port_idx - 1));
    if (val != 0)
    {
      ports_count++;
    }
  }

  info->enable = SOC_SAND_NUM2BOOL(ports_count != 0);
  info->nof_ports = ports_count;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_fatp_get_unsafe()", fatp_ndx, 0);
}

/*
 *    Converts a NIF counter enumerator to an internal register representation (cntr_intern).
 *  is_nbi_not_mal: if TRUE, this is an NBI register, if FALSE - this is  NIF-MAL register
 */
STATIC void
  soc_pb_nif_counter2intern(
    SOC_SAND_IN  SOC_PB_NIF_COUNTER_TYPE  cntr,
    SOC_SAND_OUT uint32             *cntr_intern,
    SOC_SAND_OUT uint8            *is_nbi_not_mal,
    SOC_SAND_OUT uint32            *cntr_size_bits
  )
{
  uint32
    cntr_val;
  uint32
    cntr_size = 64;
  uint8
    is_nbi = TRUE;

  switch(cntr) {
  case SOC_PB_NIF_RX_OK_OCTETS:
    cntr_val = 0;
  break;
  case SOC_PB_NIF_RX_BCAST_PACKETS:
    cntr_val = 16;
  break;
  case SOC_PB_NIF_RX_MCAST_BURSTS:
    cntr_val = 17;
  break;
  case SOC_PB_NIF_RX_OK_PACKETS:
    cntr_val = 32;
  break;
  case SOC_PB_NIF_RX_ERR_PACKETS:
    cntr_val = 33;
  break;
  case SOC_PB_NIF_RX_LEN_BELOW_MIN:
    cntr_val = 48;
  break;
  case SOC_PB_NIF_RX_LEN_MIN_59:
    cntr_val = 49;
  break;
  case SOC_PB_NIF_RX_LEN_60:
    cntr_val = 50;
  break;
  case SOC_PB_NIF_RX_LEN_61_123:
    cntr_val = 51;
  break;
  case SOC_PB_NIF_RX_LEN_124_251:
    cntr_val = 52;
  break;
  case SOC_PB_NIF_RX_LEN_252_507:
    cntr_val = 53;
  break;
  case SOC_PB_NIF_RX_LEN_508_1019:
    cntr_val = 54;
  break;
  case SOC_PB_NIF_RX_LEN_1020_1514CFG:
    cntr_val = 55;
  break;
  case SOC_PB_NIF_RX_LEN_1515CFG_MAX:
    cntr_val = 56;
  break;
  case SOC_PB_NIF_RX_LEN_ABOVE_MAX:
    cntr_val = 57;
  break;
  case SOC_PB_NIF_RX_OK_PAUSE_FRAMES:
    cntr_val = 0;
    is_nbi = FALSE; cntr_size = 32;
  break;
  case SOC_PB_NIF_RX_ERR_PAUSE_FRAMES:
    cntr_val = 1;
    is_nbi = FALSE; cntr_size = 32;
  break;
  case SOC_PB_NIF_RX_PTP_FRAMES:
    cntr_val = 2;
    is_nbi = FALSE; cntr_size = 16;
  break;
  case SOC_PB_NIF_RX_FRAME_ERR_PACKETS:
    cntr_val = 3;
    is_nbi = FALSE; cntr_size = 32;
  break;
  case SOC_PB_NIF_RX_BCT_ERR_PACKETS:
    cntr_val = 3; /* On nif-port 3 for XAUI */
    is_nbi = FALSE; cntr_size = 32;
  break;
  case SOC_PB_NIF_TX_OK_OCTETS:
    cntr_val = 64;
  break;
  case SOC_PB_NIF_TX_BCAST_PACKETS:
    cntr_val = 80;
  break;
  case SOC_PB_NIF_TX_MCAST_BURSTS:
    cntr_val = 81;
  break;
  case SOC_PB_NIF_TX_OK_PACKETS:
    cntr_val = 96;
  break;
  case SOC_PB_NIF_TX_ERR_PACKETS:
    cntr_val = 97;
  break;
  case SOC_PB_NIF_TX_PAUSE_FRAMES:
    cntr_val = 8;
    is_nbi = FALSE; cntr_size = 32;
  break;
  case SOC_PB_NIF_TX_PTP_FRAMES:
    cntr_val = 10;
    is_nbi = FALSE; cntr_size = 16;
  break;
  case SOC_PB_NIF_TX_NO_LINK_PACKETS:
    cntr_val = 11;
    is_nbi = FALSE; cntr_size = 32;
  break;
  default:
    cntr_val = SOC_PB_NIF_INVALID_VAL_INTERN;
    is_nbi = FALSE; cntr_size = 32;
  }
  
  *cntr_intern = cntr_val;
  *is_nbi_not_mal = is_nbi;
  *cntr_size_bits = cntr_size;
}

/*
 * sets the overflow of the counter if counter reached maximum value.
 */
STATIC uint8
  soc_pb_nif_mal_counter_is_overflow(
    SOC_SAND_IN   uint32     num_bits,
    SOC_SAND_IN   SOC_SAND_64CNT    *counter_val
  )
{
  SOC_SAND_U64
    max_val;
  uint32
    nof_bits = num_bits,
    first_word_bits;
  uint8
    is_overflow;

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

  is_overflow =
    SOC_SAND_NUM2BOOL(
      (soc_sand_u64_are_equal(&(counter_val->u64), &max_val)) ||
      (soc_sand_u64_is_bigger(&(counter_val->u64), &max_val))
    );

  return is_overflow;
}

/*********************************************************************
*     Gets Value of statistics counter of the NIF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_counter_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID         nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_COUNTER_TYPE        counter_type,
    SOC_SAND_OUT SOC_SAND_64CNT                 *counter_val
  )
{
  uint32
    cntr_id,
    fld_val,
    reg_val,
    res = SOC_SAND_OK;
  uint32
    nof_bits,
    nif_id,
    nif_id_inner,
    mal_id;
  uint8
    valid,
    is_nbi_not_mal;
  SOC_PETRA_POLL_INFO
    poll;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_COUNTER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(counter_val);
  soc_sand_64cnt_clear(counter_val);
  soc_petra_PETRA_POLL_INFO_clear(&poll);

  regs = soc_petra_regs();

  nif_id = soc_pb_nif2intern_id(nif_ndx);
  mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
  nif_id_inner = nif_id % SOC_PB_MAX_NIFS_PER_MAL;

  soc_pb_nif_counter2intern(
    counter_type,
    &cntr_id,
    &is_nbi_not_mal,
    &nof_bits
  );
  reg_val = 0;

  /*
   *    This is just for NBI polling.
   *  Per-MAL counters don't have a "read-done" indication, so
   *  we must just set a delay
   */
  poll.busy_wait_nof_iters = 50;
  poll.timer_nof_iters = 3;
  poll.timer_delay_msec = 1;
  poll.expected_value = 0x1;

  if (is_nbi_not_mal)
  {
    fld_val = cntr_id;
    SOC_PB_FLD_TO_REG(regs->nbi.statistics_read_select_reg.stat_read_cnt_id, fld_val, reg_val, 10, exit);
    fld_val = nif_id;
    SOC_PB_FLD_TO_REG(regs->nbi.statistics_read_select_reg.stat_read_port, fld_val, reg_val, 12, exit);
    SOC_PB_REG_SET(regs->nbi.statistics_read_select_reg, reg_val, 30, exit);

    res = soc_petra_status_fld_poll_unsafe(
            unit,
            SOC_PB_REG_DB_ACC_REF(regs->nbi.statistics_read_msb_reg.stat_cnt_data_valid),
            SOC_PETRA_DEFAULT_INSTANCE,
            &poll,
            &valid
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    if (!valid)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_CNT_RD_INVALID_ERR, 32, exit);
    }

    SOC_PB_REG_GET(regs->nbi.statistics_read_lsb_reg, reg_val, 90, exit);
    counter_val->u64.arr[0] = reg_val;
    SOC_PB_REG_GET(regs->nbi.statistics_read_msb_reg, reg_val, 92, exit);
    SOC_PB_FLD_FROM_REG(regs->nbi.statistics_read_msb_reg.stat_read_cnt_msb, fld_val, reg_val, 94, exit);
    counter_val->u64.arr[1] = fld_val;
    SOC_PB_FLD_FROM_REG(regs->nbi.statistics_read_msb_reg.stat_cnt_data_overflow, fld_val, reg_val, 96, exit);
    counter_val->overflowed = SOC_SAND_NUM2BOOL(fld_val);
  }
  else
  {
    /*
     * NIF-MAL counter.
     */
    fld_val = cntr_id;
    SOC_PB_FLD_TO_REG(regs->nif_mac_lane.statistics_read_select_reg.stat_rd_counter_id, fld_val, reg_val, 110, exit);
    fld_val = nif_id_inner;
    SOC_PB_FLD_TO_REG(regs->nif_mac_lane.statistics_read_select_reg.stat_rd_counter_port, fld_val, reg_val, 112, exit);
    fld_val = 0x0;
    SOC_PB_FLD_TO_REG(regs->nif_mac_lane.statistics_read_select_reg.stat_rd_counter_clr, fld_val, reg_val, 114, exit);
    /* Only the second write (with counter_clr set) activates the counter-read */
    SOC_PB_REG_ISET(regs->nif_mac_lane.statistics_read_select_reg, reg_val, mal_id, 120, exit);
    fld_val = 0x1;
    SOC_PB_FLD_TO_REG(regs->nif_mac_lane.statistics_read_select_reg.stat_rd_counter_clr, fld_val, reg_val, 122, exit);
    SOC_PB_REG_ISET(regs->nif_mac_lane.statistics_read_select_reg, reg_val, mal_id, 130, exit);

    soc_sand_os_nano_sleep(SOC_PB_NIF_CNTR_WAIT_NS, NULL);

    SOC_PB_REG_IGET(regs->nif_mac_lane.statistics_read_counter_reg, fld_val, mal_id, 190, exit);
    counter_val->u64.arr[0] = fld_val;
    counter_val->overflowed = soc_pb_nif_mal_counter_is_overflow(nof_bits, counter_val);
  } /* !is_nbi_not_mal */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_counter_get_unsafe()", nif_ndx, counter_type);
}

uint32
  soc_pb_nif_counter_ndx_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID         nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_COUNTER_TYPE        counter_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_COUNTER_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(counter_type, SOC_PB_NIF_COUNTER_TYPE_MAX, SOC_PB_NIF_COUNTER_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  
  res = soc_pb_nif_id_verify(nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_counter_ndx_verify()", nif_ndx, 0);
}

/*********************************************************************
*     Gets Value of statistics counter of the NIF.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_all_counters_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID         nif_ndx,
    SOC_SAND_IN  uint8                  non_data_also,
    SOC_SAND_OUT SOC_SAND_64CNT                 counter_val[SOC_PB_NIF_NOF_COUNTERS]
  )
{
  uint32
    res = SOC_SAND_OK;
  int32
    counter_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ALL_COUNTERS_GET_UNSAFE);

  for (counter_type = 0; counter_type < SOC_PB_NIF_NOF_COUNTERS; ++counter_type)
  {
    if ((!non_data_also) && SOC_PB_NIF_IS_NON_DATA_CNTR(counter_type))
    {
      continue;
    }
    soc_sand_64cnt_clear(&(counter_val[counter_type]));
    res = soc_pb_nif_counter_get_unsafe(
            unit,
            nif_ndx,
            counter_type,
            &(counter_val[counter_type])
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_all_counters_get_unsafe()", nif_ndx, 0);
}

uint32
  soc_pb_nif_all_counters_ndx_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID         nif_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ALL_COUNTERS_GET_VERIFY);

  res = soc_pb_nif_id_verify(nif_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_all_counters_ndx_verify()", 0, 0);
}

/*********************************************************************
*     Gets Statistics Counters for all the NIFs in the device.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_nif_all_nifs_all_counters_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint8                  non_data_also,
    SOC_SAND_OUT SOC_SAND_64CNT                 counters_val[SOC_PB_NIF_NOF_NIFS][SOC_PB_NIF_NOF_COUNTERS]
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    cntr_id,
    mal_id,
    nif_base_id,
    nif_last_id,
    nif_id;
  SOC_PB_NIF_TYPE
    nif_type;
  SOC_PETRA_INTERFACE_ID
    nif_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_NIF_ALL_NIFS_ALL_COUNTERS_GET_UNSAFE);

  for (nif_id = 0; nif_id < SOC_PB_NIF_NOF_NIFS; nif_id++)
  {
    for (cntr_id = 0; cntr_id < SOC_PB_NIF_NOF_COUNTERS; cntr_id++)
    {
      soc_sand_64cnt_clear(&(counters_val[nif_id][cntr_id]));
    }
  }

  for (mal_id = 0; mal_id < SOC_PB_NOF_MAC_LANES; mal_id++)
  {
    res = soc_pb_nif_mal_type_get(
                  unit,
                  mal_id,
                  &nif_type
                );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    switch(nif_type) {
    case SOC_PB_NIF_TYPE_XAUI:
      if (!SOC_PB_NIF_IS_BASE_MAL(mal_id))
      {
        continue;
      }
      nif_id = SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id);
      nif_idx = soc_pb_nif_intern2nif_id(SOC_PB_NIF_TYPE_XAUI, nif_id);
      res = soc_pb_nif_all_counters_get_unsafe(
              unit,
              nif_idx,
              non_data_also,
              counters_val[nif_id]
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        break;
    case SOC_PB_NIF_TYPE_RXAUI:
      nif_id = SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id);
      nif_idx = soc_pb_nif_intern2nif_id(SOC_PB_NIF_TYPE_RXAUI, nif_id);
      res = soc_pb_nif_all_counters_get_unsafe(
              unit,
              nif_idx,
              non_data_also,
              counters_val[nif_id]
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
        break;
    case SOC_PB_NIF_TYPE_SGMII:
      nif_base_id = SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id);
      nif_last_id = SOC_SAND_RNG_LAST(nif_base_id, 2*SOC_PB_SGMII_NIFS_PER_MAL);
      for (nif_id = nif_base_id; nif_id <= nif_last_id; nif_id+=2)
      {
        nif_idx = soc_pb_nif_intern2nif_id(SOC_PB_NIF_TYPE_SGMII, nif_id);
        res = soc_pb_nif_all_counters_get_unsafe(
                unit,
                nif_idx,
                non_data_also,
                counters_val[nif_id]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
      }
        break;
    case SOC_PB_NIF_TYPE_QSGMII:
      nif_base_id = SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id);
      nif_last_id = SOC_SAND_RNG_LAST(nif_base_id, SOC_PB_QSGMII_NIFS_PER_MAL);
      for (nif_id = nif_base_id; nif_id <= nif_last_id; nif_id++)
      {
        nif_idx = soc_pb_nif_intern2nif_id(SOC_PB_NIF_TYPE_QSGMII, nif_id);
        res = soc_pb_nif_all_counters_get_unsafe(
                unit,
                nif_idx,
                non_data_also,
                counters_val[nif_id]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
      }
        break;
    case SOC_PB_NIF_TYPE_ILKN:
      nif_idx = SOC_PB_NIF_MAL2ILKN(mal_id);
      if (SOC_PB_NIF_IS_TYPE_ID(ILKN, nif_idx))
      {
        nif_id = SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id);
        res = soc_pb_nif_all_counters_get_unsafe(
                unit,
                nif_idx,
                non_data_also,
                counters_val[nif_id]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
      }
        break;
    default:
      continue;
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_nif_all_nifs_all_counters_get_unsafe()", 0, 0);
}

uint32
  SOC_PB_NIF_STATE_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_STATE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_STATE_INFO_verify()",0,0);
}

uint32
  SOC_PB_NIF_LANES_SWAP_verify(
    SOC_SAND_IN  SOC_PB_NIF_LANES_SWAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_LANES_SWAP_verify()",0,0);
}

uint32
  SOC_PB_NIF_MAL_TOPOLOGY_verify(
    SOC_SAND_IN  SOC_PB_NIF_MAL_TOPOLOGY *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_LANES_SWAP, &(info->lanes_swap), 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_MAL_TOPOLOGY_verify()",0,0);
}

uint32
  SOC_PB_NIF_MAL_BASIC_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_MAL_BASIC_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_pb_nif_type_verify(info->type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_MAL_TOPOLOGY, &(info->topology), 11, exit);
  if(!SOC_PB_NIF_IS_TYPE_XAUI_LIKE(info->type) && 
      ((info->topology.lanes_swap.swap_rx == TRUE) ||
       (info->topology.lanes_swap.swap_tx == TRUE) )
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_LANE_SWAP_NOT_APPLICABLE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_MAL_BASIC_INFO_verify()",0,0);
}

uint32
  SOC_PB_NIF_PREAMBLE_COMPRESS_verify(
    SOC_SAND_IN  SOC_PB_NIF_PREAMBLE_COMPRESS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->size, SOC_PB_NIF_PREAMBLE_COMPRESS_SIZE_MAX, SOC_PB_NIF_PREAMBLE_COMPRESS_SIZE_OUT_OF_RANGE_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_PREAMBLE_COMPRESS_verify()",0,0);
}

uint32
  SOC_PB_NIF_IPG_COMPRESS_verify(
    SOC_SAND_IN  SOC_PB_NIF_IPG_COMPRESS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if (info->enable)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(
        info->size, SOC_PB_NIF_IPG_COMPRESS_SIZE_MIN, SOC_PB_NIF_IPG_COMPRESS_SIZE_MAX,
        SOC_PB_NIF_IPG_SIZE_OUT_OF_RANGE_ERR, 20, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
        info->dic_mode, SOC_PB_NIF_IPG_COMPRESS_DIC_MODE_MAX,
        SOC_PB_NIF_DIC_MODE_OUT_OF_RANGE_ERR, 30, exit
      );
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_IPG_COMPRESS_verify()",0,0);
}

uint32
  SOC_PB_NIF_CHANNELIZED_verify(
    SOC_SAND_IN  SOC_PB_NIF_CHANNELIZED *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if (info->enable)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->bct_channel_byte_ndx, SOC_PB_NIF_CHANNELIZED_BCT_CHANNEL_BYTE_NDX_MAX,
      SOC_PB_NIF_BCT_CHANNEL_NDX_OUT_OF_RANGE_ERR ,10, exit
    );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->bct_size, SOC_PB_NIF_CHANNELIZED_BCT_SIZE_MAX,
      SOC_PETRA_NIF_BCT_SIZE_OUT_OF_RANGE_ERR ,20, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_CHANNELIZED_verify()",0,0);
}

uint32
  SOC_PB_NIF_FAULT_RESPONSE_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_FAULT_RESPONSE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->local, SOC_PB_NIF_FR_INFO_LOCAL_MAX, SOC_PB_NIF_FR_LOCAL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->local, SOC_PB_NIF_FR_INFO_LOCAL_MAX, SOC_PB_NIF_FR_REMOTE_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_FAULT_RESPONSE_INFO_verify()",0,0);
}

uint32
  SOC_PB_NIF_SPAUI_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_SPAUI_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->enable)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_PREAMBLE_COMPRESS, &(info->preamble), 13, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(info->crc_mode, SOC_PB_NIF_SPAUI_INFO_CRC_MODE_MAX, SOC_PB_NIF_CRC_MODE_OUT_OF_RANGE_ERR, 15, exit);
    SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_CHANNELIZED, &(info->ch_conf), 16, exit);
    SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_FAULT_RESPONSE_INFO, &(info->fault_response), 17, exit);
    SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_IPG_COMPRESS, &(info->ipg), 14, exit);

    if ((info->ch_conf.enable)  &&
        (info->preamble.enable) &&
        (info->preamble.size == 0))
    {
      if ((info->preamble.skip_SOP) &&
          (info->ch_conf.bct_channel_byte_ndx != 0x0))
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_PREAMBLE_AND_BCT_CONF_CONFLICT_ERR, 40, exit)
      }

      if ((!(info->preamble.skip_SOP)) &&
          (info->ch_conf.bct_channel_byte_ndx != 0x1))
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_PREAMBLE_AND_BCT_CONF_CONFLICT_ERR, 50, exit)
      }
    }

    if (info->ch_conf.enable)
    {
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

  if (!info->link_partner_double_size_bus)
  {
    if ((info->is_double_size_sop_even_only) || (info->is_double_size_sop_odd_only))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_SPAUI_DS_CONF_NON_DS_BUS_ERR, 95, exit);
    }
  }
  else
  {
    if ((info->is_double_size_sop_even_only) && (info->is_double_size_sop_odd_only))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NIF_SPAUI_DS_SOP_ODD_EVEN_ERR, 100, exit);
    }
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_SPAUI_INFO_verify()",0,0);
}

uint32
  SOC_PB_NIF_GMII_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_GMII_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if ((info->enable_rx)|| (info->enable_tx))
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->mode, SOC_PB_NIF_GMII_INFO_MODE_MAX, SOC_PB_NIF_GMII_INFO_MODE_OUT_OF_RANGE_ERR, 10, exit);
    SOC_SAND_ERR_IF_ABOVE_MAX(info->rate, SOC_PB_NIF_GMII_INFO_RATE_MAX, SOC_PB_NIF_GMII_INFO_RATE_OUT_OF_RANGE_ERR, 20, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_GMII_INFO_verify()",0,0);
}

uint32
  SOC_PB_NIF_GMII_STAT_verify(
    SOC_SAND_IN  SOC_PB_NIF_GMII_STAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->fault_type, SOC_PB_NIF_GMII_STAT_FAULT_TYPE_MAX, SOC_PB_NIF_FR_REMOTE_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_GMII_STAT_verify()",0,0);
}

uint32
  SOC_PB_NIF_ILKN_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_ILKN_INFO_verify()",0,0);
}

uint32
  SOC_PB_NIF_ILKN_INT_verify(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_ILKN_INT_verify()",0,0);
}

uint32
  SOC_PB_NIF_ILKN_STAT_verify(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_STAT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_ILKN_STAT_verify()",0,0);
}

uint32
  SOC_PB_NIF_ILKN_DIAG_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_DIAG_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_ILKN_INT, &(info->interrupt), 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_ILKN_STAT, &(info->stat), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_ILKN_DIAG_INFO_verify()",0,0);
}

uint32
  SOC_PB_NIF_STAT_LINK_ACTIVITY_verify(
    SOC_SAND_IN  SOC_PB_NIF_STAT_LINK_ACTIVITY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_STAT_LINK_ACTIVITY_verify()",0,0);
}

uint32
  SOC_PB_NIF_ACTIVITY_STATUS_verify(
    SOC_SAND_IN  SOC_PB_NIF_ACTIVITY_STATUS *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_NIF_STAT_LINK_ACTIVITY, &(info->status), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_ACTIVITY_STATUS_verify()",0,0);
}

uint32
  SOC_PB_NIF_ACTIVITY_STATUS_ALL_verify(
    SOC_SAND_IN  SOC_PB_NIF_ACTIVITY_STATUS_ALL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_ACTIVITY_STATUS_ALL_verify()",0,0);
}

uint32
  SOC_PB_NIF_LINK_STATUS_verify(
    SOC_SAND_IN  SOC_PB_NIF_LINK_STATUS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_LINK_STATUS_verify()",0,0);
}

uint32
  SOC_PB_NIF_SYNCE_CLK_verify(
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->clk_divider, SOC_PB_NIF_SYNCE_CLK_CLK_DIVIDER_MAX, SOC_PB_NIF_CLK_DIVIDER_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_SYNCE_CLK_verify()",0,0);
}

uint32
  SOC_PB_NIF_PTP_CLK_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_PTP_CLK_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_PTP_CLK_INFO_verify()",0,0);
}

uint32
  SOC_PB_NIF_PTP_CLK_RST_VAL_verify(
    SOC_SAND_IN  SOC_PB_NIF_PTP_CLK_RST_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_PTP_CLK_RST_VAL_verify()",0,0);
}

uint32
  SOC_PB_NIF_PTP_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_PTP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->wire_delay_ns, SOC_PB_NIF_WIRE_DELAY_NS_MAX, SOC_PB_NIF_WIRE_DELAY_NS_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_PTP_INFO_verify()",0,0);
}

uint32
  SOC_PB_NIF_ELK_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_ELK_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_pb_nif_mal_id_verify(info->mal_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_ELK_INFO_verify()",0,0);
}

uint32
  SOC_PB_NIF_FATP_MODE_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mode, SOC_PB_FATP_MODE_MAX, SOC_PB_FATP_MODE_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_FATP_MODE_INFO_verify()",0,0);
}

uint32
  SOC_PB_NIF_FATP_INFO_verify(
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE mode,
    SOC_SAND_IN  SOC_PB_NIF_FATP_INFO *info
  )
{
  uint32
    max_nof_fatp,
    max_nof_ports_per_fatp = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->base_port_id, SOC_PB_NIF_FATP_PORT_MIN, SOC_PB_NIF_FATP_PORT_MAX, SOC_PB_NIF_FATP_BASE_PORT_ID_OUT_OF_RANGE_ERR, 11, exit);

  soc_pb_nif_fatp_mode2count(mode, &max_nof_fatp, &max_nof_ports_per_fatp);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_ports, max_nof_ports_per_fatp, SOC_PB_NIF_FATP_NOF_PORTS_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_NIF_FATP_INFO_verify()",mode,max_nof_ports_per_fatp);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

