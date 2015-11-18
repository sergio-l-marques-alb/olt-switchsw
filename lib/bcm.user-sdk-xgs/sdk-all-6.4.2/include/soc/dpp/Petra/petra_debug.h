/* $Id: soc_petra_debug.h,v 1.5 Broadcom SDK $
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


#ifndef __SOC_PETRA_DEBUG_INCLUDED__
/* { */
#define __SOC_PETRA_DEBUG_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_debug.h>
#include <soc/dpp/Petra/petra_general.h>

#include <soc/dpp/Petra/petra_api_general.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

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

/* $Id: soc_petra_debug.h,v 1.5 Broadcom SDK $
 *  Relevant when working in Mesh mode, Soc_petra-A only;
 *  retrieves the FAPs (FAP-Id 3 least significant bits, indicating Mesh Context) for which
 *  there are no connecting links.
 *  This API can be used to implement a WA for the following 
 *  Soc_petra-A Errata: in Mesh mode - extraction of Soc_petra can cause remaining Soc_petra devices to get stuck.
 *  The WA is to call this API periodically, or after extracting a Soc_petra device.
 *  The bitmap can be compared to the expected. If the bitmap for a specific FAP device is '1', 
 *  i.e. it has no links, but the device is known to be in System, ingress reset should be applied to the device.
 */
uint32
  soc_petra_dbg_faps_with_no_mesh_links_get_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint32  *fap_bitmap
  );

/*********************************************************************
* NAME:
*     soc_petra_dbg_route_force_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable if the traffic route should be forced. If
*     enabled, the traffic route either goes through the
*     fabric or remains local. Otherwise, the traffic route is
*     not forced.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_DBG_FORCE_MODE      force_mode -
*     Force fabric, force local or no route forcing.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_dbg_route_force_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DBG_FORCE_MODE      force_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_dbg_route_force_verify
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable if the traffic route should be forced. If
*     enabled, the traffic route either goes through the
*     fabric or remains local. Otherwise, the traffic route is
*     not forced.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_DBG_FORCE_MODE      force_mode -
*     Force fabric, force local or no route forcing.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_dbg_route_force_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DBG_FORCE_MODE      force_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_dbg_route_force_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable if the traffic route should be forced. If
*     enabled, the traffic route either goes through the
*     fabric or remains local. Otherwise, the traffic route is
*     not forced.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_DBG_FORCE_MODE      *force_mode -
*     Force fabric, force local or no route forcing.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_dbg_route_force_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_DBG_FORCE_MODE      *force_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_dbg_autocredit_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Configure the Scheduler AutoCredit parameters.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_DBG_AUTOCREDIT_INFO *info -
*     Scheduler AutoCredit parameters.
*  SOC_SAND_OUT uint32                  *exact_rate -
*     May vary due to rounding.
* REMARKS:
*     Note: if first_queue > last_queue, then the AutoCredit
*     is set for all the queues.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_dbg_autocredit_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DBG_AUTOCREDIT_INFO *info,
    SOC_SAND_OUT uint32                  *exact_rate
  );

/*********************************************************************
* NAME:
*     soc_petra_dbg_autocredit_verify
* TYPE:
*   PROC
* FUNCTION:
*     Configure the Scheduler AutoCredit parameters.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_DBG_AUTOCREDIT_INFO *info -
*     Scheduler AutoCredit parameters.
* REMARKS:
*     Note: if first_queue > last_queue, then the AutoCredit
*     is set for all the queues.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_dbg_autocredit_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DBG_AUTOCREDIT_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_dbg_autocredit_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Configure the Scheduler AutoCredit parameters.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_DBG_AUTOCREDIT_INFO *info -
*     Scheduler AutoCredit parameters.
* REMARKS:
*     Note: if first_queue > last_queue, then the AutoCredit
*     is set for all the queues.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_dbg_autocredit_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_DBG_AUTOCREDIT_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_dbg_egress_shaping_enable_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable the egress shaping.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint8                 enable -
*     If TRUE, the egress shaping is enabled. Otherwise, it is
*     disabled.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_dbg_egress_shaping_enable_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     soc_petra_dbg_egress_shaping_enable_verify
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable the egress shaping.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint8                 enable -
*     If TRUE, the egress shaping is enabled. Otherwise, it is
*     disabled.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_dbg_egress_shaping_enable_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     soc_petra_dbg_egress_shaping_enable_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable the egress shaping.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT uint8                 *enable -
*     If TRUE, the egress shaping is enabled. Otherwise, it is
*     disabled.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_dbg_egress_shaping_enable_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  );

/*********************************************************************
* NAME:
*     soc_petra_dbg_flow_control_enable_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable device-level flow control.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint8                 enable -
*     Flow control mode: enabled or disabled.
* REMARKS:
*     Note: Enable/disable device-level flow control. This
*     does not affect the flow control threshold
*     configuration, and internal per-block flow control
*     configuration (e.g. SCH, NIF). If disabled, it disables
*     inter-block flow control (e.g. EGQ to SCH, NIF to
*     EGQ/SCH, etc.). For more details on device-level flow
*     control, please refer to the user manual.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_dbg_flow_control_enable_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     soc_petra_dbg_flow_control_enable_verify
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable device-level flow control.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint8                 enable -
*     Flow control mode: enabled or disabled.
* REMARKS:
*     Note: Enable/disable device-level flow control. This
*     does not affect the flow control threshold
*     configuration, and internal per-block flow control
*     configuration (e.g. SCH, NIF). If disabled, it disables
*     inter-block flow control (e.g. EGQ to SCH, NIF to
*     EGQ/SCH, etc.). For more details on device-level flow
*     control, please refer to the user manual.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_dbg_flow_control_enable_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     soc_petra_dbg_flow_control_enable_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Enable/disable device-level flow control.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT uint8                 *enable -
*     Flow control mode: enabled or disabled.
* REMARKS:
*     Note: Enable/disable device-level flow control. This
*     does not affect the flow control threshold
*     configuration, and internal per-block flow control
*     configuration (e.g. SCH, NIF). If disabled, it disables
*     inter-block flow control (e.g. EGQ to SCH, NIF to
*     EGQ/SCH, etc.). For more details on device-level flow
*     control, please refer to the user manual.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_dbg_flow_control_enable_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  );

/*********************************************************************
* NAME:
*   soc_petra_dbg_queue_flush_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Flush one queue at the ingress.
* INPUT:
*   SOC_SAND_IN   int                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN   uint32                 queue_ndx -
*     VOQ index. Range: 0 - 32K-1.
*   SOC_SAND_IN   SOC_PETRA_DBG_FLUSH_MODE      mode -
*     Flushing mode: the packets in the queue can be dequeued
*     or deleted.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_dbg_queue_flush_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 queue_ndx,
    SOC_SAND_IN  SOC_PETRA_DBG_FLUSH_MODE      mode
  );

/*********************************************************************
* NAME:
*   soc_petra_dbg_queue_flush_all_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Flush all the queues at the ingress.
* INPUT:
*   SOC_SAND_IN   int                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN   SOC_PETRA_DBG_FLUSH_MODE      mode -
*     Flushing mode: the packets in the queue can be dequeued
*     or deleted.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_dbg_queue_flush_all_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DBG_FLUSH_MODE      mode
  );

/*********************************************************************
* NAME:
*   soc_petra_dbg_ingr_reset_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Resets the ingress pass. The following blocks are
*   soft-reset (running soft-init): IPS, IQM, IPT, MMU,
*   DPRC, IRE, IHP, IDR, IRR. As part of the reset sequence,
*   traffic is stopped, and re-started (according to the
*   original condition).
* INPUT:
*   SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* REMARKS:
*   1. This debug function is traffic-affecting (traffic is
*   stopped internally, all queues are emptied)2. Depricated
*   - the functionality is covered by dbg_dev_reset API
*   using DBG_RST_DOMAIN_INGR
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_dbg_ingr_reset_unsafe(
    SOC_SAND_IN  int                 unit
  );

 /*********************************************************************
 * NAME:
 *   soc_petra_dbg_dev_reset_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Soft-resets the device. As part of the reset sequence,
 *   traffic is stopped, and re-started (according to the
 *   original condition).
 * INPUT:
 *   SOC_SAND_IN	SOC_PETRA_DBG_RST_DOMAIN	rst_domain -
 *   Reset domain. Typical domain is "Full device reset".
 *   Can be also ingress-only, egress-only etc.
 * REMARKS:
 *   1. This debug function is traffic-affecting (traffic is
 *   stopped internally, all queues are emptied)
 * RETURNS:
 *   OK or ERROR indication.
 *********************************************************************/
uint32
  soc_petra_dbg_dev_reset_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DBG_RST_DOMAIN      rst_domain
  );

/*********************************************************************
 * NAME:
 *   soc_petra_dbg_DPRC_reset_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function does dprc reset.
 * INPUT:
 *   SOC_SAND_IN  int                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                 dram_ndx -
 *     identifier of the dram.
 * REMARKS:
 *   none
 * RETURNS:
 *   OK or ERROR indication.
 *********************************************************************/
uint32
  soc_petra_dbg_dprc_reset_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx
  );


uint32
  soc_petra_dbg_dev_reset_verify(
    SOC_SAND_IN  SOC_PETRA_DBG_RST_DOMAIN      rst_domain
  );
  
/*********************************************************************
* NAME:
*   soc_petra_dbg_eci_access_tst_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   This function tests the ECI access to Soc_petra.
* INPUT:
*   SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint32                 nof_k_iters -
*     Number of iterations, in 1000 units. For example,
*     setting '100' runs 100000 iterations.
*   SOC_SAND_IN  uint8                 use_dflt_tst_reg -
*     If TRUE, the default test register is used. This
*     register inverts the data written to it. If this option
*     is selected, the following register addresses are not
*     required.
*   SOC_SAND_IN  uint32                  reg1_addr_longs -
*     Optional (only if use_dflt_tst_reg is FALSE). If not
*     using the default test register, the data is written to
*     intermittently to the two registers supplied. The
*     register address is in longs (i.e. the same way it is
*     described in the Data Sheet.)
*   SOC_SAND_IN  uint32                  reg2_addr_longs -
*     Optional (only if use_dflt_tst_reg is FALSE) If not
*     using the default test register, the data is written to
*     intermittently to the two registers supplied. The
*     register address is in longs (i.e. the same way it is
*     described in the Data Sheet.)
*   SOC_SAND_OUT uint8                 *is_valid -
*     The returned value. If TRUE, no problems in the ECI
*     access were found. Otherwise, some problems were found
*     in the ECI access.
* REMARKS:
*   If the SOC_PETRA_DBG level is set to SOC_SAND_DBG_LVL2 or above,
*   the test will print all addresses for which the ECI
*   access failed.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_dbg_eci_access_tst_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 nof_k_iters,
    SOC_SAND_IN  uint8                 use_dflt_tst_reg,
    SOC_SAND_IN  uint32                  reg1_addr_longs,
    SOC_SAND_IN  uint32                  reg2_addr_longs,
    SOC_SAND_OUT uint8                 *is_valid
  );

/*********************************************************************
* NAME:
 *   soc_petra_dbg_sch_reset_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Resets the end-to-end scheduler. The reset is performed
 *   by clearing the internal scheduler pipes, and then
 *   performing soft-reset.
 * INPUT:
 *   SOC_SAND_IN  int unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   1. This debug function is traffic-affecting, on a system
 *   level, since all credit generation of the device is
 *   stopped until the internal scheduler initialization
 *   completes after reset. 2. Depricated - the functionality
 *   is covered by dbg_dev_reset API using DBG_RST_DOMAIN_SCH
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_dbg_sch_reset_unsafe(
    SOC_SAND_IN  int unit
  );

/*********************************************************************
* NAME:
 *   soc_petra_dbg_sch_suspect_spr_detect_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Test a Flow Shaper as a potential-stuck-shaper. The
 *   criteria is: - The shaper is active, i.e. configured
 *   max-burst and rate are non-zero. - The shaper bucket is
 *   empty (zero-size). Note: this state is a valid state,
 *   since a bucket can be emptied by received credits. The
 *   stages for detecting a stuck shaper are: 1. Detect a
 *   suspected shaper (empty bucket) 2. Start measuring the
 *   credits to the suspected shaper using a programmable
 *   counter 3. Wait for at least 500 milliseconds to see if
 *   the shaper receives credits 4. Confirm a stuck shaper:
 *   check if the shaper bucket is empty. If empty, and no
 *   credits were received over the measurement period, the
 *   shaper is stuck. This confirmation is done by the
 *   dbg_sch_suspect_spr_confirm API.
 * INPUT:
 *   SOC_SAND_IN  int unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32 flow_id -
 *     The flow to test. IMPORTANT: Note that in the case of
 *     composite flow, the flow_id can be the index of each
 *     subflow.
 *   SOC_SAND_OUT uint8 *is_suspect -
 *     If TRUE, the flow (sub-flow) is suspect
 * REMARKS:
 *   The application using this API should typically
 *   implement a task scanning all flows in use. Upon
 *   identifying a suspected flow by the
 *   'dbg_sch_suspect_spr_detect' API, wait (at list 500
 *   milliseconds) and confirm a stuck shaper using the
 *   'dbg_sch_suspect_spr_confirm' API. If a stuck shaper is
 *   confirmed, it can be released using the 'dbg_sch_reset'
 *   API (traffic effecting).
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_dbg_sch_suspect_spr_detect_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 flow_id,
    SOC_SAND_OUT uint8 *is_suspect
  );

uint32
  soc_petra_dbg_sch_suspect_spr_detect_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 flow_id
  );

/*********************************************************************
* NAME:
 *   soc_petra_dbg_sch_suspect_spr_confirm_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Confirm a potential-stuck-shaper. The stages for
 *   detecting a stuck shaper are: 1. Detect a suspected
 *   shaper (empty bucket) 2. Start measuring the credits to
 *   the suspected shaper using a programmable counter 3.
 *   Wait for at least 500 milliseconds to see if the shaper
 *   receives credits 4. Confirm a stuck shaper: check if the
 *   shaper bucket is empty. If empty, and no credits were
 *   received over the measurement period, the shaper is
 *   stuck.
 * INPUT:
 *   SOC_SAND_IN  int unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32 flow_id -
 *     The flow to test. IMPORTANT: Note that in the case of
 *     composite flow, the flow_id can be the index of each
 *     subflow.
 *   SOC_SAND_OUT uint8 *is_confirmed -
 *     If TRUE, the flow (sub-flow) shaper is stuck (confirmed)
 * REMARKS:
 *   The application using this API should typically
 *   implement a task scanning all flows in use. Upon
 *   identifying a suspected flow by the
 *   'dbg_sch_suspect_spr_detect' API, wait (at list 500
 *   milliseconds) and confirm a stuck shaper using the
 *   'dbg_sch_suspect_spr_confirm' API. If a stuck shaper is
 *   confirmed, it can be released using the 'dbg_sch_reset'
 *   API (traffic effecting).
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_dbg_sch_suspect_spr_confirm_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 flow_id,
    SOC_SAND_OUT uint8 *is_confirmed
  );

uint32
  soc_petra_dbg_sch_suspect_spr_confirm_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 flow_id
  );

/*********************************************************************
* NAME:
 *   soc_petra_dbg_pcm_readings_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Measures the PCM values for the device's blocks.
 * INPUT:
 *   SOC_SAND_IN  int                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PETRA_DBG_PCM_RESULTS     *results -
 *     The results of the measurements.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_dbg_pcm_readings_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_DBG_PCM_RESULTS     *results
  );

uint32
  soc_petra_dbg_pcm_readings_get_verify(
    SOC_SAND_IN  int                 unit
  );

uint32
  soc_petra_PETRA_DBG_AUTOCREDIT_INFO_verify(
    SOC_SAND_IN  SOC_PETRA_DBG_AUTOCREDIT_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_DEBUG_INCLUDED__*/
#endif

