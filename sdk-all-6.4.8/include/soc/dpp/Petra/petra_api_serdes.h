/* $Id$
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


#ifndef __SOC_PETRA_API_SERDES_INCLUDED__
/* { */
#define __SOC_PETRA_API_SERDES_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* $Id$
 * Total Number of SerDes Lanes in Soc_petra.
 * 24 or 32 towards the NIF.
 * 36 or 28 accordingly - towards the Fabric.
 */
#define  SOC_PETRA_SRD_NOF_LANES 60


#define  SOC_PETRA_SRD_LANE_INVALID   SOC_PETRA_SRD_NOF_LANES

/*     Total Number of quartets is 15. (Every quartet consists
 *     of 4 SerDes Lanes - 60).
 */
#define  SOC_PETRA_SRD_NOF_QUARTETS 15

#define SOC_PETRA_SRD_LANES_PER_QRTT   (SOC_PETRA_SRD_NOF_LANES/SOC_PETRA_SRD_NOF_QUARTETS)

/*     The maximal allowed value for Pre-emphasis and
 *     Post-emphasis when setting SerDes TX physical parameters
 *     configuration in explicit mode.
 */
#define  SOC_PETRA_SRD_PRE_POST_PERCENT_MAX 200

/*
 *  TX physical parameters limits - swing amplitude, mV
 */
#define SOC_PETRA_SRD_TX_SWING_MV_MIN         100
#define SOC_PETRA_SRD_TX_SWING_MV_MAX         1500


/*     Maximal number of SerDes quartets in a SerDes star.     */
#define  SOC_PETRA_SRD_NOF_QRTTS_PER_STAR_MAX   4

/*     The number of SerDes quartet in MAC-B SerDes star.      */
#define  SOC_PETRA_SRD_NOF_QRTTS_PER_STAR_MAC_B 3

/*     Total Number of SerDes STAR-s.                          */
#define  SOC_PETRA_SRD_NOF_STARS SOC_PETRA_SRD_NOF_STAR_IDS

#define  SOC_PETRA_SRD_NOF_LANES_PER_QRTT \
  (SOC_PETRA_SRD_NOF_LANES/SOC_PETRA_SRD_NOF_QUARTETS)

/* } */

/*************
 * MACROS    *
 *************/
/* { */
/*
 *  Index conversions
 */
#define SOC_PETRA_SRD_LANE2STAR(ln_id)  \
  (SOC_PETRA_SRD_QRTT2STAR_ID(SOC_PETRA_SRD_LANE2QRTT_GLBL(ln_id)))

#define SOC_PETRA_SRD_QRTT2STAR_ID(qrt_id) \
  ((qrt_id)/SOC_PETRA_SRD_NOF_QRTTS_PER_STAR_MAX)

#define SOC_PETRA_SRD_QRTT2INNER_ID(qrt_id) \
  ((qrt_id)%SOC_PETRA_SRD_NOF_QRTTS_PER_STAR_MAX)

#define SOC_PETRA_SRD_QRTT2GLOBAL_ID(star_id, qrt_id) \
  ((star_id)*SOC_PETRA_SRD_NOF_QRTTS_PER_STAR_MAX + (qrt_id))

#define SOC_PETRA_SRD_LANE2GLOBAL_ID(star_id, qrt_id, ln_id) \
  ( (((star_id) * SOC_PETRA_SRD_NOF_QRTTS_PER_STAR_MAX) + (qrt_id))*SOC_PETRA_SRD_NOF_LANES_PER_QRTT + (ln_id))

#define SOC_PETRA_SRD_LANE2INNER_ID(ln_id) \
  ((ln_id)%SOC_PETRA_SRD_NOF_LANES_PER_QRTT)

#define SOC_PETRA_SRD_STAR2INSTANCE(star_id)  \
  ((star_id) + SOC_PETRA_REGS_SRD_INST_BASE + 1)

#define SOC_PETRA_SRD_LANE2QRTT_GLBL(ln_id)  \
  ((ln_id)/SOC_PETRA_SRD_LANES_PER_QRTT)

#define SOC_PETRA_SRD_LANE2INSTANCE(ln_id)  \
  (SOC_PETRA_SRD_STAR2INSTANCE(SOC_PETRA_SRD_LANE2STAR(ln_id)))

#define SOC_PETRA_SRD_QRTT2INSTANCE(qrtt_id)  \
  (SOC_PETRA_SRD_STAR2INSTANCE(SOC_PETRA_SRD_QRTT2STAR_ID(qrtt_id)))

#define SOC_PETRA_SRD_LANE2QRTT_INNER(ln_id)  \
  (SOC_PETRA_SRD_QRTT2INNER_ID(SOC_PETRA_SRD_LANE2QRTT_GLBL(ln_id)))

#define SOC_PETRA_SRD_STAR_FIRST_QRTT(star_id) \
  ((star_id)*SOC_PETRA_SRD_NOF_QRTTS_PER_STAR_MAX)

#define SOC_PETRA_SRD_QRTT_FIRST_LANE(qrtt_id_glbl) \
  ((qrtt_id_glbl)*SOC_PETRA_SRD_LANES_PER_QRTT)

#define SOC_PETRA_SRD_QRTT_LAST_LANE(qrtt_id_glbl) \
  (SOC_PETRA_SRD_QRTT_FIRST_LANE(qrtt_id_glbl) + SOC_PETRA_SRD_LANES_PER_QRTT - 1)

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */


typedef enum
{
  /*
   *  SerDes Star 0, 3 Quartets towards NIF-A, 1 shared
   *  (NIF-A/Fabric MAC-C)
   */
  SOC_PETRA_SRD_STAR_ID_0=0,
  /*
   *  SerDes Star 1, 3 Quartets towards NIF-B, 1 shared
   *  (NIF-B/Fabric MAC-C).
   */
  SOC_PETRA_SRD_STAR_ID_1=1,
  /*
   *  SerDes Star 2, 4 Quartets towards the Fabric (3 towards
   *  MAC-A, 1 towards MAC-B).
   */
  SOC_PETRA_SRD_STAR_ID_2=2,
  /*
   *  SerDes Star 3 (MAC-B), 3 Quartets towards the Fabric (2
   *  towards MAC-B, 1 towards MAC-C).
   */
  SOC_PETRA_SRD_STAR_ID_3=3,
  /*
   *  Total number of SerDes Stars
   */
  SOC_PETRA_SRD_NOF_STAR_IDS=4
}SOC_PETRA_SRD_STAR_ID;


typedef enum
{
  /*
   *  SerDes Data Rate: 1000.00 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_1000_00=0,
  /*
   *  SerDes Data Rate: 1041.67 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_1041_67=1,
  /*
   *  SerDes Data Rate: 1171.88 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_1171_88=2,
  /*
   *  SerDes Data Rate: 1250.00 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_1250_00=3,
  /*
   *  SerDes Data Rate: 1302.03 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_1302_03=4,
  /*
   *  SerDes Data Rate: 1333.33 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_1333_33=5,
  /*
   *  SerDes Data Rate: 1562.50 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_1562_50=6,
  /*
   *  SerDes Data Rate: 2343.75 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_2343_75=9,
  /*
   *  SerDes Data Rate: 2500.00 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_2500_00=10,
  /*
   *  SerDes Data Rate: 2604.16 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_2604_16=11,
  /*
   *  SerDes Data Rate: 2666.67 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_2666_67=12,
  /*
   *  SerDes Data Rate: 2083.33 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_2083_33=13,
  /*
   *  SerDes Data Rate: 3000.00 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_3000_00=14,
  /*
   *  SerDes Data Rate: 3125.00 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_3125_00=15,
  /*
   *  SerDes Data Rate: 3125.00 Mbps. Uses Full Data Rate
   *  divisor setting to reduce power consumption.
   *  Consult with Dune support before using this configuration
   */
  SOC_PETRA_SRD_DATA_RATE_3125_00_FDR=16,
  /*
   *  SerDes Data Rate: 3750.00 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_3750_00=17,
  /*
   *  SerDes Data Rate: 4000.00 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_4000_00=18,
  /*
   *  SerDes Data Rate: 4166.67 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_4166_67=19,
  /*
   *  SerDes Data Rate: 4687.50 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_4687_50=20,
  /*
   *  SerDes Data Rate: 5000.00 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_5000_00=21,
  /*
   *  SerDes Data Rate: 5208.33 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_5208_33=22,
  /*
   *  SerDes Data Rate: 5333.33 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_5333_33=23,
  /*
   *  SerDes Data Rate: 5833.33 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_5833_33=24,
  /*
   *  SerDes Data Rate: 6000.00 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_6000_00=25,
  /*
   *  SerDes Data Rate: 6250.00 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_6250_00=26,
  /*
   *  SerDes Data Rate: 4375.00 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_4375_00=27,
  /*
   *  SerDes Data Rate: 5468.75 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_5468_75=28,
  /*
   *  SerDes Data Rate: 4250.00 Mbps
   */
  SOC_PETRA_SRD_DATA_RATE_4250_00=29,
  /*
   *  Total number of SerDes data rates.
   */
  SOC_PETRA_SRD_NOF_DATA_RATES=30
}SOC_PETRA_SRD_DATA_RATE;

#define SOC_PETRA_SRD_IS_RATE_ENUM(data_rate_val) \
  SOC_SAND_NUM2BOOL(SOC_SAND_IS_VAL_IN_RANGE(data_rate_val, SOC_PETRA_SRD_DATA_RATE_1000_00, SOC_PETRA_SRD_NOF_DATA_RATES))

/*
 *	Numeric value limitations for SerDes rate
 */
#define SOC_PETRA_SRD_NUM_RATE_KBPS_MIN 1000000
#define SOC_PETRA_SRD_NUM_RATE_KBPS_MAX 6250000

/*
 *  An indication of un-configured, or invalid data-rate.
 */
#define SOC_PETRA_SRD_DATA_RATE_NONE  SOC_PETRA_SRD_NOF_DATA_RATES
#define SOC_PETRA_SRD_DATA_RATE_FIRST SOC_PETRA_SRD_DATA_RATE_1000_00

typedef enum
{
  /*
   *  The 2 communicating chips lay on the same board,
   *  therefore very minor Loss is expected.
   */
  SOC_PETRA_SRD_MEDIA_TYPE_CHIP2CHIP=0,
  /*
   *  The 2 communicating chips lay on a short back-plane or
   *  connected through a connector.
   */
  SOC_PETRA_SRD_MEDIA_TYPE_SHORT_BACKPLANE=1,
  /*
   *  The 2 communicating chips lay on a long back-plane, this
   *  derives a relatively high Loss.
   */
  SOC_PETRA_SRD_MEDIA_TYPE_LONG_BACKPLANE=2,
  /*
   *  Total Number of slew media types.
   */
  SOC_PETRA_SRD_NOF_MEDIA_TYPES=3
}SOC_PETRA_SRD_MEDIA_TYPE;

#define SOC_PETRA_SRD_MEDIA_TYPE_NONE SOC_PETRA_SRD_NOF_MEDIA_TYPES

typedef enum
{
  /*
   *  Transmitter physical parameters are set according to an
   *  internal representation.
   */
  SOC_PETRA_SRD_TX_PHYS_CONF_MODE_INTERNAL=0,
  /*
   *  Transmitter physical parameters are set using VODD/VODP
   *  values. When reading physical parameters (using the _get
   *  API with this configuration mode), PRE/POST-Emphasis
   *  values are also returned, in percents.
   */
  SOC_PETRA_SRD_TX_PHYS_CONF_MODE_EXPLICIT=1,
  /*
   *  Transmitter physical parameters are calculated and set,
   *  based on the lane attenuation.
   */
  SOC_PETRA_SRD_TX_PHYS_CONF_MODE_ATTEN=2,
  /*
   *  Transmitter physical parameters are calculated and set,
   *  based on the Media Type.
   */
  SOC_PETRA_SRD_TX_PHYS_CONF_MODE_MEDIA_TYPE=3,
  /*
   *  Total number of Transmitter physical parameters
   *  configuration modes.
   */
  SOC_PETRA_SRD_NOF_TX_PHYS_CONF_MODES=4
}SOC_PETRA_SRD_TX_PHYS_CONF_MODE;


typedef enum
{
  /*
   *  SerDes is powered down.
   */
  SOC_PETRA_SRD_POWER_STATE_DOWN=0,
  /*
   *  SerDes is powered up.
   */
  SOC_PETRA_SRD_POWER_STATE_UP=1,
  /*
   *  SerDes is powered up. When setting this state, the
   *  SerDes is validated after power-up. If needed. a re-lock
   *  sequence is performed to verify SerDes is active. Note:
   *  this is the recommended value for powering-up the
   *  SerDes.
   */
  SOC_PETRA_SRD_POWER_STATE_UP_AND_RELOCK=2,
  /*
   *  Total Number of SerDes power states.
   */
  SOC_PETRA_SRD_NOF_POWER_STATES=3
}SOC_PETRA_SRD_POWER_STATE;


typedef enum
{
  /*
   *  Auto-equalization mode: blind.
   */
  SOC_PETRA_SRD_AEQ_MODE_BLIND=0,
  /*
   *  Auto-equalization mode: steady-state.
   */
  SOC_PETRA_SRD_AEQ_MODE_STEADY_STATE=1,
  /*
   *  Total Number of auto-equalization modes.
   */
  SOC_PETRA_SRD_NOF_AEQ_MODES=2
}SOC_PETRA_SRD_AEQ_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The pre-emphasis on top of the emphasized signal, in
   *  percents. Applicable for SerDes rates above 3.125Gbps
   *  only. Range: 0 - 200.
   */
  uint32 pre;
  /*
   *  The post-emphasis on top of the emphasized signal, in
   *  percents. Post-emphasis=(Vodd/Vodp-1)*100%. Range: 0 -
   *  200.
   */
  uint32 post;
  /*
   *  Peak-to peak differential swing of the emphasized signal
   *  [Vodp]. Units: mV. Range: 100 - 1500.
   */
  uint32 swing;
}SOC_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Transmitter amplitude value- internal representation. An
   *  amplification factor for the entire transmit waveform.
   *  Range: 0 - 31.
   */
  uint8 amp;
  /*
   *  Transmitter main value - internal representation. A
   *  weight value for the non-emphasized bits. Range: 0 - 31.
   */
  uint8 main;
  /*
   *  Transmitter pre-emphasis value - internal
   *  representation. A weight value for the Pre-Curser
   *  emphasis. Range: 0 - 7.
   */
  uint8 pre;
  /*
   *  Transmitter post-emphasis value - internal
   *  representation. A weight value for the Post-Curser
   *  emphasis. Range: 0 - 15.
   */
  uint8 post;
}SOC_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Receiver zcnt value- internal representation.
   */
  uint8 zcnt;
  /*
   *  Receiver z1cnt value- internal representation.
   */
  uint8 z1cnt;
  /*
   *  Receiver dfelth value- internal representation.
   */
  uint8 dfelth;
  /*
   *  Receiver tlth value- internal representation.
   */
  uint8 tlth;
  /*
   *  Receiver g1cnt value- internal representation.
   */
  uint8 g1cnt;
}SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS;

typedef union
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Internal TX physical parameters - SerDes registers
   *  representation. Used only if the conf_mode is 'INTERNAL'
   *  - ignored otherwise.
   */
  SOC_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS intern;
  /*
   *  TX physical parameters - explicit (Pre/Post-emphasis, in
   *  percents, and Amplitude swing, in mV) configuration.
   *  Used only if the conf_mode is 'EXPLICIT' - ignored
   *  otherwise.
   */
  SOC_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS explct;
  /*
   *  Applicable for SerDes rates below or equal to 3.125Gbps
   *  only. Estimated media attenuation, end to end. Units:
   *  Decibel(Db). Resolution: 0.1Db (e.g atten=67 for 6.7Db).
   *  Range: 0..120. (0.0-12.0Db). Used only if the conf_mode
   *  is 'ATTEN' - ignored otherwise.
   */
  uint32 atten;
  /*
   *  TX physical parameters are derived from the selected
   *  media type. Used only if the conf_mode is 'MEDIA_TYPE' -
   *  ignored otherwise.
   */
  SOC_PETRA_SRD_MEDIA_TYPE media_type;
}SOC_PETRA_SRD_TX_PHYS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Transmitter physical parameters.
   */
  SOC_PETRA_SRD_TX_PHYS conf;
}SOC_PETRA_SRD_TX_PHYS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Receiver physical parameters - internal representation
   *  (SerDes registers).
   */
  SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS intern;
}SOC_PETRA_SRD_RX_PHYS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the specified quartet CMU in the SerDes star is
   *  activated. Notes: 1. If any CMU is expected to be used
   *  at some stage in the future, it must be activated on
   *  init (the ACTIVE CMU structure). 2. Not activating a CMU
   *  (if not expected to be used at any point) improves the
   *  device power consumption.
   */
  uint8 is_active;
  /*
   *  The maximal expected rate for any lane in the quartet.
   *  SOC_PETRA_SRD_DATA_RATE type Range: according to the enumerator.
   *  Numeric value Range: 1,000,000 - 6,250,000. Units: Kbps.
   *  Must be identical for
   *  all lanes per-quartet.
   */
  uint32 max_expected_lane_rate;
}SOC_PETRA_SRD_QRTT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Quartet configuration, per SerDes quartet. (Quartet
   *  index) Range: 0 - 3. Note: if any SerDes lane in the
   *  quartet is expected to be used after initialization, the
   *  is_active bit must be set to activate the corresponding
   *  CMU.
   */
  SOC_PETRA_SRD_QRTT_INFO qrtt[SOC_PETRA_SRD_NOF_QRTTS_PER_STAR_MAX];
}SOC_PETRA_SRD_STAR_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the lane configuration is applied.
   */
  uint8 enable;
  /*
   *  Per-SerDes Lane rate configuration parameters.
   */
  uint32 rate_conf;
  /*
   *  Per-SerDes Lane transmitter physical configuration
   *  parameters.
   */
  SOC_PETRA_SRD_TX_PHYS_INFO tx_phys_conf;
  /*
   *  Per-SerDes Lane Transmitter physical parameters
   *  configuration mode - by media-type, explicit or
   *  internal.
   */
  SOC_PETRA_SRD_TX_PHYS_CONF_MODE tx_phys_conf_mode;
  /*
   *  Per-SerDes Lane receiver physical configuration
   *  parameters.
   */
  SOC_PETRA_SRD_RX_PHYS_INFO rx_phys_conf;
  /*
   *  Per-SerDes Lane power state configuration parameters.
   *  Note: if enabled, the configuration is set for both
   *  direction (receive and transmit). To set different
   *  configuration per-direction - either set FALSE here and
   *  configure using dedicated API, or override one of the
   *  directions using dedicated API.
   */
  SOC_PETRA_SRD_POWER_STATE power_state_conf;
  /*
   *  If TRUE, polarity is swapped (TX).
   */
  uint8 is_swap_polarity_tx;
  /*
   *  If TRUE, polarity is swapped (RX).
   */
  uint8 is_swap_polarity_rx;
}SOC_PETRA_SRD_ALL_LANE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the STAR configuration is applied (STAR
   *  initialization sequence).
   */
  uint8 enable;
  /*
   *  SerDes STAR configuration.
   */
  SOC_PETRA_SRD_STAR_INFO conf;
}SOC_PETRA_SRD_ALL_STAR_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Per-SerDes STAR configuration
   */
  SOC_PETRA_SRD_ALL_STAR_INFO star_conf[SOC_PETRA_SRD_NOF_STARS];
  /*
   *  Per-SerDes Lane configuration
   */
  SOC_PETRA_SRD_ALL_LANE_INFO lane_conf[SOC_PETRA_SRD_NOF_LANES];
}SOC_PETRA_SRD_ALL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the auto-equalization process finished with
   *  success. The updated receiver parameter were written to
   *  the device.
   */
  uint8 is_success;
  /*
   *  The achived eye-height. Units: TAP-s. Note: only
   *  meaningful if is_success is TRUE.
   */
  uint32 eye_height_taps;
}SOC_PETRA_SRD_AEQ_STATUS;

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
*     soc_petra_srd_rate_set
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
  soc_petra_srd_rate_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  uint32                  rate
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_rate_get
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
  soc_petra_srd_rate_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT uint32       *rate
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_tx_phys_params_set
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
  soc_petra_srd_tx_phys_params_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_INFO    *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_tx_phys_params_get
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
  soc_petra_srd_tx_phys_params_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_TX_PHYS_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_TX_PHYS_INFO    *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_polarity_set
* TYPE:
*   PROC
* FUNCTION:
*     Set, per direction (RX/TX/Both) whether to swap-polarity
*     on a certain lane.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     The index of the SerDes lane to configure. Range: 0 -
*     59.
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
  soc_petra_srd_lane_polarity_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  uint8                 is_swap_polarity
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_polarity_get
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
  soc_petra_srd_lane_polarity_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT uint8                 *is_swap_polarity_rx,
    SOC_SAND_OUT uint8                 *is_swap_polarity_tx
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_power_state_set
* TYPE:
*   PROC
* FUNCTION:
*     Set the state of the lane - power-up/power-down.
*     CAUTION! This is a low-level API that operates directly on the SerDes,
*     regardless the attached logic (Fabric/NIF). For the correct power-up/power-down sequence,
*     including powering up/down the SerDes, use the relevant API-s:
*     fabric_link_on_off_set, nif_on_off_set.
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
  soc_petra_srd_lane_power_state_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_POWER_STATE     state
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_power_state_get
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
*  SOC_SAND_OUT SOC_PETRA_SRD_POWER_STATE     *state -
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
  soc_petra_srd_lane_power_state_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_POWER_STATE     *state_rx,
    SOC_SAND_OUT SOC_PETRA_SRD_POWER_STATE     *state_tx
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_star_set
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
  soc_petra_srd_star_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_STAR_ID         star_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_STAR_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_star_get
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
  soc_petra_srd_star_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_STAR_ID         star_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_STAR_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_qrtt_set
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
  soc_petra_srd_qrtt_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 qrtt_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_QRTT_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_qrtt_get
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
  soc_petra_srd_qrtt_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 qrtt_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_QRTT_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_all_set
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
  soc_petra_srd_all_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ALL_INFO        *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_all_get
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
  soc_petra_srd_all_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_SRD_ALL_INFO        *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_auto_equalize
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
  soc_petra_srd_auto_equalize(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_AEQ_MODE        mode,
    SOC_SAND_OUT SOC_PETRA_SRD_AEQ_STATUS      *status
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_rx_phys_params_set
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
  soc_petra_srd_rx_phys_params_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_rx_phys_params_get
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
  soc_petra_srd_rx_phys_params_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS *info
  );

void
  soc_petra_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS *info
  );

void
  soc_petra_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS *info
  );

void
  soc_petra_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS *info
  );

void
  soc_petra_PETRA_SRD_TX_PHYS_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_TX_PHYS *info
  );

void
  soc_petra_PETRA_SRD_TX_PHYS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_TX_PHYS_INFO *info
  );

void
  soc_petra_PETRA_SRD_RX_PHYS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_RX_PHYS_INFO *info
  );

void
  soc_petra_PETRA_SRD_QRTT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_QRTT_INFO *info
  );

void
  soc_petra_PETRA_SRD_STAR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_STAR_INFO *info
  );

void
  soc_petra_PETRA_SRD_ALL_LANE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_ALL_LANE_INFO *info
  );

void
  soc_petra_PETRA_SRD_ALL_STAR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_ALL_STAR_INFO *info
  );

void
  soc_petra_PETRA_SRD_ALL_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_ALL_INFO *info
  );

void
  soc_petra_PETRA_SRD_AEQ_STATUS_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_AEQ_STATUS *info
  );

/*
 *  Return the SerDes quartet index, in the range [0..14],
 *  given the Combo enumerator index.
 *  If invalid Combo index, returns SOC_PETRA_SRD_NOF_QUARTETS.
 */
uint32
  soc_petra_srd_combo2qrtt_id(
    SOC_SAND_IN SOC_PETRA_COMBO_QRTT combo_qrtt
  );

/*
 *  Return the Combo quartet index, in the range [0..1],
 *  given the SerDes enumerator index.
 *  If invalid SerDes index, returns SOC_PETRA_COMBO_NOF_QRTTS.
 */
uint32
  soc_petra_srd_qrtt2combo_id(
    SOC_SAND_IN uint32 srd_qrtt
  );

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_SRD_STAR_ID_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_STAR_ID enum_val
  );


const char*
  soc_petra_PETRA_SRD_DATA_RATE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_DATA_RATE enum_val
  );



const char*
  soc_petra_PETRA_SRD_MEDIA_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_MEDIA_TYPE enum_val
  );



const char*
  soc_petra_PETRA_SRD_TX_PHYS_CONF_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_CONF_MODE enum_val
  );



const char*
  soc_petra_PETRA_SRD_POWER_STATE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_POWER_STATE enum_val
  );



const char*
  soc_petra_PETRA_SRD_AEQ_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_AEQ_MODE enum_val
  );



void
  soc_petra_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS_print(
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_EXPLICIT_PARAMS *info
  );


void
  soc_petra_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS_print(
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_INTERNAL_PARAMS *info
  );


void
  soc_petra_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS_print(
    SOC_SAND_IN SOC_PETRA_SRD_RX_PHYS_INTERNAL_PARAMS *info
  );



void
  soc_petra_PETRA_SRD_TX_PHYS_print(
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_CONF_MODE conf_mode,
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS           *info
  );



void
  soc_petra_PETRA_SRD_TX_PHYS_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_CONF_MODE conf_mode,
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_INFO      *info
  );



void
  soc_petra_PETRA_SRD_RX_PHYS_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_RX_PHYS_INFO *info
  );



void
  soc_petra_PETRA_SRD_QRTT_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_QRTT_INFO *info
  );



void
  soc_petra_PETRA_SRD_STAR_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_STAR_INFO *info
  );



void
  soc_petra_PETRA_SRD_ALL_LANE_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_CONF_MODE conf_mode,
    SOC_SAND_IN SOC_PETRA_SRD_ALL_LANE_INFO     *info
  );



void
  soc_petra_PETRA_SRD_ALL_STAR_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_ALL_STAR_INFO *info
  );



void
  soc_petra_PETRA_SRD_ALL_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_TX_PHYS_CONF_MODE conf_mode,
    SOC_SAND_IN SOC_PETRA_SRD_ALL_INFO          *info
  );



void
  soc_petra_PETRA_SRD_AEQ_STATUS_print(
    SOC_SAND_IN SOC_PETRA_SRD_AEQ_STATUS *info
  );


#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_API_SERDES_INCLUDED__*/
#endif
