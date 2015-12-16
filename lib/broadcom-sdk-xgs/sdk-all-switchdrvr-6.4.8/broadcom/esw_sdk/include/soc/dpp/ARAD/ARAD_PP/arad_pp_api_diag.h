/* $Id: arad_pp_api_diag.h,v 1.24 Broadcom SDK $
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

#ifndef __ARAD_PP_API_DIAG_INCLUDED__
/* { */
#define __ARAD_PP_API_DIAG_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_diag.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximum number of headers can be identified by device   */
#define  ARAD_PP_DIAG_MAX_NOF_HDRS                           SOC_PPC_DIAG_MAX_NOF_HDRS
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

#define ARAD_PP_DIAG_FWD_LKUP_TYPE_NONE                      SOC_PPC_DIAG_FWD_LKUP_TYPE_NONE
#define ARAD_PP_DIAG_FWD_LKUP_TYPE_MACT                      SOC_PPC_DIAG_FWD_LKUP_TYPE_MACT
#define ARAD_PP_DIAG_FWD_LKUP_TYPE_BMACT                     SOC_PPC_DIAG_FWD_LKUP_TYPE_BMACT
#define ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_UC                   SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_UC
#define ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_MC                   SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_MC
#define ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV4_VPN                  SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV4_VPN
#define ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_UC                   SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_UC
#define ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_MC                   SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_MC
#define ARAD_PP_DIAG_FWD_LKUP_TYPE_IPV6_VPN                  SOC_PPC_DIAG_FWD_LKUP_TYPE_IPV6_VPN
#define ARAD_PP_DIAG_FWD_LKUP_TYPE_ILM                       SOC_PPC_DIAG_FWD_LKUP_TYPE_ILM
#define ARAD_PP_DIAG_FWD_LKUP_TYPE_TRILL_UC                  SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_UC
#define ARAD_PP_DIAG_FWD_LKUP_TYPE_TRILL_MC                  SOC_PPC_DIAG_FWD_LKUP_TYPE_TRILL_MC
#define ARAD_PP_DIAG_FWD_LKUP_TYPE_FCF                       SOC_PPC_DIAG_FWD_LKUP_TYPE_FCF
#define ARAD_PP_NOF_DIAG_FWD_LKUP_TYPES                      SOC_PPC_NOF_DIAG_FWD_LKUP_TYPES
typedef SOC_PPC_DIAG_FWD_LKUP_TYPE                             ARAD_PP_DIAG_FWD_LKUP_TYPE;

#define ARAD_PP_DIAG_TCAM_USAGE_FRWRDING                     SOC_PPC_DIAG_TCAM_USAGE_FRWRDING
#define ARAD_PP_DIAG_TCAM_USAGE_PMF                          SOC_PPC_DIAG_TCAM_USAGE_PMF
#define ARAD_PP_DIAG_TCAM_USAGE_EGR_ACL                      SOC_PPC_DIAG_TCAM_USAGE_EGR_ACL
#define ARAD_PP_NOF_DIAG_TCAM_USAGES                         SOC_PPC_NOF_DIAG_TCAM_USAGES
typedef SOC_PPC_DIAG_TCAM_USAGE                                ARAD_PP_DIAG_TCAM_USAGE;

#define ARAD_PP_DIAG_FLAVOR_NONE                             SOC_PPC_DIAG_FLAVOR_NONE
#define ARAD_PP_DIAG_FLAVOR_RAW                              SOC_PPC_DIAG_FLAVOR_RAW
#define ARAD_PP_DIAG_FLAVOR_CLEAR_ON_GET                     SOC_PPC_DIAG_FLAVOR_CLEAR_ON_GET
#define ARAD_PP_NOF_DIAG_FLAVORS                             SOC_PPC_NOF_DIAG_FLAVORS
typedef SOC_PPC_DIAG_FLAVOR                                    ARAD_PP_DIAG_FLAVOR;

#define ARAD_PP_DIAG_LEM_LKUP_TYPE_NONE                      SOC_PPC_DIAG_LEM_LKUP_TYPE_NONE
#define ARAD_PP_DIAG_LEM_LKUP_TYPE_MACT                      SOC_PPC_DIAG_LEM_LKUP_TYPE_MACT
#define ARAD_PP_DIAG_LEM_LKUP_TYPE_BMACT                     SOC_PPC_DIAG_LEM_LKUP_TYPE_BMACT
#define ARAD_PP_DIAG_LEM_LKUP_TYPE_HOST                      SOC_PPC_DIAG_LEM_LKUP_TYPE_HOST
#define ARAD_PP_DIAG_LEM_LKUP_TYPE_ILM                       SOC_PPC_DIAG_LEM_LKUP_TYPE_ILM
#define ARAD_PP_DIAG_LEM_LKUP_TYPE_SA_AUTH                   SOC_PPC_DIAG_LEM_LKUP_TYPE_SA_AUTH
#define ARAD_PP_DIAG_LEM_LKUP_TYPE_TRILL_ADJ                 SOC_PPC_DIAG_LEM_LKUP_TYPE_TRILL_ADJ
#define ARAD_PP_DIAG_LEM_LKUP_TYPE_EXTEND_P2P                SOC_PPC_DIAG_LEM_LKUP_TYPE_EXTEND_P2P
#define ARAD_PP_NOF_DIAG_LEM_LKUP_TYPES                      SOC_PPC_NOF_DIAG_LEM_LKUP_TYPES
typedef SOC_PPC_DIAG_LEM_LKUP_TYPE                             ARAD_PP_DIAG_LEM_LKUP_TYPE;

#define ARAD_PP_DIAG_LIF_LKUP_TYPE_NONE                      SOC_PPC_DIAG_LIF_LKUP_TYPE_NONE
#define ARAD_PP_DIAG_LIF_LKUP_TYPE_AC                        SOC_PPC_DIAG_LIF_LKUP_TYPE_AC
#define ARAD_PP_DIAG_LIF_LKUP_TYPE_PWE                       SOC_PPC_DIAG_LIF_LKUP_TYPE_PWE
#define ARAD_PP_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL               SOC_PPC_DIAG_LIF_LKUP_TYPE_MPLS_TUNNEL
#define ARAD_PP_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL               SOC_PPC_DIAG_LIF_LKUP_TYPE_IPV4_TUNNEL
#define ARAD_PP_DIAG_LIF_LKUP_TYPE_TRILL                     SOC_PPC_DIAG_LIF_LKUP_TYPE_TRILL
#define ARAD_PP_DIAG_LIF_LKUP_TYPE_MIM_ISID                  SOC_PPC_DIAG_LIF_LKUP_TYPE_MIM_ISID
#define ARAD_PP_DIAG_LIF_LKUP_TYPE_L2GRE                     SOC_PPC_DIAG_LIF_LKUP_TYPE_L2GRE
#define ARAD_PP_DIAG_LIF_LKUP_TYPE_VXLAN                   SOC_PPC_DIAG_LIF_LKUP_TYPE_VXLAN
#define ARAD_PP_NOF_DIAG_LIF_LKUP_TYPES                      SOC_PPC_NOF_DIAG_LIF_LKUP_TYPES
typedef SOC_PPC_DIAG_LIF_LKUP_TYPE                             ARAD_PP_DIAG_LIF_LKUP_TYPE;

#define ARAD_PP_DIAG_DB_TYPE_LIF                             SOC_PPC_DIAG_DB_TYPE_LIF
#define ARAD_PP_DIAG_DB_TYPE_LEM                             SOC_PPC_DIAG_DB_TYPE_LEM
#define ARAD_PP_DIAG_DB_TYPE_LPM                             SOC_PPC_DIAG_DB_TYPE_LPM
#define ARAD_PP_DIAG_DB_TYPE_TCAM_FRWRD                      SOC_PPC_DIAG_DB_TYPE_TCAM_FRWRD
#define ARAD_PP_DIAG_DB_TYPE_TCAM_PMF                        SOC_PPC_DIAG_DB_TYPE_TCAM_PMF
#define ARAD_PP_DIAG_DB_TYPE_TCAM_EGR                        SOC_PPC_DIAG_DB_TYPE_TCAM_EGR
#define ARAD_PP_DIAG_DB_TYPE_EG_LIF                          SOC_PPC_DIAG_DB_TYPE_EG_LIF
#define ARAD_PP_DIAG_DB_TYPE_EG_TUNNEL                       SOC_PPC_DIAG_DB_TYPE_EG_TUNNEL
#define ARAD_PP_DIAG_DB_TYPE_EG_LL                           SOC_PPC_DIAG_DB_TYPE_EG_LL
#define ARAD_PP_NOF_DIAG_DB_TYPES                            SOC_PPC_NOF_DIAG_DB_TYPES
typedef SOC_PPC_DIAG_DB_TYPE                                   ARAD_PP_DIAG_DB_TYPE;

#define ARAD_PP_DIAG_FRWRD_DECISION_PHASE_INIT_PORT          SOC_PPC_DIAG_FRWRD_DECISION_PHASE_INIT_PORT
#define ARAD_PP_DIAG_FRWRD_DECISION_PHASE_LIF                SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LIF
#define ARAD_PP_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND         SOC_PPC_DIAG_FRWRD_DECISION_PHASE_LKUP_FOUND
#define ARAD_PP_DIAG_FRWRD_DECISION_PHASE_PMF                SOC_PPC_DIAG_FRWRD_DECISION_PHASE_PMF
#define ARAD_PP_DIAG_FRWRD_DECISION_PHASE_FEC                SOC_PPC_DIAG_FRWRD_DECISION_PHASE_FEC
#define ARAD_PP_DIAG_FRWRD_DECISION_PHASE_TRAP               SOC_PPC_DIAG_FRWRD_DECISION_PHASE_TRAP
#define ARAD_PP_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED       SOC_PPC_DIAG_FRWRD_DECISION_PHASE_ING_RESOLVED
#define ARAD_PP_NOF_DIAG_FRWRD_DECISION_PHASES               SOC_PPC_NOF_DIAG_FRWRD_DECISION_PHASES
typedef SOC_PPC_DIAG_FRWRD_DECISION_PHASE                      ARAD_PP_DIAG_FRWRD_DECISION_PHASE;

#define ARAD_PP_DIAG_MPLS_TERM_TYPE_NONE                     SOC_PPC_DIAG_MPLS_TERM_TYPE_NONE
#define ARAD_PP_DIAG_MPLS_TERM_TYPE_RANGE                    SOC_PPC_DIAG_MPLS_TERM_TYPE_RANGE
#define ARAD_PP_DIAG_MPLS_TERM_TYPE_LIF_LKUP                 SOC_PPC_DIAG_MPLS_TERM_TYPE_LIF_LKUP
#define ARAD_PP_NOF_DIAG_MPLS_TERM_TYPES                     SOC_PPC_NOF_DIAG_MPLS_TERM_TYPES
typedef SOC_PPC_DIAG_MPLS_TERM_TYPE                            ARAD_PP_DIAG_MPLS_TERM_TYPE;

#define ARAD_PP_DIAG_PKT_TRACE_LIF                           SOC_PPC_DIAG_PKT_TRACE_LIF
#define ARAD_PP_DIAG_PKT_TRACE_TUNNEL_RNG                    SOC_PPC_DIAG_PKT_TRACE_TUNNEL_RNG
#define ARAD_PP_DIAG_PKT_TRACE_TRAP                          SOC_PPC_DIAG_PKT_TRACE_TRAP
#define ARAD_PP_DIAG_PKT_TRACE_FEC                           SOC_PPC_DIAG_PKT_TRACE_FEC
#define ARAD_PP_DIAG_PKT_TRACE_EG_DROP_LOG                   SOC_PPC_DIAG_PKT_TRACE_EG_DROP_LOG
#define ARAD_PP_DIAG_PKT_TRACE_ALL                           SOC_PPC_DIAG_PKT_TRACE_ALL
#define ARAD_PP_NOF_DIAG_PKT_TRACES                          SOC_PPC_NOF_DIAG_PKT_TRACES
typedef SOC_PPC_DIAG_PKT_TRACE                                 ARAD_PP_DIAG_PKT_TRACE;

#define ARAD_PP_DIAG_EG_DROP_REASON_NONE                     SOC_PPC_DIAG_EG_DROP_REASON_NONE
#define ARAD_PP_DIAG_EG_DROP_REASON_CNM                      SOC_PPC_DIAG_EG_DROP_REASON_CNM
#define ARAD_PP_DIAG_EG_DROP_REASON_CFM_TRAP                 SOC_PPC_DIAG_EG_DROP_REASON_CFM_TRAP
#define ARAD_PP_DIAG_EG_DROP_REASON_NO_VSI_TRANSLATION       SOC_PPC_DIAG_EG_DROP_REASON_NO_VSI_TRANSLATION
#define ARAD_PP_DIAG_EG_DROP_REASON_DSS_STACKING             SOC_PPC_DIAG_EG_DROP_REASON_DSS_STACKING
#define ARAD_PP_DIAG_EG_DROP_REASON_LAG_MULTICAST            SOC_PPC_DIAG_EG_DROP_REASON_LAG_MULTICAST
#define ARAD_PP_DIAG_EG_DROP_REASON_EXCLUDE_SRC              SOC_PPC_DIAG_EG_DROP_REASON_EXCLUDE_SRC
#define ARAD_PP_DIAG_EG_DROP_REASON_VLAN_MEMBERSHIP          SOC_PPC_DIAG_EG_DROP_REASON_VLAN_MEMBERSHIP
#define ARAD_PP_DIAG_EG_DROP_REASON_UNACCEPTABLE_FRAME_TYPE  SOC_PPC_DIAG_EG_DROP_REASON_UNACCEPTABLE_FRAME_TYPE
#define ARAD_PP_DIAG_EG_DROP_REASON_SRC_EQUAL_DEST           SOC_PPC_DIAG_EG_DROP_REASON_SRC_EQUAL_DEST
#define ARAD_PP_DIAG_EG_DROP_REASON_UNKNOWN_DA               SOC_PPC_DIAG_EG_DROP_REASON_UNKNOWN_DA
#define ARAD_PP_DIAG_EG_DROP_REASON_SPLIT_HORIZON            SOC_PPC_DIAG_EG_DROP_REASON_SPLIT_HORIZON
#define ARAD_PP_DIAG_EG_DROP_REASON_PRIVATE_VLAN             SOC_PPC_DIAG_EG_DROP_REASON_PRIVATE_VLAN
#define ARAD_PP_DIAG_EG_DROP_REASON_TTL_SCOPE                SOC_PPC_DIAG_EG_DROP_REASON_TTL_SCOPE
#define ARAD_PP_DIAG_EG_DROP_REASON_MTU_VIOLATION            SOC_PPC_DIAG_EG_DROP_REASON_MTU_VIOLATION
#define ARAD_PP_DIAG_EG_DROP_REASON_TRILL_TTL_ZERO           SOC_PPC_DIAG_EG_DROP_REASON_TRILL_TTL_ZERO
#define ARAD_PP_DIAG_EG_DROP_REASON_TRILL_SAME_INTERFACE     SOC_PPC_DIAG_EG_DROP_REASON_TRILL_SAME_INTERFACE
#define ARAD_PP_DIAG_EG_DROP_REASON_BOUNCE_BACK              SOC_PPC_DIAG_EG_DROP_REASON_BOUNCE_BACK
#define ARAD_PP_DIAG_EG_DROP_REASON_ILLEGAL_EEP              SOC_PPC_DIAG_EG_DROP_REASON_ILLEGAL_EEP
#define ARAD_PP_DIAG_EG_DROP_REASON_ILLEGAL_EEI                SOC_PPC_DIAG_EG_DROP_REASON_ILLEGAL_EEI
#define ARAD_PP_DIAG_EG_DROP_REASON_PHP_CONFLICT               SOC_PPC_DIAG_EG_DROP_REASON_PHP_CONFLICT
#define ARAD_PP_DIAG_EG_DROP_REASON_POP_IP_VERSION_ERR         SOC_PPC_DIAG_EG_DROP_REASON_POP_IP_VERSION_ERR
#define ARAD_PP_DIAG_EG_DROP_REASON_MODIFY_SNOOPED_PACKET      SOC_PPC_DIAG_EG_DROP_REASON_MODIFY_SNOOPED_PACKET
#define ARAD_PP_DIAG_EG_DROP_REASON_IP_TUNNEL_SNOOPED_PACKET   SOC_PPC_DIAG_EG_DROP_REASON_IP_TUNNEL_SNOOPED_PACKET
#define ARAD_PP_DIAG_EG_DROP_REASON_MODIFY_SNOOPED_PACKET      SOC_PPC_DIAG_EG_DROP_REASON_MODIFY_SNOOPED_PACKET
#define ARAD_PP_DIAG_EG_DROP_REASON_MODIFY_SNOOPED_PACKET      SOC_PPC_DIAG_EG_DROP_REASON_MODIFY_SNOOPED_PACKET
#define ARAD_PP_DIAG_EG_DROP_REASON_EEDB_LINK_LAYER_ENTRY_NEEDED     SOC_PPC_DIAG_EG_DROP_REASON_EEDB_LINK_LAYER_ENTRY_NEEDED
#define ARAD_PP_DIAG_EG_DROP_REASON_STP_BLOCK                  SOC_PPC_DIAG_EG_DROP_REASON_STP_BLOCK
#define ARAD_PP_DIAG_EG_DROP_REASON_OUT_LIF_WITH_DROP          SOC_PPC_DIAG_EG_DROP_REASON_OUT_LIF_WITH_DROP
#define ARAD_PP_DIAG_EG_DROP_REASON_EEDB_LAST_ENTRY_NOT_AC     SOC_PPC_DIAG_EG_DROP_REASON_EEDB_LAST_ENTRY_NOT_AC
#define ARAD_PP_DIAG_EG_DROP_REASON_OTM_INVALID                SOC_PPC_DIAG_EG_DROP_REASON_OTM_INVALID
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV4_VERSION_ERR           SOC_PPC_DIAG_EG_DROP_REASON_IPV4_VERSION_ERR
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV6_VERSION_ERR           SOC_PPC_DIAG_EG_DROP_REASON_IPV6_VERSION_ERR
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV4_CHECKSUM_ERR          SOC_PPC_DIAG_EG_DROP_REASON_IPV4_CHECKSUM_ERR
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV4_IHL_SHORT             SOC_PPC_DIAG_EG_DROP_REASON_IPV4_IHL_SHORT
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV4_TOTAL_LEGNTH          SOC_PPC_DIAG_EG_DROP_REASON_IPV4_TOTAL_LEGNTH
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV4_TTL_1                 SOC_PPC_DIAG_EG_DROP_REASON_IPV4_TTL_1
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV6_TTL_1                 SOC_PPC_DIAG_EG_DROP_REASON_IPV6_TTL_1
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV4_WITH_OPTIONS          SOC_PPC_DIAG_EG_DROP_REASON_IPV4_WITH_OPTIONS
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV4_TTL_0                 SOC_PPC_DIAG_EG_DROP_REASON_IPV4_TTL_0
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV6_TTL_0                 SOC_PPC_DIAG_EG_DROP_REASON_IPV6_TTL_0
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV4_SIP_EQUAL_DIP         SOC_PPC_DIAG_EG_DROP_REASON_IPV4_SIP_EQUAL_DIP
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV4_DIP_IS_ZERO           SOC_PPC_DIAG_EG_DROP_REASON_IPV4_DIP_IS_ZERO
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV4_SIP_IS_MC             SOC_PPC_DIAG_EG_DROP_REASON_IPV4_SIP_IS_MC
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV6_SIP_IS_MC             SOC_PPC_DIAG_EG_DROP_REASON_IPV6_SIP_IS_MC
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV6_DIP_UNSPECIFIED       SOC_PPC_DIAG_EG_DROP_REASON_IPV6_DIP_UNSPECIFIED
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV6_SIP_UNSPECIFIED       SOC_PPC_DIAG_EG_DROP_REASON_IPV6_SIP_UNSPECIFIED
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV6_LOOPBACK              SOC_PPC_DIAG_EG_DROP_REASON_IPV6_LOOPBACK
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV6_HOP_BY_HOP            SOC_PPC_DIAG_EG_DROP_REASON_IPV6_HOP_BY_HOP
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV6_LINK_LOCAL_DEST       SOC_PPC_DIAG_EG_DROP_REASON_IPV6_LINK_LOCAL_DEST
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV6_SITE_LOCAL_DEST       SOC_PPC_DIAG_EG_DROP_REASON_IPV6_SITE_LOCAL_DEST
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV6_LINK_LOCAL_SRC        SOC_PPC_DIAG_EG_DROP_REASON_IPV6_LINK_LOCAL_SRC
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV6_SITE_LOCAL_SRC        SOC_PPC_DIAG_EG_DROP_REASON_IPV6_SITE_LOCAL_SRC
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV6_IPV4_COMPATIBLE       SOC_PPC_DIAG_EG_DROP_REASON_IPV6_IPV4_COMPATIBLE
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV6_IPV4_MAPPED           SOC_PPC_DIAG_EG_DROP_REASON_IPV6_IPV4_MAPPED
#define ARAD_PP_DIAG_EG_DROP_REASON_IPV6_DEST_MC               SOC_PPC_DIAG_EG_DROP_REASON_IPV6_DEST_MC
#define ARAD_PP_NOF_DIAG_EG_DROP_REASONS                     SOC_PPC_NOF_DIAG_EG_DROP_REASONS
typedef SOC_PPC_DIAG_EG_DROP_REASON                            ARAD_PP_DIAG_EG_DROP_REASON;

#define ARAD_PP_DIAG_PKT_TM_FIELD_TC                         	SOC_PPC_DIAG_PKT_TM_FIELD_TC
#define ARAD_PP_DIAG_PKT_TM_FIELD_DP                         	SOC_PPC_DIAG_PKT_TM_FIELD_DP
#define ARAD_PP_DIAG_PKT_TM_FIELD_DEST                       	SOC_PPC_DIAG_PKT_TM_FIELD_DEST
#define ARAD_PP_DIAG_PKT_TM_FIELD_METER1                     	SOC_PPC_DIAG_PKT_TM_FIELD_METER1
#define ARAD_PP_DIAG_PKT_TM_FIELD_METER2                     	SOC_PPC_DIAG_PKT_TM_FIELD_METER2
#define ARAD_PP_DIAG_PKT_TM_FIELD_MTR_CMD                    	SOC_PPC_DIAG_PKT_TM_FIELD_MTR_CMD
#define ARAD_PP_DIAG_PKT_TM_FIELD_COUNTER1                   	SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER1
#define ARAD_PP_DIAG_PKT_TM_FIELD_COUNTER2                   	SOC_PPC_DIAG_PKT_TM_FIELD_COUNTER2
#define ARAD_PP_DIAG_PKT_TM_FIELD_CUD                        	SOC_PPC_DIAG_PKT_TM_FIELD_CUD
#define ARAD_PP_DIAG_PKT_TM_FIELD_ETH_METER_PTR                	(SOC_PPC_DIAG_PKT_TM_FIELD_ETH_METER_PTR)
#define ARAD_PP_DIAG_PKT_TM_FIELD_ING_SHAPING_DA               	(SOC_PPC_DIAG_PKT_TM_FIELD_ING_SHAPING_DA)
#define ARAD_PP_DIAG_PKT_TM_FIELD_ECN_CAPABLE                   (SOC_PPC_DIAG_PKT_TM_FIELD_ECN_CAPABLE)
#define ARAD_PP_DIAG_PKT_TM_FIELD_CNI                           (SOC_PPC_DIAG_PKT_TM_FIELD_CNI)
#define ARAD_PP_DIAG_PKT_TM_FIELD_DA_TYPE                      	(SOC_PPC_DIAG_PKT_TM_FIELD_DA_TYPE)
#define ARAD_PP_DIAG_PKT_TM_FIELD_ST_VSQ_PTR                   	(SOC_PPC_DIAG_PKT_TM_FIELD_ST_VSQ_PTR)
#define ARAD_PP_DIAG_PKT_TM_FIELD_LAG_LB_KEY                   	(SOC_PPC_DIAG_PKT_TM_FIELD_LAG_LB_KEY)
#define ARAD_PP_DIAG_PKT_TM_FIELD_IGNORE_CP                    	(SOC_PPC_DIAG_PKT_TM_FIELD_IGNORE_CP)
#define ARAD_PP_DIAG_PKT_TM_FIELD_SNOOP_ID                    	(SOC_PPC_DIAG_PKT_TM_FIELD_SNOOP_ID)
#define ARAD_PP_NOF_DIAG_PKT_TM_FIELDS                       	SOC_PPC_NOF_DIAG_PKT_TM_FIELDS
typedef SOC_PPC_DIAG_PKT_TM_FIELD                              	ARAD_PP_DIAG_PKT_TM_FIELD;

typedef SOC_PPC_DIAG_BUFFER                                    ARAD_PP_DIAG_BUFFER;
typedef SOC_PPC_DIAG_MODE_INFO                                 ARAD_PP_DIAG_MODE_INFO;
typedef SOC_PPC_DIAG_VSI_INFO                                  ARAD_PP_DIAG_VSI_INFO;
typedef SOC_PPC_DIAG_DB_USE_INFO                               ARAD_PP_DIAG_DB_USE_INFO;
typedef SOC_PPC_DIAG_TCAM_USE_INFO                             ARAD_PP_DIAG_TCAM_USE_INFO;
typedef SOC_PPC_DIAG_RECEIVED_PACKET_INFO                      ARAD_PP_DIAG_RECEIVED_PACKET_INFO;
typedef SOC_PPC_DIAG_TRAP_PACKET_INFO                          ARAD_PP_DIAG_TRAP_PACKET_INFO;
typedef SOC_PPC_DIAG_TRAP_INFO                                 ARAD_PP_DIAG_TRAP_INFO;
typedef SOC_PPC_DIAG_SNOOP_INFO                                ARAD_PP_DIAG_SNOOP_INFO;
typedef SOC_PPC_DIAG_TRAPS_INFO                                ARAD_PP_DIAG_TRAPS_INFO;
typedef SOC_PPC_DIAG_TRAP_TO_CPU_INFO                          ARAD_PP_DIAG_TRAP_TO_CPU_INFO;
typedef SOC_PPC_DIAG_IPV4_VPN_ROUTE_KEY                        ARAD_PP_DIAG_IPV4_VPN_ROUTE_KEY;
typedef SOC_PPC_DIAG_LEM_KEY                                   ARAD_PP_DIAG_LEM_KEY;
typedef SOC_PPC_DIAG_LIF_KEY                                   ARAD_PP_DIAG_LIF_KEY;
typedef SOC_PPC_DIAG_LIF_VALUE                                 ARAD_PP_DIAG_LIF_VALUE;
typedef SOC_PPC_DIAG_LIF_LKUP_INFO                             ARAD_PP_DIAG_LIF_LKUP_INFO;
typedef SOC_PPC_DIAG_TCAM_KEY                                  ARAD_PP_DIAG_TCAM_KEY;
typedef SOC_PPC_DIAG_TCAM_VALUE                                ARAD_PP_DIAG_TCAM_VALUE;
typedef SOC_PPC_DIAG_LEM_VALUE                                 ARAD_PP_DIAG_LEM_VALUE;
typedef SOC_PPC_DIAG_PARSING_L2_INFO                           ARAD_PP_DIAG_PARSING_L2_INFO;
typedef SOC_PPC_DIAG_PARSING_MPLS_INFO                         ARAD_PP_DIAG_PARSING_MPLS_INFO;
typedef SOC_PPC_DIAG_PARSING_IP_INFO                           ARAD_PP_DIAG_PARSING_IP_INFO;
typedef SOC_PPC_DIAG_PARSING_HEADER_INFO                       ARAD_PP_DIAG_PARSING_HEADER_INFO;
typedef SOC_PPC_DIAG_PARSING_INFO                              ARAD_PP_DIAG_PARSING_INFO;
typedef SOC_PPC_DIAG_TERM_MPLS_LABEL_INFO                      ARAD_PP_DIAG_TERM_MPLS_LABEL_INFO;
typedef SOC_PPC_DIAG_TERMINATED_MPLS_LABEL_INFO                ARAD_PP_DIAG_TERMINATED_MPLS_LABEL_INFO;
typedef SOC_PPC_DIAG_TERM_MPLS_INFO                            ARAD_PP_DIAG_TERM_MPLS_INFO;
typedef SOC_PPC_DIAG_TERM_INFO                                 ARAD_PP_DIAG_TERM_INFO;
typedef SOC_PPC_DIAG_IPV4_VPN_KEY                              ARAD_PP_DIAG_IPV4_VPN_KEY;
typedef SOC_PPC_DIAG_IPV6_VPN_KEY                              ARAD_PP_DIAG_IPV6_VPN_KEY;
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
typedef SOC_PPC_DIAG_IPV4_UNICAST_RPF                          ARAD_PP_DIAG_IPV4_UNICAST_RPF;
typedef SOC_PPC_DIAG_IPV4_MULTICAST                            ARAD_PP_DIAG_IPV4_MULTICAST;
typedef SOC_PPC_DIAG_IPV6_UNICAST_RPF                          ARAD_PP_DIAG_IPV6_UNICAST_RPF;
typedef SOC_PPC_DIAG_IPV6_MULTICAST                            ARAD_PP_DIAG_IPV6_MULTICAST;
typedef SOC_PPC_DIAG_MPLS                                      ARAD_PP_DIAG_MPLS;
typedef SOC_PPC_DIAG_TRILL_UNICAST                             ARAD_PP_DIAG_TRILL_UNICAST;
typedef SOC_PPC_DIAG_TRILL_MULTICAST                           ARAD_PP_DIAG_TRILL_MULTICAST;
typedef SOC_PPC_DIAG_IP_REPLY_RECORD                           ARAD_PP_DIAG_IP_REPLY_RECORD;
typedef SOC_PPC_DIAG_SECOND_IP_REPLY_RECORD                    ARAD_PP_DIAG_SECOND_IP_REPLY_RECORD;
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
typedef SOC_PPC_DIAG_FRWRD_LKUP_KEY                            ARAD_PP_DIAG_FRWRD_LKUP_KEY;
typedef SOC_PPC_DIAG_FRWRD_LKUP_VALUE                          ARAD_PP_DIAG_FRWRD_LKUP_VALUE;
typedef SOC_PPC_DIAG_FRWRD_LKUP_INFO                           ARAD_PP_DIAG_FRWRD_LKUP_INFO;
typedef SOC_PPC_DIAG_LEARN_INFO                                ARAD_PP_DIAG_LEARN_INFO;
typedef SOC_PPC_DIAG_VLAN_EDIT_RES                             ARAD_PP_DIAG_VLAN_EDIT_RES;
typedef SOC_PPC_DIAG_FRWRD_DECISION_INFO                       ARAD_PP_DIAG_FRWRD_DECISION_INFO;
typedef SOC_PPC_DIAG_FRWRD_DECISION_TRACE_INFO                 ARAD_PP_DIAG_FRWRD_DECISION_TRACE_INFO;
typedef SOC_PPC_DIAG_PKT_TM_INFO                               ARAD_PP_DIAG_PKT_TM_INFO;
typedef SOC_PPC_DIAG_ENCAP_INFO                                ARAD_PP_DIAG_ENCAP_INFO;
typedef SOC_PPC_DIAG_EG_DROP_LOG_INFO                          ARAD_PP_DIAG_EG_DROP_LOG_INFO;
typedef SOC_PPC_DIAG_ETH_PACKET_INFO                           ARAD_PP_DIAG_ETH_PACKET_INFO;
typedef SOC_PPC_DIAG_PACKET_INFO                               ARAD_PP_DIAG_PACKET_INFO;
typedef SOC_PPC_DIAG_EGRESS_VLAN_EDIT_RES                      ARAD_PP_DIAG_EGRESS_VLAN_EDIT_RES;

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
 *   arad_pp_diag_sample_enable_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable/disable diagnostic APIs.affects only APIs with
 *   type: need_sample
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                                 enable -
 *     TRUE: diag APIs are enabled, FALSE diag APIs are
 *     disabled.
 * REMARKS:
 *   - when enabled will affect device power consuming
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_sample_enable_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                 enable
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_diag_sample_enable_set" API.
 *     Refer to "arad_pp_diag_sample_enable_set" API for details.
*********************************************************************/
uint32
  arad_pp_diag_sample_enable_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint8                                 *enable
  );

/*********************************************************************
* NAME:
 *   arad_pp_diag_mode_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the mode configuration for diag module, including
 *   diag-flavor
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_DIAG_MODE_INFO                      *mode_info -
 *     Mode of diagnsotcis
 * REMARKS:
 *   Diag Type: All-Packets
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_mode_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_DIAG_MODE_INFO                      *mode_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_diag_mode_info_set" API.
 *     Refer to "arad_pp_diag_mode_info_set" API for details.
*********************************************************************/
uint32
  arad_pp_diag_mode_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_DIAG_MODE_INFO                      *mode_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_diag_vsi_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the VSI information of the last packet
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_PP_DIAG_VSI_INFO                 *vsi_info -
 *     Information regarding the Last packet VSI
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_vsi_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_VSI_INFO                      *vsi_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_diag_pkt_trace_clear
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Clear the trace of transmitted packet, so next trace
 *   info will relate to next packets to transmit
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  pkt_trace -
 *     Packet traces type as encoded by SOC_PPD_DIAG_PKT_TRACE.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_pkt_trace_clear(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  uint32                                  pkt_trace
  );

/*********************************************************************
* NAME:
 *   arad_pp_diag_received_packet_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the header of last received packet entered the
 *   device and the its association to TM/system/PP ports.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_PP_DIAG_RECEIVED_PACKET_INFO           *rcvd_pkt_info -
 *     Information regards Last received packet
 * REMARKS:
 *   Diag Type: Last-Packet, need-sampleif sample is disabled
 *   then: last-packet, clear-on-read
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_received_packet_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_RECEIVED_PACKET_INFO           *rcvd_pkt_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_diag_parsing_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns network headers qualified on packet upon
 *   parsing, including packet format, L2 headers fields,...
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_PP_DIAG_PARSING_INFO                   *pars_info -
 *     Information obtained from parsing including L2 headers,
 *     packet format,...
 * REMARKS:
 *   Diag Type: Last Packet, need-sample
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_parsing_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_PARSING_INFO                   *pars_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_diag_termination_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns information obtained by termination including
 *   terminated headers
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_PP_DIAG_TERM_INFO                      *term_info -
 *     Includes terminated headers,
 * REMARKS:
 *   Diag Type: Last Packet, need-sample
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_termination_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_TERM_INFO                      *term_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_diag_frwrd_lkup_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the forwarding lookup performed including:
 *   forwarding type (bridging, routing, ILM, ...), the key
 *   used for the lookup and the result of the lookup
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_PP_DIAG_FRWRD_LKUP_INFO                *frwrd_info -
 *     forwarding lookup information including key and result
 * REMARKS:
 *   Diag Type: Last Packet, need-sample
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_frwrd_lkup_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_FRWRD_LKUP_INFO                *frwrd_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_diag_frwrd_lpm_lkup_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Simulate IP lookup in the device tables and return
 *   FEC-pointer
 * INPUT:
 *   SOC_SAND_IN  int                            unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_DIAG_IPV4_VPN_KEY              *lpm_key -
 *     forwarding lookup information including key and result
 *   SOC_SAND_OUT uint32                             *fec_ptr -
 *     FEC pointer
 *   SOC_SAND_OUT uint8                            *found -
 *     Was key found
 * REMARKS:
 *   Diag Type: lookup a key, don't need-sample
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_frwrd_lpm_lkup_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core_id,
    SOC_SAND_IN  ARAD_PP_DIAG_IPV4_VPN_KEY              *lpm_key,
    SOC_SAND_OUT uint32                             *fec_ptr,
    SOC_SAND_OUT uint8                            *found
  );
/*********************************************************************
* NAME:
 *   arad_pp_diag_traps_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns information regard packet trapping/snooping,
 *   including which traps/snoops were fulfilled, which
 *   trap/snoop was committed, and whether packet was
 *   forwarded/processed according to trap or according to
 *   normal packet processing flow.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_PP_DIAG_TRAPS_INFO                     *traps_info -
 *     Information regarding the trapping
 * REMARKS:
 *   Diag Type: Last Packet, Clear-on-read. When called after
 *   injecting more than one packet then 'trap_stack' will
 *   hold the status for all injected packets from last call.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_traps_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_TRAPS_INFO                     *traps_info
  );
/*********************************************************************
* NAME:
 *   arad_pp_diag_frwrd_decision_trace_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Return the trace (changes) for forwarding decision for
 *   last packet in several phases in processing
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_PP_DIAG_FRWRD_DECISION_TRACE_INFO      *frwrd_trace_info -
 *     Forwarding decision in several phases in the processing
 * REMARKS:
 *   Diag Type: Last Packet, Need-sample.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_frwrd_decision_trace_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_FRWRD_DECISION_TRACE_INFO      *frwrd_trace_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_diag_learning_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the information to be learned for the incoming
 *   packet. This is the information that the processing
 *   determine to be learned, the MACT supposed to learn this
 *   information.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_PP_DIAG_LEARN_INFO                     *learn_info -
 *     Learning information including key and value
 *     <destination and additional info (AC, EEP, MPLS command
 *     etc...)>
 * REMARKS:
 *   Diag Type: Last-Packet. need-sample.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_learning_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_LEARN_INFO                     *learn_info
  );


/*********************************************************************
* NAME:
 *   arad_pp_diag_ing_vlan_edit_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the result of ingress vlan editing,
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_PP_DIAG_VLAN_EDIT_RES                  *vec_res -
 *     Vlan edit command result, removed tags and build tags
 * REMARKS:
 *   Diag Type: Last Packet. need-sample.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_ing_vlan_edit_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_VLAN_EDIT_RES                  *vec_res
  );

/*********************************************************************
* NAME:
 *   arad_pp_diag_pkt_associated_tm_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   The Traffic management information associated with the
 *   packet including meter, DP, TC, etc...
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_PP_DIAG_PKT_TM_INFO                    *pkt_tm_info -
 *     Traffic management information associated with the
 *     packet
 * REMARKS:
 *   Diag Type: Last Packet. need-sample.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_pkt_associated_tm_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_PKT_TM_INFO                    *pkt_tm_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_diag_encap_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the encapsulation and editing information applied to
 *   last packet
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_PP_DIAG_ENCAP_INFO                     *encap_info -
 *     SOC_SAND_OUT SOC_PPD_DIAG_ENCAP_INFO *encap_info
 * REMARKS:
 *   Diag Type: Last Packet. need-sample.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_encap_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_OUT ARAD_PP_DIAG_ENCAP_INFO                     *encap_info
  );
/*********************************************************************
* NAME:
 *   arad_pp_diag_eg_drop_log_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the reason for packet discard
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_PP_DIAG_EG_DROP_LOG_INFO               *eg_drop_log -
 *     Egress drop log, reason why packets were dropped.
 * REMARKS:
 *   Diag Type: all Packets since last clear.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_eg_drop_log_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_DIAG_EG_DROP_LOG_INFO               *eg_drop_log
  );

/*********************************************************************
* NAME:
 *   arad_pp_diag_db_lif_lkup_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the lookup key and result used in the LIF DB
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_DIAG_DB_USE_INFO                    *db_info -
 *     The specific use of the DB, for example the lookup
 *     number
 *   SOC_SAND_OUT ARAD_PP_DIAG_LIF_LKUP_INFO                  *lkup_info -
 *     Lookup information, key and result
 * REMARKS:
 *   Diag Type: Last Packet. need-sample.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_db_lif_lkup_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_DIAG_DB_USE_INFO                    *db_info,
    SOC_SAND_OUT ARAD_PP_DIAG_LIF_LKUP_INFO                  *lkup_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_diag_db_lem_lkup_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the lookup key and result used in the LEM DB
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_DIAG_DB_USE_INFO                    *db_info -
 *     The specific use of the DB, for example the lookup
 *     number
 *   SOC_SAND_OUT ARAD_PP_DIAG_LEM_LKUP_TYPE                  *type -
 *     Lookup type
 *   SOC_SAND_OUT ARAD_PP_DIAG_LEM_KEY                        *key -
 *     Lookup key
 *   SOC_SAND_OUT ARAD_PP_DIAG_LEM_VALUE                      *val -
 *     Lookup result
 *   SOC_SAND_OUT uint8                                 *valid -
 *     Is lookup result valid.
 * REMARKS:
 *   Diag Type: Last Packet. need-sample.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_db_lem_lkup_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_DIAG_DB_USE_INFO                    *db_info,
    SOC_SAND_OUT ARAD_PP_DIAG_LEM_LKUP_TYPE                  *type,
    SOC_SAND_OUT ARAD_PP_DIAG_LEM_KEY                        *key,
    SOC_SAND_OUT ARAD_PP_DIAG_LEM_VALUE                      *val,
    SOC_SAND_OUT uint8                                 *valid
  );


/*********************************************************************
* NAME:
 *   arad_pp_diag_egress_vlan_edit_info_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Return the egress vlan edit command informations
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_PP_DIAG_EGRESS_VLAN_EDIT_RES           *prm_vec_res -
 *     
 * REMARKS:
 *   Diag Type: Last Packet. need-sample.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_diag_egress_vlan_edit_info_get (
     SOC_SAND_IN  int                                 unit,
     SOC_SAND_IN  int                                 core_id,
     SOC_SAND_OUT ARAD_PP_DIAG_EGRESS_VLAN_EDIT_RES           *prm_vec_res
  );



void
  ARAD_PP_DIAG_BUFFER_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_BUFFER *info
  );

void
  ARAD_PP_DIAG_MODE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_MODE_INFO *info
  );

void
  ARAD_PP_DIAG_VSI_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_VSI_INFO *info
  );

void
  ARAD_PP_DIAG_DB_USE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_DB_USE_INFO *info
  );

void
  ARAD_PP_DIAG_TCAM_USE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_TCAM_USE_INFO *info
  );

void
  ARAD_PP_DIAG_RECEIVED_PACKET_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_RECEIVED_PACKET_INFO *info
  );

void
  ARAD_PP_DIAG_TRAP_PACKET_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_TRAP_PACKET_INFO *info
  );

void
  ARAD_PP_DIAG_TRAP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_TRAP_INFO *info
  );

void
  ARAD_PP_DIAG_SNOOP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_SNOOP_INFO *info
  );

void
  ARAD_PP_DIAG_TRAPS_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_TRAPS_INFO *info
  );

void
  ARAD_PP_DIAG_TRAP_TO_CPU_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_TRAP_TO_CPU_INFO *info
  );

void
  ARAD_PP_DIAG_LEM_KEY_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_LEM_KEY *info
  );

void
  ARAD_PP_DIAG_LIF_KEY_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_LIF_KEY *info
  );

void
  ARAD_PP_DIAG_LIF_VALUE_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_LIF_VALUE *info
  );

void
  ARAD_PP_DIAG_LIF_LKUP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_LIF_LKUP_INFO *info
  );

void
  ARAD_PP_DIAG_TCAM_KEY_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_TCAM_KEY *info
  );

void
  ARAD_PP_DIAG_TCAM_VALUE_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_TCAM_VALUE *info
  );

void
  ARAD_PP_DIAG_LEM_VALUE_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_LEM_VALUE *info
  );

void
  ARAD_PP_DIAG_PARSING_L2_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_PARSING_L2_INFO *info
  );

void
  ARAD_PP_DIAG_PARSING_MPLS_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_PARSING_MPLS_INFO *info
  );

void
  ARAD_PP_DIAG_PARSING_IP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_PARSING_IP_INFO *info
  );

void
  ARAD_PP_DIAG_PARSING_HEADER_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_PARSING_HEADER_INFO *info
  );

void
  ARAD_PP_DIAG_PARSING_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_PARSING_INFO *info
  );

void
  ARAD_PP_DIAG_TERM_MPLS_LABEL_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_TERM_MPLS_LABEL_INFO *info
  );

void
  ARAD_PP_DIAG_TERMINATED_MPLS_LABEL_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_TERMINATED_MPLS_LABEL_INFO *info
  );

void
  ARAD_PP_DIAG_TERM_MPLS_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_TERM_MPLS_INFO *info
  );

void
  ARAD_PP_DIAG_TERM_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_TERM_INFO *info
  );

void
  ARAD_PP_DIAG_IPV4_VPN_KEY_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_IPV4_VPN_KEY *info
  );
void
  ARAD_PP_DIAG_IPV6_VPN_KEY_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_IPV6_VPN_KEY *info
  );
#if defined(INCLUDE_KBP) && !defined(BCM_88030_A0)
void
  ARAD_PP_DIAG_IPV4_UNICAST_RPF_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_IPV4_UNICAST_RPF *info
  );

void
  ARAD_PP_DIAG_IPV4_MULTICAST_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_IPV4_MULTICAST *info
  );

void
  ARAD_PP_DIAG_IPV6_UNICAST_RPF_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_IPV6_UNICAST_RPF *info
  );

void
  ARAD_PP_DIAG_IPV6_MULTICAST_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_IPV6_MULTICAST *info
  );

void
  ARAD_PP_DIAG_MPLS_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_MPLS *info
  );

void
  ARAD_PP_DIAG_TRILL_UNICAST_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_TRILL_UNICAST *info
  );

void
  ARAD_PP_DIAG_TRILL_MULTICAST_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_TRILL_MULTICAST *info
  );

void
  ARAD_PP_DIAG_IP_REPLY_RECORD_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_IP_REPLY_RECORD *info
  );

void
  ARAD_PP_DIAG_SECOND_IP_REPLY_RECORD_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_SECOND_IP_REPLY_RECORD *info
  );
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030_A0) */
void
  ARAD_PP_DIAG_FRWRD_LKUP_KEY_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_FRWRD_LKUP_KEY *info
  );

void
  ARAD_PP_DIAG_FRWRD_LKUP_VALUE_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_FRWRD_LKUP_VALUE *info
  );

void
  ARAD_PP_DIAG_FRWRD_LKUP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_FRWRD_LKUP_INFO *info
  );

void
  ARAD_PP_DIAG_LEARN_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_LEARN_INFO *info
  );

void
  ARAD_PP_DIAG_VLAN_EDIT_RES_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_VLAN_EDIT_RES *info
  );

void
  ARAD_PP_DIAG_FRWRD_DECISION_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_FRWRD_DECISION_INFO *info
  );

void
  ARAD_PP_DIAG_FRWRD_DECISION_TRACE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_FRWRD_DECISION_TRACE_INFO *info
  );

void
  ARAD_PP_DIAG_PKT_TM_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_PKT_TM_INFO *info
  );

void
  ARAD_PP_DIAG_ENCAP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_ENCAP_INFO *info
  );

void
  ARAD_PP_DIAG_EG_DROP_LOG_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_EG_DROP_LOG_INFO *info
  );

void
  ARAD_PP_DIAG_ETH_PACKET_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_ETH_PACKET_INFO *info
  );

void
  ARAD_PP_DIAG_PACKET_INFO_clear(
    SOC_SAND_OUT ARAD_PP_DIAG_PACKET_INFO *info
  );

void 
  ARAD_PP_DIAG_EGRESS_VLAN_EDIT_RES_clear(
     SOC_SAND_OUT ARAD_PP_DIAG_EGRESS_VLAN_EDIT_RES *prm_vec_res
  );  

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_DIAG_FWD_LKUP_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_DIAG_FWD_LKUP_TYPE enum_val
  );

const char*
  ARAD_PP_DIAG_TCAM_USAGE_to_string(
    SOC_SAND_IN  ARAD_PP_DIAG_TCAM_USAGE enum_val
  );

const char*
  ARAD_PP_DIAG_FLAVOR_to_string(
    SOC_SAND_IN  ARAD_PP_DIAG_FLAVOR enum_val
  );

const char*
  ARAD_PP_DIAG_LEM_LKUP_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_DIAG_LEM_LKUP_TYPE enum_val
  );

const char*
  ARAD_PP_DIAG_LIF_LKUP_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_DIAG_LIF_LKUP_TYPE enum_val
  );

const char*
  ARAD_PP_DIAG_DB_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_DIAG_DB_TYPE enum_val
  );

const char*
  ARAD_PP_DIAG_FRWRD_DECISION_PHASE_to_string(
    SOC_SAND_IN  ARAD_PP_DIAG_FRWRD_DECISION_PHASE enum_val
  );

const char*
  ARAD_PP_DIAG_MPLS_TERM_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_DIAG_MPLS_TERM_TYPE enum_val
  );

const char*
  ARAD_PP_DIAG_PKT_TRACE_to_string(
    SOC_SAND_IN  ARAD_PP_DIAG_PKT_TRACE enum_val
  );

const char*
  ARAD_PP_DIAG_EG_DROP_REASON_to_string(
    SOC_SAND_IN  ARAD_PP_DIAG_EG_DROP_REASON enum_val
  );

const char*
  ARAD_PP_DIAG_PKT_TM_FIELD_to_string(
    SOC_SAND_IN  ARAD_PP_DIAG_PKT_TM_FIELD enum_val
  );

void
  ARAD_PP_DIAG_BUFFER_print(
    SOC_SAND_IN  ARAD_PP_DIAG_BUFFER *info
  );

void
  ARAD_PP_DIAG_MODE_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_MODE_INFO *info
  );

void
  ARAD_PP_DIAG_DB_USE_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_DB_USE_INFO *info
  );

void
  ARAD_PP_DIAG_TCAM_USE_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_TCAM_USE_INFO *info
  );

void
  ARAD_PP_DIAG_RECEIVED_PACKET_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_RECEIVED_PACKET_INFO *info
  );

void
  ARAD_PP_DIAG_TRAP_PACKET_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_TRAP_PACKET_INFO *info
  );

void
  ARAD_PP_DIAG_TRAP_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_TRAP_INFO *info
  );

void
  ARAD_PP_DIAG_SNOOP_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_SNOOP_INFO *info
  );

void
  ARAD_PP_DIAG_TRAPS_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_TRAPS_INFO *info
  );

void
  ARAD_PP_DIAG_TRAP_TO_CPU_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_TRAP_TO_CPU_INFO *info
  );

void
  ARAD_PP_DIAG_LEM_KEY_print(
    SOC_SAND_IN  ARAD_PP_DIAG_LEM_KEY *info,
    SOC_SAND_IN  ARAD_PP_DIAG_LEM_LKUP_TYPE type
  );

void
  ARAD_PP_DIAG_LIF_KEY_print(
    SOC_SAND_IN  ARAD_PP_DIAG_LIF_KEY *info,
    SOC_SAND_IN  ARAD_PP_DIAG_LIF_LKUP_TYPE lkup_type
  );

void
  ARAD_PP_DIAG_LIF_VALUE_print(
    SOC_SAND_IN  ARAD_PP_DIAG_LIF_VALUE *info,
    SOC_SAND_IN  ARAD_PP_DIAG_LIF_LKUP_TYPE lkup_type
  );

void
  ARAD_PP_DIAG_LIF_LKUP_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_LIF_LKUP_INFO *info
  );

void
  ARAD_PP_DIAG_TCAM_KEY_print(
    SOC_SAND_IN  ARAD_PP_DIAG_TCAM_KEY *info
  );

void
  ARAD_PP_DIAG_TCAM_VALUE_print(
    SOC_SAND_IN  ARAD_PP_DIAG_TCAM_VALUE *info
  );

void
  ARAD_PP_DIAG_LEM_VALUE_print(
    SOC_SAND_IN  ARAD_PP_DIAG_LEM_VALUE *info,
    SOC_SAND_IN  ARAD_PP_DIAG_LEM_LKUP_TYPE type
  );

void
  ARAD_PP_DIAG_PARSING_L2_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_PARSING_L2_INFO *info
  );

void
  ARAD_PP_DIAG_PARSING_MPLS_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_PARSING_MPLS_INFO *info
  );

void
  ARAD_PP_DIAG_PARSING_IP_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_PARSING_IP_INFO *info
  );

void
  ARAD_PP_DIAG_PARSING_HEADER_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_PARSING_HEADER_INFO *info
  );

void
  ARAD_PP_DIAG_PARSING_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_PARSING_INFO *info
  );

void
  ARAD_PP_DIAG_TERM_MPLS_LABEL_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_TERM_MPLS_LABEL_INFO *info
  );

void
  ARAD_PP_DIAG_TERMINATED_MPLS_LABEL_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_TERMINATED_MPLS_LABEL_INFO *info
  );

void
  ARAD_PP_DIAG_TERM_MPLS_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_TERM_MPLS_INFO *info
  );

void
  ARAD_PP_DIAG_TERM_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_TERM_INFO *info
  );

void
  ARAD_PP_DIAG_IPV4_VPN_KEY_print(
    SOC_SAND_IN  ARAD_PP_DIAG_IPV4_VPN_KEY *info
  );
void
  ARAD_PP_DIAG_IPV6_VPN_KEY_print(
    SOC_SAND_IN  ARAD_PP_DIAG_IPV6_VPN_KEY *info
  );
void
  ARAD_PP_DIAG_FRWRD_LKUP_KEY_print(
    SOC_SAND_IN  ARAD_PP_DIAG_FRWRD_LKUP_KEY *info,
    SOC_SAND_IN  ARAD_PP_DIAG_FWD_LKUP_TYPE  frwrd_type,
    SOC_SAND_IN  uint8                       is_kbp    
  );

void
  ARAD_PP_DIAG_FRWRD_LKUP_VALUE_print(
    SOC_SAND_IN  ARAD_PP_DIAG_FRWRD_LKUP_VALUE *info,
    SOC_SAND_IN  ARAD_PP_DIAG_FWD_LKUP_TYPE  frwrd_type
  );

void
  ARAD_PP_DIAG_FRWRD_LKUP_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_FRWRD_LKUP_INFO *info
  );

void
  ARAD_PP_DIAG_LEARN_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_LEARN_INFO *info
  );

void
  ARAD_PP_DIAG_VLAN_EDIT_RES_print(
    SOC_SAND_IN  ARAD_PP_DIAG_VLAN_EDIT_RES *info
  );

void
  ARAD_PP_DIAG_FRWRD_DECISION_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_FRWRD_DECISION_INFO *info
  );

void
  ARAD_PP_DIAG_FRWRD_DECISION_TRACE_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_FRWRD_DECISION_TRACE_INFO *info
  );

void
  ARAD_PP_DIAG_PKT_TM_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_PKT_TM_INFO *info
  );

void
  ARAD_PP_DIAG_ENCAP_INFO_print(
    SOC_SAND_IN int                      unit, 
    SOC_SAND_IN  ARAD_PP_DIAG_ENCAP_INFO *info
  );

void
  ARAD_PP_DIAG_EG_DROP_LOG_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_EG_DROP_LOG_INFO *info
  );

void
  ARAD_PP_DIAG_ETH_PACKET_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_ETH_PACKET_INFO *info
  );

void
  ARAD_PP_DIAG_PACKET_INFO_print(
    SOC_SAND_IN  ARAD_PP_DIAG_PACKET_INFO *info
  );

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_DIAG_INCLUDED__*/
#endif


