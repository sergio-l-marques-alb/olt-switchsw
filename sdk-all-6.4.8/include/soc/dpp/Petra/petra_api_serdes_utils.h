/* $Id: petra_api_serdes_utils.h,v 1.6 Broadcom SDK $
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


#ifndef __SOC_PETRA_API_SERDES_UTILS_INCLUDED__
/* { */
#define __SOC_PETRA_API_SERDES_UTILS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_api_serdes.h>
#include <soc/dpp/Petra/petra_serdes.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximal value for dfelth for eye scan iteration         */
#define  SOC_PETRA_SRD_EYE_SCAN_DFELTH_MAX 63

/*     Maximal value for tlth for eye scan iteration           */
#define  SOC_PETRA_SRD_EYE_SCAN_TLTH_MAX 48

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

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Soc_petra SerDes Element Type.
   */
  uint32 element;
  /*
   *  Register Offset.
   */
  uint32 offset;
}SOC_PETRA_SRD_REGS_ADDR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Register address of the SerDes register this field
   *  belongs to.
   */
  SOC_PETRA_SRD_REGS_ADDR addr;
  /*
   *  Field Most Significant Bit in the register.
   */
  uint8 msb;
  /*
   *  Field Least Significant Bit in the register.
   */
  uint8 lsb;
}SOC_PETRA_SRD_REGS_FIELD;

typedef enum
{
  /*
   *  SerDes Lane. Range : 0 - 59.
   */
  SOC_PETRA_SRD_ENTITY_TYPE_LANE=0,
  /*
   *  Per-SerDes-quartet control unit. Note : SerDes Quartet,
   *  also referred to as SerDes Macrocell, contains four
   *  SerDes Lanes.
   */
  SOC_PETRA_SRD_ENTITY_TYPE_CMU=1,
  /*
   *  Per-SerDes-star control unit (8051). Note: SerDes Star,
   *  also referred to as SerDes STAR, is a unit containing
   *  several SerDes Quartets. Four SerDes Stars exist:
   *  NIF-A/NIF-B/Fabric-A, Fabric-B. All except Fabric-b
   *  contain four SerDes quartets. Fabric-B contains three
   *  SerDes quartets.
   */
  SOC_PETRA_SRD_ENTITY_TYPE_IPU=2,
  /*
   *  Total Number of SerDes entities.
   */
  SOC_PETRA_SRD_NOF_ENTITY_TYPE_LANES=3
}SOC_PETRA_SRD_ENTITY_TYPE;


typedef enum
{
  /*
   *  RX Low-speed control
   */
  SOC_PETRA_SRD_LANE_ELEMENT_RX=0,
  /*
   *  TX Low-speed control.
   */
  SOC_PETRA_SRD_LANE_ELEMENT_TX=1,
  /*
   *  PRBS generator/checker.
   */
  SOC_PETRA_SRD_LANE_ELEMENT_PRBS=2,
  /*
   *  RX LOS detection control registers.
   */
  SOC_PETRA_SRD_LANE_ELEMENT_RX_LOS=3,
  /*
   *  VCDL TRIM (4-bit data).
   */
  SOC_PETRA_SRD_LANE_ELEMENT_VCDL=4,
  /*
   *  RX front-end trim (4-bit data).
   */
  SOC_PETRA_SRD_LANE_ELEMENT_RX_FE=5,
  /*
   *  RX high-speed control.
   */
  SOC_PETRA_SRD_LANE_ELEMENT_RX_HS=6,
  /*
   *  RX state machine.
   */
  SOC_PETRA_SRD_LANE_ELEMENT_RX_SM=7,
  /*
   *  Preamp control.
   */
  SOC_PETRA_SRD_LANE_ELEMENT_PREAMP=8,
  /*
   *  TX high-speed control.
   */
  SOC_PETRA_SRD_LANE_ELEMENT_TX_HS=9,
  /*
   *  Total number of serdes lane elements
   */
  SOC_PETRA_SRD_LANE_NOF_ELEMENTS=10
}SOC_PETRA_SRD_LANE_ELEMENT;


typedef enum
{
  /*
   *  CMU Control Register Set
   */
  SOC_PETRA_SRD_CMU_ELEMENT_CONTROL=0,
  /*
   *  CMU Interface Arbitration Register Set.
   */
  SOC_PETRA_SRD_CMU_ELEMENT_ARBITRATION=1,
  /*
   *  Total number of CMU elements
   */
  SOC_PETRA_SRD_CMU_NOF_ELEMENTS=2
}SOC_PETRA_SRD_CMU_ELEMENT;


typedef enum
{
  /*
   *  IPU Control Register Set
   */
  SOC_PETRA_SRD_IPU_ELEMENT_CONTROL=0,
  /*
   *  IPU Interface Arbitration Register Set.
   */
  SOC_PETRA_SRD_IPU_ELEMENT_ARBITRATION=1,
  /*
   *  Total number of IPU elements
   */
  SOC_PETRA_SRD_IPU_NOF_ELEMENTS=2
}SOC_PETRA_SRD_IPU_ELEMENT;

#define SOC_PETRA_SRD_ELEMENT_MAX (SOC_PETRA_SRD_LANE_NOF_ELEMENTS-1)

typedef enum
{
  /*
   *  Adaptive equalizer is off.
   */
  SOC_PETRA_SRD_LANE_EQ_MODE_OFF=0,
  /*
   *  Run the algorithm with no previously known parameters
   *  about the signal. Blind mode will optimize all
   *  preamplifier and DFEsettings. Blind mode is often used
   *  to determine the optimum settings, which are then used
   *  as preset values.
   */
  SOC_PETRA_SRD_LANE_EQ_MODE_BLIND=1,
  /*
   *  Preset EQ mode is for setting and aiding the receiver to
   *  operate at its near-optimal AEQ settings.
   */
  SOC_PETRA_SRD_LANE_EQ_MODE_PRESET=2,
  /*
   *  Steady-State mode. The steadystate EQ mode is used for
   *  further optimizing preset EQ parameters (fine
   *  adjustment) and for adjusting AEQparameters in response
   *  to long-term communication channel characteristics
   *  variation.
   */
  SOC_PETRA_SRD_LANE_EQ_MODE_SS=3,
  /*
   *  Last value.
   */
  SOC_PETRA_SRD_NOF_LANE_EQ_MODES=4
}SOC_PETRA_SRD_LANE_EQ_MODE;


typedef enum
{
  /*
   *  Normal (No loopback).
   */
  SOC_PETRA_SRD_LANE_LOOPBACK_NONE=0,
  /*
   *  Serial Tx->Rx Loopback, without Driver and PrAmp.
   */
  SOC_PETRA_SRD_LANE_LOOPBACK_NSILB=1,
  /*
   *  Parallel Remote Loopback.
   */
  SOC_PETRA_SRD_LANE_LOOPBACK_NPILB=2,
  /*
   *  Parallel Internal Loopback.
   */
  SOC_PETRA_SRD_LANE_LOOPBACK_IPILB=3,
  /*
   *  Last value.
   */
  SOC_PETRA_SRD_NOF_LANE_LOOPBACK_MODES=4
}SOC_PETRA_SRD_LANE_LOOPBACK_MODE;

/* $Id: petra_api_serdes_utils.h,v 1.6 Broadcom SDK $
 *  An indication of un-configured, or invalid rate-devisor.
 */

typedef enum
{
  /*
   *  PRBS pattern mode: X^7 polynomial.
   */
  SOC_PETRA_SRD_PRBS_MODE_POLY_7=0,
  /*
   *  PRBS pattern mode: X^15 polynomial.
   */
  SOC_PETRA_SRD_PRBS_MODE_POLY_15=1,
  /*
   *  PRBS pattern mode: X^23 polynomial.
   */
  SOC_PETRA_SRD_PRBS_MODE_POLY_23=2,
  /*
   *  PRBS pattern mode: X^31 polynomial.
   */
  SOC_PETRA_SRD_PRBS_MODE_POLY_31=3,
  /*
   *  PRBS pattern mode: X^23 polynomial, with swapped polarity.
   *  This mode is needed when working with older devices: SOC_SAND_FAP21V; SOC_SAND_FAP20V; SOC_SAND_FE200.
   *  When used, the SerDes lane polarity is swapped before running PRBS,
   *  and restored afterwards.
   */
  SOC_PETRA_SRD_PRBS_MODE_POLY_23_SWAP_POLARITY=4,
  /*
   *  Total number of PRBS pattern modes.
   */
  SOC_PETRA_SRD_NOF_PRBS_MODES=5
}SOC_PETRA_SRD_PRBS_MODE;


typedef enum
{
  /*
   *  CRC MAC counter as a counter source.
   */
  SOC_PETRA_SRD_CNT_SRC_CRC_MAC = 0,
  /*
   *  PRBS counter as a counter source.
   */
  SOC_PETRA_SRD_CNT_SRC_PRBS = 1,
  /*
   *  BER counter as a counter source. FEC enable.
   *  Applicable for Soc_petra-B only.
   *  Applicable for Fabric links only.
   */
  SOC_PETRA_SRD_CNT_SRC_FEC_BER = 2,
  /*
   *  CER counter as a counter source. FEC enable.
   *  Applicable for Soc_petra-B only.
   *  Applicable for Fabric links only.
   */
  SOC_PETRA_SRD_CNT_SRC_FEC_CER = 3,
  /*
  *  Total number of counter sources.
  */
  SOC_PETRA_SRD_NOF_CNT_SRCS = 4
}SOC_PETRA_SRD_CNT_SRC;

typedef enum
{
  /*
   *  Generation of traffic via PRBS.
   */
  SOC_PETRA_SRD_TRAFFIC_SRC_PRBS = 0,
  /*
   *  External traffic is already generated.
   */
  SOC_PETRA_SRD_TRAFFIC_SRC_EXT = 1,
  /*
   *  Total number of traffic sources.
   */
  SOC_PETRA_SRD_NOF_TRAFFIC_SRCS = 3
}SOC_PETRA_SRD_TRAFFIC_SRC;


typedef enum
{
  /*
   *  PRBS signal status, receiver - locked. No signal loss
   *  wath detected.
   */
  SOC_PETRA_SRD_PRBS_SIGNAL_STAT_LOCKED_NO_LOSS=0,
  /*
   *  PRBS signal status, receiver - currently locked, but
   *  signal loss was detected.
   */
  SOC_PETRA_SRD_PRBS_SIGNAL_STAT_LOCKED_AFTER_LOSS=1,
  /*
   *  PRBS signal status, receiver - not locked.
   */
  SOC_PETRA_SRD_PRBS_SIGNAL_STAT_NOT_LOCKED=2,
  /*
   *  Total number of PRBS signal statuses.
   */
  SOC_PETRA_SRD_NOF_PRBS_SIGNAL_STATS=3
}SOC_PETRA_SRD_PRBS_SIGNAL_STAT;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the link is present. Note: this is a general
   *  link status, not PRBS-related.
   */
  uint8 rx_signal_present;
  /*
   *  The status of the prbs signal, reported by the receiver.
   */
  SOC_PETRA_SRD_PRBS_SIGNAL_STAT prbs_signal_stat;
  /*
   *  PRBS error counter. Counts the number of PRBS errors
   *  since last cleared.
   */
  uint32 error_cnt;
}SOC_PETRA_SRD_PRBS_RX_STATUS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Indication that the signal was lost.
   */
  uint8 rx_signal_loss;
  /*
   *  Indication that the Rx frequency is not locked.
   */
  uint8 rx_not_locked;
  /*
   *  TRUE if lane signal (SigDet) is detected and stable.
   */
  uint8 rx_signal_detected;
  /*
   *  TRUE if lane signal detect status changed since last
   *  read.
   */
  uint8 rx_sig_det_changed;
  /*
   *  TRUE if reset sequence is currently running.
   */
  uint8 rx_reset;
  /*
   *  TRUE if lane recovered frequency is invalid.
   */
  uint8 rx_freq_invalid;
  /*
   *  TRUE if rx PCS synchronization FSM achieved
   *  synchronization
   */
  uint8 rx_pcs_synced;
  /*
   *  TRUE if 10/8 bit decoder code errors were detected synced
   *  last read
   */
  uint8 rx_code_errors_detected;
  /*
   *  TRUE if 10/8 bit decoder disparity errors were detected
   *  synced last read
   */
  uint8 rx_disparity_errors_detected;
  /*
   *  Estimated recent comma alignment position. Note: The
   *  signal may be unstable - read multiple times to increase
   *  confidence.
   */
  uint32 rx_comma_allignment;
  /*
   *  TRUE if comma realignment occurred since last read.
   */
  uint8 rx_comma_reallign;
  /*
   *  Rate divisor
   */
  SOC_PETRA_SRD_RATE_DIVISOR rate_divisor;
  /*
   *  If TRUE, serdes is powered off (RX)
   *  Note: set to OFF also when the Quartet (CMU) is disabled
   *  In this case, other indications are invalid.
   */
  uint8 rx_power_off;
  /*
   *  If TRUE, serdes is powered off (TX)
   *  Note: set to OFF also when the Quartet (CMU) is disabled
   *  In this case, other indications are invalid.
   */
  uint8 tx_power_off;

  /*
   * TRUE if the SerDes lane is in loopback
   */
  uint8 is_in_lpbck;
}SOC_PETRA_SRD_LANE_STATUS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *	If TRUE, the CMU is powered up.
   */
  uint8 is_active;
  /*
   *  CMU version indication
   */
  uint8 version;
  /*
   *  Rate-devidor - PLL-M
   */
  uint8 pllm;
  /*
   *  Rate-devidor - PLL-N
   */
  uint8 plln;
  /*
   *  Indication that the CMU's PLL frequency is not locked.
   */
  uint8 pll_not_locked;
  /*
   *  Indication that the CMU's PLL frequency is not locked
   *  (due to slow VCO).
   */
  uint8 pll_vco_not_locked;
  /*
   *  Per-lane status
   */
  SOC_PETRA_SRD_LANE_STATUS_INFO lane_status[SOC_PETRA_SRD_NOF_LANES_PER_QRTT];
}SOC_PETRA_SRD_QRTT_STATUS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Eye scan sampled BER (Bit Error Rate) values
   */
  uint32 entries[SOC_PETRA_SRD_EYE_SCAN_TLTH_MAX+1][SOC_PETRA_SRD_EYE_SCAN_DFELTH_MAX+1];
}SOC_PETRA_SRD_EYE_SCAN_MATRIX;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  optimum value of tlth. Range: 0 - 48.
   */
  uint8 tlth;
  /*
   *  optimum value of dfelth. Range: 0 - 63.
   */
  uint8 dfelth;
  /*
  *  optimum margin value of tlth. Range: 0 - 48.
  */
  uint8 tlth_margin;
  /*
  *  optimum margin value of dfelth. Range: 0 - 126.
  */
  uint8 dfelth_margin;
  /*
  *  is optimum valid.
  */
  uint8 is_valid;

}SOC_PETRA_SRD_EYE_SCAN_OPTIMUM_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Value of parameter in optimum point. Range: 0 - 48.
   */
  uint8 tlth_min;
  /*
   *  Value of parameter in optimum point. Range: 0 - 48.
   */
  uint8 tlth_max;
  /*
   *  Value of parameter in optimum point. Range: 0 - 63.
   */
  uint8 dfelth_min;
  /*
   *  Value of parameter in optimum point. Range: 0 - 63.
   */
  uint8 dfelth_max;

} SOC_PETRA_SRD_EYE_SCAN_RANGE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   * PRBS polynomial to use (must be the same as enabled on TX side)
   */
  SOC_PETRA_SRD_PRBS_MODE mode;
  /*
   *	Indicate the source of the counters: PRBS, MAC-CRC,
   *  FEC-BER (Bit error rate) or FEC-CER (Cell error rate).
   *  The last two cases (BER and CER) are invalid for a PRBS traffic.
   */
  SOC_PETRA_SRD_CNT_SRC cnt_src;
  /*
   * If True, then the Tx PRBS is also enabled (both directions).
   */
  uint8 is_tx_prbs_enabled;
}SOC_PETRA_SRD_TRAFFIC_PARAMS_PRBS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *	Indicate the source of the counters: PRBS, MAC,
   *  BER (Bit error rate) or CER (Cell error rate).
   *  The first case (PRBS) is invalid for a non-PRBS traffic.
   */
  SOC_PETRA_SRD_CNT_SRC cnt_src;
}SOC_PETRA_SRD_TRAFFIC_PARAMS_EXT;


typedef union
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Parameters of the traffic if the traffic source
   *  is PRBS.
   */
  SOC_PETRA_SRD_TRAFFIC_PARAMS_PRBS prbs;
  /*
   *  Parameters of the traffic if the traffic source
   *  is not PRBS.
   *  For the SerDes screening, it can be also snake.
   */
  SOC_PETRA_SRD_TRAFFIC_PARAMS_EXT ext;
}SOC_PETRA_SRD_TRAFFIC_PARAMS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Number of valid lanes in lane_ndx array. Range: 1 - 59.
   */
  uint32 nof_lane_ndx_entries;
  /*
   *  Array of lane indices to run eye scan on.
   */
  uint32 lane_ndx[SOC_PETRA_SRD_NOF_LANES];
  /*
   *  Minimal duration to run on each point of scan, in seconds
   *  Range: 1 - 60.
   */
  uint32 duration_min_sec;
  /*
   *  Scan density. Value of x means incrementing tlth/dfelth by
   *  x on every scan iteration. Range: 1 - 10.
   */
  uint32 resolution;
  /*
   *	Traffic generation source: PRBS or external
   */
  SOC_PETRA_SRD_TRAFFIC_SRC traffic_src;
  /*
   *  Range to scan. By default, scans tlth 0-48, dfelth 0-63.
   */
  SOC_PETRA_SRD_EYE_SCAN_RANGE range;
  /*
   *	Traffic params. Set according to the traffic source.
   */
  SOC_PETRA_SRD_TRAFFIC_PARAMS params;

}SOC_PETRA_SRD_EYE_SCAN_INFO;

typedef struct
{
  /* NOTE: this structure should only be passed by reference when used
   * in the driver implementation. this is a large structure and must
   * not be allocated on the stack.
   */
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Matrix of deflth * tlth, containing the result of the
   *  eye scan.
   */
  SOC_PETRA_SRD_EYE_SCAN_MATRIX matrix;
  /*
   *  Optimal rx physical parameters as calculated from matrix.
   */
  SOC_PETRA_SRD_EYE_SCAN_OPTIMUM_INFO optimum;
}SOC_PETRA_SRD_EYE_SCAN_RES;

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
*     soc_petra_srd_reg_write
* TYPE:
*   PROC
* FUNCTION:
*     Write 8 bit value to the SerDes.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity -
*     the entity to access. Can be a SerDes lane, a CMU or an
*     IPU.
*  SOC_SAND_IN  uint32                 entity_ndx -
*     the entity index. If SerDes Lane - the SerDes lane
*     number. Range: 0-59. If SerDes CMU - the SerDes Quertet
*     index. Range: 0 - 14. If SerDes IPU - the SerDes Star
*     index (refer to SOC_PETRA_SRD_STAR_ID). Range: 0 - 3
*     (NIF-A/NIF-B/FABRIC-A/FABRIC-B).
*  SOC_SAND_IN  SOC_PETRA_SRD_REGS_ADDR       *reg -
*     Register access information: element, offset.
*  SOC_SAND_IN  uint8                   val -
*     The data to write to the register.
* REMARKS:
*     1. Writing to the SerDes is implemented through the
*     SerDes EPB.2. Refer to the SerDes Data Sheet for details
*     on SerDes EPB access.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_reg_write(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity,
    SOC_SAND_IN  uint32                 entity_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_REGS_ADDR       *reg,
    SOC_SAND_IN  uint8                   val
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_reg_read
* TYPE:
*   PROC
* FUNCTION:
*     Read 8 bit value from the SerDes.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity -
*     the entity to access. Can be a SerDes lane, a CMU or an
*     IPU.
*  SOC_SAND_IN  uint32                 entity_ndx -
*     the entity index. If SerDes Lane - the SerDes lane
*     number. Range: 0-59. If SerDes CMU - the SerDes Quertet
*     index. Range: 0 - 14. If SerDes IPU - the SerDes Star
*     index (refer to SOC_PETRA_SRD_STAR_ID). Range: 0 - 3
*     (NIF-A/NIF-B/FABRIC-A/FABRIC-B).
*  SOC_SAND_IN  SOC_PETRA_SRD_REGS_ADDR       *reg -
*     Register access information: element, offset.
*  SOC_SAND_OUT uint8                   *val -
*     The data to read from the register.
* REMARKS:
*     1. Writing to the SerDes is implemented through the
*     SerDes EPB.2. Refer to the SerDes Data Sheet for details
*     on SerDes EPB access.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_reg_read(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity,
    SOC_SAND_IN  uint32                 entity_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_REGS_ADDR       *reg,
    SOC_SAND_OUT uint8                   *val
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_fld_write
* TYPE:
*   PROC
* FUNCTION:
*     Write a field (up to 8 bit) value to the SerDes.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity -
*     the entity to access. Can be a SerDes lane, a CMU or an
*     IPU.
*  SOC_SAND_IN  uint32                 entity_ndx -
*     the entity index. If SerDes Lane - the SerDes lane
*     number. Range: 0-59. If SerDes CMU - the SerDes Quertet
*     index. Range: 0 - 14. If SerDes IPU - the SerDes Star
*     index (refer to SOC_PETRA_SRD_STAR_ID). Range: 0 - 3
*     (NIF-A/NIF-B/FABRIC-A/FABRIC-B).
*  SOC_SAND_IN  SOC_PETRA_SRD_REGS_FIELD      *fld -
*     Field access information: element, offset, MSB, LSB.
*  SOC_SAND_IN  uint8                   val -
*     The data to write to the field.
* REMARKS:
*     1. Writing to the SerDes is implemented through the
*     SerDes EPB.2. Refer to the SerDes Data Sheet for details
*     on SerDes EPB access.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_fld_write(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity,
    SOC_SAND_IN  uint32                 entity_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_REGS_FIELD      *fld,
    SOC_SAND_IN  uint8                   val
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_fld_read
* TYPE:
*   PROC
* FUNCTION:
*     Read 8 bit value from the SerDes.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity -
*     the entity to access. Can be a SerDes lane, a CMU or an
*     IPU.
*  SOC_SAND_IN  uint32                 entity_ndx -
*     the entity index. If SerDes Lane - the SerDes lane
*     number. Range: 0-59. If SerDes CMU - the SerDes Quertet
*     index. Range: 0 - 14. If SerDes IPU - the SerDes Star
*     index (refer to SOC_PETRA_SRD_STAR_ID). Range: 0 - 3
*     (NIF-A/NIF-B/FABRIC-A/FABRIC-B).
*  SOC_SAND_IN  SOC_PETRA_SRD_REGS_FIELD      *fld -
*     Field access information: element, offset, MSB, LSB.
*  SOC_SAND_OUT uint8                   *val -
*     The data to read from the field.
* REMARKS:
*     1. Writing to the SerDes is implemented through the
*     SerDes EPB.2. Refer to the SerDes Data Sheet for details
*     on SerDes EPB access.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_fld_read(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity,
    SOC_SAND_IN  uint32                 entity_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_REGS_FIELD      *fld,
    SOC_SAND_OUT uint8                   *val
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_link_rx_eye_monitor
* TYPE:
*   PROC
* FUNCTION:
*     This function sets the equalizer mode and retrieves the
*     eye-sample.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     The SerDes number of which we want to set.
*  SOC_SAND_IN  SOC_PETRA_SRD_LANE_EQ_MODE    equalizer_mode -
*     This mode indicates whether and how to run an automatic
*     algorithm to read the Eye-Sample. (modes:
*     off/normal/preset).
*  SOC_SAND_OUT uint32                  *eye_sample -
*     A pointer to the returned data, the Eye-Opening (in mV).
*     If the routine fails to return the eye-sample, it
*     returns 0.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_link_rx_eye_monitor(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_LANE_EQ_MODE    equalizer_mode,
    SOC_SAND_OUT uint32                  *eye_sample
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_loopback_mode_set
* TYPE:
*   PROC
* FUNCTION:
*     This function sets the loopback mode of a specified
*     lane.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     The SerDes lane number of which we want to set.(0..59).
*  SOC_SAND_IN  SOC_PETRA_SRD_LANE_LOOPBACK_MODE loopback_mode -
*     There are 5 possible loopback modes.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_lane_loopback_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_LANE_LOOPBACK_MODE loopback_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_loopback_mode_get
* TYPE:
*   PROC
* FUNCTION:
*     This function sets the loopback mode of a specified
*     lane.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     The SerDes lane number of which we want to set.(0..59).
*  SOC_SAND_OUT SOC_PETRA_SRD_LANE_LOOPBACK_MODE *loopback_mode -
*     There are 5 possible loopback modes.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_lane_loopback_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_LANE_LOOPBACK_MODE *loopback_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_prbs_mode_set
* TYPE:
*   PROC
* FUNCTION:
*     Sets the PRBS pattern.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     SerDes lane index. Range: 0-59.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     PRBS direction: TX - for PRBS generation, RX - for PRBS
*     reception, or BOTH.
*  SOC_SAND_IN  SOC_PETRA_SRD_PRBS_MODE       mode -
*     SerDes PRBS pattern mode.
* REMARKS:
*     Typical PRBS sequence is: 1. Set PRBS mode. 2. Start
*     PRBS generation on the TX side of the link. 4. Start
*     PRBS reception on the RX side of the link. 5. Read and
*     clear receiver statuses (repeat if needed). 6. Stop PRBS
*     - generation and reception.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_prbs_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_PRBS_MODE       mode
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_prbs_mode_get
* TYPE:
*   PROC
* FUNCTION:
*     Sets the PRBS pattern.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     SerDes lane index. Range: 0-59.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     PRBS direction: TX - for PRBS generation, RX - for PRBS
*     reception, or BOTH.
*  SOC_SAND_OUT SOC_PETRA_SRD_PRBS_MODE       *mode -
*     SerDes PRBS pattern mode.
* REMARKS:
*     Typical PRBS sequence is: 1. Set PRBS mode. 2. Start
*     PRBS generation on the TX side of the link. 4. Start
*     PRBS reception on the RX side of the link. 5. Read and
*     clear receiver statuses (repeat if needed). 6. Stop PRBS
*     - generation and reception.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_prbs_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_PRBS_MODE       *rx_mode,
    SOC_SAND_OUT SOC_PETRA_SRD_PRBS_MODE       *tx_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_prbs_start
* TYPE:
*   PROC
* FUNCTION:
*     Start PRBS - generation (TX), reception (RX) or both.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     SerDes lane index. Range: 0-59.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     PRBS direction: TX - for PRBS generation, RX - for PRBS
*     reception, or BOTH.
* REMARKS:
*     Typical PRBS sequence is: 1. Set PRBS mode. 2. Start
*     PRBS generation on the TX side of the link. 4. Start
*     PRBS reception on the RX side of the link. 5. Read and
*     clear receiver statuses (repeat if needed). 6. Stop PRBS
*     - generation and reception.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_prbs_start(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_prbs_stop
* TYPE:
*   PROC
* FUNCTION:
*     Stop PRBS - generation (TX), reception (RX) or both.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     SerDes lane index. Range: 0-59.
*  SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx -
*     PRBS direction: TX - for PRBS generation, RX - for PRBS
*     reception, or BOTH.
* REMARKS:
*     Typical PRBS sequence is: 1. Set PRBS mode. 2. Start
*     PRBS generation on the TX side of the link. 4. Start
*     PRBS reception on the RX side of the link. 5. Read and
*     clear receiver statuses (repeat if needed). 6. Stop PRBS
*     - generation and reception.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_prbs_stop(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_prbs_get_and_clear_stat
* TYPE:
*   PROC
* FUNCTION:
*     Read the status on the receiver side. All indications
*     are cleared on read.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     SerDes lane index. Range: 0-59.
*  SOC_SAND_OUT SOC_PETRA_SRD_PRBS_RX_STATUS  *status -
*     Receiver status - error counter and signal status.
* REMARKS:
*     Typical PRBS sequence is: 1. Set PRBS mode. 2. Start
*     PRBS generation on the TX side of the link. 4. Start
*     PRBS reception on the RX side of the link. 5. Read and
*     clear receiver statuses (repeat if needed). 6. Stop PRBS
*     - generation and reception.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_prbs_get_and_clear_stat(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_PRBS_RX_STATUS  *status
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_status_get
* TYPE:
*   PROC
* FUNCTION:
*     This function is used as a diagnostics tool per lane
*     that gives an indication about the lane and CMU s
*     status. It indicates per lane - loss of signal,
*     frequency lock is lost. And per CMU - PLL lock is lost.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 lane_ndx -
*     SOC_SAND_IN uint32 lane_ndx
*  SOC_SAND_OUT SOC_PETRA_SRD_LANE_STATUS_INFO *lane_stt_info -
*     Different indication about a lane and CMU.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_lane_status_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_LANE_STATUS_INFO *lane_stt_info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_qrtt_status_get
* TYPE:
*   PROC
* FUNCTION:
*     Prints SerDes diagnostics, per SerDes quartet
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 qrtt_id -
*     quartet index. Range: 0 - 15.
*  SOC_SAND_OUT SOC_PETRA_SRD_QRTT_STATUS_INFO *qrtt_status -
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_qrtt_status_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 qrtt_id,
    SOC_SAND_OUT SOC_PETRA_SRD_QRTT_STATUS_INFO *qrtt_status
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_eye_scan_run
* TYPE:
*   PROC
* FUNCTION:
*     Runs the eye scan, in order to map between serdes rx
*     parameters and amount of crc errors. Eye scan sequence
*     is as following: For each tlth * dfelth pair, run prbs
*     for a given time, and collect results to a matrix, per
*     lane. The matrix is used to find optimal rx parameters.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_SRD_EYE_SCAN_INFO   *info -
*     Scan parameters.
*  SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_RES    *res -
*     Scan results. A pointer to an array of
*     SOC_PETRA_SRD_EYE_SCAN_RES structures (a result matrix and
*     optimum per scanned lane). The size of the array must be
*     at least info->nof_lanes_ndx_entries.
* REMARKS:
*     1. Eye scan assumes that for each scanned rx lane, the
*     corresponding tx lane (either in same device, or
*     different, has enabled prbs transmit with x^23
*     polynomial. 2. When running this function on more than
*     one device, running time can be optimized for devices
*     sharing a CPU by either: a. Running a different thread
*     for each of devices which share a CPU b. Implementing a
*     similar function, which in each iteration configures all
*     lanes in all devices sharing a CPU, than waits the
*     remainig time, and samples all devices. This should
*     allow performing a single delay for all devices, rather
*     than calling this function which will awaits each device
*     separately.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_srd_eye_scan_run(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_EYE_SCAN_INFO   *info,
    SOC_SAND_IN  uint8                 silent,
    SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_RES    *result
  );

/*********************************************************************
* PROTOTYPE
*   uint32
*     soc_petra_srd_eye_scan_run_multiple_alloc(
*       SOC_SAND_IN  int   unit,
*       SOC_SAND_IN  SOC_PETRA_SRD_EYE_SCAN_INFO   *info,
*       SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_RES    **res);
* FUNCTION
*   This functions is the same as soc_petra_srd_eye_scan_run, but receives an
*     array of pointers for the result, instead of an array. This should be
*     used by user who a cannot allocate a consecutive array of result
*     structures.
*  INPUT
*   SOC_SAND_IN  SOC_PETRA_SRD_EYE_SCAN_INFO   *info - Scan parameters.
*   SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_RES    **res - Scan results. A pointer
*     to an array of SOC_PETRA_SRD_EYE_SCAN_RES pointers (a result
*     matrix and optimum per scanned lane). The size of the array must
*     be at least info->nof_lanes_ndx_entries.
* RETURNS
*   OK or Error indication
* REMARKS
*   See soc_petra_srd_eye_scan_run().
*********************************************************************/

uint32
  soc_petra_srd_eye_scan_run_multiple_alloc(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_EYE_SCAN_INFO   *info,
    SOC_SAND_IN  uint8                 silent,
    SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_RES    **result
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_scif_enable_set
* TYPE:
*   PROC
* FUNCTION:
*     Enables/disables the SCIF interface. The SCIF is a
*     utility serial interface that can be used to access the
*     SerDes for debug configuration and diagnostics. It is
*     not required for normal operation, but may be used to
*     access the SerDes by external tools.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint8                 is_enabled -
*     If TRUE, the SCIF interface is enabled for use.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_srd_scif_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_enabled
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_scif_enable_get
* TYPE:
*   PROC
* FUNCTION:
*     Enables/disables the SCIF interface. The SCIF is a
*     utility serial interface that can be used to access the
*     SerDes for debug configuration and diagnostics. It is
*     not required for normal operation, but may be used to
*     access the SerDes by external tools.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT uint8                 *is_enabled -
*     If TRUE, the SCIF interface is enabled for use.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_srd_scif_enable_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *is_enabled
  );

/*********************************************************************
* NAME:
*   soc_petra_srd_relock
* TYPE:
*   PROC
* FUNCTION:
*   Performs SerDes CDR relock
* INPUT:
*   SOC_SAND_IN  int                    unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint32                    lane_ndx -
*     SerDes lane index. Range: 0 - 59.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_relock(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    lane_ndx
  );

void
  soc_petra_PETRA_SRD_PRBS_RX_STATUS_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_PRBS_RX_STATUS *info
  );

void
  soc_petra_PETRA_SRD_LANE_STATUS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_LANE_STATUS_INFO *info
  );

void
  soc_petra_PETRA_SRD_QRTT_STATUS_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_QRTT_STATUS_INFO *info
  );

void
  soc_petra_PETRA_SRD_EYE_SCAN_MATRIX_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_MATRIX *info
  );

void
  soc_petra_PETRA_SRD_EYE_SCAN_OPTIMUM_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_OPTIMUM_INFO *info
  );

void
  soc_petra_PETRA_SRD_TRAFFIC_PARAMS_PRBS_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_TRAFFIC_PARAMS_PRBS *info
  );

void
  soc_petra_PETRA_SRD_TRAFFIC_PARAMS_EXT_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_TRAFFIC_PARAMS_EXT *info
  );

void
  soc_petra_PETRA_SRD_TRAFFIC_PARAMS_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_TRAFFIC_PARAMS *info
  );

void
  soc_petra_PETRA_SRD_EYE_SCAN_RANGE_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_RANGE *info
  );

void
  soc_petra_PETRA_SRD_EYE_SCAN_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_INFO *info
  );

void
  soc_petra_PETRA_SRD_EYE_SCAN_RES_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_RES *info
  );

void
  soc_petra_PETRA_SRD_REGS_ADDR_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_REGS_ADDR *info
  );

void
  soc_petra_PETRA_SRD_REGS_FIELD_clear(
    SOC_SAND_OUT SOC_PETRA_SRD_REGS_FIELD *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1
/*
* Dump all the serdeses related information.
*/
uint32
  soc_petra_srd_diag_regs_dump(
    SOC_SAND_IN  int                 unit
  );

const char*
  soc_petra_PETRA_SRD_ENTITY_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_ENTITY_TYPE enum_val
  );

const char*
  soc_petra_PETRA_SRD_LANE_ELEMENT_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_LANE_ELEMENT enum_val
  );



const char*
  soc_petra_PETRA_SRD_CMU_ELEMENT_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_CMU_ELEMENT enum_val
  );



const char*
  soc_petra_PETRA_SRD_IPU_ELEMENT_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_IPU_ELEMENT enum_val
  );



const char*
  soc_petra_PETRA_SRD_LANE_EQ_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_LANE_EQ_MODE enum_val
  );



const char*
  soc_petra_PETRA_SRD_LANE_LOOPBACK_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_LANE_LOOPBACK_MODE enum_val
  );



const char*
  soc_petra_PETRA_SRD_PRBS_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_PRBS_MODE enum_val
  );

const char*
  soc_petra_PETRA_SRD_CNT_SRC_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_CNT_SRC enum_val
  );

const char*
  soc_petra_PETRA_SRD_TRAFFIC_SRC_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_TRAFFIC_SRC enum_val
  );

const char*
  soc_petra_PETRA_SRD_PRBS_SIGNAL_STAT_to_string(
    SOC_SAND_IN SOC_PETRA_SRD_PRBS_SIGNAL_STAT enum_val
  );


void
  soc_petra_PETRA_SRD_PRBS_RX_STATUS_print(
    SOC_SAND_IN SOC_PETRA_SRD_PRBS_RX_STATUS *info
  );



void
  soc_petra_PETRA_SRD_LANE_STATUS_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_LANE_STATUS_INFO *info
  );

void
  soc_petra_PETRA_SRD_QRTT_STATUS_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_QRTT_STATUS_INFO *info
  );

void
  soc_petra_PETRA_SRD_EYE_SCAN_MATRIX_print(
    SOC_SAND_IN SOC_PETRA_SRD_EYE_SCAN_MATRIX *info,
    SOC_SAND_IN SOC_PETRA_SRD_EYE_SCAN_OPTIMUM_INFO *optimum
  );

void
  soc_petra_PETRA_SRD_EYE_SCAN_OPTIMUM_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_EYE_SCAN_OPTIMUM_INFO *info
  );

void
  soc_petra_PETRA_SRD_EYE_SCAN_RANGE_print(
    SOC_SAND_IN  SOC_PETRA_SRD_EYE_SCAN_RANGE *info
  );

void
  soc_petra_PETRA_SRD_EYE_SCAN_INFO_print(
    SOC_SAND_IN SOC_PETRA_SRD_EYE_SCAN_INFO *info
  );

void
  soc_petra_PETRA_SRD_EYE_SCAN_RES_print(
    SOC_SAND_IN SOC_PETRA_SRD_EYE_SCAN_RES *info
  );

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_API_SERDES_UTILS_INCLUDED__*/
#endif
