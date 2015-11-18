/* $Id: petra_scheduler_flows.h,v 1.6 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       soc_petra_scheduler_flows.h
*
* MODULE PREFIX:  soc_petra_scheduler_flows
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


#ifndef __SOC_PETRA_SCHEDULER_FLOWS_H_INCLUDED__
/* { */
#define __SOC_PETRA_SCHEDULER_FLOWS_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_end2end_scheduler.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

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
 *   soc_petra_sch_flow_ipf_config_mode_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the Independent-Per-Flow Weight configuration mode
 *   (proportional or inverse-proportional). The mode affects
 *   all subsequent configurations of the flow weight in
 *   Independent-Per-Flow mode, as configured by flow_set and
 *   aggregate_set APIs.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of device to access.
 *   SOC_SAND_IN  SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE mode -
 *     Independent-Per-Flow Weight configuration mode.
 * REMARKS:
 *   This API only sets the configuration mode, and affects
 *   the behavior of other APIs (flow_set, aggregate_set). It
 *   does not change current flow settings.
 * RETURNS:
 *   OK or ERROR indication.
 *********************************************************************/
uint32
  soc_petra_sch_flow_ipf_config_mode_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE mode
  );

uint32
  soc_petra_sch_flow_ipf_config_mode_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE mode
  );

/*********************************************************************
 *     Gets the configuration set by the
 *     "soc_petra_sch_flow_ipf_config_mode_set_unsafe" API.
 *     Refer to "soc_petra_sch_flow_ipf_config_mode_set_unsafe" API
 *     for details.
 *********************************************************************/
uint32
  soc_petra_sch_flow_ipf_config_mode_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW_IPF_CONFIG_MODE *mode
  );

/*****************************************************
* NAME
*   soc_petra_sch_flow_nof_subflows_get
* TYPE:
*   PROC
* DATE:
*   11/11/2007
* FUNCTION:
*   Gets the number of subflows of a given flow from the device.
* INPUT:
*   SOC_SAND_IN     int             unit -
*     Identifier of device to access.
*   SOC_SAND_IN     SOC_PETRA_SCH_FLOW_ID          base_flow_id -
*     Flow index of the base flow (the lower index of the two subflows)
*   SOC_SAND_OUT    uint32                   *nof_subflows -
*     The number of subflows (1 or 2)
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_flow_nof_subflows_get(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     SOC_PETRA_SCH_FLOW_ID          base_flow_id,
    SOC_SAND_OUT    uint32                   *nof_subflows
  );

/*****************************************************
* NAME
*   soc_petra_sch_flow_nof_subflows_set
* TYPE:
*   PROC
* DATE:
*   11/11/2007
* FUNCTION:
*   Sets the number of subflows of a given flow to the device.
* INPUT:
*   SOC_SAND_IN     int             unit -
*     Identifier of device to access.
*   SOC_SAND_IN     SOC_PETRA_SCH_FLOW_ID          base_flow_id -
*     Flow index of the base flow (the lower index of the two subflows)
*   SOC_SAND_OUT    uint32                   *nof_subflows -
*     The number of subflows (1 or 2)
*   SOC_SAND_IN     uint8                   is_odd_even -
*     TRUE if OddEven (0-1) per-1K configuration
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_flow_nof_subflows_set(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     SOC_PETRA_SCH_FLOW_ID          base_flow_id,
    SOC_SAND_IN     uint32                    nof_subflows,
    SOC_SAND_IN     uint8                   is_odd_even
  );

/*****************************************************
* NAME
*   soc_petra_sch_flow_slow_enable_set
* TYPE:
*   PROC
* DATE:
*   11/11/2007
* FUNCTION:
*   Gets slow enable indicantion for a given flow from the device.
* INPUT:
*   SOC_SAND_IN     SOC_PETRA_SCH_FLOW_ID          flow_ndx -
*     Flow index. Range: 0 - 56K-1.
*   SOC_SAND_IN     uint8                   *is_slow_enabled -
*     True if the flow is slow-enabled (meaning that it can be
*     in a slow state, with maximal rate according to the
*     configured slow rate.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_flow_slow_enable_set(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     SOC_PETRA_SCH_FLOW_ID           flow_ndx,
    SOC_SAND_IN     uint8                   is_slow_enabled
  );


/*****************************************************
* NAME
*   soc_petra_sch_flow_slow_enable_get
* TYPE:
*   PROC
* DATE:
*   11/11/2007
* FUNCTION:
*   Gets slow enable indicantion for a given flow from the device.
* INPUT:
*   SOC_SAND_IN     int             unit -
*     Identifier of device to access.
*   SOC_SAND_IN     SOC_PETRA_SCH_FLOW_ID          flow_ndx -
*     Flow index. Range: 0 - 56K-1.
*   SOC_SAND_OUT    uint8                   *is_slow_enabled -
*     True if the flow is slow-enabled (meaning that it can be
*     in a slow state, with maximal rate according to the
*     configured slow rate.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_flow_slow_enable_get(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     SOC_PETRA_SCH_FLOW_ID          flow_ndx,
    SOC_SAND_OUT    uint8                   *is_slow_enabled
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_flow_verify_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_flow_verify_set
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx -
*     Flow index to configure. This index must be identical to
*     the subflow index of the first (even) subflow in the
*     flow structure.
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW           *flow -
*     Flow parameters to set
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_flow_verify_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW           *flow
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_flow_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Sets a scheduler flow, from a scheduling element (or
*     elements) another element . The driver writes to the
*     following tables: Scheduler Enable Memory (SEM), Shaper
*     Descriptor Memory Static(SHDS) Flow Sub-Flow (FSF) Flow
*     Descriptor Memory Static (FDMS)
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx -
*     Flow index to configure. This index must be identical to
*     the subflow index of the first (even) subflow in the
*     flow structure.
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW           *flow -
*     Flow parameters to set
*  SOC_SAND_OUT SOC_PETRA_SCH_FLOW           *exact_flow -
*     Loaded with the actual written values. These can differ
*     from the given values due to rounding.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   1. When OddEven is FALSE (1-3 configuration),
*     composite configuration (flow with two subflows)
*     is only applicable for aggregates - not for simple flows.
*   2. If a sub-flow is declared invalid (flow->sub_flow.is_valid == FALSE),
*      it is deleted. The only relevant configuration for such a sub-flow
*      is the sub-flow 'id' field.
*   3. The sub-flow id-s are validated by the driver according to the flow_ndx
*      and the per-1K configuration. The index of the first sub-flow must be
*      equal to flow_ndx, and the index of the second sub-flow is according to
*      the odd-even configuration.
*********************************************************************/
uint32
  soc_petra_sch_flow_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID          flow_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW             *flow,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW             *exact_flow
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_flow_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_flow_set_unsafe
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx -
*     Flow index to configure. This index must be identical to
*     the subflow index of the first (even) subflow in the
*     flow structure.
*  SOC_SAND_OUT SOC_PETRA_SCH_FLOW           *flow -
*     Flow parameters to set
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_flow_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW           *flow
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_flow_status_verify
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_flow_status_set
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx -
*     The flow id (0-56K) of the requested flow.
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW_STATUS    state -
*     The requested state.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_flow_status_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_STATUS    state
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_flow_status_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Set flow state to off/on. The state of the flow will be
*     updated, unless was configured otherwise. Note: useful
*     for virtual flows, for which the flow state must be
*     explicitly set
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx -
*     The flow id (0-56K) of the requested flow.
*  SOC_SAND_IN  SOC_PETRA_SCH_FLOW_STATUS    state -
*     The requested state.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_flow_status_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW_STATUS    state
  );


/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_SCHEDULER_FLOWS_H_INCLUDED__*/
#endif
