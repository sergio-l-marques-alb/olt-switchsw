/* $Id: ppd_api_frwrd_mact_mgmt.h,v 1.16 Broadcom SDK $
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
 * $
*/
/******************************************************************
*
* FILENAME:       DuneDriver/ppd/include/soc_ppd_api_frwrd_mact_mgmt.h
*
* MODULE PREFIX:  soc_ppd_frwrd
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

#ifndef __SOC_PPD_API_FRWRD_MACT_MGMT_INCLUDED__
/* { */
#define __SOC_PPD_API_FRWRD_MACT_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>

#include <soc/dpp/PPC/ppc_api_frwrd_mact_mgmt.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Number of longs in the message header (DSP header).     */
#define  SOC_PPD_FRWRD_MACT_MSG_HDR_SIZE (SOC_PPC_FRWRD_MACT_MSG_HDR_SIZE)

/*     Maximum size of the event buffer in longs               */
#define  SOC_PPD_FRWRD_MACT_EVENT_BUFF_MAX_SIZE (SOC_PPC_FRWRD_MACT_EVENT_BUFF_MAX_SIZE)


/* use to indicate no limit on numnber MACT entries see glbl_limit    */
#define  SOC_PPD_FRWRD_MACT_NO_GLOBAL_LIMIT (SOC_PPC_FRWRD_MACT_NO_GLOBAL_LIMIT)

/* Max number of LIF ranges that can be mapped for LIF Learn Limit enforcing */

/* Max number of LIF ranges that are mapped for LIF Learn Limit enforcing */
#define  SOC_PPD_MAX_NOF_MACT_LIMIT_MAPPED_LIF_RANGES   (SOC_PPC_MAX_NOF_MACT_LIMIT_MAPPED_LIF_RANGES)

/*  L2 MACT Limit Mode */
#define  SOC_PPD_FRWRD_MACT_LEARN_LIMIT_MODE            (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE)

/* Identify whether is VMAC limit or MAC limit by bit 15 */
#define  SOC_PPD_FRWRD_MACT_LEARN_VMAC_LIMIT            (SOC_PPC_FRWRD_MACT_LEARN_VMAC_LIMIT)

/* Max number of VMAC limit */
#define  SOC_PPD_FRWRD_MACT_LEARN_MAX_VMAC_LIMIT        (SOC_PPC_FRWRD_MACT_LEARN_MAX_VMAC_LIMIT)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/*  L2 MACT Learn Limit LIF Range number (SOC_PPD_MAX_NOF_MACT_LIMIT_MAPPED_LIF_RANGES) for one of the mapped ranges */
#define  SOC_PPD_FRWRD_MACT_LEARN_LIF_RANGE_BASE(range_num)     (SOC_PPC_FRWRD_MACT_LEARN_LIF_RANGE_BASE(range_num))

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */
typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_SET = SOC_PPD_PROC_DESC_BASE_FRWRD_MACT_MGMT_FIRST,
  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_GET,
  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_AGING_INFO_SET,
  SOC_PPD_FRWRD_MACT_AGING_ONE_PASS_SET,
  SOC_PPD_FRWRD_MACT_AGING_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_AGING_INFO_GET,
  SOC_PPD_FRWRD_MACT_AGING_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET,
  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET,
  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET,
  SOC_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_PRINT,
  SOC_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET,
  SOC_PPD_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_PRINT,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_PER_TUNNEL_INFO_SET,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_PER_TUNNEL_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_PER_TUNNEL_INFO_GET,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_PER_TUNNEL_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET,
  SOC_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET,
  SOC_PPD_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET,
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET,
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_PRINT,
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET,
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_PRINT,
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET,
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_GET,
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET,
  SOC_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET,
  SOC_PPD_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET,
  SOC_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET,
  SOC_PPD_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_PORT_INFO_SET,
  SOC_PPD_FRWRD_MACT_PORT_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_PORT_INFO_GET,
  SOC_PPD_FRWRD_MACT_PORT_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_TRAP_INFO_SET,
  SOC_PPD_FRWRD_MACT_TRAP_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_TRAP_INFO_GET,
  SOC_PPD_FRWRD_MACT_TRAP_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET,
  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_PRINT,
  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET,
  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_PRINT,
  SOC_PPD_FRWRD_MACT_EVENT_GET,
  SOC_PPD_FRWRD_MACT_EVENT_GET_PRINT,
  SOC_PPD_FRWRD_MACT_EVENT_PARSE,
  SOC_PPD_FRWRD_MACT_EVENT_PARSE_PRINT,
  SOC_PPD_FRWRD_MACT_MGMT_GET_PROCS_PTR,
  SOC_PPD_FRWRD_MACT_MIM_INIT_SET,
  SOC_PPD_FRWRD_MACT_MIM_INIT_GET,
  SOC_PPD_FRWRD_MACT_ROUTED_LEARNING_SET,
  SOC_PPD_FRWRD_MACT_ROUTED_LEARNING_GET,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE,
  SOC_PPD_FRWRD_MACT_LOOKUP_TYPE_SET,

  /*
   * Last element. Do no touch.
   */
  SOC_PPD_FRWRD_MACT_MGMT_PROCEDURE_DESC_LAST
} SOC_PPD_FRWRD_MACT_MGMT_PROCEDURE_DESC;

#define SOC_PPD_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED   SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED
#define SOC_PPD_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED   SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED
#define SOC_PPD_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED    SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED
#define SOC_PPD_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED    SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED
#define SOC_PPD_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT        SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT
typedef SOC_PPC_FRWRD_MACT_LEARNING_MODE                       SOC_PPD_FRWRD_MACT_LEARNING_MODE;

#define SOC_PPD_FRWRD_MACT_SHADOW_MODE_NONE                    SOC_PPC_FRWRD_MACT_SHADOW_MODE_NONE
#define SOC_PPD_FRWRD_MACT_SHADOW_MODE_ARP                     SOC_PPC_FRWRD_MACT_SHADOW_MODE_ARP
#define SOC_PPD_FRWRD_MACT_SHADOW_MODE_LAG                     SOC_PPC_FRWRD_MACT_SHADOW_MODE_LAG
#define SOC_PPD_FRWRD_MACT_SHADOW_MODE_ALL                     SOC_PPC_FRWRD_MACT_SHADOW_MODE_ALL
#define SOC_PPD_NOF_FRWRD_MACT_SHADOW_MODES                    SOC_PPC_NOF_FRWRD_MACT_SHADOW_MODES
typedef SOC_PPC_FRWRD_MACT_SHADOW_MODE                         SOC_PPD_FRWRD_MACT_SHADOW_MODE;

#define SOC_PPD_FRWRD_MACT_MSG_HDR_TYPE_RAW                          SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_RAW
#define SOC_PPD_FRWRD_MACT_MSG_HDR_TYPE_ITMH                         SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ITMH
#define SOC_PPD_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_ITMH                   SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_ITMH
#define SOC_PPD_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_RAW                    SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_RAW
#define SOC_PPD_FRWRD_MACT_MSG_HDR_TYPE_OTMH                         SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_OTMH
#define SOC_PPD_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_OTMH                   SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_OTMH
typedef SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE                              SOC_PPD_FRWRD_MACT_MSG_HDR_TYPE;

#define SOC_PPD_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_NONE       SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_NONE
#define SOC_PPD_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_INTERRUPT  SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_INTERRUPT
#define SOC_PPD_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_MSG        SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_MSG
#define SOC_PPD_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_EVENT      SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_EVENT
typedef SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE            SOC_PPD_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE;

#define SOC_PPD_FRWRD_MACT_TRAP_TYPE_SA_DROP                   SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_DROP
#define SOC_PPD_FRWRD_MACT_TRAP_TYPE_SA_UNKNOWN                SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_UNKNOWN
#define SOC_PPD_FRWRD_MACT_TRAP_TYPE_DA_UNKNOWN                SOC_PPC_FRWRD_MACT_TRAP_TYPE_DA_UNKNOWN
typedef SOC_PPC_FRWRD_MACT_TRAP_TYPE                           SOC_PPD_FRWRD_MACT_TRAP_TYPE;

#define SOC_PPD_FRWRD_MACT_EVENT_TYPE_NONE                     SOC_PPC_FRWRD_MACT_EVENT_TYPE_NONE
#define SOC_PPD_FRWRD_MACT_EVENT_TYPE_AGED_OUT                       SOC_PPC_FRWRD_MACT_EVENT_TYPE_AGED_OUT
#define SOC_PPD_FRWRD_MACT_EVENT_TYPE_LEARN                          SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN
#define SOC_PPD_FRWRD_MACT_EVENT_TYPE_TRANSPLANT                     SOC_PPC_FRWRD_MACT_EVENT_TYPE_TRANSPLANT
#define SOC_PPD_FRWRD_MACT_EVENT_TYPE_REFRESH                        SOC_PPC_FRWRD_MACT_EVENT_TYPE_REFRESH
#define SOC_PPD_FRWRD_MACT_EVENT_TYPE_RETRIEVE                 SOC_PPC_FRWRD_MACT_EVENT_TYPE_RETRIEVE
#define SOC_PPD_FRWRD_MACT_EVENT_TYPE_LIMIT_EXCEED             SOC_PPC_FRWRD_MACT_EVENT_TYPE_LIMIT_EXCEED
#define SOC_PPD_FRWRD_MACT_EVENT_TYPE_ACK                            SOC_PPC_FRWRD_MACT_EVENT_TYPE_ACK
#define SOC_PPD_FRWRD_MACT_EVENT_TYPE_REQ_FAIL                       SOC_PPC_FRWRD_MACT_EVENT_TYPE_REQ_FAIL
typedef SOC_PPC_FRWRD_MACT_EVENT_TYPE                                SOC_PPD_FRWRD_MACT_EVENT_TYPE;

#define SOC_PPD_FRWRD_MACT_EVENT_PATH_TYPE_MSG_SINGLE                       SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_MSG_SINGLE
#define SOC_PPD_FRWRD_MACT_EVENT_PATH_TYPE_MSG_AGGR                         SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_MSG_AGGR
#define SOC_PPD_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS       SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS
typedef SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE                                  SOC_PPD_FRWRD_MACT_EVENT_PATH_TYPE;

typedef SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO               SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO;
typedef SOC_PPC_FRWRD_MACT_AGING_INFO                          SOC_PPD_FRWRD_MACT_AGING_INFO;
typedef SOC_PPC_FRWRD_MACT_AGING_ONE_PASS_INFO                          SOC_PPD_FRWRD_MACT_AGING_ONE_PASS_INFO;
typedef SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY                          SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY;
typedef SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO                         SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO;
typedef SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE                       SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE;
typedef SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                            SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO;
typedef SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO                                 SOC_PPD_FRWRD_MACT_EVENT_PROCESSING_INFO;
typedef SOC_PPC_FRWRD_MACT_OPER_MODE_INFO                      SOC_PPD_FRWRD_MACT_OPER_MODE_INFO;
typedef SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO                      SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO;
typedef SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO             SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO;
typedef SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO                      SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO;
typedef SOC_PPC_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO             SOC_PPD_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO;
typedef SOC_PPC_FRWRD_MACT_MAC_LIMIT_MAPPING_INFO               SOC_PPD_FRWRD_MACT_MAC_LIMIT_MAPPING_INFO;
typedef SOC_PPC_FRWRD_MACT_PORT_INFO                           SOC_PPD_FRWRD_MACT_PORT_INFO;
typedef SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO                 SOC_PPD_FRWRD_MACT_EVENT_LAG_INFO;
typedef SOC_PPC_FRWRD_MACT_EVENT_INFO                     SOC_PPD_FRWRD_MACT_EVENT_INFO;
typedef SOC_PPC_FRWRD_MACT_EVENT_BUFFER                        SOC_PPD_FRWRD_MACT_EVENT_BUFFER;
typedef SOC_PPC_FRWRD_MACT_LEARN_MSG            SOC_PPD_FRWRD_MACT_LEARN_MSG;
typedef SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF       SOC_PPD_FRWRD_MACT_LEARN_MSG_CONF;
typedef SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_INFO SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE_INFO;


#define SOC_PPD_FRWRD_MACT_LEARN_MSG_TYPE_INDEX   SOC_PPC_FRWRD_MACT_LEARN_MSG_TYPE_INDEX
#define SOC_PPD_FRWRD_MACT_LEARN_MSG_TYPE_LEN     SOC_PPC_FRWRD_MACT_LEARN_MSG_TYPE_LEN
#define SOC_PPD_FRWRD_MACT_LEARN_MSG_TYPE_INDEX   SOC_PPC_FRWRD_MACT_LEARN_MSG_TYPE_INDEX
#define SOC_PPD_FRWRD_MACT_LEARN_MSG_TYPE_LEN     SOC_PPC_FRWRD_MACT_LEARN_MSG_TYPE_LEN


typedef SOC_PPC_FRWRD_MACT_LOOKUP_TYPE    SOC_PPD_FRWRD_MACT_LOOKUP_TYPE;

#define SOC_PPD_FRWRD_MACT_LEARN_LIMIT_TYPE_VSI     SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_VSI
#define SOC_PPD_FRWRD_MACT_LEARN_LIMIT_TYPE_LIF     SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_LIF
#define SOC_PPD_FRWRD_MACT_LEARN_LIMIT_TYPE_TUNNEL  SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_TUNNEL
typedef SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE         SOC_PPD_FRWRD_MACT_LEARN_LIMIT_TYPE;

#define SOC_PPD_FRWRD_MACT_L3_LEARN_IPV4_UC     SOC_PPC_FRWRD_MACT_L3_LEARN_IPV4_UC
#define SOC_PPD_FRWRD_MACT_L3_LEARN_IPV4_MC     SOC_PPC_FRWRD_MACT_L3_LEARN_IPV4_MC
#define SOC_PPD_FRWRD_MACT_L3_LEARN_IPV6_UC     SOC_PPC_FRWRD_MACT_L3_LEARN_IPV6_UC
#define SOC_PPD_FRWRD_MACT_L3_LEARN_IPV6_MC     SOC_PPC_FRWRD_MACT_L3_LEARN_IPV6_MC
#define SOC_PPD_FRWRD_MACT_L3_LEARN_MPLS        SOC_PPC_FRWRD_MACT_L3_LEARN_MPLS

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
 *   Configures the SA lookup type. Also configured 'opportunistic learning',
 *   which is affected by whether SA authentication is enabled.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_lookup_type_set(      
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_LOOKUP_TYPE         lookup_type
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_oper_mode_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the mode of the MACT, including - ingress vs.
 *   egress learning- how each device responds internally to
 *   events (learn/aged-out/refresh) - which events to inform
 *   other devices.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info -
 *     MACT learning setting
 * REMARKS:
 *   - T20E supports only Egress independent and centralized
 *   learning modes
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_oper_mode_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_mact_oper_mode_info_set" API.
 *     Refer to "soc_ppd_frwrd_mact_oper_mode_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_oper_mode_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  );


/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_fid_aging_profile_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map the mac-learn-profile to the fid_aging profile.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                           mac_learn_profile_ndx -
 *     MAC-learn-profile ID. Range: 0 - 3.
 *   SOC_SAND_IN  uint32                           fid_aging_profile -
 *     Profile used to set how MACT events are handled. 
 * REMARKS:
 *   - Soc_petra-B only. Error is return if called for T20E. -
 *   mac_learn_profile_ndx is set according to
 *   soc_ppd_vsi_info_set()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_fid_aging_profile_set(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       mac_learn_profile_ndx,
    SOC_SAND_IN  uint32       fid_aging_profile
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_mact_fid_aging_profile_set" API.
 *     Refer to "soc_ppd_frwrd_mact_fid_aging_profile_set" API
 *     for details.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_fid_aging_profile_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       mac_learn_profile_ndx,
    SOC_SAND_OUT uint32      *fid_aging_profile
  );

/*********************************************************************
*     Set the configuration of 'fid_aging_profile'
*     with 'fid_ading_cycles
*********************************************************************/

uint32
  soc_ppd_frwrd_mact_aging_profile_config(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       fid_aging_profile,
    SOC_SAND_IN uint32        fid_aging_cycles
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_aging_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the aging info including enable aging and aging
 *   time.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_INFO               *aging_info -
 *     Whether to perform aging over the MAC entries and time
 *     of aging.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_aging_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_INFO               *aging_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_mact_aging_info_set" API.
 *     Refer to "soc_ppd_frwrd_mact_aging_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_aging_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_AGING_INFO               *aging_info
  );


/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_aging_one_pass_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   perform one aging iteration
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_ONE_PASS_INFO               *pass_info -
 *     Whether to perform aging over the MAC entries and time
 *     of aging.
 *   SOC_SAND_IN  SOC_SAND_SUCCESS_FAILURE               *success -
 *     success or fail
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_aging_one_pass_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_ONE_PASS_INFO   *pass_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE             *success
  );


/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_aging_events_handle_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the device action upon events invoked by the aging
 *   process: - Whether the device deletes aged-out entries
 *   internally - Whether the device generates an event for
 *   aged-out entries - Whether the device generates an event
 *   for refreshed entries
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info -
 *     Device actions upon aging-related events.
 * REMARKS:
 *   - use soc_ppd_frwrd_mact_event_handle_info_set() to set how
 *   the OLP should distribute age-out and refresh events.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_aging_events_handle_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_mact_aging_events_handle_info_set" API.
 *     Refer to "soc_ppd_frwrd_mact_aging_events_handle_info_set"
 *     API for details.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_aging_events_handle_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_fid_profile_to_fid_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Maps FID-Profile to FID, for shared learning.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                fid_profile_ndx -
 *     FID Profile. Range: 1 - 7. The 0 value is used when FID
 *     = VSI.
 *   SOC_SAND_IN  SOC_PPD_FID                                 fid -
 *     Filtering ID. Range: 0 - 16K-1.
 * REMARKS:
 *   - The FID-profile is an attribute of the VSI.- For VSIs
 *   with an FID-profile = 0, FID = VSI.- T20E: This API can
 *   be omitted since for all VSIs, FID = VSI.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_fid_profile_to_fid_map_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                fid_profile_ndx,
    SOC_SAND_IN  SOC_PPD_FID                                 fid
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_mact_fid_profile_to_fid_map_set" API.
 *     Refer to "soc_ppd_frwrd_mact_fid_profile_to_fid_map_set" API
 *     for details.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_fid_profile_to_fid_map_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                fid_profile_ndx,
    SOC_SAND_OUT SOC_PPD_FID                                 *fid
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_mac_limit_glbl_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable the MAC limit feature, which limits per fid the
 *   maximum number of entries allowed to be in the MAC
 *   Table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info -
 *     Limitation settings, including if this feature is
 *     enabled and how to act when static entry tries to exceed
 *     the limit.
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_mac_limit_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_mact_mac_limit_glbl_info_set" API.
 *     Refer to "soc_ppd_frwrd_mact_mac_limit_glbl_info_set" API
 *     for details.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_mac_limit_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_learn_profile_limit_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the limit information including the MAC-limit (i.e.,
 *   the maximum number of entries an FID can hold in the MAC
 *   Table), and the notification action if the configured
 *   limit is exceeded.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               mac_learn_profile_ndx -
 *     MAC-learn-profile ID. Range: 0 - 7.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO          *limit_info -
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
  soc_ppd_frwrd_mact_learn_profile_limit_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                mac_learn_profile_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO          *limit_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_mact_learn_profile_limit_info_set" API.
 *     Refer to "soc_ppd_frwrd_mact_learn_profile_limit_info_set"
 *     API for details.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_learn_profile_limit_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                mac_learn_profile_ndx,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO          *limit_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_mac_limit_exceeded_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the information if the MAC limitation is exceeded,
 *   i.e. when a MAC Table entry is tryied to be inserted and
 *   exceeds the limitation set per FID. This insertion can
 *   be triggered by CPU or after a packet learning.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT  SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO  *exceed_info -
 *     Information if the MAC limit was exceed, including the
 *     last FID which caused this limitation violation.
 * REMARKS:
 *   - Soc_petra-B only. Error is return if called for T20E.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_mac_limit_exceeded_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT  SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO  *exceed_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_event_handle_profile_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map the mac-learn-profile to the event-handle profile.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               mac_learn_profile_ndx -
 *     MAC-learn-profile ID. Range: 0 - 7.
 *   SOC_SAND_IN  uint32                               event_handle_profile -
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
  soc_ppd_frwrd_mact_event_handle_profile_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                               mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                               event_handle_profile
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_mact_event_handle_profile_set" API.
 *     Refer to "soc_ppd_frwrd_mact_event_handle_profile_set" API
 *     for details.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_event_handle_profile_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                               mac_learn_profile_ndx,
    SOC_SAND_OUT uint32                               *event_handle_profile
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_event_handle_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set how to handle an event according to the event key
 *   parameters (event-type,vsi-handle-profile,is-lag)
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY        *event_key -
 *     The key that identifies the event
 *     (event-type,vsi-handle-profile,is-lag).
 *   SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO       *handle_info -
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
  soc_ppd_frwrd_mact_event_handle_info_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY        *event_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO       *handle_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_mact_event_handle_info_set" API.
 *     Refer to "soc_ppd_frwrd_mact_event_handle_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_event_handle_info_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY        *event_key,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO       *handle_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_learn_msgs_distribution_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set how to distribute the learn messages to other
 *   devices/CPU.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO          *distribution_info -
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
  soc_ppd_frwrd_mact_learn_msgs_distribution_info_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO          *distribution_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_mact_learn_msgs_distribution_info_set" API.
 *     Refer to
 *     "soc_ppd_frwrd_mact_learn_msgs_distribution_info_set" API
 *     for details.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_learn_msgs_distribution_info_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO          *distribution_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set how to distribute the shadow messages to the other
 *   devices/CPU.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO          *distribution_info -
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
  soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO          *distribution_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set" API.
 *     Refer to
 *     "soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set" API
 *     for details.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO          *distribution_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_port_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set per port MACT management information including which
 *   profile to activate when SA is known in this port.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_PORT                               local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_MACT_PORT_INFO               *port_info -
 *     Port information.
 * REMARKS:
 *   - Use soc_ppd_frwrd_mact_trap_info_set() to set the drop
 *   action for an SA MAC.- Soc_petra-B only, error when called
 *   over T20E device.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_port_info_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  SOC_PPD_PORT                               local_port_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_PORT_INFO               *port_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_mact_port_info_set" API.
 *     Refer to "soc_ppd_frwrd_mact_port_info_set" API for details.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_port_info_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  SOC_PPD_PORT                               local_port_ndx,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_PORT_INFO               *port_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_trap_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   For each trap type, set the action profile. Different
 *   actions may be assigned to the same trap type according
 *   to the port-profile (4 possibilities).
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAP_TYPE               trap_type_ndx -
 *     Type of the Trap.
 *   SOC_SAND_IN  uint32                               port_profile_ndx -
 *     Per port profile, to enable the setting of different
 *     actions for the same trap type. To set this profile for
 *     a port, use soc_ppd_frwrd_mact_port_info_set().
 *   SOC_SAND_IN  SOC_PPD_ACTION_PROFILE                     *action_profile -
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
  soc_ppd_frwrd_mact_trap_info_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAP_TYPE               trap_type_ndx,
    SOC_SAND_IN  uint32                               port_profile_ndx,
    SOC_SAND_IN  SOC_PPD_ACTION_PROFILE                     *action_profile
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_mact_trap_info_set" API.
 *     Refer to "soc_ppd_frwrd_mact_trap_info_set" API for details.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_trap_info_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAP_TYPE               trap_type_ndx,
    SOC_SAND_IN  uint32                               port_profile_ndx,
    SOC_SAND_OUT SOC_PPD_ACTION_PROFILE                     *action_profile
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_ip_compatible_mc_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the information for bridging compatible Multicast
 *   MAC addresses.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO   *info -
 *     Compatible Multicast MAC information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_ip_compatible_mc_info_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO   *info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_mact_ip_compatible_mc_info_set" API.
 *     Refer to "soc_ppd_frwrd_mact_ip_compatible_mc_info_set" API
 *     for details.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_ip_compatible_mc_info_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO   *info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_event_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Read MACT event from the events FIFO into buffer.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_BUFFER            *event_buf -
 *     Buffer to copy the Event to.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_event_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_BUFFER            *event_buf
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_event_parse
 * TYPE:
 *   PROC
 * FUNCTION:
 *   The MACT may report different events using the event
 *   FIFO (e.g., learn, age, transplant, and retrieve). This
 *   API Parses the event buffer into a meaningful structure.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_BUFFER            *event_buf -
 *     Buffer includes MACT event
 *   SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_INFO              *mact_event -
 *     MACT Event parsed into structure
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_event_parse(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_BUFFER            *event_buf,
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_INFO              *mact_event
  );


/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_learn_msg_parse
 * TYPE:
 *   PROC
 * FUNCTION:
 *   parse learning message given as buffer.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_MACT_LEARN_MSG            *learn_msg -
 *     learning message as bytes buffer
 *   SOC_SAND_OUT SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE_INFO              *learn_events -
 *     learning events
 * REMARKS:
 *   Soc_petra-B:
 *   - Maximum number of events in an individual learn message is 8
 *   - Assuming packet received calling soc_petra_pkt_packet_recv
 *   - Assuming packet includes system header particularly FTMH header
 *   - Assuming called on local device
 *   - Calling soc_pb_pp_frwrd_mact_learn_msg_conf_get_unsafe/soc_pb_pp_frwrd_mact_learn_msg_parse_unsafe
 *   - To bypass above assumptions or to speed up performance user can call directly soc_pb_pp function/s
 *      1. call soc_pb_pp_frwrd_mact_learn_msg_conf_get_unsafe() once to get device configuration
 *      2. call soc_pb_pp_frwrd_mact_learn_msg_parse_unsafe() directly for each received packet
 *      - user may fill SOC_PPD_FRWRD_MACT_LEARN_MSG_CONF differently instead of calling 
 *        soc_pb_pp_frwrd_mact_learn_msg_conf_get_unsafe
 * RETURNS:
 *   OK or ERROR indication.
 *   - error is return if passed packet/buffer is not learning message
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_learn_msg_parse(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_LEARN_MSG                 *learn_msg,
    SOC_SAND_OUT  SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE_INFO     *learn_events
  );


/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_mim_init_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   set whether mim init function has been called 
 *   set soc arad sw database that will be kept in WB.
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                               mim_initialized-
 *     whether mim init function has been called 1/0
 * REMARKS:
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_mim_init_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN   uint8                              mim_initialized
                                  );
/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_mim_init_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   get whether mim init function has been called 
 *   get it from the soc arad sw database 
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                               mim_initialized-
 *     whether mim init function has been called 1/0
 * REMARKS:
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_mim_init_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT uint8                               *mim_initialized
                                  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_routed_learning_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   set routed packets MAC learning for different application types
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                              appFlags-
 *     indicates the L3 applications for which to enable/disable learning
 * REMARKS:
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_routed_learning_set(
    SOC_SAND_IN  int                             unit, 
    SOC_SAND_IN  uint32                             appFlags
                                        );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_routed_learning_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   get routed packets MAC learning mode for different application types
 * INPUT:
 *   SOC_SAND_IN  int                              unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                              appFlags-
 *     indicates the L3 applications for which learning is enabled
 * REMARKS:
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_routed_learning_get(
    SOC_SAND_IN  int                             unit, 
    SOC_SAND_OUT uint32                             *appFlags
                                        );

#ifdef BCM_88660_A0
/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_limit_mapped_val_to_table_index_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map a value to the common MACT limit table.
 * INPUT:
 *   SOC_SAND_IN  int                          unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                          mapped_val -
 *     LIF number for mapping to the common limit table.
 *   SOC_SAND_OUT uint32                          *limit_tbl_idx -
 *     Index to the entry in the common limit table.
 *   SOC_SAND_OUT uint32                          *is_reserved -
 *     Flag indicating whether the resulting index points to
 *     the reserved entry for invalid mappings.
 * REMARKS:
 *   Applicable only for Arad+.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_mact_limit_mapped_val_to_table_index_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 mapped_val,
    SOC_SAND_OUT uint32                                 *limit_tbl_idx,
    SOC_SAND_OUT uint32                                 *is_reserved
  );
#endif

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_mact_mgmt_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_ppd_api_frwrd_mact_mgmt module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_frwrd_mact_mgmt_get_procs_ptr(void);

void
  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_AGING_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_AGING_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_AGING_ONE_PASS_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_AGING_ONE_PASS_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY *info
  );

void
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE *info
  );

void
  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_EVENT_PROCESSING_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_PROCESSING_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_OPER_MODE_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_PORT_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_EVENT_LAG_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_LAG_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_EVENT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_EVENT_BUFFER_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_EVENT_BUFFER *info
  );


void
  SOC_PPD_FRWRD_MACT_LEARN_MSG_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_LEARN_MSG *info
  );

void
  SOC_PPD_FRWRD_MACT_LEARN_MSG_CONF_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_LEARN_MSG_CONF *info
  );

void
  SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE_INFO *info
  );


#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_FRWRD_MACT_LEARNING_MODE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_LEARNING_MODE enum_val
  );

const char*
  SOC_PPD_FRWRD_MACT_SHADOW_MODE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_SHADOW_MODE enum_val
  );

const char*
  SOC_PPD_FRWRD_MACT_MSG_HDR_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MSG_HDR_TYPE enum_val
  );

const char*
  SOC_PPD_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE enum_val
  );

const char*
  SOC_PPD_FRWRD_MACT_TRAP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAP_TYPE enum_val
  );

const char*
  SOC_PPD_FRWRD_MACT_EVENT_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_TYPE enum_val
  );

const char*
  SOC_PPD_FRWRD_MACT_EVENT_PATH_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_PATH_TYPE enum_val
  );

void
  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_AGING_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_AGING_ONE_PASS_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_ONE_PASS_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY *info
  );

void
  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE *info
  );

void
  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_EVENT_PROCESSING_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_PROCESSING_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_PORT_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_EVENT_LAG_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_LAG_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_EVENT_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_INFO *info
  );

void
  SOC_PPD_FRWRD_MACT_EVENT_BUFFER_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_BUFFER *info
  );

void
  SOC_PPD_FRWRD_MACT_LEARN_MSG_print(
    SOC_SAND_IN SOC_PPD_FRWRD_MACT_LEARN_MSG *info
  );

void
  SOC_PPD_FRWRD_MACT_LEARN_MSG_CONF_print(
    SOC_SAND_IN SOC_PPD_FRWRD_MACT_LEARN_MSG_CONF *info
  );

void
  SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE_INFO *info
  );


#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_frwrd_mact_oper_mode_info_set_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_OPER_MODE_INFO          *oper_mode_info
  );

void
  soc_ppd_frwrd_mact_oper_mode_info_get_print(
    SOC_SAND_IN  int                              unit
  );

void
  soc_ppd_frwrd_mact_aging_info_set_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_INFO              *aging_info
  );

void
  soc_ppd_frwrd_mact_aging_info_get_print(
    SOC_SAND_IN  int                              unit
  );

void
  soc_ppd_frwrd_mact_aging_events_handle_info_set_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_AGING_EVENTS_HANDLE     *aging_info
  );

void
  soc_ppd_frwrd_mact_aging_events_handle_info_get_print(
    SOC_SAND_IN  int                              unit
  );

void
  soc_ppd_frwrd_mact_fid_profile_to_fid_map_set_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                               fid_profile_ndx,
    SOC_SAND_IN  SOC_PPD_FID                                fid
  );

void
  soc_ppd_frwrd_mact_fid_profile_to_fid_map_get_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                               fid_profile_ndx
  );

void
  soc_ppd_frwrd_mact_mac_limit_glbl_info_set_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MAC_LIMIT_GLBL_INFO     *limit_info
  );

void
  soc_ppd_frwrd_mact_mac_limit_glbl_info_get_print(
    SOC_SAND_IN  int                              unit
  );

void
  soc_ppd_frwrd_mact_learn_profile_limit_info_set_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                               mac_learn_profile_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MAC_LIMIT_INFO          *limit_info
  );

void
  soc_ppd_frwrd_mact_learn_profile_limit_info_get_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                               mac_learn_profile_ndx
  );

void
  soc_ppd_frwrd_mact_mac_limit_exceeded_info_get_print(
    SOC_SAND_IN  int                              unit
  );

void
  soc_ppd_frwrd_mact_event_handle_profile_set_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                               mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                               event_handle_profile
  );

void
  soc_ppd_frwrd_mact_event_handle_profile_get_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                               mac_learn_profile_ndx
  );

void
  soc_ppd_frwrd_mact_event_handle_info_set_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY        *event_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_INFO       *handle_info
  );

void
  soc_ppd_frwrd_mact_event_handle_info_get_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_HANDLE_KEY        *event_key
  );

void
  soc_ppd_frwrd_mact_fid_aging_profile_set_print(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32    mac_learn_profile_ndx,
    SOC_SAND_IN  uint32    fid_aging_profile
  );

void
  soc_ppd_frwrd_mact_event_fid_aging_profile_get_print(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32    mac_learn_profile_ndx
  );

void
  soc_ppd_frwrd_mact_learn_msgs_distribution_info_set_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO          *distribution_info
  );

void
  soc_ppd_frwrd_mact_learn_msgs_distribution_info_get_print(
    SOC_SAND_IN  int                              unit
  );

void
  soc_ppd_frwrd_mact_shadow_msgs_distribution_info_set_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_MSG_DISTR_INFO          *distribution_info
  );

void
  soc_ppd_frwrd_mact_shadow_msgs_distribution_info_get_print(
    SOC_SAND_IN  int                              unit
  );

void
  soc_ppd_frwrd_mact_port_info_set_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_PORT                               local_port_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_PORT_INFO               *port_info
  );

void
  soc_ppd_frwrd_mact_port_info_get_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_PORT                               local_port_ndx
  );

void
  soc_ppd_frwrd_mact_trap_info_set_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAP_TYPE               trap_type_ndx,
    SOC_SAND_IN  uint32                               port_profile_ndx,
    SOC_SAND_IN  SOC_PPD_ACTION_PROFILE                     *action_profile
  );

void
  soc_ppd_frwrd_mact_trap_info_get_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_TRAP_TYPE               trap_type_ndx,
    SOC_SAND_IN  uint32                               port_profile_ndx
  );

void
  soc_ppd_frwrd_mact_ip_compatible_mc_info_set_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_IP_COMPATIBLE_MC_INFO   *info
  );

void
  soc_ppd_frwrd_mact_ip_compatible_mc_info_get_print(
    SOC_SAND_IN  int                              unit
  );

void
  soc_ppd_frwrd_mact_event_get_print(
    SOC_SAND_IN  int                              unit
  );

void
  soc_ppd_frwrd_mact_event_parse_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_EVENT_BUFFER            *event_buf
  );

void
  soc_ppd_frwrd_mact_learn_msg_parse_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_MACT_LEARN_MSG            *learn_msg
  );

#ifdef BCM_88660_A0
void
  soc_ppd_frwrd_mact_limit_mapped_val_to_table_index_get_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              mapped_val
  );
#endif

#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPD_API_FRWRD_MACT_MGMT_INCLUDED__*/
#endif

