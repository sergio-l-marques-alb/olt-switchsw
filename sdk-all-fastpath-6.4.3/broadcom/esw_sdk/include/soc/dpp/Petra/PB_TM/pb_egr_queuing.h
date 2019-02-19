/* $Id: soc_pb_egr_queuing.h,v 1.6 Broadcom SDK $
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


#ifndef __SOC_PB_EGR_QUEUING_INCLUDED__
/* { */
#define __SOC_PB_EGR_QUEUING_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/Petra/PB_TM/pb_api_nif.h>
#include <soc/dpp/Petra/PB_TM/pb_api_egr_queuing.h>
#include <soc/dpp/Petra/petra_api_egr_queuing.h>

#include <soc/dpp/Petra/PB_TM/pb_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: soc_pb_egr_queuing.h,v 1.6 Broadcom SDK $
 *	FQP NIF Port MUX table predefined values
 */
/* Give BW to other interfaces only if they need it */
#define SOC_PB_EGQ_NIF_PORT_CAL_BW_GIVE_UPON_REQUEST  125
/* Give BW to other interfaces always */
/* Indicates invalid entry */
#define SOC_PB_EGQ_NIF_PORT_CAL_BW_INVALID            127

#define SOC_PB_EGQ_NIF_OTHERS_DEF_VAL                     20
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
*     soc_pb_egr_q_nif_cal_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Jul 22 2010
* FUNCTION:
*         Set the calender of the nif port "processing time" i.e every 2 clk there
*         is a context switch and different nif get "service" or others which include
*         delete and recycling.
* INPUT:
*         SOC_SAND_IN int unit
*
* REMARKS:
*         This function consist of four steps:
*         1. Classifying the nif, mal configuration
*         2. Giving each nif  weight according to its type
*         3. Adding the "others" processes of recycling, deleting as competitor with a given value
*         4. Computing the optimal calender using "black box" soc_pb_ofp_rates_from_rates_to_calendar function
*         5. Using the values computed in 4. to initialize SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL address: 0x004a0000
*
*         The calender should admit the constraint derived from the formula BW=(128 * 8)/(ClkFreq * # Clk )
*         => # Clk = (128 * 8)/(ClkFreq * BW) Given that the clock frequency is 3.3 Ns
*         WE get #Clk= (128 * 8) /(3.3 * BW)
*
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
 uint32
   soc_pb_egr_q_nif_cal_set_unsafe(
     SOC_SAND_IN int unit
     );

uint32
  soc_pb_egr_q_nif_cal_set_verify(
    SOC_SAND_IN  int                     unit
  );

uint32
  soc_pb_egr_q_nif_cal_get_verify(
    SOC_SAND_IN  int                     unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_egr_q_nif_cal_set_unsafe" API.
 *     Refer to "soc_pb_egr_q_nif_cal_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_egr_q_nif_cal_get_unsafe(
    SOC_SAND_IN  int                     unit
  );
     

/*********************************************************************
* NAME:
*     soc_pb_egr_queuing_init
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
  soc_pb_egr_queuing_init(
    SOC_SAND_IN  int                 unit
  );


/*********************************************************************
* NAME:
*     soc_pb_egr_unsched_drop_set_unsafe
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
*     Queue priority index. Range: SOC_PB_EGR_Q_PRIO_LOW -
*     SOC_PB_EGR_Q_PRIO_HIGH.
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
*     by the 'soc_pb_egr_unsched_drop_prio_set' API, per TC and DP
*     The "dp_ndx" in this API refers to this Drop Priority, and not
*     directly to the packet Drop Precedence field
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_unsched_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *exact_thresh
  );

/*********************************************************************
* NAME:
*     soc_pb_egr_unsched_drop_get_unsafe
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
*     Queue priority index. Range: SOC_PB_EGR_Q_PRIO_LOW -
*     SOC_PB_EGR_Q_PRIO_HIGH.
*  SOC_SAND_IN  uint32                 dp_ndx -
*     Drop precedence index. Range: 0 - SOC_PETRA_NOF_DP-1.
*  SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *thresh -
*     drop thresholds to set.
* REMARKS:
*     The unscheduled traffic is assigned to the matching threshold
*     not according to the packet Drop Precedence, rather then
*     according to the Unscheduled Drop Priority value, as set
*     by the 'soc_pb_egr_unsched_drop_prio_set' API, per TC and DP
*     The "dp_ndx" in this API refers to this Drop Priority, and not
*     directly to the packet Drop Precedence field
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_unsched_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *thresh
  );

/*********************************************************************
* NAME:
 *   soc_pb_egr_q_prio_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets egress queue priority per traffic class and drop
 *   precedence.
 * INPUT:
 *   SOC_SAND_IN  int                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx -
 *     mapping type, defining what traffic type
 *     (unicast/multicast) is mapped to what egress queue type
 *     (scheduled/unscheduled).
 *   SOC_SAND_IN  uint32                     tc_ndx -
 *     Traffic Class, as embedded in the packet header. Range:
 *     0 - 7.
 *   SOC_SAND_IN  uint32                     dp_ndx -
 *     Drop Precedence, as embedded in the packet header.
 *     Range: 0 - 3.
 *   SOC_SAND_IN  uint32                     map_profile_ndx -
 *     Mapping profile index. Range: 0 - 3.
 *   SOC_SAND_IN  SOC_PB_EGR_Q_PRIORITY             *priority -
 *     Egress Queuing priority (Drop precedence and Traffic
 *     class).
 * REMARKS:
 *   Each OFP is mapped to an egress queue profile by the
 *   soc_pb_egr_q_profile_map_set API.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_q_prio_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  uint32                     tc_ndx,
    SOC_SAND_IN  uint32                     dp_ndx,
    SOC_SAND_IN  uint32                     map_profile_ndx,
    SOC_SAND_IN  SOC_PB_EGR_Q_PRIORITY             *priority
  );

uint32
  soc_pb_egr_q_prio_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  uint32                     tc_ndx,
    SOC_SAND_IN  uint32                     dp_ndx,
    SOC_SAND_IN  uint32                     map_profile_ndx,
    SOC_SAND_IN  SOC_PB_EGR_Q_PRIORITY             *priority
  );

uint32
  soc_pb_egr_q_prio_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  uint32                     tc_ndx,
    SOC_SAND_IN  uint32                     dp_ndx,
    SOC_SAND_IN  uint32                     map_profile_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_egr_q_prio_set_unsafe" API.
 *     Refer to "soc_pb_egr_q_prio_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_egr_q_prio_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  uint32                     tc_ndx,
    SOC_SAND_IN  uint32                     dp_ndx,
    SOC_SAND_IN  uint32                     map_profile_ndx,
    SOC_SAND_OUT SOC_PB_EGR_Q_PRIORITY             *priority
  );

/*********************************************************************
* NAME:
 *   soc_pb_egr_q_profile_map_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Function description
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID              ofp_ndx -
 *     Outgoing Fap Port index. Range: 0 - 79.
 *   SOC_SAND_IN  uint32                      map_profile_id -
 *     Egress queue priority profile index. Range: 0 - 3.
 * REMARKS:
 *   The profile configuration is set by the
 *   soc_pb_egr_q_prio_set API.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_egr_q_profile_map_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID              ofp_ndx,
    SOC_SAND_IN  uint32                      map_profile_id
  );

uint32
  soc_pb_egr_q_profile_map_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID              ofp_ndx,
    SOC_SAND_IN  uint32                      map_profile_id
  );

uint32
  soc_pb_egr_q_profile_map_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID              ofp_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_egr_q_profile_map_set_unsafe" API.
 *     Refer to "soc_pb_egr_q_profile_map_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_egr_q_profile_map_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID              ofp_ndx,
    SOC_SAND_OUT uint32                      *map_profile_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_egr_queuing_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_api_egr_queuing module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_egr_queuing_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_egr_queuing_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_api_egr_queuing module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_egr_queuing_get_errs_ptr(void);

uint32
  SOC_PB_EGR_Q_PRIORITY_verify(
    SOC_SAND_IN  SOC_PB_EGR_Q_PRIORITY *info
  );

/*********************************************************************
* NAME:
*     soc_pb_egr_xaui_spaui_fc_get_unsafe
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
  soc_pb_egr_xaui_spaui_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh
  );

/*********************************************************************
* NAME:
*     soc_pb_egr_xaui_spaui_fc_set_unsafe
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
  soc_pb_egr_xaui_spaui_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *exact_thresh
  );

uint32
  soc_pb_egr_xaui_spaui_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_EGR_QUEUING_INCLUDED__*/
#endif

