/* $Id: soc_petra_api_auto_queue_flow_mgmt.h,v 1.5 Broadcom SDK $
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


#ifndef __SOC_PETRA_API_AUTO_QUEUE_FLOW_MGMT_INCLUDED__
/* { */
#define __SOC_PETRA_API_AUTO_QUEUE_FLOW_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/Petra/petra_api_ofp_rates.h>
#include <soc/dpp/Petra/petra_api_end2end_scheduler.h>
#include <soc/dpp/Petra/petra_api_ingress_traffic_mgmt.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_AQFM_NOF_HIERARCY              (2)
#define SOC_PETRA_AQFM_NOF_INDEX                 (SOC_PETRA_NOF_TRAFFIC_CLASSES)
#define SOC_PETRA_AQFM_MAX_NOF_FAPS              (2048)

#define SOC_PETRA_AQFM_MAX_NOF_CPU_PORTS         (7)
#define SOC_PETRA_AQFM_RCY_PORT_ID_DEFAULT       (72)

/* $Id: soc_petra_api_auto_queue_flow_mgmt.h,v 1.5 Broadcom SDK $
 * First UP: The first data port is 1.
 */

/*
 * Second UP: The first data port is 2.
 */

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
   *  The egress port information, consisting of the maximum
   *  scheduler credit rate and the maximum nominal rate of
   *  the port.
   */
  SOC_PETRA_OFP_RATE_INFO eg_port;
  /*
   *  The port scheduler is always of type HR. However, one
   *  might choose between the HR modes: single, dual or
   *  enhanced.
   */
  SOC_PETRA_SCH_PORT_INFO port_info;
}SOC_PETRA_AQFM_PORT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  This field characterizes the scheduling element: id, is
   *  enabled, type, per-type info and is dual.
   */
  SOC_PETRA_SCH_SE_INFO se;
  /*
   *  This field characterizes the flow: sub flows
   *  information, flow type and whether slow state is
   *  enabled.
   */
  SOC_PETRA_SCH_FLOW flow;
  /*
   *  Whether this flow is valid or not.
   */
  uint8 valid;
}SOC_PETRA_AQFM_AGG_ITEM_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  This parameter is a matrix of the number of hierarchies
   *  times the number of classes. Each element in the matrix
   *  represents an item that has the characteristics of the
   *  scheduling element and of the flow relevant for this
   *  item.
   */
  SOC_PETRA_AQFM_AGG_ITEM_INFO item[SOC_PETRA_AQFM_NOF_HIERARCY][SOC_PETRA_AQFM_NOF_INDEX];
}SOC_PETRA_AQFM_AGG_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  This array represents a group of flows as the number of
   *  Traffic Classes that one would like to open per port.
   */
  SOC_PETRA_SCH_FLOW flows[SOC_PETRA_NOF_TRAFFIC_CLASSES];
}SOC_PETRA_AQFM_FLOW_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  This array represents a group of queues as the number of
   *  Traffic Classes that one would like to open per
   *  destination FAP and destination port.
   */
  SOC_PETRA_ITM_QUEUE_INFO queues[SOC_PETRA_NOF_TRAFFIC_CLASSES];
}SOC_PETRA_AQFM_QUEUE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The minimum value of this scheduling element type range.
   */
  SOC_PETRA_SCH_SE_ID min;
  /*
   *  The maximum value of this scheduling element type range.
   */
  SOC_PETRA_SCH_SE_ID max;
}SOC_PETRA_AQFM_SYSTEM_SE_RANGE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  There are three types of scheduling elements: HR/ CL/
   *  FQ. Each type is given a certain range, determined by a
   *  MIN value and MAX value. This field sets the range for
   *  HR type SEs.
   */
  SOC_PETRA_AQFM_SYSTEM_SE_RANGE_INFO  hr;
  /*
   *  This field sets the range for CL type SEs.
   */
  SOC_PETRA_AQFM_SYSTEM_SE_RANGE_INFO  cl;
  /*
   *  This field sets the range for FQ type SEs.
   */
  SOC_PETRA_AQFM_SYSTEM_SE_RANGE_INFO  fq;
}SOC_PETRA_AQFM_AGG_BASE_IDS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The maximum number of faps to be configured in the
   *  system, and the maximum number of ports per FAP. The
   *  auto management will open all ports, flows, aggregates
   *  schedulers and queues, from each FAP to each destination
   *  port.
   */
  uint32 max_nof_faps_in_system;
  /*
   *  Number of FAP Data ports should be equal to:
   *  (nof_ports_per_nif) x (num_of_nifs) + nof_cpu_ports + nof_rcy_ports.
   */
  uint32 max_nof_ports_per_fap;
  /*
  *  Number of ports, connected to the NIF interface.
  */
  uint32 nof_nif_ports;
  /*
   *  According to the number of CPU ports in the system, the
   *  auto management will open the CPU port with ID 0 and 73
   *  - 78.
   */
  uint32 nof_cpu_ports;
  /*
  *  If TRUE, port 79 is the OLP port
  */
  uint8 support_olp_port;
  /*
   *  When the auto management will open the RCY according to
   *  the given ID, unless (rcy_port_id ==
   *  SOC_PETRA_FAP_PORT_ID_INVALID).
   */
  uint32 rcy_port_id;
  /*
   *  The number of Traffic Classes the system uses.
   */
  uint32 nof_traffic_classes;
  /*
   *  Scheduler Elements types ranges.
   */
  SOC_PETRA_AQFM_AGG_BASE_IDS agg_base_id;
}SOC_PETRA_AQFM_SYSTEM_INFO;

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

uint8
  soc_petra_aqfm_module_is_initialized_get(
    SOC_SAND_IN int unit
  );
/*********************************************************************
* NAME:
*     soc_petra_aqfm_system_info_save
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     This function saves the provided data in the SW database
*     for future use.
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_AQFM_SYSTEM_INFO    *p_info -
*     pointer to the struct to fill data.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_aqfm_system_info_save(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_AQFM_SYSTEM_INFO   *p_info
  );

/*********************************************************************
* NAME:
*     soc_petra_aqfm_system_info_defaults_get
* TYPE:
*   PROC
* DATE:
*   Nov 27 2007
* FUNCTION:
*     Each structure in the auto management system has a
*     function soc_petra_aqfm_XXX_defaults(). This function will:
*     1. Clean the structure; 2. Set default values to
*     parameters that have logical common values. 3. Set
*     invalid values to parameters that have to be configured
*     by user. The function
*     gets the structure SOC_PETRA_AQFM_SYSTEM_INFO as a parameter,
*     and fills it with the system's default values.
* INPUT:
*  SOC_SAND_OUT SOC_PETRA_AQFM_SYSTEM_INFO   *p_info -
*     pointer to the struct to fill data.
* REMARKS:
*     None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_aqfm_system_info_defaults_get(
    SOC_SAND_OUT SOC_PETRA_AQFM_SYSTEM_INFO   *p_info
  );

/*********************************************************************
* NAME:
*     soc_petra_aqfm_all_ports_open
* TYPE:
*   PROC
* DATE:
*   Nov 27 2007
* FUNCTION:
*     The function should be called at the initialization
*     phase The function will set the SOC_PETRA_AQFM_PORT_INFO as
*     the information of all the ports, and will also open all
*     the ports in the system according to that.
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_AQFM_PORT_INFO     *p_info -
*     The port information to be used for all the ports on the
*     card (CPU).
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_aqfm_all_ports_open(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_AQFM_PORT_INFO     *p_info
  );

/*********************************************************************
* NAME:
*     soc_petra_aqfm_all_aggregates_open
* TYPE:
*   PROC
* DATE:
*   Nov 27 2007
* FUNCTION:
*     The function should be called at the initialization
*     phase The function will set the
*     SOC_PETRA_AQFM_AGG_INFO[class_i] as the information of all
*     the aggs of class_i, and will also open all the aggs in
*     the system according to that.
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_AQFM_AGG_INFO      *p_info -
*     The aggregate information to be used for all the ports
*     on the card (CPU).
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_aqfm_all_aggregates_open(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_AQFM_AGG_INFO      *p_info
  );

/*********************************************************************
* NAME:
*     soc_petra_aqfm_all_queues_open
* TYPE:
*   PROC
* DATE:
*   Nov 27 2007
* FUNCTION:
*     The function should be called at the initialization
*     phase The function will set the
*     SOC_PETRA_AQFM_QUEUE_INFO[class_i] as the information of
*     all the queues of class_i, and will also open all the
*     flows in the system according to that.
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_AQFM_QUEUE_INFO    *p_info -
*     The queues information to be used for all the ports on
*     the card (CPU).
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_aqfm_all_queues_open(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_AQFM_QUEUE_INFO    *p_info
  );

/*********************************************************************
* NAME:
*     soc_petra_aqfm_all_flows_open
* TYPE:
*   PROC
* DATE:
*   Nov 27 2007
* FUNCTION:
*     The function should be called at the initialization
*     phase The function will set the SOC_PETRA_AQFM_FLOW_INFO
*     [class_i] as the information of all the flows of
*     class_i, and will also open all the flows in the system
*     according to that
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_AQFM_FLOW_INFO     *p_info -
*     The flows information to be used for all the ports on
*     the card (CPU).
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_aqfm_all_flows_open(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_AQFM_FLOW_INFO     *p_info
  );

uint32
  soc_petra_aqfm_port_open(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_PORT_INFO  *port_info
  );

uint32
  soc_petra_aqfm_port_update(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_PORT_INFO *p_port_info,
    SOC_SAND_OUT SOC_PETRA_AQFM_PORT_INFO *exact_port_info
  );

uint32
  soc_petra_aqfm_port_close(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            port_id
  );

uint32
  soc_petra_aqfm_agg_port_aggs_open(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_AGG_INFO*  agg_info,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_INFO*  exact_aggs_info
  );

uint32
  soc_petra_aqfm_agg_port_aggs_close(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            port_id
  );

uint32
  soc_petra_aqfm_agg_port_aggs_update(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_AGG_INFO       *aggs_info,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_INFO       *exact_aggs_info,
    SOC_SAND_IN  SOC_PETRA_AQFM_FLOW_INFO      *flows_info,
    SOC_SAND_OUT SOC_PETRA_AQFM_FLOW_INFO      *exact_flows_info
  );

uint32
  soc_petra_aqfm_agg_port_single_aggs_update(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  port_id,
    SOC_SAND_IN  uint32                  agg_level,
    SOC_SAND_IN  uint32                  agg_index,
    SOC_SAND_IN  SOC_PETRA_AQFM_AGG_ITEM_INFO  *aggs_info_item,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_ITEM_INFO  *exact_aggs_info_item
  );

uint32
  soc_petra_aqfm_queues_open(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              destination_fap_id,
    SOC_SAND_IN  uint32              destination_port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_QUEUE_INFO *auto_queue_info
  );

uint32
  soc_petra_aqfm_queues_update(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              destination_fap_id,
    SOC_SAND_IN  uint32              destination_port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_QUEUE_INFO *queues_info
  );

uint32
  soc_petra_aqfm_queues_close(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              destination_fap_id,
    SOC_SAND_IN  uint32              destination_port_id
  );

uint32
  soc_petra_aqfm_flow_open(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_FLOW_INFO  *flow_info
  );

uint32
  soc_petra_aqfm_flow_update(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              port_id,
    SOC_SAND_IN  SOC_PETRA_AQFM_FLOW_INFO   *flows_info,
    SOC_SAND_OUT SOC_PETRA_AQFM_FLOW_INFO   *exact_flows_info
  );

uint32
  soc_petra_aqfm_flow_close(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32            port_id
  );

void
  soc_petra_PETRA_AQFM_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_AQFM_PORT_INFO *info
  );

void
  soc_petra_PETRA_AQFM_AGG_ITEM_INFO_clear(
    int unit,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_ITEM_INFO *info
  );

void
  soc_petra_PETRA_AQFM_AGG_INFO_clear(
    int unit,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_INFO *info
  );

void
  soc_petra_PETRA_AQFM_FLOW_INFO_clear(
     int unit,
     SOC_SAND_OUT SOC_PETRA_AQFM_FLOW_INFO *info
  );

void
  soc_petra_PETRA_AQFM_QUEUE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_AQFM_QUEUE_INFO *info
  );

void
  soc_petra_PETRA_AQFM_SYSTEM_SE_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_AQFM_SYSTEM_SE_RANGE_INFO *info
  );

void
  soc_petra_PETRA_AQFM_AGG_BASE_IDS_clear(
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_BASE_IDS *info
  );

void
  soc_petra_PETRA_AQFM_SYSTEM_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_AQFM_SYSTEM_INFO *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1

void
  soc_petra_PETRA_AQFM_SYSTEM_SE_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PETRA_AQFM_SYSTEM_SE_RANGE_INFO *info
  );

void
  soc_petra_PETRA_AQFM_AGG_BASE_IDS_print(
    SOC_SAND_IN  SOC_PETRA_AQFM_AGG_BASE_IDS *info
  );

void
  soc_petra_PETRA_AQFM_PORT_INFO_print(
    SOC_SAND_IN SOC_PETRA_AQFM_PORT_INFO *info
  );

void
  soc_petra_PETRA_AQFM_AGG_ITEM_INFO_print(
    SOC_SAND_IN SOC_PETRA_AQFM_AGG_ITEM_INFO *info
  );

void
  soc_petra_PETRA_AQFM_AGG_INFO_print(
    SOC_SAND_IN SOC_PETRA_AQFM_AGG_INFO *info
  );

void
  soc_petra_PETRA_AQFM_FLOW_INFO_print(
    SOC_SAND_IN SOC_PETRA_AQFM_FLOW_INFO *info
  );

void
  soc_petra_PETRA_AQFM_QUEUE_INFO_print(
    SOC_SAND_IN SOC_PETRA_AQFM_QUEUE_INFO *info
  );

void
  soc_petra_PETRA_AQFM_SYSTEM_INFO_print(
    SOC_SAND_IN SOC_PETRA_AQFM_SYSTEM_INFO *info
  );

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_API_AQFM_SCHEME_INCLUDED__*/
#endif
