/* $Id: soc_petra_serdes_utils.h,v 1.5 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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


#ifndef __SOC_PETRA_SERDES_UTILS_INCLUDED__
/* { */
#define __SOC_PETRA_SERDES_UTILS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_serdes_utils.h>
#include <soc/dpp/Petra/petra_chip_regs.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* minimal tlth margin for eye scanner */

/* maximal tlth value in eye scan optimum */

/* Min/Max values for eye scan resolution */
#define SOC_PETRA_SRD_EYE_SCAN_RESOLUTION_MIN (1)
#define SOC_PETRA_SRD_EYE_SCAN_RESOLUTION_MAX (10)

/* Min/Max values for eye scan prbs duration */
#define SOC_PETRA_SRD_EYE_SCAN_PRBS_DURATION_MIN_SEC_MIN (1)
#define SOC_PETRA_SRD_EYE_SCAN_PRBS_DURATION_MIN_SEC_MAX (60)

#define SOC_PETRA_SRD_RX_THRESH_OVRD_ENABLE   0

#if SOC_PETRA_SRD_RX_THRESH_OVRD_ENABLE
  #define SOC_PETRA_SRD_DISCKPAT_DFLT_VAL       0x1
  #define SOC_PETRA_SRD_RXTHRESHOLD_DFLT_VAL    0x10
    /*#define SOC_PETRA_SRD_DISCKPAT_LPBK_VAL       0x0*/
  #define SOC_PETRA_SRD_RXTHRESHOLD_LPBK_VAL    0x16
#else
  #define SOC_PETRA_SRD_DISCKPAT_DFLT_VAL       0x0
  #define SOC_PETRA_SRD_RXTHRESHOLD_DFLT_VAL    0x16
    /*#define SOC_PETRA_SRD_DISCKPAT_LPBK_VAL       0x0*/
  #define SOC_PETRA_SRD_RXTHRESHOLD_LPBK_VAL    0x16
#endif
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
*     soc_petra_srd_reg_write_unsafe
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
  soc_petra_srd_reg_write_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity,
    SOC_SAND_IN  uint32                 entity_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_REGS_ADDR       *reg,
    SOC_SAND_IN  uint8                   val
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_reg_read_unsafe
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
  soc_petra_srd_reg_read_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity,
    SOC_SAND_IN  uint32                 entity_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_REGS_ADDR       *reg,
    SOC_SAND_OUT uint8                   *val
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_fld_write_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Write a field (up to 8 bit) value to the SerDes.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
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
  soc_petra_srd_fld_write_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity,
    SOC_SAND_IN  uint32                 entity_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_REGS_FIELD      *fld,
    SOC_SAND_IN  uint8                   val
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_fld_read_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Read 8 bit value from the SerDes.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
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
  soc_petra_srd_fld_read_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity,
    SOC_SAND_IN  uint32                 entity_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_REGS_FIELD      *fld,
    SOC_SAND_OUT uint8                   *val
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_link_rx_eye_monitor_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function sets the equalizer mode and retrieves the
*     eye-sample.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
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
  soc_petra_srd_link_rx_eye_monitor_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_LANE_EQ_MODE    equalizer_mode,
    SOC_SAND_OUT uint32                  *eye_sample
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_loopback_mode_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function sets the loopback mode of a specified
*     lane.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
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
  soc_petra_srd_lane_loopback_mode_set_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_LANE_LOOPBACK_MODE loopback_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_loopback_mode_verify
* TYPE:
*   PROC
* FUNCTION:
*     This function sets the loopback mode of a specified
*     lane.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
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
  soc_petra_srd_lane_loopback_mode_verify(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_LANE_LOOPBACK_MODE loopback_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_loopback_mode_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function sets the loopback mode of a specified
*     lane.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
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
  soc_petra_srd_lane_loopback_mode_get_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_LANE_LOOPBACK_MODE *loopback_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_prbs_mode_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Sets the PRBS pattern.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
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
  soc_petra_srd_prbs_mode_set_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_PRBS_MODE       mode
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_prbs_mode_verify
* TYPE:
*   PROC
* FUNCTION:
*     Sets the PRBS pattern.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
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
  soc_petra_srd_prbs_mode_verify(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_PRBS_MODE       mode
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_prbs_mode_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Sets the PRBS pattern.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
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
  soc_petra_srd_prbs_mode_get_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_PRBS_MODE       *rx_mode,
    SOC_SAND_OUT SOC_PETRA_SRD_PRBS_MODE       *tx_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_prbs_start_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Start PRBS - generation (TX), reception (RX) or both.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
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
  soc_petra_srd_prbs_start_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_prbs_stop_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Stop PRBS - generation (TX), reception (RX) or both.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
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
  soc_petra_srd_prbs_stop_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_CONNECTION_DIRECTION direction_ndx
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_prbs_get_and_clear_stat_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Read the status on the receiver side. All indications
*     are cleared on read.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
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
  soc_petra_srd_prbs_get_and_clear_stat_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_PRBS_RX_STATUS  *status
  );

/* $Id: soc_petra_serdes_utils.h,v 1.5 Broadcom SDK $
 *  Returns TRUE if any SerDes lane indication is invalid
 */
uint8
  soc_petra_srd_lane_status_is_ok(
    SOC_SAND_IN SOC_PETRA_SRD_LANE_STATUS_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_lane_status_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function is used as a diagnostics tool per lane
*     that gives an indication about the lane and CMU's
*     status. It indicates per lane - loss of signal,
*     frequency lock is lost. And per CMU - PLL lock is lost.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
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
  soc_petra_srd_lane_status_get_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT SOC_PETRA_SRD_LANE_STATUS_INFO *lane_stt_info
  );

/*********************************************************************
*     This is a service function that sets the following configurations
*     per divisor mode (FULL/HALF/QUARTER data rate):
*     Slew-Mode and the parameters that are derived from the
*     Amplitude, Pre-Emphsis and Media-Type (amplitude, pre-curser,
*     post-curser and main-curser).
*********************************************************************/
uint32
  soc_petra_srd_lane_phys_params_config(
    SOC_SAND_IN  uint32               unit,
    SOC_SAND_IN  uint32               lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_RATE_DIVISOR  divisor,
    SOC_SAND_IN  uint8                 slew,
    SOC_SAND_IN  uint8                 amp,
    SOC_SAND_IN  uint8                 pre_curser_weight,
    SOC_SAND_IN  uint8                 post_curser_weight,
    SOC_SAND_IN  uint8                 main_weight
  );

/*********************************************************************
*  This is a service function that sets the equalization mode, for
*  the eye-monitor.
*********************************************************************/
uint32
  soc_petra_srd_link_rx_eye_monitor_mode_set(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_LANE_EQ_MODE    equalizer_mode
  );

/*********************************************************************
*  This is a service function that retrieves the equalization mode, for
*  the eye-monitor.
*********************************************************************/
uint32
  soc_petra_srd_link_rx_eye_monitor_mode_get(
    SOC_SAND_IN   uint32                 unit,
    SOC_SAND_IN   uint32                 lane_ndx,
    SOC_SAND_OUT  SOC_PETRA_SRD_LANE_EQ_MODE    *equalizer_mode
  );

/*********************************************************************
*  This is a service function that starts and runs the equalization
*  mode for the eye-monitor, for a given time.
*********************************************************************/
uint32
  soc_petra_srd_link_rx_eye_monitor_run(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_IN  uint32                 timeout_mili
  );

/*********************************************************************
*  This is a service function that retrieves the eye-height of the
*  eye-monitor.
*********************************************************************/
uint32
  soc_petra_srd_link_rx_eye_monitor_height_get(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 lane_ndx,
    SOC_SAND_OUT uint32                  *height
  );

/*********************************************************************
*  This is a service function that performs the procedure of loading
*  the IPU.
*********************************************************************/
uint32
  soc_petra_srd_ipu_mem_load(
    SOC_SAND_IN uint32            unit,
    SOC_SAND_IN  SOC_PETRA_SRD_STAR_ID  star_id
  );

/*****************************************************
*NAME
*  soc_petra_srd_fld_from_reg_get
*TYPE:
*  PROC
*DATE:
*  01/10/2007
*FUNCTION:
*  Gets field bits from an input buffer and puts them
*  in the output buffer
*INPUT:
*  SOC_SAND_IN  uint8          *reg_buffer,
*    Input buffer from which the function reads -
*    the register to read
*  SOC_SAND_IN  SOC_PETRA_SRD_REGS_FIELD  *field,
*    The field from which the bits are taken
*  SOC_SAND_IN  uint8          *fld_buffer
*    Output buffer to which the function writes -
*    the field to write.
*OUTPUT:
*****************************************************/
uint32
  soc_petra_srd_fld_from_reg_get(
    SOC_SAND_IN  uint8               *reg_buffer,
    SOC_SAND_IN  SOC_PETRA_SRD_REGS_FIELD  *fld,
    SOC_SAND_OUT uint8               *fld_buffer
  );

/*****************************************************
*NAME
*  soc_petra_srd_fld_from_reg_set
*TYPE:
*  PROC
*DATE:
*  01/10/2007
*FUNCTION:
*  Sets field bits in an output buffer after read from
*  an input buffer
*INPUT:
*  SOC_SAND_IN  uint8          *fld_buffer,
*    Input buffer from which the function reads -
*    the field to set.
*  SOC_SAND_IN  SOC_PETRA_SRD_REGS_FIELD  *field,
*    The field from which the bits are taken
*  SOC_SAND_INOUT uint8             *reg_buffer
*    Output buffer to which the function writes -
*    the register to set with the field
*OUTPUT:
*****************************************************/
uint32
  soc_petra_srd_fld_from_reg_set(
    SOC_SAND_IN  uint8               *fld_buffer,
    SOC_SAND_IN  SOC_PETRA_SRD_REGS_FIELD  *fld,
    SOC_SAND_INOUT uint8             *reg_buffer
  );


/************************************************************************/
/* Returns the number of SerDes quartets on the specified plane         */
/* This number is 4 for NIF-A, NIF-B and FAB-A, and 3 for FAB-B         */
/************************************************************************/
uint32
  soc_petra_srd_star_nof_qrtts_get(
    SOC_SAND_IN  SOC_PETRA_SRD_STAR_ID   star_id
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_qrtt_status_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Prints SerDes diagnostics, per SerDes quartet
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
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
  soc_petra_srd_qrtt_status_get_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint32                 qrtt_id,
    SOC_SAND_OUT SOC_PETRA_SRD_QRTT_STATUS_INFO *qrtt_status
  );

/*********************************************************************
* NAME:
*   soc_petra_srd_relock_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Performs SerDes CDR relock
* INPUT:
*   SOC_SAND_IN  uint32                    unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint32                    lane_ndx -
*     SerDes lane index. Range: 0 - 59.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_relock_unsafe(
    SOC_SAND_IN  uint32                  unit,
    SOC_SAND_IN  uint32                  lane_ndx
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_eye_scan_run_multiple_alloc_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Runs the eye scan, in order to map between serdes rx
*     parameters and amount of crc errors. Eye scan sequence
*     is as following: For each tlth * dfelth pair, run prbs
*     for a given time, and collect results to a matrix, per
*     lane. The matrix is used to find optimal rx parameters.
*     At the end of the run, optimal tlth and dfelth are
*     configured.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_SRD_EYE_SCAN_INFO   *info -
*     Scan parameters.
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
  soc_petra_srd_eye_scan_run_multiple_alloc_verify(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_EYE_SCAN_INFO   *info);

/*********************************************************************
* NAME:
*     soc_petra_srd_eye_scan_run_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Runs the eye scan, in order to map between serdes rx
*     parameters and amount of crc errors. Eye scan sequence
*     is as following: For each tlth * dfelth pair, run prbs
*     for a given time, and collect results to a matrix, per
*     lane. The matrix is used to find optimal rx parameters.
*     At the end of the run, optimal tlth and dfelth are
*     configured.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
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
  soc_petra_srd_eye_scan_run_multiple_alloc_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_EYE_SCAN_INFO   *info,
    SOC_SAND_IN  uint8                 silent,
    SOC_SAND_OUT SOC_PETRA_SRD_EYE_SCAN_RES    **res
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_scif_enable_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Enables/disables the SCIF interface. The SCIF is a
*     utility serial interface that can be used to access the
*     SerDes for debug configuration and diagnostics. It is
*     not required for normal operation, but may be used to
*     access the SerDes by external tools.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint8                 is_enabled -
*     If TRUE, the SCIF interface is enabled for use.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_srd_scif_enable_set_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint8                 is_enabled
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_scif_enable_verify
* TYPE:
*   PROC
* FUNCTION:
*     Enables/disables the SCIF interface. The SCIF is a
*     utility serial interface that can be used to access the
*     SerDes for debug configuration and diagnostics. It is
*     not required for normal operation, but may be used to
*     access the SerDes by external tools.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint8                 is_enabled -
*     If TRUE, the SCIF interface is enabled for use.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_srd_scif_enable_verify(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  uint8                 is_enabled
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_scif_enable_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Enables/disables the SCIF interface. The SCIF is a
*     utility serial interface that can be used to access the
*     SerDes for debug configuration and diagnostics. It is
*     not required for normal operation, but may be used to
*     access the SerDes by external tools.
* INPUT:
*  SOC_SAND_IN  uint32                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT uint8                 *is_enabled -
*     If TRUE, the SCIF interface is enabled for use.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_srd_scif_enable_get_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_OUT uint8                 *is_enabled
  );

/* } */
#if SOC_PETRA_DEBUG
uint32
  soc_petra_srd_diag_regs_dump_unsafe(
    SOC_SAND_IN uint32    unit
  );

#endif /*SOC_PETRA_DEBUG*/


#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_SERDES_UTILS_INCLUDED__*/
#endif

