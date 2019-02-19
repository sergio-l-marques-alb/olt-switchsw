/* $Id: pb_pp_api_frwrd_mact_mgmt.h,v 1.7 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_api_frwrd_mact_mgmt.h
*
* MODULE PREFIX:  soc_pb_pp
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

#ifndef __SOC_PB_PP_API_FRWRD_MACT_MGMT_INCLUDED__
/* { */
#define __SOC_PB_PP_API_FRWRD_MACT_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_frwrd_mact_mgmt.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Number of longs in the message header (DSP header).     */
#define  SOC_PB_PP_FRWRD_MACT_MSG_HDR_SIZE (SOC_PPC_FRWRD_MACT_MSG_HDR_SIZE)

/*     Maximum size of the event buffer in longs               */
#define  SOC_PB_PP_FRWRD_MACT_EVENT_BUFF_MAX_SIZE (SOC_PPC_FRWRD_MACT_EVENT_BUFF_MAX_SIZE)

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

#define SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED
#define SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED
#define SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED  SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED
#define SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED  SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED
#define SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT      SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT
#define SOC_PB_PP_NOF_FRWRD_MACT_LEARNING_MODES                SOC_PPC_NOF_FRWRD_MACT_LEARNING_MODES
typedef SOC_PPC_FRWRD_MACT_LEARNING_MODE                       SOC_PB_PP_FRWRD_MACT_LEARNING_MODE;

#define SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_NONE                  SOC_PPC_FRWRD_MACT_SHADOW_MODE_NONE
#define SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_ARP                   SOC_PPC_FRWRD_MACT_SHADOW_MODE_ARP
#define SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_LAG                   SOC_PPC_FRWRD_MACT_SHADOW_MODE_LAG
#define SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_ALL                   SOC_PPC_FRWRD_MACT_SHADOW_MODE_ALL
#define SOC_PB_PP_NOF_FRWRD_MACT_SHADOW_MODES                  SOC_PPC_NOF_FRWRD_MACT_SHADOW_MODES
typedef SOC_PPC_FRWRD_MACT_SHADOW_MODE                         SOC_PB_PP_FRWRD_MACT_SHADOW_MODE;

#define SOC_PB_PP_FRWRD_MACT_MSG_HDR_TYPE_RAW                        SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_RAW
#define SOC_PB_PP_FRWRD_MACT_MSG_HDR_TYPE_ITMH                       SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ITMH
#define SOC_PB_PP_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_ITMH                 SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_ITMH
#define SOC_PB_PP_NOF_FRWRD_MACT_MSG_HDR_TYPES                       SOC_PPC_NOF_FRWRD_MACT_MSG_HDR_TYPES
typedef SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE                              SOC_PB_PP_FRWRD_MACT_MSG_HDR_TYPE;

#define SOC_PB_PP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_NONE     SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_NONE
#define SOC_PB_PP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_INTERRUPT SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_INTERRUPT
#define SOC_PB_PP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_MSG      SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_MSG
#define SOC_PB_PP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_EVENT    SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_EVENT
#define SOC_PB_PP_NOF_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPES     SOC_PPC_NOF_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPES
typedef SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE            SOC_PB_PP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE;

#define SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_SA_DROP                 SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_DROP
#define SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_SA_UNKNOWN              SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_UNKNOWN
#define SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_DA_UNKNOWN              SOC_PPC_FRWRD_MACT_TRAP_TYPE_DA_UNKNOWN
#define SOC_PB_PP_NOF_FRWRD_MACT_TRAP_TYPES                    SOC_PPC_NOF_FRWRD_MACT_TRAP_TYPES
typedef SOC_PPC_FRWRD_MACT_TRAP_TYPE                           SOC_PB_PP_FRWRD_MACT_TRAP_TYPE;

#define SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_AGED_OUT                     SOC_PPC_FRWRD_MACT_EVENT_TYPE_AGED_OUT
#define SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_LEARN                        SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN
#define SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_TRANSPLANT                   SOC_PPC_FRWRD_MACT_EVENT_TYPE_TRANSPLANT
#define SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_REFRESH                      SOC_PPC_FRWRD_MACT_EVENT_TYPE_REFRESH
#define SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_ACK                          SOC_PPC_FRWRD_MACT_EVENT_TYPE_ACK
#define SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_REQ_FAIL                     SOC_PPC_FRWRD_MACT_EVENT_TYPE_REQ_FAIL
#define SOC_PB_PP_NOF_FRWRD_MACT_EVENT_TYPES                         SOC_PPC_NOF_FRWRD_MACT_EVENT_TYPES
typedef SOC_PPC_FRWRD_MACT_EVENT_TYPE                                SOC_PB_PP_FRWRD_MACT_EVENT_TYPE;

#define SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE_MSG_SINGLE                     SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_MSG_SINGLE
#define SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE_MSG_AGGR                       SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_MSG_AGGR
#define SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS                         SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS
#define SOC_PB_PP_NOF_FRWRD_MACT_EVENT_PATH_TYPES                           SOC_PPC_NOF_FRWRD_MACT_EVENT_PATH_TYPES
typedef SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE                                  SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE;

typedef SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO               SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO;
typedef SOC_PPC_FRWRD_MACT_AGING_INFO                          SOC_PB_PP_FRWRD_MACT_AGING_INFO;
typedef SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY                          SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY;
typedef SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO                         SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO;
typedef SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE                       SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE;
typedef SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                            SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO;
typedef SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO                                 SOC_PB_PP_FRWRD_MACT_EVENT_PROCESSING_INFO;
typedef SOC_PPC_FRWRD_MACT_OPER_MODE_INFO                      SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO;
typedef SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO                      SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO;
typedef SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO             SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO;
typedef SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO                      SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO;
typedef SOC_PPC_FRWRD_MACT_PORT_INFO                           SOC_PB_PP_FRWRD_MACT_PORT_INFO;
typedef SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO                 SOC_PB_PP_FRWRD_MACT_EVENT_LAG_INFO;
typedef SOC_PPC_FRWRD_MACT_EVENT_INFO                     SOC_PB_PP_FRWRD_MACT_EVENT_INFO;
typedef SOC_PPC_FRWRD_MACT_EVENT_BUFFER                        SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER;
typedef SOC_PPC_FRWRD_MACT_LEARN_MSG            SOC_PB_PP_FRWRD_MACT_LEARN_MSG;
typedef SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF       SOC_PB_PP_FRWRD_MACT_LEARN_MSG_CONF;
typedef SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_INFO SOC_PB_PP_FRWRD_MACT_LEARN_MSG_PARSE_INFO;

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
 *   soc_pb_pp_frwrd_mact_oper_mode_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the mode of the MACT, including - ingress vs.
 *   egress learning- how each device responds internally to
 *   events (learn/aged-out/refresh) - which events to inform
 *   other devices.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info -
 *     MACT learning setting
 * REMARKS:
 *   - T20E supports only Egress independent and centralized
 *   learning modes
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_oper_mode_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_oper_mode_info_set" API.
 *     Refer to "soc_pb_pp_frwrd_mact_oper_mode_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_oper_mode_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_aging_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the aging info including enable aging and aging
 *   time.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_INFO               *aging_info -
 *     Whether to perform aging over the MAC entries and time
 *     of aging.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_aging_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_INFO               *aging_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_aging_info_set" API.
 *     Refer to "soc_pb_pp_frwrd_mact_aging_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_aging_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_AGING_INFO               *aging_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_aging_events_handle_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the device action upon events invoked by the aging
 *   process: - Whether the device deletes aged-out entries
 *   internally - Whether the device generates an event for
 *   aged-out entries - Whether the device generates an event
 *   for refreshed entries
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE     *aging_info -
 *     Device actions upon aging-related events.
 * REMARKS:
 *   - use soc_ppd_frwrd_mact_event_handle_info_set() to set how
 *   the OLP should distribute age-out and refresh events.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_aging_events_handle_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_aging_events_handle_info_set" API.
 *     Refer to "soc_pb_pp_frwrd_mact_aging_events_handle_info_set"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_aging_events_handle_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Maps FID-Profile to FID, for shared learning.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  fid_profile_ndx -
 *     FID Profile. Range: 1 - 7. The 0 value is used when FID
 *     = VSI.
 *   SOC_SAND_IN  SOC_PB_PP_FID                                 fid -
 *     Filtering ID. Range: 0 - 16K-1.
 * REMARKS:
 *   - The FID-profile is an attribute of the VSI.- For VSIs
 *   with an FID-profile = 0, FID = VSI.- T20E: This API can
 *   be omitted since for all VSIs, FID = VSI.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_FID                                 fid
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_set" API.
 *     Refer to "soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_set"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_fid_profile_to_fid_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_FID                                 *fid
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_mac_limit_glbl_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable the MAC limit feature, which limits per fid the
 *   maximum number of entries allowed to be in the MAC
 *   Table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info -
 *     Limitation settings, including if this feature is
 *     enabled and how to act when static entry tries to exceed
 *     the limit.
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_mac_limit_glbl_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_mac_limit_glbl_info_set" API.
 *     Refer to "soc_pb_pp_frwrd_mact_mac_limit_glbl_info_set" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_mac_limit_glbl_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_learn_profile_limit_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the limit information including the MAC-limit (i.e.,
 *   the maximum number of entries an FID can hold in the MAC
 *   Table), and the notification action if the configured
 *   limit is exceeded.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  mac_learn_profile_ndx -
 *     MAC-learn-profile ID. Range: 0 - 7.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO          *limit_info -
 *     Limit information including the maximum number of
 *     entries that can be learned/inserted; the action to
 *     perform when an entry is to be inserted/learned while
 *     exceeding the limitation.
 * REMARKS:
 *   - to set 'No limitation' for a specific profile set
 *   limit_info.is_limited = FALSE; - mac_learn_profile_ndx
 *   is set according to soc_ppd_vsi_info_set()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_learn_profile_limit_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO          *limit_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_learn_profile_limit_info_set" API.
 *     Refer to "soc_pb_pp_frwrd_mact_learn_profile_limit_info_set"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_learn_profile_limit_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO          *limit_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_mac_limit_exceeded_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the information if the MAC limitation is exceeded,
 *   i.e. when a MAC Table entry is tryied to be inserted and
 *   exceeds the limitation set per FID. This insertion can
 *   be triggered by CPU or after a packet learning.
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *exceed_info -
 *     Information if the MAC limit was exceed, including the
 *     last FID which caused this limitation violation.
 * REMARKS:
 *   - Soc_petra-B only. Error is return if called for T20E.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_mac_limit_exceeded_info_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *exceed_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_event_handle_profile_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map the mac-learn-profile to the event-handle profile.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  mac_learn_profile_ndx -
 *     MAC-learn-profile ID. Range: 0 - 7.
 *   SOC_SAND_IN  uint32                                  event_handle_profile -
 *     Profile used to set how MACT events are handled. See
 *     soc_ppd_frwrd_mact_event_handle_info_set(). Range: 0 - 1.
 * REMARKS:
 *   - Soc_petra-B only. Error is return if called for T20E. -
 *   mac_learn_profile_ndx is set according to
 *   soc_ppd_vsi_info_set()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_handle_profile_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                                  event_handle_profile
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_event_handle_profile_set" API.
 *     Refer to "soc_pb_pp_frwrd_mact_event_handle_profile_set" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_handle_profile_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_OUT uint32                                  *event_handle_profile
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_event_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the last event parameters for the direct access mode
 *   (i.e., if no OLP messages are sent in case of event)
 *   from the Event FIFO.
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_INFO         *info -
 *     Event information parameters (Key, Payload, Command
 *     type).
 * REMARKS:
 *   - Soc_petra-B only. Error is return if called for T20E. -
 *   This API must be called only if the Learn / Shadow
 *   distribution type is through direct access to the Event
 *   FIFO
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER            *event_buf
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_event_handle_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set how to handle an event according to the event key
 *   parameters (event-type,vsi-handle-profile,is-lag)
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key -
 *     The key that identifies the event
 *     (event-type,vsi-handle-profile,is-lag).
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info -
 *     How to handle the given events, including where (i.e.,
 *     which FIFO) to send these events.
 * REMARKS:
 *   - Soc_petra-B only. Error is return if called for T20E. - To
 *   set the FIFO configuration, use for the System learn
 *   FIFO soc_ppd_frwrd_mact_sys_learn_msgs_distribution_info_set
 *   and for the Shadow FIFO
 *   soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set. - For
 *   the get API, a single event type must be set in the
 *   event-key parameter.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_handle_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_event_handle_info_set" API.
 *     Refer to "soc_pb_pp_frwrd_mact_event_handle_info_set" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_handle_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set how to distribute the learn messages to other
 *   devices/CPU.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info -
 *     Distribution information, in particular the header to
 *     associate with the learn messages.
 * REMARKS:
 *   - Soc_petra-B only. Error is return if called for T20E. -
 *   Soc_petra-B: both learn messages and shadow message must
 *   have the same external header, i.e. either both have an
 *   ITMH Header or none of them. - The get API returns also
 *   the EtherType if not inserted by the user.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_set" API.
 *     Refer to
 *     "soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_set" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_learn_msgs_distribution_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set how to distribute the shadow messages to the other
 *   devices/CPU.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info -
 *     Distribution information, in particular the header to
 *     associate with the shadow messages.
 * REMARKS:
 *   - Soc_petra-B only. Error is return if called for T20E. -
 *   Soc_petra-B: both learn messages and shadow message must
 *   have the same external header, i.e. either both have an
 *   ITMH Header or none of them. - The get API returns also
 *   the EtherType if not inserted by the user.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_set"
 *     API.
 *     Refer to
 *     "soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_set" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_shadow_msgs_distribution_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_port_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set per port MACT management information including which
 *   profile to activate when SA is known in this port.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_PORT_INFO                *port_info -
 *     Port information.
 * REMARKS:
 *   - Use soc_ppd_frwrd_mact_trap_info_set() to set the drop
 *   action for an SA MAC.- Soc_petra-B only, error when called
 *   over T20E device.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_port_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_PORT_INFO                *port_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_port_info_set" API.
 *     Refer to "soc_pb_pp_frwrd_mact_port_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_port_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_PORT_INFO                *port_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_trap_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   For each trap type, set the action profile. Different
 *   actions may be assigned to the same trap type according
 *   to the port-profile (4 possibilities).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAP_TYPE                trap_type_ndx -
 *     Type of the Trap.
 *   SOC_SAND_IN  uint32                                  port_profile_ndx -
 *     Per port profile, to enable the setting of different
 *     actions for the same trap type. To set this profile for
 *     a port, use soc_ppd_frwrd_mact_port_info_set().
 *   SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                      *action_profile -
 *     Trap information including snoop/forwarding action. The
 *     trap_code is necessarily 0.
 * REMARKS:
 *   - For the Trap is
 *   SOC_PPD_FRWRD_MACT_TRAP_TYPE_SAME_INTERFACE port_profile_ndx
 *   has to be 0, since there is port profile is not relevant
 *   for this Trap.- Use soc_ppd_frwrd_mact_port_info_set() to
 *   set the mapping from the port profile to the profile for
 *   action.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_trap_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                      *action_profile
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_trap_info_set" API.
 *     Refer to "soc_pb_pp_frwrd_mact_trap_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_trap_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_ACTION_PROFILE                      *action_profile
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_ip_compatible_mc_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the information for bridging compatible Multicast
 *   MAC addresses.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info -
 *     Compatible Multicast MAC information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_ip_compatible_mc_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  );


/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_learn_msg_parse
 * TYPE:
 *   PROC
 * FUNCTION:
 *   parse learning message given as buffer.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_LEARN_MSG_CONF        *learn_msg_conf -
 *     auxiliary information needed to parse the packet
 *     calculated either by calling soc_pb_pp_frwrd_mact_learn_msg_conf_get
 *     or by user external function
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_LEARN_MSG            *learn_msg -
 *     learning message as bytes buffer
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_LEARN_MSG_PARSE_INFO              *learn_events -
 *     learning events
 * REMARKS:
 *   Soc_petra-B:
 *   - Assuming called on local device
 *   - calling soc_pb_pp_frwrd_mact_learn_msg_parse_unsafe take no device-id parameter
 *     i.e. no access to device nor lock is taken.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_learn_msg_parse(
    SOC_SAND_IN   int                                   unit,
    SOC_SAND_IN   SOC_PB_PP_FRWRD_MACT_LEARN_MSG                  *learn_msg,
    SOC_SAND_IN   SOC_PB_PP_FRWRD_MACT_LEARN_MSG_CONF             *learn_msg_conf,
    SOC_SAND_OUT  SOC_PB_PP_FRWRD_MACT_LEARN_MSG_PARSE_INFO       *learn_events
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_learn_msg_conf_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   'auxliary' function to get size of system headers in learn message
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_LEARN_MSG            *learn_msg -
 *     learning message as bytes buffer
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_LEARN_MSG_CONF        *learn_msg_conf -
 *     auxiliary information needed to parse the packet
 * REMARKS:
 *   Soc_petra-B:
 *   - Assuming called on local device
 *   - generally user can replace this function, by fixed values
 *     so far no change in system headers of learning packet 
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_learn_msg_conf_get(
    SOC_SAND_IN   int                              unit,
    SOC_SAND_IN   SOC_PB_PP_FRWRD_MACT_LEARN_MSG            *learn_msg,
    SOC_SAND_OUT  SOC_PB_PP_FRWRD_MACT_LEARN_MSG_CONF       *learn_msg_conf
  );


/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_mact_ip_compatible_mc_info_set" API.
 *     Refer to "soc_pb_pp_frwrd_mact_ip_compatible_mc_info_set"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_ip_compatible_mc_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_mact_event_parse
 * TYPE:
 *   PROC
 * FUNCTION:
 *   The MACT may report different events using the event
 *   FIFO (e.g., learn, age, transplant, and retrieve). This
 *   API Parses the event buffer into a meaningful structure.
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER            *event_buf -
 *     Buffer includes MACT event
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_INFO              *mact_event -
 *     MACT Event parsed into structure
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_mact_event_parse(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER            *event_buf,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_INFO              *mact_event
  );

void
  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_AGING_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_AGING_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY *info
  );

void
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE *info
  );

void
  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_EVENT_PROCESSING_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_PROCESSING_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_PORT_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_EVENT_LAG_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_LAG_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_EVENT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER *info
  );

void
  SOC_PB_PP_FRWRD_MACT_LEARN_MSG_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_LEARN_MSG *info
  );

void
  SOC_PB_PP_FRWRD_MACT_LEARN_MSG_CONF_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_LEARN_MSG_CONF *info
  );

void
  SOC_PB_PP_FRWRD_MACT_LEARN_MSG_PARSE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_MACT_LEARN_MSG_PARSE_INFO *info
  );


#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_FRWRD_MACT_LEARNING_MODE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_LEARNING_MODE enum_val
  );

const char*
  SOC_PB_PP_FRWRD_MACT_SHADOW_MODE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_SHADOW_MODE enum_val
  );

const char*
  SOC_PB_PP_FRWRD_MACT_MSG_HDR_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_HDR_TYPE enum_val
  );

const char*
  SOC_PB_PP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE enum_val
  );

const char*
  SOC_PB_PP_FRWRD_MACT_TRAP_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_TRAP_TYPE enum_val
  );

const char*
  SOC_PB_PP_FRWRD_MACT_EVENT_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_TYPE enum_val
  );

const char*
  SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_PATH_TYPE enum_val
  );

void
  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_AGING_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_KEY *info
  );

void
  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_HANDLE_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_AGING_EVENTS_HANDLE *info
  );

void
  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MSG_DISTR_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_EVENT_PROCESSING_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_PROCESSING_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_OPER_MODE_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_PORT_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_PORT_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_EVENT_LAG_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_LAG_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_EVENT_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_INFO *info
  );

void
  SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_MACT_EVENT_BUFFER *info
  );


void
  SOC_PB_PP_FRWRD_MACT_LEARN_MSG_print(
    SOC_SAND_IN SOC_PB_PP_FRWRD_MACT_LEARN_MSG *info
  );

void
  SOC_PB_PP_FRWRD_MACT_LEARN_MSG_CONF_print(
    SOC_SAND_IN SOC_PB_PP_FRWRD_MACT_LEARN_MSG_CONF *info
  );

void
  SOC_PB_PP_FRWRD_MACT_LEARN_MSG_PARSE_INFO_print(
    SOC_SAND_IN SOC_PB_PP_FRWRD_MACT_LEARN_MSG_PARSE_INFO *info
  );

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_API_FRWRD_MACT_MGMT_INCLUDED__*/
#endif

