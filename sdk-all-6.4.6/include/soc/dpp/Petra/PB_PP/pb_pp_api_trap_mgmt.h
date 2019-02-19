/* $Id: pb_pp_api_trap_mgmt.h,v 1.10 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_api_trap_mgmt.h
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

#ifndef __SOC_PB_PP_API_TRAP_MGMT_INCLUDED__
/* { */
#define __SOC_PB_PP_API_TRAP_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_trap_mgmt.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Value used to assign discard action to trapped packet.
 *     Use as detailed in the APIs refering to this definition. */
#define  SOC_PB_PP_TRAP_ACTION_PKT_DISCARD_ID (SOC_PPC_TRAP_ACTION_PKT_DISCARD_ID)

/*     Value used to disable egress trap.
*     Use as detailed in the APIs refering to this definition. */
#define  SOC_PB_PP_TRAP_EG_NO_ACTION       (SOC_PPC_TRAP_EG_NO_ACTION)

/*     Maximum buffer size for events.                         */
#define  SOC_PB_PP_TRAP_EVENT_BUFF_MAX_SIZE (SOC_PPC_TRAP_EVENT_BUFF_MAX_SIZE)

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

#define SOC_PB_PP_NOF_TRAP_SNOOP_ACTION_SIZES                  SOC_PPC_NOF_TRAP_SNOOP_ACTION_SIZES
typedef SOC_PPC_TRAP_SNOOP_ACTION_SIZE                         SOC_PB_PP_TRAP_SNOOP_ACTION_SIZE;

#define SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_DEST           SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST
#define SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_TC             SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_TC
#define SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_DP             SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DP
#define SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_POLICER        SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_POLICER
#define SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_OFFSET   SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_OFFSET
#define SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_CUD            SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_CUD
#define SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_DP_METER_CMD   SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DP_METER_CMD
typedef SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE                  SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE;

#define SOC_PB_PP_NOF_TRAP_MACT_EVENT_TYPES                    SOC_PPC_NOF_TRAP_MACT_EVENT_TYPES
typedef SOC_PPC_TRAP_MACT_EVENT_TYPE                           SOC_PB_PP_TRAP_MACT_EVENT_TYPE;

#define SOC_PB_PP_TRAP_CODE_PBP_SA_DROP_0                      SOC_PPC_TRAP_CODE_PBP_SA_DROP_0
#define SOC_PB_PP_TRAP_CODE_PBP_SA_DROP_1                      SOC_PPC_TRAP_CODE_PBP_SA_DROP_1
#define SOC_PB_PP_TRAP_CODE_PBP_SA_DROP_2                      SOC_PPC_TRAP_CODE_PBP_SA_DROP_2
#define SOC_PB_PP_TRAP_CODE_PBP_SA_DROP_3                      SOC_PPC_TRAP_CODE_PBP_SA_DROP_3
#define SOC_PB_PP_TRAP_CODE_PBP_TE_TRANSPLANT                  SOC_PPC_TRAP_CODE_PBP_TE_TRANSPLANT
#define SOC_PB_PP_TRAP_CODE_PBP_TE_UNKNOWN_TUNNEL              SOC_PPC_TRAP_CODE_PBP_TE_UNKNOWN_TUNNEL
#define SOC_PB_PP_TRAP_CODE_PBP_TRANSPLANT                     SOC_PPC_TRAP_CODE_PBP_TRANSPLANT
#define SOC_PB_PP_TRAP_CODE_PBP_LEARN_SNOOP                    SOC_PPC_TRAP_CODE_PBP_LEARN_SNOOP
#define SOC_PB_PP_TRAP_CODE_SA_AUTHENTICATION_FAILED           SOC_PPC_TRAP_CODE_SA_AUTHENTICATION_FAILED
#define SOC_PB_PP_TRAP_CODE_PORT_NOT_PERMITTED                 SOC_PPC_TRAP_CODE_PORT_NOT_PERMITTED
#define SOC_PB_PP_TRAP_CODE_UNEXPECTED_VID                     SOC_PPC_TRAP_CODE_UNEXPECTED_VID
#define SOC_PB_PP_TRAP_CODE_SA_MULTICAST                       SOC_PPC_TRAP_CODE_SA_MULTICAST
#define SOC_PB_PP_TRAP_CODE_SA_EQUALS_DA                       SOC_PPC_TRAP_CODE_SA_EQUALS_DA
#define SOC_PB_PP_TRAP_CODE_8021X                              SOC_PPC_TRAP_CODE_8021X
#define SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP         SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP
#define SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT       SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT
#define SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_1     SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_1
#define SOC_PB_PP_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_2     SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_2
#define SOC_PB_PP_TRAP_CODE_MY_ARP                             SOC_PPC_TRAP_CODE_MY_ARP
#define SOC_PB_PP_TRAP_CODE_ARP                                SOC_PPC_TRAP_CODE_ARP
#define SOC_PB_PP_TRAP_CODE_IGMP_MEMBERSHIP_QUERY              SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY
#define SOC_PB_PP_TRAP_CODE_IGMP_REPORT_LEAVE_MSG              SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG
#define SOC_PB_PP_TRAP_CODE_IGMP_UNDEFINED                     SOC_PPC_TRAP_CODE_IGMP_UNDEFINED
#define SOC_PB_PP_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY       SOC_PPC_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY
#define SOC_PB_PP_TRAP_CODE_RESERVED_MC_0                      SOC_PPC_TRAP_CODE_RESERVED_MC_0
#define SOC_PB_PP_TRAP_CODE_RESERVED_MC_7                      SOC_PPC_TRAP_CODE_RESERVED_MC_7
#define SOC_PB_PP_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG         SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG
#define SOC_PB_PP_TRAP_CODE_ICMPV6_MLD_UNDEFINED               SOC_PPC_TRAP_CODE_ICMPV6_MLD_UNDEFINED
#define SOC_PB_PP_TRAP_CODE_DHCP_SERVER                        SOC_PPC_TRAP_CODE_DHCP_SERVER
#define SOC_PB_PP_TRAP_CODE_DHCP_CLIENT                        SOC_PPC_TRAP_CODE_DHCP_CLIENT
#define SOC_PB_PP_TRAP_CODE_DHCPV6_SERVER                      SOC_PPC_TRAP_CODE_DHCPV6_SERVER
#define SOC_PB_PP_TRAP_CODE_DHCPV6_CLIENT                      SOC_PPC_TRAP_CODE_DHCPV6_CLIENT
#define SOC_PB_PP_TRAP_CODE_PROG_TRAP_0                        SOC_PPC_TRAP_CODE_PROG_TRAP_0
#define SOC_PB_PP_TRAP_CODE_PROG_TRAP_1                        SOC_PPC_TRAP_CODE_PROG_TRAP_1
#define SOC_PB_PP_TRAP_CODE_PROG_TRAP_2                        SOC_PPC_TRAP_CODE_PROG_TRAP_2
#define SOC_PB_PP_TRAP_CODE_PROG_TRAP_3                        SOC_PPC_TRAP_CODE_PROG_TRAP_3
#define SOC_PB_PP_TRAP_CODE_PORT_NOT_VLAN_MEMBER               SOC_PPC_TRAP_CODE_PORT_NOT_VLAN_MEMBER
#define SOC_PB_PP_TRAP_CODE_HEADER_SIZE_ERR                    SOC_PPC_TRAP_CODE_HEADER_SIZE_ERR
#define SOC_PB_PP_TRAP_CODE_HEADER_SIZE_ERR_O_MPLS             SOC_PPC_TRAP_CODE_HEADER_SIZE_ERR_O_MPLS
#define SOC_PB_PP_TRAP_CODE_MY_B_MAC_AND_LEARN_NULL            SOC_PPC_TRAP_CODE_MY_B_MAC_AND_LEARN_NULL
#define SOC_PB_PP_TRAP_CODE_MY_B_DA_UNKNOWN_I_SID              SOC_PPC_TRAP_CODE_MY_B_DA_UNKNOWN_I_SID
#define SOC_PB_PP_TRAP_CODE_STP_STATE_BLOCK                    SOC_PPC_TRAP_CODE_STP_STATE_BLOCK
#define SOC_PB_PP_TRAP_CODE_STP_STATE_LEARN                    SOC_PPC_TRAP_CODE_STP_STATE_LEARN
#define SOC_PB_PP_TRAP_CODE_IP_COMP_MC_INVALID_IP              SOC_PPC_TRAP_CODE_IP_COMP_MC_INVALID_IP
#define SOC_PB_PP_TRAP_CODE_MY_MAC_AND_IP_DISABLE              SOC_PPC_TRAP_CODE_MY_MAC_AND_IP_DISABLE
#define SOC_PB_PP_TRAP_CODE_TRILL_VERSION                      SOC_PPC_TRAP_CODE_TRILL_VERSION
#define SOC_PB_PP_TRAP_CODE_TRILL_INVALID_TTL                  SOC_PPC_TRAP_CODE_TRILL_INVALID_TTL
#define SOC_PB_PP_TRAP_CODE_TRILL_CHBH                         SOC_PPC_TRAP_CODE_TRILL_CHBH
#define SOC_PB_PP_TRAP_CODE_TRILL_NO_REVERSE_FEC               SOC_PPC_TRAP_CODE_TRILL_NO_REVERSE_FEC
#define SOC_PB_PP_TRAP_CODE_TRILL_CITE                         SOC_PPC_TRAP_CODE_TRILL_CITE
#define SOC_PB_PP_TRAP_CODE_TRILL_ILLEGAL_INNER_MC             SOC_PPC_TRAP_CODE_TRILL_ILLEGAL_INNER_MC
#define SOC_PB_PP_TRAP_CODE_MY_MAC_AND_MPLS_DISABLE            SOC_PPC_TRAP_CODE_MY_MAC_AND_MPLS_DISABLE
#define SOC_PB_PP_TRAP_CODE_MY_MAC_AND_ARP                     SOC_PPC_TRAP_CODE_MY_MAC_AND_ARP
#define SOC_PB_PP_TRAP_CODE_MY_MAC_AND_UNKNOWN_L3              SOC_PPC_TRAP_CODE_MY_MAC_AND_UNKNOWN_L3
#define SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_0                 SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_0
#define SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_1                 SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_1
#define SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_2                 SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_2
#define SOC_PB_PP_TRAP_CODE_MPLS_LABEL_VALUE_3                 SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_3
#define SOC_PB_PP_TRAP_CODE_MPLS_NO_RESOURCES                  SOC_PPC_TRAP_CODE_MPLS_NO_RESOURCES
#define SOC_PB_PP_TRAP_CODE_INVALID_LABEL_IN_RANGE             SOC_PPC_TRAP_CODE_INVALID_LABEL_IN_RANGE
#define SOC_PB_PP_TRAP_CODE_MPLS_INVALID_LABEL_IN_SEM          SOC_PPC_TRAP_CODE_MPLS_INVALID_LABEL_IN_SEM
#define SOC_PB_PP_TRAP_CODE_MPLS_LSP_BOS                       SOC_PPC_TRAP_CODE_MPLS_LSP_BOS
#define SOC_PB_PP_TRAP_CODE_MPLS_PWE_NO_BOS_LABEL_14           SOC_PPC_TRAP_CODE_MPLS_PWE_NO_BOS_LABEL_14
#define SOC_PB_PP_TRAP_CODE_MPLS_PWE_NO_BOS                    SOC_PPC_TRAP_CODE_MPLS_PWE_NO_BOS
#define SOC_PB_PP_TRAP_CODE_MPLS_VRF_NO_BOS                    SOC_PPC_TRAP_CODE_MPLS_VRF_NO_BOS
#define SOC_PB_PP_TRAP_CODE_MPLS_TERM_TTL_0                    SOC_PPC_TRAP_CODE_MPLS_TERM_TTL_0
#define SOC_PB_PP_TRAP_CODE_MPLS_TERM_CONTROL_WORD_TRAP        SOC_PPC_TRAP_CODE_MPLS_TERM_CONTROL_WORD_TRAP
#define SOC_PB_PP_TRAP_CODE_MPLS_TERM_CONTROL_WORD_DROP        SOC_PPC_TRAP_CODE_MPLS_TERM_CONTROL_WORD_DROP
#define SOC_PB_PP_TRAP_CODE_IPV4_TERM_VERSION_ERROR            SOC_PPC_TRAP_CODE_IPV4_TERM_VERSION_ERROR
#define SOC_PB_PP_TRAP_CODE_IPV4_TERM_CHECKSUM_ERROR           SOC_PPC_TRAP_CODE_IPV4_TERM_CHECKSUM_ERROR
#define SOC_PB_PP_TRAP_CODE_IPV4_TERM_HEADER_LENGTH_ERROR      SOC_PPC_TRAP_CODE_IPV4_TERM_HEADER_LENGTH_ERROR
#define SOC_PB_PP_TRAP_CODE_IPV4_TERM_TOTAL_LENGTH_ERROR       SOC_PPC_TRAP_CODE_IPV4_TERM_TOTAL_LENGTH_ERROR
#define SOC_PB_PP_TRAP_CODE_IPV4_TERM_TTL0                     SOC_PPC_TRAP_CODE_IPV4_TERM_TTL0
#define SOC_PB_PP_TRAP_CODE_IPV4_TERM_HAS_OPTIONS              SOC_PPC_TRAP_CODE_IPV4_TERM_HAS_OPTIONS
#define SOC_PB_PP_TRAP_CODE_IPV4_TERM_TTL1                     SOC_PPC_TRAP_CODE_IPV4_TERM_TTL1
#define SOC_PB_PP_TRAP_CODE_IPV4_TERM_SIP_EQUAL_DIP            SOC_PPC_TRAP_CODE_IPV4_TERM_SIP_EQUAL_DIP
#define SOC_PB_PP_TRAP_CODE_IPV4_TERM_DIP_ZERO                 SOC_PPC_TRAP_CODE_IPV4_TERM_DIP_ZERO
#define SOC_PB_PP_TRAP_CODE_IPV4_TERM_SIP_IS_MC                SOC_PPC_TRAP_CODE_IPV4_TERM_SIP_IS_MC
#define SOC_PB_PP_TRAP_CODE_CFM_ACCELERATED_INGRESS            SOC_PPC_TRAP_CODE_CFM_ACCELERATED_INGRESS
#define SOC_PB_PP_TRAP_CODE_ILLEGEL_PFC                        SOC_PPC_TRAP_CODE_ILLEGEL_PFC
#define SOC_PB_PP_TRAP_CODE_SA_DROP_0                          SOC_PPC_TRAP_CODE_SA_DROP_0
#define SOC_PB_PP_TRAP_CODE_SA_DROP_1                          SOC_PPC_TRAP_CODE_SA_DROP_1
#define SOC_PB_PP_TRAP_CODE_SA_DROP_2                          SOC_PPC_TRAP_CODE_SA_DROP_2
#define SOC_PB_PP_TRAP_CODE_SA_DROP_3                          SOC_PPC_TRAP_CODE_SA_DROP_3
#define SOC_PB_PP_TRAP_CODE_SA_NOT_FOUND_0                     SOC_PPC_TRAP_CODE_SA_NOT_FOUND_0
#define SOC_PB_PP_TRAP_CODE_SA_NOT_FOUND_1                     SOC_PPC_TRAP_CODE_SA_NOT_FOUND_1
#define SOC_PB_PP_TRAP_CODE_SA_NOT_FOUND_2                     SOC_PPC_TRAP_CODE_SA_NOT_FOUND_2
#define SOC_PB_PP_TRAP_CODE_SA_NOT_FOUND_3                     SOC_PPC_TRAP_CODE_SA_NOT_FOUND_3
#define SOC_PB_PP_TRAP_CODE_UNKNOWN_DA_0                       SOC_PPC_TRAP_CODE_UNKNOWN_DA_0
#define SOC_PB_PP_TRAP_CODE_UNKNOWN_DA_7                       SOC_PPC_TRAP_CODE_UNKNOWN_DA_7
#define SOC_PB_PP_TRAP_CODE_ELK_ERROR                          SOC_PPC_TRAP_CODE_ELK_ERROR
#define SOC_PB_PP_TRAP_CODE_DA_NOT_FOUND_0                     SOC_PPC_TRAP_CODE_DA_NOT_FOUND_0
#define SOC_PB_PP_TRAP_CODE_DA_NOT_FOUND_1                     SOC_PPC_TRAP_CODE_DA_NOT_FOUND_1
#define SOC_PB_PP_TRAP_CODE_DA_NOT_FOUND_2                     SOC_PPC_TRAP_CODE_DA_NOT_FOUND_2
#define SOC_PB_PP_TRAP_CODE_DA_NOT_FOUND_3                     SOC_PPC_TRAP_CODE_DA_NOT_FOUND_3
#define SOC_PB_PP_TRAP_CODE_P2P_MISCONFIGURATION               SOC_PPC_TRAP_CODE_P2P_MISCONFIGURATION
#define SOC_PB_PP_TRAP_CODE_SAME_INTERFACE                     SOC_PPC_TRAP_CODE_SAME_INTERFACE
#define SOC_PB_PP_TRAP_CODE_TRILL_UNKNOWN_UC                   SOC_PPC_TRAP_CODE_TRILL_UNKNOWN_UC
#define SOC_PB_PP_TRAP_CODE_TRILL_UNKNOWN_MC                   SOC_PPC_TRAP_CODE_TRILL_UNKNOWN_MC
#define SOC_PB_PP_TRAP_CODE_UC_LOOSE_RPF_FAIL                  SOC_PPC_TRAP_CODE_UC_LOOSE_RPF_FAIL
#define SOC_PB_PP_TRAP_CODE_DEFAULT_UCV6                       SOC_PPC_TRAP_CODE_DEFAULT_UCV6
#define SOC_PB_PP_TRAP_CODE_DEFAULT_MCV6                       SOC_PPC_TRAP_CODE_DEFAULT_MCV6
#define SOC_PB_PP_TRAP_CODE_MPLS_P2P_NO_BOS                    SOC_PPC_TRAP_CODE_MPLS_P2P_NO_BOS
#define SOC_PB_PP_TRAP_CODE_MPLS_CONTROL_WORD_TRAP             SOC_PPC_TRAP_CODE_MPLS_CONTROL_WORD_TRAP
#define SOC_PB_PP_TRAP_CODE_MPLS_CONTROL_WORD_DROP             SOC_PPC_TRAP_CODE_MPLS_CONTROL_WORD_DROP
#define SOC_PB_PP_TRAP_CODE_MPLS_UNKNOWN_LABEL                 SOC_PPC_TRAP_CODE_MPLS_UNKNOWN_LABEL
#define SOC_PB_PP_TRAP_CODE_MPLS_P2P_MPLSX4                    SOC_PPC_TRAP_CODE_MPLS_P2P_MPLSX4
#define SOC_PB_PP_TRAP_CODE_ETH_L2CP_PEER                      SOC_PPC_TRAP_CODE_ETH_L2CP_PEER
#define SOC_PB_PP_TRAP_CODE_ETH_L2CP_DROP                      SOC_PPC_TRAP_CODE_ETH_L2CP_DROP
#define SOC_PB_PP_TRAP_CODE_ETH_FL_IGMP_MEMBERSHIP_QUERY       SOC_PPC_TRAP_CODE_ETH_FL_IGMP_MEMBERSHIP_QUERY
#define SOC_PB_PP_TRAP_CODE_ETH_FL_IGMP_REPORT_LEAVE_MSG       SOC_PPC_TRAP_CODE_ETH_FL_IGMP_REPORT_LEAVE_MSG
#define SOC_PB_PP_TRAP_CODE_ETH_FL_IGMP_UNDEFINED              SOC_PPC_TRAP_CODE_ETH_FL_IGMP_UNDEFINED
#define SOC_PB_PP_TRAP_CODE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY
#define SOC_PB_PP_TRAP_CODE_ETH_FL_ICMPV6_MLD_REPORT_DONE      SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_REPORT_DONE
#define SOC_PB_PP_TRAP_CODE_ETH_FL_ICMPV6_MLD_UNDEFINED        SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_UNDEFINED
#define SOC_PB_PP_TRAP_CODE_IPV4_VERSION_ERROR                 SOC_PPC_TRAP_CODE_IPV4_VERSION_ERROR
#define SOC_PB_PP_TRAP_CODE_IPV4_CHECKSUM_ERROR                SOC_PPC_TRAP_CODE_IPV4_CHECKSUM_ERROR
#define SOC_PB_PP_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR           SOC_PPC_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR
#define SOC_PB_PP_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR            SOC_PPC_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR
#define SOC_PB_PP_TRAP_CODE_IPV4_TTL0                          SOC_PPC_TRAP_CODE_IPV4_TTL0
#define SOC_PB_PP_TRAP_CODE_IPV4_HAS_OPTIONS                   SOC_PPC_TRAP_CODE_IPV4_HAS_OPTIONS
#define SOC_PB_PP_TRAP_CODE_IPV4_TTL1                          SOC_PPC_TRAP_CODE_IPV4_TTL1
#define SOC_PB_PP_TRAP_CODE_IPV4_SIP_EQUAL_DIP                 SOC_PPC_TRAP_CODE_IPV4_SIP_EQUAL_DIP
#define SOC_PB_PP_TRAP_CODE_IPV4_DIP_ZERO                      SOC_PPC_TRAP_CODE_IPV4_DIP_ZERO
#define SOC_PB_PP_TRAP_CODE_IPV4_SIP_IS_MC                     SOC_PPC_TRAP_CODE_IPV4_SIP_IS_MC
#define SOC_PB_PP_TRAP_CODE_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED SOC_PPC_TRAP_CODE_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED
#define SOC_PB_PP_TRAP_CODE_IPV6_VERSION_ERROR                 SOC_PPC_TRAP_CODE_IPV6_VERSION_ERROR
#define SOC_PB_PP_TRAP_CODE_IPV6_HOP_COUNT0                    SOC_PPC_TRAP_CODE_IPV6_HOP_COUNT0
#define SOC_PB_PP_TRAP_CODE_IPV6_HOP_COUNT1                    SOC_PPC_TRAP_CODE_IPV6_HOP_COUNT1
#define SOC_PB_PP_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION       SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION
#define SOC_PB_PP_TRAP_CODE_IPV6_LOOPBACK_ADDRESS              SOC_PPC_TRAP_CODE_IPV6_LOOPBACK_ADDRESS
#define SOC_PB_PP_TRAP_CODE_IPV6_MULTICAST_SOURCE              SOC_PPC_TRAP_CODE_IPV6_MULTICAST_SOURCE
#define SOC_PB_PP_TRAP_CODE_IPV6_NEXT_HEADER_NULL              SOC_PPC_TRAP_CODE_IPV6_NEXT_HEADER_NULL
#define SOC_PB_PP_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE            SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE
#define SOC_PB_PP_TRAP_CODE_IPV6_LOCAL_LINK_DESTINATION        SOC_PPC_TRAP_CODE_IPV6_LOCAL_LINK_DESTINATION
#define SOC_PB_PP_TRAP_CODE_IPV6_LOCAL_SITE_DESTINATION        SOC_PPC_TRAP_CODE_IPV6_LOCAL_SITE_DESTINATION
#define SOC_PB_PP_TRAP_CODE_IPV6_LOCAL_LINK_SOURCE             SOC_PPC_TRAP_CODE_IPV6_LOCAL_LINK_SOURCE
#define SOC_PB_PP_TRAP_CODE_IPV6_LOCAL_SITE_SOURCE             SOC_PPC_TRAP_CODE_IPV6_LOCAL_SITE_SOURCE
#define SOC_PB_PP_TRAP_CODE_IPV6_IPV4_COMPATIBLE_DESTINATION   SOC_PPC_TRAP_CODE_IPV6_IPV4_COMPATIBLE_DESTINATION
#define SOC_PB_PP_TRAP_CODE_IPV6_IPV4_MAPPED_DESTINATION       SOC_PPC_TRAP_CODE_IPV6_IPV4_MAPPED_DESTINATION
#define SOC_PB_PP_TRAP_CODE_IPV6_MULTICAST_DESTINATION         SOC_PPC_TRAP_CODE_IPV6_MULTICAST_DESTINATION
#define SOC_PB_PP_TRAP_CODE_MPLS_TTL0                          SOC_PPC_TRAP_CODE_MPLS_TTL0
#define SOC_PB_PP_TRAP_CODE_MPLS_TTL1                          SOC_PPC_TRAP_CODE_MPLS_TTL1
#define SOC_PB_PP_TRAP_CODE_TCP_SN_FLAGS_ZERO                  SOC_PPC_TRAP_CODE_TCP_SN_FLAGS_ZERO
#define SOC_PB_PP_TRAP_CODE_TCP_SN_ZERO_FLAGS_SET              SOC_PPC_TRAP_CODE_TCP_SN_ZERO_FLAGS_SET
#define SOC_PB_PP_TRAP_CODE_TCP_SYN_FIN                        SOC_PPC_TRAP_CODE_TCP_SYN_FIN
#define SOC_PB_PP_TRAP_CODE_TCP_EQUAL_PORTS                    SOC_PPC_TRAP_CODE_TCP_EQUAL_PORTS
#define SOC_PB_PP_TRAP_CODE_TCP_FRAGMENT_INCOMPLETE_HEADER     SOC_PPC_TRAP_CODE_TCP_FRAGMENT_INCOMPLETE_HEADER
#define SOC_PB_PP_TRAP_CODE_TCP_FRAGMENT_OFFSET_LT8            SOC_PPC_TRAP_CODE_TCP_FRAGMENT_OFFSET_LT8
#define SOC_PB_PP_TRAP_CODE_UDP_EQUAL_PORTS                    SOC_PPC_TRAP_CODE_UDP_EQUAL_PORTS
#define SOC_PB_PP_TRAP_CODE_ICMP_DATA_GT_576                   SOC_PPC_TRAP_CODE_ICMP_DATA_GT_576
#define SOC_PB_PP_TRAP_CODE_ICMP_FRAGMENTED                    SOC_PPC_TRAP_CODE_ICMP_FRAGMENTED
#define SOC_PB_PP_TRAP_CODE_GENERAL                            SOC_PPC_TRAP_CODE_GENERAL
#define SOC_PB_PP_TRAP_CODE_FACILITY_INVALID                   SOC_PPC_TRAP_CODE_FACILITY_INVALID
#define SOC_PB_PP_TRAP_CODE_FEC_ENTRY_ACCESSED                 SOC_PPC_TRAP_CODE_FEC_ENTRY_ACCESSED
#define SOC_PB_PP_TRAP_CODE_UC_STRICT_RPF_FAIL                 SOC_PPC_TRAP_CODE_UC_STRICT_RPF_FAIL
#define SOC_PB_PP_TRAP_CODE_MC_EXPLICIT_RPF_FAIL               SOC_PPC_TRAP_CODE_MC_EXPLICIT_RPF_FAIL
#define SOC_PB_PP_TRAP_CODE_MC_USE_SIP_AS_IS_RPF_FAIL          SOC_PPC_TRAP_CODE_MC_USE_SIP_AS_IS_RPF_FAIL
#define SOC_PB_PP_TRAP_CODE_MC_USE_SIP_RPF_FAIL                SOC_PPC_TRAP_CODE_MC_USE_SIP_RPF_FAIL
#define SOC_PB_PP_TRAP_CODE_MC_USE_SIP_ECMP                    SOC_PPC_TRAP_CODE_MC_USE_SIP_ECMP
#define SOC_PB_PP_TRAP_CODE_ICMP_REDIRECT                      SOC_PPC_TRAP_CODE_ICMP_REDIRECT
#define SOC_PB_PP_TRAP_CODE_USER_OAMP                          SOC_PPC_TRAP_CODE_USER_OAMP
#define SOC_PB_PP_TRAP_CODE_USER_ETH_OAM_ACCELERATED           SOC_PPC_TRAP_CODE_USER_ETH_OAM_ACCELERATED
#define SOC_PB_PP_TRAP_CODE_USER_MPLS_OAM_ACCELERATED          SOC_PPC_TRAP_CODE_USER_MPLS_OAM_ACCELERATED
#define SOC_PB_PP_TRAP_CODE_USER_BFD_IP_OAM_TUNNEL_ACCELERATED SOC_PPC_TRAP_CODE_USER_BFD_IP_OAM_TUNNEL_ACCELERATED
#define SOC_PB_PP_TRAP_CODE_USER_BFD_PWE_OAM_ACCELERATED       SOC_PPC_TRAP_CODE_USER_BFD_PWE_OAM_ACCELERATED
#define SOC_PB_PP_TRAP_CODE_USER_ETH_OAM_UP_ACCELERATED        SOC_PPC_TRAP_CODE_USER_ETH_OAM_UP_ACCELERATED
#define SOC_PB_PP_TRAP_CODE_USER_DEFINED_0                     SOC_PPC_TRAP_CODE_USER_DEFINED_0
#define SOC_PB_PP_TRAP_CODE_USER_DEFINED_5                     SOC_PPC_TRAP_CODE_USER_DEFINED_5
#define SOC_PB_PP_TRAP_CODE_USER_DEFINED_6                     SOC_PPC_TRAP_CODE_USER_DEFINED_6
#define SOC_PB_PP_TRAP_CODE_USER_DEFINED_7                     SOC_PPC_TRAP_CODE_USER_DEFINED_7
#define SOC_PB_PP_TRAP_CODE_USER_DEFINED_8                     SOC_PPC_TRAP_CODE_USER_DEFINED_8
#define SOC_PB_PP_TRAP_CODE_USER_DEFINED_10                    SOC_PPC_TRAP_CODE_USER_DEFINED_10
#define SOC_PB_PP_TRAP_CODE_USER_DEFINED_11                    SOC_PPC_TRAP_CODE_USER_DEFINED_11
#define SOC_PB_PP_TRAP_CODE_USER_DEFINED_12                    SOC_PPC_TRAP_CODE_USER_DEFINED_12
#define SOC_PB_PP_TRAP_CODE_USER_DEFINED_13                    SOC_PPC_TRAP_CODE_USER_DEFINED_13
#define SOC_PB_PP_TRAP_CODE_USER_DEFINED_27                    SOC_PPC_TRAP_CODE_USER_DEFINED_27
#define SOC_PB_PP_TRAP_CODE_USER_DEFINED_28                    SOC_PPC_TRAP_CODE_USER_DEFINED_28
#define SOC_PB_PP_TRAP_CODE_USER_DEFINED_35                    SOC_PPC_TRAP_CODE_USER_DEFINED_35
#define SOC_PB_PP_TRAP_CODE_USER_DEFINED_36                    SOC_PPC_TRAP_CODE_USER_DEFINED_36
#define SOC_PB_PP_TRAP_CODE_USER_DEFINED_51                    SOC_PPC_TRAP_CODE_USER_DEFINED_51
#define SOC_PB_PP_TRAP_CODE_USER_DEFINED_52                    SOC_PPC_TRAP_CODE_USER_DEFINED_52
#define SOC_PB_PP_TRAP_CODE_USER_DEFINED_59                    SOC_PPC_TRAP_CODE_USER_DEFINED_59
#define SOC_PB_PP_NOF_TRAP_CODES                               SOC_PPC_NOF_TRAP_CODES
typedef SOC_PPC_TRAP_CODE                                      SOC_PB_PP_TRAP_CODE;

#define SOC_PB_PP_TRAP_EG_TYPE_NO_VSI_TRANSLATION              SOC_PPC_TRAP_EG_TYPE_NO_VSI_TRANSLATION
#define SOC_PB_PP_TRAP_EG_TYPE_VSI_MEMBERSHIP                  SOC_PPC_TRAP_EG_TYPE_VSI_MEMBERSHIP
#define SOC_PB_PP_TRAP_EG_TYPE_ACC_FRM                         SOC_PPC_TRAP_EG_TYPE_ACC_FRM
#define SOC_PB_PP_TRAP_EG_TYPE_HAIR_PIN                        SOC_PPC_TRAP_EG_TYPE_HAIR_PIN
#define SOC_PB_PP_TRAP_EG_TYPE_UNKNOWN_DA                      SOC_PPC_TRAP_EG_TYPE_UNKNOWN_DA
#define SOC_PB_PP_TRAP_EG_TYPE_SPLIT_HORIZON                   SOC_PPC_TRAP_EG_TYPE_SPLIT_HORIZON
#define SOC_PB_PP_TRAP_EG_TYPE_PRIVATE_VLAN                    SOC_PPC_TRAP_EG_TYPE_PRIVATE_VLAN
#define SOC_PB_PP_TRAP_EG_TYPE_TTL_SCOPE                       SOC_PPC_TRAP_EG_TYPE_TTL_SCOPE
#define SOC_PB_PP_TRAP_EG_TYPE_MTU_VIOLATION                   SOC_PPC_TRAP_EG_TYPE_MTU_VIOLATION
#define SOC_PB_PP_TRAP_EG_TYPE_TRILL_TTL_0                     SOC_PPC_TRAP_EG_TYPE_TRILL_TTL_0
#define SOC_PB_PP_TRAP_EG_TYPE_TRILL_SAME_INTERFACE            SOC_PPC_TRAP_EG_TYPE_TRILL_SAME_INTERFACE
#define SOC_PB_PP_TRAP_EG_TYPE_TRILL_BOUNCE_BACK               SOC_PPC_TRAP_EG_TYPE_TRILL_BOUNCE_BACK
#define SOC_PB_PP_TRAP_EG_TYPE_DSS_STACKING                    SOC_PPC_TRAP_EG_TYPE_DSS_STACKING
#define SOC_PB_PP_TRAP_EG_TYPE_LAG_MULTICAST                   SOC_PPC_TRAP_EG_TYPE_LAG_MULTICAST
#define SOC_PB_PP_TRAP_EG_TYPE_EXCLUDE_SRC                     SOC_PPC_TRAP_EG_TYPE_EXCLUDE_SRC
#define SOC_PB_PP_TRAP_EG_TYPE_CFM_TRAP                        SOC_PPC_TRAP_EG_TYPE_CFM_TRAP
#define SOC_PB_PP_NOF_TRAP_EG_TYPES                            SOC_PPC_NOF_TRAP_EG_TYPES_PB
typedef SOC_PPC_TRAP_EG_TYPE                                   SOC_PB_PP_TRAP_EG_TYPE;

typedef SOC_PPC_TRAP_ACTION_PROFILE_DEST_INFO                  SOC_PB_PP_TRAP_ACTION_PROFILE_DEST_INFO;
typedef SOC_PPC_TRAP_ACTION_PROFILE_COUNT_INFO                 SOC_PB_PP_TRAP_ACTION_PROFILE_COUNT_INFO;
typedef SOC_PPC_TRAP_ACTION_PROFILE_METER_INFO                 SOC_PB_PP_TRAP_ACTION_PROFILE_METER_INFO;
typedef SOC_PPC_TRAP_ACTION_PROFILE_POLICE_INFO                SOC_PB_PP_TRAP_ACTION_PROFILE_POLICE_INFO;
typedef SOC_PPC_TRAP_ACTION_PROFILE_COS_INFO                   SOC_PB_PP_TRAP_ACTION_PROFILE_COS_INFO;
typedef SOC_PPC_TRAP_ACTION_PROFILE_PROCESS_INFO               SOC_PB_PP_TRAP_ACTION_PROFILE_PROCESS_INFO;
typedef SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO                 SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO;
typedef SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO                 SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO;
typedef SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO                    SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO;
typedef SOC_PPC_TRAP_MACT_EVENT_INFO                           SOC_PB_PP_TRAP_MACT_EVENT_INFO;
typedef SOC_PPC_TRAP_PACKET_INFO                               SOC_PB_PP_TRAP_PACKET_INFO;

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
 *   soc_pb_pp_trap_frwrd_profile_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set forwarding action profile information.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                               trap_code_ndx -
 *     Trap code. Soc_petraB range: 0-255. T20E range: 0-255, only
 *     enumarators mentioned as supported in T20e.user can use
 *     values not in the enum for used defined values.
 *   SOC_SAND_IN  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO          *profile_info -
 *     Information to set to the forwarding profile.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_trap_frwrd_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_IN  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO          *profile_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_trap_frwrd_profile_info_set" API.
 *     Refer to "soc_pb_pp_trap_frwrd_profile_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_trap_frwrd_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_OUT SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO          *profile_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_trap_snoop_profile_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set snoop action profile information.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                               trap_code_ndx -
 *     Trap code. Soc_petraB range: 0-255. T20E range: 0-255, only
 *     enumarators mentioned as supported in T20E.user can use
 *     values not in the enum for used defined values.
 *   SOC_SAND_IN  SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO          *profile_info -
 *     Information to set to the snoop profile.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_trap_snoop_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_IN  SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO          *profile_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_trap_snoop_profile_info_set" API.
 *     Refer to "soc_pb_pp_trap_snoop_profile_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_trap_snoop_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_OUT SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO          *profile_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_trap_to_eg_action_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Maps egress trap type to egress action profile.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      trap_type_bitmap_ndx -
 *     Trap Type bitmap, the cause for trapping/filtering the packets
 *     (for example MTU). Use SOC_PB_PP_TRAP_EG_TYPE for bit offsets.
 *   SOC_SAND_IN  uint32                                      eg_action_profile -
 *     Egress action profile, to process/forward the packet
 *     according. To set the action pointed by this parameter
 *     use soc_ppd_trap_eg_profile_info_set(). Use
 *     SOC_PPD_TRAP_EG_NO_ACTION in order to bypass this trapping
 *     and then the packet will be processed/forwarded normal
 *     (as no trap was identified).
 * REMARKS:
 *   - Soc_petra-B only. Error is returned if called for T20E.-
 *   In T20E: use soc_ppd_trap_snoop_profile_info_set() and
 *   soc_ppd_trap_frwrd_profile_info_set() to set action for Trap
 *   occurs in the T20E egress.- For part of the
 *   filters/traps (see SOC_PPD_EG_FILTER_PORT_ENABLE) user can
 *   set whether to perform the filter/trap per port. See
 *   soc_ppd_eg_filter_port_info_set()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_trap_to_eg_action_map_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      trap_type_bitmap_ndx,
    SOC_SAND_IN  uint32                                      eg_action_profile
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_trap_to_eg_action_map_set" API.
 *     Refer to "soc_pb_pp_trap_to_eg_action_map_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_trap_to_eg_action_map_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      trap_type_bitmap_ndx,
    SOC_SAND_OUT uint32                                      *eg_action_profile
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_trap_eg_profile_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set egress action profile information.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      profile_ndx -
 *     Egress action profile. Range: 0 - 8.
 *   SOC_SAND_IN  SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO             *profile_info -
 *     Information to set to the egress profile.
 * REMARKS:
 *   - Soc_petra-B only. Error is returned if called for T20E.-
 *   In T20E: use soc_ppd_trap_snoop_profile_info_set() and
 *   soc_ppd_trap_frwrd_profile_info_set() to set action for Trap
 *   occurs in the T20E egress.- Use
 *   soc_ppd_trap_to_eg_action_map_set() to map trap (reason) to
 *   action profile
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_trap_eg_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO             *profile_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_trap_eg_profile_info_set" API.
 *     Refer to "soc_pb_pp_trap_eg_profile_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_trap_eg_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO             *profile_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_trap_mact_event_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Read MACT event from the events FIFO into buffer.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT uint32                                      buff -
 *     SOC_PB_PP_TRAP_EVENT_BUFF_MAX_SIZE]- Buffer to copy the Event
 *     to. Event is copied to buff starting from buff[0] lsb.
 *   SOC_SAND_OUT uint32                                      *buff_len -
 *     the actual length of the returned buffer (in longs)
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_trap_mact_event_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT uint32                                      buff[SOC_PB_PP_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_OUT uint32                                      *buff_len
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_trap_mact_event_parse
 * TYPE:
 *   PROC
 * FUNCTION:
 *   The MACT may report different events using the event
 *   FIFO (e.g., learn, age, transplant, and retrieve). This
 *   API Parses the event buffer into a meaningful structure.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      buff -
 *     SOC_PB_PP_TRAP_EVENT_BUFF_MAX_SIZE]- Buffer includes MACT
 *     event
 *   SOC_SAND_IN  uint32                                      buff_len -
 *     the actual length of the given buffer (in longs)
 *   SOC_SAND_OUT SOC_PB_PP_TRAP_MACT_EVENT_INFO                    *mact_event -
 *     MACT Event parsed into structure
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_trap_mact_event_parse(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      buff[SOC_PB_PP_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_IN  uint32                                      buff_len,
    SOC_SAND_OUT SOC_PB_PP_TRAP_MACT_EVENT_INFO                    *mact_event
  );

void
  SOC_PB_PP_TRAP_ACTION_PROFILE_DEST_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_TRAP_ACTION_PROFILE_DEST_INFO *info
  );

void
  SOC_PB_PP_TRAP_ACTION_PROFILE_COUNT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_TRAP_ACTION_PROFILE_COUNT_INFO *info
  );

void
  SOC_PB_PP_TRAP_ACTION_PROFILE_METER_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_TRAP_ACTION_PROFILE_METER_INFO *info
  );

void
  SOC_PB_PP_TRAP_ACTION_PROFILE_POLICE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_TRAP_ACTION_PROFILE_POLICE_INFO *info
  );

void
  SOC_PB_PP_TRAP_ACTION_PROFILE_COS_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_TRAP_ACTION_PROFILE_COS_INFO *info
  );

void
  SOC_PB_PP_TRAP_ACTION_PROFILE_PROCESS_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_TRAP_ACTION_PROFILE_PROCESS_INFO *info
  );

void
  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO *info
  );

void
  SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO *info
  );

void
  SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO *info
  );

void
  SOC_PB_PP_TRAP_MACT_EVENT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_TRAP_MACT_EVENT_INFO *info
  );

void
  SOC_PB_PP_TRAP_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_TRAP_PACKET_INFO *info
  );

#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_TRAP_SNOOP_ACTION_SIZE_to_string(
    SOC_SAND_IN  SOC_PB_PP_TRAP_SNOOP_ACTION_SIZE enum_val
  );

const char*
  SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE_to_string(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_OVERWRITE enum_val
  );

const char*
  SOC_PB_PP_TRAP_MACT_EVENT_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_TRAP_MACT_EVENT_TYPE enum_val
  );

const char*
  SOC_PB_PP_TRAP_CODE_to_string(
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE enum_val
  );

const char*
  SOC_PB_PP_TRAP_EG_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_TRAP_EG_TYPE enum_val
  );

void
  SOC_PB_PP_TRAP_ACTION_PROFILE_DEST_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_DEST_INFO *info
  );

void
  SOC_PB_PP_TRAP_ACTION_PROFILE_COUNT_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_COUNT_INFO *info
  );

void
  SOC_PB_PP_TRAP_ACTION_PROFILE_METER_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_METER_INFO *info
  );

void
  SOC_PB_PP_TRAP_ACTION_PROFILE_POLICE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_POLICE_INFO *info
  );

void
  SOC_PB_PP_TRAP_ACTION_PROFILE_COS_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_COS_INFO *info
  );

void
  SOC_PB_PP_TRAP_ACTION_PROFILE_PROCESS_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_PROCESS_INFO *info
  );

void
  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO *info
  );

void
  SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO *info
  );

void
  SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO *info
  );

void
  SOC_PB_PP_TRAP_MACT_EVENT_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_TRAP_MACT_EVENT_INFO *info
  );

void
  SOC_PB_PP_TRAP_PACKET_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_TRAP_PACKET_INFO *info
  );

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_API_TRAP_MGMT_INCLUDED__*/
#endif
