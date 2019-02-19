/* $Id: petra_serdes.h,v 1.7 Broadcom SDK $
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


#ifndef __SOC_PETRA_SERDES_INCLUDED__
/* { */
#define __SOC_PETRA_SERDES_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_serdes.h>
#include <soc/dpp/Petra/petra_chip_regs.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: petra_serdes.h,v 1.7 Broadcom SDK $
 * The internal values after calculation
 */
#define SOC_PETRA_SRD_TX_PRE_MAX   7
#define SOC_PETRA_SRD_TX_POST_MAX  15
#define SOC_PETRA_SRD_TX_AMP_MAX   31
#define SOC_PETRA_SRD_TX_MAIN_MAX  31

/*
 *  Maximal attenuation - 12Db.
 */
#define SOC_PETRA_SRD_TX_ATTEN_X10_MAX  120
#define SOC_PETRA_SRD_TX_ATTEN_INVALID  (SOC_PETRA_SRD_TX_ATTEN_X10_MAX+1)

/* } */

/*************
 * MACROS    *
 *************/
/* { */
/*
 *  TRUE if the SerDes lane belongs to NIF (not including Combo)
 */
#define SOC_PETRA_SRD_IS_NIF_LANE(lane_id)      \
  (                                         \
    SOC_SAND_IS_VAL_IN_RANGE(lane_id, 0, 11) || \
    SOC_SAND_IS_VAL_IN_RANGE(lane_id, 16, 27)   \
  )

#define SOC_PETRA_SRD_FIRST_FABRIC_LANE (32)

/*
 *  TRUE if the SerDes lane belongs to Fabric (not including Combo)
 */
#define SOC_PETRA_SRD_IS_FABRIC_LANE(lane_id)      \
  SOC_SAND_NUM2BOOL(SOC_SAND_IS_VAL_IN_RANGE(lane_id, SOC_PETRA_SRD_FIRST_FABRIC_LANE, (SOC_PETRA_SRD_NOF_LANES-1)))

#define SOC_PETRA_SRD_IS_COMBO_0_LANE(lane_id)    \
SOC_SAND_NUM2BOOL(                                \
    SOC_SAND_IS_VAL_IN_RANGE(lane_id, 12, 15)     \
  )

#define SOC_PETRA_SRD_IS_COMBO_1_LANE(lane_id)    \
SOC_SAND_NUM2BOOL(                                \
    SOC_SAND_IS_VAL_IN_RANGE(lane_id, 28, 31)     \
  )

#define SOC_PETRA_SRD_IS_COMBO_BY_IDX(lane_id, combo_idx)    \
SOC_SAND_NUM2BOOL(                                  \
    ( ((combo_idx) == SOC_PETRA_COMBO_QRTT_0) &&    \
      (SOC_PETRA_SRD_IS_COMBO_0_LANE(lane_id)) ) || \
      ( ((combo_idx) == SOC_PETRA_COMBO_QRTT_1) &&  \
      (SOC_PETRA_SRD_IS_COMBO_1_LANE(lane_id)) )    \
  )

/*
 *  TRUE if the SerDes lane belongs to a COMBO quartet -
 *  can be either NIF or Fabric.
 */
#define SOC_PETRA_SRD_IS_COMBO_LANE(lane_id)      \
  (SOC_PETRA_SRD_IS_COMBO_0_LANE(lane_id) || SOC_PETRA_SRD_IS_COMBO_1_LANE(lane_id))
/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  /*
   *  Full Data-rate - as it is.
   */
  SOC_PETRA_SRD_RATE_DIVISOR_FULL=0,
  /*
   *  Half Data-rate - divide full data rate by 2.
   */
  SOC_PETRA_SRD_RATE_DIVISOR_HALF=1,
  /*
   *  Quarter Data-rate - divide full data rate by 4.
   */
  SOC_PETRA_SRD_RATE_DIVISOR_QUARTER=2,
  /*
   *  Last value.
   */
  SOC_PETRA_SRD_NOF_RATE_DIVISORS=3
}SOC_PETRA_SRD_RATE_DIVISOR;

typedef struct  {
  uint32                 pll_m;
  uint32                 pll_n;
  SOC_PETRA_SRD_RATE_DIVISOR   dvsr;
} SOC_PETRA_SRD_RATE_INTERNAL;

typedef struct  {
  /* Pre-emphasis */
  uint8 ipre;
  /* Main */
  uint8 imain;
  /* Post-emphasis */
  uint8 ipost;
  /* Amplitude */
  uint8 amp;
  /* Tap-set 8 MS-bits*/
  uint8 tap_hi;
  /* Tap-set 8 LS-bits*/
  uint8 tap_low;
} SOC_PETRA_SRD_TX_PHYS_INTERNAL;

/*
*  Per media-type physical parameters configuration,
*  Different values are set for different SerDes rates.
*  The ranges below define these ranges
*/
typedef enum
{
  /*
  *  Below 3.2 Gbps
  *  Includes all rates from 3125Mbps and below.
  */
  SOC_PETRA_SRD_RATE_RNG_BELOW_3200 = 0,
  /*
  *  Above 3.2 Gbps
  *  Includes all rates from 3750Mbps and above.
  */
  SOC_PETRA_SRD_RATE_RNG_ABOVE_3200 = 1,
  SOC_PETRA_SRD_NOF_RATE_RNGS       = 2
}SOC_PETRA_SRD_RATE_RNG;

/*
 *  Deviation, in percents, from the requested
 *  pre-emphasis and post-emphasis values.
 */
typedef struct  {
  uint32 pre;
  uint32 post;
  uint32 swing;
}SOC_PETRA_SRD_TX_EXPLCT_DEVIATION;

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
*     soc_petra_serdes_init
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
  soc_petra_serdes_init(
    SOC_SAND_IN  uint32                 unit
  );

/*
 *	If TRUE, the SerDes quartet is active (powered-on)
 */
uint8
  soc_petra_srd_is_qrtt_active(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN uint32 qrtt_id
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_rate_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set SerDes lane rate.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     SerDes lane index. Range: 0 - 59.
*  SOC_SAND_IN  uint32       rate -
*     The rate to configure for the specified lane. Range:
*     1000 - 6250. Units: Mbps.
*     Alternatively, a specific rate in Kbps can be specified,
*     see details in the Remarks below.
* REMARKS:
*     Two configuration modes are supported:
*     Option 1. By enumerator. In this case, each reference clock value must be set
*     to a value as defined in the SOC_PETRA_MGMT_SRD_REF_CLK enumerator.
*     For example, to set the a 156.25 MHz ref-clock, use the value: SOC_PETRA_MGMT_SRD_REF_CLK_156_25.
*     The SerDes rate configuration in this case is also an enumerator (SOC_PETRA_SRD_DATA_RATE),
*     and the internal configuration (m/n/divisor) is then chosen accordingly.
*     Option 2. By number. In this case, the exact ref-clock in kilohertz is specified
*     as part of the operation mode. For example, to set the a 156. MHz ref-clock,
*     use the value: 156250. Also, the SerDes rate configuration is specified in Kbps.
*     The internal configuration then uses m = n = 0, and the appropriate divisor.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_rate_set_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  uint32                  rate
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_rate_verify
* TYPE:
*   PROC
* FUNCTION:
*     Set SerDes lane rate.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     SerDes lane index. Range: 0 - 59.
*  SOC_SAND_IN  uint32       rate -
*     The rate to configure for the specified lane. Range:
*     1000 - 6250. Units: Mbps.
*     Alternatively, a specific rate in Kbps can be specified,
*     see details in the Remarks below.
* REMARKS:
*     Two configuration modes are supported:
*     Option 1. By enumerator. In this case, each reference clock value must be set
*     to a value as defined in the SOC_PETRA_MGMT_SRD_REF_CLK enumerator.
*     For example, to set the a 156.25 MHz ref-clock, use the value: SOC_PETRA_MGMT_SRD_REF_CLK_156_25.
*     The SerDes rate configuration in this case is also an enumerator (SOC_PETRA_SRD_DATA_RATE),
*     and the internal configuration (m/n/divisor) is then chosen accordingly.
*     Option 2. By number. In this case, the exact ref-clock in kilohertz is specified
*     as part of the operation mode. For example, to set the a 156. MHz ref-clock,
*     use the value: 156250. Also, the SerDes rate configuration is specified in Kbps.
*     The internal configuration then uses m = n = 0, and the appropriate divisor.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_rate_verify(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  uint32                  rate
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_rate_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set SerDes lane rate.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     SerDes lane index. Range: 0 - 59.
*  SOC_SAND_OUT uint32       *rate -
*     The rate to configure for the specified lane. Range:
*     1000 - 6250. Units: Mbps.
*     Alternatively, a specific rate in Kbps can be specified,
*     see details in the Remarks below.
* REMARKS:
*     Two configuration modes are supported:
*     Option 1. By enumerator. In this case, each reference clock value must be set
*     to a value as defined in the SOC_PETRA_MGMT_SRD_REF_CLK enumerator.
*     For example, to set the a 156.25 MHz ref-clock, use the value: SOC_PETRA_MGMT_SRD_REF_CLK_156_25.
*     The SerDes rate configuration in this case is also an enumerator (SOC_PETRA_SRD_DATA_RATE),
*     and the internal configuration (m/n/divisor) is then chosen accordingly.
*     Option 2. By number. In this case, the exact ref-clock in kilohertz is specified
*     as part of the operation mode. For example, to set the a 156. MHz ref-clock,
*     use the value: 156250. Also, the SerDes rate configuration is specified in Kbps.
*     The internal configuration then uses m = n = 0, and the appropriate divisor.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_rate_get_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT uint32                  *rate
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_tx_phys_params_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set SerDes Physical Parameters configuration, on the
*     transmitter side.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     The index of the SerDes lane to configure. Range: 0 -
*     59.
*  SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_CONF_MODE conf_mode_ndx -
*     The index of the transmitter physical parameters
*     configuration mode.
*  SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_INFO    *info -
*     Transmitter physical parameters, according to the
*     selected configuration mode.
* REMARKS:
*     Typically, the SerDes physical parameters can be
*     configured according to Media-type. Further refinement
*     may be needed in the following sequence: by-attenuation
*     or explicit, internal.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_tx_phys_params_set_unsafe(
    SOC_SAND_IN  uint32                   unit,
    SOC_SAND_IN  uint32                   lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_INFO      *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_tx_phys_params_verify
* TYPE:
*   PROC
* FUNCTION:
*     Set SerDes Physical Parameters configuration, on the
*     transmitter side.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     The index of the SerDes lane to configure. Range: 0 -
*     59.
*  SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_CONF_MODE conf_mode_ndx -
*     The index of the transmitter physical parameters
*     configuration mode.
*  SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_INFO    *info -
*     Transmitter physical parameters, according to the
*     selected configuration mode.
* REMARKS:
*     Typically, the SerDes physical parameters can be
*     configured according to Media-type. Further refinement
*     may be needed in the following sequence: by-attenuation
*     or explicit, internal.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_tx_phys_params_verify(
    SOC_SAND_IN  uint32                    unit,
    SOC_SAND_IN  uint32                    lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_CONF_MODE  conf_mode_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_tx_phys_params_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set SerDes Physical Parameters configuration, on the
*     transmitter side.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     The index of the SerDes lane to configure. Range: 0 -
*     59.
*  SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_CONF_MODE conf_mode_ndx -
*     The index of the transmitter physical parameters
*     configuration mode.
*  SOC_SAND_OUT SOC_PETRA_SRD_TX_PHYS_INFO    *info -
*     Transmitter physical parameters, according to the
*     selected configuration mode.
* REMARKS:
*     Typically, the SerDes physical parameters can be
*     configured according to Media-type. Further refinement
*     may be needed in the following sequence: by-attenuation
*     or explicit, internal.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_tx_phys_params_get_unsafe(
    SOC_SAND_IN  uint32                    unit,
    SOC_SAND_IN  uint32                    lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_CONF_MODE  conf_mode_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_TX_PHYS_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_polarity_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set, per direction (RX/TX/Both) whether to swap-polarity
*     on a certain lane.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     The index of the SerDes lane to configure. Range: 0-59.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     Direction identifier: Transmission (Tx), Reception (Rx)
*     or Both.
*  SOC_SAND_IN  uint8                 is_swap_polarity -
*     If TRUE, polarity is swapped.
*  SOC_SAND_IN  uint8                  update_orig_polarity_db -
*     If TRUE, the SW-DB of original swap-polarity is updated.
*     Normally, this should be the case.
*     In some cases, e.g. PRBS with old devices (SOC_SAND_FAP20V etc.),
*     The driver inverts the polarity internally for a specific task (e.g. PRBS).
*     In this cases, this field should be set to FALSE.
* REMARKS:
*     The get function is not symmetric to the set function:
*     both RX and TX settings are returned (direction_ndx is
*     not passed).
* RETURNS:
*       The get function is not entirely symmetric to the set function
*       (where only rx, tx or both directions can be defined). The get
*       function returns both directions.
*********************************************************************/
uint32
  soc_petra_srd_lane_polarity_set_unsafe(
    SOC_SAND_IN  uint32                  unit,
    SOC_SAND_IN  uint32                  lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  uint8                  is_swap_polarity,
    SOC_SAND_IN  uint8                  update_orig_polarity_db
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_polarity_verify
* TYPE:
*   PROC
* FUNCTION:
*     Set, per direction (RX/TX/Both) whether to swap-polarity
*     on a certain lane.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     The index of the SerDes lane to configure. Range: 0-59.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     Direction identifier: Transmission (Tx), Reception (Rx)
*     or Both.
*  SOC_SAND_IN  uint8                 is_swap_polarity -
*     If TRUE, polarity is swapped.
* REMARKS:
*     The get function is not symmetric to the set function:
*     both RX and TX settings are returned (direction_ndx is
*     not passed).
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_lane_polarity_verify(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  uint8                 is_swap_polarity
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_polarity_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set, per direction (RX/TX/Both) whether to swap-polarity
*     on a certain lane.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     The index of the SerDes lane to configure. Range: 0-59.
*  SOC_SAND_OUT uint8                 *is_swap_polarity_rx -
*     If TRUE, polarity is swapped for rx.
*  SOC_SAND_OUT uint8                 *is_swap_polarity_tx -
*     If TRUE, polarity is swapped for tx.
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*   1. Not entirely symmetric to the set function (where only rx,
*      tx or both directions can be defined). The get function returns
*      the both directions.
*********************************************************************/
uint32
  soc_petra_srd_lane_polarity_get_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT uint8                 *is_swap_polarity_rx,
    SOC_SAND_OUT uint8                 *is_swap_polarity_tx
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_power_state_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set the state of the lane - power-up/power-down.
*     CAUTION! This is a low-level API that operates directly on the SerDes,
*     regardless the attached logic (Fabric/NIF). For the correct power-up/power-down sequence,
*     including powering up/down the SerDes, use the relevant API-s:
*     fabric_link_on_off_set, nif_on_off_set.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     The index of the SerDes lane to configure. Range: 0 -
*     59.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     Direction identifier: Transmission (Tx), Reception (Rx)
*     or Both.
*  SOC_SAND_IN  SOC_PETRA_SRD_POWER_STATE     state -
*     Lane power-state (up/down).
* REMARKS:
*     1. Note that turning down the SerDes affects the
*     attached logic (Fabric/NIF). For the correct
*     power-up/power-down sequence, including powering up/down
*     the SerDes, refer to the relevant API-s
*     (fabric_link_on_off_set, nif_on_off_set). 2. The The get
*     function is not symmetric to the set function: both RX
*     and TX settings are returned (direction_ndx is not
*     passed).
* RETURNS:
*     Not entirely symmetric to the set function (where only rx,
*      tx or both directions can be defined). The get function returns
*      the both directions.
*********************************************************************/

uint32
  soc_petra_srd_lane_power_state_set_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_POWER_STATE     state
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_power_state_verify
* TYPE:
*   PROC
* FUNCTION:
*     Set the state of the lane - power-up/power-down.
*     CAUTION! This is a low-level API that operates directly on the SerDes,
*     regardless the attached logic (Fabric/NIF). For the correct power-up/power-down sequence,
*     including powering up/down the SerDes, use the relevant API-s:
*     fabric_link_on_off_set, nif_on_off_set.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     The index of the SerDes lane to configure. Range: 0 -
*     59.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     Direction identifier: Transmission (Tx), Reception (Rx)
*     or Both.
*  SOC_SAND_IN  SOC_PETRA_SRD_POWER_STATE     state -
*     Lane power-state (up/down).
* REMARKS:
*     1. Note that turning down the SerDes affects the
*     attached logic (Fabric/NIF). For the correct
*     power-up/power-down sequence, including powering up/down
*     the SerDes, refer to the relevant API-s
*     (fabric_link_on_off_set, nif_on_off_set). 2. The The get
*     function is not symmetric to the set function: both RX
*     and TX settings are returned (direction_ndx is not
*     passed).
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_srd_lane_power_state_verify(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_POWER_STATE     state
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_power_state_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set the state of the lane - power-up/power-down.
*     CAUTION! This is a low-level API that operates directly on the SerDes,
*     regardless the attached logic (Fabric/NIF). For the correct power-up/power-down sequence,
*     including powering up/down the SerDes, use the relevant API-s:
*     fabric_link_on_off_set, nif_on_off_set.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     The index of the SerDes lane to configure. Range: 0 -
*     59.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     Direction identifier: Transmission (Tx), Reception (Rx)
*     or Both.
*  SOC_SAND_OUT SOC_PETRA_SRD_POWER_STATE     *state_rx -
*     Lane rx power-state.
*  SOC_SAND_OUT SOC_PETRA_SRD_POWER_STATE     *state_tx -
*     Lane tx power-state.
* REMARKS:
*     1. Note that turning down the SerDes affects the
*     attached logic (Fabric/NIF). For the correct
*     power-up/power-down sequence, including powering up/down
*     the SerDes, refer to the relevant API-s
*     (fabric_link_on_off_set, nif_on_off_set). 2. The The get
*     function is not symmetric to the set function: both RX
*     and TX settings are returned (direction_ndx is not
*     passed).
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_srd_lane_power_state_get_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_POWER_STATE     *state_rx,
    SOC_SAND_OUT SOC_PETRA_SRD_POWER_STATE     *state_tx
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_star_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Performs a SerDes star initialization sequence. This
*     sequence includes in-out of reset sequence for the star
*     IPU, and the CMU-s of the specified SerDes quartets. It
*     also includes CMU trimming and power-up validation.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_SRD_STAR_ID         star_ndx -
*     The Index of the star containing the IPU to initialize.
*     Range: 0 - 3. (NIF-A/NIF-B/MAC-A/MAC-B).
*  SOC_SAND_IN  SOC_PETRA_SRD_STAR_INFO       *info -
*     SerDes star configuration.
* REMARKS:
*     1. This API does not include physical parameters
*     configuration. 2. This API does not change SerDes lane
*     power-state. Powering-up the SerDes is performed by a
*     dedicated API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_star_set_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_STAR_ID         star_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_STAR_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_star_verify
* TYPE:
*   PROC
* FUNCTION:
*     Performs a SerDes star initialization sequence. This
*     sequence includes in-out of reset sequence for the star
*     IPU, and the CMU-s of the specified SerDes quartets. It
*     also includes CMU trimming and power-up validation.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_SRD_STAR_ID         star_ndx -
*     The Index of the star containing the IPU to initialize.
*     Range: 0 - 3. (NIF-A/NIF-B/MAC-A/MAC-B).
*  SOC_SAND_IN  SOC_PETRA_SRD_STAR_INFO       *info -
*     SerDes star configuration.
* REMARKS:
*     1. This API does not include physical parameters
*     configuration. 2. This API does not change SerDes lane
*     power-state. Powering-up the SerDes is performed by a
*     dedicated API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_star_verify(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_STAR_ID         star_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_STAR_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_star_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Performs a SerDes star initialization sequence. This
*     sequence includes in-out of reset sequence for the star
*     IPU, and the CMU-s of the specified SerDes quartets. It
*     also includes CMU trimming and power-up validation.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_SRD_STAR_ID         star_ndx -
*     The Index of the star containing the IPU to initialize.
*     Range: 0 - 3. (NIF-A/NIF-B/MAC-A/MAC-B).
*  SOC_SAND_OUT SOC_PETRA_SRD_STAR_INFO       *info -
*     SerDes star configuration.
* REMARKS:
*     1. This API does not include physical parameters
*     configuration. 2. This API does not change SerDes lane
*     power-state. Powering-up the SerDes is performed by a
*     dedicated API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_star_get_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_STAR_ID         star_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_STAR_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_qrtt_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Performs a SerDes Quartet reset/initialization sequence.
*     The maximal supported rate of the SerDes quartet is set
*     as part of this initialization. Also, per-lane rate is
*     initialized according to the indicated rate.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 qrtt_ndx -
*     The Index of the SerDes quartet to initialize. Range: 0
*     - 14.
*  SOC_SAND_IN  SOC_PETRA_SRD_QRTT_INFO       *info -
*     SerDes quartet configuration: 1. is_active: if TRUE, the
*     quartet CMU is taken out-of-reset. Otherwise, the
*     quartet CMU remains in-reset.2. max_expected_lane_rate:
*     The maximal expected rate for any lane in the quartet.
*     Range: 1000 - 6250. Units: Mbps. Note: the actual rate
*     is configured per-lane. Only a subset of
*     max_expecter_in_qrtt_rate/lane-rate combinations is
*     legal.
* REMARKS:
*     1. This API does not change SerDes lane power-state.
*     Powering-up/down the SerDes is performed by a dedicated
*     API.2. Trimming validation is only performed on the
*     powered-up lanes
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_srd_qrtt_set_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 qrtt_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_QRTT_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_qrtt_verify
* TYPE:
*   PROC
* FUNCTION:
*     Performs a SerDes Quartet reset/initialization sequence.
*     The maximal supported rate of the SerDes quartet is set
*     as part of this initialization. Also, per-lane rate is
*     initialized according to the indicated rate.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 qrtt_ndx -
*     The Index of the SerDes quartet to initialize. Range: 0
*     - 14.
*  SOC_SAND_IN  SOC_PETRA_SRD_QRTT_INFO       *info -
*     SerDes quartet configuration: 1. is_active: if TRUE, the
*     quartet CMU is taken out-of-reset. Otherwise, the
*     quartet CMU remains in-reset.2. max_expected_lane_rate:
*     The maximal expected rate for any lane in the quartet.
*     Range: 1000 - 6250. Units: Mbps. Note: the actual rate
*     is configured per-lane. Only a subset of
*     max_expecter_in_qrtt_rate/lane-rate combinations is
*     legal.
* REMARKS:
*     1. This API does not change SerDes lane power-state.
*     Powering-up/down the SerDes is performed by a dedicated
*     API.2. Trimming validation is only performed on the
*     powered-up lanes
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_srd_qrtt_verify(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 qrtt_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_QRTT_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_qrtt_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Performs a SerDes Quartet reset/initialization sequence.
*     The maximal supported rate of the SerDes quartet is set
*     as part of this initialization. Also, per-lane rate is
*     initialized according to the indicated rate.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 qrtt_ndx -
*     The Index of the SerDes quartet to initialize. Range: 0
*     - 14.
*  SOC_SAND_OUT SOC_PETRA_SRD_QRTT_INFO       *info -
*     SerDes quartet configuration: 1. is_active: if TRUE, the
*     quartet CMU is taken out-of-reset. Otherwise, the
*     quartet CMU remains in-reset.2. max_expected_lane_rate:
*     The maximal expected rate for any lane in the quartet.
*     Range: 1000 - 6250. Units: Mbps. Note: the actual rate
*     is configured per-lane. Only a subset of
*     max_expecter_in_qrtt_rate/lane-rate combinations is
*     legal.
* REMARKS:
*     1. This API does not change SerDes lane power-state.
*     Powering-up/down the SerDes is performed by a dedicated
*     API.2. Trimming validation is only performed on the
*     powered-up lanes
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_srd_qrtt_get_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 qrtt_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_QRTT_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_all_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set SerDes parameters, for the selected SerDes.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_SRD_ALL_INFO        *info -
*     SerDes configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_all_set_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ALL_INFO        *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_all_verify
* TYPE:
*   PROC
* FUNCTION:
*     Set SerDes parameters, for the selected SerDes.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_SRD_ALL_INFO        *info -
*     SerDes configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_all_verify(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ALL_INFO        *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_all_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set SerDes parameters, for the selected SerDes.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_SRD_ALL_INFO        *info -
*     SerDes configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_all_get_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_OUT SOC_PETRA_SRD_ALL_INFO        *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_auto_equalize_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Perform auto-equalization process. This process targets
*     to achive optimal receiver configuration, resulting in
*     maximal eye-opening.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     SerDes lane index. Range: 0 - 59.
*  SOC_SAND_IN  SOC_PETRA_SRD_AEQ_MODE        mode -
*     SerDes Auto-equalization mode.
*  SOC_SAND_OUT SOC_PETRA_SRD_AEQ_STATUS      *status -
*     Indicates whether the auto-equalization process was
*     successful. If successful, the receiver configuration is
*     set. The resulting eye-heigt is also indicated.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_auto_equalize_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_AEQ_MODE        mode,
    SOC_SAND_OUT SOC_PETRA_SRD_AEQ_STATUS      *status
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_rx_phys_params_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Receiver physical parameters.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     lane index. Range: 0 - 59.
*  SOC_SAND_IN  SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS *info -
*     receiver configuration.
* REMARKS:
*     1. The API is intended for the following sequence: 1.1
*     Run auto-equalization. 1.2 Read the resulting
*     configuration. 1.3 The values can be used as default
*     presets for conditions similar to the auto-equalization
*     conditions.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_rx_phys_params_set_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_rx_phys_params_verify
* TYPE:
*   PROC
* FUNCTION:
*     Receiver physical parameters.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     lane index. Range: 0 - 59.
*  SOC_SAND_IN  SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS *info -
*     receiver configuration.
* REMARKS:
*     1. The API is intended for the following sequence: 1.1
*     Run auto-equalization. 1.2 Read the resulting
*     configuration. 1.3 The values can be used as default
*     presets for conditions similar to the auto-equalization
*     conditions.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_rx_phys_params_verify(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_rx_phys_params_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Receiver physical parameters.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     lane index. Range: 0 - 59.
*  SOC_SAND_OUT SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS *info -
*     receiver configuration.
* REMARKS:
*     1. The API is intended for the following sequence: 1.1
*     Run auto-equalization. 1.2 Read the resulting
*     configuration. 1.3 The values can be used as default
*     presets for conditions similar to the auto-equalization
*     conditions.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_rx_phys_params_get_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS *info
  );
/*
 *  Read the internal rate configuration from the device
 */
uint32
  soc_petra_srd_internal_rate_get(
    SOC_SAND_IN  uint32                unit,
    SOC_SAND_IN  uint32                lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_RATE_INTERNAL  *rate
  );

/*
 *  Returns TRUE if and only if the SerDes lane
 *  with index 'lane_ndx' is towards the fabric.
 *  This depends on the index, and the COMBO
 *  quartet configuration (whether dedicated to the Fabric or the NIF)
 */
uint8
  soc_petra_srd_is_fabric_lane(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN uint32 lane_ndx
  );

const char*
  soc_petra_PETRA_SRD_RATE_DIVISOR_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_RATE_DIVISOR enum_val
  );

uint8
  soc_petra_srd_tx_phys_explicit_from_intern(
    SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS *tx_intern,
    SOC_SAND_IN  SOC_PETRA_SRD_RATE_RNG                 range,
    SOC_SAND_OUT SOC_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS  *tx_explct
  );

uint8
  soc_petra_srd_tx_phys_explicit_to_intern(
    SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS  *tx_explct,
    SOC_SAND_IN  SOC_PETRA_SRD_RATE_RNG                 range,
    SOC_SAND_OUT SOC_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS  *tx_intern,
    SOC_SAND_OUT SOC_PETRA_SRD_TX_EXPLCT_DEVIATION      *deviation
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_qrtt_sync_fifo_en
* TYPE:
*   PROC
* FUNCTION:
*     Quartet sync fifo enable config. If enabled, Tx data from all lanes
*     will be synced to XCK[0] (from lane 0) before driven to SerDes.
*     Otherwise, synchronization is done per lane, using the
*     internal TxFIFO of the serdes.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 qrtt_id -
*     lane index. Range: 0 - 15.
*  SOC_SAND_IN uint8                  enable -
*     If true, sync per quartet. Else, sync per lane.
* REMARKS:
*      none.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_qrtt_sync_fifo_en(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 qrtt_id,
    SOC_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_init
* TYPE:
*   PROC
* FUNCTION:
*     Initialize serdes module (including serder register access).
*     This function must be called before accessing the serdes registers.
* INPUT:
*      none.
* REMARKS:
*      none.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_init(void);

/*
 * Returns the SerDes rate in Kbps.
 * The returned value is the numeric (Kbps) value,
 * regardless the rate configuration mode (ENUM/NUMERIC)
 */
uint32
  soc_petra_srd_rate_calc_kbps(
    SOC_SAND_IN  uint32   unit,
    SOC_SAND_IN  uint32   lane_ndx
  );

#if SOC_PETRA_DEBUG

void
  soc_petra_PETRA_SRD_TX_PHYS_INTERNAL_print(
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_INTERNAL *info
  );
#endif
/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_SERDES_INCLUDED__*/
#endif
