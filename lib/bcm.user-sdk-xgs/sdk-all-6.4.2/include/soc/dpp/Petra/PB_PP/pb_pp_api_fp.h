/* $Id: pb_pp_api_fp.h,v 1.15 Broadcom SDK $
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

#ifndef __SOC_PB_PP_API_FP_INCLUDED__
/* { */
#define __SOC_PB_PP_API_FP_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_fp.h>
#include <soc/dpp/TMC/tmc_pmf_pgm_mgmt.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_general.h>

#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_fem_tag.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximum number of Qualifier fields per Database.        */
#define  SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX (SOC_PPC_FP_NOF_QUALS_PER_DB_MAX)

/*     Maximum number of Actions per Database.                 */
#define  SOC_PB_PP_FP_NOF_ACTIONS_PER_DB_MAX (SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX)

/*     Maximum number of field values that can be extracted to
 *     build the action value.                                 */
#define  SOC_PB_PP_FP_DIR_EXTR_MAX_NOF_FIELDS (SOC_PPC_FP_DIR_EXTR_MAX_NOF_FIELDS)


#define SOC_PB_PP_FP_NOF_ENTRY_IDS              (((SOC_DPP_NOF_TCAM_BIG_BANKS_PETRAB + SOC_DPP_NOF_TCAM_SMALL_BANKS_PETRAB) * SOC_DPP_NOF_TCAM_BIG_BANK_LINES_PETRAB * 2))

/*     Maximum number of Control values.                       */

/*     Number of User-Defined Fields (UDF)                      */
#define SOC_PB_PP_FP_NOF_HDR_USER_DEFS                             (10)

/*     Number of Egress ACL Databases                          */
#define  SOC_PB_PP_FP_NOF_EGR_ACL_DBS            (8)

/*     Number of Packet-Format-Groups                          */
#define  SOC_PB_PP_FP_NOF_PFGS                   (5)

/*     Number of Tag Action types                              */
#define  SOC_PB_PP_FP_NOF_TAG_ACTION_TYPES        (4)

/*     Maximum number of Databases.                            */
#define  SOC_PB_PP_FP_NOF_DBS (SOC_PPC_FP_NOF_DBS)

/*     Number of cycles in TCAM and Macros.                    */

/*     Number of TCAM Banks.                                   */

/*     Number of Macros.                                       */
#define  SOC_PB_PP_FP_NOF_MACROS (SOC_PPC_FP_NOF_MACROS)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* $Id: pb_pp_api_fp.h,v 1.15 Broadcom SDK $
 *  Set the list of admissible qualifier types for databases based
 *  on predefined keys. Must be called during the Database create
 *  API to set the Database qualifier types.
 */
#define SOC_PB_PP_FP_QUAL_TYPE_PRESET(unit, predefined_key, qual_types)  \
  soc_pb_pp_fp_qual_type_preset(                                              \
          unit,                                                      \
          predefined_key,                                                 \
          qual_types                                                      \
        );
/*
 *  Encode the Forward Decision 'fwd_dec' in a Qualifier value
 *  format. If an entry Qualifier value is set to 'qual_val',
 *  then only packets with this Forwarding Decision are qualified.
 */

/*
 *  Encode the MAC-Address 'mac' and the 'is_valid' mask in a
 *  Qualifier value format. If an entry Qualifier value is set to
 *  'qual_val', then only packets with a MAC-Address equals to 'mac'
 *  - only for bits set by 'is_valid' - are qualified.
 */


/*
 *  Encode the IP Subnet (according to the IP-Address 'ip' and the
 *  'subnet_length') in a Qualifier value format. If an entry
 *  Qualifier value is set to 'qual_val', then only packets with an
 *  IP-Address in the subnet defined by the IP-Address 'ip' and the
 *  'subnet_length' are qualified.
 */

/*
 *   Encode the IP Subnet (according to the IP-Address 'ip' and the
 *  'subnet_length', and also the 'is_low' parameter which sets if it
 *  relates to the 64 LSBs - low part-) in a Qualifier value format.
 *  If an entry Qualifier value is set to 'qual_val', then only packets
 *  with an IP-Address in the subnet defined by the IP-Address 'ip'
 *  and the 'subnet_length' are qualified.
 */
/*
 *  Encode the Ethernet Tag format (according to the parameter
 *  'eth_tag_format') in a Qualifier value format.
 */

/*
 *  Encode the FTMH (according to the parameter
 *  'ftmh') in a Qualifier value format.
 */

/*
 *  Set the list of the egress action types for Egress databases.
 */


/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

#define SOC_PB_PP_FP_DATABASE_STAGE_INGRESS_PMF                  SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF
#define SOC_PB_PP_FP_DATABASE_STAGE_EGRESS                       SOC_PPC_FP_DATABASE_STAGE_EGRESS
typedef SOC_PPC_FP_DATABASE_STAGE                                SOC_PB_PP_FP_DATABASE_STAGE;

typedef SOC_PPC_FP_FTMH                                     SOC_PB_PP_FP_FTMH;

#define SOC_PB_PP_FP_QUAL_HDR_FWD_VLAN_TAG                     SOC_PPC_FP_QUAL_HDR_FWD_VLAN_TAG
#define SOC_PB_PP_FP_QUAL_HDR_FWD_SA                           SOC_PPC_FP_QUAL_HDR_FWD_SA
#define SOC_PB_PP_FP_QUAL_HDR_FWD_DA                           SOC_PPC_FP_QUAL_HDR_FWD_DA
#define SOC_PB_PP_FP_QUAL_HDR_FWD_ETHERTYPE                    SOC_PPC_FP_QUAL_HDR_FWD_ETHERTYPE
#define SOC_PB_PP_FP_QUAL_HDR_FWD_2ND_VLAN_TAG                 SOC_PPC_FP_QUAL_HDR_FWD_2ND_VLAN_TAG
#define SOC_PB_PP_FP_QUAL_HDR_INNER_VLAN_TAG                   SOC_PPC_FP_QUAL_HDR_INNER_VLAN_TAG
#define SOC_PB_PP_FP_QUAL_HDR_INNER_SA                         SOC_PPC_FP_QUAL_HDR_INNER_SA
#define SOC_PB_PP_FP_QUAL_HDR_INNER_DA                         SOC_PPC_FP_QUAL_HDR_INNER_DA
#define SOC_PB_PP_FP_QUAL_HDR_INNER_ETHERTYPE                  SOC_PPC_FP_QUAL_HDR_INNER_ETHERTYPE
#define SOC_PB_PP_FP_QUAL_HDR_INNER_2ND_VLAN_TAG               SOC_PPC_FP_QUAL_HDR_INNER_2ND_VLAN_TAG
#define SOC_PB_PP_FP_QUAL_HDR_VLAN_FORMAT                      SOC_PPC_FP_QUAL_HDR_VLAN_FORMAT
#define SOC_PB_PP_FP_QUAL_HDR_VLAN_TAG                   SOC_PPC_FP_QUAL_HDR_VLAN_TAG
#define SOC_PB_PP_FP_QUAL_HDR_SA                         SOC_PPC_FP_QUAL_HDR_SA
#define SOC_PB_PP_FP_QUAL_HDR_DA                         SOC_PPC_FP_QUAL_HDR_DA
#define SOC_PB_PP_FP_QUAL_HDR_ETHERTYPE                  SOC_PPC_FP_QUAL_HDR_ETHERTYPE
#define SOC_PB_PP_FP_QUAL_HDR_2ND_VLAN_TAG               SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG
#define SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_NEXT_PRTCL              SOC_PPC_FP_QUAL_HDR_FWD_IPV4_NEXT_PRTCL
#define SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_DF                      SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DF
#define SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_MF                      SOC_PPC_FP_QUAL_HDR_FWD_IPV4_MF
#define SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_SIP                     SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP
#define SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_DIP                     SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP
#define SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_SRC_PORT                SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SRC_PORT
#define SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_DEST_PORT               SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DEST_PORT
#define SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_TOS                     SOC_PPC_FP_QUAL_HDR_FWD_IPV4_TOS
#define SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_TCP_CTL                 SOC_PPC_FP_QUAL_HDR_FWD_IPV4_TCP_CTL
#define SOC_PB_PP_FP_QUAL_HDR_FWD_IPV4_IN_VID                  SOC_PPC_FP_QUAL_HDR_FWD_IPV4_IN_VID
#define SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_NEXT_PRTCL            SOC_PPC_FP_QUAL_HDR_INNER_IPV4_NEXT_PRTCL
#define SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_DF                    SOC_PPC_FP_QUAL_HDR_INNER_IPV4_DF
#define SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_MF                    SOC_PPC_FP_QUAL_HDR_INNER_IPV4_MF
#define SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_SIP                   SOC_PPC_FP_QUAL_HDR_INNER_IPV4_SIP
#define SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_DIP                   SOC_PPC_FP_QUAL_HDR_INNER_IPV4_DIP
#define SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_SRC_PORT              SOC_PPC_FP_QUAL_HDR_INNER_IPV4_SRC_PORT
#define SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_DEST_PORT             SOC_PPC_FP_QUAL_HDR_INNER_IPV4_DEST_PORT
#define SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_TOS                   SOC_PPC_FP_QUAL_HDR_INNER_IPV4_TOS
#define SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_TTL                   SOC_PPC_FP_QUAL_HDR_INNER_IPV4_TTL
#define SOC_PB_PP_FP_QUAL_HDR_INNER_IPV4_TCP_CTL               SOC_PPC_FP_QUAL_HDR_INNER_IPV4_TCP_CTL
#define SOC_PB_PP_FP_QUAL_HDR_IPV4_NEXT_PRTCL                  SOC_PPC_FP_QUAL_HDR_IPV4_NEXT_PRTCL
#define SOC_PB_PP_FP_QUAL_HDR_IPV4_DF                          SOC_PPC_FP_QUAL_HDR_IPV4_DF
#define SOC_PB_PP_FP_QUAL_HDR_IPV4_MF                          SOC_PPC_FP_QUAL_HDR_IPV4_MF
#define SOC_PB_PP_FP_QUAL_HDR_IPV4_SIP                         SOC_PPC_FP_QUAL_HDR_IPV4_SIP
#define SOC_PB_PP_FP_QUAL_HDR_IPV4_DIP                         SOC_PPC_FP_QUAL_HDR_IPV4_DIP
#define SOC_PB_PP_FP_QUAL_HDR_IPV4_SRC_PORT                    SOC_PPC_FP_QUAL_HDR_IPV4_SRC_PORT
#define SOC_PB_PP_FP_QUAL_HDR_IPV4_DEST_PORT                   SOC_PPC_FP_QUAL_HDR_IPV4_DEST_PORT
#define SOC_PB_PP_FP_QUAL_HDR_IPV4_TOS                         SOC_PPC_FP_QUAL_HDR_IPV4_TOS
#define SOC_PB_PP_FP_QUAL_HDR_IPV4_TTL                         SOC_PPC_FP_QUAL_HDR_IPV4_TTL
#define SOC_PB_PP_FP_QUAL_HDR_IPV4_TCP_CTL                     SOC_PPC_FP_QUAL_HDR_IPV4_TCP_CTL
#define SOC_PB_PP_FP_QUAL_HDR_IPV4_L4OPS_HI                    SOC_PPC_FP_QUAL_HDR_IPV4_L4OPS_HI
#define SOC_PB_PP_FP_QUAL_HDR_IPV4_L4OPS_LOW                   SOC_PPC_FP_QUAL_HDR_IPV4_L4OPS_LOW
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_HIGH                    SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_SIP_LOW                     SOC_PPC_FP_QUAL_HDR_IPV6_SIP_LOW
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_HIGH                    SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_DIP_LOW                     SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_NEXT_PRTCL                  SOC_PPC_FP_QUAL_HDR_IPV6_NEXT_PRTCL
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_TCP_CTL                     SOC_PPC_FP_QUAL_HDR_IPV6_TCP_CTL
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_L4OPS                       SOC_PPC_FP_QUAL_HDR_IPV6_L4OPS
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_TC                          SOC_PPC_FP_QUAL_HDR_IPV6_TC
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_FLOW_LABEL                  SOC_PPC_FP_QUAL_HDR_IPV6_FLOW_LABEL
#define SOC_PB_PP_FP_QUAL_HDR_IPV6_HOP_LIMIT                   SOC_PPC_FP_QUAL_HDR_IPV6_HOP_LIMIT
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_LABEL_FWD                   SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_FWD
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_LABEL_ID_FWD                SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_EXP_FWD                     SOC_PPC_FP_QUAL_HDR_MPLS_EXP_FWD
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_TTL_FWD                     SOC_PPC_FP_QUAL_HDR_MPLS_TTL_FWD
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_BOS_FWD                     SOC_PPC_FP_QUAL_HDR_MPLS_BOS_FWD
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_LABEL1                      SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_EXP1                        SOC_PPC_FP_QUAL_HDR_MPLS_EXP1
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_TTL1                        SOC_PPC_FP_QUAL_HDR_MPLS_TTL1
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_BOS1                        SOC_PPC_FP_QUAL_HDR_MPLS_BOS1
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_LABEL2                      SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_EXP2                        SOC_PPC_FP_QUAL_HDR_MPLS_EXP2
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_TTL2                        SOC_PPC_FP_QUAL_HDR_MPLS_TTL2
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_BOS2                        SOC_PPC_FP_QUAL_HDR_MPLS_BOS2
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_LABEL3                      SOC_PPC_FP_QUAL_HDR_MPLS_LABEL3
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_EXP3                        SOC_PPC_FP_QUAL_HDR_MPLS_EXP3
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_TTL3                        SOC_PPC_FP_QUAL_HDR_MPLS_TTL3
#define SOC_PB_PP_FP_QUAL_HDR_MPLS_BOS3                        SOC_PPC_FP_QUAL_HDR_MPLS_BOS3
#define SOC_PB_PP_FP_QUAL_IRPP_SRC_TM_PORT                     SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT
#define SOC_PB_PP_FP_QUAL_IRPP_SRC_SYST_PORT                   SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT
#define SOC_PB_PP_FP_QUAL_IRPP_SRC_PP_PORT                     SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT
#define SOC_PB_PP_FP_QUAL_IRPP_PROCESSING_TYPE                 SOC_PPC_FP_QUAL_IRPP_PROCESSING_TYPE
#define SOC_PB_PP_FP_QUAL_IRPP_PKT_HDR_TYPE                    SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE
#define SOC_PB_PP_FP_QUAL_IRPP_ETH_TAG_FORMAT                  SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT
#define SOC_PB_PP_FP_QUAL_IRPP_FWD_DEC_DEST                    SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST
#define SOC_PB_PP_FP_QUAL_IRPP_FWD_DEC_TC                      SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC
#define SOC_PB_PP_FP_QUAL_IRPP_FWD_DEC_DP                      SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP
#define SOC_PB_PP_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE           SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE
#define SOC_PB_PP_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_QUAL           SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_QUAL
#define SOC_PB_PP_FP_QUAL_IRPP_UP                              SOC_PPC_FP_QUAL_IRPP_UP
#define SOC_PB_PP_FP_QUAL_IRPP_SNOOP_CODE                      SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE
#define SOC_PB_PP_FP_QUAL_IRPP_LEARN_DECISION_DEST             SOC_PPC_FP_QUAL_IRPP_LEARN_DECISION_DEST
#define SOC_PB_PP_FP_QUAL_IRPP_LEARN_ADD_INFO                  SOC_PPC_FP_QUAL_IRPP_LEARN_ADD_INFO
#define SOC_PB_PP_FP_QUAL_IRPP_IN_LIF                          SOC_PPC_FP_QUAL_IRPP_IN_LIF
#define SOC_PB_PP_FP_QUAL_IRPP_LL_MIRROR_CMD                   SOC_PPC_FP_QUAL_IRPP_LL_MIRROR_CMD
#define SOC_PB_PP_FP_QUAL_IRPP_SYSTEM_VSI                      SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI
#define SOC_PB_PP_FP_QUAL_IRPP_ORIENTATION_IS_HUB              SOC_PPC_FP_QUAL_IRPP_ORIENTATION_IS_HUB
#define SOC_PB_PP_FP_QUAL_IRPP_VLAN_ID                         SOC_PPC_FP_QUAL_IRPP_VLAN_ID
#define SOC_PB_PP_FP_QUAL_IRPP_VLAN_PCP                        SOC_PPC_FP_QUAL_IRPP_VLAN_PCP
#define SOC_PB_PP_FP_QUAL_IRPP_VLAN_DEI                        SOC_PPC_FP_QUAL_IRPP_VLAN_DEI
#define SOC_PB_PP_FP_QUAL_IRPP_STP_STATE                       SOC_PPC_FP_QUAL_IRPP_STP_STATE
#define SOC_PB_PP_FP_QUAL_IRPP_FWD_TYPE                        SOC_PPC_FP_QUAL_IRPP_FWD_TYPE
#define SOC_PB_PP_FP_QUAL_IRPP_SUB_HEADER_NDX                  SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX
#define SOC_PB_PP_FP_QUAL_IRPP_KEY_CHANGED                     SOC_PPC_FP_QUAL_IRPP_KEY_CHANGED
#define SOC_PB_PP_FP_QUAL_IRPP_IN_RIF                          SOC_PPC_FP_QUAL_IRPP_IN_RIF
#define SOC_PB_PP_FP_QUAL_IRPP_VRF                             SOC_PPC_FP_QUAL_IRPP_VRF
#define SOC_PB_PP_FP_QUAL_IRPP_PCKT_IS_COMP_MC                 SOC_PPC_FP_QUAL_IRPP_PCKT_IS_COMP_MC
#define SOC_PB_PP_FP_QUAL_IRPP_MY_BMAC                         SOC_PPC_FP_QUAL_IRPP_MY_BMAC
#define SOC_PB_PP_FP_QUAL_IRPP_IN_TTL                          SOC_PPC_FP_QUAL_IRPP_IN_TTL
#define SOC_PB_PP_FP_QUAL_IRPP_IN_DSCP_EXP                     SOC_PPC_FP_QUAL_IRPP_IN_DSCP_EXP
#define SOC_PB_PP_FP_QUAL_IRPP_PACKET_SIZE_RANGE               SOC_PPC_FP_QUAL_IRPP_PACKET_SIZE_RANGE
#define SOC_PB_PP_FP_QUAL_IRPP_TERM_TYPE                       SOC_PPC_FP_QUAL_IRPP_TERM_TYPE
#define SOC_PB_PP_FP_QUAL_OUT_LIF                              SOC_PPC_FP_QUAL_OUT_LIF
#define SOC_PB_PP_FP_QUAL_ERPP_PP_PORT_DATA                    SOC_PPC_FP_QUAL_ERPP_PP_PORT_DATA
#define SOC_PB_PP_FP_QUAL_ERPP_IPV4_NEXT_PROTOCOL              SOC_PPC_FP_QUAL_ERPP_IPV4_NEXT_PROTOCOL
#define SOC_PB_PP_FP_QUAL_ERPP_FTMH                            SOC_PPC_FP_QUAL_ERPP_FTMH
#define SOC_PB_PP_FP_QUAL_ERPP_PAYLOAD                         SOC_PPC_FP_QUAL_ERPP_PAYLOAD
#define SOC_PB_PP_FP_QUAL_HDR_USER_DEF_0                       SOC_PPC_FP_QUAL_HDR_USER_DEF_0
#define SOC_PB_PP_FP_QUAL_HDR_USER_DEF_9                       SOC_PPC_FP_QUAL_HDR_USER_DEF_9
#define SOC_PB_PP_FP_QUAL_IRPP_LEM_2ND_LKUP_ASD                SOC_PPC_FP_QUAL_IRPP_LEM_2ND_LKUP_ASD
#define SOC_PB_PP_FP_QUAL_OUT_LIF                              SOC_PPC_FP_QUAL_OUT_LIF
#define SOC_PB_PP_NOF_FP_QUAL_TYPES                            SOC_PPC_NOF_FP_QUAL_TYPES
typedef SOC_PPC_FP_QUAL_TYPE                                   SOC_PB_PP_FP_QUAL_TYPE;

#define SOC_PB_PP_FP_PKT_HDR_TYPE_IPV4_ETH                     SOC_PPC_FP_PKT_HDR_TYPE_IPV4_ETH
#define SOC_PB_PP_FP_PKT_HDR_TYPE_ETH_MPLS1_ETH                SOC_PPC_FP_PKT_HDR_TYPE_ETH_MPLS1_ETH
#define SOC_PB_PP_FP_PKT_HDR_TYPE_ETH_MPLS2_ETH                SOC_PPC_FP_PKT_HDR_TYPE_ETH_MPLS2_ETH
#define SOC_PB_PP_FP_PKT_HDR_TYPE_ETH_MPLS3_ETH                SOC_PPC_FP_PKT_HDR_TYPE_ETH_MPLS3_ETH
#define SOC_PB_PP_FP_PKT_HDR_TYPE_IPV4_MPLS1_ETH               SOC_PPC_FP_PKT_HDR_TYPE_IPV4_MPLS1_ETH
#define SOC_PB_PP_FP_PKT_HDR_TYPE_IPV4_MPLS2_ETH               SOC_PPC_FP_PKT_HDR_TYPE_IPV4_MPLS2_ETH
#define SOC_PB_PP_FP_PKT_HDR_TYPE_IPV4_MPLS3_ETH               SOC_PPC_FP_PKT_HDR_TYPE_IPV4_MPLS3_ETH
#define SOC_PB_PP_FP_PKT_HDR_TYPE_IPV6_MPLS1_ETH               SOC_PPC_FP_PKT_HDR_TYPE_IPV6_MPLS1_ETH
#define SOC_PB_PP_FP_PKT_HDR_TYPE_IPV6_MPLS2_ETH               SOC_PPC_FP_PKT_HDR_TYPE_IPV6_MPLS2_ETH
#define SOC_PB_PP_FP_PKT_HDR_TYPE_IPV6_MPLS3_ETH               SOC_PPC_FP_PKT_HDR_TYPE_IPV6_MPLS3_ETH
#define SOC_PB_PP_FP_PKT_HDR_TYPE_IPV6_IPV4_ETH                SOC_PPC_FP_PKT_HDR_TYPE_IPV6_IPV4_ETH
#define SOC_PB_PP_FP_PKT_HDR_TYPE_ETH_TRILL_ETH                SOC_PPC_FP_PKT_HDR_TYPE_ETH_TRILL_ETH
#define SOC_PB_PP_NOF_FP_PKT_HDR_TYPES                         SOC_PPC_PP_NOF_FP_PKT_HDR_TYPES
typedef SOC_PPC_FP_PKT_HDR_TYPE                                SOC_PB_PP_FP_PKT_HDR_TYPE;

#define SOC_PB_PP_FP_FWD_TYPE_BRIDGED                          SOC_PPC_FP_FWD_TYPE_BRIDGED
#define SOC_PB_PP_FP_FWD_TYPE_IPV4_UC                          SOC_PPC_FP_FWD_TYPE_IPV4_UC
#define SOC_PB_PP_FP_FWD_TYPE_IPV4_MC                          SOC_PPC_FP_FWD_TYPE_IPV4_MC
#define SOC_PB_PP_FP_FWD_TYPE_IPV6_UC                          SOC_PPC_FP_FWD_TYPE_IPV6_UC
#define SOC_PB_PP_FP_FWD_TYPE_IPV6_MC                          SOC_PPC_FP_FWD_TYPE_IPV6_MC
#define SOC_PB_PP_FP_FWD_TYPE_MPLS                             SOC_PPC_FP_FWD_TYPE_MPLS
#define SOC_PB_PP_FP_FWD_TYPE_TRILL                            SOC_PPC_FP_FWD_TYPE_TRILL
#define SOC_PB_PP_FP_FWD_TYPE_BRIDGED_AFTER_TERM               SOC_PPC_FP_FWD_TYPE_BRIDGED_AFTER_TERM
#define SOC_PB_PP_FP_FWD_TYPE_CPU_TRAP                         SOC_PPC_FP_FWD_TYPE_CPU_TRAP
#define SOC_PB_PP_FP_FWD_TYPE_TM                               SOC_PPC_FP_FWD_TYPE_TM
#define SOC_PB_PP_NOF_FP_FWD_TYPES                             SOC_PPC_NOF_FP_FWD_TYPES
typedef SOC_PPC_FP_FWD_TYPE                                    SOC_PB_PP_FP_FWD_TYPE;

typedef SOC_PPC_FP_PROCESSING_TYPE                             SOC_PB_PP_FP_PROCESSING_TYPE;

#define SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_0                 SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_0
#define SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_1                 SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_1
#define SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_2                 SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_2
#define SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_3                 SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_3
#define SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_4                 SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_4
#define SOC_PB_PP_FP_BASE_HEADER_TYPE_HEADER_5                 SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_5
#define SOC_PB_PP_FP_BASE_HEADER_TYPE_FWD                      SOC_PPC_FP_BASE_HEADER_TYPE_FWD
#define SOC_PB_PP_FP_BASE_HEADER_TYPE_FWD_POST                 SOC_PPC_FP_BASE_HEADER_TYPE_FWD_POST
#define SOC_PB_PP_NOF_FP_BASE_HEADER_TYPES                     SOC_PPC_NOF_FP_BASE_HEADER_TYPES
typedef SOC_PPC_FP_BASE_HEADER_TYPE                            SOC_PB_PP_FP_BASE_HEADER_TYPE;

#define SOC_PB_PP_FP_ACTION_TYPE_DEST                          SOC_PPC_FP_ACTION_TYPE_DEST
#define SOC_PB_PP_FP_ACTION_TYPE_DP                            SOC_PPC_FP_ACTION_TYPE_DP
#define SOC_PB_PP_FP_ACTION_TYPE_TC                            SOC_PPC_FP_ACTION_TYPE_TC
#define SOC_PB_PP_FP_ACTION_TYPE_TRAP                           SOC_PPC_FP_ACTION_TYPE_TRAP
#define SOC_PB_PP_FP_ACTION_TYPE_SNP                           SOC_PPC_FP_ACTION_TYPE_SNP
#define SOC_PB_PP_FP_ACTION_TYPE_MIRROR                        SOC_PPC_FP_ACTION_TYPE_MIRROR
#define SOC_PB_PP_FP_ACTION_TYPE_MIR_DIS                       SOC_PPC_FP_ACTION_TYPE_MIR_DIS
#define SOC_PB_PP_FP_ACTION_TYPE_EXC_SRC                       SOC_PPC_FP_ACTION_TYPE_EXC_SRC
#define SOC_PB_PP_FP_ACTION_TYPE_IS                            SOC_PPC_FP_ACTION_TYPE_IS
#define SOC_PB_PP_FP_ACTION_TYPE_METER                         SOC_PPC_FP_ACTION_TYPE_METER
#define SOC_PB_PP_FP_ACTION_TYPE_COUNTER                       SOC_PPC_FP_ACTION_TYPE_COUNTER
#define SOC_PB_PP_FP_ACTION_TYPE_STAT                          SOC_PPC_FP_ACTION_TYPE_STAT
#define SOC_PB_PP_FP_ACTION_TYPE_OUTLIF                        SOC_PPC_FP_ACTION_TYPE_OUTLIF
#define SOC_PB_PP_FP_ACTION_TYPE_LAG_LB                        SOC_PPC_FP_ACTION_TYPE_LAG_LB
#define SOC_PB_PP_FP_ACTION_TYPE_ECMP_LB                       SOC_PPC_FP_ACTION_TYPE_ECMP_LB
#define SOC_PB_PP_FP_ACTION_TYPE_STACK_RT_HIST                 SOC_PPC_FP_ACTION_TYPE_STACK_RT_HIST
#define SOC_PB_PP_FP_ACTION_TYPE_VSQ_PTR                       SOC_PPC_FP_ACTION_TYPE_VSQ_PTR
#define SOC_PB_PP_FP_ACTION_TYPE_CHANGE_KEY                    SOC_PPC_FP_ACTION_TYPE_CHANGE_KEY
#define SOC_PB_PP_FP_ACTION_TYPE_NOP                           SOC_PPC_FP_ACTION_TYPE_NOP
#define SOC_PB_PP_FP_ACTION_TYPE_EGR_TRAP                      SOC_PPC_FP_ACTION_TYPE_EGR_TRAP
#define SOC_PB_PP_FP_ACTION_TYPE_EGR_OFP                       SOC_PPC_FP_ACTION_TYPE_EGR_OFP
#define SOC_PB_PP_FP_ACTION_TYPE_EGR_TC_DP                     SOC_PPC_FP_ACTION_TYPE_EGR_TC_DP
#define SOC_PB_PP_FP_ACTION_TYPE_EGR_OUTLIF                    SOC_PPC_FP_ACTION_TYPE_EGR_OUTLIF
#define SOC_PB_PP_NOF_FP_ACTION_TYPES                          SOC_PPC_NOF_FP_ACTION_TYPES_PB

typedef SOC_PPC_FP_ACTION_TYPE                                 SOC_PB_PP_FP_ACTION_TYPE;

#define SOC_PB_PP_FP_DB_TYPE_TCAM                              SOC_PPC_FP_DB_TYPE_TCAM
#define SOC_PB_PP_FP_DB_TYPE_DIRECT_TABLE                      SOC_PPC_FP_DB_TYPE_DIRECT_TABLE
#define SOC_PB_PP_FP_DB_TYPE_DIRECT_EXTRACTION                 SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION
#define SOC_PB_PP_FP_DB_TYPE_EGRESS                            SOC_PPC_FP_DB_TYPE_EGRESS
#define SOC_PB_PP_NOF_FP_DATABASE_TYPES                        SOC_PPC_NOF_FP_DATABASE_TYPES_PETRA_B
typedef SOC_PPC_FP_DATABASE_TYPE                               SOC_PB_PP_FP_DATABASE_TYPE;

#define SOC_PB_PP_FP_PREDEFINED_ACL_KEY_L2                     SOC_PPC_FP_PREDEFINED_ACL_KEY_L2
#define SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV4                   SOC_PPC_FP_PREDEFINED_ACL_KEY_IPV4
#define SOC_PB_PP_FP_PREDEFINED_ACL_KEY_IPV6                   SOC_PPC_FP_PREDEFINED_ACL_KEY_IPV6
#define SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_ETH                SOC_PPC_FP_PREDEFINED_ACL_KEY_EGR_ETH
#define SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_IPV4               SOC_PPC_FP_PREDEFINED_ACL_KEY_EGR_IPV4
#define SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_TM                 SOC_PPC_FP_PREDEFINED_ACL_KEY_EGR_TM
#define SOC_PB_PP_FP_PREDEFINED_ACL_KEY_EGR_MPLS               SOC_PPC_FP_PREDEFINED_ACL_KEY_EGR_MPLS
#define SOC_PB_PP_NOF_FP_PREDEFINED_ACL_KEYS                   SOC_PPC_NOF_FP_PREDEFINED_ACL_KEYS
typedef SOC_PPC_FP_PREDEFINED_ACL_KEY                          SOC_PB_PP_FP_PREDEFINED_ACL_KEY;

#define SOC_PB_PP_FP_CONTROL_TYPE_L4OPS_RANGE                  SOC_PPC_FP_CONTROL_TYPE_L4OPS_RANGE
#define SOC_PB_PP_FP_CONTROL_TYPE_PACKET_SIZE_RANGE            SOC_PPC_FP_CONTROL_TYPE_PACKET_SIZE_RANGE
#define SOC_PB_PP_FP_CONTROL_TYPE_ETHERTYPE                    SOC_PPC_FP_CONTROL_TYPE_ETHERTYPE
#define SOC_PB_PP_FP_CONTROL_TYPE_NEXT_PROTOCOL_IP             SOC_PPC_FP_CONTROL_TYPE_NEXT_PROTOCOL_IP
#define SOC_PB_PP_FP_CONTROL_TYPE_EGR_PP_PORT_DATA             SOC_PPC_FP_CONTROL_TYPE_EGR_PP_PORT_DATA
#define SOC_PB_PP_FP_CONTROL_TYPE_EGR_L2_ETHERTYPES            SOC_PPC_FP_CONTROL_TYPE_EGR_L2_ETHERTYPES
#define SOC_PB_PP_FP_CONTROL_TYPE_EGR_IPV4_NEXT_PROTOCOL       SOC_PPC_FP_CONTROL_TYPE_EGR_IPV4_NEXT_PROTOCOL
#define SOC_PB_PP_FP_CONTROL_TYPE_PP_PORT_PROFILE              SOC_PPC_FP_CONTROL_TYPE_PP_PORT_PROFILE
#define SOC_PB_PP_FP_CONTROL_TYPE_L2_L3_KEY_IN_LIF_ENABLE      SOC_PPC_FP_CONTROL_TYPE_L2_L3_KEY_IN_LIF_ENABLE
#define SOC_PB_PP_FP_CONTROL_TYPE_L3_IPV6_TCP_CTL_ENABLE       SOC_PPC_FP_CONTROL_TYPE_L3_IPV6_TCP_CTL_ENABLE
#define SOC_PB_PP_FP_CONTROL_TYPE_HDR_USER_DEF                 SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF
#define SOC_PB_PP_FP_CONTROL_TYPE_EGRESS_DP                    SOC_PPC_FP_CONTROL_TYPE_EGRESS_DP
#define SOC_PB_PP_FP_CONTROL_TYPE_INNER_ETH_NOF_VLAN_TAGS      SOC_PPC_FP_CONTROL_TYPE_INNER_ETH_NOF_VLAN_TAGS
#define SOC_PB_PP_FP_CONTROL_TYPE_KEY_CHANGE_SIZE              SOC_PPC_FP_CONTROL_TYPE_KEY_CHANGE_SIZE
#define SOC_PB_PP_NOF_FP_CONTROL_TYPES                         SOC_PPC_NOF_FP_CONTROL_TYPES
typedef SOC_PPC_FP_CONTROL_TYPE                                SOC_PB_PP_FP_CONTROL_TYPE;

typedef SOC_PPC_FP_QUAL_VAL                                    SOC_PB_PP_FP_QUAL_VAL;
typedef SOC_TMC_PMF_PFG_INFO                                   SOC_PB_PP_FP_PFG_INFO;
typedef SOC_PPC_FP_ETH_TAG_FORMAT                              SOC_PB_PP_FP_ETH_TAG_FORMAT;
typedef SOC_PPC_FP_DATABASE_INFO                               SOC_PB_PP_FP_DATABASE_INFO;
typedef SOC_PPC_FP_ACTION_VAL                                  SOC_PB_PP_FP_ACTION_VAL;
typedef SOC_PPC_FP_ENTRY_INFO                                  SOC_PB_PP_FP_ENTRY_INFO;
typedef SOC_PPC_FP_DIR_EXTR_ACTION_LOC                         SOC_PB_PP_FP_DIR_EXTR_ACTION_LOC;
typedef SOC_PPC_FP_DIR_EXTR_ACTION_VAL                         SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL;
typedef SOC_PPC_FP_DIR_EXTR_ENTRY_INFO                         SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO;
typedef SOC_PPC_FP_CONTROL_INDEX                               SOC_PB_PP_FP_CONTROL_INDEX;
typedef SOC_PPC_FP_CONTROL_INFO                                SOC_PB_PP_FP_CONTROL_INFO;

/*
 * Second-cycle key location (as HW encoded)
 */
typedef enum
{
  /*
   *  Not defined.
   */
  SOC_PB_PP_FP_KEY_CHANGE_LOCATION_RESERVED = 0,
  /*
   *  Key A [11:0].
   */
  SOC_PB_PP_FP_KEY_CHANGE_LOCATION_A_11_0 = 1,
  /*
   *  Key A [43:32].
   */
  SOC_PB_PP_FP_KEY_CHANGE_LOCATION_A_43_32 = 2,
  /*
   *  Key B [11:0].
   */
  SOC_PB_PP_FP_KEY_CHANGE_LOCATION_B_11_0 = 3,
  /*
   *  Key B [43:32].
   */
  SOC_PB_PP_FP_KEY_CHANGE_LOCATION_B_43_32 = 4,
  /*
   *  L2 [11:0].
   */
  SOC_PB_PP_FP_KEY_CHANGE_LOCATION_L2_11_0 = 5,
  /*
   *  L3 IPv4 [11:0].
   */
  SOC_PB_PP_FP_KEY_CHANGE_LOCATION_L3_IPV4_11_0 = 6,
  /*
   *  L3 IPv6 [11:0].
   */
  SOC_PB_PP_FP_KEY_CHANGE_LOCATION_L3_IPV6_11_0 = 7,
  /*
   *  Number of types in SOC_PB_PP_FP_KEY_CHANGE_LOCATION
   */
  SOC_PB_PP_NOF_FP_KEY_CHANGE_LOCATIONS = 8
}SOC_PB_PP_FP_KEY_CHANGE_LOCATION;


typedef enum
{
  /*
   *  For macro SOC_PPD_FP_QUAL_VAL_ENCODE_FWD_DECISION.
   */
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_FWD_DECISION = 0,
  /*
   *  For macro SOC_PPD_FP_QUAL_VAL_ENCODE_MAC_ADDRESS.
   */
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_MAC_ADDRESS = 1,
  /*
   *  For macro SOC_PPD_FP_QUAL_VAL_ENCODE_IPV4_SUBNET.
   */
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_IPV4_SUBNET = 2,
  /*
   *  For macro SOC_PPD_FP_QUAL_VAL_ENCODE_IPV6_SUBNET.
   */
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_IPV6_SUBNET = 3,
  /*
   *  For macro SOC_PPD_FP_QUAL_VAL_ENCODE_ETH_TAG_FORMAT.
   */
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_ETH_TAG_FORMAT = 4,
  /*
   *  For macro SOC_PPD_FP_QUAL_VAL_ENCODE_FTMH.
   */
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_FTMH = 5,
  /*
   *  Number of types in SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE
   */
  SOC_PB_PP_NOF_FP_QUAL_VAL_ENCODE_INFO_TYPES = 6
}SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE ;

typedef struct
{
  SOC_PB_PP_FRWRD_DECISION_INFO fwd_dec;
  uint32 dest_nof_bits;
} SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_FWD_DECISION;

typedef struct
{
  SOC_SAND_PP_MAC_ADDRESS  mac;
  SOC_SAND_PP_MAC_ADDRESS  is_valid;
} SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_MAC;

typedef struct
{
  SOC_SAND_PP_IPV4_ADDRESS ip;
  uint32 subnet_length;
} SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_IPV4;

typedef struct
{
  SOC_SAND_PP_IPV6_ADDRESS ip;
  uint32 subnet_length;
  uint8 is_low;
} SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_IPV6;

typedef struct
{
  SOC_PB_PP_FP_ETH_TAG_FORMAT tag_format;
} SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_ETF;

typedef struct
{
  SOC_PB_PP_FP_FTMH ftmh;
} SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_FTMH;

typedef union
{
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_FWD_DECISION fd;
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_MAC mac;
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_IPV4 ipv4;
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_IPV6 ipv6;
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_ETF etf;
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_FTMH ftmh;

} SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_VAL;

typedef struct
{
  /*
   *  Qual val encode type: MAC, IPv4, ..
   */
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE type;
  /*
   *  Qualifier value (union according to type).
   */
  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO_VAL val;

} SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO;

typedef SOC_PPC_FP_DIAG_PP_PORT_INFO                           SOC_PB_PP_FP_DIAG_PP_PORT_INFO;
typedef SOC_PPC_FP_PACKET_DIAG_PARSER                          SOC_PB_PP_FP_PACKET_DIAG_PARSER;
typedef SOC_PPC_FP_PACKET_DIAG_PGM                             SOC_PB_PP_FP_PACKET_DIAG_PGM;
typedef SOC_PPC_FP_PACKET_DIAG_QUAL                            SOC_PB_PP_FP_PACKET_DIAG_QUAL;
typedef SOC_PPC_FP_PACKET_DIAG_DB_QUAL                         SOC_PB_PP_FP_PACKET_DIAG_DB_QUAL;
typedef SOC_PPC_FP_PACKET_DIAG_KEY                             SOC_PB_PP_FP_PACKET_DIAG_KEY;
typedef SOC_PPC_FP_PACKET_DIAG_TCAM_DT                         SOC_PB_PP_FP_PACKET_DIAG_TCAM_DT;
typedef SOC_PPC_FP_PACKET_DIAG_MACRO                           SOC_PB_PP_FP_PACKET_DIAG_MACRO;
typedef SOC_PPC_FP_PACKET_DIAG_INFO                            SOC_PB_PP_FP_PACKET_DIAG_INFO;

#define SOC_PB_PP_FP_KEY_CHANGE_SIZE_DEFAULT                   (12)

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
 *   soc_pb_pp_fp_packet_format_group_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set a Packet Format Group (PFG). The packet format group
 *   defines the supported Packet formats. The user must
 *   indicate for each Database which Packet format(s) are
 *   associated with this Database. E.g.: A Packet Format
 *   Group including only IPv6 packets can be defined to use
 *   Databases with IPv6 Destination-IP qualifiers.
 * INPUT:
 *   SOC_SAND_IN  int                    unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                    pfg_ndx -
 *     Packet Format Group index. Range: 0 - 4.
 *   SOC_SAND_IN  SOC_PB_PP_FP_PFG_INFO            *info -
 *     Packet-Format-Group parameters.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success -
 *     Indicate if the database is created successfully.
 * REMARKS:
 *   The user should set a minimal number of Packet Format
 *   Groups since each one uses many Hardware resources.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_packet_format_group_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_PFG_INFO            *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_fp_packet_format_group_set" API.
 *     Refer to "soc_pb_pp_fp_packet_format_group_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_fp_packet_format_group_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_PFG_INFO            *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_database_create
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Create a database. Each database specifies the action
 *   types to perform and the qualifier fields for this
 *   Database. Entries in the database specify the specific
 *   actions to be taken upon specific values of the
 *   packet. E.g.: Policy Based Routing database update the
 *   FEC value according to DSCP DIP and In-RIF. An entry in
 *   the database may set the FEC of a packet with DIP
 *   1.2.2.3, DSCP value 7 and In-RIF 3 to be 9.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO                    *info -
 *     Database parameters.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Indicate if the database is created successfully.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_database_create(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO                    *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_database_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the database parameters.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_OUT SOC_PB_PP_FP_DATABASE_INFO                    *info -
 *     Database parameters.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_database_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_DATABASE_INFO                    *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_database_destroy
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Destroy the database: all its entries are suppressed and
 *   the Database-ID is freed.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_database_destroy(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an entry to the Database. The database entry is
 *   selected if the entire relevant packet field values are
 *   matched to the database entry qualifiers values. When
 *   the packet is qualified to several entries, the entry
 *   with the strongest priority is chosen.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                                 entry_id_ndx -
 *     Entry handle ID. The entry index is a SW handle, to
 *     enable retrieving the entry attributes by the
 *     soc_ppd_fp_entry_get() function, and remove it by the
 *     soc_ppd_fp_entry_remove() function. The actual location of
 *     the entry in the database is selected according to the
 *     entry's priority. Range: 0 - 4K-1.
 *   SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO                       *info -
 *     Entry parameters.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Indicate if the database entry is created successfully.
 * REMARKS:
 *   1. The database must be created before the insertion of
 *   an entry. To create a Database, call the
 *   soc_ppd_fp_database_create API.2. The database must be NOT
 *   of type 'direct extraction'3. For a Database of type
 *   'Direct Table', the entry qualifier value must be not
 *   masked.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO                       *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_entry_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an entry from the Database.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                                 entry_id_ndx -
 *     Entry-ID. Range: 0 - 4K-1.
 *   SOC_SAND_OUT uint8                                 *is_found -
 *     If True, then the entry is found and the entry
 *     parameters are returned in the 'info' structure.
 *     Otherwise, the entry is not present in the Database.
 *   SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO                       *info -
 *     Entry parameters. Meaningful only the entry is found.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_OUT uint8                                 *is_found,
    SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO                       *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_entry_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an entry from the Database.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                                 entry_id_ndx -
 *     Entry-ID. Range: 0 - 4K-1.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_entry_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_database_entries_get_block
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the Database entries. The function returns list of
 *   entries that were added to a database with database ID
 *   'db_id_ndx'.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range -
 *     Block range to get entries in this Database.
 *   SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO                       *entries -
 *     Database entries.
 *   SOC_SAND_OUT uint32                                  *nof_entries -
 *     Number of valid entries.
 * REMARKS:
 *   This API can be called only if the Database is NOT of
 *   type 'direct extraction'.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_database_entries_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO                       *entries,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_direct_extraction_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an entry to the Database. The database entry is
 *   selected if all the Packet Qualifier field values are in
 *   the Database entry range.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                                 entry_id_ndx -
 *     Entry-ID. Range: 0 - 15.
 *   SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *info -
 *     Entry parameters.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Indicate if the database entry is created successfully.
 * REMARKS:
 *   1. The database must be created before the insertion of
 *   an entry. To create a Database, call the
 *   soc_ppd_fp_database_create API.2. The database must be of
 *   type 'direct extraction'3. The priority enables
 *   selection between two database entries with a
 *   superposition in the Qualifier field ranges.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_direct_extraction_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_direct_extraction_entry_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an entry from the Database.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                                 entry_id_ndx -
 *     Entry-ID. Range: 0 - 15.
 *   SOC_SAND_OUT uint8                                 *is_found -
 *     If True, then the entry is found and the entry
 *     parameters are returned in the 'info' structure.
 *     Otherwise, the entry is not present in the Database.
 *   SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *info -
 *     Entry parameters. Meaningful only the entry is found.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_direct_extraction_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_OUT uint8                                 *is_found,
    SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_direct_extraction_entry_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an entry from the Database.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                                 entry_id_ndx -
 *     Entry-ID. Range: 0 - 15.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_direct_extraction_entry_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_direct_extraction_db_entries_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the Database entries.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range -
 *     Block range to get entries in this Database.
 *   SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *entries -
 *     Database entries.
 *   SOC_SAND_OUT uint32                                  *nof_entries -
 *     Number of valid entries.
 * REMARKS:
 *   This API can be called only if the Database is of type
 *   'direct extraction'.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_direct_extraction_db_entries_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO              *entries,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_control_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set one of the control options.
 * INPUT:
 *   SOC_SAND_IN  int                    unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX       *control_ndx -
 *     Index for the control set API.
 *   SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INFO        *info -
 *     Type and Values of selected control option.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success -
 *     Indicate if the operation has succeeded.
 * REMARKS:
 *   The exact semantics of the info are determined by the
 *   control option specified by the type.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_control_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INFO        *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  );

/*********************************************************************
*     Gets the configuration set by the "soc_pb_pp_fp_control_set"
 *     API.
 *     Refer to "soc_pb_pp_fp_control_set" API for details.
*********************************************************************/
uint32
  soc_pb_pp_fp_control_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_OUT SOC_PB_PP_FP_CONTROL_INFO        *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_egr_db_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the mapping between the Packet forward type and the
 *   Port profile to the Database-ID.
 * INPUT:
 *   SOC_SAND_IN  int                    unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE            fwd_type_ndx -
 *     Packet forward type.
 *   SOC_SAND_IN  uint32                    port_profile_ndx -
 *     PP-Port profile. Range: 0 - 3.
 *   SOC_SAND_IN  uint32                     db_id -
 *     Database-Id to use for these packets. Range: 0 - 63.
 * REMARKS:
 *   1. The mapping between PP-Port and PP-Port profile is
 *   set via the soc_ppd_fp_control_set API with type
 *   SOC_PPD_FP_CONTROL_TYPE_PP_PORT_PROFILE.2. The Database with
 *   this Database-Id must exist and correspond to an Egress
 *   ACL Database
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_egr_db_map_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_IN  uint32                     db_id
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_fp_egr_db_map_set" API.
 *     Refer to "soc_pb_pp_fp_egr_db_map_set" API for details.
*********************************************************************/
uint32
  soc_pb_pp_fp_egr_db_map_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_OUT uint32                     *db_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_fp_packet_diag_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the Field Processing of the last packets.
 * INPUT:
 *   SOC_SAND_IN  int                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_INFO *info -
 *     Field Processor specifications for this packet.
 * REMARKS:
 *   This API must be called during a continuous stream of
 *   the identical packets coming from the same source.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_fp_packet_diag_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_INFO *info
  );
/*
 * Macro selection
 */
uint32
  soc_pb_pp_fp_qual_type_preset(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_PP_FP_PREDEFINED_ACL_KEY  predefined_key,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_TYPE           qual_types[SOC_PB_PP_FP_NOF_QUALS_PER_DB_MAX]
  );

uint32
  soc_pb_pp_fp_qual_val_encode(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_VAL_ENCODE_INFO *qual_val_encode,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_VAL             *qual_val
  );

uint32
  soc_pb_pp_fp_qual_val_encode_fwd_decision(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_DECISION_INFO fwd_dec1,
    SOC_SAND_IN  uint32 dest_nof_bits1,
    SOC_SAND_OUT uint32 *qual_val_dest,
    SOC_SAND_OUT uint32 *qual_val_add
  );

uint32
  soc_pb_pp_fp_qual_val_encode_mac_address(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS mac1,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS is_valid1,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_VAL *qual_val
  );

uint32
  soc_pb_pp_fp_qual_val_encode_ipv4_subnet(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_ADDRESS ip1,
    SOC_SAND_IN  uint32 subnet_length1,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_VAL *qual_val
  );

uint32
  soc_pb_pp_fp_qual_val_encode_ipv6_subnet(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_ADDRESS ip1,
    SOC_SAND_IN  uint32 subnet_length1,
    SOC_SAND_IN  uint8 is_low1,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_VAL *qual_val
  );

uint32
  soc_pb_pp_fp_qual_val_encode_eth_tag_format(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  SOC_PB_PP_FP_ETH_TAG_FORMAT tag_format,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_VAL *qual_val
  );

uint32
  soc_pb_pp_fp_qual_val_encode_ftmh(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  SOC_PB_PP_FP_FTMH ftmh1,
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_VAL *qual_val
  );

uint32
  soc_pb_pp_fp_egress_action_type_preset(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT SOC_PB_PP_FP_ACTION_TYPE *action_types
  );

void
  SOC_PB_PP_FP_QUAL_VAL_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_QUAL_VAL *info
  );

void
  SOC_PB_PP_FP_ETH_TAG_FORMAT_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_ETH_TAG_FORMAT *info
  );

void
  SOC_PB_PP_FP_PFG_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PFG_INFO *info
  );

void
  SOC_PB_PP_FP_DATABASE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_DATABASE_INFO *info
  );

void
  SOC_PB_PP_FP_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_ACTION_VAL *info
  );

void
  SOC_PB_PP_FP_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_ENTRY_INFO *info
  );

void
  SOC_PB_PP_FP_DIR_EXTR_ACTION_LOC_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ACTION_LOC *info
  );

void
  SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL *info
  );

void
  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO *info
  );

void
  SOC_PB_PP_FP_CONTROL_INDEX_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_CONTROL_INDEX *info
  );

void
  SOC_PB_PP_FP_CONTROL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_CONTROL_INFO *info
  );

void
  SOC_PB_PP_FP_DIAG_PP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_DIAG_PP_PORT_INFO *info
  );

void
  SOC_PB_PP_FP_PACKET_DIAG_PARSER_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_PARSER *info
  );

void
  SOC_PB_PP_FP_PACKET_DIAG_PGM_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_PGM *info
  );

void
  SOC_PB_PP_FP_PACKET_DIAG_QUAL_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_QUAL *info
  );

void
  SOC_PB_PP_FP_PACKET_DIAG_DB_QUAL_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_DB_QUAL *info
  );

void
  SOC_PB_PP_FP_PACKET_DIAG_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_KEY *info
  );

void
  SOC_PB_PP_FP_PACKET_DIAG_TCAM_DT_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_TCAM_DT *info
  );

void
  SOC_PB_PP_FP_PACKET_DIAG_MACRO_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_MACRO *info
  );

void
  SOC_PB_PP_FP_PACKET_DIAG_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_PACKET_DIAG_INFO *info
  );

void
  SOC_PB_PP_FP_FTMH_clear(
    SOC_SAND_OUT SOC_PB_PP_FP_FTMH *info
  );

#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_FP_QUAL_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_TYPE enum_val
  );

const char*
  SOC_PB_PP_FP_PKT_HDR_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_PKT_HDR_TYPE enum_val
  );

const char*
  SOC_PB_PP_FP_FWD_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_FWD_TYPE enum_val
  );

const char*
  SOC_PB_PP_FP_PROCESSING_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_PROCESSING_TYPE enum_val
  );

const char*
  SOC_PB_PP_FP_BASE_HEADER_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_BASE_HEADER_TYPE enum_val
  );

const char*
  SOC_PB_PP_FP_ACTION_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_TYPE enum_val
  );

const char*
  SOC_PB_PP_FP_DATABASE_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_TYPE enum_val
  );

const char*
  SOC_PB_PP_FP_PREDEFINED_ACL_KEY_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_PREDEFINED_ACL_KEY enum_val
  );

const char*
  SOC_PB_PP_FP_CONTROL_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_TYPE enum_val
  );

void
  SOC_PB_PP_FP_QUAL_VAL_print(
    SOC_SAND_IN  SOC_PB_PP_FP_QUAL_VAL *info
  );

void
  SOC_PB_PP_FP_PFG_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FP_PFG_INFO *info
  );

void
  SOC_PB_PP_FP_DATABASE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FP_DATABASE_INFO *info
  );

void
  SOC_PB_PP_FP_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PB_PP_FP_ACTION_VAL *info
  );

void
  SOC_PB_PP_FP_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FP_ENTRY_INFO *info
  );

void
  SOC_PB_PP_FP_DIR_EXTR_ACTION_LOC_print(
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ACTION_LOC *info
  );

void
  SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ACTION_VAL *info
  );

void
  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FP_DIR_EXTR_ENTRY_INFO *info
  );

void
  SOC_PB_PP_FP_CONTROL_INDEX_print(
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INDEX *info
  );

void
  SOC_PB_PP_FP_CONTROL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FP_CONTROL_INFO *info
  );
void
  SOC_PB_PP_FP_DIAG_PP_PORT_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FP_DIAG_PP_PORT_INFO *info
  );

void
  SOC_PB_PP_FP_PACKET_DIAG_PARSER_print(
    SOC_SAND_IN  SOC_PB_PP_FP_PACKET_DIAG_PARSER *info
  );

void
  SOC_PB_PP_FP_PACKET_DIAG_PGM_print(
    SOC_SAND_IN  SOC_PB_PP_FP_PACKET_DIAG_PGM *info
  );

void
  SOC_PB_PP_FP_PACKET_DIAG_QUAL_print(
    SOC_SAND_IN  SOC_PB_PP_FP_PACKET_DIAG_QUAL *info
  );

void
  SOC_PB_PP_FP_PACKET_DIAG_DB_QUAL_print(
    SOC_SAND_IN  uint32                  ind,
    SOC_SAND_IN  SOC_PB_PP_FP_PACKET_DIAG_DB_QUAL *info
  );

void
  SOC_PB_PP_FP_PACKET_DIAG_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_FP_PACKET_DIAG_KEY *info
  );

void
  SOC_PB_PP_FP_PACKET_DIAG_TCAM_DT_print(
    SOC_SAND_IN  SOC_PB_PP_FP_PACKET_DIAG_TCAM_DT *info
  );

void
  SOC_PB_PP_FP_PACKET_DIAG_MACRO_print(
    SOC_SAND_IN  SOC_PB_PP_FP_PACKET_DIAG_MACRO *info
  );

void
  SOC_PB_PP_FP_PACKET_DIAG_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FP_PACKET_DIAG_INFO *info
  );

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_API_FP_INCLUDED__*/
#endif

