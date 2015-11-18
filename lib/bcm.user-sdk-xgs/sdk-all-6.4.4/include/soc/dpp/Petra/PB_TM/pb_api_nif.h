/* $Id: pb_api_nif.h,v 1.7 Broadcom SDK $
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

#ifndef __SOC_PB_API_NIF_INCLUDED__
/* { */
#define __SOC_PB_API_NIF_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_64cnt.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/Petra/PB_TM/pb_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Total number of NIF types                               */
#define  SOC_PB_NIF_NOF_TYPES (5)

/*     Minimal FAP Port that can be dedicated to a Fat pipe    */
#define  SOC_PB_NIF_FATP_PORT_MIN (1)

/*     Maximal FAP Port that can be dedicated to a Fat pipe    */
#define  SOC_PB_NIF_FATP_PORT_MAX (12)

/*     The size, in bytes, of resequencing index header for the
 *     Fat pipe interface. This header is added before the
 *     layer 2 header (e.g. ITMH header for TM ports). This
 *     value must be added to the ihp_port_set API.
 *     IHP_PORT_INFO parameters - sop2header_offset_bytes and
 *     strip_from_sop_bytes.                                   */

#define SOC_PB_NIF_ID_XAUI_FIRST    SOC_PETRA_NIF_ID_XAUI_0
#define SOC_PB_NIF_ID_XAUI_LAST     SOC_PETRA_NIF_ID_XAUI_7

#define SOC_PB_NIF_ID_RXAUI_FIRST   SOC_PETRA_NIF_ID_RXAUI_0
#define SOC_PB_NIF_ID_RXAUI_LAST    SOC_PETRA_NIF_ID_RXAUI_15

#define SOC_PB_NIF_ID_SGMII_FIRST   SOC_PETRA_NIF_ID_SGMII_0
#define SOC_PB_NIF_ID_SGMII_LAST    SOC_PETRA_NIF_ID_SGMII_31

#define SOC_PB_NIF_ID_QSGMII_FIRST  SOC_PETRA_NIF_ID_QSGMII_0
#define SOC_PB_NIF_ID_QSGMII_LAST   SOC_PETRA_NIF_ID_QSGMII_63

#define SOC_PB_NIF_ID_ILKN_FIRST    SOC_PETRA_NIF_ID_ILKN_0
#define SOC_PB_NIF_ID_ILKN_LAST     SOC_PETRA_NIF_ID_ILKN_1

#define SOC_PB_NIF_MALS_IN_MALG           8

/* $Id: pb_api_nif.h,v 1.7 Broadcom SDK $
 *	Number of Serdes lanes per MAC Lane
 */
#define SOC_PB_NOF_MAC_LANES              16
#define SOC_PB_SRD_LANES_PER_MAL          2
/*
 *	Number of Serdes lanes per Dual MAC Lane,
 *  i.e. a pair of MAC lanes typically (but not always)
 *  matching to the same NIF type
 */
#define SOC_PB_SRD_LANES_PER_DMAL         (2 * SOC_PB_SRD_LANES_PER_MAL)
#define SOC_PB_QSGMII_NIFS_PER_MAL        4
#define SOC_PB_MAX_NIFS_PER_MAL           SOC_PB_QSGMII_NIFS_PER_MAL
#define SOC_PB_SGMII_NIFS_PER_MAL         SOC_PB_SRD_LANES_PER_MAL

#define SOC_PB_NIF_NOF_NIFS               64
#define SOC_PB_NIF_NOF_MALGS              2
#define SOC_PB_NIFS_IN_MALG               (SOC_PB_NIF_NOF_NIFS / SOC_PB_NIF_NOF_MALGS)
#define SOC_PB_NIF_IN_MALG(nif_id)        ((nif_id)%SOC_PB_NIFS_IN_MALG)

/* Just different naming */
#define SOC_PB_NIF_NOF_NIFS_MAX           SOC_PB_NIF_NOF_NIFS
/* Just different naming */

/*
 *	An indication of invalid internal NIF index (0..63 are valid values).
 */
#define SOC_PB_NIF_INVALID_VAL_INTERN SOC_PETRA_NIF_ID_NONE

/* } */
/*************
 * MACROS    *
 *************/
/* { */
/*
 *	Get per-type NIF-index.
 *  For example, SOC_PB_NIF_ID(XAUI, 3) is
 *  SOC_PETRA_NIF_ID_XAUI_3
 */
#define SOC_PB_NIF_ID(tp,id) \
  (SOC_PB_NIF_ID_##tp##_FIRST + (id))

/*
 *	Check if a NIF-id belongs to a specific NIF type
 */
#define SOC_PB_NIF_IS_TYPE_ID(tp,id) \
  SOC_SAND_NUM2BOOL(SOC_SAND_IS_VAL_IN_RANGE(id, SOC_PB_NIF_ID_##tp##_FIRST, SOC_PB_NIF_ID_##tp##_LAST))

/*
 *	Get the offset-index inside a certain NIF type
 */
#define SOC_PB_NIF_ID_OFFSET(tp,id) \
  SOC_SAND_DELTA((int32)(id), (int32)SOC_PB_NIF_ID_##tp##_FIRST)

/*
 *	Get the maximal number of interfaces of a certain NIF type
 */

/*
 *	Soc_petra-A style indexing (0 - 31)
 */
#define SOC_PB_NIF_IS_PA_ID(id) \
  SOC_SAND_IS_VAL_IN_RANGE(id, SOC_PETRA_IF_ID_0, SOC_PETRA_IF_ID_31)

/*
 *	Soc_petra-B style indexing (type, in-type-id)
 */
#define SOC_PB_NIF_IS_PB_ID(id)\
  SOC_SAND_NUM2BOOL(SOC_PB_NIF_IS_TYPE_ID(XAUI,id)  || SOC_PB_NIF_IS_TYPE_ID(RXAUI,id)  || \
   SOC_PB_NIF_IS_TYPE_ID(SGMII,id) || SOC_PB_NIF_IS_TYPE_ID(QSGMII,id) || \
   SOC_PB_NIF_IS_TYPE_ID(ILKN,id))

#define SOC_PB_NIF_IS_VALID_ID(id) \
  SOC_SAND_NUM2BOOL(SOC_PB_NIF_IS_PA_ID(id) || SOC_PB_NIF_IS_PB_ID(id))

/*
 *	Even (primary) MAL in a Dual MAL (pair of MALs)
 */
#define SOC_PB_NIF_BASE_MAL(mal_id)         ((mal_id) & ~0x1)
/*
 *	Odd (secondary) MAL in a Dual MAL (pair of MALs)
 */
#define SOC_PB_NIF_SCND_MAL(mal_id)         ((mal_id) |  0x1)
#define SOC_PB_NIF_IS_BASE_MAL(mal_id)      SOC_SAND_NUM2BOOL((mal_id) == SOC_PB_NIF_BASE_MAL(mal_id))
#define SOC_PB_NIF_IS_SCND_MAL(mal_id)      SOC_SAND_NUM2BOOL((mal_id) == SOC_PB_NIF_SCND_MAL(mal_id))
#define SOC_PB_NIF_MAL2MALG_ID(mal_id)      ((mal_id)/SOC_PB_NIF_MALS_IN_MALG)
#define SOC_PB_NIF_MAL2INNER_ID(mal_id)     ((mal_id)%SOC_PB_NIF_MALS_IN_MALG)
#define SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id)  ((mal_id)*SOC_PB_MAX_NIFS_PER_MAL)
#define SOC_PB_MAL2NIF_INNER_ID(mal_id)     (SOC_PB_NIF_MAL2INNER_ID(mal_id)*SOC_PB_MAX_NIFS_PER_MAL)
#define SOC_PB_NIF2MAL_GLBL_ID(nif_id)      ((nif_id)/SOC_PB_MAX_NIFS_PER_MAL)
#define SOC_PB_MAL2NIF_GLBL_ID(mal_id)      (SOC_PB_NIF_MAL2NIF_BASE_ID(mal_id))

#define SOC_PB_NIF_BOTH_DIRECTIONS(drct_id) \
  for(drct_id = SOC_PB_REG_NIF_DIRECTION_RX; drct_id <= SOC_PB_REG_NIF_DIRECTION_TX; drct_id++)

/*
 *	GMII
 */
/* TRUE for even id-s. Relevant for nif_port_id indexing 0 .. 63 */
#define SOC_PB_NIF_IS_SGMII_NIF_P_ID(nif_port_id) \
  ( ((nif_port_id) % SOC_PB_SGMII_NIFS_PER_MAL) == 0) ? TRUE:FALSE)

#define SOC_PB_NIF_IS_GMII_ONLY_ID(nif_id) \
  SOC_SAND_NUM2BOOL_INVERSE(((nif_id) % SOC_PB_MAX_NIFS_PER_MAL) == 0)

/* nif_ndx: SGMII_0 .. SGMII_31 */
#define SOC_PB_NIF_SGMII_LANE(nif_ndx) \
  (SOC_PB_NIF_ID_OFFSET(SGMII, nif_ndx) % SOC_PB_SGMII_NIFS_PER_MAL)

/* nif_ndx: QSGMII_0 .. QSGMII_63 */

/* nif_ndx: QSGMII_0 .. QSGMII_63 or SGMII_0 .. SGMII_31 */


#define SOC_PB_NIF_GMII_LANE_INTERN(nif_id) \
  ((nif_id) % SOC_PB_MAX_NIFS_PER_MAL)

#define SOC_PB_SGMII2GLBL_ID(sgmii_id) (sgmii_id * 2)

#define SOC_PB_NIF_IS_TYPE_GMII(nif_type) \
  SOC_SAND_NUM2BOOL(((nif_type) == SOC_PB_NIF_TYPE_SGMII) || ((nif_type) == SOC_PB_NIF_TYPE_QSGMII))

#define SOC_PB_NIF_IS_TYPE_XAUI_LIKE(nif_type) \
  SOC_SAND_NUM2BOOL(((nif_type) == SOC_PB_NIF_TYPE_XAUI) || ((nif_type) == SOC_PB_NIF_TYPE_RXAUI))


/*
 *	ILKN
 */

#define SOC_PB_NIF_MAL2ILKN(mal_ndx) \
  ((mal_ndx == 0)?SOC_PETRA_NIF_ID_ILKN_0:((mal_ndx == 8)?SOC_PETRA_NIF_ID_ILKN_1:SOC_PETRA_NIF_ID_NONE))

#define SOC_PB_NIF_IS_ILKN_MAL(mal_ndx) \
  (SOC_PB_NIF_MAL2ILKN(mal_ndx) != SOC_PETRA_NIF_ID_NONE)
/*
 *	ELK
 */
/* TRUE if the MAL can server as an ELK-interface */
/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  /*
   *  MAL Group A. Contains MAL-Ids 0 - 7
   */
  SOC_PB_NIF_MALG_ID_A = 0,
  /*
   *  MAL Group B. Contains MAL-Ids 8 - 15
   */
  SOC_PB_NIF_MALG_ID_B = 1,
  /*
   *  Number of types in SOC_PB_NIF_MALG_ID
   */
  SOC_PB_NIF_NOF_MALG_IDS = 2
}SOC_PB_NIF_MALG_ID;

typedef enum
{
  /*
   *  Undefined or invalid.
   */
  SOC_PB_NIF_TYPE_NONE = 0,
  /*
   *  XAUI standard 10GE interface (3.125 SerDes). Up to 16GE
   *  at 5Gbps SerDes. If SPAUI extensions are enabled, can be
   *  referred to as SPAUI interface.
   */
  SOC_PB_NIF_TYPE_XAUI = 1,
  /*
   *  SGMII standard interface or 1000Base-X interface, 1GE
   *  (1.25Gbps SerDes). Up to 4GE at 5Gbps SerDes.
   */
   SOC_PB_NIF_TYPE_SGMII = 3,
  /*
   *  RXAUI standard 10GE, at 6.25Gbps SerDes.
   */
  SOC_PB_NIF_TYPE_RXAUI = 5,
  /*
   *  QSGMII standard 1GE (5Gbps SerDes).
   */
  SOC_PB_NIF_TYPE_QSGMII = 6,
  /*
   *  Interlaken standard interface : 8-24 SerDes lanes for
   *  ILKN-A4-12 SerDes lanes for ILKN-BUp to 150GE for
   *  ILKN-24 at 6.25Gbps SerDes
   */
  SOC_PB_NIF_TYPE_ILKN = 7
}SOC_PB_NIF_TYPE;

typedef enum
{
  /*
   *  Preamble size 0 columns. No preamble completion will be
   *  performed (i.e. only SOP and/or BCT).
   */
  SOC_PB_NIF_PRMBL_SIZE_COLUMNS_0 = 0,
  /*
   *  Preamble size 1 columns - complete to one column.
   */
  SOC_PB_NIF_PRMBL_SIZE_COLUMNS_1 = 1,
  /*
   *  Preamble size 2 columns - complete to two columns.
   */
  SOC_PB_NIF_PRMBL_SIZE_COLUMNS_2 = 2,
  /*
   *  Total number of preamble size configurations.
   */
  SOC_PB_NIF_NOF_PRMBL_SIZES = 3
}SOC_PB_NIF_PREAMBLE_SIZE;

typedef enum
{
  /*
   *  Inter-Packet Gap Deficit Idle Count mode. Use average
   *  number of characters between frames configuration.
   */
  SOC_PB_NIF_IPG_DIC_MODE_AVERAGE = 0,
  /*
   *  Use minimal number of characters between frames
   *  configuration.
   */
  SOC_PB_NIF_IPG_DIC_MODE_MINIMUM = 1,
  /*
   *  Total number of Deficit Idle Count modes.
   */
  SOC_PB_NIF_NOF_IPG_DIC_MODES = 2
}SOC_PB_NIF_IPG_DIC_MODE;

typedef enum
{
  /*
   *  Standard 4B CRC-32
   */
  SOC_PB_NIF_CRC_MODE_32 = 0,
  /*
   *  Non-standard 3B CRC-24
   */
  SOC_PB_NIF_CRC_MODE_24 = 1,
  /*
   *  RX: no CRC is expected, and the NIF does not remove the
   *  CRC bytes from the packet. TX: no CRC is calculated and
   *  added to the packet. This mode can be used when the
   *  packet CRC is handled by an external logic.
   */
  SOC_PB_NIF_CRC_MODE_NONE = 2,
  /*
   *  Total number of CRC configurations
   */
  SOC_PB_NIF_NOF_CRC_MODES = 3
}SOC_PB_NIF_CRC_MODE;

typedef enum
{
  /*
   *  Local Fault indication
   */
  SOC_PB_NIF_LINK_FAULT_LOCATION_LOCAL = 0,
  /*
   *  Remote Fault indication
   */
  SOC_PB_NIF_LINK_FAULT_LOCATION_REMOTE = 1,
  /*
   *  Number of types in SOC_PB_NIF_LINK_FAULT_LOCATION
   */
  SOC_PB_NIF_NOF_LINK_FAULT_LOCATIONS = 2
}SOC_PB_NIF_LINK_FAULT_LOCATION;

typedef enum
{
  /*
   *  Continue sending data, send Idles
   */
  SOC_PB_NIF_FAULT_RESPONSE_DATA_AND_IDLE = 0,
  /*
   *  Continue sending data, send Remote Fault Indication
   */
  SOC_PB_NIF_FAULT_RESPONSE_DATA_AND_RF = 1,
  /*
   *  Continue sending data, send Local Fault Indication
   */
  SOC_PB_NIF_FAULT_RESPONSE_DATA_AND_LF = 2,
  /*
   *  Stop sending data, send Idles
   */
  SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_IDLE = 3,
  /*
   *  Stop sending data, send Remote Fault Indication
   */
  SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_RF = 4,
  /*
   *  Stop sending data, send Local Fault Indication
   */
  SOC_PB_NIF_FAULT_RESPONSE_NO_DATA_LF = 5,
  /*
   *  Number of types in SOC_PB_NIF_FAULT_RESPONSE
   */
  SOC_PB_NIF_NOF_FAULT_RESPONSES = 6
}SOC_PB_NIF_FAULT_RESPONSE;

typedef enum
{
  /*
   *  1000BASE-X Gigabit Ethernet - 1G Ethernet MAC/PHY which
   *  connects directly to the fiber.
   */
  SOC_PB_NIF_1GE_MODE_1000BASE_X = 0,
  /*
   *  SGMII/QSGMII MAC which communicates with an external PHY
   *  device
   */
  SOC_PB_NIF_1GE_MODE_SGMII = 1,
  /*
   *  Total number of 1GbE modes.
   */
  SOC_PB_NIF_NOF_1GE_MODES = 2
}SOC_PB_NIF_1GE_MODE;

typedef enum
{
  /*
   *  SGMII rate is determined by the auto-negotiation
   */
  SOC_PB_NIF_GMII_RATE_AUTONEG = 0,
  /*
   *  SGMII rate is 10Mbit/sec
   */
  SOC_PB_NIF_GMII_RATE_10Mbps = 1,
  /*
   *  SGMII rate is 100Mbit/sec
   */
  SOC_PB_NIF_GMII_RATE_100Mbps = 2,
  /*
   *  SGMII rate is 1000Mbit/sec
   */
  SOC_PB_NIF_GMII_RATE_1000Mbps = 3,
  /*
   *  Total number of SGMII rates.
   */
  SOC_PB_NIF_NOF_GMII_RATES = 4
}SOC_PB_NIF_GMII_RATE;

typedef enum
{
  /*
   *  Indication of type of Remote Fault - OK, no error.
   */
  SOC_PB_NIF_GMII_RF_OK = 0,
  /*
   *  Indication of type of Remote Fault - Offline request by
   *  link partner.
   */
  SOC_PB_NIF_GMII_RF_OFFLINE_REQ = 1,
  /*
   *  Indication of type of Remote Fault - Link failure
   *  detected by partner.
   */
  SOC_PB_NIF_GMII_RF_LNK_FAIL = 2,
  /*
   *  Indication of type of Remote Fault - AutoNegotiation
   *  Error.
   */
  SOC_PB_NIF_GMII_RF_AUTONEG_ERR = 3,
  /*
   *  Total number of SGMII remote fault type indications.
   *  Note: only valid with GotRemoteFault bit for
   *  corresponding lane.
   */
  SOC_PB_NIF_NOF_GMII_RFS = 4
}SOC_PB_NIF_GMII_RF;

typedef enum
{
  /*
   *  Interlaken Interface A. Corresponds to INTERFCE_ID 5000
   *  (NIF_ID_ILKN_0). Can consume 8 - 24 SerDes lanes.
   */
  SOC_PB_NIF_ILKN_ID_A = SOC_PETRA_NIF_ID_ILKN_0,
  /*
   *  Interlaken Interface B. Corresponds to INTERFCE_ID 5001
   *  (NIF_ID_ILKN_1). Can consume 4 - 12 SerDes lanes
   */
  SOC_PB_NIF_ILKN_ID_B = SOC_PETRA_NIF_ID_ILKN_1,
  /*
   *  Number of types in SOC_PB_NIF_ILKN_ID
   */
  SOC_PB_NIF_NOF_ILKN_IDS = 2
}SOC_PB_NIF_ILKN_ID;

typedef enum
{
  /*
   *  Clock Divider for the selected recovered clock rate
   *  (based on SerDes lane rate) - divide by 20
   */
  SOC_PB_NIF_SYNCE_CLK_DIV_20 = 0,
  /*
   *  Clock Divider for the selected recovered clock rate
   *  (based on SerDes lane rate) - divide by 40
   */
  SOC_PB_NIF_SYNCE_CLK_DIV_40 = 1,
  /*
   *  Clock Divider for the selected recovered clock rate
   *  (based on SerDes lane rate) - divide by 80
   */
  SOC_PB_NIF_SYNCE_CLK_DIV_80 = 2,
  /*
   *  Number of types in SOC_PB_NIF_SYNCE_CLK_DIV
   */
  SOC_PB_NIF_NOF_SYNCE_CLK_DIVS = 3
}SOC_PB_NIF_SYNCE_CLK_DIV;

typedef enum
{
  /*
   *  MALG-A Clock/Differential Clock
   */
  SOC_PB_NIF_SYNCE_CLK_ID_0 = 0,
  /*
   *  MALG-A Clock/Valid
   */
  SOC_PB_NIF_SYNCE_CLK_ID_1_OR_VALID = 1,
  /*
   *  MALG-B Clock/Differential Clock
   */
  SOC_PB_NIF_SYNCE_CLK_ID_2 = 2,
  /*
   *  MALG-B Clock/Valid
   */
  SOC_PB_NIF_SYNCE_CLK_ID_3_OR_VALID = 3,
  /*
   *  Number of types in SOC_PB_NIF_SYNCE_CLK_ID
   */
  SOC_PB_NIF_NOF_SYNCE_CLK_IDS = 4
}SOC_PB_NIF_SYNCE_CLK_ID;

typedef enum
{
  /*
   *  Synchronous Ethernet signal - differential (two signals
   *  per clock) recovered clock, two differential outputs
   */
  SOC_PB_NIF_SYNCE_MODE_TWO_DIFF_CLK = 0,
  /*
   *  Synchronous Ethernet signal - recovered clock, four
   *  outputs - two from each MAL group. Each clock may be
   *  connected to any NIF in the same MAL group.
   */
  SOC_PB_NIF_SYNCE_MODE_FOUR_CLK = 1,
  /*
   *  Synchronous Ethernet signal - recovered clock accompanied
   *  by a valid indication, two clk+valid outputs
   */
  SOC_PB_NIF_SYNCE_MODE_TWO_CLK_AND_VALID = 2,
  /*
   *  Number of types in SOC_PB_NIF_SYNCE_MODE
   */
  SOC_PB_NIF_NOF_SYNCE_MODES = 3
}SOC_PB_NIF_SYNCE_MODE;

/*
 *	TRUE if two clocks are used (and not four/none)
 */
#define SOC_PB_NIF_SYNCE_IS_TWO_CLK_MODE(_mode)                  \
  SOC_SAND_NUM2BOOL((_mode == SOC_PB_NIF_SYNCE_MODE_TWO_DIFF_CLK) || \
                (_mode == SOC_PB_NIF_SYNCE_MODE_TWO_CLK_AND_VALID))

 /*
 *	TRUE if the clock-id is applicable for a two-clock mode
 */
#define SOC_PB_NIF_SYNCE_IS_TWO_CLK_ID(_clk_id) \
  SOC_SAND_NUM2BOOL(                            \
    (_clk_id == SOC_PB_NIF_SYNCE_CLK_ID_0) ||   \
    (_clk_id == SOC_PB_NIF_SYNCE_CLK_ID_2)      \
  )
/*
 *	TRUE if the clock-id is applicable for the SYNCE-MODE
 */
#define SOC_PB_NIF_SYNCE_IS_CLK_OF_MODE(_mode, _clk_id) \
  SOC_SAND_NUM2BOOL(                                    \
    (SOC_PB_NIF_SYNCE_IS_TWO_CLK_MODE(_mode) && SOC_PB_NIF_SYNCE_IS_TWO_CLK_ID(_clk_id)) || \
    ((_mode == SOC_PB_NIF_SYNCE_MODE_FOUR_CLK) && (_clk_id < SOC_PB_NIF_NOF_SYNCE_CLK_IDS))   \
  )

/*
 * The Fat Pipe Index.
 * The OFP port that is used as the Fat pipe destination is
 * derived from the Fat-pipe index: OFP 1 for Fat-pipe A, OFP 2 for Fat-pipe B,
 * OFP 3 for Fat-pipe C.
 */
typedef enum
{
  /*
   *  Fat-Pipe Interface A (0)
   */
  SOC_PB_NIF_FATP_ID_A = 0,
  /*
   *  Fat-Pipe Interface B (1). Can be used only in 2x6 or 3x4
   *  modes.
   */
  SOC_PB_NIF_FATP_ID_B = 1,
  /*
   *  Fat-Pipe Interface C (2). Can be used only in 3x4 modes
   */
  SOC_PB_NIF_FATP_ID_C = 2,
  /*
   *  Number of types in SOC_PB_NIF_FATP_ID
   */
  SOC_PB_NIF_NOF_FATP_IDS = 3
}SOC_PB_NIF_FATP_ID;

typedef enum
{
  /*
   *  Fat-Pipe is disabled
   */
  SOC_PB_NIF_FATP_MODE_DISABLED = 0,
  /*
   *  Fat-Pipe Enabled on (at maximum) 3 Fat-pipe interfaces
   *  A-C, (at maximum) 4 FAP ports per interface
   */
  SOC_PB_NIF_FATP_MODE_3_X_4 = 1,
  /*
   *  Fat-Pipe Enabled on (at maximum) 2 Fat-pipe interfaces
   *  A-B, (at maximum) 6 FAP ports per interface
   */
  SOC_PB_NIF_FATP_MODE_2_X_6 = 2,
  /*
   *  Fat-Pipe Enabled on 1 Fat-pipe interface A, (at maximum)
   *  12 FAP ports per interface
   */
  SOC_PB_NIF_FATP_MODE_1_X_12 = 3,
  /*
   *  Number of types in SOC_PB_NIF_FATP_MODE
   */
  SOC_PB_NIF_NOF_FATP_MODES = 4
}SOC_PB_NIF_FATP_MODE;

typedef enum
{
  /*
   *  Received broadcast packets. . Configurable:
   *  include/exclude bad packets.
   */
  SOC_PB_NIF_RX_BCAST_PACKETS,
  /*
   *  Received multicast packets. . Configurable:
   *  include/exclude bad packets.
   */
  SOC_PB_NIF_RX_MCAST_BURSTS,
  
  /*
   *  Received invalid packets ( e.g. CRC errors).
   *  Configurable: only Unicast packets/ All packets.
   */
  SOC_PB_NIF_RX_ERR_PACKETS,
  /*
   *  Received packets, below minimal-packet-size. Units:
   *  Bytes. Configurable: include/exclude bad packets.
   */
  SOC_PB_NIF_RX_LEN_BELOW_MIN,
  /*
   *  Received packets, minimal-packet-size - 59 Bytes.
   *  Configurable: include/exclude bad packets.
   */
  SOC_PB_NIF_RX_LEN_MIN_59,
  /*
   *  Received packets, 60. Units: Bytes. Configurable:
   *  include/exclude bad packets.
   */
  SOC_PB_NIF_RX_LEN_60,
  /*
   *  Received packets, 0 - X Bytes. Configurable:
   *  include/exclude bad packets.
   */
  SOC_PB_NIF_RX_LEN_61_123,
  /*
   *  Received packets, 0 - X Bytes. Configurable:
   *  include/exclude bad packets.
   */
  SOC_PB_NIF_RX_LEN_124_251,
  /*
   *  Received packets, 252 - 507 Bytes. Configurable:
   *  include/exclude bad packets.
   */
  SOC_PB_NIF_RX_LEN_252_507,
  /*
   *  Received packets, 508 - 1019 Bytes. Configurable:
   *  include/exclude bad packets.
   */
  SOC_PB_NIF_RX_LEN_508_1019,
  /*
   *  Received packets, 1020 - 1514/1518 Bytes (configurable).
   *  Configurable: include/exclude bad packets.
   */
  SOC_PB_NIF_RX_LEN_1020_1514CFG,
  /*
   *  Received packets, 1515/1519 - maximal-packet-size Bytes.
   *  Configurable: include/exclude bad packets.
   */
  SOC_PB_NIF_RX_LEN_1515CFG_MAX,
  /*
   *  Received packets, above maximal packet size.
   *  Configurable: include/exclude bad packets.
   */
  SOC_PB_NIF_RX_LEN_ABOVE_MAX,
  /*
   *  Received valid pause frames.
   */
  SOC_PB_NIF_RX_OK_PAUSE_FRAMES,
  /*
   *  Received invalid pause frames.
   */
  SOC_PB_NIF_RX_ERR_PAUSE_FRAMES,
  /*
   *  Received PTP frames
   */
  SOC_PB_NIF_RX_PTP_FRAMES,
  /*
   *  Received invalid packets with frame errors
   */
  SOC_PB_NIF_RX_FRAME_ERR_PACKETS,
  /*
   *  Received invalid packets with BCT - CRC errors (counted
   *  on port 1 on XAUI interfaces)
   */
  SOC_PB_NIF_RX_BCT_ERR_PACKETS,
  /*
   *  Transmitted broadcast packets. . Configurable:
   *  include/exclude bad packets.
   */
  SOC_PB_NIF_TX_BCAST_PACKETS,
  /*
   *  Transmitted multicast packets. . Configurable:
   *  include/exclude bad packets.
   */
  SOC_PB_NIF_TX_MCAST_BURSTS,
  
  /*
   *  Transmitted invalid packets ( e.g. CRC errors).
   *  Configurable: only Unicast packets/ All packets.
   */
  SOC_PB_NIF_TX_ERR_PACKETS,
  /*
   *  Transmitted pause frames.
   */
  SOC_PB_NIF_TX_PAUSE_FRAMES,
  /*
   *  Transmitted PTP frames.
   */
  SOC_PB_NIF_TX_PTP_FRAMES,
  /*
   *  Approximate number of dropped packets (TX) due to a link
   *  failure
   */
  SOC_PB_NIF_TX_NO_LINK_PACKETS,
  /*
   *  Received valid octets. Configurable: include/exclude bad
   *  packets.
   */
  SOC_PB_NIF_RX_OK_OCTETS,
  /*
   *  Transmitted valid octets. Configurable: include/exclude
   *  bad packets.
   */
  SOC_PB_NIF_TX_OK_OCTETS,
  /*
   *  Received valid packets. Configurable: only Unicast
   *  packets/ All packets.
   */
  SOC_PB_NIF_RX_OK_PACKETS,
  /*
   *  Transmitted valid packets. Configurable: only Unicast
   *  packets/ All packets.
   */
  SOC_PB_NIF_TX_OK_PACKETS,
  /*
   *  Number of types in SOC_PB_NIF_COUNTER_TYPE
   */
  SOC_PB_NIF_NOF_COUNTER_TYPES
}SOC_PB_NIF_COUNTER_TYPE;

#define SOC_PB_NIF_NOF_COUNTERS   SOC_PB_NIF_NOF_COUNTER_TYPES

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the NIF is up.
   */
  uint8 is_nif_on;
  /*
   *  Set the serdes lane accordingly - power down when NIF is
   *  turned off, power up when turned on.
   */
  uint8 serdes_also;

} SOC_PB_NIF_STATE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If set, order of the SerDes Physical Lanes will be
   *  swapped when mapped into Logical Lanes. Per SerDes
   *  quartet, Lane 0 is swapped with Lane 3, Lane 1 with
   *  Lane2Note: This is applicable for XAUI/RXAUI interfaces
   *  only
   */
  uint8 swap_rx;
  /*
   *  Same as RX
   */
  uint8 swap_tx;

} SOC_PB_NIF_LANES_SWAP;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/Disable Rx/Tx lanes swap. Note: this is applicable
   *  for XAUI/RXAUI interfaces onlyDefault: disabled.
   */
  SOC_PB_NIF_LANES_SWAP lanes_swap;
  /*
   *  If TRUE, an Alternative SGMII mapping to SerDes is used,
   *  as follows: MAL 5 is mapped to QSRD-0 lane 1, MAL 6 is
   *  mapped to QSRD-1 lane 3, MAL 7 is mapped to QSRD-0 lane
   *  3. This mapping allows to facilitate the QSGMII NIF
   *  ports in the case that the COMBO QSRD is towards the
   *  fabric, or the spouse-MAL is used as XAUI. Default:
   *  disabled.
   */
  uint8 is_qsgmii_alt;

} SOC_PB_NIF_MAL_TOPOLOGY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The interface type. Can be XAUI/RXAUI/SGMII/QSGMII
   */
  SOC_PB_NIF_TYPE type;
  /*
   *  Topology configuration. Defines the SerDes lanes used by
   *  the MAL. If not configured, typical (not alternative)
   *  topology is applied.
   */
  SOC_PB_NIF_MAL_TOPOLOGY topology;

} SOC_PB_NIF_MAL_BASIC_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, Preamble Compression is enabled. Applicable for
   *  SPAUI interfaces only. Other fields are only applicable
   *  for get/set operations if enable = TRUE
   */
  uint8 enable;
  /*
   *  Preamble size
   */
  SOC_PB_NIF_PREAMBLE_SIZE size;
  /*
   *  If set, /S/ character will not be inserted at the
   *  beginning of a packet.
   */
  uint8 skip_SOP;

} SOC_PB_NIF_PREAMBLE_COMPRESS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, Inter Packet Gap Compression is
   *  enabled. Applicable for SPAUI interfaces only. Other
   *  fields are only applicable for get/set operations if
   *  enable = TRUE
   */
  uint8 enable;
  /*
   *  Inter Packet Gap size in bytes. Range: 1 - 255.
   */
  uint32 size;
  /*
   *  Deficit Idle Count mode - average or minimal.
   */
  SOC_PB_NIF_IPG_DIC_MODE dic_mode;

} SOC_PB_NIF_IPG_COMPRESS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, Burst Control is enabled. Applicable for SPAUI
   *  interfaces only. Other fields are only applicable for
   *  get/set operations if enable = TRUE
   */
  uint8 enable;
  /*
   *  Burst Control Tag Size. Range: 0 - 2.
   */
  uint32 bct_size;
  /*
   *  Index of the byte containing the CH (Channel) field
   *  inside the first column of the preamble. Possible values
   *  are 0 (if no SOP in preamble), 1, 2, 3Range: 0 - 3.
   */
  uint32 bct_channel_byte_ndx;
  /*
   *  If TRUE, the channelized interface will function in
   *  burst interleaving mode. Otherwise - in full packet mode.
   *  Note: if TRUE, the bct_size must be set to '2'
   */
  uint8 is_burst_interleaving;

} SOC_PB_NIF_CHANNELIZED;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Defines the response to local fault indication. Default:
   *  Stop sending data, send Remote Fault Indication
   */
  SOC_PB_NIF_FAULT_RESPONSE local;
  /*
   *  Defines the response to remote fault indication.
   *  Default: Stop sending data, send Idles
   */
  SOC_PB_NIF_FAULT_RESPONSE remote;

} SOC_PB_NIF_FAULT_RESPONSE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/disable the interface in specified direction.
   *  Note: to define a XAUI/SPAUI NIF-port as half-duplex,
   *  enable it only in the required direction.
   */
  uint8 enable;
  /*
   *  If TRUE - the link partner's bus size is 64 bits
   *  length. If FALSE - the link partner's bus size is 32 bits
   *  length.
   */
  uint8 link_partner_double_size_bus;
  /*
   *  Relevant only if link_partner_double_size_bus is set,
   *  ignored otherwise.
   *  If TRUE - the SOP will be only in odd position.
   */
  uint8 is_double_size_sop_odd_only;
  /*
   *  Relevant only if link_partner_double_size_bus is set,
   *  ignored otherwise.
   *  If TRUE - the SOP will be only in even position.
   */
  uint8 is_double_size_sop_even_only;
  /*
   *  Preamble Compression configuration.
   */
  SOC_PB_NIF_PREAMBLE_COMPRESS preamble;
  /*
   *  Inter Packet Gap configuration
   */
  SOC_PB_NIF_IPG_COMPRESS ipg;
  /*
   *  24/32 Byte CRC mode configuration.
   */
  SOC_PB_NIF_CRC_MODE crc_mode;
  /*
   *  Channelized configuration.
   */
  SOC_PB_NIF_CHANNELIZED ch_conf;
  /*
   *  Defines the response to local/remote fault indication.
   */
  SOC_PB_NIF_FAULT_RESPONSE_INFO fault_response;

} SOC_PB_NIF_SPAUI_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the appropriate SGMII interface is enabled in
   *  the RX direction.
   */
  uint8 enable_rx;
  /*
   *  If TRUE, the appropriate SGMII interface is enabled in
   *  the TX direction.
   */
  uint8 enable_tx;
  /*
   *  1000BASE-X or SGMII.
   */
  SOC_PB_NIF_1GE_MODE mode;
  /*
   *  SGMII link-rate - explicit, or auto-negotiation
   */
  SOC_PB_NIF_GMII_RATE rate;

} SOC_PB_NIF_GMII_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the auto-negotiation is not complete.
   */
  uint8 autoneg_not_complete;
  /*
   *	If TRUE, the auto-negotiation completed with error, i.e.
   *  capabilities could not be matched.
   *  This may be the result of Half-Duplex link  partner.
   */
  uint8 autoneg_error;
  /*
   *  If TRUE, indicates that received fault indication from
   *  link partner. In 1000base-X mode, indication is Remote
   *  Fault in AutoNegotiation register. In SGMII mode,
   *  indication is received from PHY, and indicates link-down. 
   */
  uint8 remote_fault;
  /*
   *  Indication of type of Remote Fault (1000base-X mode). Only valid with
   *  GotRemoteFault bit for corresponding lane.
   *  For SGMII, if link-down indication is received, this field is set to 
   *  SOC_PB_NIF_GMII_RF_LNK_FAIL, otherwise to SOC_PB_NIF_GMII_RF_OK
   */
  SOC_PB_NIF_GMII_RF fault_type;

} SOC_PB_NIF_GMII_STAT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Number of lanes in the Interlaken interface. For ILKN-A;
   *  Range: 8 - 24. For ILKN-B; Range: 4 - 12
   */
  uint32 nof_lanes;
  /*
   *  If TRUE, one of the ILKN lanes is declared invalid, and
   *  is disabled. Cannot be used for ILKN-A with 8 lanes or
   *  ILKN-B with 4 lanes.
   */
  uint8 is_invalid_lane;
  /*
   *  If is_invalid_lane is TRUE, identifies the invalid lane.
   *  Ignored otherwise. For ILKN-A; Range: 0 - 23. For ILKN-B;
   *  Range: 0 - 11
   */
  uint32 invalid_lane_id;
  /*
   *  If TRUE, the channelized interface functions in burst
   *  interleaving mode. Otherwise - in full packet mode.
   *  Note: when configuring FAT-PIPE over ILKN,
   *  ILKN must be configured to work as interleaved
   *  (is_burst_interleaving == TRUE)
   */
  uint8 is_burst_interleaving;
  /*
   *  The maximal interval, in words, between meta-frame sync
   *  words (see section 5.4.3 of Interlaken spec 1.1). Units:
   *  words (66-bit blocks). Default: 4K. Range: 64 - 16K.
   */
  uint32 metaframe_sync_period;

} SOC_PB_NIF_ILKN_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Indicates there has been a change in the alignment of
   *  this port.
   */
  uint8 ilkn_rx_port_status_change_int;
  /*
   *  Indicates there has been a change in the interface
   *  status of the link partner. We receive this indication
   *  from bit 33 in the diagword, according to the Interlaken
   *  protocol.
   */
  uint8 ilkn_link_partner_status_change_lanes_int;
  /*
   *  Indicates the lane alignment failed after several
   *  attempts, or lane alignment was lost.
   */
  uint8 ilkn_rx_port_align_err_int;
  /*
   *  Indicates that the CRC32 in the most recently received
   *  Diagnostic Word for the respective lane is not as
   *  expected.
   */
  uint8 ilkn_crc32_lane_err_indication_lanes_int;
  /*
   *  Indicates one of the following: 64B/67B Word Boundary
   *  Lock with Framing bits [65:64] was not achieved OR
   *  errors were detected on Framing bits [65:64] after
   *  synchronization OR a Meta Frame Synchronization Word was
   *  never correctly receive
   */
  uint8 ilkn_lane_synch_err_lanes_int;
  /*
   *  Indicates an illegal framing attern was detected in the
   *  espective lane after being Word Boundary Synchronized
   */
  uint8 ilkn_lane_framing_err_lanes_int;
  /*
   *  Indicates an illegal framing layer block type was
   *  detected
   */
  uint8 ilkn_lane_bad_framing_type_err_lanes_int;
  /*
   *  Indicates the Meta Frame Synchronization Word in the
   *  respective lane contained an error.
   */
  uint8 ilkn_lane_meta_frame_sync_word_err_lanes_int;
  /*
   *  Indicates the Scrambler State Control Word in the
   *  respective lane contained an error.
   */
  uint8 ilkn_lane_scrambler_state_err_lanes_int;
  /*
   *  Indicates the length of the Meta Frame being received in
   *  the respective lane is different from the expected
   *  length.
   */
  uint8 ilkn_lane_meta_frame_length_err_lanes_int;
  /*
   *  Indicates one or more of the following occurred on the
   *  respective lane: - four consecutive Meta Frame
   *  Synchronization Words contained errors - three
   *  consecutive Scrambler State Control Words contained
   *  errors
   */
  uint8 ilkn_lane_meta_frame_repeat_err_lanes_int;
  /*
   *  Parity error in one of the RX lanes / ports.
   */
  uint8 ilkn_rx_port_status_err_int;
  /*
   *  Parity error in one of the RX lanes / ports.
   */
  uint8 ilkn_tx_port_status_err_int;
  /*
   *  Indicates one of the errors in RxIlknStatus has
   *  occurred.
   */
  uint8 ilkn_rx_parity_err_int;
  /*
   *  Indicates one of the errors in TxIlknStatus has
   *  occurred.
   */
  uint8 ilkn_tx_parity_err_int;

} SOC_PB_NIF_ILKN_INT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *
   */
  uint8 rx_stat_aligned_raw;
  /*
   *
   */
  uint8 rx_port_active;
  /*
   *
   */
  uint8 rx_stat_misaligned;
  /*
   *
   */
  uint8 rx_stat_aligned_err;
  /*
   *
   */
  uint8 rx_stat_crc24_err;
  /*
   *
   */
  uint8 rx_stat_miss_sop_err;
  /*
   *
   */
  uint8 rx_stat_miss_eop_err;
  /*
   *
   */
  uint8 rx_stat_overflow_err;
  /*
   *
   */
  uint8 rx_stat_burstmax_err;
  /*
   *
   */
  uint8 rx_stat_burst_err;
  /*
   *
   */
  uint8 rx_lp_ifc_status;
  /*
   *
   */
  uint8 tx_ovfout;
  /*
   *
   */
  uint8 tx_port_active;
  /*
   *
   */
  uint8 tx_stat_underflow_err;
  /*
   *
   */
  uint8 tx_stat_burst_err;
  /*
   *
   */
  uint8 tx_stat_overflow_err;
  /*
   *
   */
  uint32 link_partner_lanes_status_bitmap;
  /*
   *
   */
  uint32 link_partner_interface_status_bitmap;
  /*
   *
   */
  uint32 crc32_lane_valid_indication_bitmap;
  /*
   *
   */
  uint32 crc32_lane_err_indication_bitmap;
  /*
   *
   */
  uint32 lane_synchronization_achieved_bitmap;
  /*
   *
   */
  uint32 lane_synchronization_err_bitmap;
  /*
   *
   */
  uint32 lane_framing_err_bitmap;
  /*
   *
   */
  uint32 lane_bad_framing_layer_err_bitmap;
  /*
   *
   */
  uint32 lane_meta_frame_sync_word_err_bitmap;
  /*
   *
   */
  uint32 lane_scrambler_state_err_bitmap;
  /*
   *
   */
  uint32 lane_meta_frame_length_err_bitmap;
  /*
   *
   */
  uint32 lane_meta_frame_repeat_err_bitmap;
  /*
   *
   */
  uint32 ilkn_rx_stat_lanes_parity_err_bitmap;
  /*
   *
   */
  uint8 ilkn_rx_stat_buff_parity_err;
  /*
   *
   */
  uint32 ilkn_tx_stat_lanes_parity_err;
  /*
   *
   */
  uint8 ilkn_tx_stat_buff_parity_err;

} SOC_PB_NIF_ILKN_STAT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Interrupt indications
   */
  SOC_PB_NIF_ILKN_INT interrupt;
  /*
   *  Status indications
   */
  SOC_PB_NIF_ILKN_STAT stat;

} SOC_PB_NIF_ILKN_DIAG_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the corresponding NIF port links are up. Each
   *  link indication is cleared if link fails and remains 0
   *  until read by CPU.
   */
  uint8 is_up;
  /*
   *  If TRUE, indicates there was Tx traffic since the
   *  indication was last read.
   */
  uint8 is_active_rx;
  /*
   *  If TRUE, indicates there was Tx traffic since the
   *  indication was last read.
   */
  uint8 is_active_tx;

} SOC_PB_NIF_STAT_LINK_ACTIVITY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the NIF port is enabled
   */
  uint8 enabled;
  /*
   *  Per NIF port, if enabled, indicates whether its links
   *  are valid, and active in RX/TX direction. The indication
   *  is cleared on read.
   */
  SOC_PB_NIF_STAT_LINK_ACTIVITY status;

} SOC_PB_NIF_ACTIVITY_STATUS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Per-NIF activity status
   */
  SOC_PB_NIF_STAT_LINK_ACTIVITY activity[SOC_PB_NIF_NOF_NIFS];

} SOC_PB_NIF_ACTIVITY_STATUS_ALL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Current link status. In XAUI mode, indicates that all
   *  lanes are comma aligned and that lane alignment (deskew)
   *  has been achieved. Equivalent to link_fault variable in
   *  IEEE 802.3ae Clause 46.3.4.2. In 1G Ethernet mode,
   *  indicates that the XMIT indication for the corresponding
   *  lane's AutoNegotiation FSM is XMIT=DATA. In SGMII mode,
   *  reflects the link status received from the PHY.
   */
  uint8 link_status;
  /*
   *  Indicates change in link status from down to up,
   *  TRUE only if the link was down since the last time the indication was read.
   */
  uint8 link_status_change;
  /*
   *  Indicates all relevant physical lanes achieved
   *  synchronization.
   */
  uint8 pcs_sync;

} SOC_PB_NIF_LINK_STATUS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the selected Recovered Clock is active.
   */
  uint8 enable;
  /*
   *  The source NIF port for the recovered clock output of
   *  the MALG. Range: NIF_ID(NIF_TYPE, 0 - 63). Note: the
   *  actual source is a single SerDes lane in the specified
   *  NIF port.
   */
  SOC_PETRA_INTERFACE_ID nif_id;
  /*
   *  Clock Divider for the selected recovered clock.
   */
  SOC_PB_NIF_SYNCE_CLK_DIV clk_divider;
  /*
   *  If TRUE, automatic squelch function is enabled for the
   *  recovered clock. This function powers down the clock
   *  output whenever the link is not synced, i.e. the clock
   *  is invalid (even if the VALID indication is not present
   *  on the pin).
   */
  uint8 squelch_enable;

} SOC_PB_NIF_SYNCE_CLK;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The PTP clock increment, each PTP clock. The resolution
   *  is 2^(-8) nanosecond. Zero value disables PTP clock. For
   *  example, 1000[ns]*256 corresponds to a 1MHz PTP clock.
   *  Range: 0 - 524287.
   */
  uint32 ptp_clk_delta;
  /*
   *  The absolute time value at the next PTP-Sync pulse. This
   *  value will be used to reset the internal PTP timer on
   *  the next PTP-Sync. This is 48-bit value. Units:
   *  nanoseconds: Range: 0 - 259,200*10^9. (0[nsec] -
   *  72[hrs])
   */
  SOC_SAND_U64 sync_reset_val;
  /*
   *  If set to '0', PTP-sync auto-increment function is
   *  disabled. Otherwise, PTP-sync reset value is
   *  auto-incremented once every interval. Units: nanoseconds.
   *  Range: 259,200*10^9. (0[nsec] - 72[hrs])
   */
  SOC_SAND_U64 sync_autoinc_interval;

} SOC_PB_NIF_PTP_CLK_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The PTP clock current reset time value. Units:
   *  Nanoseconds.
   */
  SOC_SAND_U64 curr_time;

} SOC_PB_NIF_PTP_CLK_RST_VAL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/disable Precision Time Protocol
   */
  uint8 enable;
  /*
   *  Wire Delay to add. Units: Nanoseconds.
   */
  uint32 wire_delay_ns;

} SOC_PB_NIF_PTP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the External Lookup Interface (ELK) is enabled
   */
  uint8 enable;
  /*
   *  If enabled, the MAL consumed by the ELK interface. The
   *  appropriate MAL cannot be used for NIF configuration.
   *  Range: 0, 12, 14
   */
  uint32 mal_id;

} SOC_PB_NIF_ELK_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  One of the following modes: disabled, 1x12, 2x6, 3x4
   */
  SOC_PB_NIF_FATP_MODE mode;
  /*
   *  If TRUE, Enables a mode where some packets can bypass
   *  the resequencing process. If set, then the sequence
   *  number range is only 15b [14:0] instead of 16b, and bit
   *  15, if set, indicates that the packet should bypass the
   *  resequencing process
   */
  uint8 is_bypass_enable;

} SOC_PB_NIF_FATP_MODE_INFO;

/*
 * Note: if a Fat Pipe is enabled,
 * the resources of OFP-s 1-12 are consumedby the Fat Pipe,
 * i.e. OFP-s 1-12 cannot be used elsewhere
 */
typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the Fat-pipe interface is enabled.
   */
  uint8 enable;
  /*
   *  If the Fat-pipe is enabled, the FAP Port index of the
   *  base port of the Fat-pipe. Range: 1 - 12. If the
   *  Fat-pipe is disabled, this field is ignored.
   */
  uint32 base_port_id;
  /*
   *  If the Fat-pipe is enabled, the number of Fat-pipe consecutive OFP ports,
   *  and accordingly Network Interfaces, that comprise
   *  the Fat-pipe. According to the NIF_FATP_MODE: For 3x4:
   *  Range: 1 - 4. For 2x6: Range: 1 - 6. For 1x12: Range: 1 -
   *  12. If the Fat-pipe is disabled, this field is ignored.
   */
  uint32 nof_ports;

} SOC_PB_NIF_FATP_INFO;


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
/*
 *	This utility converts from Soc_petra-A-style indexing
 *  to Soc_petra-B-style indexing.
 *  Only Soc_petra-B indexing can be used for NIF configuration
 */
uint32 soc_pb_nif_pa2pb_id(
            SOC_PB_NIF_TYPE      nif_type,
            SOC_PETRA_INTERFACE_ID  soc_pa_nif_id
          );

/*
 *	Derive Interface type
 *  from  Soc_petra-B style index
 */
SOC_PB_NIF_TYPE
  soc_pb_nif_id2type(
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID  soc_pb_nif_id
  );

/*
 *	Derive Soc_petra Interface ID
 *  from Soc_petra-B style type, index
 */
SOC_PETRA_INTERFACE_ID
  soc_pb_nif_type2id(
    SOC_SAND_IN SOC_PB_NIF_TYPE soc_pb_nif_type,
    SOC_SAND_IN uint32 internal_id
  );

/*
 *	This utility converts from Soc_petra-B-style indexing
 *  to Soc_petra-A-style indexing.
 *  Only Soc_petra-B indexing can be used for NIF configuration
 *  Soc_petra-A style indexing can be used in Soc_petra-A compatible APIs
 *  that do not configure the NIF (e.g. soc_petra_sch_if_shaper_rate_set)
 *  This APIs support both old and new style indexing.
 */
uint32 soc_pb_nif_pb2pa_id(
            SOC_PETRA_INTERFACE_ID  soc_pb_nif_id
          );

/*********************************************************************
* NAME:
 *   soc_pb_nif_on_off_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Turns the interface on/off. Optionally, powers up/down
 *   the attached SerDes also.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID                nif_ndx -
 *     NIF Port index. Range: SOC_PB_NIF_ID(NIF-Type, 0 - 63).
 *   SOC_SAND_IN  SOC_PB_NIF_STATE_INFO              *info -
 *     Chooses whether to turn the NIF on or off, and whether
 *     to power up/down the attached SerDes also.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_on_off_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_STATE_INFO              *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_nif_on_off_set"
 *     API.
 *     Refer to "soc_pb_nif_on_off_set" API for details.
*********************************************************************/
uint32
  soc_pb_nif_on_off_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID                nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_STATE_INFO              *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_loopback_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the specified Network Interface to loopback, by
 *   setting all its SerDeses in loopback (NSILB)
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     NIF Port index. Range: SOC_PB_NIF_ID(NIF-Type, 0 - 63).
 *   SOC_SAND_IN  uint8                      enable -
 *     If TRUE, the loopback is enabled
 * REMARKS:
 *   1. For QSGMII, the loopback is enabled/disabled on all the (four)
 *    QSGMII interfaces sharing the same SerDes
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_loopback_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint8                      enable
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_nif_loopback_set"
 *     API.
 *     Refer to "soc_pb_nif_loopback_set" API for details.
*********************************************************************/
uint32
  soc_pb_nif_loopback_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT uint8                      *enable
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_mal_basic_conf_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets basic MAL configuration. This configuration defines
 *   MAL to SerDes mapping topology. Following setting basic
 *   configuration, per-NIF-type (e.g. SPAUI/SGMII etc.)
 *   configuration must be set before enabling the MAL.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                      mal_ndx -
 *     MAL index. Range: 0 - 15
 *   SOC_SAND_IN  SOC_PB_NIF_MAL_BASIC_INFO          *info -
 *     Basic MAL figuration: Type:
 *     XAUI/RXAUI/SGMII/QSGMIITopology-related: lanes swap,
 *     alternative-QSGMII mapping
 * REMARKS:
 *   1. Refer to the Soc_petra-B User Manual for a detailed
 *   description of the supported NIF topologies 2. After MAL
 *   configuration, and enabling the requested MAL, the
 *   topology can be validated by calling
 *   soc_petra_nif_topology_validate API 3. This API should not
 *   be used for ILKN interface configuration. ILKN interface
 *   is configured using soc_pb_nif_ilkn_set API 4. This API does not set
 *   SerDes configuration (rate, RX/TX physical parameters
 *   etc.). The SerDes is expected to be initialized prior to
 *   calling this API. *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_mal_basic_conf_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PB_NIF_MAL_BASIC_INFO          *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_mal_basic_conf_set" API.
 *     Refer to "soc_pb_nif_mal_basic_conf_set" API for details.
*********************************************************************/
uint32
  soc_pb_nif_mal_basic_conf_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT SOC_PB_NIF_MAL_BASIC_INFO          *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_min_packet_size_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets minimal packet size as limited by the NIF. Note:
 *   Normally, the packet size is limited using
 *   soc_petra_mgmt_pckt_size_range_set(), and not this API.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                      mal_ndx -
 *     MAL index. Range: 0 - 15.
 *   SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx -
 *     Defines whether the configuration refers to RX, TX or
 *     both.
 *   SOC_SAND_IN  uint32                       pckt_size -
 *     Minimal packet size in Bytes. Range: 0 - 255.
 * REMARKS:
 *   1. Normally, the packet size is limited using
 *   soc_petra_mgmt_pckt_size_range_set(), and not this API. 2.
 *   For RX direction - the NIF will drop smaller packets.
 *   Note that the inspected packet size includes packet CRC.
 *   3. For TX direction - the NIF will pad smaller packets
 *   with trailing zeros. The packet CRC is calculated on the
 *   whole packet, including zeros. 4. The get function is
 *   not symmetric to the set function: both RX and TX
 *   settings are returned (direction_ndx is not passed).
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_min_packet_size_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  uint32                       pckt_size
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_min_packet_size_set" API.
 *     Refer to "soc_pb_nif_min_packet_size_set" API for details.
*********************************************************************/
uint32
  soc_pb_nif_min_packet_size_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      mal_ndx,
    SOC_SAND_OUT uint32                       *pckt_size_rx,
    SOC_SAND_OUT uint32                       *pckt_size_tx
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_spaui_conf_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets SPAUI configuration - configure SPAUI extensions
 *   for XAUI interface.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     Interface index. SPAUI Range: 1000 - 1007.
 *     (SOC_PB_NIF_ID(XAUI, 0-7)). RSPAUI Range: 2000 - 2015.
 *     (SOC_PB_NIF_ID(RXAUI, 0-15))
 *   SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx -
 *     Defines whether the configuration refers to RX, TX or
 *     both.
 *   SOC_SAND_IN  SOC_PB_NIF_SPAUI_INFO              *info -
 *     SPAUI extensions information - - preamble- ipg
 *     (inter-packet gap)- channelized interface configuration-
 *     crc mode
 * REMARKS:
 *   1. This is a SPAUI extension to standard XAUI
 *   interface.2. IPG configuration is only applicable in TX
 *   direction - will be ignored for RX configuration.3. The
 *   get function is not symmetric to the set function: both
 *   RX and TX settings are returned (direction_ndx is not
 *   passed).
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_spaui_conf_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_SPAUI_INFO              *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_spaui_conf_set" API.
 *     Refer to "soc_pb_nif_spaui_conf_set" API for details.
*********************************************************************/
uint32
  soc_pb_nif_spaui_conf_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_SPAUI_INFO              *info_rx,
    SOC_SAND_OUT SOC_PB_NIF_SPAUI_INFO              *info_tx
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_gmii_conf_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets SGMII/1000BASE-X interface configuration
 * INPUT:
 *   SOC_SAND_IN  int                  unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID         nif_ndx -
 *     Interface index. SGMII Range: 3000 - 3031.
 *     (SOC_PB_NIF_ID(SGMII, 0-31)). QSGMII Range: 4000 - 4063.
 *     (SOC_PB_NIF_ID(QSGMII, 0-63))
 *   SOC_SAND_IN  SOC_PB_NIF_GMII_INFO           *info -
 *     SGMII-specific configuration.
 * REMARKS:
 *   1. This API does not configure the SerDes. Dedicated API
 *   must be used for SerDes configuration. 2. The rate can
 *   also be configured via a separate API.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_gmii_conf_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID         nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_GMII_INFO           *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_gmii_conf_set" API.
 *     Refer to "soc_pb_nif_gmii_conf_set" API for details.
*********************************************************************/
uint32
  soc_pb_nif_gmii_conf_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_GMII_INFO              *info_tx
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_gmii_status_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets SGMII/1000BASE-X interface diagnostics status
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     Interface index. Range: 3000 - 3031. (SOC_PB_NIF_ID(SGMII,
 *     0-31))
 *   SOC_SAND_OUT SOC_PB_NIF_GMII_STAT              *status -
 *     SGMII interface status.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_gmii_status_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_GMII_STAT              *status
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_gmii_rate_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets SGMII interface link-rate.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     Interface index. Range: 3000 - 3031. (SOC_PB_NIF_ID(SGMII,
 *     0-31))
 *   SOC_SAND_IN  SOC_PB_NIF_GMII_RATE              rate -
 *     SGMII rate (10/100/1000 Mbps).
 * REMARKS:
 *   1. The SGMII rate is typically configured upon
 *   initialization by the sgmii_conf_set API, called by
 *   init_sequence_phase1.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_gmii_rate_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_GMII_RATE              rate
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_gmii_rate_set" API.
 *     Refer to "soc_pb_nif_gmii_rate_set" API for details.
*********************************************************************/
uint32
  soc_pb_nif_gmii_rate_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_GMII_RATE              *rate
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_ilkn_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the Interlaken interface
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx -
 *     Interlaken Index. Range: 5000 - 5001. (ILKN-A/ILKN-B).
 *     Can also use SOC_PB_NIF_ID(ILKN, 0 - 1).
 *   SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx -
 *     Defines whether the configuration refers to RX, TX or
 *     both.
 *   SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO               *info -
 *     Interlaken Configuration
 * REMARKS:
 *   This API does not set Flow-Control-related configuration
 *   for Interlaken. This configuration is covered in the FC
 *   module.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_ilkn_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO               *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_nif_ilkn_set" API.
 *     Refer to "soc_pb_nif_ilkn_set" API for details.
*********************************************************************/
uint32
  soc_pb_nif_ilkn_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_OUT SOC_PB_NIF_ILKN_INFO               *info_rx,
    SOC_SAND_OUT SOC_PB_NIF_ILKN_INFO               *info_tx
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_ilkn_diag
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Reads the Interlaken status from the ILKN interrupt
 *   register and from other status registers. Clears the
 *   interrupts only if requested.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx -
 *     Interlaken Index. Range: 5000 - 5001. (ILKN-A/ILKN-B).
 *     Can also use SOC_PB_NIF_ID(ILKN, 0 - 1).
 *   SOC_SAND_IN  uint8                      clear_interrupts -
 *     If TRUE, the interrupts are cleared after reading
 *   SOC_SAND_OUT SOC_PB_NIF_ILKN_DIAG_INFO          *info -
 *     Interlaken Diagnostics information
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_ilkn_diag(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID                 ilkn_ndx,
    SOC_SAND_IN  uint8                      clear_interrupts,
    SOC_SAND_OUT SOC_PB_NIF_ILKN_DIAG_INFO          *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_stat_activity_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Indicates whether the interface is configured and
 *   powered up
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     NIF Port index. Range: SOC_PB_NIF_ID(NIF-Type, 0 - 63).
 *   SOC_SAND_OUT SOC_PB_NIF_ACTIVITY_STATUS         *info -
 *     NIF-port link status and activity
 * REMARKS:
 *   1. Not applicable for ILKN 2. All the indications are
 *   cleared on read 3. Only the indications for the
 *   specified interface are cleared 4. For a status
 *   read/clear on all NIFs in single-access (per MALG), use
 *   soc_pb_nif_stat_activity_all_get
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_stat_activity_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_ACTIVITY_STATUS         *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_stat_activity_all_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Indicates whether the interface is configured and
 *   powered up. All interfaces are read at once.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT  SOC_PB_NIF_ACTIVITY_STATUS_ALL     *info -
 *     Per NIF-port link status and activity
 * REMARKS:
 *   1. Not applicable for ILKN 2. All the indications are
 *   cleared on read 3. The API does not check if the
 *   interface is configured and enabled, for efficiency
 *   reasons. For a disabled NIF, the indication should be
 *   ignored.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_stat_activity_all_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT  SOC_PB_NIF_ACTIVITY_STATUS_ALL     *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_link_status_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets link status, and whether there was change in the
 *   status.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     Network interface index. SOC_PB_NIF_ID(NIF-Type, 0 - 63).
 *   SOC_SAND_OUT SOC_PB_NIF_LINK_STATUS             *link_status -
 *     link status, and whether there was change in the status
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_link_status_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_LINK_STATUS             *link_status
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_synce_clk_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   The transmit clocks of all network interfaces are locked to a
 *   reference clock provided by an external CLKREF input pin.
 *   This function sets the SyncE recovered (reference) clock configuration.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_ID            clk_ndx -
 *     SOC_SAND_IN SOC_PB_NIF_SYNCE_CLK_ID clk_ndx
 *   SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK               *clk -
 *     ndx - Recovered Clock Index. Range: 0 - 3. SOC_SAND_IN
 *     SOC_PB_NIF_SYNCE_CLK *clk - Recovered Clock configuration
 * REMARKS:
 *   1. soc_pb_nif_synce_mode_set must be used before this API to
 *   set the SYNCE signal mode. 2. If used, MAL Group B SYNCE
 *   clocks must be explicitly enabled at the initialization
 *   stage.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_synce_clk_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_ID            clk_ndx,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK               *clk
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_nif_synce_clk_set"
 *     API.
 *     Refer to "soc_pb_nif_synce_clk_set" API for details.
*********************************************************************/
uint32
  soc_pb_nif_synce_clk_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_ID            clk_ndx,
    SOC_SAND_OUT SOC_PB_NIF_SYNCE_CLK               *clk
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_synce_mode_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Define the Recovered Clock signal to be transmitted on
 *   the Soc_petra external pins
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_SYNCE_MODE              mode -
 *     The SYNCE signal mode.
 * REMARKS:
 *   If used, MAL Group B SYNCE clocks must be explicitly
 *   enabled at the initialization stage.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_synce_mode_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_MODE              mode
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_nif_synce_mode_set" API.
 *     Refer to "soc_pb_nif_synce_mode_set" API for details.
*********************************************************************/
uint32
  soc_pb_nif_synce_mode_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_NIF_SYNCE_MODE              *mode
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_ptp_clk_reset_value_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Reads the PTP clock current reset time value at high
 *   resolution (nanoseconds).
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT  SOC_PB_NIF_PTP_CLK_RST_VAL         *info -
 *     The PTP clock current reset time value. Units:
 *     Nanoseconds.
 * REMARKS:
 *   The current reset value can also be read using the
 *   nif_ptp_clk_get API, at lower resolution (msec)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_ptp_clk_reset_value_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_NIF_PTP_CLK_RST_VAL         *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_ptp_clk_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the PTP Clock configuration for the Precision Time
 *   Protocol
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_PTP_CLK_INFO            *info -
 *     PTP Clock Configuration: frequency, reset-value and
 *     auto-increment.
 * REMARKS:
 *   When reading the configuration using het
 *   nif_ptp_clk_get, the reset value indicates current value
 *   and not originally-configured. This value can also be
 *   read using nif_ptp_clk_reset_value_get.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_ptp_clk_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_PTP_CLK_INFO            *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_nif_ptp_clk_set"
 *     API.
 *     Refer to "soc_pb_nif_ptp_clk_set" API for details.
*********************************************************************/
uint32
  soc_pb_nif_ptp_clk_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_NIF_PTP_CLK_INFO            *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_ptp_conf_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable/disable and configure parameters for the
 *   Precision Time Protocol
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     NIF Port index. SOC_PB_NIF_ID(NIF-Type, 0 - 63).
 *   SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx -
 *     Defines whether the configuration refers to RX, TX or
 *     both.
 *   SOC_SAND_IN  SOC_PB_NIF_PTP_INFO                *info -
 *     Enable/disable, and set configuration, of the Precision
 *     Time Protocol (PTP) on the specified MAL in the
 *     specified direction.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_ptp_conf_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_PB_NIF_PTP_INFO                *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_nif_ptp_conf_set"
 *     API.
 *     Refer to "soc_pb_nif_ptp_conf_set" API for details.
*********************************************************************/
uint32
  soc_pb_nif_ptp_conf_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_OUT SOC_PB_NIF_PTP_INFO                *info_rx,
    SOC_SAND_OUT SOC_PB_NIF_PTP_INFO                *info_tx
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_elk_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Select the MALG used for the External Lookup interface
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_ELK_INFO                *info -
 *     External Lookup Interface (ELK) configuration
 * REMARKS:
 *   1. The consumed MAL cannot be used for any Network
 *   Interface 2. If ILKN-A is enabled, MAL-0 cannot be used
 *   for ELK. If ILKN-B is enabled, ELK can be set on MAL-14
 *   (only).
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_elk_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_ELK_INFO                *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_nif_elk_set" API.
 *     Refer to "soc_pb_nif_elk_set" API for details.
*********************************************************************/
uint32
  soc_pb_nif_elk_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_NIF_ELK_INFO                *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_fatp_mode_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets Fat-pipe configuration mode
 * INPUT:
 *   SOC_SAND_IN  int                  unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_FATP_MODE_INFO      *info -
 *     Fat-pipe configuration mode. Can be disabled, or enabled
 *     in one of the supported modes.
 * REMARKS:
 *   1. This API must be called before setting any Fat-pipe
 *   related configuration
 *   2. If a Fat Pipe is enabled,
 *   the resources of OFP-s 1-12 are consumedby the Fat Pipe,
 *   i.e. OFP-s 1-12 cannot be used elsewhere
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_fatp_mode_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE_INFO      *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_nif_fatp_mode_set"
 *     API.
 *     Refer to "soc_pb_nif_fatp_mode_set" API for details.
*********************************************************************/
uint32
  soc_pb_nif_fatp_mode_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PB_NIF_FATP_MODE_INFO      *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_fatp_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets Fat-pipe configuration mode
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_NIF_FATP_ID                 fatp_ndx -
 *     Fat-pipe index. According to the NIF_FATP_MODE: For 3x4:
 *     Range: 1 - 3. For 2x6: Range: 1 - 2. For 1x12: Range: 1 -
 *     1.
 *   The OFP port that is used as the Fat pipe destination is
 *   derived from the Fat-pipe index: OFP 1 for Fat-pipe A, OFP 2 for Fat-pipe B,
 *        OFP 3 for Fat-pipe C.
 *   SOC_SAND_IN  SOC_PB_NIF_FATP_INFO               *info -
 *     Fat-pipe configuration.
 * REMARKS:
 *   1. Fat-pipe mode must be set (soc_pb_nif_fatp_mode_set API)
 *   before calling this API.
 *   If the TM-port processing is Ethernet, the mapping from TM-Port
 *   to PP-Port must be set before calling this API.
 *   2. If a Fat Pipe is enabled,
 *   the resources of OFP-s 1-12 are consumedby the Fat Pipe,
 *   i.e. OFP-s 1-12 cannot be used elsewhere
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_fatp_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID                 fatp_ndx,
    SOC_SAND_IN  SOC_PB_NIF_FATP_INFO               *info
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_nif_fatp_set" API.
 *     Refer to "soc_pb_nif_fatp_set" API for details.
*********************************************************************/
uint32
  soc_pb_nif_fatp_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID                 fatp_ndx,
    SOC_SAND_OUT SOC_PB_NIF_FATP_INFO               *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_counter_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets Value of statistics counter of the NIF.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     NIF Port index. Range: SOC_PB_NIF_ID(NIF-Type, 0 - 63).
 *   SOC_SAND_IN  SOC_PB_NIF_COUNTER_TYPE            counter_type -
 *     Counter Type.
 *   SOC_SAND_OUT SOC_SAND_64CNT                     *counter_val -
 *     Counter Value.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_counter_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  SOC_PB_NIF_COUNTER_TYPE            counter_type,
    SOC_SAND_OUT SOC_SAND_64CNT                     *counter_val
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_all_counters_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets Value of statistics counter of the NIF.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx -
 *     NIF Port index. Range: SOC_PB_NIF_ID(NIF-Type, 0 - 63).
 *   SOC_SAND_IN  uint8            non_data_also -
 *     If set, reads also Pause Frames and PTP counters
 *   SOC_SAND_OUT SOC_SAND_64CNT                     counter_val -
 *     SOC_PB_NIF_NOF_COUNTERS] - Counters Values
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_all_counters_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID             nif_ndx,
    SOC_SAND_IN  uint8                      non_data_also,
    SOC_SAND_OUT SOC_SAND_64CNT                     counter_val[SOC_PB_NIF_NOF_COUNTERS]
  );

/*********************************************************************
* NAME:
 *   soc_pb_nif_all_nifs_all_counters_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets Statistics Counters for all the NIF-s in the
 *   device.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8            non_data_also -
 *     If set, reads also Pause Frames and PTP counters
 *   SOC_SAND_OUT SOC_SAND_64CNT                     counters_val -
 *     [SOC_PETRA_IF_NOF_NIFS][SOC_PB_NIF_NOF_COUNTERS] - Counters
 *     Values, all NIF-s.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_nif_all_nifs_all_counters_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint8                      non_data_also,
    SOC_SAND_OUT SOC_SAND_64CNT                     counters_val[SOC_PB_NIF_NOF_NIFS][SOC_PB_NIF_NOF_COUNTERS]
  );

void
  SOC_PB_NIF_STATE_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_STATE_INFO *info
  );

void
  SOC_PB_NIF_LANES_SWAP_clear(
    SOC_SAND_OUT SOC_PB_NIF_LANES_SWAP *info
  );

void
  SOC_PB_NIF_MAL_TOPOLOGY_clear(
    SOC_SAND_OUT SOC_PB_NIF_MAL_TOPOLOGY *info
  );

void
  SOC_PB_NIF_MAL_BASIC_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_MAL_BASIC_INFO *info
  );

void
  SOC_PB_NIF_PREAMBLE_COMPRESS_clear(
    SOC_SAND_OUT SOC_PB_NIF_PREAMBLE_COMPRESS *info
  );

void
  SOC_PB_NIF_IPG_COMPRESS_clear(
    SOC_SAND_OUT SOC_PB_NIF_IPG_COMPRESS *info
  );

void
  SOC_PB_NIF_CHANNELIZED_clear(
    SOC_SAND_OUT SOC_PB_NIF_CHANNELIZED *info
  );

void
  SOC_PB_NIF_FAULT_RESPONSE_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_FAULT_RESPONSE_INFO *info
  );

void
  SOC_PB_NIF_SPAUI_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_SPAUI_INFO *info
  );

void
  SOC_PB_NIF_GMII_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_GMII_INFO *info
  );

void
  SOC_PB_NIF_GMII_STAT_clear(
    SOC_SAND_OUT SOC_PB_NIF_GMII_STAT *info
  );

void
  SOC_PB_NIF_ILKN_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_ILKN_INFO *info
  );

void
  SOC_PB_NIF_ILKN_INT_clear(
    SOC_SAND_OUT SOC_PB_NIF_ILKN_INT *info
  );

void
  SOC_PB_NIF_ILKN_STAT_clear(
    SOC_SAND_OUT SOC_PB_NIF_ILKN_STAT *info
  );

void
  SOC_PB_NIF_ILKN_DIAG_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_ILKN_DIAG_INFO *info
  );

void
  SOC_PB_NIF_STAT_LINK_ACTIVITY_clear(
    SOC_SAND_OUT SOC_PB_NIF_STAT_LINK_ACTIVITY *info
  );

void
  SOC_PB_NIF_ACTIVITY_STATUS_clear(
    SOC_SAND_OUT SOC_PB_NIF_ACTIVITY_STATUS *info
  );

void
  SOC_PB_NIF_ACTIVITY_STATUS_ALL_clear(
    SOC_SAND_OUT SOC_PB_NIF_ACTIVITY_STATUS_ALL *info
  );

void
  SOC_PB_NIF_LINK_STATUS_clear(
    SOC_SAND_OUT SOC_PB_NIF_LINK_STATUS *info
  );

void
  SOC_PB_NIF_SYNCE_CLK_clear(
    SOC_SAND_OUT SOC_PB_NIF_SYNCE_CLK *info
  );

void
  SOC_PB_NIF_PTP_CLK_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_PTP_CLK_INFO *info
  );

void
  SOC_PB_NIF_PTP_CLK_RST_VAL_clear(
    SOC_SAND_OUT SOC_PB_NIF_PTP_CLK_RST_VAL *info
  );

void
  SOC_PB_NIF_PTP_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_PTP_INFO *info
  );

void
  SOC_PB_NIF_ELK_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_ELK_INFO *info
  );

void
  SOC_PB_NIF_FATP_MODE_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_FATP_MODE_INFO *info
  );

/*
 *	Convert from Soc_petra NIF-id to internal representation (0 - 63).
 *  Expects PB-style indexing
 */
uint32
  soc_pb_nif2intern_id(
    SOC_PETRA_INTERFACE_ID  soc_pb_nif_id
  );

/*
 *	Converts from a Network interface offset (0 - 63),
 *  to PB-stype Soc_petra-NIF-id, given the NIF type.
 *  For example SOC_PB_NIF_TYPE_XAUI with offset 3 is converted to
 *  SOC_PETRA_NIF_ID_XAUI_3
 */
SOC_PETRA_INTERFACE_ID
  soc_pb_nif_offset2nif_id(
    SOC_SAND_IN  SOC_PB_NIF_TYPE       nif_type,
    SOC_SAND_IN  uint32         nif_offset
  );

/*
 *	Converts from an internal indexing (0 - 63)
 *  to PB-stype Soc_petra-NIF-id, given the NIF type
 */
SOC_PETRA_INTERFACE_ID
  soc_pb_nif_intern2nif_id(
    SOC_SAND_IN  SOC_PB_NIF_TYPE       nif_type,
    SOC_SAND_IN  uint32         internal_id
  );

void
  SOC_PB_NIF_FATP_INFO_clear(
    SOC_SAND_OUT SOC_PB_NIF_FATP_INFO *info
  );

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_NIF_MALG_ID_to_string(
    SOC_SAND_IN  SOC_PB_NIF_MALG_ID enum_val
  );

const char*
  SOC_PB_NIF_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_TYPE enum_val
  );

const char*
  SOC_PB_NIF_PREAMBLE_SIZE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_PREAMBLE_SIZE enum_val
  );

const char*
  SOC_PB_NIF_IPG_DIC_MODE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_IPG_DIC_MODE enum_val
  );

const char*
  SOC_PB_NIF_CRC_MODE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_CRC_MODE enum_val
  );

const char*
  SOC_PB_NIF_LINK_FAULT_LOCATION_to_string(
    SOC_SAND_IN  SOC_PB_NIF_LINK_FAULT_LOCATION enum_val
  );

const char*
  SOC_PB_NIF_FAULT_RESPONSE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_FAULT_RESPONSE enum_val
  );

const char*
  SOC_PB_NIF_1GE_MODE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_1GE_MODE enum_val
  );

const char*
  SOC_PB_NIF_GMII_RATE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_GMII_RATE enum_val
  );

const char*
  SOC_PB_NIF_GMII_RF_to_string(
    SOC_SAND_IN  SOC_PB_NIF_GMII_RF enum_val
  );

const char*
  SOC_PB_NIF_ILKN_ID_to_string(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_ID enum_val
  );

const char*
  SOC_PB_NIF_SYNCE_CLK_DIV_to_string(
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_DIV enum_val
  );

const char*
  SOC_PB_NIF_SYNCE_CLK_ID_to_string(
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK_ID enum_val
  );

const char*
  SOC_PB_NIF_SYNCE_MODE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_MODE enum_val
  );

const char*
  SOC_PB_NIF_FATP_ID_to_string(
    SOC_SAND_IN  SOC_PB_NIF_FATP_ID enum_val
  );

const char*
  SOC_PB_NIF_FATP_MODE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE enum_val
  );

const char*
  SOC_PB_NIF_COUNTER_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_NIF_COUNTER_TYPE enum_val
  );

void
  SOC_PB_NIF_STATE_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_STATE_INFO *info
  );

void
  SOC_PB_NIF_LANES_SWAP_print(
    SOC_SAND_IN  SOC_PB_NIF_LANES_SWAP *info
  );

void
  SOC_PB_NIF_MAL_TOPOLOGY_print(
    SOC_SAND_IN  SOC_PB_NIF_MAL_TOPOLOGY *info
  );

void
  SOC_PB_NIF_MAL_BASIC_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_MAL_BASIC_INFO *info
  );

void
  SOC_PB_NIF_PREAMBLE_COMPRESS_print(
    SOC_SAND_IN  SOC_PB_NIF_PREAMBLE_COMPRESS *info
  );

void
  SOC_PB_NIF_IPG_COMPRESS_print(
    SOC_SAND_IN  SOC_PB_NIF_IPG_COMPRESS *info
  );

void
  SOC_PB_NIF_CHANNELIZED_print(
    SOC_SAND_IN  SOC_PB_NIF_CHANNELIZED *info
  );

void
  SOC_PB_NIF_FAULT_RESPONSE_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_FAULT_RESPONSE_INFO *info
  );

void
  SOC_PB_NIF_SPAUI_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_SPAUI_INFO *info
  );

void
  SOC_PB_NIF_GMII_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_GMII_INFO *info
  );

void
  SOC_PB_NIF_GMII_STAT_print(
    SOC_SAND_IN  SOC_PB_NIF_GMII_STAT *info
  );

void
  SOC_PB_NIF_ILKN_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INFO *info
  );

void
  SOC_PB_NIF_ILKN_INT_print(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_INT *info
  );

void
  SOC_PB_NIF_ILKN_STAT_print(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_STAT *info
  );

void
  SOC_PB_NIF_ILKN_DIAG_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_ILKN_DIAG_INFO *info
  );

void
  SOC_PB_NIF_STAT_LINK_ACTIVITY_print(
    SOC_SAND_IN  SOC_PB_NIF_STAT_LINK_ACTIVITY *info
  );

void
  SOC_PB_NIF_ACTIVITY_STATUS_print(
    SOC_SAND_IN  SOC_PB_NIF_ACTIVITY_STATUS *info
  );

void
  SOC_PB_NIF_ACTIVITY_STATUS_ALL_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PB_NIF_ACTIVITY_STATUS_ALL *info
  );

void
  SOC_PB_NIF_LINK_STATUS_print(
    SOC_SAND_IN  SOC_PB_NIF_LINK_STATUS *info
  );

void
  SOC_PB_NIF_SYNCE_CLK_print(
    SOC_SAND_IN  SOC_PB_NIF_SYNCE_CLK *info
  );

void
  SOC_PB_NIF_PTP_CLK_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_PTP_CLK_INFO *info
  );

void
  SOC_PB_NIF_PTP_CLK_RST_VAL_print(
    SOC_SAND_IN  SOC_PB_NIF_PTP_CLK_RST_VAL *info
  );

void
  SOC_PB_NIF_PTP_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_PTP_INFO *info
  );

void
  SOC_PB_NIF_ELK_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_ELK_INFO *info
  );

void
  SOC_PB_NIF_FATP_MODE_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_FATP_MODE_INFO *info
  );

void
  SOC_PB_NIF_FATP_INFO_print(
    SOC_SAND_IN  SOC_PB_NIF_FATP_INFO *info
  );

void
  SOC_PB_NIF_ALL_STATISTIC_COUNTERS_print(
    SOC_SAND_IN SOC_SAND_64CNT                        all_counters[SOC_PB_NIF_NOF_COUNTERS]
  );

void
  SOC_PB_NIF_ALL_NIFS_ALL_STATISTIC_COUNTERS_print(
    SOC_SAND_IN     int                        unit,
    SOC_SAND_INOUT  SOC_SAND_64CNT                        all_counters[SOC_PB_NIF_NOF_NIFS][SOC_PB_NIF_NOF_COUNTERS]
  );

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_API_NIF_INCLUDED__*/
#endif

