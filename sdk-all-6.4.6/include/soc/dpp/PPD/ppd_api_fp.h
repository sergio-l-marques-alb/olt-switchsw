/* $Id: ppd_api_fp.h,v 1.59 Broadcom SDK $
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

#ifndef __SOC_PPD_API_FP_INCLUDED__
/* { */
#define __SOC_PPD_API_FP_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>

#include <soc/dpp/PPC/ppc_api_fp.h>
#include <soc/dpp/TMC/tmc_pmf_pgm_mgmt.h>

#include <soc/dpp/PPD/ppd_api_general.h>

#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_PP/pb_pp_fp.h>
#endif

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximum number of Qualifier fields per Database.        */
#define  SOC_PPD_FP_NOF_QUALS_PER_DB_MAX (SOC_PPC_FP_NOF_QUALS_PER_DB_MAX)

/*     Maximum number of Actions per Database.                 */
#define  SOC_PPD_FP_NOF_ACTIONS_PER_DB_MAX (SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX)

/*     Maximum number of field values that can be extracted to
 *     build the action value.                                 */
#define  SOC_PPD_FP_DIR_EXTR_MAX_NOF_FIELDS (SOC_PPC_FP_DIR_EXTR_MAX_NOF_FIELDS)

/*     Maximum number of Control values.                       */

/*     Maximum number of Databases.                            */
#define  SOC_PPD_FP_NOF_DBS (SOC_PPC_FP_NOF_DBS)

/*     Number of cycles in TCAM and Macros.                    */

/*     Number of TCAM Banks.                                   */

/*     Number of Macros.                                       */

/*     Maxmimum number of TCAM Entries per Database.           */

/* flag determine if the offset value is negative */
#define SOC_PPD_FP_CONTROL_TYPE_HDR_USER_DEF_FLAG_NEGATIVE  SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF_FLAG_NEGATIVE

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* $Id: ppd_api_fp.h,v 1.59 Broadcom SDK $
 *  Set the list of admissible qualifier types for databases based
 *  on predefined keys. Must be called during the Database create
 *  API to set the Database qualifier types.
 */
#define SOC_PPD_FP_QUAL_TYPE_PRESET(unit, predefined_key, qual_types, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_FUNC_CALL(unit, FP_QUAL_TYPE_PRESET, (unit, predefined_key, qual_types), res);

/*
 *  Encode the Forward Decision 'fwd_dec' in a Qualifier value
 *  format. If an entry Qualifier value is set to 'qual_val',
 *  then only packets with this Forwarding Decision are qualified.
 */
#define SOC_PPD_FP_QUAL_VAL_ENCODE_FWD_DECISION(unit, fwd_dec, dest_nof_bits, qual_val_dest, qual_val_add, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_FUNC_CALL(unit, FP_QUAL_VAL_ENCODE_FWD_DECISION, (unit, fwd_dec, dest_nof_bits, qual_val_dest, qual_val_add), res);

/*
 *  Encode the MAC-Address 'mac' and the 'is_valid' mask in a
 *  Qualifier value format. If an entry Qualifier value is set to
 *  'qual_val', then only packets with a MAC-Address equals to 'mac'
 *  - only for bits set by 'is_valid' - are qualified.
 */
#define SOC_PPD_FP_QUAL_VAL_ENCODE_MAC_ADDRESS(unit, mac, is_valid, qual_val, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_FUNC_CALL(unit, FP_QUAL_VAL_ENCODE_MAC_ADDRESS ,(unit, mac, is_valid, qual_val), res);

/*
 *  Encode the IP Subnet (according to the IP-Address 'ip' and the
 *  'subnet_length') in a Qualifier value format. If an entry
 *  Qualifier value is set to 'qual_val', then only packets with an
 *  IP-Address in the subnet defined by the IP-Address 'ip' and the
 *  'subnet_length' are qualified.
 */
#define SOC_PPD_FP_QUAL_VAL_ENCODE_IPV4_SUBNET(unit, ip, subnet_length, qual_val, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_FUNC_CALL(unit, FP_QUAL_VAL_ENCODE_IPV4_SUBNET, (unit, ip, subnet_length, qual_val), res);

/*
 *   Encode the IP Subnet (according to the IP-Address 'ip' and the
 *  'subnet_length', and also the 'is_low' parameter which sets if it
 *  relates to the 64 LSBs - low part-) in a Qualifier value format.
 *  If an entry Qualifier value is set to 'qual_val', then only packets
 *  with an IP-Address in the subnet defined by the IP-Address 'ip'
 *  and the 'subnet_length' are qualified.
 */
#define SOC_PPD_FP_QUAL_VAL_ENCODE_IPV6_SUBNET(unit, ip, subnet_length, is_low, qual_val, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_FUNC_CALL(unit, FP_QUAL_VAL_ENCODE_IPV6_SUBNET, (unit, ip, subnet_length, is_low, qual_val), res);

/*
 *  Encode the Ethernet Tag format (according to the parameter
 *  'eth_tag_format') in a Qualifier value format.
 */
#define SOC_PPD_FP_QUAL_VAL_ENCODE_ETH_TAG_FORMAT(unit, tag_format, qual_val, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_FUNC_CALL(FP_QUAL_VAL_ENCODE_ETH_TAG_FORMAT, (unit, tag_format, qual_val), res);

/*
 *  Encode the FTMH (according to the parameter
 *  'ftmh') in a Qualifier value format.
 */
#define SOC_PPD_FP_QUAL_VAL_ENCODE_FTMH(unit, ftmh, qual_val, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_FUNC_CALL(unit, FP_QUAL_VAL_ENCODE_FTMH, (unit, ftmh, qual_val), res);

/*
 *  Set the list of the egress action types for Egress databases.
 */
#define SOC_PPD_FP_EGRESS_ACTION_TYPE_PRESET(unit, action_types, res)  \
	SOC_PPD_ARAD_TMP_DEVICE_FUNC_CALL(unit, FP_EGRESS_ACTION_TYPE_PRESET, (unit, action_types), res);

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
  SOC_PPD_FP_PACKET_FORMAT_GROUP_SET = SOC_PPD_PROC_DESC_BASE_FP_FIRST,
  SOC_PPD_FP_PACKET_FORMAT_GROUP_SET_PRINT,
  SOC_PPD_FP_PACKET_FORMAT_GROUP_GET,
  SOC_PPD_FP_PACKET_FORMAT_GROUP_GET_PRINT,
  SOC_PPD_FP_DATABASE_CREATE,
  SOC_PPD_FP_DATABASE_CREATE_PRINT,
  SOC_PPD_FP_DATABASE_GET,
  SOC_PPD_FP_DATABASE_GET_PRINT,
  SOC_PPD_FP_DATABASE_DESTROY,
  SOC_PPD_FP_DATABASE_DESTROY_PRINT,
  SOC_PPD_FP_ENTRY_ADD,
  SOC_PPD_FP_ENTRY_ADD_PRINT,
  SOC_PPD_FP_ENTRY_GET,
  SOC_PPD_FP_ENTRY_GET_PRINT,
  SOC_PPD_FP_ENTRY_REMOVE,
  SOC_PPD_FP_ENTRY_REMOVE_PRINT,
  SOC_PPD_FP_DATABASE_ENTRIES_GET_BLOCK,
  SOC_PPD_FP_DATABASE_ENTRIES_GET_BLOCK_PRINT,
  SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD,
  SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_ADD_PRINT,
  SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_GET,
  SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_GET_PRINT,
  SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_REMOVE,
  SOC_PPD_FP_DIRECT_EXTRACTION_ENTRY_REMOVE_PRINT,
  SOC_PPD_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET,
  SOC_PPD_FP_DIRECT_EXTRACTION_DB_ENTRIES_GET_PRINT,
  SOC_PPD_FP_CONTROL_SET,
  SOC_PPD_FP_CONTROL_SET_PRINT,
  SOC_PPD_FP_CONTROL_GET,
  SOC_PPD_FP_CONTROL_GET_PRINT,
  SOC_PPD_FP_EGR_DB_MAP_SET,
  SOC_PPD_FP_EGR_DB_MAP_SET_PRINT,
  SOC_PPD_FP_EGR_DB_MAP_GET,
  SOC_PPD_FP_EGR_DB_MAP_GET_PRINT,
  SOC_PPD_FP_PACKET_DIAG_GET,
  SOC_PPD_FP_PACKET_DIAG_GET_PRINT,
  SOC_PPD_FP_RESOURCE_DIAG_GET,
  SOC_PPD_FP_RESOURCE_DIAG_GET_PRINT,
  SOC_PPD_FP_GET_PROCS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PPD_FP_PROCEDURE_DESC_LAST
} SOC_PPD_FP_PROCEDURE_DESC;

typedef SOC_PPC_FP_FTMH                                        SOC_PPD_FP_FTMH;


#define SOC_PPD_FP_QUAL_HDR_FWD_VLAN_TAG                       SOC_PPC_FP_QUAL_HDR_FWD_VLAN_TAG
#define SOC_PPD_FP_QUAL_HDR_FWD_VLAN_TAG_ID                    SOC_PPC_FP_QUAL_HDR_FWD_VLAN_TAG_ID
#define SOC_PPD_FP_QUAL_HDR_FWD_SA                             SOC_PPC_FP_QUAL_HDR_FWD_SA
#define SOC_PPD_FP_QUAL_HDR_FWD_DA                             SOC_PPC_FP_QUAL_HDR_FWD_DA
#define SOC_PPD_FP_QUAL_HDR_FWD_ETHERTYPE                      SOC_PPC_FP_QUAL_HDR_FWD_ETHERTYPE
#define SOC_PPD_FP_QUAL_HDR_FWD_2ND_VLAN_TAG                   SOC_PPC_FP_QUAL_HDR_FWD_2ND_VLAN_TAG
#define SOC_PPD_FP_QUAL_HDR_INNER_VLAN_TAG                     SOC_PPC_FP_QUAL_HDR_INNER_VLAN_TAG
#define SOC_PPD_FP_QUAL_HDR_INNER_SA                           SOC_PPC_FP_QUAL_HDR_INNER_SA
#define SOC_PPD_FP_QUAL_HDR_INNER_DA                           SOC_PPC_FP_QUAL_HDR_INNER_DA
#define SOC_PPD_FP_QUAL_HDR_INNER_ETHERTYPE                    SOC_PPC_FP_QUAL_HDR_INNER_ETHERTYPE
#define SOC_PPD_FP_QUAL_HDR_INNER_2ND_VLAN_TAG                 SOC_PPC_FP_QUAL_HDR_INNER_2ND_VLAN_TAG
#define SOC_PPD_FP_QUAL_HDR_VLAN_FORMAT                        SOC_PPC_FP_QUAL_HDR_VLAN_FORMAT
#define SOC_PPD_FP_QUAL_HDR_VLAN_TAG                           SOC_PPC_FP_QUAL_HDR_VLAN_TAG
#define SOC_PPD_FP_QUAL_HDR_VLAN_TAG_ID                        SOC_PPC_FP_QUAL_HDR_VLAN_TAG_ID
#define SOC_PPD_FP_QUAL_HDR_VLAN_TAG_PRI                       SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI
#define SOC_PPD_FP_QUAL_HDR_VLAN_TAG_CFI                       SOC_PPC_FP_QUAL_HDR_VLAN_TAG_CFI
#define SOC_PPD_FP_QUAL_HDR_VLAN_TAG_PRI_CFI                   SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI_CFI
#define SOC_PPD_FP_QUAL_HDR_VLAN_TAG_TPID                      SOC_PPC_FP_QUAL_HDR_VLAN_TAG_TPID
#define SOC_PPD_FP_QUAL_HDR_SA                                 SOC_PPC_FP_QUAL_HDR_SA
#define SOC_PPD_FP_QUAL_HDR_DA                                 SOC_PPC_FP_QUAL_HDR_DA
#define SOC_PPD_FP_QUAL_HDR_ETHERTYPE                          SOC_PPC_FP_QUAL_HDR_ETHERTYPE
#define SOC_PPD_FP_QUAL_HDR_2ND_VLAN_TAG                       SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG
#define SOC_PPD_FP_QUAL_HDR_2ND_VLAN_TAG_ID                    SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG_ID
#define SOC_PPD_FP_QUAL_HDR_2ND_VLAN_TAG_CFI                   SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG_CFI
#define SOC_PPD_FP_QUAL_HDR_2ND_VLAN_TAG_PRI                   SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG_PRI
#define SOC_PPD_FP_QUAL_HDR_2ND_VLAN_TAG_TPID                  SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG_TPID
#define SOC_PPD_FP_QUAL_HDR_FWD_IPV4_NEXT_PRTCL                SOC_PPC_FP_QUAL_HDR_FWD_IPV4_NEXT_PRTCL
#define SOC_PPD_FP_QUAL_HDR_FWD_IPV4_DF                        SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DF
#define SOC_PPD_FP_QUAL_HDR_FWD_IPV4_MF                        SOC_PPC_FP_QUAL_HDR_FWD_IPV4_MF
#define SOC_PPD_FP_QUAL_HDR_FWD_IPV4_SIP                       SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP
#define SOC_PPD_FP_QUAL_HDR_FWD_IPV4_DIP                       SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP
#define SOC_PPD_FP_QUAL_HDR_FWD_IPV4_SRC_PORT                  SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SRC_PORT
#define SOC_PPD_FP_QUAL_HDR_FWD_IPV4_DEST_PORT                 SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DEST_PORT
#define SOC_PPD_FP_QUAL_HDR_FWD_IPV4_TOS                       SOC_PPC_FP_QUAL_HDR_FWD_IPV4_TOS
#define SOC_PPD_FP_QUAL_HDR_FWD_IPV4_TCP_CTL                   SOC_PPC_FP_QUAL_HDR_FWD_IPV4_TCP_CTL
#define SOC_PPD_FP_QUAL_HDR_FWD_IPV4_IN_VID                    SOC_PPC_FP_QUAL_HDR_FWD_IPV4_IN_VID
#define SOC_PPD_FP_QUAL_HDR_INNER_IPV4_NEXT_PRTCL              SOC_PPC_FP_QUAL_HDR_INNER_IPV4_NEXT_PRTCL
#define SOC_PPD_FP_QUAL_HDR_INNER_IPV4_DF                      SOC_PPC_FP_QUAL_HDR_INNER_IPV4_DF
#define SOC_PPD_FP_QUAL_HDR_INNER_IPV4_MF                      SOC_PPC_FP_QUAL_HDR_INNER_IPV4_MF
#define SOC_PPD_FP_QUAL_HDR_INNER_IPV4_SIP                     SOC_PPC_FP_QUAL_HDR_INNER_IPV4_SIP
#define SOC_PPD_FP_QUAL_HDR_INNER_IPV4_DIP                     SOC_PPC_FP_QUAL_HDR_INNER_IPV4_DIP
#define SOC_PPD_FP_QUAL_HDR_INNER_IPV4_SRC_PORT                SOC_PPC_FP_QUAL_HDR_INNER_IPV4_SRC_PORT
#define SOC_PPD_FP_QUAL_HDR_INNER_IPV4_DEST_PORT               SOC_PPC_FP_QUAL_HDR_INNER_IPV4_DEST_PORT
#define SOC_PPD_FP_QUAL_HDR_INNER_IPV4_TOS                     SOC_PPC_FP_QUAL_HDR_INNER_IPV4_TOS
#define SOC_PPD_FP_QUAL_HDR_INNER_IPV4_TTL                     SOC_PPC_FP_QUAL_HDR_INNER_IPV4_TTL
#define SOC_PPD_FP_QUAL_HDR_INNER_IPV4_TCP_CTL                 SOC_PPC_FP_QUAL_HDR_INNER_IPV4_TCP_CTL
#define SOC_PPD_FP_QUAL_HDR_IPV4_NEXT_PRTCL                    SOC_PPC_FP_QUAL_HDR_IPV4_NEXT_PRTCL
#define SOC_PPD_FP_QUAL_HDR_IPV4_FLAGS                         SOC_PPC_FP_QUAL_HDR_IPV4_FLAGS
#define SOC_PPD_FP_QUAL_HDR_IPV4_DF                            SOC_PPC_FP_QUAL_HDR_IPV4_DF
#define SOC_PPD_FP_QUAL_HDR_IPV4_MF                            SOC_PPC_FP_QUAL_HDR_IPV4_MF
#define SOC_PPD_FP_QUAL_HDR_IPV4_SIP                           SOC_PPC_FP_QUAL_HDR_IPV4_SIP
#define SOC_PPD_FP_QUAL_HDR_IPV4_DIP                           SOC_PPC_FP_QUAL_HDR_IPV4_DIP
#define SOC_PPD_FP_QUAL_HDR_IPV4_SRC_PORT                      SOC_PPC_FP_QUAL_HDR_IPV4_SRC_PORT
#define SOC_PPD_FP_QUAL_HDR_IPV4_DEST_PORT                     SOC_PPC_FP_QUAL_HDR_IPV4_DEST_PORT
#define SOC_PPD_FP_QUAL_HDR_IPV4_TOS                           SOC_PPC_FP_QUAL_HDR_IPV4_TOS
#define SOC_PPD_FP_QUAL_HDR_IPV4_TTL                           SOC_PPC_FP_QUAL_HDR_IPV4_TTL
#define SOC_PPD_FP_QUAL_HDR_IPV4_TCP_CTL                       SOC_PPC_FP_QUAL_HDR_IPV4_TCP_CTL
#define SOC_PPD_FP_QUAL_HDR_IPV4_L4OPS_HI                      SOC_PPC_FP_QUAL_HDR_IPV4_L4OPS_HI
#define SOC_PPD_FP_QUAL_HDR_IPV4_L4OPS_LOW                     SOC_PPC_FP_QUAL_HDR_IPV4_L4OPS_LOW
#define SOC_PPD_FP_QUAL_HDR_IPV6_SIP_HIGH                      SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH
#define SOC_PPD_FP_QUAL_HDR_IPV6_SIP_LOW                       SOC_PPC_FP_QUAL_HDR_IPV6_SIP_LOW
#define SOC_PPD_FP_QUAL_HDR_IPV6_DIP_HIGH                      SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH
#define SOC_PPD_FP_QUAL_HDR_IPV6_DIP_LOW                       SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW
#define SOC_PPD_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH                  SOC_PPC_FP_QUAL_HDR_IPV6_FWD_SIP_HIGH
#define SOC_PPD_FP_QUAL_HDR_FWD_IPV6_SIP_LOW                   SOC_PPC_FP_QUAL_HDR_IPV6_FWD_SIP_LOW
#define SOC_PPD_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH                  SOC_PPC_FP_QUAL_HDR_IPV6_FWD_DIP_HIGH
#define SOC_PPD_FP_QUAL_HDR_FWD_IPV6_DIP_LOW                   SOC_PPC_FP_QUAL_HDR_IPV6_FWD_DIP_LOW
#define SOC_PPD_FP_QUAL_HDR_IPV6_NEXT_PRTCL                    SOC_PPC_FP_QUAL_HDR_IPV6_NEXT_PRTCL
#define SOC_PPD_FP_QUAL_HDR_IPV6_TCP_CTL                       SOC_PPC_FP_QUAL_HDR_IPV6_TCP_CTL
#define SOC_PPD_FP_QUAL_HDR_IPV6_L4OPS                         SOC_PPC_FP_QUAL_HDR_IPV6_L4OPS
#define SOC_PPD_FP_QUAL_HDR_IPV6_TC                            SOC_PPC_FP_QUAL_HDR_IPV6_TC
#define SOC_PPD_FP_QUAL_HDR_IPV6_FLOW_LABEL                    SOC_PPC_FP_QUAL_HDR_IPV6_FLOW_LABEL
#define SOC_PPD_FP_QUAL_HDR_IPV6_HOP_LIMIT                     SOC_PPC_FP_QUAL_HDR_IPV6_HOP_LIMIT
#define SOC_PPD_FP_QUAL_HDR_MPLS_LABEL_FWD                     SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_FWD
#define SOC_PPD_FP_QUAL_HDR_MPLS_LABEL_ID_FWD                  SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD
#define SOC_PPD_FP_QUAL_HDR_MPLS_EXP_FWD                       SOC_PPC_FP_QUAL_HDR_MPLS_EXP_FWD
#define SOC_PPD_FP_QUAL_HDR_MPLS_TTL_FWD                       SOC_PPC_FP_QUAL_HDR_MPLS_TTL_FWD
#define SOC_PPD_FP_QUAL_HDR_MPLS_BOS_FWD                       SOC_PPC_FP_QUAL_HDR_MPLS_BOS_FWD
#define SOC_PPD_FP_QUAL_HDR_MPLS_LABEL1                        SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1
#define SOC_PPD_FP_QUAL_HDR_MPLS_LABEL1_ID                     SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1_ID
#define SOC_PPD_FP_QUAL_HDR_MPLS_EXP1                          SOC_PPC_FP_QUAL_HDR_MPLS_EXP1
#define SOC_PPD_FP_QUAL_HDR_MPLS_TTL1                          SOC_PPC_FP_QUAL_HDR_MPLS_TTL1
#define SOC_PPD_FP_QUAL_HDR_MPLS_BOS1                          SOC_PPC_FP_QUAL_HDR_MPLS_BOS1
#define SOC_PPD_FP_QUAL_HDR_MPLS_LABEL2                        SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2
#define SOC_PPD_FP_QUAL_HDR_MPLS_LABEL2_ID                     SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2_ID
#define SOC_PPD_FP_QUAL_HDR_MPLS_EXP2                          SOC_PPC_FP_QUAL_HDR_MPLS_EXP2
#define SOC_PPD_FP_QUAL_HDR_MPLS_TTL2                          SOC_PPC_FP_QUAL_HDR_MPLS_TTL2
#define SOC_PPD_FP_QUAL_HDR_MPLS_BOS2                          SOC_PPC_FP_QUAL_HDR_MPLS_BOS2
#define SOC_PPD_FP_QUAL_HDR_MPLS_LABEL3                        SOC_PPC_FP_QUAL_HDR_MPLS_LABEL3
#define SOC_PPD_FP_QUAL_HDR_MPLS_LABEL3_ID                     SOC_PPC_FP_QUAL_HDR_MPLS_LABEL3_ID
#define SOC_PPD_FP_QUAL_HDR_MPLS_EXP3                          SOC_PPC_FP_QUAL_HDR_MPLS_EXP3
#define SOC_PPD_FP_QUAL_HDR_MPLS_TTL3                          SOC_PPC_FP_QUAL_HDR_MPLS_TTL3
#define SOC_PPD_FP_QUAL_HDR_MPLS_BOS3                          SOC_PPC_FP_QUAL_HDR_MPLS_BOS3
#define SOC_PPD_FP_QUAL_IRPP_SRC_TM_PORT                       SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT
#define SOC_PPD_FP_QUAL_IRPP_SRC_SYST_PORT                     SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT
#define SOC_PPD_FP_QUAL_IRPP_SRC_PP_PORT                       SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT
#define SOC_PPD_FP_QUAL_IRPP_PROCESSING_TYPE                   SOC_PPC_FP_QUAL_IRPP_PROCESSING_TYPE
#define SOC_PPD_FP_QUAL_IRPP_PKT_HDR_TYPE                      SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE
#define SOC_PPD_FP_QUAL_IRPP_ETH_TAG_FORMAT                    SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT
#define SOC_PPD_FP_QUAL_IRPP_FWD_DEC_DEST                      SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST
#define SOC_PPD_FP_QUAL_IRPP_FWD_DEC_TC                        SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC
#define SOC_PPD_FP_QUAL_IRPP_FWD_DEC_DP                        SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP
#define SOC_PPD_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE             SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE
#define SOC_PPD_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_QUAL             SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_QUAL
#define SOC_PPD_FP_QUAL_IRPP_UP                                SOC_PPC_FP_QUAL_IRPP_UP
#define SOC_PPD_FP_QUAL_IRPP_SNOOP_CODE                        SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE
#define SOC_PPD_FP_QUAL_IRPP_LEARN_DECISION_DEST               SOC_PPC_FP_QUAL_IRPP_LEARN_DECISION_DEST
#define SOC_PPD_FP_QUAL_IRPP_LEARN_ADD_INFO                    SOC_PPC_FP_QUAL_IRPP_LEARN_ADD_INFO
#define SOC_PPD_FP_QUAL_IRPP_IN_LIF                            SOC_PPC_FP_QUAL_IRPP_IN_LIF
#define SOC_PPD_FP_QUAL_IRPP_LL_MIRROR_CMD                     SOC_PPC_FP_QUAL_IRPP_LL_MIRROR_CMD
#define SOC_PPD_FP_QUAL_IRPP_SYSTEM_VSI                        SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI
#define SOC_PPD_FP_QUAL_IRPP_ORIENTATION_IS_HUB                SOC_PPC_FP_QUAL_IRPP_ORIENTATION_IS_HUB
#define SOC_PPD_FP_QUAL_IRPP_VLAN_ID                           SOC_PPC_FP_QUAL_IRPP_VLAN_ID
#define SOC_PPD_FP_QUAL_IRPP_VLAN_PCP                          SOC_PPC_FP_QUAL_IRPP_VLAN_PCP
#define SOC_PPD_FP_QUAL_IRPP_VLAN_DEI                          SOC_PPC_FP_QUAL_IRPP_VLAN_DEI
#define SOC_PPD_FP_QUAL_IRPP_STP_STATE                         SOC_PPC_FP_QUAL_IRPP_STP_STATE
#define SOC_PPD_FP_QUAL_IRPP_FWD_TYPE                          SOC_PPC_FP_QUAL_IRPP_FWD_TYPE
#define SOC_PPD_FP_QUAL_IRPP_SUB_HEADER_NDX                    SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX
#define SOC_PPD_FP_QUAL_IRPP_KEY_CHANGED                       SOC_PPC_FP_QUAL_IRPP_KEY_CHANGED
#define SOC_PPD_FP_QUAL_IRPP_IN_RIF                            SOC_PPC_FP_QUAL_IRPP_IN_RIF
#define SOC_PPD_FP_QUAL_IRPP_VRF                               SOC_PPC_FP_QUAL_IRPP_VRF
#define SOC_PPD_FP_QUAL_IRPP_PCKT_IS_COMP_MC                   SOC_PPC_FP_QUAL_IRPP_PCKT_IS_COMP_MC
#define SOC_PPD_FP_QUAL_IRPP_MY_BMAC                           SOC_PPC_FP_QUAL_IRPP_MY_BMAC
#define SOC_PPD_FP_QUAL_IRPP_IN_TTL                            SOC_PPC_FP_QUAL_IRPP_IN_TTL
#define SOC_PPD_FP_QUAL_IRPP_IN_DSCP_EXP                       SOC_PPC_FP_QUAL_IRPP_IN_DSCP_EXP
#define SOC_PPD_FP_QUAL_IRPP_PACKET_SIZE_RANGE                 SOC_PPC_FP_QUAL_IRPP_PACKET_SIZE_RANGE
#define SOC_PPD_FP_QUAL_IRPP_TERM_TYPE                         SOC_PPC_FP_QUAL_IRPP_TERM_TYPE
#define SOC_PPD_FP_QUAL_ERPP_PP_PORT_DATA                      SOC_PPC_FP_QUAL_ERPP_PP_PORT_DATA
#define SOC_PPD_FP_QUAL_ERPP_IPV4_NEXT_PROTOCOL                SOC_PPC_FP_QUAL_ERPP_IPV4_NEXT_PROTOCOL
#define SOC_PPD_FP_QUAL_ERPP_FTMH                              SOC_PPC_FP_QUAL_ERPP_FTMH
#define SOC_PPD_FP_QUAL_ERPP_PAYLOAD                           SOC_PPC_FP_QUAL_ERPP_PAYLOAD
#define SOC_PPD_FP_QUAL_HDR_USER_DEF_0                         SOC_PPC_FP_QUAL_HDR_USER_DEF_0
#define SOC_PPD_FP_QUAL_HDR_USER_DEF_1                         SOC_PPC_FP_QUAL_HDR_USER_DEF_1
#define SOC_PPD_FP_QUAL_HDR_USER_DEF_2                         SOC_PPC_FP_QUAL_HDR_USER_DEF_2
#define SOC_PPD_FP_QUAL_HDR_USER_DEF_3                         SOC_PPC_FP_QUAL_HDR_USER_DEF_3
#define SOC_PPD_FP_QUAL_HDR_USER_DEF_4                         SOC_PPC_FP_QUAL_HDR_USER_DEF_4
#define SOC_PPD_FP_QUAL_HDR_USER_DEF_5                         SOC_PPC_FP_QUAL_HDR_USER_DEF_5
#define SOC_PPD_FP_QUAL_HDR_USER_DEF_6                         SOC_PPC_FP_QUAL_HDR_USER_DEF_6
#define SOC_PPD_FP_QUAL_HDR_USER_DEF_7                         SOC_PPC_FP_QUAL_HDR_USER_DEF_7
#define SOC_PPD_FP_QUAL_HDR_USER_DEF_8                         SOC_PPC_FP_QUAL_HDR_USER_DEF_8
#define SOC_PPD_FP_QUAL_HDR_USER_DEF_9                         SOC_PPC_FP_QUAL_HDR_USER_DEF_9
#define SOC_PPD_FP_QUAL_HDR_USER_DEF_LAST                      SOC_PPC_FP_QUAL_HDR_USER_DEF_LAST
#define SOC_PPD_FP_QUAL_IRPP_LEM_2ND_LKUP_ASD                  SOC_PPC_FP_QUAL_IRPP_LEM_2ND_LKUP_ASD
#define SOC_PPD_FP_QUAL_OUT_LIF                                SOC_PPC_FP_QUAL_OUT_LIF
#define SOC_PPD_FP_QUAL_IRPP_INVALID                           SOC_PPC_FP_QUAL_IRPP_INVALID 
#define SOC_PPD_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR               SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR 
#define SOC_PPD_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR_PS            SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR_PS 
#define SOC_PPD_FP_QUAL_IRPP_PTC_KEY_GEN_VAR                   SOC_PPC_FP_QUAL_IRPP_PTC_KEY_GEN_VAR 
#define SOC_PPD_FP_QUAL_IRPP_ALL_ZEROES                        SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES 
#define SOC_PPD_FP_QUAL_IRPP_ALL_ONES                          SOC_PPC_FP_QUAL_IRPP_ALL_ONES 
#define SOC_PPD_FP_QUAL_HEADER_OFFSET1                         SOC_PPC_FP_QUAL_HEADER_OFFSET1                        
#define SOC_PPD_FP_QUAL_HEADER_OFFSET_0_UNTIL_5                SOC_PPC_FP_QUAL_HEADER_OFFSET_0_UNTIL_5                        
#define SOC_PPD_FP_QUAL_PACKET_FORMAT_QUALIFIER0               SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER0              
#define SOC_PPD_FP_QUAL_PACKET_FORMAT_QUALIFIER1_ENCAPSULATION SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_ENCAPSULATION              
#define SOC_PPD_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL
#define SOC_PPD_FP_QUAL_PACKET_FORMAT_QUALIFIER1_OUTER_TAG     SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_OUTER_TAG
#define SOC_PPD_FP_QUAL_PACKET_FORMAT_QUALIFIER1_PRIORITY SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_PRIORITY
#define SOC_PPD_FP_QUAL_PACKET_FORMAT_QUALIFIER1_INNER_TAG SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_INNER_TAG
#define SOC_PPD_FP_QUAL_PACKET_FORMAT_QUALIFIER2_BOS           SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_BOS              
#define SOC_PPD_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL 
#define SOC_PPD_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_FRAGMENTED SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_FRAGMENTED              
#define SOC_PPD_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_HAS_OPTIONS SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_HAS_OPTIONS
#define SOC_PPD_FP_QUAL_PACKET_FORMAT_QUALIFIER3_IP_FRAGMENTED SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3_IP_FRAGMENTED
#define SOC_PPD_FP_QUAL_FORWARDING_ACTION_METER_TRAFFIC_CLASS  SOC_PPC_FP_QUAL_FORWARDING_ACTION_METER_TRAFFIC_CLASS 
#define SOC_PPD_FP_QUAL_VSI_PROFILE                            SOC_PPC_FP_QUAL_VSI_PROFILE                           
#define SOC_PPD_FP_QUAL_FID                                    SOC_PPC_FP_QUAL_FID                                   
#define SOC_PPD_FP_QUAL_VLAN_EDIT_CMD_VID2                     SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID2                    
#define SOC_PPD_FP_QUAL_VLAN_EDIT_CMD_VID1                     SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID1                    
#define SOC_PPD_FP_QUAL_FORWARDING_OFFSET_EXTENSION            SOC_PPC_FP_QUAL_FORWARDING_OFFSET_EXTENSION           
#define SOC_PPD_FP_QUAL_EEI                                    SOC_PPC_FP_QUAL_EEI                                   
#define SOC_PPD_FP_QUAL_OUT_LIF                                SOC_PPC_FP_QUAL_OUT_LIF                               
#define SOC_PPD_FP_QUAL_RPF_DESTINATION                        SOC_PPC_FP_QUAL_RPF_DESTINATION                       
#define SOC_PPD_FP_QUAL_RPF_DESTINATION_VALID                  SOC_PPC_FP_QUAL_RPF_DESTINATION_VALID                 
#define SOC_PPD_FP_QUAL_INGRESS_LEARN_ENABLE                   SOC_PPC_FP_QUAL_INGRESS_LEARN_ENABLE                  
#define SOC_PPD_FP_QUAL_EGRESS_LEARN_ENABLE                    SOC_PPC_FP_QUAL_EGRESS_LEARN_ENABLE                   
#define SOC_PPD_FP_QUAL_LEARN_KEY_VLAN                         SOC_PPC_FP_QUAL_LEARN_KEY_VLAN                             
#define SOC_PPD_FP_QUAL_LEARN_KEY_MAC                          SOC_PPC_FP_QUAL_LEARN_KEY_MAC                             
#define SOC_PPD_FP_QUAL_IN_LIF_PROFILE                         SOC_PPC_FP_QUAL_IN_LIF_PROFILE                        
#define SOC_PPD_FP_QUAL_IN_LIF_PROFILE_SAME_IF                 SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF
#define SOC_PPD_FP_QUAL_LEARN_OR_TRANSPLANT                    SOC_PPC_FP_QUAL_LEARN_OR_TRANSPLANT                   
#define SOC_PPD_FP_QUAL_PACKET_IS_BOOTP_DHCP                   SOC_PPC_FP_QUAL_PACKET_IS_BOOTP_DHCP                  
#define SOC_PPD_FP_QUAL_UNKNOWN_ADDR                           SOC_PPC_FP_QUAL_UNKNOWN_ADDR                          
#define SOC_PPD_FP_QUAL_FWD_PRCESSING_PROFILE                  SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE                 
#define SOC_PPD_FP_QUAL_ELK_LOOKUP_FOUND_0                     SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_0                       
#define SOC_PPD_FP_QUAL_ELK_LOOKUP_FOUND_1                     SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_1                       
#define SOC_PPD_FP_QUAL_ELK_LOOKUP_FOUND_2                     SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_2                       
#define SOC_PPD_FP_QUAL_ELK_LOOKUP_FOUND_3                     SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_3                       
#define SOC_PPD_FP_QUAL_ELK_LOOKUP_RESULT_0                    SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_0                       
#define SOC_PPD_FP_QUAL_ELK_LOOKUP_RESULT_1                    SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_1                       
#define SOC_PPD_FP_QUAL_ELK_LOOKUP_RESULT_2                    SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_2                       
#define SOC_PPD_FP_QUAL_ELK_LOOKUP_RESULT_3                    SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_3                       
#define SOC_PPD_FP_QUAL_LEM_1ST_LOOKUP_FOUND                   SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND                  
#define SOC_PPD_FP_QUAL_LEM_1ST_LOOKUP_RESULT                  SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_RESULT                 
#define SOC_PPD_FP_QUAL_LEARN_DATA                             SOC_PPC_FP_QUAL_LEARN_DATA                            
#define SOC_PPD_FP_QUAL_LEM_2ND_LOOKUP_FOUND                   SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_FOUND                  
#define SOC_PPD_FP_QUAL_LEM_2ND_LOOKUP_RESULT                  SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_RESULT                  
#define SOC_PPD_FP_QUAL_LPM_1ST_LOOKUP_FOUND                   SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_FOUND                  
#define SOC_PPD_FP_QUAL_LPM_1ST_LOOKUP_RESULT                  SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_RESULT                 
#define SOC_PPD_FP_QUAL_LPM_2ND_LOOKUP_FOUND                   SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_FOUND                  
#define SOC_PPD_FP_QUAL_LPM_2ND_LOOKUP_RESULT                  SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_RESULT                 
#define SOC_PPD_FP_QUAL_TCAM_MATCH                             SOC_PPC_FP_QUAL_TCAM_MATCH                            
#define SOC_PPD_FP_QUAL_TCAM_RESULT                            SOC_PPC_FP_QUAL_TCAM_RESULT                           
#define SOC_PPD_FP_QUAL_TT_LOOKUP0_FOUND                       SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND                      
#define SOC_PPD_FP_QUAL_TT_LOOKUP0_PAYLOAD                     SOC_PPC_FP_QUAL_TT_LOOKUP0_PAYLOAD                    
#define SOC_PPD_FP_QUAL_TT_LOOKUP1_FOUND                       SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND                      
#define SOC_PPD_FP_QUAL_TT_LOOKUP1_PAYLOAD                     SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD                    
#define SOC_PPD_FP_QUAL_VT_LOOKUP0_FOUND                       SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND                      
#define SOC_PPD_FP_QUAL_VT_LOOKUP0_PAYLOAD                     SOC_PPC_FP_QUAL_VT_LOOKUP0_PAYLOAD                    
#define SOC_PPD_FP_QUAL_VT_LOOKUP1_FOUND                       SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND                      
#define SOC_PPD_FP_QUAL_VT_LOOKUP1_PAYLOAD                     SOC_PPC_FP_QUAL_VT_LOOKUP1_PAYLOAD                    
#define SOC_PPD_FP_QUAL_DA_IS_BPDU                             SOC_PPC_FP_QUAL_DA_IS_BPDU                            
#define SOC_PPD_FP_QUAL_TUNNEL_ID                              SOC_PPC_FP_QUAL_TUNNEL_ID
#define SOC_PPD_FP_QUAL_ARP_SENDER_IP4                         SOC_PPC_FP_QUAL_ARP_SENDER_IP4
#define SOC_PPD_FP_QUAL_ARP_TARGET_IP4                         SOC_PPC_FP_QUAL_ARP_TARGET_IP4
#define SOC_PPD_FP_QUAL_ARP_OPCODE_IP4                         SOC_PPC_FP_QUAL_ARP_OPCODE_IP4
#define SOC_PPD_FP_QUAL_KEY_AFTER_HASHING                      SOC_PPC_FP_QUAL_KEY_AFTER_HASHING                                   
#define SOC_PPD_FP_QUAL_IS_FEC_DEST_14_0                       SOC_PPC_FP_QUAL_IS_FEC_DEST_14_0                            
#define SOC_PPD_FP_QUAL_IS_EQUAL                               SOC_PPC_FP_QUAL_IS_EQUAL
#define SOC_PPD_FP_QUAL_EXTENSION_HEADER_TYPE                  SOC_PPC_FP_QUAL_EXTENSION_HEADER_TYPE
#define SOC_PPD_FP_QUAL_ERPP_FHEI                              SOC_PPC_FP_QUAL_ERPP_FHEI
#define SOC_PPD_FP_QUAL_ERPP_FHEI_EXP                          SOC_PPC_FP_QUAL_ERPP_FHEI_EXP
#define SOC_PPD_FP_QUAL_ERPP_FHEI_DSCP                         SOC_PPC_FP_QUAL_ERPP_FHEI_DSCP
#define SOC_PPD_FP_QUAL_ERPP_FHEI_IPV4_TTL                     SOC_PPC_FP_QUAL_ERPP_FHEI_IPV4_TTL
#define SOC_PPD_FP_QUAL_ERPP_FHEI_CODE                         SOC_PPC_FP_QUAL_ERPP_FHEI_CODE
#define SOC_PPD_FP_QUAL_ERPP_DSP_EXT                           SOC_PPC_FP_QUAL_ERPP_DSP_EXT
#define SOC_PPD_FP_QUAL_ERPP_STACKING_ROUTE_HISTORY_BITMAP     SOC_PPC_FP_QUAL_ERPP_STACKING_ROUTE_HISTORY_BITMAP
#define SOC_PPD_FP_QUAL_ERPP_OUT_TM_PORT_PMF_DATA                       SOC_PPC_FP_QUAL_ERPP_OUT_TM_PORT_PMF_DATA
#define SOC_PPD_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA                       SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA
#define SOC_PPD_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA_PS                    SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA_PS
#define SOC_PPD_FP_QUAL_ERPP_EXT_IN_LIF                                 SOC_PPC_FP_QUAL_ERPP_EXT_IN_LIF
#define SOC_PPD_FP_QUAL_ERPP_EXT_OUT_LIF                                SOC_PPC_FP_QUAL_ERPP_EXT_OUT_LIF
#define SOC_PPD_FP_QUAL_ERPP_PACKET_SIZE                                SOC_PPC_FP_QUAL_ERPP_PACKET_SIZE
#define SOC_PPD_FP_QUAL_ERPP_DST_SYSTEM_PORT                            SOC_PPC_FP_QUAL_ERPP_DST_SYSTEM_PORT
#define SOC_PPD_FP_QUAL_ERPP_SRC_SYSTEM_PORT                            SOC_PPC_FP_QUAL_ERPP_SRC_SYSTEM_PORT
#define SOC_PPD_FP_QUAL_ERPP_VSI_OR_VRF                                 SOC_PPC_FP_QUAL_ERPP_VSI_OR_VRF
#define SOC_PPD_FP_QUAL_ERPP_FWD_OFFSET                                 SOC_PPC_FP_QUAL_ERPP_FWD_OFFSET
#define SOC_PPD_FP_QUAL_ERPP_SYS_VALUE1                                 SOC_PPC_FP_QUAL_ERPP_SYS_VALUE1
#define SOC_PPD_FP_QUAL_ERPP_SYS_VALUE2                                 SOC_PPC_FP_QUAL_ERPP_SYS_VALUE2
#define SOC_PPD_FP_QUAL_ERPP_OUT_PP_PORT                                SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT
#define SOC_PPD_FP_QUAL_ERPP_LB_KEY                                     SOC_PPC_FP_QUAL_ERPP_LB_KEY
#define SOC_PPD_FP_QUAL_ERPP_TC                                         SOC_PPC_FP_QUAL_ERPP_TC
#define SOC_PPD_FP_QUAL_ERPP_ACTION_PROFILE                             SOC_PPC_FP_QUAL_ERPP_ACTION_PROFILE
#define SOC_PPD_FP_QUAL_ERPP_HEADER_CODE                                SOC_PPC_FP_QUAL_ERPP_HEADER_CODE
#define SOC_PPD_FP_QUAL_ERPP_IN_LIF_ORIENTATION                         SOC_PPC_FP_QUAL_ERPP_IN_LIF_ORIENTATION
#define SOC_PPD_FP_QUAL_ERPP_TM_ACTION_TYPE                             SOC_PPC_FP_QUAL_ERPP_TM_ACTION_TYPE
#define SOC_PPD_FP_QUAL_ERPP_DP                                         SOC_PPC_FP_QUAL_ERPP_DP
#define SOC_PPD_FP_QUAL_ERPP_LEARN_ALLOWED                              SOC_PPC_FP_QUAL_ERPP_LEARN_ALLOWED
#define SOC_PPD_FP_QUAL_ERPP_LEARN_EXT_SRC_PORT                SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_SRC_PORT
#define SOC_PPD_FP_QUAL_ERPP_LEARN_EXT_IN_VPORT                SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_IN_VPORT
#define SOC_PPD_FP_QUAL_ERPP_UNKNOWN_ADDR                               SOC_PPC_FP_QUAL_ERPP_UNKNOWN_ADDR
#define SOC_PPD_FP_QUAL_ERPP_BYPASS_FILTERING                           SOC_PPC_FP_QUAL_ERPP_BYPASS_FILTERING
#define SOC_PPD_FP_QUAL_ERPP_CNI                                        SOC_PPC_FP_QUAL_ERPP_CNI
#define SOC_PPD_FP_QUAL_ERPP_SYSTEM_MC                                  SOC_PPC_FP_QUAL_ERPP_SYSTEM_MC
#define SOC_PPD_FP_QUAL_ERPP_OUT_MIRROR_DISABLE                         SOC_PPC_FP_QUAL_ERPP_OUT_MIRROR_DISABLE
#define SOC_PPD_FP_QUAL_ERPP_EXCLUDE_SRC                                SOC_PPC_FP_QUAL_ERPP_EXCLUDE_SRC
#define SOC_PPD_FP_QUAL_ERPP_FIRST_COPY                                 SOC_PPC_FP_QUAL_ERPP_FIRST_COPY
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_ETH_DA                            SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_DA
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_ETH_SA                            SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_SA
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID0                         SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID0
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TPID                    SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TPID
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TPID                    SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TPID
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG                     SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_ID                  SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_ID
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_PRI                 SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_PRI
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_CFI                 SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_CFI
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_PRI_CFI             SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_PRI_CFI
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG                     SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_ID                  SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_ID
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_PRI                 SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_PRI
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_CFI                 SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_CFI
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG                     SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_ID                  SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_ID
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_PRI                 SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_PRI
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_CFI                 SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_CFI
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG                     SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_ID                  SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_ID
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_PRI                 SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_PRI
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_CFI                 SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_CFI
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_IPV4_CHECKSUM_ERROR               SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_CHECKSUM_ERROR
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_IPV4_VERSION_ERROR                SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_VERSION_ERROR
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_IPV4_SIP                          SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_SIP
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_IPV4_DIP                          SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_DIP
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_IPV4_PROTOCOL                     SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_PROTOCOL
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_IPV4_L4_SRC_PORT                  SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_L4_SRC_PORT
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_IPV4_L4_DEST_PORT                 SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_L4_DEST_PORT
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_FLOW_LABEL              SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_FLOW_LABEL
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_NEXT_PROTOCOL           SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_NEXT_PROTOCOL
#define SOC_PPD_FP_QUAL_ERPP_NWK_RCRD_IPV6_VERSION_ERROR                SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_VERSION_ERROR
#define SOC_PPD_FP_QUAL_IRPP_ELK_LKP_PAYLOAD_LSB                        SOC_PPC_FP_QUAL_IRPP_ELK_LKP_PAYLOAD_LSB                        
#define SOC_PPD_FP_QUAL_IRPP_IN_RIF_PROFILE                             SOC_PPC_FP_QUAL_IRPP_IN_RIF_PROFILE                             
#define SOC_PPD_FP_QUAL_IRPP_TCAM0_MATCH                                SOC_PPC_FP_QUAL_IRPP_TCAM0_MATCH                                
#define SOC_PPD_FP_QUAL_IRPP_TCAM0_RESULT                               SOC_PPC_FP_QUAL_IRPP_TCAM0_RESULT                               
#define SOC_PPD_FP_QUAL_IRPP_TCAM1_MATCH                                SOC_PPC_FP_QUAL_IRPP_TCAM1_MATCH                                
#define SOC_PPD_FP_QUAL_IRPP_TCAM1_RESULT                               SOC_PPC_FP_QUAL_IRPP_TCAM1_RESULT                               
#define SOC_PPD_FP_QUAL_IRPP_IN_LIF_DATA                                SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA                                
#define SOC_PPD_FP_QUAL_IRPP_RPF_STAMP_NATIVE_VSI                       SOC_PPC_FP_QUAL_IRPP_RPF_STAMP_NATIVE_VSI                       
#define SOC_PPD_FP_QUAL_IRPP_RPF_NATIVE_VSI                             SOC_PPC_FP_QUAL_IRPP_RPF_NATIVE_VSI                             
#define SOC_PPD_FP_QUAL_IRPP_IN_PORT_MAPPED_PP_PORT                     SOC_PPC_FP_QUAL_IRPP_IN_PORT_MAPPED_PP_PORT                     
#define SOC_PPD_FP_QUAL_IRPP_IN_LIF_DATA_INDEX                          SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA_INDEX                          
#define SOC_PPD_FP_QUAL_IRPP_LOCAL_IN_LIF                               SOC_PPC_FP_QUAL_IRPP_LOCAL_IN_LIF                               
#define SOC_PPD_FP_QUAL_IRPP_CONSISTENT_HASHING_PGM_KEY_GEN_VAR         SOC_PPC_FP_QUAL_IRPP_CONSISTENT_HASHING_PGM_KEY_GEN_VAR    
#define SOC_PPD_FP_QUAL_IRPP_PACKET_FORMAT_CODE_ACL                     SOC_PPC_FP_QUAL_IRPP_PACKET_FORMAT_CODE_ACL    
#define SOC_PPD_FP_QUAL_OUTER_VLAN_ACTION_RANGE                         SOC_PPC_FP_QUAL_OUTER_VLAN_ACTION_RANGE
#define SOC_PPD_FP_QUAL_INNER_VLAN_ACTION_RANGE                         SOC_PPC_FP_QUAL_INNER_VLAN_ACTION_RANGE
#define SOC_PPD_FP_QUAL_CLASS_ID                                        SOC_PPC_FP_QUAL_CLASS_ID
                                                                                                         
#define SOC_PPD_FP_QUAL_VTT_OAM_LIF_VALID                      SOC_PPC_FP_QUAL_VTT_OAM_LIF_VALID            
#define SOC_PPD_FP_QUAL_VTT_OAM_LIF                            SOC_PPC_FP_QUAL_VTT_OAM_LIF                  
#define SOC_PPD_FP_QUAL_OAM_UP_MEP                             SOC_PPC_FP_QUAL_OAM_UP_MEP                   
#define SOC_PPD_FP_QUAL_OAM_SUB_TYPE                           SOC_PPC_FP_QUAL_OAM_SUB_TYPE                 
#define SOC_PPD_FP_QUAL_OAM_OFFSET                             SOC_PPC_FP_QUAL_OAM_OFFSET                   
#define SOC_PPD_FP_QUAL_OAM_STAMP_OFFSET                       SOC_PPC_FP_QUAL_OAM_STAMP_OFFSET             
#define SOC_PPD_FP_QUAL_OAM_ID                                 SOC_PPC_FP_QUAL_OAM_ID                       
#define SOC_PPD_FP_QUAL_OAM_METER_DISABLE                      SOC_PPC_FP_QUAL_OAM_METER_DISABLE            
#define SOC_PPD_FP_QUAL_ERPP_OAM_TS                            SOC_PPC_FP_QUAL_ERPP_OAM_TS  
 
#define SOC_PPD_FP_QUAL_ETH_OAM_HEADER_BITS_0_31               SOC_PPC_FP_QUAL_ETH_OAM_HEADER_BITS_0_31
#define SOC_PPD_FP_QUAL_ETH_OAM_HEADER_BITS_32_63              SOC_PPC_FP_QUAL_ETH_OAM_HEADER_BITS_32_63
#define SOC_PPD_FP_QUAL_MPLS_OAM_HEADER_BITS_0_31              SOC_PPC_FP_QUAL_MPLS_OAM_HEADER_BITS_0_31
#define SOC_PPD_FP_QUAL_MPLS_OAM_HEADER_BITS_32_63             SOC_PPC_FP_QUAL_MPLS_OAM_HEADER_BITS_32_63
#define SOC_PPD_FP_QUAL_MPLS_OAM_ACH                           SOC_PPC_FP_QUAL_MPLS_OAM_ACH
#define SOC_PPD_FP_QUAL_OAM_HEADER_BITS_0_31                   SOC_PPC_FP_QUAL_OAM_HEADER_BITS_0_31
#define SOC_PPD_FP_QUAL_OAM_HEADER_BITS_32_63                  SOC_PPC_FP_QUAL_OAM_HEADER_BITS_32_63

#define SOC_PPD_FP_QUAL_IRPP_PTC_KEY_GEN_VAR_PS                SOC_PPC_FP_QUAL_IRPP_PTC_KEY_GEN_VAR_PS


#define SOC_PPD_NOF_FP_QUAL_TYPES                                       SOC_PPC_NOF_FP_QUAL_TYPES
#define SOC_PPD_FP_QUAL_VLAN_DOMAIN                             SOC_PPC_FP_QUAL_VLAN_DOMAIN
#define SOC_PPD_FP_QUAL_INITIAL_VID                             SOC_PPC_FP_QUAL_INITIAL_VID
#define SOC_PPD_FP_QUAL_MPLS_KEY3                               SOC_PPC_FP_QUAL_MPLS_KEY3
#define SOC_PPD_FP_QUAL_MPLS_KEY2                               SOC_PPC_FP_QUAL_MPLS_KEY2
#define SOC_PPD_FP_QUAL_MPLS_KEY1                               SOC_PPC_FP_QUAL_MPLS_KEY1
#define SOC_PPD_FP_QUAL_MPLS_KEY0                               SOC_PPC_FP_QUAL_MPLS_KEY0
#define SOC_PPD_FP_QUAL_CMPRSD_INNER_VID                        SOC_PPC_FP_QUAL_CMPRSD_INNER_VID
#define SOC_PPD_FP_QUAL_CMPRSD_OUTER_VID                        SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID
#define SOC_PPD_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF2              SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF2
#define SOC_PPD_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF1              SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF1
#define SOC_PPD_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF0              SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF0
#define SOC_PPD_FP_QUAL_LABEL3_IDX                              SOC_PPC_FP_QUAL_LABEL3_IDX
#define SOC_PPD_FP_QUAL_LABEL2_IDX                              SOC_PPC_FP_QUAL_LABEL2_IDX
#define SOC_PPD_FP_QUAL_LABEL1_IDX                              SOC_PPC_FP_QUAL_LABEL1_IDX
#define SOC_PPD_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_DOMAIN         SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_DOMAIN
#define SOC_PPD_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_SA_DROP        SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_SA_DROP
#define SOC_PPD_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_IS_LEARN_LIF   SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_IS_LEARN_LIF
#define SOC_PPD_FP_QUAL_LEM_DYNAMIC_LEM_1ST_LOOKUP_FOUND_LEM    SOC_PPC_FP_QUAL_LEM_DYNAMIC_LEM_1ST_LOOKUP_FOUND_LEM
#define SOC_PPD_FP_QUAL_LEM_1ST_LOOKUP_FOUND_DESTINATION        SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_DESTINATION
#define SOC_PPD_FP_QUAL_ISB_FOUND_IN_LIF_IDX                    SOC_PPC_FP_QUAL_ISB_FOUND_IN_LIF_IDX
#define SOC_PPD_FP_QUAL_ISB_FOUND                               SOC_PPC_FP_QUAL_ISB_FOUND
#define SOC_PPD_FP_QUAL_ISB_IN_LIF_IDX                          SOC_PPC_FP_QUAL_ISB_IN_LIF_IDX
#define SOC_PPD_FP_QUAL_ISA_FOUND_IN_LIF_IDX                    SOC_PPC_FP_QUAL_ISA_FOUND_IN_LIF_IDX
#define SOC_PPD_FP_QUAL_ISA_FOUND                               SOC_PPC_FP_QUAL_ISA_FOUND
#define SOC_PPD_FP_QUAL_ISA_IN_LIF_IDX                          SOC_PPC_FP_QUAL_ISA_IN_LIF_IDX
#define SOC_PPD_FP_QUAL_VT_TCAM_MATCH_IN_LIF_IDX                SOC_PPC_FP_QUAL_VT_TCAM_MATCH_IN_LIF_IDX
#define SOC_PPD_FP_QUAL_VT_TCAM_MATCH                           SOC_PPC_FP_QUAL_VT_TCAM_MATCH
#define SOC_PPD_FP_QUAL_VT_TCAM_IN_LIF_IDX                      SOC_PPC_FP_QUAL_VT_TCAM_IN_LIF_IDX
#define SOC_PPD_FP_QUAL_INITIAL_VSI                             SOC_PPC_FP_QUAL_INITIAL_VSI
#define SOC_PPD_FP_QUAL_IN_RIF_VALID_VRF                        SOC_PPC_FP_QUAL_IN_RIF_VALID_VRF
#define SOC_PPD_FP_QUAL_IN_RIF_VALID_RIF_PROFILE                SOC_PPC_FP_QUAL_IN_RIF_VALID_RIF_PROFILE
#define SOC_PPD_FP_QUAL_IN_RIF_VALID_IN_RIF                     SOC_PPC_FP_QUAL_IN_RIF_VALID_IN_RIF
#define SOC_PPD_FP_QUAL_IN_RIF_VALID                            SOC_PPC_FP_QUAL_IN_RIF_VALID
#define SOC_PPD_FP_QUAL_VT_LEM_1ST_LOOKUP_FOUND                 SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_FOUND
#define SOC_PPD_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND             SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND
#define SOC_PPD_FP_QUAL_MACT_DOMAIN                             SOC_PPC_FP_QUAL_MACT_DOMAIN
#define SOC_PPD_FP_QUAL_MACT_SA_DROP                            SOC_PPC_FP_QUAL_MACT_SA_DROP
#define SOC_PPD_FP_QUAL_MACT_IS_LEARN                           SOC_PPC_FP_QUAL_MACT_IS_LEARN
#define SOC_PPD_FP_QUAL_MACT_DYNAMIC                            SOC_PPC_FP_QUAL_MACT_DYNAMIC
#define SOC_PPD_FP_QUAL_MACT_DESTINATION                        SOC_PPC_FP_QUAL_MACT_DESTINATION
#define SOC_PPD_FP_QUAL_KEY3                                    SOC_PPC_FP_QUAL_KEY3
#define SOC_PPD_FP_QUAL_KEY3_16_INST0                           SOC_PPC_FP_QUAL_KEY3_16_INST0
#define SOC_PPD_FP_QUAL_KEY3_16_INST1                           SOC_PPC_FP_QUAL_KEY3_16_INST1
#define SOC_PPD_FP_QUAL_KEY3_16_INST2                           SOC_PPC_FP_QUAL_KEY3_16_INST2
#define SOC_PPD_FP_QUAL_KEY3_32_INST0                           SOC_PPC_FP_QUAL_KEY3_32_INST0
#define SOC_PPD_FP_QUAL_KEY3_32_INST1                           SOC_PPC_FP_QUAL_KEY3_32_INST1
#define SOC_PPD_FP_QUAL_KEY3_LABEL                              SOC_PPC_FP_QUAL_KEY3_LABEL
#define SOC_PPD_FP_QUAL_KEY3_OUTER_VID_VALID                    SOC_PPC_FP_QUAL_KEY3_OUTER_VID_VALID
#define SOC_PPD_FP_QUAL_KEY3_OUTER_VID                          SOC_PPC_FP_QUAL_KEY3_OUTER_VID
#define SOC_PPD_FP_QUAL_KEY3_INNER_VID_VALID                    SOC_PPC_FP_QUAL_KEY3_INNER_VID_VALID
#define SOC_PPD_FP_QUAL_KEY3_INNER_VID                          SOC_PPC_FP_QUAL_KEY3_INNER_VID
#define SOC_PPD_FP_QUAL_KEY2                                    SOC_PPC_FP_QUAL_KEY2
#define SOC_PPD_FP_QUAL_KEY2_16_INST0                           SOC_PPC_FP_QUAL_KEY2_16_INST0
#define SOC_PPD_FP_QUAL_KEY2_16_INST1                           SOC_PPC_FP_QUAL_KEY2_16_INST1
#define SOC_PPD_FP_QUAL_KEY2_16_INST2                           SOC_PPC_FP_QUAL_KEY2_16_INST2
#define SOC_PPD_FP_QUAL_KEY2_32_INST0                           SOC_PPC_FP_QUAL_KEY2_32_INST0
#define SOC_PPD_FP_QUAL_KEY2_32_INST1                           SOC_PPC_FP_QUAL_KEY2_32_INST1
#define SOC_PPD_FP_QUAL_KEY2_LABEL                              SOC_PPC_FP_QUAL_KEY2_LABEL
#define SOC_PPD_FP_QUAL_KEY2_OUTER_VID_VALID                    SOC_PPC_FP_QUAL_KEY2_OUTER_VID_VALID
#define SOC_PPD_FP_QUAL_KEY2_OUTER_VID                          SOC_PPC_FP_QUAL_KEY2_OUTER_VID
#define SOC_PPD_FP_QUAL_KEY2_INNER_VID_VALID                    SOC_PPC_FP_QUAL_KEY2_INNER_VID_VALID
#define SOC_PPD_FP_QUAL_KEY2_INNER_VID                          SOC_PPC_FP_QUAL_KEY2_INNER_VID
#define SOC_PPD_FP_QUAL_KEY1                                    SOC_PPC_FP_QUAL_KEY1
#define SOC_PPD_FP_QUAL_KEY1_16_INST0                           SOC_PPC_FP_QUAL_KEY1_16_INST0
#define SOC_PPD_FP_QUAL_KEY1_16_INST1                           SOC_PPC_FP_QUAL_KEY1_16_INST1
#define SOC_PPD_FP_QUAL_KEY1_16_INST2                           SOC_PPC_FP_QUAL_KEY1_16_INST2
#define SOC_PPD_FP_QUAL_KEY1_32_INST0                           SOC_PPC_FP_QUAL_KEY1_32_INST0
#define SOC_PPD_FP_QUAL_KEY1_32_INST1                           SOC_PPC_FP_QUAL_KEY1_32_INST1
#define SOC_PPD_FP_QUAL_KEY1_LABEL                              SOC_PPC_FP_QUAL_KEY1_LABEL
#define SOC_PPD_FP_QUAL_KEY1_OUTER_VID_VALID                    SOC_PPC_FP_QUAL_KEY1_OUTER_VID_VALID
#define SOC_PPD_FP_QUAL_KEY1_OUTER_VID                          SOC_PPC_FP_QUAL_KEY1_OUTER_VID
#define SOC_PPD_FP_QUAL_KEY1_INNER_VID_VALID                    SOC_PPC_FP_QUAL_KEY1_INNER_VID_VALID
#define SOC_PPD_FP_QUAL_KEY1_INNER_VID                          SOC_PPC_FP_QUAL_KEY1_INNER_VID
#define SOC_PPD_FP_QUAL_KEY0                                    SOC_PPC_FP_QUAL_KEY0
#define SOC_PPD_FP_QUAL_KEY0_16_INST0                           SOC_PPC_FP_QUAL_KEY0_16_INST0
#define SOC_PPD_FP_QUAL_KEY0_16_INST1                           SOC_PPC_FP_QUAL_KEY0_16_INST1
#define SOC_PPD_FP_QUAL_KEY0_16_INST2                           SOC_PPC_FP_QUAL_KEY0_16_INST2
#define SOC_PPD_FP_QUAL_KEY0_32_INST0                           SOC_PPC_FP_QUAL_KEY0_32_INST0
#define SOC_PPD_FP_QUAL_KEY0_32_INST1                           SOC_PPC_FP_QUAL_KEY0_32_INST1
#define SOC_PPD_FP_QUAL_KEY0_LABEL                              SOC_PPC_FP_QUAL_KEY0_LABEL
#define SOC_PPD_FP_QUAL_KEY0_OUTER_VID_VALID                    SOC_PPC_FP_QUAL_KEY0_OUTER_VID_VALID
#define SOC_PPD_FP_QUAL_KEY0_OUTER_VID                          SOC_PPC_FP_QUAL_KEY0_OUTER_VID
#define SOC_PPD_FP_QUAL_KEY0_INNER_VID_VALID                    SOC_PPC_FP_QUAL_KEY0_INNER_VID_VALID
#define SOC_PPD_FP_QUAL_KEY0_INNER_VID                          SOC_PPC_FP_QUAL_KEY0_INNER_VID

typedef SOC_PPC_FP_QUAL_TYPE                                   SOC_PPD_FP_QUAL_TYPE;

#define SOC_PPD_FP_PKT_HDR_TYPE_RAW                            SOC_PPC_FP_PKT_HDR_TYPE_RAW
#define SOC_PPD_FP_PKT_HDR_TYPE_FTMH                           SOC_PPC_FP_PKT_HDR_TYPE_FTMH
#define SOC_PPD_FP_PKT_HDR_TYPE_TM                             SOC_PPC_FP_PKT_HDR_TYPE_TM
#define SOC_PPD_FP_PKT_HDR_TYPE_TM_IS                          SOC_PPC_FP_PKT_HDR_TYPE_TM_IS
#define SOC_PPD_FP_PKT_HDR_TYPE_TM_PPH                         SOC_PPC_FP_PKT_HDR_TYPE_TM_PPH
#define SOC_PPD_FP_PKT_HDR_TYPE_TM_IS_PPH                      SOC_PPC_FP_PKT_HDR_TYPE_TM_IS_PPH
#define SOC_PPD_FP_PKT_HDR_TYPE_ETH                            SOC_PPC_FP_PKT_HDR_TYPE_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_ETH_ETH                        SOC_PPC_FP_PKT_HDR_TYPE_ETH_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_IPV4_ETH                       SOC_PPC_FP_PKT_HDR_TYPE_IPV4_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_IPV6_ETH                       SOC_PPC_FP_PKT_HDR_TYPE_IPV6_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_MPLS1_ETH                      SOC_PPC_FP_PKT_HDR_TYPE_MPLS1_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_MPLS2_ETH                      SOC_PPC_FP_PKT_HDR_TYPE_MPLS2_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_MPLS3_ETH                      SOC_PPC_FP_PKT_HDR_TYPE_MPLS3_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_ETH_MPLS1_ETH                  SOC_PPC_FP_PKT_HDR_TYPE_ETH_MPLS1_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_ETH_MPLS2_ETH                  SOC_PPC_FP_PKT_HDR_TYPE_ETH_MPLS2_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_ETH_MPLS3_ETH                  SOC_PPC_FP_PKT_HDR_TYPE_ETH_MPLS3_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_IPV4_MPLS1_ETH                 SOC_PPC_FP_PKT_HDR_TYPE_IPV4_MPLS1_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_IPV4_MPLS2_ETH                 SOC_PPC_FP_PKT_HDR_TYPE_IPV4_MPLS2_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_IPV4_MPLS3_ETH                 SOC_PPC_FP_PKT_HDR_TYPE_IPV4_MPLS3_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_IPV6_MPLS1_ETH                 SOC_PPC_FP_PKT_HDR_TYPE_IPV6_MPLS1_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_IPV6_MPLS2_ETH                 SOC_PPC_FP_PKT_HDR_TYPE_IPV6_MPLS2_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_IPV6_MPLS3_ETH                 SOC_PPC_FP_PKT_HDR_TYPE_IPV6_MPLS3_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_IPV4_IPV4_ETH                  SOC_PPC_FP_PKT_HDR_TYPE_IPV4_IPV4_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_IPV6_IPV4_ETH                  SOC_PPC_FP_PKT_HDR_TYPE_IPV6_IPV4_ETH
#define SOC_PPD_FP_PKT_HDR_TYPE_ETH_TRILL_ETH                  SOC_PPC_FP_PKT_HDR_TYPE_ETH_TRILL_ETH
typedef SOC_PPC_FP_PKT_HDR_TYPE                                SOC_PPD_FP_PKT_HDR_TYPE;

#define SOC_PPD_FP_FWD_TYPE_BRIDGED                            SOC_PPC_FP_FWD_TYPE_BRIDGED
#define SOC_PPD_FP_FWD_TYPE_IPV4_UC                            SOC_PPC_FP_FWD_TYPE_IPV4_UC
#define SOC_PPD_FP_FWD_TYPE_IPV4_MC                            SOC_PPC_FP_FWD_TYPE_IPV4_MC
#define SOC_PPD_FP_FWD_TYPE_IPV6_UC                            SOC_PPC_FP_FWD_TYPE_IPV6_UC
#define SOC_PPD_FP_FWD_TYPE_IPV6_MC                            SOC_PPC_FP_FWD_TYPE_IPV6_MC
#define SOC_PPD_FP_FWD_TYPE_MPLS                               SOC_PPC_FP_FWD_TYPE_MPLS
#define SOC_PPD_FP_FWD_TYPE_TRILL                              SOC_PPC_FP_FWD_TYPE_TRILL
#define SOC_PPD_FP_FWD_TYPE_BRIDGED_AFTER_TERM                 SOC_PPC_FP_FWD_TYPE_BRIDGED_AFTER_TERM
#define SOC_PPD_FP_FWD_TYPE_CPU_TRAP                           SOC_PPC_FP_FWD_TYPE_CPU_TRAP
#define SOC_PPD_FP_FWD_TYPE_TM                                 SOC_PPC_FP_FWD_TYPE_TM
typedef SOC_PPC_FP_FWD_TYPE                                    SOC_PPD_FP_FWD_TYPE;

#define SOC_PPD_FP_PARSED_ETHERTYPE_NO_MATCH                   SOC_PPC_FP_PARSED_ETHERTYPE_NO_MATCH            
#define SOC_PPD_FP_PARSED_ETHERTYPE_USER_DEFINED_FIRST         SOC_PPC_FP_PARSED_ETHERTYPE_USER_DEFINED_FIRST  
#define SOC_PPD_FP_PARSED_ETHERTYPE_TRILL                      SOC_PPC_FP_PARSED_ETHERTYPE_TRILL               
#define SOC_PPD_FP_PARSED_ETHERTYPE_MAC_IN_MAC                 SOC_PPC_FP_PARSED_ETHERTYPE_MAC_IN_MAC          
#define SOC_PPD_FP_PARSED_ETHERTYPE_ARP                        SOC_PPC_FP_PARSED_ETHERTYPE_ARP                 
#define SOC_PPD_FP_PARSED_ETHERTYPE_CFM                        SOC_PPC_FP_PARSED_ETHERTYPE_CFM                 
#define SOC_PPD_FP_PARSED_ETHERTYPE_FC_E                       SOC_PPC_FP_PARSED_ETHERTYPE_FC_E                
#define SOC_PPD_FP_PARSED_ETHERTYPE_IPV4                       SOC_PPC_FP_PARSED_ETHERTYPE_IPV4                
#define SOC_PPD_FP_PARSED_ETHERTYPE_IPV6                       SOC_PPC_FP_PARSED_ETHERTYPE_IPV6                
#define SOC_PPD_FP_PARSED_ETHERTYPE_MPLS                       SOC_PPC_FP_PARSED_ETHERTYPE_MPLS                
typedef SOC_PPC_FP_PARSED_ETHERTYPE                            SOC_PPD_FP_PARSED_ETHERTYPE;

#define SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_NO_MATCH                   SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_NO_MATCH            
#define SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_TRILL                      SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_TRILL               
#define SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_MAC_IN_MAC                 SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_MAC_IN_MAC          
#define SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_ARP                        SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_ARP                 
#define SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_CFM                        SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_CFM                 
#define SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_IPV4                       SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_IPV4                
#define SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_IPV6                       SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_IPV6                
#define SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS_MPLS                       SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_MPLS                
typedef SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS                            SOC_PPD_FP_PARSED_ETHERTYPE_EGRESS;

#define SOC_PPD_FP_ETH_ENCAPSULATION_ETH_II                    SOC_PPC_FP_ETH_ENCAPSULATION_ETH_II               
#define SOC_PPD_FP_ETH_ENCAPSULATION_LLC                       SOC_PPC_FP_ETH_ENCAPSULATION_LLC                
#define SOC_PPD_FP_ETH_ENCAPSULATION_LLC_SNAP                  SOC_PPC_FP_ETH_ENCAPSULATION_LLC_SNAP              
#define SOC_PPD_FP_ETH_ENCAPSULATION_UNDEF                     SOC_PPC_FP_ETH_ENCAPSULATION_UNDEF              
typedef SOC_PPC_FP_ETH_ENCAPSULATION                           SOC_PPD_FP_ETH_ENCAPSULATION;


#define SOC_PPD_PARSED_IP_NEXT_PROTOCOL_NO_MATCH               SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_NO_MATCH             
#define SOC_PPD_PARSED_IP_NEXT_PROTOCOL_TCP                    SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_TCP                  
#define SOC_PPD_PARSED_IP_NEXT_PROTOCOL_UDP                    SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_UDP                  
#define SOC_PPD_PARSED_IP_NEXT_PROTOCOL_IGMP                   SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_IGMP                 
#define SOC_PPD_PARSED_IP_NEXT_PROTOCOL_ICMP                   SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_ICMP                 
#define SOC_PPD_PARSED_IP_NEXT_PROTOCOL_ICMP_V6                SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_ICMP_V6              
#define SOC_PPD_PARSED_IP_NEXT_PROTOCOL_IPV4                   SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_IPV4                 
#define SOC_PPD_PARSED_IP_NEXT_PROTOCOL_IPV6                   SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_IPV6                 
#define SOC_PPD_PARSED_IP_NEXT_PROTOCOL_MPLS                   SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_MPLS                 
typedef SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL                     SOC_PPD_FP_PARSED_IP_NEXT_PROTOCOL;


#define SOC_PPD_FP_PROCESSING_TYPE_RAW                         SOC_PPC_FP_PROCESSING_TYPE_RAW
#define SOC_PPD_FP_PROCESSING_TYPE_ETH                         SOC_PPC_FP_PROCESSING_TYPE_ETH
#define SOC_PPD_FP_PROCESSING_TYPE_TM                          SOC_PPC_FP_PROCESSING_TYPE_TM
#define SOC_PPD_FP_PROCESSING_TYPE_FTMH                        SOC_PPC_FP_PROCESSING_TYPE_FTMH
typedef SOC_PPC_FP_PROCESSING_TYPE                             SOC_PPD_FP_PROCESSING_TYPE;

#define SOC_PPD_FP_BASE_HEADER_TYPE_HEADER_0                   SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_0
#define SOC_PPD_FP_BASE_HEADER_TYPE_HEADER_1                   SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_1
#define SOC_PPD_FP_BASE_HEADER_TYPE_HEADER_2                   SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_2
#define SOC_PPD_FP_BASE_HEADER_TYPE_HEADER_3                   SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_3
#define SOC_PPD_FP_BASE_HEADER_TYPE_HEADER_4                   SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_4
#define SOC_PPD_FP_BASE_HEADER_TYPE_HEADER_5                   SOC_PPC_FP_BASE_HEADER_TYPE_HEADER_5
#define SOC_PPD_FP_BASE_HEADER_TYPE_FWD                        SOC_PPC_FP_BASE_HEADER_TYPE_FWD
#define SOC_PPD_FP_BASE_HEADER_TYPE_FWD_POST                   SOC_PPC_FP_BASE_HEADER_TYPE_FWD_POST
typedef SOC_PPC_FP_BASE_HEADER_TYPE                            SOC_PPD_FP_BASE_HEADER_TYPE;

#define SOC_PPD_FP_ACTION_TYPE_DEST                            SOC_PPC_FP_ACTION_TYPE_DEST
#define SOC_PPD_FP_ACTION_TYPE_DP                              SOC_PPC_FP_ACTION_TYPE_DP
#define SOC_PPD_FP_ACTION_TYPE_TC                              SOC_PPC_FP_ACTION_TYPE_TC
#define SOC_PPD_FP_ACTION_TYPE_TRAP                            SOC_PPC_FP_ACTION_TYPE_TRAP
#define SOC_PPD_FP_ACTION_TYPE_TRAP_REDUCED                    SOC_PPC_FP_ACTION_TYPE_TRAP_REDUCED
#define SOC_PPD_FP_ACTION_TYPE_SNP                             SOC_PPC_FP_ACTION_TYPE_SNP
#define SOC_PPD_FP_ACTION_TYPE_MIRROR                          SOC_PPC_FP_ACTION_TYPE_MIRROR
#define SOC_PPD_FP_ACTION_TYPE_MIR_DIS                         SOC_PPC_FP_ACTION_TYPE_MIR_DIS
#define SOC_PPD_FP_ACTION_TYPE_EXC_SRC                         SOC_PPC_FP_ACTION_TYPE_EXC_SRC
#define SOC_PPD_FP_ACTION_TYPE_IS                              SOC_PPC_FP_ACTION_TYPE_IS
#define SOC_PPD_FP_ACTION_TYPE_METER                           SOC_PPC_FP_ACTION_TYPE_METER
#define SOC_PPD_FP_ACTION_TYPE_COUNTER                         SOC_PPC_FP_ACTION_TYPE_COUNTER
#define SOC_PPD_FP_ACTION_TYPE_STAT                            SOC_PPC_FP_ACTION_TYPE_STAT
#define SOC_PPD_FP_ACTION_TYPE_OUTLIF                          SOC_PPC_FP_ACTION_TYPE_OUTLIF
#define SOC_PPD_FP_ACTION_TYPE_LAG_LB                          SOC_PPC_FP_ACTION_TYPE_LAG_LB
#define SOC_PPD_FP_ACTION_TYPE_ECMP_LB                         SOC_PPC_FP_ACTION_TYPE_ECMP_LB
#define SOC_PPD_FP_ACTION_TYPE_STACK_RT_HIST                   SOC_PPC_FP_ACTION_TYPE_STACK_RT_HIST
#define SOC_PPD_FP_ACTION_TYPE_VSQ_PTR                         SOC_PPC_FP_ACTION_TYPE_VSQ_PTR
#define SOC_PPD_FP_ACTION_TYPE_CHANGE_KEY                      SOC_PPC_FP_ACTION_TYPE_CHANGE_KEY
#define SOC_PPD_FP_ACTION_TYPE_NOP                             SOC_PPC_FP_ACTION_TYPE_NOP
#define SOC_PPD_FP_ACTION_TYPE_EGR_TRAP                        SOC_PPC_FP_ACTION_TYPE_EGR_TRAP
#define SOC_PPD_FP_ACTION_TYPE_EGR_OFP                         SOC_PPC_FP_ACTION_TYPE_EGR_OFP
#define SOC_PPD_FP_ACTION_TYPE_EGR_TC_DP                       SOC_PPC_FP_ACTION_TYPE_EGR_TC_DP
#define SOC_PPD_FP_ACTION_TYPE_EGR_OUTLIF                      SOC_PPC_FP_ACTION_TYPE_EGR_OUTLIF
/* FOR arad*/
#define SOC_PPD_FP_ACTION_TYPE_EEI                             SOC_PPC_FP_ACTION_TYPE_EEI
#define SOC_PPD_FP_ACTION_TYPE_METER_A                         SOC_PPC_FP_ACTION_TYPE_METER_A
#define SOC_PPD_FP_ACTION_TYPE_METER_B                         SOC_PPC_FP_ACTION_TYPE_METER_B
#define SOC_PPD_FP_ACTION_TYPE_COUNTER_A                       SOC_PPC_FP_ACTION_TYPE_COUNTER_A
#define SOC_PPD_FP_ACTION_TYPE_COUNTER_B                       SOC_PPC_FP_ACTION_TYPE_COUNTER_B
#define SOC_PPD_FP_ACTION_TYPE_DP_METER_COMMAND                SOC_PPC_FP_ACTION_TYPE_DP_METER_COMMAND
#define SOC_PPD_FP_ACTION_TYPE_SRC_SYST_PORT                   SOC_PPC_FP_ACTION_TYPE_SRC_SYST_PORT
#define SOC_PPD_FP_ACTION_TYPE_FWD_CODE                        SOC_PPC_FP_ACTION_TYPE_FWD_CODE
#define SOC_PPD_FP_ACTION_TYPE_FWD_OFFSET                      SOC_PPC_FP_ACTION_TYPE_FWD_OFFSET
#define SOC_PPD_FP_ACTION_TYPE_BYTES_TO_REMOVE                 SOC_PPC_FP_ACTION_TYPE_BYTES_TO_REMOVE
#define SOC_PPD_FP_ACTION_TYPE_VSI                             SOC_PPC_FP_ACTION_TYPE_VSI
#define SOC_PPD_FP_ACTION_TYPE_ORIENTATION_IS_HUB              SOC_PPC_FP_ACTION_TYPE_ORIENTATION_IS_HUB
#define SOC_PPD_FP_ACTION_TYPE_VLAN_EDIT_COMMAND               SOC_PPC_FP_ACTION_TYPE_VLAN_EDIT_COMMAND
#define SOC_PPD_FP_ACTION_TYPE_VLAN_EDIT_VID_1                 SOC_PPC_FP_ACTION_TYPE_VLAN_EDIT_VID_1
#define SOC_PPD_FP_ACTION_TYPE_VLAN_EDIT_VID_2                 SOC_PPC_FP_ACTION_TYPE_VLAN_EDIT_VID_2
#define SOC_PPD_FP_ACTION_TYPE_VLAN_EDIT_PCP_DEI               SOC_PPC_FP_ACTION_TYPE_VLAN_EDIT_PCP_DEI
#define SOC_PPD_FP_ACTION_TYPE_IN_RIF                          SOC_PPC_FP_ACTION_TYPE_IN_RIF
#define SOC_PPD_FP_ACTION_TYPE_VRF                             SOC_PPC_FP_ACTION_TYPE_VRF
#define SOC_PPD_FP_ACTION_TYPE_IN_TTL                          SOC_PPC_FP_ACTION_TYPE_IN_TTL
#define SOC_PPD_FP_ACTION_TYPE_IN_DSCP_EXP                     SOC_PPC_FP_ACTION_TYPE_IN_DSCP_EXP
#define SOC_PPD_FP_ACTION_TYPE_RPF_DESTINATION_VALID           SOC_PPC_FP_ACTION_TYPE_RPF_DESTINATION_VALID
#define SOC_PPD_FP_ACTION_TYPE_RPF_DESTINATION                 SOC_PPC_FP_ACTION_TYPE_RPF_DESTINATION
#define SOC_PPD_FP_ACTION_TYPE_INGRESS_LEARN_ENABLE            SOC_PPC_FP_ACTION_TYPE_INGRESS_LEARN_ENABLE
#define SOC_PPD_FP_ACTION_TYPE_EGRESS_LEARN_ENABLE             SOC_PPC_FP_ACTION_TYPE_EGRESS_LEARN_ENABLE
#define SOC_PPD_FP_ACTION_TYPE_LEARN_FID                       SOC_PPC_FP_ACTION_TYPE_LEARN_FID
#define SOC_PPD_FP_ACTION_TYPE_LEARN_DATA_0_TO_15              SOC_PPC_FP_ACTION_TYPE_LEARN_DATA_0_TO_15
#define SOC_PPD_FP_ACTION_TYPE_LEARN_DATA_16_TO_39             SOC_PPC_FP_ACTION_TYPE_LEARN_DATA_16_TO_39
#define SOC_PPD_FP_ACTION_TYPE_IN_LIF                          SOC_PPC_FP_ACTION_TYPE_IN_LIF
#define SOC_PPD_FP_ACTION_TYPE_IGNORE_CP                       SOC_PPC_FP_ACTION_TYPE_IGNORE_CP
#define SOC_PPD_FP_ACTION_TYPE_PACKET_IS_BOOTP_DHCP            SOC_PPC_FP_ACTION_TYPE_PACKET_IS_BOOTP_DHCP
#define SOC_PPD_FP_ACTION_TYPE_UNKNOWN_ADDR                    SOC_PPC_FP_ACTION_TYPE_UNKNOWN_ADDR
#define SOC_PPD_FP_ACTION_TYPE_FWD_HDR_ENCAPSULATION           SOC_PPC_FP_ACTION_TYPE_FWD_HDR_ENCAPSULATION
#define SOC_PPD_FP_ACTION_TYPE_USER_HEADER_1                   SOC_PPC_FP_ACTION_TYPE_USER_HEADER_1
#define SOC_PPD_FP_ACTION_TYPE_USER_HEADER_2                   SOC_PPC_FP_ACTION_TYPE_USER_HEADER_2
#define SOC_PPD_FP_ACTION_TYPE_ACE_POINTER                     SOC_PPC_FP_ACTION_TYPE_ACE_POINTER
#define SOC_PPD_FP_ACTION_TYPE_COUNTER_PROFILE                 SOC_PPC_FP_ACTION_TYPE_COUNTER_PROFILE
#define SOC_PPD_FP_ACTION_TYPE_COS_PROFILE                     SOC_PPC_FP_ACTION_TYPE_COS_PROFILE
#define SOC_PPD_FP_ACTION_TYPE_FLP_ACTION_0                    SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_0
#define SOC_PPD_FP_ACTION_TYPE_FLP_ACTION_1                    SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_1
#define SOC_PPD_FP_ACTION_TYPE_FLP_ACTION_2                    SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_2
#define SOC_PPD_FP_ACTION_TYPE_FLP_ACTION_3                    SOC_PPC_FP_ACTION_TYPE_FLP_ACTION_3
#define SOC_PPD_FP_ACTION_TYPE_SLB_HASH_VALUE                  SOC_PPC_FP_ACTION_TYPE_SLB_HASH_VALUE
#define SOC_PPD_FP_ACTION_TYPE_PPH_TYPE                        SOC_PPC_FP_ACTION_TYPE_PPH_TYPE
#define SOC_PPD_FP_ACTION_TYPE_SYSTEM_HEADER_PROFILE_ID        SOC_PPC_FP_ACTION_TYPE_SYSTEM_HEADER_PROFILE_ID
#define SOC_PPD_NOF_FP_ACTION_TYPES                            SOC_PPC_NOF_FP_ACTION_TYPES
#define SOC_PPD_FP_ACTION_TYPE_INVALID                         SOC_PPC_FP_ACTION_TYPE_INVALID

#define SOC_PPD_FP_ACTION_TYPE_OAM                             SOC_PPC_FP_ACTION_TYPE_OAM  

typedef SOC_PPC_FP_ACTION_TYPE                                 SOC_PPD_FP_ACTION_TYPE;

#define SOC_PPD_FP_DB_TYPE_TCAM                                SOC_PPC_FP_DB_TYPE_TCAM
#define SOC_PPD_FP_DB_TYPE_DIRECT_TABLE                        SOC_PPC_FP_DB_TYPE_DIRECT_TABLE
#define SOC_PPD_FP_DB_TYPE_DIRECT_EXTRACTION                   SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION
#define SOC_PPD_FP_DB_TYPE_EGRESS                              SOC_PPC_FP_DB_TYPE_EGRESS
#define SOC_PPD_FP_DB_TYPE_FLP                                 SOC_PPC_FP_DB_TYPE_FLP
#define SOC_PPD_FP_DB_TYPE_SLB                                 SOC_PPC_FP_DB_TYPE_SLB
#define SOC_PPD_NOF_FP_DATABASE_TYPES                          SOC_PPC_NOF_FP_DATABASE_TYPES
typedef SOC_PPC_FP_DATABASE_TYPE                               SOC_PPD_FP_DATABASE_TYPE;

#define SOC_PPD_FP_DATABASE_STAGE_INGRESS_PMF                  SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF
#define SOC_PPD_FP_DATABASE_STAGE_EGRESS                       SOC_PPC_FP_DATABASE_STAGE_EGRESS
#define SOC_PPD_FP_DATABASE_STAGE_INGRESS_FLP                  SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP
#define SOC_PPD_FP_DATABASE_STAGE_INGRESS_SLB                  SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB
typedef SOC_PPC_FP_DATABASE_STAGE                              SOC_PPD_FP_DATABASE_STAGE;

#define SOC_PPD_FP_PREDEFINED_ACL_KEY_L2                       SOC_PPC_FP_PREDEFINED_ACL_KEY_L2
#define SOC_PPD_FP_PREDEFINED_ACL_KEY_IPV4                     SOC_PPC_FP_PREDEFINED_ACL_KEY_IPV4
#define SOC_PPD_FP_PREDEFINED_ACL_KEY_IPV6                     SOC_PPC_FP_PREDEFINED_ACL_KEY_IPV6
#define SOC_PPD_FP_PREDEFINED_ACL_KEY_EGR_ETH                  SOC_PPC_FP_PREDEFINED_ACL_KEY_EGR_ETH
#define SOC_PPD_FP_PREDEFINED_ACL_KEY_EGR_IPV4                 SOC_PPC_FP_PREDEFINED_ACL_KEY_EGR_IPV4
#define SOC_PPD_FP_PREDEFINED_ACL_KEY_EGR_MPLS                 SOC_PPC_FP_PREDEFINED_ACL_KEY_EGR_MPLS
#define SOC_PPD_FP_PREDEFINED_ACL_KEY_EGR_TM                   SOC_PPC_FP_PREDEFINED_ACL_KEY_EGR_TM
#define SOC_PPD_NOF_FP_PREDEFINED_ACL_KEYS                     SOC_PPC_NOF_FP_PREDEFINED_ACL_KEYS
typedef SOC_PPC_FP_PREDEFINED_ACL_KEY                          SOC_PPD_FP_PREDEFINED_ACL_KEY;

#define SOC_PPD_FP_CONTROL_TYPE_L4OPS_RANGE                    SOC_PPC_FP_CONTROL_TYPE_L4OPS_RANGE
#define SOC_PPD_FP_CONTROL_TYPE_PACKET_SIZE_RANGE              SOC_PPC_FP_CONTROL_TYPE_PACKET_SIZE_RANGE
#define SOC_PPD_FP_CONTROL_TYPE_ETHERTYPE                      SOC_PPC_FP_CONTROL_TYPE_ETHERTYPE
#define SOC_PPD_FP_CONTROL_TYPE_NEXT_PROTOCOL_IP               SOC_PPC_FP_CONTROL_TYPE_NEXT_PROTOCOL_IP
#define SOC_PPD_FP_CONTROL_TYPE_EGR_PP_PORT_DATA               SOC_PPC_FP_CONTROL_TYPE_EGR_PP_PORT_DATA
#define SOC_PPD_FP_CONTROL_TYPE_EGR_TM_PORT_DATA               SOC_PPC_FP_CONTROL_TYPE_EGR_TM_PORT_DATA
#define SOC_PPD_FP_CONTROL_TYPE_FLP_PP_PORT_DATA               SOC_PPC_FP_CONTROL_TYPE_FLP_PP_PORT_DATA
#define SOC_PPD_FP_CONTROL_TYPE_ING_PP_PORT_DATA               SOC_PPC_FP_CONTROL_TYPE_ING_PP_PORT_DATA
#define SOC_PPD_FP_CONTROL_TYPE_ING_TM_PORT_DATA               SOC_PPC_FP_CONTROL_TYPE_ING_TM_PORT_DATA
#define SOC_PPD_FP_CONTROL_TYPE_EGR_L2_ETHERTYPES              SOC_PPC_FP_CONTROL_TYPE_EGR_L2_ETHERTYPES
#define SOC_PPD_FP_CONTROL_TYPE_EGR_IPV4_NEXT_PROTOCOL         SOC_PPC_FP_CONTROL_TYPE_EGR_IPV4_NEXT_PROTOCOL
#define SOC_PPD_FP_CONTROL_TYPE_PP_PORT_PROFILE                SOC_PPC_FP_CONTROL_TYPE_PP_PORT_PROFILE
#define SOC_PPD_FP_CONTROL_TYPE_L2_L3_KEY_IN_LIF_ENABLE        SOC_PPC_FP_CONTROL_TYPE_L2_L3_KEY_IN_LIF_ENABLE
#define SOC_PPD_FP_CONTROL_TYPE_L3_IPV6_TCP_CTL_ENABLE         SOC_PPC_FP_CONTROL_TYPE_L3_IPV6_TCP_CTL_ENABLE
#define SOC_PPD_FP_CONTROL_TYPE_HDR_USER_DEF                   SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF
#define SOC_PPD_FP_CONTROL_TYPE_EGRESS_DP                      SOC_PPC_FP_CONTROL_TYPE_EGRESS_DP
#define SOC_PPD_FP_CONTROL_TYPE_INNER_ETH_NOF_VLAN_TAGS        SOC_PPC_FP_CONTROL_TYPE_INNER_ETH_NOF_VLAN_TAGS
#define SOC_PPD_FP_CONTROL_TYPE_KEY_CHANGE_SIZE                SOC_PPC_FP_CONTROL_TYPE_KEY_CHANGE_SIZE
#define SOC_PPD_FP_CONTROL_TYPE_ACE_POINTER_PP_PORT            SOC_PPC_FP_CONTROL_TYPE_ACE_POINTER_PP_PORT
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#define SOC_PPD_FP_CONTROL_TYPE_KBP_CACHE                      SOC_PPC_FP_CONTROL_TYPE_KBP_CACHE
#define SOC_PPD_FP_CONTROL_TYPE_KBP_COMMIT                     SOC_PPC_FP_CONTROL_TYPE_KBP_COMMIT
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
typedef SOC_PPC_FP_CONTROL_TYPE                                SOC_PPD_FP_CONTROL_TYPE;

typedef SOC_PPC_FP_QUAL_VAL                                    SOC_PPD_FP_QUAL_VAL;
typedef SOC_TMC_PMF_PFG_INFO                                   SOC_PPD_FP_PFG_INFO;
typedef SOC_PPC_FP_ETH_TAG_FORMAT                              SOC_PPD_FP_ETH_TAG_FORMAT;
typedef SOC_PPC_FP_DATABASE_INFO                               SOC_PPD_FP_DATABASE_INFO;
typedef SOC_PPC_FP_ACTION_VAL                                  SOC_PPD_FP_ACTION_VAL;
typedef SOC_PPC_FP_ENTRY_INFO                                  SOC_PPD_FP_ENTRY_INFO;
typedef SOC_PPC_FP_DIR_EXTR_ACTION_LOC                         SOC_PPD_FP_DIR_EXTR_ACTION_LOC;
typedef SOC_PPC_FP_DIR_EXTR_ACTION_VAL                         SOC_PPD_FP_DIR_EXTR_ACTION_VAL;
typedef SOC_PPC_FP_DIR_EXTR_ENTRY_INFO                         SOC_PPD_FP_DIR_EXTR_ENTRY_INFO;
typedef SOC_PPC_FP_CONTROL_INDEX                               SOC_PPD_FP_CONTROL_INDEX;
typedef SOC_PPC_FP_CONTROL_INFO                                SOC_PPD_FP_CONTROL_INFO;

typedef SOC_PPC_FP_DIAG_PP_PORT_INFO                           SOC_PPD_FP_DIAG_PP_PORT_INFO;
typedef SOC_PPC_FP_PACKET_DIAG_PARSER                          SOC_PPD_FP_PACKET_DIAG_PARSER;
typedef SOC_PPC_FP_PACKET_DIAG_PGM                             SOC_PPD_FP_PACKET_DIAG_PGM;
typedef SOC_PPC_FP_PACKET_DIAG_QUAL                            SOC_PPD_FP_PACKET_DIAG_QUAL;
typedef SOC_PPC_FP_PACKET_DIAG_DB_QUAL                         SOC_PPD_FP_PACKET_DIAG_DB_QUAL;
typedef SOC_PPC_FP_PACKET_DIAG_KEY                             SOC_PPD_FP_PACKET_DIAG_KEY;
typedef SOC_PPC_FP_PACKET_DIAG_TCAM_DT                         SOC_PPD_FP_PACKET_DIAG_TCAM_DT;
typedef SOC_PPC_FP_PACKET_DIAG_MACRO                           SOC_PPD_FP_PACKET_DIAG_MACRO;
typedef SOC_PPC_FP_PACKET_DIAG_INFO                            SOC_PPD_FP_PACKET_DIAG_INFO;

typedef SOC_PPC_FP_RESOURCE_KEY_LSB_MSB 					   SOC_PPD_FP_RESOURCE_KEY_LSB_MSB;
typedef SOC_PPC_FP_RESOURCE_KEY 						   	   SOC_PPD_FP_RESOURCE_KEY;
typedef SOC_PPC_FP_RESOURCE_DB_BANK 						   SOC_PPD_FP_RESOURCE_DB_BANK;
typedef SOC_PPC_FP_RESOURCE_ACTION 						   	   SOC_PPD_FP_RESOURCE_ACTION;
typedef SOC_PPC_FP_RESOURCE_DB_TCAM 						   SOC_PPD_FP_RESOURCE_DB_TCAM;
typedef SOC_PPC_FP_RESOURCE_DB_DE 						       SOC_PPD_FP_RESOURCE_DB_DE;
typedef SOC_PPC_FP_RESOURCE_DB 						   		   SOC_PPD_FP_RESOURCE_DB;
typedef SOC_PPC_FP_RESOURCE_BANK_DB 						   SOC_PPD_FP_RESOURCE_BANK_DB;
typedef SOC_PPC_FP_RESOURCE_BANK 						       SOC_PPD_FP_RESOURCE_BANK;
typedef SOC_PPC_FP_RESOURCE_PRESEL 						   	   SOC_PPD_FP_RESOURCE_PRESEL;
typedef SOC_PPC_FP_RESOURCE_DIAG_MODE                          SOC_PPD_FP_RESOURCE_DIAG_MODE;
typedef SOC_PPC_FP_RESOURCE_DIAG_INFO                          SOC_PPD_FP_RESOURCE_DIAG_INFO;

                                                                                     




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
 *   soc_ppd_fp_packet_format_group_set
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
 *   SOC_SAND_IN  int                  unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                  pfg_ndx -
 *     Packet Format Group index. Range: 0 - 4.
 *   SOC_SAND_IN  SOC_PPD_FP_PFG_INFO            *info -
 *     Packet-Format-Group parameters.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success -
 *     Indicate if the database is created successfully.
 * REMARKS:
 *   The user should set a minimal number of Packet Format
 *   Groups since each one uses many Hardware resources.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_fp_packet_format_group_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  pfg_ndx,
    SOC_SAND_IN  SOC_PPD_FP_PFG_INFO            *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_fp_packet_format_group_set" API.
 *     Refer to "soc_ppd_fp_packet_format_group_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_fp_packet_format_group_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  pfg_ndx,
    SOC_SAND_OUT SOC_PPD_FP_PFG_INFO            *info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_fp_database_create
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
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  SOC_PPD_FP_DATABASE_INFO                    *info -
 *     Database parameters.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
 *     Indicate if the database is created successfully.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_fp_database_create(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  SOC_PPD_FP_DATABASE_INFO                    *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_fp_database_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the database parameters.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_OUT SOC_PPD_FP_DATABASE_INFO                    *info -
 *     Database parameters.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_fp_database_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_OUT SOC_PPD_FP_DATABASE_INFO                    *info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_fp_database_destroy
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Destroy the database: all its entries are suppressed and
 *   the Database-ID is freed.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_fp_database_destroy(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx
  );

/*********************************************************************
* NAME:
 *   soc_ppd_fp_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an entry to the Database. The database entry is
 *   selected if the entire relevant packet field values are
 *   matched to the database entry qualifiers values. When
 *   the packet is qualified to several entries, the entry
 *   with the strongest priority is chosen.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                               entry_id_ndx -
 *     Entry handle ID. The entry index is a SW handle, to
 *     enable retrieving the entry attributes by the
 *     soc_ppd_fp_entry_get() function, and remove it by the
 *     soc_ppd_fp_entry_remove() function. The actual location of
 *     the entry in the database is selected according to the
 *     entry's priority. Range: 0 - 4K-1.
 *   SOC_SAND_IN  SOC_PPD_FP_ENTRY_INFO                       *info -
 *     Entry parameters.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
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
  soc_ppd_fp_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_IN  SOC_PPD_FP_ENTRY_INFO                       *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_fp_entry_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an entry from the Database.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                               entry_id_ndx -
 *     Entry-ID. Range: 0 - 4K-1.
 *   SOC_SAND_OUT uint8                               *is_found -
 *     If True, then the entry is found and the entry
 *     parameters are returned in the 'info' structure.
 *     Otherwise, the entry is not present in the Database.
 *   SOC_SAND_OUT SOC_PPD_FP_ENTRY_INFO                       *info -
 *     Entry parameters. Meaningful only the entry is found.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_fp_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_OUT uint8                                *is_found,
    SOC_SAND_INOUT SOC_PPD_FP_ENTRY_INFO              *info
  );

uint32
  soc_ppd_fp_entry_remove_by_key(
    SOC_SAND_IN     int                               unit,
    SOC_SAND_IN     uint32                               db_id_ndx,
    SOC_SAND_INOUT  SOC_PPD_FP_ENTRY_INFO                *info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_fp_entry_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an entry from the Database.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                               entry_id_ndx -
 *     Entry-ID. Range: 0 - 4K-1.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_fp_entry_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_IN  uint32                               is_sw_remove_only
  );

/*********************************************************************
* NAME:
 *   soc_ppd_fp_database_entries_get_block
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the Database entries. The function returns list of
 *   entries that were added to a database with database ID
 *   'db_id_ndx'.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range -
 *     Block range to get entries in this Database.
 *   SOC_SAND_OUT SOC_PPD_FP_ENTRY_INFO                       *entries -
 *     Database entries.
 *   SOC_SAND_OUT uint32                                *nof_entries -
 *     Number of valid entries.
 * REMARKS:
 *   This API can be called only if the Database is NOT of
 *   type 'direct extraction'.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_fp_database_entries_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range,
    SOC_SAND_OUT SOC_PPD_FP_ENTRY_INFO                       *entries,
    SOC_SAND_OUT uint32                                *nof_entries
  );

/*********************************************************************
* NAME:
 *   soc_ppd_fp_direct_extraction_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an entry to the Database. The database entry is
 *   selected if all the Packet Qualifier field values are in
 *   the Database entry range.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                               entry_id_ndx -
 *     Entry-ID. Range: 0 - 15.
 *   SOC_SAND_IN  SOC_PPD_FP_DIR_EXTR_ENTRY_INFO              *info -
 *     Entry parameters.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
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
  soc_ppd_fp_direct_extraction_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_IN  SOC_PPD_FP_DIR_EXTR_ENTRY_INFO              *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_fp_direct_extraction_entry_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an entry from the Database.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                               entry_id_ndx -
 *     Entry-ID. Range: 0 - 15.
 *   SOC_SAND_OUT uint8                               *is_found -
 *     If True, then the entry is found and the entry
 *     parameters are returned in the 'info' structure.
 *     Otherwise, the entry is not present in the Database.
 *   SOC_SAND_OUT SOC_PPD_FP_DIR_EXTR_ENTRY_INFO              *info -
 *     Entry parameters. Meaningful only the entry is found.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_fp_direct_extraction_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_OUT uint8                               *is_found,
    SOC_SAND_OUT SOC_PPD_FP_DIR_EXTR_ENTRY_INFO              *info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_fp_direct_extraction_entry_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an entry from the Database.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_IN  uint32                               entry_id_ndx -
 *     Entry-ID. Range: 0 - 15.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_fp_direct_extraction_entry_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx
  );

/*********************************************************************
* NAME:
 *   soc_ppd_fp_direct_extraction_db_entries_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the Database entries.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               db_id_ndx -
 *     Database-ID. Range: 0 - 127.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range -
 *     Block range to get entries in this Database.
 *   SOC_SAND_OUT SOC_PPD_FP_DIR_EXTR_ENTRY_INFO              *entries -
 *     Database entries.
 *   SOC_SAND_OUT uint32                                *nof_entries -
 *     Number of valid entries.
 * REMARKS:
 *   This API can be called only if the Database is of type
 *   'direct extraction'.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_fp_direct_extraction_db_entries_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE     *block_range,
    SOC_SAND_OUT SOC_PPD_FP_DIR_EXTR_ENTRY_INFO *entries,
    SOC_SAND_OUT uint32                   *nof_entries
  );

/*********************************************************************
* NAME:
 *   soc_ppd_fp_control_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set one of the control options.
 * INPUT:
 *   SOC_SAND_IN  int                  unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FP_CONTROL_INDEX      *control_ndx -
 *     Index for the control set API.
 *   SOC_SAND_IN  SOC_PPD_FP_CONTROL_INFO        *info -
 *     Type and Values of selected control option.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success -
 *     Indicate if the operation has succeeded.
 * REMARKS:
 *   The exact semantics of info are determined by the
 *   control option specified by type.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_fp_control_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  int                  core_id,
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_INDEX      *control_ndx,
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_INFO        *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success
  );

/*********************************************************************
*     Gets the configuration set by the "soc_ppd_fp_control_set"
 *     API.
 *     Refer to "soc_ppd_fp_control_set" API for details.
*********************************************************************/
uint32
  soc_ppd_fp_control_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  int                  core_id,
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_INDEX       *control_ndx,
    SOC_SAND_OUT SOC_PPD_FP_CONTROL_INFO        *info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_fp_egr_db_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the mapping between the Packet forward type and the
 *   Port profile to the Database-ID.
 * INPUT:
 *   SOC_SAND_IN  int                  unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FP_FWD_TYPE            fwd_type_ndx -
 *     Packet forward type.
 *   SOC_SAND_IN  uint32                  port_profile_ndx -
 *     PP-Port profile. Range: 0 - 3.
 *   SOC_SAND_IN  uint32                   db_id -
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
  soc_ppd_fp_egr_db_map_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPD_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                  port_profile_ndx,
    SOC_SAND_IN  uint32                   db_id
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_fp_egr_db_map_set" API.
 *     Refer to "soc_ppd_fp_egr_db_map_set" API for details.
*********************************************************************/
uint32
  soc_ppd_fp_egr_db_map_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPD_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                  port_profile_ndx,
    SOC_SAND_OUT uint32                   *db_id
  );

/*********************************************************************
*     Compress a TCAM Database: compress the entries to minimum
*     number of banks.
*********************************************************************/
uint32
  soc_ppd_fp_database_compress(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  db_id_ndx
  );


/*********************************************************************
* NAME:
 *   soc_ppd_fp_packet_diag_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the Field Processing of the last packets.
 * INPUT:
 *   SOC_SAND_IN  int               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_INFO *info -
 *     Field Processor specifications for this packet.
 * REMARKS:
 *   This API must be called during a continuous stream of
 *   the identical packets coming from the same source.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_fp_packet_diag_get(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core_id,
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_INFO *info
  );


uint32
  soc_ppd_fp_resource_diag_get(
    SOC_SAND_IN 	int               		unit,
	SOC_SAND_IN	 	SOC_PPD_FP_RESOURCE_DIAG_MODE	mode,
    SOC_SAND_OUT 	SOC_PPD_FP_RESOURCE_DIAG_INFO	*info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_fp_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_ppd_api_fp module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_fp_get_procs_ptr(void);

void
  SOC_PPD_FP_QUAL_VAL_clear(
    SOC_SAND_OUT SOC_PPD_FP_QUAL_VAL *info
  );

void
  SOC_PPD_FP_ETH_TAG_FORMAT_clear(
    SOC_SAND_OUT SOC_PPD_FP_ETH_TAG_FORMAT *info
  );

void
  SOC_PPD_FP_PFG_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FP_PFG_INFO *info
  );

void
  SOC_PPD_FP_DATABASE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FP_DATABASE_INFO *info
  );

void
  SOC_PPD_FP_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PPD_FP_ACTION_VAL *info
  );

void
  SOC_PPD_FP_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FP_ENTRY_INFO *info
  );

void
  SOC_PPD_FP_DIR_EXTR_ACTION_LOC_clear(
    SOC_SAND_OUT SOC_PPD_FP_DIR_EXTR_ACTION_LOC *info
  );

void
  SOC_PPD_FP_DIR_EXTR_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PPD_FP_DIR_EXTR_ACTION_VAL *info
  );

void
  SOC_PPD_FP_DIR_EXTR_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FP_DIR_EXTR_ENTRY_INFO *info
  );

void
  SOC_PPD_FP_CONTROL_INDEX_clear(
    SOC_SAND_OUT SOC_PPD_FP_CONTROL_INDEX *info
  );

void
  SOC_PPD_FP_CONTROL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FP_CONTROL_INFO *info
  );
void
  SOC_PPD_FP_DIAG_PP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FP_DIAG_PP_PORT_INFO *info
  );

void
  SOC_PPD_FP_PACKET_DIAG_PARSER_clear(
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_PARSER *info
  );

void
  SOC_PPD_FP_PACKET_DIAG_PGM_clear(
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_PGM *info
  );

void
  SOC_PPD_FP_PACKET_DIAG_QUAL_clear(
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_QUAL *info
  );

void
  SOC_PPD_FP_PACKET_DIAG_DB_QUAL_clear(
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_DB_QUAL *info
  );

void
  SOC_PPD_FP_PACKET_DIAG_KEY_clear(
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_KEY *info
  );

void
  SOC_PPD_FP_PACKET_DIAG_TCAM_DT_clear(
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_TCAM_DT *info
  );

void
  SOC_PPD_FP_PACKET_DIAG_MACRO_clear(
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_MACRO *info
  );

void
  SOC_PPD_FP_PACKET_DIAG_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FP_PACKET_DIAG_INFO *info
  );

void
  SOC_PPD_FP_RESOURCE_KEY_LSB_MSB_clear(
	  SOC_SAND_OUT SOC_PPD_FP_RESOURCE_KEY_LSB_MSB *info
	);
void
  SOC_PPD_FP_RESOURCE_KEY_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_KEY *info
  );
void
  SOC_PPD_FP_RESOURCE_DB_BANK_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_DB_BANK *info
  );
void
  SOC_PPD_FP_RESOURCE_ACTION_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_ACTION *info
  );
void
  SOC_PPD_FP_RESOURCE_DB_TCAM_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_DB_TCAM *info
  );
void
  SOC_PPD_FP_RESOURCE_DB_DE_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_DB_DE *info
  );
void
  SOC_PPD_FP_RESOURCE_DB_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_DB *info
  );
void
  SOC_PPD_FP_RESOURCE_BANK_DB_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_BANK_DB *info
  );
void
  SOC_PPD_FP_RESOURCE_BANK_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_BANK *info
  );
void
  SOC_PPD_FP_RESOURCE_PRESEL_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_PRESEL *info
  );
void
  SOC_PPD_FP_RESOURCE_DIAG_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FP_RESOURCE_DIAG_INFO *info
  );

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_FP_QUAL_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FP_QUAL_TYPE enum_val
  );

const char*
  SOC_PPD_FP_PKT_HDR_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FP_PKT_HDR_TYPE enum_val
  );

const char*
  SOC_PPD_FP_FWD_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FP_FWD_TYPE enum_val
  );

const char*
  SOC_PPD_FP_PROCESSING_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FP_PROCESSING_TYPE enum_val
  );

const char*
  SOC_PPD_FP_BASE_HEADER_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FP_BASE_HEADER_TYPE enum_val
  );

const char*
  SOC_PPD_FP_ACTION_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FP_ACTION_TYPE enum_val
  );

const char*
  SOC_PPD_FP_DATABASE_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FP_DATABASE_TYPE enum_val
  );

const char*
  SOC_PPD_FP_PREDEFINED_ACL_KEY_to_string(
    SOC_SAND_IN  SOC_PPD_FP_PREDEFINED_ACL_KEY enum_val
  );

const char*
  SOC_PPD_FP_CONTROL_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_TYPE enum_val
  );

void
  SOC_PPD_FP_QUAL_VAL_print(
    SOC_SAND_IN  SOC_PPD_FP_QUAL_VAL *info
  );

void
  SOC_PPD_FP_PFG_INFO_print(
    SOC_SAND_IN  SOC_PPD_FP_PFG_INFO *info
  );

void
  SOC_PPD_FP_DATABASE_INFO_print(
    SOC_SAND_IN  SOC_PPD_FP_DATABASE_INFO *info
  );

void
  SOC_PPD_FP_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PPD_FP_ACTION_VAL *info
  );

void
  SOC_PPD_FP_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPD_FP_ENTRY_INFO *info
  );

void
  SOC_PPD_FP_DIR_EXTR_ACTION_LOC_print(
    SOC_SAND_IN  SOC_PPD_FP_DIR_EXTR_ACTION_LOC *info
  );

void
  SOC_PPD_FP_DIR_EXTR_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PPD_FP_DIR_EXTR_ACTION_VAL *info
  );

void
  SOC_PPD_FP_DIR_EXTR_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPD_FP_DIR_EXTR_ENTRY_INFO *info
  );

void
  SOC_PPD_FP_CONTROL_INDEX_print(
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_INDEX *info
  );

void
  SOC_PPD_FP_CONTROL_INFO_print(
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_INFO *info
  );
void
  SOC_PPD_FP_DIAG_PP_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPD_FP_DIAG_PP_PORT_INFO *info
  );

void
  SOC_PPD_FP_PACKET_DIAG_PARSER_print(
    SOC_SAND_IN  SOC_PPD_FP_PACKET_DIAG_PARSER *info
  );

void
  SOC_PPD_FP_PACKET_DIAG_PGM_print(
    SOC_SAND_IN  SOC_PPD_FP_PACKET_DIAG_PGM *info
  );

void
  SOC_PPD_FP_PACKET_DIAG_QUAL_print(
    SOC_SAND_IN  SOC_PPD_FP_PACKET_DIAG_QUAL *info
  );

void
  SOC_PPD_FP_PACKET_DIAG_DB_QUAL_print(
  SOC_SAND_IN  uint32                  ind,
   SOC_SAND_IN  SOC_PPD_FP_PACKET_DIAG_DB_QUAL *info
  );

void
  SOC_PPD_FP_PACKET_DIAG_KEY_print(
    SOC_SAND_IN  SOC_PPD_FP_PACKET_DIAG_KEY *info
  );

void
  SOC_PPD_FP_PACKET_DIAG_TCAM_DT_print(
    SOC_SAND_IN  SOC_PPD_FP_PACKET_DIAG_TCAM_DT *info
  );

void
  SOC_PPD_FP_PACKET_DIAG_MACRO_print(
    SOC_SAND_IN  SOC_PPD_FP_PACKET_DIAG_MACRO *info
  );

void
  SOC_PPD_FP_PACKET_DIAG_INFO_print(
    SOC_SAND_IN  SOC_PPD_FP_PACKET_DIAG_INFO *info
  );

void
  SOC_PPD_FP_RESOURCE_KEY_LSB_MSB_print(
	SOC_SAND_IN	SOC_PPC_FP_KEY_BIT_TYPE_LSB_MSB lsb_msb,
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_KEY_LSB_MSB *info
	);

void
  SOC_PPD_FP_RESOURCE_KEY_print(
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_KEY *info
  );

void
  SOC_PPD_FP_RESOURCE_DB_BANK_print(
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_DB_BANK *info
  );

void
  SOC_PPD_FP_RESOURCE_ACTION_print(
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_ACTION *info
  );

void
  SOC_PPD_FP_RESOURCE_DB_TCAM_print(
    SOC_SAND_IN int                         unit,
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_DB_TCAM *info
  );

void
  SOC_PPD_FP_RESOURCE_DB_DE_print(
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_DB_DE *info
  );

void
  SOC_PPD_FP_RESOURCE_DB_print(
    SOC_SAND_IN int                    unit,
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_DB *info
  );

void
  SOC_PPD_FP_RESOURCE_BANK_DB_print(
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_BANK_DB *info
  );

void
  SOC_PPD_FP_RESOURCE_BANK_print(
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_BANK *info
  );

void
  SOC_PPD_FP_RESOURCE_PRESEL_print(
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_PRESEL *info
  );

void
  SOC_PPD_FP_RESOURCE_DIAG_INFO_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN SOC_PPD_FP_RESOURCE_DIAG_INFO *info
  );
#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_fp_packet_format_group_set_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  pfg_ndx,
    SOC_SAND_IN  SOC_PPD_FP_PFG_INFO            *info
  );

void
  soc_ppd_fp_packet_format_group_get_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  pfg_ndx
  );

void
  soc_ppd_fp_database_create_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  SOC_PPD_FP_DATABASE_INFO                    *info
  );

void
  soc_ppd_fp_database_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx
  );

void
  soc_ppd_fp_database_destroy_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx
  );

void
  soc_ppd_fp_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_IN  SOC_PPD_FP_ENTRY_INFO                       *info
  );

void
  soc_ppd_fp_entry_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx
  );

void
  soc_ppd_fp_entry_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx
  );

void
  soc_ppd_fp_database_entries_get_block_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range
  );

void
  soc_ppd_fp_direct_extraction_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx,
    SOC_SAND_IN  SOC_PPD_FP_DIR_EXTR_ENTRY_INFO              *info
  );

void
  soc_ppd_fp_direct_extraction_entry_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx
  );

void
  soc_ppd_fp_direct_extraction_entry_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_IN  uint32                               entry_id_ndx
  );

void
  soc_ppd_fp_direct_extraction_db_entries_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               db_id_ndx,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                  *block_range
  );

void
  soc_ppd_fp_control_set_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_INDEX      *control_ndx,
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_INFO        *info
  );

void
  soc_ppd_fp_control_get_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPD_FP_CONTROL_INDEX      *control_ndx
  );

void
  soc_ppd_fp_egr_db_map_set_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPD_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                  port_profile_ndx,
    SOC_SAND_IN  uint32                   db_id
  );

void
  soc_ppd_fp_egr_db_map_get_print(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPD_FP_FWD_TYPE            fwd_type_ndx,
    SOC_SAND_IN  uint32                  port_profile_ndx
  );
void
  soc_ppd_fp_packet_diag_get_print(
    SOC_SAND_IN  int               unit
  );

#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPD_API_FP_INCLUDED__*/
#endif

