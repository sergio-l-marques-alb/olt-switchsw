/* $Id: arad_pp_api_fp.h,v 1.33 Broadcom SDK $
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
 * $
*/

#ifndef __ARAD_PP_API_FP_INCLUDED__
/* { */
#define __ARAD_PP_API_FP_INCLUDED__


/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_fp.h>
#include <soc/dpp/TMC/tmc_pmf_pgm_mgmt.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>
#include <soc/dpp/ARAD/arad_pmf_prog_select.h>
#include <soc/dpp/ARAD/arad_tcam.h>



/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximum number of Qualifier fields per Database.        */
#define  ARAD_PP_FP_NOF_QUALS_PER_DB_MAX (SOC_PPC_FP_NOF_QUALS_PER_DB_MAX)

/*     Maximum number of Actions per Database.                 */
#define  ARAD_PP_FP_NOF_ACTIONS_PER_DB_MAX (SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX)

/*     Maximum number of field values that can be extracted to
 *     build the action value.                                 */
#define  ARAD_PP_FP_DIR_EXTR_MAX_NOF_FIELDS (SOC_PPC_FP_DIR_EXTR_MAX_NOF_FIELDS)

/*     Maximum number of Control values.                       */
#define  ARAD_PP_FP_NOF_CONTROL_VALS (SOC_PPC_FP_NOF_CONTROL_VALS)

/*     Number of Packet-Format-Groups                          */
#define  ARAD_PP_FP_NOF_PFGS                   (SOC_PPC_FP_NOF_PFGS_ARAD)

/*     Number of Priority decoders                          */
#define  ARAD_PP_FP_NOF_PDS        (8)

/*     Number of cycles in TCAM and Macros.                    */
#define  ARAD_PP_FP_NOF_CYCLES (SOC_PPC_FP_NOF_CYCLES)

/*     Number of TCAM Banks.                                   */
#define  ARAD_PP_FP_TCAM_NOF_BANKS (SOC_PPC_FP_TCAM_NOF_BANKS)

/*     Number of Macros.                                       */
#define  ARAD_PP_FP_NOF_MACROS (SOC_PPC_FP_NOF_MACROS)

/* flag determine if the offset value is negative */
#define ARAD_PP_FP_CONTROL_TYPE_HDR_USER_DEF_FLAG_NEGATIVE  SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF_FLAG_NEGATIVE

/* } */
/*************
 * MACROS    *
 *************/
/* { */
/* NOTICE:  You may compare this macros to the aquivalent macros in petra b.
 * The macros in petra b leads to functions.
 */
 
/*
 *  Set the list of admissible qualifier types for databases based
 *  on predefined keys. Must be called during the Database create
 *  API to set the Database qualifier types.
 */
#define ARAD_PP_FP_QUAL_TYPE_PRESET(unit, predefined_key, qual_types)  \
  SOC_PPD_INVALID_DEVICE_TYPE_ERR

/*
 *  Encode the Forward Decision 'fwd_dec' in a Qualifier value
 *  format. If an entry Qualifier value is set to 'qual_val',
 *  then only packets with this Forwarding Decision are qualified.
 */
#define ARAD_PP_FP_QUAL_VAL_ENCODE_FWD_DECISION(unit, fwd_dec1, dest_nof_bits1, qual_val_dest, qual_val_add) \
  SOC_PPD_INVALID_DEVICE_TYPE_ERR

/*
 *  Encode the MAC-Address 'mac' and the 'is_valid' mask in a
 *  Qualifier value format. If an entry Qualifier value is set to
 *  'qual_val', then only packets with a MAC-Address equals to 'mac'
 *  - only for bits set by 'is_valid' - are qualified.
 */
#define ARAD_PP_FP_QUAL_VAL_ENCODE_MAC_ADDRESS(unit, mac1, is_valid1, qual_val)									\
  SOC_PPD_INVALID_DEVICE_TYPE_ERR


/*
 *  Encode the IP Subnet (according to the IP-Address 'ip' and the
 *  'subnet_length') in a Qualifier value format. If an entry
 *  Qualifier value is set to 'qual_val', then only packets with an
 *  IP-Address in the subnet defined by the IP-Address 'ip' and the
 *  'subnet_length' are qualified.
 */
#define ARAD_PP_FP_QUAL_VAL_ENCODE_IPV4_SUBNET(unit, ip1, subnet_length1, qual_val)								\
  SOC_PPD_INVALID_DEVICE_TYPE_ERR

/*
 *   Encode the IP Subnet (according to the IP-Address 'ip' and the
 *  'subnet_length', and also the 'is_low' parameter which sets if it
 *  relates to the 64 LSBs - low part-) in a Qualifier value format.
 *  If an entry Qualifier value is set to 'qual_val', then only packets
 *  with an IP-Address in the subnet defined by the IP-Address 'ip'
 *  and the 'subnet_length' are qualified.
 */
#define ARAD_PP_FP_QUAL_VAL_ENCODE_IPV6_SUBNET(unit, ip1, subnet_length1, is_low1, qual_val)				\
  SOC_PPD_INVALID_DEVICE_TYPE_ERR
/*
 *  Encode the Ethernet Tag format (according to the parameter
 *  'eth_tag_format') in a Qualifier value format.
 */
#define ARAD_PP_FP_QUAL_VAL_ENCODE_ETH_TAG_FORMAT(unit, tag_format, qual_val)      					    \
  SOC_PPD_INVALID_DEVICE_TYPE_ERR

/*
 *  Encode the FTMH (according to the parameter
 *  'ftmh') in a Qualifier value format.
 */
#define ARAD_PP_FP_QUAL_VAL_ENCODE_FTMH(unit, ftmh1, qual_val)      					                    \
  SOC_PPD_INVALID_DEVICE_TYPE_ERR

/*
 *  Set the list of the egress action types for Egress databases.
 */
#define ARAD_PP_FP_EGRESS_ACTION_TYPE_PRESET(unit, action_types)       \
  SOC_PPD_INVALID_DEVICE_TYPE_ERR


/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */
typedef SOC_PPC_FP_FTMH                                     ARAD_PP_FP_FTMH;


#define ARAD_PP_FP_PKT_HDR_TYPE_RAW                          SOC_PPC_FP_PKT_HDR_TYPE_RAW
#define ARAD_PP_FP_PKT_HDR_TYPE_FTMH                         SOC_PPC_FP_PKT_HDR_TYPE_FTMH
#define ARAD_PP_FP_PKT_HDR_TYPE_TM                           SOC_PPC_FP_PKT_HDR_TYPE_TM
#define ARAD_PP_FP_PKT_HDR_TYPE_TM_IS                        SOC_PPC_FP_PKT_HDR_TYPE_TM_IS
#define ARAD_PP_FP_PKT_HDR_TYPE_TM_PPH                       SOC_PPC_FP_PKT_HDR_TYPE_TM_PPH
#define ARAD_PP_FP_PKT_HDR_TYPE_TM_IS_PPH                    SOC_PPC_FP_PKT_HDR_TYPE_TM_IS_PPH
#define ARAD_PP_FP_PKT_HDR_TYPE_ETH                          SOC_PPC_FP_PKT_HDR_TYPE_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_ETH_ETH                      SOC_PPC_FP_PKT_HDR_TYPE_ETH_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_IPV4_ETH                     SOC_PPC_FP_PKT_HDR_TYPE_IPV4_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_IPV6_ETH                     SOC_PPC_FP_PKT_HDR_TYPE_IPV6_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_MPLS1_ETH                    SOC_PPC_FP_PKT_HDR_TYPE_MPLS1_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_MPLS2_ETH                    SOC_PPC_FP_PKT_HDR_TYPE_MPLS2_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_MPLS3_ETH                    SOC_PPC_FP_PKT_HDR_TYPE_MPLS3_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_ETH_MPLS1_ETH                SOC_PPC_FP_PKT_HDR_TYPE_ETH_MPLS1_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_ETH_MPLS2_ETH                SOC_PPC_FP_PKT_HDR_TYPE_ETH_MPLS2_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_ETH_MPLS3_ETH                SOC_PPC_FP_PKT_HDR_TYPE_ETH_MPLS3_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_IPV4_MPLS1_ETH               SOC_PPC_FP_PKT_HDR_TYPE_IPV4_MPLS1_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_IPV4_MPLS2_ETH               SOC_PPC_FP_PKT_HDR_TYPE_IPV4_MPLS2_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_IPV4_MPLS3_ETH               SOC_PPC_FP_PKT_HDR_TYPE_IPV4_MPLS3_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_IPV6_MPLS1_ETH               SOC_PPC_FP_PKT_HDR_TYPE_IPV6_MPLS1_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_IPV6_MPLS2_ETH               SOC_PPC_FP_PKT_HDR_TYPE_IPV6_MPLS2_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_IPV6_MPLS3_ETH               SOC_PPC_FP_PKT_HDR_TYPE_IPV6_MPLS3_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_IPV4_IPV4_ETH                SOC_PPC_FP_PKT_HDR_TYPE_IPV4_IPV4_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_IPV6_IPV4_ETH                SOC_PPC_FP_PKT_HDR_TYPE_IPV6_IPV4_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_ETH_TRILL_ETH                SOC_PPC_FP_PKT_HDR_TYPE_ETH_TRILL_ETH
#define ARAD_PP_FP_PKT_HDR_TYPE_ETH_IPV4_ETH                 SOC_TMC_FP_PKT_HDR_TYPE_ETH_IPV4_ETH
#define ARAD_PP_NOF_FP_PKT_HDR_TYPES                         SOC_PPC_PP_NOF_FP_PKT_HDR_TYPES
typedef SOC_PPC_FP_PKT_HDR_TYPE                                ARAD_PP_FP_PKT_HDR_TYPE;

#define ARAD_PP_FP_FWD_TYPE_BRIDGED                          SOC_PPC_FP_FWD_TYPE_BRIDGED
#define ARAD_PP_FP_FWD_TYPE_IPV4_UC                          SOC_PPC_FP_FWD_TYPE_IPV4_UC
#define ARAD_PP_FP_FWD_TYPE_IPV4_MC                          SOC_PPC_FP_FWD_TYPE_IPV4_MC
#define ARAD_PP_FP_FWD_TYPE_IPV6_UC                          SOC_PPC_FP_FWD_TYPE_IPV6_UC
#define ARAD_PP_FP_FWD_TYPE_IPV6_MC                          SOC_PPC_FP_FWD_TYPE_IPV6_MC
#define ARAD_PP_FP_FWD_TYPE_MPLS                             SOC_PPC_FP_FWD_TYPE_MPLS
#define ARAD_PP_FP_FWD_TYPE_TRILL                            SOC_PPC_FP_FWD_TYPE_TRILL
#define ARAD_PP_FP_FWD_TYPE_BRIDGED_AFTER_TERM               SOC_PPC_FP_FWD_TYPE_BRIDGED_AFTER_TERM
#define ARAD_PP_FP_FWD_TYPE_CPU_TRAP                         SOC_PPC_FP_FWD_TYPE_CPU_TRAP
#define ARAD_PP_FP_FWD_TYPE_TM                               SOC_PPC_FP_FWD_TYPE_TM
#define ARAD_PP_NOF_FP_FWD_TYPES                             SOC_PPC_NOF_FP_FWD_TYPES
typedef SOC_PPC_FP_FWD_TYPE                                    ARAD_PP_FP_FWD_TYPE;

#define ARAD_PP_FP_PROCESSING_TYPE_RAW                       SOC_PPC_FP_PROCESSING_TYPE_RAW
#define ARAD_PP_FP_PROCESSING_TYPE_ETH                       SOC_PPC_FP_PROCESSING_TYPE_ETH
#define ARAD_PP_FP_PROCESSING_TYPE_TM                        SOC_PPC_FP_PROCESSING_TYPE_TM
#define ARAD_PP_FP_PROCESSING_TYPE_FTMH                      SOC_PPC_FP_PROCESSING_TYPE_FTMH
#define ARAD_PP_NOF_FP_PROCESSING_TYPES                      SOC_PPC_NOF_FP_PROCESSING_TYPES
typedef SOC_PPC_FP_PROCESSING_TYPE                             ARAD_PP_FP_PROCESSING_TYPE;

#define ARAD_PP_FP_BASE_HEADER_TYPE_HEADER_0                 SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_0
#define ARAD_PP_FP_BASE_HEADER_TYPE_HEADER_1                 SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_1
#define ARAD_PP_FP_BASE_HEADER_TYPE_HEADER_2                 SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_2
#define ARAD_PP_FP_BASE_HEADER_TYPE_HEADER_3                 SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_3
#define ARAD_PP_FP_BASE_HEADER_TYPE_HEADER_4                 SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_4
#define ARAD_PP_FP_BASE_HEADER_TYPE_HEADER_5                 SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_5
#define ARAD_PP_FP_BASE_HEADER_TYPE_FWD                      SOC_PPC_FP_BASE_HEADER_TYPE_FWD
#define ARAD_PP_FP_BASE_HEADER_TYPE_FWD_POST                 SOC_PPC_FP_BASE_HEADER_TYPE_FWD_POST
#define ARAD_PP_NOF_FP_BASE_HEADER_TYPES                     SOC_PPC_NOF_FP_BASE_HEADER_TYPES
typedef SOC_PPC_FP_BASE_HEADER_TYPE                            ARAD_PP_FP_BASE_HEADER_TYPE;

#define ARAD_PP_FP_DB_TYPE_TCAM                              SOC_PPC_FP_DB_TYPE_TCAM
#define ARAD_PP_FP_DB_TYPE_DIRECT_TABLE                      SOC_PPC_FP_DB_TYPE_DIRECT_TABLE
#define ARAD_PP_FP_DB_TYPE_DIRECT_EXTRACTION                 SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION
#define ARAD_PP_FP_DB_TYPE_EGRESS                            SOC_PPC_FP_DB_TYPE_EGRESS
#define ARAD_PP_FP_DB_TYPE_FLP                               SOC_PPC_FP_DB_TYPE_FLP
#define ARAD_PP_FP_DB_TYPE_SLB                               SOC_PPC_FP_DB_TYPE_SLB
#define ARAD_PP_FP_DB_TYPE_VT                                SOC_PPC_FP_DB_TYPE_VT
#define ARAD_PP_FP_DB_TYPE_TT                                SOC_PPC_FP_DB_TYPE_TT
#define ARAD_PP_NOF_FP_DATABASE_TYPES                        SOC_PPC_NOF_FP_DATABASE_TYPES
typedef SOC_PPC_FP_DATABASE_TYPE                               ARAD_PP_FP_DATABASE_TYPE;

#define ARAD_PP_FP_PREDEFINED_ACL_KEY_L2                     SOC_PPC_FP_PREDEFINED_ACL_KEY_L2
#define ARAD_PP_FP_PREDEFINED_ACL_KEY_IPV4                   SOC_PPC_FP_PREDEFINED_ACL_KEY_IPV4
#define ARAD_PP_FP_PREDEFINED_ACL_KEY_IPV6                   SOC_PPC_FP_PREDEFINED_ACL_KEY_IPV6
#define ARAD_PP_FP_PREDEFINED_ACL_KEY_EGR_ETH                SOC_PPC_FP_PREDEFINED_ACL_KEY_EGR_ETH
#define ARAD_PP_FP_PREDEFINED_ACL_KEY_EGR_IPV4               SOC_PPC_FP_PREDEFINED_ACL_KEY_EGR_IPV4
#define ARAD_PP_FP_PREDEFINED_ACL_KEY_EGR_TM                 SOC_PPC_FP_PREDEFINED_ACL_KEY_EGR_TM
#define ARAD_PP_FP_PREDEFINED_ACL_KEY_EGR_MPLS               SOC_PPC_FP_PREDEFINED_ACL_KEY_EGR_MPLS
#define ARAD_PP_NOF_FP_PREDEFINED_ACL_KEYS                   SOC_PPC_NOF_FP_PREDEFINED_ACL_KEYS
typedef SOC_PPC_FP_PREDEFINED_ACL_KEY                          ARAD_PP_FP_PREDEFINED_ACL_KEY;

#define ARAD_PP_FP_CONTROL_TYPE_L4OPS_RANGE                  SOC_PPC_FP_CONTROL_TYPE_L4OPS_RANGE
#define ARAD_PP_FP_CONTROL_TYPE_PACKET_SIZE_RANGE            SOC_PPC_FP_CONTROL_TYPE_PACKET_SIZE_RANGE
#define ARAD_PP_FP_CONTROL_TYPE_OUT_LIF_RANGE                SOC_PPC_FP_CONTROL_TYPE_OUT_LIF_RANGE
#define ARAD_PP_FP_CONTROL_TYPE_ETHERTYPE                    SOC_PPC_FP_CONTROL_TYPE_ETHERTYPE
#define ARAD_PP_FP_CONTROL_TYPE_NEXT_PROTOCOL_IP             SOC_PPC_FP_CONTROL_TYPE_NEXT_PROTOCOL_IP
#define ARAD_PP_FP_CONTROL_TYPE_EGR_PP_PORT_DATA             SOC_PPC_FP_CONTROL_TYPE_EGR_PP_PORT_DATA
#define ARAD_PP_FP_CONTROL_TYPE_EGR_TM_PORT_DATA             SOC_PPC_FP_CONTROL_TYPE_EGR_TM_PORT_DATA
#define ARAD_PP_FP_CONTROL_TYPE_ING_PP_PORT_DATA             SOC_PPC_FP_CONTROL_TYPE_ING_PP_PORT_DATA
#define ARAD_PP_FP_CONTROL_TYPE_FLP_PP_PORT_DATA             SOC_PPC_FP_CONTROL_TYPE_FLP_PP_PORT_DATA
#define ARAD_PP_FP_CONTROL_TYPE_ING_TM_PORT_DATA             SOC_PPC_FP_CONTROL_TYPE_ING_TM_PORT_DATA
#define ARAD_PP_FP_CONTROL_TYPE_EGR_L2_ETHERTYPES            SOC_PPC_FP_CONTROL_TYPE_EGR_L2_ETHERTYPES
#define ARAD_PP_FP_CONTROL_TYPE_EGR_IPV4_NEXT_PROTOCOL       SOC_PPC_FP_CONTROL_TYPE_EGR_IPV4_NEXT_PROTOCOL
#define ARAD_PP_FP_CONTROL_TYPE_PP_PORT_PROFILE              SOC_PPC_FP_CONTROL_TYPE_PP_PORT_PROFILE
#define ARAD_PP_FP_CONTROL_TYPE_L2_L3_KEY_IN_LIF_ENABLE      SOC_PPC_FP_CONTROL_TYPE_L2_L3_KEY_IN_LIF_ENABLE
#define ARAD_PP_FP_CONTROL_TYPE_L3_IPV6_TCP_CTL_ENABLE       SOC_PPC_FP_CONTROL_TYPE_L3_IPV6_TCP_CTL_ENABLE
#define ARAD_PP_FP_CONTROL_TYPE_HDR_USER_DEF                 SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF
#define ARAD_PP_FP_CONTROL_TYPE_EGRESS_DP                    SOC_PPC_FP_CONTROL_TYPE_EGRESS_DP
#define ARAD_PP_FP_CONTROL_TYPE_INNER_ETH_NOF_VLAN_TAGS      SOC_PPC_FP_CONTROL_TYPE_INNER_ETH_NOF_VLAN_TAGS
#define ARAD_PP_FP_CONTROL_TYPE_KEY_CHANGE_SIZE              SOC_PPC_FP_CONTROL_TYPE_KEY_CHANGE_SIZE
#define ARAD_PP_FP_CONTROL_TYPE_ACE_POINTER_PP_PORT          SOC_PPC_FP_CONTROL_TYPE_ACE_POINTER_PP_PORT
#define ARAD_PP_FP_CONTROL_TYPE_ACE_POINTER_ONLY             SOC_PPC_FP_CONTROL_TYPE_ACE_POINTER_ONLY
#define ARAD_PP_FP_CONTROL_TYPE_ACE_POINTER_OUT_LIF          SOC_PPC_FP_CONTROL_TYPE_ACE_POINTER_OUT_LIF
#define ARAD_PP_FP_CONTROL_TYPE_IN_PORT_PROFILE              SOC_PPC_FP_CONTROL_TYPE_IN_PORT_PROFILE
#define ARAD_PP_FP_CONTROL_TYPE_OUT_PORT_PROFILE             SOC_PPC_FP_CONTROL_TYPE_OUT_PORT_PROFILE
#define ARAD_PP_FP_CONTROL_TYPE_FLP_PGM_PROFILE              SOC_PPC_FP_CONTROL_TYPE_FLP_PGM_PROFILE
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#define ARAD_PP_FP_CONTROL_TYPE_KBP_CACHE                    SOC_PPC_FP_CONTROL_TYPE_KBP_CACHE
#define ARAD_PP_FP_CONTROL_TYPE_KBP_COMMIT                   SOC_PPC_FP_CONTROL_TYPE_KBP_COMMIT
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
#define ARAD_PP_NOF_FP_CONTROL_TYPES                         SOC_PPC_NOF_FP_CONTROL_TYPES
typedef SOC_PPC_FP_CONTROL_TYPE                                ARAD_PP_FP_CONTROL_TYPE;

typedef SOC_PPC_FP_QUAL_VAL                                    ARAD_PP_FP_QUAL_VAL;
typedef SOC_TMC_PMF_PFG_INFO                                   ARAD_PP_FP_PFG_INFO;
typedef SOC_PPC_FP_ETH_TAG_FORMAT                              ARAD_PP_FP_ETH_TAG_FORMAT;
typedef SOC_PPC_FP_ACTION_VAL                                  ARAD_PP_FP_ACTION_VAL;
typedef SOC_PPC_FP_ENTRY_INFO                                  ARAD_PP_FP_ENTRY_INFO;
typedef SOC_PPC_FP_DIR_EXTR_ACTION_LOC                         ARAD_PP_FP_DIR_EXTR_ACTION_LOC;
typedef SOC_PPC_FP_DIR_EXTR_ACTION_VAL                         ARAD_PP_FP_DIR_EXTR_ACTION_VAL;
typedef SOC_PPC_FP_DIR_EXTR_ENTRY_INFO                         ARAD_PP_FP_DIR_EXTR_ENTRY_INFO;
typedef SOC_PPC_FP_CONTROL_INDEX                               ARAD_PP_FP_CONTROL_INDEX;
typedef SOC_PPC_FP_CONTROL_INFO                                ARAD_PP_FP_CONTROL_INFO;

/*
 * Second-cycle key location (as HW encoded)
 */
typedef enum
{
  /*
   *  Not defined.
   */
  ARAD_PP_FP_KEY_CHANGE_LOCATION_RESERVED = 0,
  /*
   *  Key A [11:0].
   */
  ARAD_PP_FP_KEY_CHANGE_LOCATION_A_11_0 = 1,
  /*
   *  Key A [43:32].
   */
  ARAD_PP_FP_KEY_CHANGE_LOCATION_A_43_32 = 2,
  /*
   *  Key B [11:0].
   */
  ARAD_PP_FP_KEY_CHANGE_LOCATION_B_11_0 = 3,
  /*
   *  Key B [43:32].
   */
  ARAD_PP_FP_KEY_CHANGE_LOCATION_B_43_32 = 4,
  /*
   *  L2 [11:0].
   */
  ARAD_PP_FP_KEY_CHANGE_LOCATION_L2_11_0 = 5,
  /*
   *  L3 IPv4 [11:0].
   */
  ARAD_PP_FP_KEY_CHANGE_LOCATION_L3_IPV4_11_0 = 6,
  /*
   *  L3 IPv6 [11:0].
   */
  ARAD_PP_FP_KEY_CHANGE_LOCATION_L3_IPV6_11_0 = 7,
  /*
   *  Number of types in ARAD_PP_FP_KEY_CHANGE_LOCATION
   */
  ARAD_PP_NOF_FP_KEY_CHANGE_LOCATIONS = 8
}ARAD_PP_FP_KEY_CHANGE_LOCATION;


typedef enum
{
  /*
   *  For macro SOC_PPD_FP_QUAL_VAL_ENCODE_FWD_DECISION.
   */
  ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_FWD_DECISION = 0,
  /*
   *  For macro SOC_PPD_FP_QUAL_VAL_ENCODE_MAC_ADDRESS.
   */
  ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_MAC_ADDRESS = 1,
  /*
   *  For macro SOC_PPD_FP_QUAL_VAL_ENCODE_IPV4_SUBNET.
   */
  ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_IPV4_SUBNET = 2,
  /*
   *  For macro SOC_PPD_FP_QUAL_VAL_ENCODE_IPV6_SUBNET.
   */
  ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_IPV6_SUBNET = 3,
  /*
   *  For macro SOC_PPD_FP_QUAL_VAL_ENCODE_ETH_TAG_FORMAT.
   */
  ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_ETH_TAG_FORMAT = 4,
  /*
   *  For macro SOC_PPD_FP_QUAL_VAL_ENCODE_FTMH.
   */
  ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE_FTMH = 5,
  /*
   *  Number of types in ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE
   */
  ARAD_PP_NOF_FP_QUAL_VAL_ENCODE_INFO_TYPES = 6
}ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE ;

typedef struct
{
  ARAD_PP_FRWRD_DECISION_INFO fwd_dec;
  uint32 dest_nof_bits;
} ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_FWD_DECISION;

typedef struct
{
  SOC_SAND_PP_MAC_ADDRESS  mac;
  SOC_SAND_PP_MAC_ADDRESS  is_valid;
} ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_MAC;

typedef struct
{
  SOC_SAND_PP_IPV4_ADDRESS ip;
  uint32 subnet_length;
} ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_IPV4;

typedef struct
{
  SOC_SAND_PP_IPV6_ADDRESS ip;
  uint32 subnet_length;
  uint8 is_low;
} ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_IPV6;

typedef struct
{
  ARAD_PP_FP_ETH_TAG_FORMAT tag_format;
} ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_ETF;

typedef struct
{
  ARAD_PP_FP_FTMH ftmh;
} ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_FTMH;

typedef union
{
  ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_FWD_DECISION fd;
  ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_MAC mac;
  ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_IPV4 ipv4;
  ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_IPV6 ipv6;
  ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_ETF etf;
  ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_VAL_FTMH ftmh;

} ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_VAL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The key value of teh direct table entry.
   */
  uint32 value;
  /*
   *  The mask for direct table entry key.
   */
  uint32 mask;

} ARAD_PP_FP_DT_KEY;

/* 
 * This union is defined to allow usage of the TCAM location hash table
 * for field processor direct table keys.
 */
typedef union
{
  ARAD_TCAM_LOCATION location;
  ARAD_PP_FP_DT_KEY entry_dt_key;
}ARAD_PP_FP_LOCATION_HASH;

typedef struct
{
  /*
   *  Qual val encode type: MAC, IPv4, ..
   */
  ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_TYPE type;
  /*
   *  Qualifier value (union according to type).
   */
  ARAD_PP_FP_QUAL_VAL_ENCODE_INFO_VAL val;

} ARAD_PP_FP_QUAL_VAL_ENCODE_INFO;

typedef SOC_PPC_FP_DIAG_PP_PORT_INFO                           ARAD_PP_FP_DIAG_PP_PORT_INFO;
typedef SOC_PPC_FP_PACKET_DIAG_PARSER                          ARAD_PP_FP_PACKET_DIAG_PARSER;
typedef SOC_PPC_FP_PACKET_DIAG_PGM                             ARAD_PP_FP_PACKET_DIAG_PGM;
typedef SOC_PPC_FP_PACKET_DIAG_QUAL                            ARAD_PP_FP_PACKET_DIAG_QUAL;
typedef SOC_PPC_FP_PACKET_DIAG_DB_QUAL                         ARAD_PP_FP_PACKET_DIAG_DB_QUAL;
typedef SOC_PPC_FP_PACKET_DIAG_KEY                             ARAD_PP_FP_PACKET_DIAG_KEY;
typedef SOC_PPC_FP_PACKET_DIAG_TCAM_DT                         ARAD_PP_FP_PACKET_DIAG_TCAM_DT;
typedef SOC_PPC_FP_PACKET_DIAG_MACRO                           ARAD_PP_FP_PACKET_DIAG_MACRO;
typedef SOC_PPC_FP_PACKET_DIAG_INFO                            ARAD_PP_FP_PACKET_DIAG_INFO;
typedef SOC_PPC_FP_PACKET                                      ARAD_PP_FP_PACKET;

typedef SOC_PPC_FP_RESOURCE_KEY_LSB_MSB 					   ARAD_PP_FP_RESOURCE_KEY_LSB_MSB;
typedef SOC_PPC_FP_RESOURCE_KEY 						   	   ARAD_PP_FP_RESOURCE_KEY;
typedef SOC_PPC_FP_RESOURCE_DB_BANK 						   ARAD_PP_FP_RESOURCE_DB_BANK;
typedef SOC_PPC_FP_RESOURCE_ACTION 						   	   ARAD_PP_FP_RESOURCE_ACTION;
typedef SOC_PPC_FP_RESOURCE_DB_TCAM 						   ARAD_PP_FP_RESOURCE_DB_TCAM;
typedef SOC_PPC_FP_RESOURCE_DB_DE 						       ARAD_PP_FP_RESOURCE_DB_DE;
typedef SOC_PPC_FP_RESOURCE_DB 						   		   ARAD_PP_FP_RESOURCE_DB;
typedef SOC_PPC_FP_RESOURCE_BANK_DB 						   ARAD_PP_FP_RESOURCE_BANK_DB;
typedef SOC_PPC_FP_RESOURCE_BANK 						       ARAD_PP_FP_RESOURCE_BANK;
typedef SOC_PPC_FP_RESOURCE_PRESEL 						   	   ARAD_PP_FP_RESOURCE_PRESEL;
typedef SOC_PPC_FP_RESOURCE_DIAG_INFO	 					   ARAD_PP_FP_RESOURCE_DIAG_INFO;

#define ARAD_PP_FP_RESOURCE_MODE_USAGE                         SOC_PPC_FP_RESOURCE_MODE_USAGE
#define ARAD_PP_FP_RESOURCE_MODE_WITH_AVAILABLE_RESOURCES      SOC_PPC_FP_RESOURCE_MODE_WITH_AVAILABLE_RESOURCES
#define ARAD_PP_FP_RESOURCE_MODE_DIAG                          SOC_PPC_FP_RESOURCE_MODE_DIAG 
#define ARAD_PP_FP_RESOURCE_MODE_ALL                           SOC_PPC_FP_RESOURCE_MODE_ALL 
typedef SOC_PPC_FP_RESOURCE_DIAG_MODE 						   ARAD_PP_FP_RESOURCE_DIAG_MODE;

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
 *   arad_pp_fp_packet_format_group_set
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
 *   SOC_SAND_IN  ARAD_PP_FP_PFG_INFO            *info -
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
  arad_pp_fp_packet_format_group_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_IN  ARAD_PP_FP_PFG_INFO            *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_fp_packet_format_group_set" API.
 *     Refer to "arad_pp_fp_packet_format_group_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_fp_packet_format_group_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pfg_ndx,
    SOC_SAND_OUT ARAD_PP_FP_PFG_INFO            *info
  );

/*********************************************************************
* NAME:
 *   arad_pp_fp_database_create
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
 *   SOC_SAND_IN  ARAD_PP_FP_DATABASE_INFO                    *info -
 *     Database parameters.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Indicate if the database is created successfully.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_fp_database_create(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  ARAD_PP_FP_DATABASE_INFO                    *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_fp_database_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the database parameters.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_OUT ARAD_PP_FP_DATABASE_INFO                    *info -
 *     Database parameters.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_fp_database_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_OUT ARAD_PP_FP_DATABASE_INFO                    *info
  );

/*********************************************************************
* NAME:
 *   arad_pp_fp_database_destroy
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
  arad_pp_fp_database_destroy(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx
  );

/*********************************************************************
* NAME:
 *   arad_pp_fp_entry_add
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
 *   SOC_SAND_IN  ARAD_PP_FP_ENTRY_INFO                       *info -
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
  arad_pp_fp_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  ARAD_PP_FP_ENTRY_INFO                       *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_fp_entry_get
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
 *   SOC_SAND_OUT ARAD_PP_FP_ENTRY_INFO                       *info -
 *     Entry parameters. Meaningful only the entry is found.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_fp_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_OUT uint8                                  *is_found,
    SOC_SAND_INOUT ARAD_PP_FP_ENTRY_INFO                *info
  );

/*********************************************************************
* NAME:
 *   arad_pp_fp_entry_remove
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
  arad_pp_fp_entry_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  uint32                                 is_sw_remove_only
  );

uint32 
    arad_pp_fp_entry_remove_by_key(
       SOC_SAND_IN  int                               unit,
       SOC_SAND_IN  uint32                               db_id_ndx,
       SOC_SAND_INOUT ARAD_PP_FP_ENTRY_INFO              *info
  );

/*********************************************************************
* NAME:
 *   arad_pp_fp_database_entries_get_block
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
 *   SOC_SAND_OUT ARAD_PP_FP_ENTRY_INFO                       *entries -
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
  arad_pp_fp_database_entries_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT ARAD_PP_FP_ENTRY_INFO                       *entries,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

/*********************************************************************
* NAME:
 *   arad_pp_fp_direct_extraction_entry_add
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
 *   SOC_SAND_IN  ARAD_PP_FP_DIR_EXTR_ENTRY_INFO              *info -
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
  arad_pp_fp_direct_extraction_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_IN  ARAD_PP_FP_DIR_EXTR_ENTRY_INFO              *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_fp_direct_extraction_entry_get
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
 *   SOC_SAND_OUT ARAD_PP_FP_DIR_EXTR_ENTRY_INFO              *info -
 *     Entry parameters. Meaningful only the entry is found.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_fp_direct_extraction_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx,
    SOC_SAND_OUT uint8                                 *is_found,
    SOC_SAND_OUT ARAD_PP_FP_DIR_EXTR_ENTRY_INFO              *info
  );

/*********************************************************************
* NAME:
 *   arad_pp_fp_direct_extraction_entry_remove
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
  arad_pp_fp_direct_extraction_entry_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_IN  uint32                                 entry_id_ndx
  );

/*********************************************************************
* NAME:
 *   arad_pp_fp_direct_extraction_db_entries_get
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
 *   SOC_SAND_OUT ARAD_PP_FP_DIR_EXTR_ENTRY_INFO              *entries -
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
  arad_pp_fp_direct_extraction_db_entries_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT ARAD_PP_FP_DIR_EXTR_ENTRY_INFO              *entries,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

/*********************************************************************
* NAME:
 *   arad_pp_fp_control_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set one of the control options.
 * INPUT:
 *   SOC_SAND_IN  int                    unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FP_CONTROL_INDEX       *control_ndx -
 *     Index for the control set API.
 *   SOC_SAND_IN  ARAD_PP_FP_CONTROL_INFO        *info -
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
  arad_pp_fp_control_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  ARAD_PP_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_IN  ARAD_PP_FP_CONTROL_INFO        *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  );

/*********************************************************************
*     Gets the configuration set by the "arad_pp_fp_control_set"
 *     API.
 *     Refer to "arad_pp_fp_control_set" API for details.
*********************************************************************/
uint32
  arad_pp_fp_control_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  ARAD_PP_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_OUT ARAD_PP_FP_CONTROL_INFO        *info
  );

/*********************************************************************
* NAME:
 *   arad_pp_fp_egr_db_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the mapping between the Packet forward type and the
 *   Port profile to the Database-ID.
 * INPUT:
 *   SOC_SAND_IN  int                    unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FP_FWD_TYPE            fwd_type_ndx -
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
  arad_pp_fp_egr_db_map_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_IN  uint32                     db_id
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_fp_egr_db_map_set" API.
 *     Refer to "arad_pp_fp_egr_db_map_set" API for details.
*********************************************************************/
uint32
  arad_pp_fp_egr_db_map_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                    port_profile_ndx,
    SOC_SAND_OUT uint32                     *db_id
  );

/*********************************************************************
*     Compress a TCAM Database: compress the entries to minimum
*     number of banks.
*********************************************************************/
uint32
  arad_pp_fp_database_compress(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  db_id_ndx
  );

/*********************************************************************
* NAME:
 *   arad_pp_fp_packet_diag_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the Field Processing of the last packets.
 * INPUT:
 *   SOC_SAND_IN  int                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT ARAD_PP_FP_PACKET_DIAG_INFO *info -
 *     Field Processor specifications for this packet.
 * REMARKS:
 *   This API must be called during a continuous stream of
 *   the identical packets coming from the same source.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_fp_packet_diag_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT ARAD_PP_FP_PACKET_DIAG_INFO *info
  );

uint32
  arad_pp_fp_dbs_action_info_show(
    SOC_SAND_IN		int unit
  ) ;

uint32
  arad_pp_fp_action_info_show(
    SOC_SAND_IN		int unit
  ) ;

/*********************************************************************
 * NAME:
 *   arad_pp_fp_print_all_fems_for_stage
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Print all FEMs and corresponding DBs for specified stage,
 *   for TM or non-TM (FTMH header)
 * INPUT:
 *   SOC_SAND_IN  int                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_FP_DATABASE_STAGE  stage -
 *     Stage for which to print the corresponding FEMs
 *   SOC_SAND_IN  uint8                   is_for_tm -
 *     If non-zero the print FEMs which are to be used for TM (ITMH header).
 *     Otherwise print FEMs for packets with FTMH header.
 * REMARKS:
 *   See SOC_PPC_FP_ACTION_TYPE, ARAD_FP_DATABASE_STAGE,
 *   ARAD_PP_FP_FEM_ENTRY
 *   arad_pmf_fem_action_width_get_unsafe
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_fp_print_all_fems_for_stage(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN   ARAD_FP_DATABASE_STAGE stage,
    SOC_SAND_IN  uint8                   is_for_tm
  ) ;
/*********************************************************************
 * NAME:
 *   arad_pp_fp_print_fes_info_for_stage
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Print FES info for specified stage and program.
 *   Show corresponding DBs with priorities as well.
 * INPUT:
 *   SOC_SAND_IN  int                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_FP_DATABASE_STAGE  stage -
 *     Stage for which to print the corresponding FESs
 *   SOC_SAND_IN  uint32                  pmf_pgm_ndx -
 *     Program for which to print the corresponding FESs
 * REMARKS:
 *   See SOC_PPC_FP_ACTION_TYPE, ARAD_FP_DATABASE_STAGE,
 *   ARAD_PP_FP_FEM_ENTRY
 *   arad_pmf_fem_action_width_get_unsafe
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_fp_print_fes_info_for_stage(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_FP_DATABASE_STAGE stage,
    SOC_SAND_IN  uint32                 pmf_pgm_ndx
  ) ;

 
uint32
  arad_pp_fp_resource_diag_get(
    SOC_SAND_IN		int						unit,
	SOC_SAND_IN		ARAD_PP_FP_RESOURCE_DIAG_MODE	mode,
	SOC_SAND_OUT 	ARAD_PP_FP_RESOURCE_DIAG_INFO	*info
  );

/*
 * Macro selection
 */
uint32
  arad_pp_fp_qual_type_preset(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_PP_FP_PREDEFINED_ACL_KEY  predefined_key,
    SOC_SAND_OUT ARAD_PP_FP_QUAL_TYPE           qual_types[ARAD_PP_FP_NOF_QUALS_PER_DB_MAX]
  );

uint32
  arad_pp_fp_qual_val_encode(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_PP_FP_QUAL_VAL_ENCODE_INFO *qual_val_encode,
    SOC_SAND_OUT ARAD_PP_FP_QUAL_VAL             *qual_val
  );

uint32
    arad_pp_fp_ire_traffic_send(
        SOC_SAND_IN int          unit,
        SOC_SAND_IN ARAD_PP_FP_PACKET  *packet,
        SOC_SAND_IN int          tx_count,
        SOC_SAND_IN int          core
    );

void
  ARAD_PP_FP_PACKET_clear(
    SOC_SAND_OUT ARAD_PP_FP_PACKET *info
  );

void
  ARAD_PP_FP_QUAL_VAL_clear(
    SOC_SAND_OUT ARAD_PP_FP_QUAL_VAL *info
  );

void
  ARAD_PP_FP_ETH_TAG_FORMAT_clear(
    SOC_SAND_OUT ARAD_PP_FP_ETH_TAG_FORMAT *info
  );

void
  ARAD_PP_FP_PFG_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FP_PFG_INFO *info
  );

void
  ARAD_PP_FP_DATABASE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FP_DATABASE_INFO *info
  );

void
  ARAD_PP_FP_ACTION_VAL_clear(
    SOC_SAND_OUT ARAD_PP_FP_ACTION_VAL *info
  );

void
  ARAD_PP_FP_ENTRY_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FP_ENTRY_INFO *info
  );

void
  ARAD_PP_FP_DIR_EXTR_ACTION_LOC_clear(
    SOC_SAND_OUT ARAD_PP_FP_DIR_EXTR_ACTION_LOC *info
  );

void
  ARAD_PP_FP_DIR_EXTR_ACTION_VAL_clear(
    SOC_SAND_OUT ARAD_PP_FP_DIR_EXTR_ACTION_VAL *info
  );

void
  ARAD_PP_FP_DIR_EXTR_ENTRY_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FP_DIR_EXTR_ENTRY_INFO *info
  );

void
  ARAD_PP_FP_CONTROL_INDEX_clear(
    SOC_SAND_OUT ARAD_PP_FP_CONTROL_INDEX *info
  );

void
  ARAD_PP_FP_CONTROL_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FP_CONTROL_INFO *info
  );

void
  ARAD_PP_FP_DIAG_PP_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FP_DIAG_PP_PORT_INFO *info
  );

void
  ARAD_PP_FP_PACKET_DIAG_PARSER_clear(
    SOC_SAND_OUT ARAD_PP_FP_PACKET_DIAG_PARSER *info
  );

void
  ARAD_PP_FP_PACKET_DIAG_PGM_clear(
    SOC_SAND_OUT ARAD_PP_FP_PACKET_DIAG_PGM *info
  );

void
  ARAD_PP_FP_PACKET_DIAG_QUAL_clear(
    SOC_SAND_OUT ARAD_PP_FP_PACKET_DIAG_QUAL *info
  );

void
  ARAD_PP_FP_PACKET_DIAG_DB_QUAL_clear(
    SOC_SAND_OUT ARAD_PP_FP_PACKET_DIAG_DB_QUAL *info
  );

void
  ARAD_PP_FP_PACKET_DIAG_KEY_clear(
    SOC_SAND_OUT ARAD_PP_FP_PACKET_DIAG_KEY *info
  );

void
  ARAD_PP_FP_PACKET_DIAG_TCAM_DT_clear(
    SOC_SAND_OUT ARAD_PP_FP_PACKET_DIAG_TCAM_DT *info
  );

void
  ARAD_PP_FP_PACKET_DIAG_MACRO_clear(
    SOC_SAND_OUT ARAD_PP_FP_PACKET_DIAG_MACRO *info
  );

void
  ARAD_PP_FP_PACKET_DIAG_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FP_PACKET_DIAG_INFO *info
  );

void
  ARAD_PP_FP_RESOURCE_KEY_LSB_MSB_clear(
	  SOC_SAND_OUT ARAD_PP_FP_RESOURCE_KEY_LSB_MSB *info
	);

void
  ARAD_PP_FP_RESOURCE_KEY_clear(
    SOC_SAND_OUT ARAD_PP_FP_RESOURCE_KEY *info
  );

void
  ARAD_PP_FP_RESOURCE_DB_BANK_clear(
    SOC_SAND_OUT ARAD_PP_FP_RESOURCE_DB_BANK *info
  );

void
  ARAD_PP_FP_RESOURCE_ACTION_clear(
    SOC_SAND_OUT ARAD_PP_FP_RESOURCE_ACTION *info
  );

void
  ARAD_PP_FP_RESOURCE_DB_TCAM_clear(
    SOC_SAND_OUT ARAD_PP_FP_RESOURCE_DB_TCAM *info
  );

void
  ARAD_PP_FP_RESOURCE_DB_DE_clear(
    SOC_SAND_OUT ARAD_PP_FP_RESOURCE_DB_DE *info
  );

void
  ARAD_PP_FP_RESOURCE_DB_clear(
    SOC_SAND_OUT ARAD_PP_FP_RESOURCE_DB *info
  );

void
  ARAD_PP_FP_RESOURCE_BANK_DB_clear(
    SOC_SAND_OUT ARAD_PP_FP_RESOURCE_BANK_DB *info
  );

void
  ARAD_PP_FP_RESOURCE_BANK_clear(
    SOC_SAND_OUT ARAD_PP_FP_RESOURCE_BANK *info
  );

void
  ARAD_PP_FP_RESOURCE_PRESEL_clear(
    SOC_SAND_OUT ARAD_PP_FP_RESOURCE_PRESEL *info
  );

void
  ARAD_PP_FP_RESOURCE_DIAG_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FP_RESOURCE_DIAG_INFO *info
  );

void
  ARAD_PP_FP_FTMH_clear(
    SOC_SAND_OUT ARAD_PP_FP_FTMH *info
  );

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_FP_QUAL_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FP_QUAL_TYPE enum_val
  );

const char*
  ARAD_PP_FP_PKT_HDR_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FP_PKT_HDR_TYPE enum_val
  );

const char*
  ARAD_PP_FP_FWD_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FP_FWD_TYPE enum_val
  );

const char*
  ARAD_PP_FP_PROCESSING_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FP_PROCESSING_TYPE enum_val
  );

const char*
  ARAD_PP_FP_BASE_HEADER_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FP_BASE_HEADER_TYPE enum_val
  );

const char*
  ARAD_PP_FP_ACTION_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FP_ACTION_TYPE enum_val
  );

const char*
  ARAD_PP_FP_DATABASE_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FP_DATABASE_TYPE enum_val
  );

const char*
  ARAD_PP_FP_PREDEFINED_ACL_KEY_to_string(
    SOC_SAND_IN  ARAD_PP_FP_PREDEFINED_ACL_KEY enum_val
  );

const char*
  ARAD_PP_FP_CONTROL_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_FP_CONTROL_TYPE enum_val
  );

void
  ARAD_PP_FP_PACKET_print(
    SOC_SAND_IN  ARAD_PP_FP_PACKET *info
  );

void
  ARAD_PP_FP_QUAL_VAL_print(
    SOC_SAND_IN  ARAD_PP_FP_QUAL_VAL *info
  );

void
  ARAD_PP_FP_PFG_INFO_print(
    SOC_SAND_IN  ARAD_PP_FP_PFG_INFO *info
  );

void
  ARAD_PP_FP_DATABASE_INFO_print(
    SOC_SAND_IN  ARAD_PP_FP_DATABASE_INFO *info
  );

void
  ARAD_PP_FP_ACTION_VAL_print(
    SOC_SAND_IN  ARAD_PP_FP_ACTION_VAL *info
  );

void
  ARAD_PP_FP_ENTRY_INFO_print(
    SOC_SAND_IN  ARAD_PP_FP_ENTRY_INFO *info
  );

void
  ARAD_PP_FP_DIR_EXTR_ACTION_LOC_print(
    SOC_SAND_IN  ARAD_PP_FP_DIR_EXTR_ACTION_LOC *info
  );

void
  ARAD_PP_FP_DIR_EXTR_ACTION_VAL_print(
    SOC_SAND_IN  ARAD_PP_FP_DIR_EXTR_ACTION_VAL *info
  );

void
  ARAD_PP_FP_DIR_EXTR_ENTRY_INFO_print(
    SOC_SAND_IN  ARAD_PP_FP_DIR_EXTR_ENTRY_INFO *info
  );

void
  ARAD_PP_FP_CONTROL_INDEX_print(
    SOC_SAND_IN  ARAD_PP_FP_CONTROL_INDEX *info
  );

void
  ARAD_PP_FP_CONTROL_INFO_print(
    SOC_SAND_IN  ARAD_PP_FP_CONTROL_INFO *info
  );
void
  ARAD_PP_FP_DIAG_PP_PORT_INFO_print(
    SOC_SAND_IN  ARAD_PP_FP_DIAG_PP_PORT_INFO *info
  );

void
  ARAD_PP_FP_PACKET_DIAG_PARSER_print(
    SOC_SAND_IN  ARAD_PP_FP_PACKET_DIAG_PARSER *info
  );

void
  ARAD_PP_FP_PACKET_DIAG_PGM_print(
    SOC_SAND_IN  ARAD_PP_FP_PACKET_DIAG_PGM *info
  );

void
  ARAD_PP_FP_PACKET_DIAG_QUAL_print(
    SOC_SAND_IN  ARAD_PP_FP_PACKET_DIAG_QUAL *info
  );

void
  ARAD_PP_FP_PACKET_DIAG_DB_QUAL_print(
    SOC_SAND_IN  uint32                  ind,
    SOC_SAND_IN  ARAD_PP_FP_PACKET_DIAG_DB_QUAL *info
  );

void
  ARAD_PP_FP_PACKET_DIAG_KEY_print(
    SOC_SAND_IN  ARAD_PP_FP_PACKET_DIAG_KEY *info
  );

void
  ARAD_PP_FP_PACKET_DIAG_TCAM_DT_print(
    SOC_SAND_IN  ARAD_PP_FP_PACKET_DIAG_TCAM_DT *info
  );

void
  ARAD_PP_FP_PACKET_DIAG_MACRO_print(
    SOC_SAND_IN  ARAD_PP_FP_PACKET_DIAG_MACRO *info
  );

void
  ARAD_PP_FP_PACKET_DIAG_INFO_print(
    SOC_SAND_IN  ARAD_PP_FP_PACKET_DIAG_INFO *info
  );

void
  ARAD_PP_FP_RESOURCE_KEY_LSB_MSB_print(
	  SOC_SAND_IN ARAD_PP_FP_RESOURCE_KEY_LSB_MSB *info
	);

void
  ARAD_PP_FP_RESOURCE_KEY_print(
    SOC_SAND_IN ARAD_PP_FP_RESOURCE_KEY *info
  );

void
  ARAD_PP_FP_RESOURCE_DB_BANK_print(
    SOC_SAND_IN ARAD_PP_FP_RESOURCE_DB_BANK *info
  );

void
  ARAD_PP_FP_RESOURCE_ACTION_print(
    SOC_SAND_IN ARAD_PP_FP_RESOURCE_ACTION *info
  );

void
  ARAD_PP_FP_RESOURCE_DB_TCAM_print(
    SOC_SAND_IN int                         unit,
    SOC_SAND_IN ARAD_PP_FP_RESOURCE_DB_TCAM *info
  );

void
  ARAD_PP_FP_RESOURCE_DB_DE_print(
    SOC_SAND_IN ARAD_PP_FP_RESOURCE_DB_DE *info
  );

void
  ARAD_PP_FP_RESOURCE_DB_print(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN ARAD_PP_FP_RESOURCE_DB *info
  );

void
  ARAD_PP_FP_RESOURCE_BANK_DB_print(
    SOC_SAND_IN ARAD_PP_FP_RESOURCE_BANK_DB *info
  );

void
  ARAD_PP_FP_RESOURCE_BANK_print(
    SOC_SAND_IN ARAD_PP_FP_RESOURCE_BANK *info
  );

void
  ARAD_PP_FP_RESOURCE_PRESEL_print(
    SOC_SAND_IN ARAD_PP_FP_RESOURCE_PRESEL *info
  );

void
  ARAD_PP_FP_RESOURCE_DIAG_INFO_print(
    SOC_SAND_IN int                            unit,
    SOC_SAND_IN ARAD_PP_FP_RESOURCE_DIAG_INFO *info
  );

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_FP_INCLUDED__*/


#endif
