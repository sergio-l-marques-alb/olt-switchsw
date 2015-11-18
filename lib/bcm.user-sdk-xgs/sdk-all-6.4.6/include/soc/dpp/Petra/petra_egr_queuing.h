/* $Id: petra_egr_queuing.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/include/soc_petra_egr_queuing.h
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


#ifndef __SOC_PETRA_EGR_QUEUING_INCLUDED__
/* { */
#define __SOC_PETRA_EGR_QUEUING_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_egr_queuing.h>
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

/*********************************************************************
* NAME:
*     soc_petra_egr_queuing_init
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
  soc_petra_egr_queuing_init(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_thresh_type_set_unsafe
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
  soc_petra_egr_ofp_thresh_type_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_thresh_type
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_thresh_type_verify
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
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
  soc_petra_egr_ofp_thresh_type_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_thresh_type
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_thresh_type_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*   See soc_petra_egr_ofp_thresh_type_set_unsafe
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
  soc_petra_egr_ofp_thresh_type_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_PORT_THRESH_TYPE *ofp_thresh_type
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_sched_drop_set_unsafe
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
  soc_petra_egr_sched_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *exact_thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_sched_drop_verify
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
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_sched_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_sched_drop_get_unsafe
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
  soc_petra_egr_sched_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_unsched_drop_set_unsafe
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
  soc_petra_egr_unsched_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *exact_thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_unsched_drop_verify
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
  soc_petra_egr_unsched_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_unsched_drop_get_unsafe
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
  soc_petra_egr_unsched_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_dev_fc_set_unsafe
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
  soc_petra_egr_dev_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_DEVICE_THRESH *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_DEVICE_THRESH *exact_thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_dev_fc_verify
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*     Set Flow Control thresholds for egress queues, based on
*     device-level resources. Threshold are set for overall
*     resources, and scheduled resources.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_EGR_FC_DEVICE_THRESH *thresh -
*     Flow Control thresholds to set - Device-level resources.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_dev_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_DEVICE_THRESH *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_dev_fc_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
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
  soc_petra_egr_dev_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_DEVICE_THRESH *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_xaui_spaui_fc_set_unsafe
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
  soc_petra_egr_xaui_spaui_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *exact_thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_xaui_spaui_fc_verify
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
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_xaui_spaui_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_xaui_spaui_fc_get_unsafe
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
  soc_petra_egr_xaui_spaui_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_fc_set_unsafe
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
  soc_petra_egr_ofp_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_OFP_THRESH   *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_OFP_THRESH   *exact_thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_fc_verify
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
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_ofp_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_OFP_THRESH   *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_fc_get_unsafe
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
  soc_petra_egr_ofp_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_OFP_THRESH   *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_mci_fc_set_unsafe
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
  soc_petra_egr_mci_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_MCI_THRESH   *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_MCI_THRESH   *exact_thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_mci_fc_verify
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
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_mci_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_MCI_THRESH   *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_mci_fc_get_unsafe
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
*  SOC_SAND_OUT SOC_PETRA_EGR_FC_MCI_THRESH   *thresh -
*     Flow Control thresholds to set - Unscheduled resources.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_mci_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_MCI_THRESH   *thresh
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_mci_fc_enable_set_unsafe
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
  soc_petra_egr_mci_fc_enable_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_IN  uint8                 mci_enable,
    SOC_SAND_IN  uint8                 erp_enable
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_mci_fc_enable_verify
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*   See soc_petra_egr_mci_fc_enable_set_unsafe
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
  soc_petra_egr_mci_fc_enable_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_IN  uint8                 mci_enable,
    SOC_SAND_IN  uint8                 erp_enable
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_mci_fc_enable_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*   See soc_petra_egr_mci_fc_enable_set_unsafe
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
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
  soc_petra_egr_mci_fc_enable_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_OUT uint8                 *mci_enable,
    SOC_SAND_OUT uint8                 *erp_enable
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_sch_mode_set_unsafe
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
  soc_petra_egr_ofp_sch_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_MODE    *sch_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_sch_mode_verify
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*   See soc_petra_egr_ofp_sch_mode_set_unsafe
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
  soc_petra_egr_ofp_sch_mode_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_MODE    *sch_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_sch_mode_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*   See soc_petra_egr_ofp_sch_mode_set_unsafe
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
  soc_petra_egr_ofp_sch_mode_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_EGR_OFP_SCH_MODE    *sch_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_scheduling_set_unsafe
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
  soc_petra_egr_ofp_scheduling_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_INFO    *info
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_scheduling_verify
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*   See soc_petra_egr_ofp_scheduling_set_unsafe
* REMARKS:
*   Some of the scheduling info may be irrelevant -
*   i.e. channelized interface priority is only relevant if the port
*   is mapped to a channelized NIF.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_ofp_scheduling_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_INFO    *info
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_ofp_scheduling_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*   See soc_petra_egr_ofp_scheduling_set_unsafe
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
  soc_petra_egr_ofp_scheduling_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_OFP_SCH_INFO    *info
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_unsched_drop_prio_set_unsafe
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
  soc_petra_egr_unsched_drop_prio_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  tc_ndx,
    SOC_SAND_IN  uint32  dp_ndx,
    SOC_SAND_IN  uint32   drop_prio
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_unsched_drop_prio_verify
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
  soc_petra_egr_unsched_drop_prio_verify(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  tc_ndx,
    SOC_SAND_IN  uint32  dp_ndx,
    SOC_SAND_IN  uint32   drop_prio
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_unsched_drop_prio_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Gets Unscheduled Traffic Drop Priority,
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
*  SOC_SAND_OUT  uint32   *drop_prio -
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
  soc_petra_egr_unsched_drop_prio_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  tc_ndx,
    SOC_SAND_IN  uint32  dp_ndx,
    SOC_SAND_OUT uint32   *drop_prio
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_q_prio_set_unsafe
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
  soc_petra_egr_q_prio_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  int                 is_sched_mc,
    SOC_SAND_IN  uint32                 tc_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_q_prio_verify
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*   See soc_petra_egr_q_prio_set_unsafe
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
  soc_petra_egr_q_prio_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  uint32                 tc_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio
  );

/*********************************************************************
* NAME:
*     soc_petra_egr_q_prio_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*   See soc_petra_egr_q_prio_set_unsafe
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
  soc_petra_egr_q_prio_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  int                 is_sched_mc,
    SOC_SAND_IN  uint32                 tc_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_Q_PRIO          *prio
  );


uint32
  soc_petra_egr_thresh_fld_to_mnt_exp(
    SOC_SAND_IN  SOC_PETRA_REG_FIELD     *thresh_fld,
    SOC_SAND_IN  uint32            thresh,
    SOC_SAND_OUT uint32            *exact_thresh,
    SOC_SAND_OUT uint32            *thresh_fld_val
  );

uint32
  soc_petra_egr_mnt_exp_to_thresh_fld(
    SOC_SAND_IN  SOC_PETRA_REG_FIELD  *thresh_fld,
    SOC_SAND_IN  uint32         *thresh_fld_val_in,
    SOC_SAND_OUT uint32         *thresh_out
  );

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_EGR_QUEUING_INCLUDED__*/
#endif
