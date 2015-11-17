/* $Id: soc_pb_api_mgmt.h,v 1.7 Broadcom SDK $
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

#ifndef __SOC_PB_API_MGMT_INCLUDED__
/* { */
#define __SOC_PB_API_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/petra_api_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_api_nif.h>
#include <soc/dpp/Petra/PB_TM/pb_api_stat_if.h>
#include <soc/dpp/Petra/PB_TM/pb_api_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_api_flow_control.h>
#include <soc/dpp/Petra/PB_TM/pb_api_ports.h>

#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximal number of OFP groups. Since each group is at
 *     least 2 OFPs, at most 40 groups can be defined          */

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

typedef enum
{
  /*
   * FTMH load balancing extension is disabled.
   */
  SOC_PB_MGMT_FTMH_LB_EXT_MODE_DISABLED = 0,
  /*
   * FTMH load balancing extension is enabled, and contains an 8-bit
   * load balancing key and an 8-bit stacking route history bitmap.
   */
  SOC_PB_MGMT_FTMH_LB_EXT_MODE_8B_LB_KEY_8B_STACKING_ROUTE_HISTORY = 1,
  /*
   * FTMH load balancing extension is enabled, and contains a 16-bit
   * stacking route history.
   */
  SOC_PB_MGMT_FTMH_LB_EXT_MODE_16B_STACKING_ROUTE_HISTORY = 2,
  /*
   *  Number of types in SOC_PB_MGMT_FTMH_LB_EXT_MODE
   */
  SOC_PB_MGMT_NOF_FTMH_LB_EXT_MODES = 3

} SOC_PB_MGMT_FTMH_LB_EXT_MODE;

/* $Id: soc_pb_api_mgmt.h,v 1.7 Broadcom SDK $
 * Device Core Frequency given in high resolution.
 * Self-Device frequency can also be given as part of SOC_PETRA_HW_ADJ_CORE_FREQ
 * structure, in MHz resolution.
 * For some applications, e.g. hybrid systems containing devices with different core frequency,
 * or devices configured to non-standard frequency (standard frequencies for Soc_petra-B are 250MHz and 300MHz),
 * higher resolution is required.
 * If high-resolution core frequency is explicitly specified by the user (by initializing this structure to non-default value),
 * this value overrides the low-resulution frequency specified in SOC_PETRA_HW_ADJ_CORE_FREQ structure.
 */
typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  /*
   * Core frequency of the current device. Units: KHz. Range: 100,000 - 500,000
   */
  uint32 self_freq;

  /*
   * The minimal core frequency of any FAP device in system. Units: KHz. Range: 100,000 - 500,000
   */
  uint32 min_fap_freq_in_system;

}SOC_PB_MGMT_CORE_FREQ_HI_RESOLUTION;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable Packet Processing features. Valid only for
   *  revisions that support Packet Processing.
   */
  uint8 pp_enable;
  /*
   *  If TRUE, fap20 devices exist in the system. This imposes
   *  certain limitations on the device behavior (e.g. fabric
   *  cells must be fixed size, fap20-compatible fabric header
   *  must be used etc.).
   */
  uint8 is_fap20_in_system;
  /*
   *  If TRUE, fap21 devices exist in the system. This imposes
   *  certain limitations on the device behavior (e.g. fabric
   *  cells must be fixed size etc.).
   */
  uint8 is_fap21_in_system;
  /*
   *  If TRUE, Soc_petra Rev-A devices exist in the system. This imposes
   *  certain limitations on the device behavior (e.g. PPH is in
   *  Soc_petra-A compatible mode).
   */
  uint8 is_petra_rev_a_in_system;
  /*
   *  If TRUE, the system uses SOC_SAND_FE200 fabric.
   */
  uint8 is_fe200_fabric;
  /*
   *  If TRUE, the system uses FE1600 fabric.
   */
  uint8 is_fe1600_in_system;
  /*
   *  Fixed/variable size cells, enable/disable packet
   *  segmentation.
   */
  SOC_PETRA_FABRIC_CELL_FORMAT fabric_cell_format;
  /*
   *  Out of total of 15 SerDes quartets, two (one per
   *  internal NIF Group consisting of 4 MALs) may be assigned
   *  to either Network or Fabric interfaces.
   */
  uint8 is_combo_nif[SOC_PETRA_COMBO_NOF_QRTTS];
  /*
   *  SerDes Reference Clocks. Three ref-clocks are
   *  configured: 1. Towards NIF 2. Towards Fabric MAC3.
   *  Shared (Combo, either to NIF or MAC) - either to NIF or
   *  MAC value, according to the shared_quartet
   *  configuration.
   */
  SOC_PETRA_MGMT_SRD_REF_CLKS ref_clocks_conf;
  /*
   *  Enable/disable statistics tag.
   */
  uint8 stag_enable;
  /*
   *  This configuration affects the maximal number of Egress
   *  MC groups that can be opened. Can only be enabled (TRUE)
   *  when working with SOC_SAND_FE600 (not SOC_SAND_FE200 or mesh). If TRUE, up
   *  to 16K Egress MC groups can be opened. Otherwise - up to
   *  8K. The configuration must be consistant with the SOC_SAND_FE600
   *  device configuration. Note! If enabled, the FAP-IDs range
   *  in the system is limited to 0 - 511.
   */
  uint8 egr_mc_16k_groups_enable;
  /*
   *  TDM traffic mode and FTMH format configuration.
   */
  SOC_PETRA_MGMT_TDM_MODE tdm_mode;
  /*
   *  FTMH load balancing mode.
   */
  SOC_PB_MGMT_FTMH_LB_EXT_MODE ftmh_lb_ext_mode;

  /*
   * If TRUE, eep extension is added to PPH header. This field is
   * valid only when working in soc_petra-B mode (is_petra_rev_a_in_system == FALSE),
   * and PP is enabled
   */
  uint8 add_pph_eep_ext;

 /*
  * Device Core Frequency given in high resolution.
  * Self-Device frequency can also be given as part of SOC_PETRA_HW_ADJ_CORE_FREQ
  * structure, in MHz resolution.
  * For some applications, e.g. hybrid systems containing devices with different core frequency,
  * or devices configured to non-standard frequency (standard frequencies for Soc_petra-B are 250MHz and 300MHz),
  * higher resolution is required.
  * If high-resolution core frequency is explicitly specified by the user (by initializing this structure to non-default value),
  * this value overrides the low-resulution frequency specified in SOC_PETRA_HW_ADJ_CORE_FREQ structure.
  */
  SOC_PB_MGMT_CORE_FREQ_HI_RESOLUTION core_freq_high_res;

} SOC_PB_MGMT_OPERATION_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  PP Port index to configure. Range: 0 - 63.
   */
  uint32 profile_ndx;
  /*
   *  PP Port configuration.
   */
  SOC_PB_PORT_PP_PORT_INFO conf;

} SOC_PB_INIT_PP_PORT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  FAP Port index to configure. Range: 0 - 79.
   */
  uint32 port_ndx;
  /*
   *  PP Port for this TM Port.
   */
  uint32 pp_port;

} SOC_PB_INIT_PP_PORT_MAP;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  FAP Port index to configure. Range: 0 - 79.
   */
  uint32 port_ndx;
  /*
   *  Egress queue priority profile index. Range: 0 - 3.
   */
  uint32 conf;

} SOC_PB_INIT_EGR_Q_PROFILE_MAP;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Number of entries to configure - FAP ports packet header
   *  type. Range: 0 - 80.
   */
  uint32 hdr_type_nof_entries;
  /*
   *  FAP ports packet header type - RAW/TM/Eth/Programmable
   *  configuration.
   */
  SOC_PETRA_INIT_PORT_HDR_TYPE hdr_type[SOC_PETRA_NOF_FAP_PORTS];
  /*
   *  Number of entries to configure - FAP ports to NIF
   *  mapping. Range: 0 - 80.
   */
  uint32 if_map_nof_entries;
  /*
   *  FAP ports to Interface mapping configuration.
   */
  SOC_PETRA_INIT_PORT_TO_IF_MAP if_map[SOC_PETRA_NOF_FAP_PORTS];
  /*
   *  Number of PP Port entries. Range: 0 - 64.
   */
  uint32 pp_port_nof_entries;
  /*
   *  PP Port. Each TM port must be mapped to a valid PP Port.
   */
  SOC_PB_INIT_PP_PORT pp_port[SOC_PB_PORT_NOF_PP_PORTS];
  /*
   *  Number of entries to configure - TM Ports to profile
   *  mapping.
   */
  uint32 tm2pp_port_map_nof_entries;
  /*
   *  TM Ports to PP Port mapping configuration. Range: 0 -
   *  80.
   */
  SOC_PB_INIT_PP_PORT_MAP tm2pp_port_map[SOC_PETRA_NOF_FAP_PORTS];
  /*
   *  Number of entries to configure - Egress queue profile
   *  mapping. Range: 0 - 80.
   */
  uint32 egr_q_profile_map_nof_entries;
  /*
   *  Egress queue profile mapping configuration.
   */
  SOC_PB_INIT_EGR_Q_PROFILE_MAP egr_q_profile_map[SOC_PETRA_NOF_FAP_PORTS];
  /*
   *  OTMH extensions enables (valid only for outgoing TM ports)
   */
  SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN otmh_ext_en[SOC_PETRA_NOF_FAP_PORTS];

} SOC_PB_INIT_PORTS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  MAC lane index. Range: 0 - 15.
   */
  uint32 mal_ndx;
  /*
   *  Basic per-mal configuration - NIF type,
   *  topology-related.
   */
  SOC_PB_NIF_MAL_BASIC_INFO conf;

} SOC_PB_HW_ADJ_MAL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Interface lane index.
   */
  SOC_PETRA_INTERFACE_ID if_ndx;
  /*
   *  SPAUI extensions configuration
   */
  SOC_PB_NIF_SPAUI_INFO conf;

} SOC_PB_HW_ADJ_SPAUI;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Network Interface index, SGMII/QSGMII. Range:
   *  SOC_PB_NIF_ID(SGMII, 0 - 31) or SOC_PB_NIF_ID(QSGMII, 0 - 63)
   */
  SOC_PETRA_INTERFACE_ID nif_ndx;
  /*
   *  SGMII configuration
   */
  SOC_PB_NIF_GMII_INFO conf;

} SOC_PB_HW_ADJ_GMII;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Fat Pipe index. Range: 0 - 2.
   */
  SOC_PB_NIF_FATP_ID fatp_ndx;
  /*
   *  Fat Pipe configuration
   */
  SOC_PB_NIF_FATP_INFO conf;

} SOC_PB_HW_ADJ_FATP;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Interlaken index. Range: 0 - 2.
   */
  SOC_PB_NIF_ILKN_ID ilkn_ndx;
  /*
   *  Interlaken configuration
   */
  SOC_PB_NIF_ILKN_INFO conf;

} SOC_PB_HW_ADJ_ILKN;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  External lookup configuration
   */
  SOC_PB_NIF_ELK_INFO conf;

} SOC_PB_HW_ADJ_ELK;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the Synchronous Ethernet pins of MALG-B can be
   *  used (4 SYNCE signals in total). Otherwise - only 2
   *  SYNCE signals can be used. Note: if TRUE, not fully
   *  compatible with Soc_petra-A pinout (override VSS pins).
   */
  uint8 is_malg_b_enabled;
  /*
   *  Synchronous Ethernet Signal Mode
   */
  SOC_PB_NIF_SYNCE_MODE mode;
  /*
   *  Synchronous Ethernet configuration
   */
  SOC_PB_NIF_SYNCE_CLK conf[SOC_PB_NIF_NOF_SYNCE_CLK_IDS];

} SOC_PB_HW_ADJ_SYNCE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 /*
  * MDIO frequency. Units: KHz.
  */
  uint32 clk_freq_khz;

} SOC_PB_HW_ADJ_MDIO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Number of entries: per-MAC lane basic configuration.
   *  Range: 0 - 16.
   */
  uint32 mal_nof_entries;
  /*
   *  MAL basic configuration, including interface type and
   *  topology (serdes)-related.
   */
  SOC_PB_HW_ADJ_MAL mal[SOC_PB_NOF_MAC_LANES];
  /*
   *  Number of entries: SPAUI/RSPAUI extensions
   *  configuration. Range: 0 - 16.
   */
  uint32 spaui_nof_entries;
  /*
   *  SPAUI/RSPAUI extensions configuration. If enabled,
   *  configured in both RX and TX directions.
   */
  SOC_PB_HW_ADJ_SPAUI spaui[SOC_PB_NOF_MAC_LANES];
  /*
   *  Number of entries: SGMII/QSGMII configuration. Range: 0
   *  - 64.
   */
  uint32 gmii_nof_entries;
  /*
   *  SGMII/QSGMII configuration. If enabled, configured in
   *  both RX and TX directions.
   */
  SOC_PB_HW_ADJ_GMII gmii[SOC_PB_NIF_NOF_NIFS];
  /*
   *  Number of entries: Interlaken configuration. Range: 0 -
   *  2.
   */
  uint32 ilkn_nof_entries;
  /*
   *  Interlaken configuration
   */
  SOC_PB_HW_ADJ_ILKN ilkn[SOC_PB_NIF_NOF_ILKN_IDS];
  /*
   *  Number of entries: Fat pipe configuration. Range: 0 - 3.
   */
  uint32 fatp_nof_entries;
  /*
   *  Fat-Pipe Mode: 1x12, 2x6 or 3x4.
   */
  SOC_PB_NIF_FATP_MODE_INFO fatp_mode;
  /*
   *  Fat pipe configuration.
   */
  SOC_PB_HW_ADJ_FATP fatp[SOC_PB_NIF_NOF_FATP_IDS];
  /*
   *  Number of entries: ELK configuration. Range: 0 - 1.
   */
  uint32 elk_nof_entries;
  /*
   *  ELK configuration
   */
  SOC_PB_HW_ADJ_ELK elk;
  /*
   *  Number of entries: Synchronous Ethernet
   *  configuration. Range: 0 - 1.
   */
  uint32 synce_nof_entries;
  /*
   *  Synchronous Ethernet configuration.
   */
  SOC_PB_HW_ADJ_SYNCE synce;
  /*
   *  Number of entries: MDIO configuration. Range: 0 - 1
   */
  uint32 mdio_nof_entries;
  /*
   *  MDIO configuration.
   */
  SOC_PB_HW_ADJ_MDIO mdio;

} SOC_PB_HW_ADJ_NIF;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/disable Flow Control NIF Oversubscription Scheme
   *  configuration
   */
  uint8 enable;
  /*
   *  Flow Control NIF Oversubscription Scheme configuration
   */
  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME conf;

} SOC_PB_HW_ADJ_FC_SCHEME;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Statistics interface configuration.
   */
  SOC_PETRA_STAT_IF_INFO if_conf;
  /*
   *  Statistics interface report configuration.
   */
  SOC_PB_STAT_IF_REPORT_INFO rep_conf;

} SOC_PB_HW_ADJ_STAT_IF;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If set, the CPU streaming IF is in Multi-Port Mode.
   *  Otherwise, the CPU is in Single-Port Mode.
   */
  uint8 multi_port_mode;
  /*
   *  If set, the CSI time-out counter is activated and
   *  the CSI will send a read reply command back to
   *  the CPU after timeout_prd cycles, if no read
   *  reply was received from the Soc_petra blocks.
   */
  uint8 enable_timeoutcnt;
  /*
   *  Number of cycles the CSI waits for a read reply from
   *  the Soc_petra blocks before issuing a read reply command.
   */
  uint32 timeout_prd;
  /*
   *  If set, the CSI will not send a reply command for
   *  write requests. As for read requests, the CSI will
   *  send a 32b reply command containing the read data only.
   */
  uint8 quiet_mode;
  /*
   *  If set, the CSI does not discard data received with a
   *  parity error and treats it as valid data. Default is
   *  to set this register to assist in the bring-up phase.
   *  The application should clear this register after the
   *  CPU interface is working.
   */
  uint8 discard_bad_parity;
  /*
   *  If set, disables transmitting packets over the streaming
   *  interface. These packets can be read through the
   *  cpu_asynchronous_packet_data address.
   */
  uint8 discard_pkt_streaming;

}SOC_PB_HW_ADJ_STREAMING_IF;

/*
 * New (Soc_petra-B) DRMA PLL.
 * The DRAM frequency is derived from the following formula:
 * F-out = F-Ref*(2*(f+1)/[(r+1)*2^q]), where F-out is twice the DRAM
 * frequency.
 * Limitations:
 *  75MHz      <= F-ref          <=    175MHz
 *  25MHz         <= F-ref/r+1 <=    45MHz
 *  1600MHz <= F-vco          <=    3200MHz
 */
typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  /*
   *  Range: 0 - 7.
   */
  uint32  r;
  /*
   *  Range: 17 - 63.
   */
  uint32  f;
  /*
   *  Range: 1 - 4.
   */
  uint32  q;
}SOC_PB_HW_ADJ_DRAM_PLL;

typedef enum
{
  /*
   * QDR type 2. This is the default QDR type,
   * and can typically be used also for QDR type 2-plus and
   * QDR type 3
   */
  SOC_PB_HW_QDR_TYPE_QDR = 0,
  /*
   * QDR type 2-plus. Choosing this value
   * may be needed if using this QDR type
   */
  SOC_PB_HW_QDR_TYPE_QDR2P = 1,
  /*
   * QDR type 3. Choosing this value
   * may be needed if using this QDR type
   */
  SOC_PB_HW_QDR_TYPE_QDR3 = 2,
  SOC_PB_HW_QDR_NOF_TYPES = 3
} SOC_PB_HW_QDR_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/disable QDR configuration.
   */
  uint8 enable;
  /*
   *  Select Parity or ECC protection type.
   */
  SOC_PETRA_HW_QDR_PROTECT_TYPE protection_type;
  /*
   *  If TRUE, the 250Mhz Core clock is used as QDR reference
   *  clock. Otherwise (lower frequency) - QDR clock is used.
   *  In the later case, pll configuration must be set.
   */
  uint8 is_core_clock_freq;
  /*
   *  QDR Pll configuration as derived from the QDR reference
   *  clock. Note: this field is only relevant if
   *  is_core_clock_freq is FALSE - ignored otherwise.
   *  CAUTION: it is a misconfiguration to use QDR clock,
      configured to the Core clock frequency (250Mhz) or above!
   */
  SOC_PETRA_HW_PLL_PARAMS pll_conf;
  /*
   *  Total QDR SRAM memory size Units: Mbits.
   */
  SOC_PETRA_HW_QDR_SIZE_MBIT qdr_size_mbit;

  SOC_PB_HW_QDR_TYPE qdr_type;
}SOC_PB_HW_ADJ_QDR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  DRAM configuration. DRAM is used by the device to store
   *  packets at the ingress.
   */
  SOC_PETRA_HW_ADJ_DDR    dram;

/*
 * New (Soc_petra-B) DRMA PLL.
 * Must be set for Soc_petra-B revision A1 and above.
 * In this case, the SOC_PETRA_HW_PLL_PARAMS structure of the dram HW_ADJUSTMENTS
 * configuration is ignored
 * The DRAM frequency is derived from the following formula:
 * F-out = F-Ref*(2*(f+1)/[(r+1)*2^q]), where F-out is twice the DRAM
 * frequency.
 * Limitations:
 *  75MHz      <= F-ref          <=    175MHz
 *  25MHz         <= F-ref/r+1 <=    45MHz
 *  1600MHz <= F-vco          <=    3200MHz
 */
  SOC_PB_HW_ADJ_DRAM_PLL  dram_pll;
  /*
   *  QDR configuration. QDR is used by the device to store
   *  packet buffer descriptors and per-packet information at
   *  the ingress.
   */
  SOC_PB_HW_ADJ_QDR qdr;
  /*
   *  SerDes physical parameters configuration.
   */
  SOC_PETRA_HW_ADJ_SERDES serdes;
  /*
   *  Fabric configuration.
   */
  SOC_PETRA_HW_ADJ_FABRIC fabric;
  /*
   *  Network Interfaces configuration.
   */
  SOC_PB_HW_ADJ_NIF nif;
  /*
   *  Core clock frequency configuration.
   */
  SOC_PETRA_HW_ADJ_CORE_FREQ core_freq;
  /*
   *  Statistics interface configuration.
   */
  SOC_PB_HW_ADJ_STAT_IF stat_if;
  /*
   *  Statistics interface configuration.
   */
  SOC_PB_HW_ADJ_STREAMING_IF streaming_if;
  /*
   *  Flow Control Scheme configuration.
   */
  SOC_PB_HW_ADJ_FC_SCHEME fc_scheme;

} SOC_PB_HW_ADJUSTMENTS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Statistics Tag field configuration - position and size
   *  inside the packet
   */
  SOC_PB_ITM_STAG_INFO encoding;

} SOC_PB_INIT_STAG_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/disable credit worth configuration.
   */
  uint8 credit_worth_enable;
  /*
   *  Bytes-worth of a single credit. Units: bytesRange: 256 -
   *  8K. Resolution: 1 byte
   */
  uint32 credit_worth;
  /*
   *  Enable/disable Statistics Tag configuration
   */
  uint8 stag_enable;
  /*
   *  Statistics Tag configuration - in-packet position and
   *  size and inner encoding.
   */
  SOC_PB_INIT_STAG_INFO stag;

} SOC_PB_INIT_BASIC_CONF;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Number of entries to configure. Range: 0 - 64.
   */
  uint32 nof_entries;
  /*
   *  The index of the NIF to be activated. The appropriate
   *  MAL is taken out-of-reset.
   */
  SOC_PETRA_INTERFACE_ID nif_id_active[SOC_PB_NIF_NOF_NIFS_MAX];

} SOC_PB_INIT_OOR;

typedef enum
{
  /*
   *  Static routing for TDM multicast cells is disabled.
   */
  SOC_PB_INIT_MC_STATIC_ROUTE_MODE_DIS = 0,
  /*
   *  In this mode, the 4 MSB of the Multicast ID corresponds
   *  to the selected route and the 12 LSB to the Egress
   *  Multicast ID. A partial number of Multicast IDs (12 K) is
   *  available then.
   */
  SOC_PB_INIT_MC_STATIC_ROUTE_MODE_PARTIAL = 1,
  /*
   *  In this mode, the 4 MSB of the Multicast ID corresponds
   *  to the selected route and the 13 LSB to the Egress
   *  Multicast ID. A partial number of Multicast IDs (8K) is
   *  available then. The bits 12:10 of the Multicast-ID have 
   *  a double meaning (Route number and Multicast-ID MSBs).
   */
  SOC_PB_INIT_MC_STATIC_ROUTE_MODE_PARTIAL_COMMON = 2,
  /*
   *  In this mode, the 4 MSB of the Multicast ID corresponds
   *  to the selected route and all the 14 bits to the Egress
   *  Multicast ID. All multicast IDs (16K) can be used as long
   *  as the 4 MSBs have double meanings (Route number and
   *  Multicast-ID MSBs).
   */
  SOC_PB_INIT_MC_STATIC_ROUTE_MODE_FULL = 3,
  /*
   *  Number of types in SOC_PB_INIT_MC_STATIC_ROUTE_MODE
   */
  SOC_PB_INIT_NOF_MC_STATIC_ROUTE_MODES = 4
}SOC_PB_INIT_MC_STATIC_ROUTE_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If set, all Soc_petra-B rev. B0 bugfixes and errata
   *  corrections are enabled. Otherwise, the device behaves
   *  as a Soc_petra-B rev. A1 device.
   */
  uint8 bugfixes_enabled;
  /*
   *  Encapsulation type of inner Ethernet frames (default:
   *  Ethernet II).
   */
  SOC_SAND_PP_ETH_ENCAP_TYPE inner_eth_encap;
  /*
   *  TDM static route mode for multicast cells.
   */
  SOC_PB_INIT_MC_STATIC_ROUTE_MODE tdm_mc_route_mode;
  /*
   *  If set, EGQ-OFP flow control signals of OFPs 0 - 63 are
   *  connected to 128 aggregate-level HR scheduling elements.
   *  Otherwise the signals are connected to their respective
   *  port HR scheduling elements.
   */
  uint8 is_fc_sch_mode_cbfc;
  /*
   *  This 16-bit bitmap corresponds to the 16K Multicast-ID
   *  range divided in 16 ranges of 1K Multicast-ID. For each
   *  range, setting its bit means that all the ingress
   *  Multicast groups in this range are used for Fabric
   *  Multicast (single replication per Multicast Group),
   *  in particular in case of Enhanced Fabric Multicast
   *  Queue configuration. This feature optimizes the Hardware
   *  Multicast resources.
   *  The user must verify that no Ingress Multicast-ID with
   *  multiple replications is configured in a range prior to
   *  setting its bit.
   */
  uint32 single_copy_mc_rng_bmp;

} SOC_PB_MGMT_B0_INFO;


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
 *   soc_pb_register_device
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This procedure registers a new device to be taken care
 *   of by this device driver. Physical device must be
 *   accessible by CPU when this call is made..
 * INPUT:
 *     uint32                                 *base_address -
 *     Base address of direct access memory assigned for
 *     device's registers. This parameter needs to be specified
 *     even if physical access to device is not by direct
 *     access memory since all logic, within driver, up to
 *     actual physical access, assumes 'virtual' direct access
 *     memory. Memory block assigned by this pointer must not
 *     overlap other memory blocks in user's system and
 *     certainly not memory blocks assigned to other SOC_PETRA
 *     devices using this procedure.
 *   SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR     reset_device_ptr -
 *     BSP-function for device reset. Refer to
 *     'SOC_SAND_RESET_DEVICE_FUNC_PTR' definition.
 *   SOC_SAND_OUT uint32                      *unit_ptr -
 *     This procedure loads pointed memory with identifier of
 *     newly added device. This identifier is to be used by the
 *     caller for further accesses to this device..
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_register_device(
             uint32                       *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR     reset_device_ptr,
    SOC_SAND_OUT int                      *unit_ptr
  );

/*********************************************************************
* NAME:
 *   soc_pb_unregister_device
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Undo soc_petra_register_device()
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     The device ID to be unregistered.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_unregister_device(
    SOC_SAND_IN  int                      unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_mgmt_operation_mode_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set soc_petra device operation mode. This defines
 *   configurations, such as support for certain header
 *   types, etc.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_MGMT_OPERATION_MODE         *op_mode -
 *     Device operation mode.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mgmt_operation_mode_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_MGMT_OPERATION_MODE         *op_mode
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_mgmt_operation_mode_set" API.
 *     Refer to "soc_pb_mgmt_operation_mode_set" API for details.
*********************************************************************/
uint32
  soc_pb_mgmt_operation_mode_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_MGMT_OPERATION_MODE         *op_mode
  );

/*********************************************************************
* NAME:
 *   soc_pb_mgmt_init_sequence_phase1
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Initialize the device, including:1. Prevent all the
 *   control cells. 2. Initialize the device tables and
 *   registers to default values. 3. Initialize
 *   board-specific hardware interfaces according to
 *   configurable information, as passed in 'hw_adjust'. 4.
 *   Perform basic device initialization. The configuration
 *   can be enabled/disabled as passed in 'enable_info'.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS              *hw_adjust -
 *     Contains user-defined initialization information for
 *     hardware interfaces.
 *   SOC_SAND_IN  SOC_PB_INIT_BASIC_CONF             *basic_conf -
 *     Basic configuration that must be configured for all
 *     systems - credit worth, dram buffers configuration etc.
 *   SOC_SAND_IN  SOC_PB_INIT_PORTS                  *fap_ports -
 *     local FAP ports configuration - header parsing type,
 *     mapping to NIF etc. scheduler etc.
 *   SOC_SAND_IN  uint8                      silent -
 *     If TRUE, progress printing will be suppressed.
 * REMARKS:
 *   1. For all configurations that can be done per-direction
 *   (e.g. NIF - rx/tx, FAP - incoming/outgoing) - the
 *   configuration is performed for both directions if
 *   enabled. It may be overridden before phase2 if needed.
 *   2. For all input structures, NULL pointer may be passed.
 *   If input structure is passed as NULL, the appropriate
 *   configuration will not be performed.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mgmt_init_sequence_phase1(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_INOUT  SOC_PB_HW_ADJUSTMENTS              *hw_adjust,
    SOC_SAND_IN  SOC_PB_INIT_BASIC_CONF             *basic_conf,
    SOC_SAND_IN  SOC_PB_INIT_PORTS                  *fap_ports,
    SOC_SAND_IN  uint8                      silent
  );

/*********************************************************************
* NAME:
 *   soc_pb_mgmt_init_sequence_phase2
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Out-of-reset sequence. Enable/Disable the device from
 *   receiving and transmitting control cells.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_INIT_OOR                    *oor_info -
 *     Out-Of-Reset configuration. Some blocks need to be set
 *     out of reset before traffic can be enabled.
 *   SOC_SAND_IN  uint8                      silent -
 *     TRUE - Print progress messages. FALSE - Do not print
 *     progress messages.
 * REMARKS:
 *   1. After phase 2 initialization, traffic can be enabled.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mgmt_init_sequence_phase2(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_INIT_OOR                    *oor_info,
    SOC_SAND_IN  uint8                      silent
  );

/*********************************************************************
* NAME:
 *   soc_pb_mgmt_hw_interfaces_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Initialize a subset of the HW interfaces of the device.
 *   The function might be called more than once, each time
 *   with different fields, indicated to be written to the
 *   device
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS              *hw_adjust -
 *     BSP related information, and a valid bit for each field.
 *   SOC_SAND_IN  uint8                      silent -
 *     If TRUE, all printing will be suppressed.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mgmt_hw_interfaces_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS              *hw_adjust,
    SOC_SAND_IN  uint8                      silent
  );


/*********************************************************************
* NAME:
 *   soc_pb_mgmt_max_pckt_size_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the maximal allowed packet size. The limitation can
 *   be performed based on the packet size before or after
 *   the ingress editing (external and internal configuration
 *   mode, accordingly). Packets above the specified value
 *   are dropped.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                      port_ndx -
 *     Incoming port index. Range: 0 - 79.
 *   SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx -
 *     External mode filters the packets according to there
 *     original size. Internal mode filters the packets
 *     according to their size inside the device, after ingress
 *     editing.
 *   SOC_SAND_IN  uint32                       max_size -
 *     Maximal allowed packet size per incoming port. Packets
 *     above this value will be dropped. Units: bytes.
 * REMARKS:
 *   1. This API gives a better resolution (i.e., per
 *   incoming port) than soc_petra_mgmt_pckt_size_range_set. 2.
 *   If both APIs are used to configure the maximal packet
 *   size, the value configured is set by the API called at
 *   last.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mgmt_max_pckt_size_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  uint32                       max_size
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_mgmt_max_pckt_size_set" API.
 *     Refer to "soc_pb_mgmt_max_pckt_size_set" API for details.
*********************************************************************/
uint32
  soc_pb_mgmt_max_pckt_size_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT uint32                       *max_size
  );

/*********************************************************************
* NAME:
 *   soc_pb_mgmt_core_frequency_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the core clock frequency of the device.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT uint32                       clk_freq -
 *     Device core clock frequency. Units: MHz. Range: 150 -
 *     300.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mgmt_core_frequency_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint32                       *clk_freq
  );

/*********************************************************************
* NAME:
 *   soc_pb_mgmt_rev_b0_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Soc_petra-B B0 revision specific features.
 * INPUT:
 *   SOC_SAND_IN  int       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_MGMT_B0_INFO *info -
 *     Soc_petra-B rev. B0 revision-specific features.
 * REMARKS:
 *   Relevant for Soc_petra-B rev. B0 only.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_mgmt_rev_b0_set(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  SOC_PB_MGMT_B0_INFO *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_mgmt_rev_b0_set"
 *     API.
 *     Refer to "soc_pb_mgmt_rev_b0_set" API for details.
*********************************************************************/
uint32
  soc_pb_mgmt_rev_b0_get(
    SOC_SAND_IN  int       unit,
    SOC_SAND_OUT SOC_PB_MGMT_B0_INFO *info
  );

void
  SOC_PB_MGMT_OPERATION_MODE_clear(
    SOC_SAND_OUT SOC_PB_MGMT_OPERATION_MODE *info
  );
void
  SOC_PB_MGMT_CORE_FREQ_HI_RESOLUTION_clear(
    SOC_SAND_OUT SOC_PB_MGMT_CORE_FREQ_HI_RESOLUTION *info
  );

void
  SOC_PB_INIT_PP_PORT_clear(
    SOC_SAND_OUT SOC_PB_INIT_PP_PORT *info
  );

void
  SOC_PB_INIT_PP_PORT_MAP_clear(
    SOC_SAND_OUT SOC_PB_INIT_PP_PORT_MAP *info
  );

void
  SOC_PB_INIT_EGR_Q_PROFILE_MAP_clear(
    SOC_SAND_OUT SOC_PB_INIT_EGR_Q_PROFILE_MAP *info
  );

void
  SOC_PB_INIT_PORTS_clear(
    SOC_SAND_OUT SOC_PB_INIT_PORTS *info
  );

void
  SOC_PB_HW_ADJ_MAL_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_MAL *info
  );

void
  SOC_PB_HW_ADJ_SPAUI_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_SPAUI *info
  );

void
  SOC_PB_HW_ADJ_GMII_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_GMII *info
  );

void
  SOC_PB_HW_ADJ_FATP_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_FATP *info
  );

void
  SOC_PB_HW_ADJ_ILKN_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_ILKN *info
  );

void
  SOC_PB_HW_ADJ_ELK_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_ELK *info
  );

void
  SOC_PB_HW_ADJ_SYNCE_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_SYNCE *info
  );
void
  SOC_PB_HW_ADJ_MDIO_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_MDIO *info
  );

void
  SOC_PB_HW_ADJ_NIF_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_NIF *info
  );

void
  SOC_PB_HW_ADJ_FC_SCHEME_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_FC_SCHEME *info
  );

void
  SOC_PB_HW_ADJ_DRAM_PLL_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_DRAM_PLL *info
  );

void
  SOC_PB_HW_ADJ_STAT_IF_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_STAT_IF *info
  );

void
  SOC_PB_HW_ADJ_STREAMING_IF_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_STREAMING_IF *info
  );

void
  SOC_PB_HW_ADJUSTMENTS_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJUSTMENTS *info
  );

void
  SOC_PB_HW_ADJ_CORE_FREQ_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_CORE_FREQ *info
  );

void
  SOC_PB_INIT_STAG_INFO_clear(
    SOC_SAND_OUT SOC_PB_INIT_STAG_INFO *info
  );

void
  SOC_PB_INIT_BASIC_CONF_clear(
    SOC_SAND_OUT SOC_PB_INIT_BASIC_CONF *info
  );

void
  SOC_PB_INIT_OOR_clear(
    SOC_SAND_OUT SOC_PB_INIT_OOR *info
  );

void
  SOC_PB_HW_ADJ_QDR_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_QDR *info
  );

void
  SOC_PB_MGMT_B0_INFO_clear(
    SOC_SAND_OUT SOC_PB_MGMT_B0_INFO *info
  );

#if SOC_PB_DEBUG_IS_LVL1

void
  SOC_PB_MGMT_OPERATION_MODE_print(
    SOC_SAND_IN  SOC_PB_MGMT_OPERATION_MODE *info
  );

void
  SOC_PB_MGMT_CORE_FREQ_HI_RESOLUTION_print(
    SOC_SAND_IN  SOC_PB_MGMT_CORE_FREQ_HI_RESOLUTION *info
  );

void
  SOC_PB_INIT_PP_PORT_print(
    SOC_SAND_IN  SOC_PB_INIT_PP_PORT *info
  );

void
  SOC_PB_INIT_PP_PORT_MAP_print(
    SOC_SAND_IN  SOC_PB_INIT_PP_PORT_MAP *info
  );

void
  SOC_PB_INIT_EGR_Q_PROFILE_MAP_print(
    SOC_SAND_IN  SOC_PB_INIT_EGR_Q_PROFILE_MAP *info
  );

void
  SOC_PB_INIT_PORTS_print(
    SOC_SAND_IN  SOC_PB_INIT_PORTS *info
  );

void
  SOC_PB_HW_ADJ_MAL_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_MAL *info
  );

void
  SOC_PB_HW_ADJ_SPAUI_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_SPAUI *info
  );

void
  SOC_PB_HW_ADJ_GMII_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_GMII *info
  );

void
  SOC_PB_HW_ADJ_FATP_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_FATP *info
  );

void
  SOC_PB_HW_ADJ_ILKN_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_ILKN *info
  );

void
  SOC_PB_HW_ADJ_ELK_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_ELK *info
  );

void
  SOC_PB_HW_ADJ_SYNCE_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_SYNCE *info
  );
void
  SOC_PB_HW_ADJ_MDIO_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_MDIO *info
  );

void
  SOC_PB_HW_ADJ_QDR_print(
    SOC_SAND_IN SOC_PB_HW_ADJ_QDR *info
  );

void
  SOC_PB_HW_ADJ_NIF_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_NIF *info
  );

void
  SOC_PB_HW_ADJ_FC_SCHEME_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_FC_SCHEME *info
  );

void
  SOC_PB_HW_ADJ_DRAM_PLL_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_DRAM_PLL *info
  );

void
  SOC_PB_HW_ADJ_STAT_IF_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_STAT_IF *info
  );

void
  SOC_PB_HW_ADJ_STREAMING_IF_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_STREAMING_IF *info
  );

void
  SOC_PB_HW_ADJUSTMENTS_print(
    SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS *info
  );

void
  SOC_PB_INIT_STAG_INFO_print(
    SOC_SAND_IN  SOC_PB_INIT_STAG_INFO *info
  );

void
  SOC_PB_INIT_BASIC_CONF_print(
    SOC_SAND_IN  SOC_PB_INIT_BASIC_CONF *info
  );

void
  SOC_PB_INIT_OOR_print(
    SOC_SAND_IN  SOC_PB_INIT_OOR *info
  );

const char*
  soc_pb_PB_MGMT_FTMH_LB_EXT_MODE_to_string(
    SOC_SAND_IN  SOC_PB_MGMT_FTMH_LB_EXT_MODE enum_val
  );

const char*
  SOC_PB_INIT_MC_STATIC_ROUTE_MODE_to_string(
    SOC_SAND_IN  SOC_PB_INIT_MC_STATIC_ROUTE_MODE enum_val
  );

void
  SOC_PB_MGMT_B0_INFO_print(
    SOC_SAND_IN  SOC_PB_MGMT_B0_INFO *info
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_API_MGMT_INCLUDED__*/
#endif

