/* $Id: petra_api_egr_queuing.h,v 1.8 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/soc_petra/include/soc_petra_api_egr_queuing.h
*
* MODULE PREFIX:  soc_petra_egr
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


#ifndef __SOC_PETRA_API_EGR_QUEUING_INCLUDED__
/* { */
#define __SOC_PETRA_API_EGR_QUEUING_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/TMC/tmc_api_egr_queuing.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PETRA_EGR_OFP_SCH_WFQ_WEIGHT_MAX 255

#define SOC_PETRA_EGR_OFP_INTERFACE_PRIO_NONE SOC_TMC_EGR_OFP_INTERFACE_PRIO_NONE
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

#define SOC_PETRA_EGR_Q_PRIO_LOW                               SOC_TMC_EGR_Q_PRIO_LOW
#define SOC_PETRA_EGR_Q_PRIO_HIGH                              SOC_TMC_EGR_Q_PRIO_HIGH
#define SOC_PETRA_EGR_NOF_Q_PRIO                               SOC_TMC_EGR_NOF_Q_PRIO_PB
typedef SOC_TMC_EGR_Q_PRIO                                     SOC_PETRA_EGR_Q_PRIO;

#define SOC_PETRA_EGR_PORT_THRESH_TYPE_0                       SOC_TMC_EGR_PORT_THRESH_TYPE_0
#define SOC_PETRA_EGR_PORT_THRESH_TYPE_1                       SOC_TMC_EGR_PORT_THRESH_TYPE_1
#define SOC_PETRA_EGR_PORT_THRESH_TYPE_2                       SOC_TMC_EGR_PORT_THRESH_TYPE_2
#define SOC_PETRA_EGR_PORT_THRESH_TYPE_3                       SOC_TMC_EGR_PORT_THRESH_TYPE_3
#define SOC_PETRA_EGR_PORT_NOF_THRESH_TYPES                    SOC_TMC_EGR_PORT_NOF_THRESH_TYPES_PETRA
typedef SOC_TMC_EGR_PORT_THRESH_TYPE                           SOC_PETRA_EGR_PORT_THRESH_TYPE;

#define SOC_PETRA_EGR_MCI_GUARANTEED                           SOC_TMC_EGR_MCI_GUARANTEED
#define SOC_PETRA_EGR_MCI_BE                                   SOC_TMC_EGR_MCI_BE
#define SOC_PETRA_EGR_NOF_MCI_PRIORITIES                       SOC_TMC_EGR_NOF_MCI_PRIORITIES
typedef SOC_TMC_EGR_MCI_PRIO                                   SOC_PETRA_EGR_MCI_PRIO;

#define SOC_PETRA_EGR_OFP_INTERFACE_PRIO_HIGH                  SOC_TMC_EGR_OFP_INTERFACE_PRIO_HIGH
#define SOC_PETRA_EGR_OFP_INTERFACE_PRIO_MID                   SOC_TMC_EGR_OFP_INTERFACE_PRIO_MID
#define SOC_PETRA_EGR_OFP_INTERFACE_PRIO_LOW                   SOC_TMC_EGR_OFP_INTERFACE_PRIO_LOW
#define SOC_PETRA_EGR_OFP_INTERFACE_PRIO_PFC_LOWEST            SOC_TMC_EGR_OFP_INTERFACE_PRIO_PFC_LOWEST
#define SOC_PETRA_EGR_OFP_CHNIF_NOF_PRIORITIES                 SOC_TMC_EGR_OFP_CHNIF_NOF_PRIORITIES
typedef SOC_TMC_EGR_OFP_INTERFACE_PRIO                         SOC_PETRA_EGR_OFP_INTERFACE_PRIO;

#define SOC_PETRA_EGR_HP_OVER_LP_ALL                           SOC_TMC_EGR_HP_OVER_LP_ALL
#define SOC_PETRA_EGR_HP_OVER_LP_PER_TYPE                      SOC_TMC_EGR_HP_OVER_LP_PER_TYPE
#define SOC_PETRA_EGR_HP_OVER_LP_FAIR                          SOC_TMC_EGR_HP_OVER_LP_FAIR
typedef SOC_TMC_EGR_OFP_SCH_MODE                               SOC_PETRA_EGR_OFP_SCH_MODE;

#define SOC_PETRA_EGR_UCAST_TO_SCHED                           SOC_TMC_EGR_UCAST_TO_SCHED
#define SOC_PETRA_EGR_MCAST_TO_UNSCHED                         SOC_TMC_EGR_MCAST_TO_UNSCHED
#define SOC_PETRA_EGR_NOF_Q_PRIO_MAPPING_TYPES                 SOC_TMC_EGR_NOF_Q_PRIO_MAPPING_TYPES
typedef SOC_TMC_EGR_Q_PRIO_MAPPING_TYPE                        SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE;

typedef SOC_TMC_EGR_DROP_THRESH                                SOC_PETRA_EGR_DROP_THRESH;
typedef SOC_TMC_EGR_FC_DEV_THRESH_INNER                        SOC_PETRA_EGR_FC_DEV_THRESH_INNER;
typedef SOC_TMC_EGR_FC_DEVICE_THRESH                           SOC_PETRA_EGR_FC_DEVICE_THRESH;
typedef SOC_TMC_EGR_FC_MCI_THRESH                              SOC_PETRA_EGR_FC_MCI_THRESH;
typedef SOC_TMC_EGR_FC_CHNIF_THRESH                            SOC_PETRA_EGR_FC_CHNIF_THRESH;
typedef SOC_TMC_EGR_FC_OFP_THRESH                              SOC_PETRA_EGR_FC_OFP_THRESH;
typedef SOC_TMC_EGR_OFP_SCH_WFQ                                SOC_PETRA_EGR_OFP_SCH_WFQ;
typedef SOC_TMC_EGR_OFP_SCH_INFO                               SOC_PETRA_EGR_OFP_SCH_INFO;


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
*     soc_petra_egr_ofp_thresh_type_set
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Sets Outgoing FAP Port (OFP) threshold type, per port.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx -
*     OFP index. Range: 0 - SOC_PETRA_FAP_PORT_ID_MAX
*  SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_thresh_type -
*     The threshold type to set. Port-level Drop thresholds
*     and Flow control thresholds will be set per threshold
*     type.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_ofp_thresh_type_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_thresh_type
  );

uint32
  soc_petra_egr_ofp_thresh_type_set_dispatch(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  int                     core_id,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID   ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_thresh_type
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_thresh_type_get
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Sets Outgoing FAP Port (OFP) threshold type, per port.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx -
*     OFP index. Range: 0 - SOC_PETRA_FAP_PORT_ID_MAX
*  SOC_SAND_OUT SOC_PETRA_EGR_PORT_THRESH_TYPE *ofp_thresh_type -
*     The threshold type to set. Port-level Drop thresholds
*     and Flow control thresholds will be set per threshold
*     type.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_ofp_thresh_type_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_PORT_THRESH_TYPE *ofp_thresh_type
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_sched_drop_set
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Set scheduled drop thresholds for egress queues per
*     queue-priority.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx -
*     Queue priority index. Range: SOC_PETRA_EGR_Q_PRIO_LOW -
*     SOC_PETRA_EGR_Q_PRIO_HIGH.
*  SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh -
*     drop thresholds to set.
*  SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *exact_thresh -
*     will be filled with exact values.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_sched_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  int                 profile,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *exact_thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_sched_drop_get
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Set scheduled drop thresholds for egress queues per
*     queue-priority.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx -
*     Queue priority index. Range: SOC_PETRA_EGR_Q_PRIO_LOW -
*     SOC_PETRA_EGR_Q_PRIO_HIGH.
*  SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *thresh -
*     drop thresholds to set.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_sched_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_unsched_drop_set
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Set unscheduled drop thresholds for egress queues, per
*     queue-priority and drop precedence.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx -
*     Queue priority index. Range: SOC_PETRA_EGR_Q_PRIO_LOW -
*     SOC_PETRA_EGR_Q_PRIO_HIGH.
*  SOC_SAND_IN  uint32                 dp_ndx -
*     Drop precedence index. Range: 0 - SOC_PETRA_NOF_DP-1.
*  SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh -
*     drop thresholds to set.
*  SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *exact_thresh -
*     Will be filled with exact values.
* REMARKS:
*     The unscheduled traffic is assigned to the matching threshold
*     not according to the packet Drop Precedence, rather then
*     according to the Unscheduled Drop Priority value, as set
*     by the 'soc_petra_egr_unsched_drop_prio_set' API, per TC and DP
*     The "dp_ndx" in this API refers to this Drop Priority, and not
*     directly to the packet Drop Precedence field
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_unsched_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  int                 profile,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *exact_thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_unsched_drop_get
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Set unscheduled drop thresholds for egress queues, per
*     queue-priority and drop precedence.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx -
*     Queue priority index. Range: SOC_PETRA_EGR_Q_PRIO_LOW -
*     SOC_PETRA_EGR_Q_PRIO_HIGH.
*  SOC_SAND_IN  uint32                 dp_ndx -
*     Drop precedence index. Range: 0 - SOC_PETRA_NOF_DP-1.
*  SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *thresh -
*     drop thresholds to set.
* REMARKS:
*     The unscheduled traffic is assigned to the matching threshold
*     not according to the packet Drop Precedence, rather then
*     according to the Unscheduled Drop Priority value, as set
*     by the 'soc_petra_egr_unsched_drop_prio_set' API, per TC and DP
*     The "dp_ndx" in this API refers to this Drop Priority, and not
*     directly to the packet Drop Precedence field
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_unsched_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_dev_fc_set
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Set Flow Control thresholds for egress queues, based on
*     device-level resources. Threshold are set for overall
*     resources, and scheduled resources.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_EGR_FC_DEVICE_THRESH *thresh -
*     Flow Control thresholds to set - Device-level resources.
*  SOC_SAND_OUT SOC_PETRA_EGR_FC_DEVICE_THRESH *exact_thresh -
*     Will be filled with exact values.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_dev_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_DEVICE_THRESH *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_DEVICE_THRESH *exact_thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_dev_fc_get
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Set Flow Control thresholds for egress queues, based on
*     device-level resources. Threshold are set for overall
*     resources, and scheduled resources.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_EGR_FC_DEVICE_THRESH *thresh -
*     Flow Control thresholds to set - Device-level resources.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_dev_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_DEVICE_THRESH *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_xaui_spaui_fc_set
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Set Flow Control thresholds for egress queues, per
*     channelized interface port, based on Channelized NIF
*     Ports resources.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx -
*     Channelized NIF index. Range: 0 -
*     SOC_PETRA_NOF_INTERFACES-1.
*  SOC_SAND_IN  SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh -
*     Flow Control thresholds to set - CHNIF resources.
*  SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *exact_thresh -
*     Will be filled with exact values.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_xaui_spaui_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *exact_thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_xaui_spaui_fc_get
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Set Flow Control thresholds for egress queues, per
*     channelized interface port, based on Channelized NIF
*     Ports resources.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx -
*     Channelized NIF index. Range: 0 -
*     SOC_PETRA_NOF_INTERFACES-1.
*  SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh -
*     Flow Control thresholds to set - CHNIF resources.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_xaui_spaui_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_fc_set
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Set Flow Control thresholds for egress queues, per port
*     queue priority and threshold type, based on Outgoing FAP
*     Port (OFP) resources.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx -
*     Queue priority index. Range: SOC_PETRA_EGR_Q_PRIO_LOW -
*     SOC_PETRA_EGR_Q_PRIO_HIGH.
*  SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_type_ndx -
*     Per-port threshold type index. Range: 0 -
*     SOC_PETRA_EGR_PORT_NOF_THRESH_TYPES-1.
*  SOC_SAND_IN  SOC_PETRA_EGR_FC_OFP_THRESH   *thresh -
*     Flow Control thresholds to set - FAP Port resources.
*  SOC_SAND_OUT SOC_PETRA_EGR_FC_OFP_THRESH   *exact_thresh -
*     Will be filled with exact values.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_ofp_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_OFP_THRESH   *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_OFP_THRESH   *exact_thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_fc_get
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Set Flow Control thresholds for egress queues, per port
*     queue priority and threshold type, based on Outgoing FAP
*     Port (OFP) resources.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx -
*     Queue priority index. Range: SOC_PETRA_EGR_Q_PRIO_LOW -
*     SOC_PETRA_EGR_Q_PRIO_HIGH.
*  SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_type_ndx -
*     Per-port threshold type index. Range: 0 -
*     SOC_PETRA_EGR_PORT_NOF_THRESH_TYPES-1.
*  SOC_SAND_OUT SOC_PETRA_EGR_FC_OFP_THRESH   *thresh -
*     Flow Control thresholds to set - FAP Port resources.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_ofp_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_OFP_THRESH   *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_mci_fc_set
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Set Flow Control thresholds for egress queues, per MCI
*     priority, based on unscheduled traffic resources.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx -
*     MCI priority index. Range: 0 -
*     SOC_PETRA_EGR_NOF_MCI_PRIORITIES-1.
*  SOC_SAND_IN  SOC_PETRA_EGR_FC_MCI_THRESH   *thresh -
*     Flow Control thresholds to set - Unscheduled resources.
*  SOC_SAND_OUT SOC_PETRA_EGR_FC_MCI_THRESH   *exact_thresh -
*     Will be filled with exact values.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_mci_fc_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_MCI_THRESH   *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_MCI_THRESH   *exact_thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_mci_fc_get
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Set Flow Control thresholds for egress queues, per MCI
*     priority, based on unscheduled traffic resources.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx -
*     MCI priority index. Range: 0 -
*     SOC_PETRA_EGR_NOF_MCI_PRIORITIES-1.
*  SOC_SAND_OUT SOC_PETRA_EGR_FC_MCI_THRESH   *thresh -
*     Flow Control thresholds to set - Unscheduled resources.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_mci_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_MCI_THRESH   *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_mci_fc_enable_set
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Enable/Disable Flow Control for- MCI (Multicast
*     Congestion Indication)- ERP (Egress Replication Port)
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx -
*     MCI priority index. Range: 0 -
*     SOC_PETRA_EGR_NOF_MCI_PRIORITIES-1.
*  SOC_SAND_IN  uint8                 mci_enable -
*     If TRUE, the requested MCI FC will be enabled. If FALSE,
*     the requested MCI FC will be disabled.
*  SOC_SAND_IN  uint8                 erp_enable -
*     Enable/Disable the Egress Replication Port (ERP) flow
*     control.
* REMARKS:
*   1. ERP flow control may only be enabled for mci_prio_ndx
*      equal to SOC_PETRA_EGR_MCI_BE.
*      Setting it for other indexes will cause an error.
*   2. ERP flow control may only be enabled
*      if the appropriate MCI Flow Control is disabled.
*      Setting it and also setting the appropriate MCI FC
*      will cause an error.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_mci_fc_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_IN  uint8                 mci_enable,
    SOC_SAND_IN  uint8                 erp_enable
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_mci_fc_enable_get
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Enable/Disable Flow Control for- MCI (Multicast
*     Congestion Indication)- ERP (Egress Replication Port)
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx -
*     MCI priority index. Range: 0 -
*     SOC_PETRA_EGR_NOF_MCI_PRIORITIES-1.
*  SOC_SAND_OUT uint8                 *mci_enable -
*     If TRUE, the requested MCI FC will be enabled. If FALSE,
*     the requested MCI FC will be disabled.
*  SOC_SAND_OUT uint8                 *erp_enable -
*     Enable/Disable the Egress Replication Port (ERP) flow
*     control.
* REMARKS:
*   1. ERP flow control may only be enabled for mci_prio_ndx
*      equal to SOC_PETRA_EGR_MCI_BE.
*      Setting it for other indexes will cause an error.
*   2. ERP flow control may only be enabled
*      if the appropriate MCI Flow Control is disabled.
*      Setting it and also setting the appropriate MCI FC
*      will cause an error.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_mci_fc_enable_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_OUT uint8                 *mci_enable,
    SOC_SAND_OUT uint8                 *erp_enable
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_sch_mode_set
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Set egress scheduling mode for outgoing FAP ports. Two
*     possible modes exist: - SP between the HP (low delay)
*     and the LP (Normal) queues, and SP among the two HP
*     queues, and WFQ among the LP queues. This configuration
*     ensures that any low delay traffic is always sent ahead
*     of any normal traffic. - WFQ among overall egress
*     scheduled and unscheduled traffic, and SP among HP and
*     LP. This configuration first allocates the bandwidth
*     between the scheduled and the unscheduled traffic types,
*     then for each type ensures that low delay (HP) is sent
*     before normal (LP) traffic.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_MODE    *sch_mode -
*     Outgoing FAP Port scheduling configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_ofp_sch_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_MODE    *sch_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_sch_mode_get
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Set egress scheduling mode for outgoing FAP ports. Two
*     possible modes exist: - SP between the HP (low delay)
*     and the LP (Normal) queues, and SP among the two HP
*     queues, and WFQ among the LP queues. This configuration
*     ensures that any low delay traffic is always sent ahead
*     of any normal traffic. - WFQ among overall egress
*     scheduled and unscheduled traffic, and SP among HP and
*     LP. This configuration first allocates the bandwidth
*     between the scheduled and the unscheduled traffic types,
*     then for each type ensures that low delay (HP) is sent
*     before normal (LP) traffic.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_EGR_OFP_SCH_MODE    *sch_mode -
*     Outgoing FAP Port scheduling configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_ofp_sch_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_EGR_OFP_SCH_MODE    *sch_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_scheduling_set
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Set per-port egress scheduling information.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx -
*     Outgoing Fap Port index to configure.
*  SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_INFO    *info -
*     Per-port egress scheduling info
* REMARKS:
*   Some of the scheduling info may be irrelevant -
*   i.e. channelized interface priority is only relevant if the port
*   is mapped to a channelized NIF.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_ofp_scheduling_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                           core,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_INFO    *info
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_scheduling_get
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Set per-port egress scheduling information.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx -
*     Outgoing Fap Port index to configure.
*  SOC_SAND_OUT SOC_PETRA_EGR_OFP_SCH_INFO    *info -
*     Per-port egress scheduling info
* REMARKS:
*   Some of the scheduling info may be irrelevant -
*   i.e. channelized interface priority is only relevant if the port
*   is mapped to a channelized NIF.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_ofp_scheduling_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                           core,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_OFP_SCH_INFO    *info
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_unsched_drop_prio_set
* TYPE:
*   PROC
* FUNCTION:
*     Sets Unscheduled Traffic Drop Priority,
*     based on packet TC and DP.
*     According to the OFP Drop Threshold Type,
*     Unscheduled Packets with different Drop Priorities
*     are dropped based on the appropriate Drop Thresholds
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 tc_ndx -
*     Traffic Class, as embedded in the packet header
*  SOC_SAND_IN  uint32                 dp_ndx -
*     Drop Precedence, as embedded in the packet header
*  SOC_SAND_IN  uint32   drop_prio -
*     Unscheduled Drop Priority. Range: 0 - 3.
* REMARKS:
*     The thresholds per Drop Threshold Type
*     and Drop Priority are configured by the 'soc_petra_egr_sched_drop_set' API,
*     'thresh.queue_words_consumed' field
*     Not valid for Soc_petra-B. Use the soc_pb_egr_q_prio_set API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_unsched_drop_prio_set(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  tc_ndx,
    SOC_SAND_IN  uint32  dp_ndx,
    SOC_SAND_IN  uint32   drop_prio
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_unsched_drop_prio_get
* TYPE:
*   PROC
* FUNCTION:
*     Sets Unscheduled Traffic Drop Priority,
*     based on packet TC and DP.
*     According to the OFP Drop Threshold Type,
*     Unscheduled Packets with different Drop Priorities
*     are dropped based on the appropriate Drop Thresholds
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 tc_ndx -
*     Traffic Class, as embedded in the packet header
*  SOC_SAND_IN  uint32                 dp_ndx -
*     Drop Precedence, as embedded in the packet header
*  SOC_SAND_OUT uint32   *drop_prio -
*     Unscheduled Drop Priority. Range: 0 - 3.
* REMARKS:
*     The thresholds per Drop Threshold Type
*     and Drop Priority are configured by the 'soc_petra_egr_sched_drop_set' API,
*     'thresh.queue_words_consumed' field
*     Not valid for Soc_petra-B. Use the soc_pb_egr_q_prio_set API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_unsched_drop_prio_get(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  tc_ndx,
    SOC_SAND_IN  uint32  dp_ndx,
    SOC_SAND_OUT uint32   *drop_prio
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_q_prio_set
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Sets egress queue priority per traffic class and drop
*     precedence.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx -
*     mapping type, defining what traffic type
*     (unicast/multicast) is mapped to what egress queue type
*     (scheduled/unscheduled).
*  SOC_SAND_IN  uint32                 tc_ndx -
*     Traffic Class, as embedded in the packet header
*  SOC_SAND_IN  uint32                 dp_ndx -
*     Drop Precedence, as embedded in the packet header
*  SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio -
*     Egress Queuing priority (High/Low)
* REMARKS:
*     Not valid for Soc_petra-B. Use the soc_pb_egr_q_prio_set API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_q_prio_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  int                 is_sched_mc,
    SOC_SAND_IN  uint32                 tc_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_q_prio_get
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Sets egress queue priority per traffic class and drop
*     precedence.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx -
*     mapping type, defining what traffic type
*     (unicast/multicast) is mapped to what egress queue type
*     (scheduled/unscheduled).
*  SOC_SAND_IN  uint32                 tc_ndx -
*     Traffic Class, as embedded in the packet header
*  SOC_SAND_IN  uint32                 dp_ndx -
*     Drop Precedence, as embedded in the packet header
*  SOC_SAND_OUT SOC_PETRA_EGR_Q_PRIO          *prio -
*     Egress Queuing priority (High/Low)
* REMARKS:
*     Not valid for Soc_petra-B. Use the soc_pb_egr_q_prio_set API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_q_prio_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  int                 is_sched_mc,
    SOC_SAND_IN  uint32                 tc_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_Q_PRIO          *prio
  );

void
  soc_petra_PETRA_EGR_DROP_THRESH_clear(
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH *info
  );

void
  soc_petra_PETRA_EGR_FC_DEV_THRESH_INNER_clear(
    SOC_SAND_OUT SOC_PETRA_EGR_FC_DEV_THRESH_INNER *info
  );

void
  soc_petra_PETRA_EGR_FC_DEVICE_THRESH_clear(
    SOC_SAND_OUT SOC_PETRA_EGR_FC_DEVICE_THRESH *info
  );

void
  soc_petra_PETRA_EGR_FC_MCI_THRESH_clear(
    SOC_SAND_OUT SOC_PETRA_EGR_FC_MCI_THRESH *info
  );

void
  soc_petra_PETRA_EGR_FC_CHNIF_THRESH_clear(
    SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *info
  );

void
  soc_petra_PETRA_EGR_FC_OFP_THRESH_clear(
    SOC_SAND_OUT SOC_PETRA_EGR_FC_OFP_THRESH *info
  );

void
  soc_petra_PETRA_EGR_OFP_SCH_WFQ_clear(
    SOC_SAND_OUT SOC_PETRA_EGR_OFP_SCH_WFQ *info
  );

void
  soc_petra_PETRA_EGR_OFP_SCH_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_EGR_OFP_SCH_INFO *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1


const char*
  soc_petra_PETRA_EGR_Q_PRIO_to_string(
    SOC_SAND_IN SOC_PETRA_EGR_Q_PRIO enum_val
  );



const char*
  soc_petra_PETRA_EGR_PORT_THRESH_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_EGR_PORT_THRESH_TYPE enum_val
  );



const char*
  soc_petra_PETRA_EGR_MCI_PRIO_to_string(
    SOC_SAND_IN SOC_PETRA_EGR_MCI_PRIO enum_val
  );



const char*
  soc_petra_PETRA_EGR_OFP_INTERFACE_PRIO_to_string(
    SOC_SAND_IN SOC_PETRA_EGR_OFP_INTERFACE_PRIO enum_val
  );



const char*
  soc_petra_PETRA_EGR_OFP_SCH_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_EGR_OFP_SCH_MODE enum_val
  );



const char*
  soc_petra_PETRA_EGR_Q_PRIO_MAPPING_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE enum_val
  );



void
  soc_petra_PETRA_EGR_DROP_THRESH_print(
    SOC_SAND_IN SOC_PETRA_EGR_DROP_THRESH *info
  );



void
  soc_petra_PETRA_EGR_FC_DEV_THRESH_INNER_print(
    SOC_SAND_IN SOC_PETRA_EGR_FC_DEV_THRESH_INNER *info
  );



void
  soc_petra_PETRA_EGR_FC_DEVICE_THRESH_print(
    SOC_SAND_IN SOC_PETRA_EGR_FC_DEVICE_THRESH *info
  );



void
  soc_petra_PETRA_EGR_FC_MCI_THRESH_print(
    SOC_SAND_IN SOC_PETRA_EGR_FC_MCI_THRESH *info
  );



void
  soc_petra_PETRA_EGR_FC_CHNIF_THRESH_print(
    SOC_SAND_IN SOC_PETRA_EGR_FC_CHNIF_THRESH *info
  );



void
  soc_petra_PETRA_EGR_FC_OFP_THRESH_print(
    SOC_SAND_IN SOC_PETRA_EGR_FC_OFP_THRESH *info
  );



void
  soc_petra_PETRA_EGR_OFP_SCH_WFQ_print(
    SOC_SAND_IN SOC_PETRA_EGR_OFP_SCH_WFQ *info
  );



void
  soc_petra_PETRA_EGR_OFP_SCH_INFO_print(
    SOC_SAND_IN SOC_PETRA_EGR_OFP_SCH_INFO *info
  );


#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_API_EGR_QUEUING_INCLUDED__*/
#endif
