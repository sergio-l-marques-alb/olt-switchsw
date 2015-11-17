/* $Id: ppd_api_eg_encap.h,v 1.36 Broadcom SDK $
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
 * $
*/
/******************************************************************
*
* FILENAME:       DuneDriver/ppd/include/soc_ppd_api_eg_encap.h
*
* MODULE PREFIX:  soc_ppd_eg
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

#ifndef __SOC_PPD_API_EG_ENCAP_INCLUDED__
/* { */
#define __SOC_PPD_API_EG_ENCAP_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_eg_encap.h>

#include <soc/dpp/PPD/ppd_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximum number of tunnels per encapsulation             */

/*     Data entry maximum size in uint32 */
#define  SOC_PPD_EG_ENCAP_DATA_INFO_MAX_SIZE                        (SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE)

/*    Number of banks in egress encapsulation                   */ 
#define SOC_PPD_EG_ENCAP_NOF_BANKS              (SOC_PPC_EG_ENCAP_NOF_BANKS)

#define SOC_PPD_EG_ENCAP_NEXT_EEP_INVALID                        (SOC_PPC_EG_ENCAP_NEXT_EEP_INVALID)


/* update next-out-LIF */
#define SOC_PPD_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF     (SOC_PPC_EG_ENCAP_ENTRY_UPDATE_NEXT_LIF)

/* update drop  */
#define SOC_PPD_EG_ENCAP_ENTRY_UPDATE_DROP     (SOC_PPC_EG_ENCAP_ENTRY_UPDATE_DROP)

#define SOC_PPD_PRGE_DATA_ENTRY_LSBS_COUPLING_SWAP SOC_PPC_PRGE_DATA_ENTRY_LSBS_COUPLING_SWAP
#define SOC_PPD_PRGE_DATA_ENTRY_LSBS_RSPAN         SOC_PPC_PRGE_DATA_ENTRY_LSBS_RSPAN
#define SOC_PPD_PRGE_DATA_ENTRY_LSBS_PON_TUNNEL    SOC_PPC_PRGE_DATA_ENTRY_LSBS_PON_TUNNEL


/* definition for ERSPAN mirroring*/
#define SOC_PPD_EG_ENCAP_ERSPAN_PRIO_VAL          SOC_PPC_EG_ENCAP_ERSPAN_PRIO_VAL 
#define SOC_PPD_EG_ENCAP_ERSPAN_TRUNC_VAL         SOC_PPC_EG_ENCAP_ERSPAN_TRUNC_VAL



/* } */
/*************
 * MACROS    *
 *************/


/* { */

#define SOC_PPD_EG_ENCAP_EEDB_DATA_ERSPAN_FORMAT_SET   SOC_PPC_EG_ENCAP_EEDB_DATA_ERSPAN_FORMAT_SET

#define SOC_PPD_EG_ENCAP_DATA_ERSPAN_FORMAT_SPAN_ID_GET(__unit, __eg_encap_data_info)  (((__eg_encap_data_info)->data_entry[0] >> 8)& 0x3ff)

/* Macro determines weather a EEDB entry is used for ERSPAN or RSAPAN*/
#define SOC_PPD_EG_ENCAP_EEDB_IS_FORMAT_ERSPAN(__eg_encap_data_info)  (((__eg_encap_data_info)->data_entry[0]  & 0xff) == SOC_PPC_PRGE_DATA_ENTRY_LSBS_ERSPAN)


  /* 6LSBytes {TPID(16),PCP(4),VID(12)} Directly Extracted from EES1[55:8]*/
#define SOC_PPD_EG_ENCAP_DATA_RSPAN_FORMAT_SET(__unit, __tpid, __pcp, __vid,__eg_encap_data_info)  \
      (__eg_encap_data_info)->data_entry[1] = (__tpid>>8) & 0xFF;  \
      (__eg_encap_data_info)->data_entry[0] = (((__tpid & 0xFF) << 24) | ((__pcp) << 21) | ((__vid) << 8) | SOC_PPD_PRGE_DATA_ENTRY_LSBS_RSPAN);

/* 6LSBytes {TPID(16),PCP(4),VID(12)} Directly Extracted from EES1[55:8]*/
#define SOC_PPD_EG_ENCAP_DATA_RSPAN_FORMAT_GET(tpid, pcp, vid, __eg_encap_data_info) \
      tpid = (__eg_encap_data_info)->data_entry[0]  >>24;\
      pcp = ((__eg_encap_data_info)->data_entry[0]  & 0xe00000) >>21;\
      vid =  ((__eg_encap_data_info)->data_entry[0]  & 0x1FFF00) >>8;

  /* Eth-Type (16), MPLS-Label(20b), 6'h0, identifier(2) */
#define SOC_PPD_EG_ENCAP_DATA_COUPLING_SWAP_FORMAT_SET(__unit, __ether_type, __mpls_label, __eg_encap_data_info)  \
      (__eg_encap_data_info)->data_entry[1] = 0;  \
      (__eg_encap_data_info)->data_entry[0] = (((__ether_type) << 28) | ((__mpls_label) << 8) | SOC_PPD_PRGE_DATA_ENTRY_LSBS_COUPLING_SWAP);

#define SOC_PPD_EG_ENCAP_DATA_COUPLING_SWAP_LABEL_GET(__unit, __eg_encap_data_info)  \
      (((__eg_encap_data_info)->data_entry[0] >> 8) & 0xFFFFF);

#define SOC_PPD_EG_ENCAP_DATA_COUPLING_SWAP_TYPE(__unit, __entry_type, __eg_encap_data_info) \
      ( (__entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_DATA) && ((__eg_encap_data_info)->data_entry[0] & SOC_PPD_PRGE_DATA_ENTRY_LSBS_COUPLING_SWAP))

#define SOC_PPD_EG_ENCAP_DATA_FORMAT_GET(__unit, __eg_encap_data_info)  ((__eg_encap_data_info)->data_entry[0] & 3)

  /* 16'h0, LLID-TPID(16b), PCP(4),Tunnel-ID(12), 6'h0, identifier(2) */
#define SOC_PPD_EG_ENCAP_DATA_PON_TUNNEL_FORMAT_SET(__unit, __tpid, __pcp, __tunnel_id, __eg_encap_data_info)  \
      (__eg_encap_data_info)->data_entry[1] = (__tpid>>8) & 0xFF;  \
      (__eg_encap_data_info)->data_entry[0] = ((((__tpid) & 0xFF) << 24) | ((__pcp) << 20) | ((__tunnel_id) << 8) | SOC_PPD_PRGE_DATA_ENTRY_LSBS_PON_TUNNEL);

#define SOC_PPD_EG_ENCAP_DATA_PON_TUNNEL_FORMAT_TPID_GET(__unit, __eg_encap_data_info)  \
      ((((__eg_encap_data_info)->data_entry[1]&0xFF) << 8) | (((__eg_encap_data_info)->data_entry[0] >> 24) & 0xFF))

#define SOC_PPD_EG_ENCAP_DATA_PON_TUNNEL_FORMAT_PCP_GET(__unit, __eg_encap_data_info)  \
      (((__eg_encap_data_info)->data_entry[0] >> 20) & 0xF)

#define SOC_PPD_EG_ENCAP_DATA_PON_TUNNEL_FORMAT_TUNNEL_ID_GET(__unit, __eg_encap_data_info)  \
      (((__eg_encap_data_info)->data_entry[0] >> 8) & 0xFFF)

#ifdef BCM_88660_A0
/* LLID-TPID(16b), PCP(4),Tunnel-ID(12), 2'h0, identifier(2) */
#define SOC_PPD_EG_ENCAP_DATA_2ND_PON_TUNNEL_FORMAT_SET(__unit, __tpid, __pcp, __tunnel_id, __eg_encap_data_info)  \
      (__eg_encap_data_info)->data[1] = (__tpid>>12) & 0x0F;  \
      (__eg_encap_data_info)->data[0] = ((((__tpid) & 0x0FFF) << 20) | ((__pcp) << 16) | (__tunnel_id << 4) | 0x04)



#define SOC_PPD_EG_ENCAP_DATA_2ND_PON_TUNNEL_FORMAT_TUNNEL_ID_GET(__unit, __eg_encap_data_info)  \
      (((__eg_encap_data_info)->data[0] >> 4) & 0x0FFF)
#endif

/* CUD0 [0:23], CUD1 [24:37] */
#define SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_NOF_BITS (24)

#define SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_MASK ((1 << SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_NOF_BITS) - 1) 

#define SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_COMPLIMENT_ENTRTY_0_MASK ((1 << (32 - SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_NOF_BITS)) - 1) 

#define SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_COMPLIMENT_ENTRTY_1_MASK ((1 << ((SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_NOF_BITS*2) - 32 )) - 1) 

#define SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_NOF_BITS (28)

#define SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_MASK ((1 << SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_NOF_BITS) - 1) 

#define SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_COMPLIMENT_ENTRTY_0_MASK ((1 << (32 - SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_NOF_BITS)) - 1) 

#define SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_COMPLIMENT_ENTRTY_1_MASK ((1 << ((SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_NOF_BITS*2) - 32 )) - 1) 


#define SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_SET(__unit, __cud_1, __cud_2, __eg_encap_data_info) \
      (__eg_encap_data_info)->data_entry[0] = \
      ((((__cud_2) & SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_COMPLIMENT_ENTRTY_0_MASK) << SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_NOF_BITS) |\
      ((__cud_1) & SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_MASK)); \
      (__eg_encap_data_info)->data_entry[1] = \
      ((__cud_2  >> (32 - SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_NOF_BITS)) & SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_COMPLIMENT_ENTRTY_1_MASK);

#define SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_GET(__unit, __index, __eg_encap_data_info) \
      (((((__eg_encap_data_info)->data_entry[1] & (SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_COMPLIMENT_ENTRTY_1_MASK)) << (32 - SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_NOF_BITS)) * (__index)) | \
      (((__eg_encap_data_info)->data_entry[0] >> (__index * SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_NOF_BITS)) & SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_MASK));

#define SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_SET(__unit, __cud_1, __cud_2, __eg_encap_data_info) \
      (__eg_encap_data_info)->data_entry[0] = \
      ((((__cud_2) & SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_COMPLIMENT_ENTRTY_0_MASK) << SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_NOF_BITS) |\
      ((__cud_1) & SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_MASK)); \
      (__eg_encap_data_info)->data_entry[1] = \
      ((__cud_2  >> (32 - SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_NOF_BITS)) & SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_COMPLIMENT_ENTRTY_1_MASK);

#define SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_GET(__unit, __index, __eg_encap_data_info) \
      (((((__eg_encap_data_info)->data_entry[1] & (SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_COMPLIMENT_ENTRTY_1_MASK)) << (32 - SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_NOF_BITS)) * (__index)) | \
      (((__eg_encap_data_info)->data_entry[0] >> (__index * SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_NOF_BITS)) & SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_MASK));


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
  SOC_PPD_EG_ENCAP_RANGE_INFO_SET = SOC_PPD_PROC_DESC_BASE_EG_ENCAP_FIRST,
  SOC_PPD_EG_ENCAP_RANGE_INFO_SET_PRINT,
  SOC_PPD_EG_ENCAP_RANGE_INFO_GET,
  SOC_PPD_EG_ENCAP_RANGE_INFO_GET_PRINT,
  SOC_PPD_EG_ENCAP_NULL_LIF_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_NULL_LIF_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_DATA_LIF_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_DATA_LIF_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_AC_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_AC_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_PWE_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_PWE_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_POP_COMMAND_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_VSI_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_VSI_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_MPLS_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_MPLS_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_IPV4_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_IPV4_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_IPV6_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_IPV6_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_MIRROR_ENTRY_SET,
  SOC_PPD_EG_ENCAP_MIRROR_ENTRY_GET,
  SOC_PPD_EG_ENCAP_PORT_ERSPAN_SET,
  SOC_PPD_EG_ENCAP_PORT_ERSPAN_GET,
  SOC_PPD_EG_ENCAP_LL_ENTRY_ADD,
  SOC_PPD_EG_ENCAP_LL_ENTRY_ADD_PRINT,
  SOC_PPD_EG_ENCAP_ENTRY_REMOVE,
  SOC_PPD_EG_ENCAP_ENTRY_REMOVE_PRINT,
  SOC_PPD_EG_ENCAP_ENTRY_GET,
  SOC_PPD_EG_ENCAP_ENTRY_GET_PRINT,
  SOC_PPD_EG_ENCAP_ENTRY_TYPE_GET,
  SOC_PPD_EG_ENCAP_ENTRY_TYPE_GET_PRINT,
  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET,
  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_SET_PRINT,
  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_GET,
  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_GET_PRINT,
  SOC_PPD_EG_ENCAP_PUSH_EXP_INFO_SET,
  SOC_PPD_EG_ENCAP_PUSH_EXP_INFO_SET_PRINT,
  SOC_PPD_EG_ENCAP_PUSH_EXP_INFO_GET,
  SOC_PPD_EG_ENCAP_PUSH_EXP_INFO_GET_PRINT,
  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_SET,
  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_SET_PRINT,
  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_GET,
  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_GET_PRINT,
  SOC_PPD_EG_ENCAP_GLBL_INFO_SET,
  SOC_PPD_EG_ENCAP_GLBL_INFO_SET_PRINT,
  SOC_PPD_EG_ENCAP_GLBL_INFO_GET,
  SOC_PPD_EG_ENCAP_GLBL_INFO_GET_PRINT,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_PRINT,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_PRINT,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_PRINT,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_PRINT,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_PRINT,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET,
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_PRINT,
  SOC_PPD_EG_ENCAP_GET_PROCS_PTR,
  SOC_PPD_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_INIT,
  SOC_PPD_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_SET,
  SOC_PPD_EG_ENCAP_DATA_LOCAL_TO_GLOBAL_CUD_GET,
  /*
   * } Auto generated. Do not edit previous section.
   */
  /*
   * Last element. Do no touch.
   */
  SOC_PPD_EG_ENCAP_PROCEDURE_DESC_LAST
} SOC_PPD_EG_ENCAP_PROCEDURE_DESC;
#define SOC_PPD_EG_ENCAP_EEP_TYPE_LIF_EEP                      SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP
#define SOC_PPD_EG_ENCAP_EEP_TYPE_TUNNEL_EEP                   SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP
#define SOC_PPD_EG_ENCAP_EEP_TYPE_LL                           SOC_PPC_EG_ENCAP_EEP_TYPE_LL
#define SOC_PPD_EG_ENCAP_EEP_TYPE_VSI                          SOC_PPC_EG_ENCAP_EEP_TYPE_VSI
#define SOC_PPD_NOF_EG_ENCAP_EEP_TYPES                         SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX
typedef SOC_PPC_EG_ENCAP_EEP_TYPE                              SOC_PPD_EG_ENCAP_EEP_TYPE;

#define SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_NONE               SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_NONE 
#define SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_ETHER_IP           SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ETHER_IP
#define SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_BASIC_GRE          SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_BASIC_GRE
#define SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE        SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE
#define SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_VXLAN              SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_VXLAN
typedef SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE                    SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE;

#define SOC_PPD_EG_ENCAP_EXP_MARK_MODE_FROM_PUSH_PROFILE       SOC_PPC_EG_ENCAP_EXP_MARK_MODE_FROM_PUSH_PROFILE
typedef SOC_PPC_EG_ENCAP_EXP_MARK_MODE                         SOC_PPD_EG_ENCAP_EXP_MARK_MODE;

#define SOC_PPD_EG_ENCAP_ENTRY_TYPE_VSI                        SOC_PPC_EG_ENCAP_ENTRY_TYPE_VSI
#define SOC_PPD_EG_ENCAP_ENTRY_TYPE_AC                         SOC_PPC_EG_ENCAP_ENTRY_TYPE_AC
#define SOC_PPD_EG_ENCAP_ENTRY_TYPE_SWAP_CMND                  SOC_PPC_EG_ENCAP_ENTRY_TYPE_SWAP_CMND
#define SOC_PPD_EG_ENCAP_ENTRY_TYPE_PWE                        SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE
#define SOC_PPD_EG_ENCAP_ENTRY_TYPE_POP_CMND                   SOC_PPC_EG_ENCAP_ENTRY_TYPE_POP_CMND
#define SOC_PPD_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP                 SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP
#define SOC_PPD_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP                 SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP
#define SOC_PPD_EG_ENCAP_ENTRY_TYPE_LL_ENCAP                   SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP
#define SOC_PPD_EG_ENCAP_ENTRY_TYPE_NULL                       SOC_PPC_EG_ENCAP_ENTRY_TYPE_NULL
#define SOC_PPD_EG_ENCAP_ENTRY_TYPE_DATA                       SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA
#define SOC_PPD_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP                 SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP
#define SOC_PPD_EG_ENCAP_ENTRY_TYPE_OVERLAY_ARP_ENCAP          SOC_PPC_EG_ENCAP_ENTRY_TYPE_OVERLAY_ARP_ENCAP
#define SOC_PPD_EG_ENCAP_ENTRY_TYPE_ROO_LL_ENCAP               SOC_PPC_EG_ENCAP_ENTRY_TYPE_ROO_LL_ENCAP
typedef SOC_PPC_EG_ENCAP_ENTRY_TYPE                            SOC_PPD_EG_ENCAP_ENTRY_TYPE;

#define SOC_PPD_EG_ENCAP_ACCESS_PHASE_ONE                      SOC_PPC_EG_ENCAP_ACCESS_PHASE_ONE
#define SOC_PPD_EG_ENCAP_ACCESS_PHASE_TWO                      SOC_PPC_EG_ENCAP_ACCESS_PHASE_TWO 
#define SOC_PPD_EG_ENCAP_ACCESS_PHASE_THREE                    SOC_PPC_EG_ENCAP_ACCESS_PHASE_THREE 
#define SOC_PPD_EG_ENCAP_ACCESS_PHASE_FOUR                     SOC_PPC_EG_ENCAP_ACCESS_PHASE_FOUR
#define SOC_PPD_NOF_EG_ENCAP_ACCESS_PHASE_TYPES                SOC_PPC_NOF_EG_ENCAP_ACCESS_PHASE_TYPES    
typedef SOC_PPC_EG_ENCAP_ACCESS_PHASE                           SOC_PPD_EG_ENCAP_ACCESS_PHASE;

typedef SOC_PPC_EG_ENCAP_RANGE_INFO                            SOC_PPD_EG_ENCAP_RANGE_INFO;
typedef SOC_PPC_EG_ENCAP_SWAP_INFO                             SOC_PPD_EG_ENCAP_SWAP_INFO;
typedef SOC_PPC_EG_ENCAP_PWE_INFO                              SOC_PPD_EG_ENCAP_PWE_INFO;
typedef SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO                     SOC_PPD_EG_ENCAP_POP_INTO_ETH_INFO;
typedef SOC_PPC_EG_ENCAP_POP_INFO                              SOC_PPD_EG_ENCAP_POP_INFO;
typedef SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO                      SOC_PPD_EG_ENCAP_MPLS_TUNNEL_INFO;
typedef SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO                       SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO;
typedef SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO                      SOC_PPD_EG_ENCAP_IPV4_TUNNEL_INFO;
typedef SOC_PPC_EG_ENCAP_IPV6_TUNNEL_INFO                      SOC_PPD_EG_ENCAP_IPV6_TUNNEL_INFO;
typedef SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO                       SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO;
typedef SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO                       SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO;
typedef SOC_PPC_EG_ENCAP_MIRROR_TUNNEL_INFO                    SOC_PPD_EG_ENCAP_MIRROR_TUNNEL_INFO;
typedef SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO                     SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO;
typedef SOC_PPC_EG_ENCAP_DATA_INFO                             SOC_PPD_EG_ENCAP_DATA_INFO;
typedef SOC_PPC_EG_ENCAP_LL_INFO                               SOC_PPD_EG_ENCAP_LL_INFO;
typedef SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO                     SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO;
typedef SOC_PPC_EG_ENCAP_PUSH_EXP_KEY                          SOC_PPD_EG_ENCAP_PUSH_EXP_KEY;
typedef SOC_PPC_EG_ENCAP_PWE_GLBL_INFO                         SOC_PPD_EG_ENCAP_PWE_GLBL_INFO;
typedef SOC_PPC_EG_ENCAP_GLBL_INFO                         SOC_PPD_EG_ENCAP_GLBL_INFO;
typedef SOC_PPC_EG_ENCAP_ENTRY_VALUE                           SOC_PPD_EG_ENCAP_ENTRY_VALUE;
typedef SOC_PPC_EG_ENCAP_ENTRY_INFO                            SOC_PPD_EG_ENCAP_ENTRY_INFO;
typedef SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO                        SOC_PPD_EG_ENCAP_VSI_ENCAP_INFO;


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
 *   soc_ppd_eg_encap_entry_init
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Inits devision of the Egress Encapsulation Table between
 *   the different usages (Link layer/ IP tunnels/ MPLS
 *   tunnels).
 *   This configuration only take effect the entry type is not ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_OTHER and 
 *    ARAD_PP_EG_ENCAP_ACCESS_PREFIX_TYPE_NONE.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               lif_eep_ndx -
 *     Lif eep index.
 * REMARKS:
 *   - T20E: not supported. Error will be returned if called.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_entry_init(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               lif_eep_ndx
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_range_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets devision of the Egress Encapsulation Table between
 *   the different usages (Link layer/ IP tunnels/ MPLS
 *   tunnels).
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_RANGE_INFO                 *range_info -
 *     Range information.
 * REMARKS:
 *   - T20E: not supported. Error will be returned if called.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_range_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_RANGE_INFO                 *range_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_eg_encap_range_info_set" API.
 *     Refer to "soc_ppd_eg_encap_range_info_set" API for details.
*********************************************************************/
uint32
  soc_ppd_eg_encap_range_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_RANGE_INFO                 *range_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_null_lif_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set LIF Editing entry to be NULL Entry.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                lif_eep_ndx -
 *     Egress Encapsulation table entry, of type Out-LIF (First
 *     encapsulation)
 *   SOC_SAND_IN  uint32                                next_eep -
 *     Pointer to the next egress encapsulation table entry
 * REMARKS:
 *   - T20E Only API- Needed when the packet should only be
 *   encapsulated with Tunnel information and the EEI is NULL
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_null_lif_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  uint32                                next_eep
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_next_eep_update
 * TYPE:
 *   PROC
 * FUNCTION:
 *   update only the next-outlif.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                lif_eep_ndx -
 *     Egress Encapsulation table entry, of type Out-LIF (First
 *     encapsulation)
 *   SOC_SAND_IN  uint32                                next_outlif -
 *     Pointer to the next egress encapsulation table entry
 *   flags: see SOC_PPD_EG_ENCAP_ENTRY_UPDATE
 * REMARKS:
 *   - T20E Only API- Needed when the packet should only be
 *   encapsulated with Tunnel information and the EEI is NULL
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_next_outlif_update(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  uint32                                flags,
    SOC_SAND_IN  uint32                                next_outlif
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_field_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   get one field of out-lif info
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                lif_eep_ndx -
 *     Egress Encapsulation table entry, of type Out-LIF (First
 *     encapsulation)
 *   SOC_SAND_IN  uint32                                val -
 *     Pointer to the next egress encapsulation table entry
 *   flags: see SOC_PPD_EG_ENCAP_ENTRY_UPDATE
 * REMARKS:
 *   - T20E Only API- Needed when the packet should only be
 *   encapsulated with Tunnel information and the EEI is NULL
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_lif_field_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  uint32                                flags,
    SOC_SAND_OUT  uint32                               *val
  );


/*********************************************************************
*     Init CUD global extension.
*********************************************************************/
uint32
  soc_ppd_eg_encap_data_local_to_global_cud_init(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Set CUD global extension.
*********************************************************************/
uint32
  soc_ppd_eg_encap_data_local_to_global_cud_set(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  in_cud,
    SOC_SAND_IN  uint32                                  out_cud
  );

/*********************************************************************
*     Set CUD global extension.
*********************************************************************/
uint32
  soc_ppd_eg_encap_data_local_to_global_cud_get(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  in_cud,
    SOC_SAND_OUT uint32*                                 out_cud
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_data_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set LIF Editing entry to hold DATA entry.
 *   Needed on some cases, where further programming is needed.
 *   For example: IPV6 tunnels.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  lif_eep_ndx -
 *     Egress Encapsulation table entry, of type Out-LIF.
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_DATA_INFO              *data_info -
 *     Data entry information.
 *   SOC_SAND_IN  uint8                                  next_eep_valid,
 *       Is next_eep valid. 
 *   SOC_SAND_IN  uint32                                  next_eep -
 *     Pointer to Egress Encapsulation table entry.
 * REMARKS:
 *  
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_data_lif_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_DATA_INFO                 *data_info,
    SOC_SAND_IN  uint8                                   next_eep_valid,
    SOC_SAND_IN  uint32                                  next_eep
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_ac_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set LIF Editing entry to hold AC ID. Actually maps from
 *   CUD to AC.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                lif_eep_ndx -
 *     Egress Encapsulation table entry, of type Out-LIF (First
 *     encapsulation)
 *   SOC_SAND_IN  SOC_PPD_AC_ID                               ac_id -
 *     The Out-AC ID, the packet is associated with
 * REMARKS:
 *   - In Soc_petra-B: mapping has to maintain the encoding of
 *   the AC in the CUD.- Use this API to MAP CUD (from the TM
 *   header) to AC.- Mapping - Use LIF Editing Table.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_ac_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_AC_ID                               ac_id
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_swap_command_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set LIF Editing entry to hold MPLS LSR SWAP label.
 *   Needed for MPLS multicast services.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                lif_eep_ndx -
 *     Egress Encapsulation table entry, of type Out-LIF (First
 *     encapsulation)
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_SWAP_INFO                  *swap_info -
 *     Swap label for LSR applications (typically for
 *     Multicast).
 *   SOC_SAND_IN  uint32                                next_eep -
 *     Pointer to Egress Encapsulation table entry of type
 *     tunnel (second encapsulation), or link-layer
 * REMARKS:
 *   - In T20E, next_eep always points to Tunnels Editing
 *   Table.- The usage of this API for Multicast is to give a
 *   unique encapsulation for each copy.- If the user needs
 *   SWAP for Unicast packets, then the swap can be done in
 *   the ingress using the EEI.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_swap_command_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_SWAP_INFO                  *swap_info,
    SOC_SAND_IN  uint32                                next_eep
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_pwe_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set LIF Editing entry to hold PWE info (VC label and
 *   push profile).
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                lif_eep_ndx -
 *     Egress Encapsulation table entry, of type Out-LIF (First
 *     encapsulation)
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_PWE_INFO                   *pwe_info -
 *     PWE info VC label and push profile.
 *   SOC_SAND_IN  uint32                                next_eep -
 *     Pointer to the next egress encapsulation table entry
 * REMARKS:
 *   - In T20E: next_eep always points to Tunnels Editing
 *   Table.- The usage of this API for Multicast is to give
 *   special treatment for each copy.- In some applications,
 *   such as path protection in VPWS, the VC label can be set
 *   explicitely in the EEI in the ingress PP device, in
 *   which case the 1st encapsulation table is skipped.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_pwe_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PWE_INFO                   *pwe_info,
    SOC_SAND_IN  uint32                                next_eep
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_pop_command_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set LIF Editing entry to hold MPLS LSR POP command.
 *   Needed for MPLS multicast services.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                lif_eep_ndx -
 *     Egress Encapsulation table entry, of type Out-LIF (First
 *     encapsulation)
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_POP_INFO                   *pop_info -
 *     POP information including type of POP, and information
 *     to process the inner Ethernet in case the POP is into
 *     Ethernet.
 *   SOC_SAND_IN  uint32                                next_eep -
 *     Pointer to the next egress encapsulation table entry
 * REMARKS:
 *   - In T20E: next_eep always points to Tunnels Editing
 *   Table.- The usage of this API for Multicast is to give
 *   special treatment for each copy.- If the user needs to
 *   add a POP label for Unicast packet, then the POP command
 *   can be set in the EEI in the ingress.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_pop_command_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_POP_INFO                   *pop_info,
    SOC_SAND_IN  uint32                                next_eep
  );

/*********************************************************************
* NAME: 
 *   soc_ppd_eg_encap_vsi_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add RIF encapsulation entry to the Editing Table.
 *   ARAD only.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                lif_eep_ndx -
 *     Egress Encapsulation table entry, of type Out-LIF (First
 *     encapsulation)
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_VSI_ENCAP_INFO             vsi_info -
 *      RIF (vsi) encapsulation information
 *   SOC_SAND_IN  uint8                                  next_eep_valid,
 *       Is next_eep valid. 
 *   SOC_SAND_IN  uint32                                next_eep -
 *     Pointer to the next egress encapsulation table entry
 * REMARKS:
 *  ARAD only.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_vsi_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN SOC_PPD_EG_ENCAP_VSI_ENCAP_INFO             *vsi_info, 
    SOC_SAND_IN uint8                                  next_eep_valid, 
    SOC_SAND_IN  uint32                                  next_eep
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_mpls_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add MPLS tunnels encapsulation entry to the Tunnels
 *   Editing Table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                tunnel_eep_ndx -
 *     Egress Encapsulation table entry, of type Tunnel (Second
 *     encapsulation)
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO            *mpls_encap_info -
 *     MPLS encapsulation information.
 *   SOC_SAND_IN  uint32                                ll_eep -
 *     Points to next encapsulation (LL).
 * REMARKS:
 *   - In order to set entry with no encapsulation set
 *   'nof_tunnels' at 'mpls_encap_info' to zero.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_mpls_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO            *mpls_encap_info,
    SOC_SAND_IN  uint32                                ll_eep
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_ipv4_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add IPv4 tunnels encapsulation entry to the Egress
 *   Encapsulation Tunnels Editing Table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                tunnel_eep_ndx -
 *     Egress Encapsulation table entry, of type Tunnel (Second
 *     encapsulation)
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO            *ipv4_encap_info -
 *     IPv4 encapsulation information.
 *   SOC_SAND_IN  uint32                                ll_eep -
 *     Points to next encapsulation (LL). IP tunnel must point
 *     to a link-layer encapsulation entry and a link layer.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_ipv4_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO            *ipv4_encap_info,
    SOC_SAND_IN  uint32                                ll_eep
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_ipv6_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add IPV6 tunnels encapsulation entry to the Egress
 *   Encapsulation Tunnels Editing Table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                tunnel_eep_ndx -
 *     Egress Encapsulation table entry, of type Tunnel (Second
 *     encapsulation)
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO            *ipv6_encap_info -
 *     IPV6 encapsulation information.
 *   SOC_SAND_IN  uint32                                ll_eep -
 *     Points to next encapsulation (LL). IP tunnel must point
 *     to a link-layer encapsulation entry and a link layer.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_ipv6_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO            *ipv6_encap_info,
    SOC_SAND_IN  uint32                                ll_eep
  );



/********************************************************************* 
* NAME:
 *   soc_ppd_eg_encap_overlay_arp_data_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add overlay arp encapsulation entry to the Egress 
 *   Encapsulation Tunnels Editing Table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                overlay_ll_eep_ndx -
 *     Egress Encapsulation table entry, of type LL 
 *   SOC_SAND_INOUT  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO            *ll_encap_info -
 *     overlay arp encapsulation information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_overlay_arp_data_entry_add(
     SOC_SAND_IN  int                                    unit,
     SOC_SAND_IN  uint32                                 overlay_ll_eep_ndx, 
     SOC_SAND_INOUT  SOC_PPC_EG_ENCAP_OVERLAY_ARP_ENCAP_INFO *ll_encap_info
     );


/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_mirror_entry_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add Mirror encapsulation entry to the Egress
 *   Encapsulation Tunnels Editing Table.
 * INPUT:
 *   SOC_SAND_IN  int                                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      tunnel_eep_ndx -
 *     Egress Encapsulation table entry 0-15. each mirror entry occupies 2 entries:
 *       1. 224 + tunnel_eep_ndx
 *       2. 224 + tunnel_eep_ndx + 16
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO  *mirror_encap_info -
 *     Mirror encapsulation information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_mirror_entry_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              mirror_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO *mirror_encap_info
  );


/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_mirror_entry_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add Mirror encapsulation entry to the Egress
 *   Encapsulation Tunnels Editing Table.
 * INPUT:
 *   SOC_SAND_IN  int                                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      mirror_ndx -
 *     Egress Encapsulation table entry 0-15. each mirror entry occupies 2 entries:
 *       1. mirror_base + mirror_ndx
 *       2. mirror_base + mirror_ndx + 16
 *   SOC_SAND_OUT  SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO  *mirror_encap_info -
 *     Mirror encapsulation information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_mirror_entry_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              mirror_ndx,
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO *mirror_encap_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_port_erspan_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   disable/enable ERSPAN per port,
 *   port can be used for ERSPAN or RSPAN/SPAN but not both .
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                local_port_ndx -
 *     pp port 
 *   SOC_SAND_IN  uint8                                  is_erspan -
 *     whether port is used for erspan (1) or not (0)
 * REMARKS:
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_port_erspan_set(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPD_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  is_erspan
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_port_erspan_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   get whether ERSPAN is disabled/enabled per port,
 *   port can be used for ERSPAN or RSPAN/SPAN but not both .
 * INPUT:
 *   SOC_SAND_IN  int                                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                local_port_ndx -
 *     pp port 
 *   SOC_SAND_OUT  uint8                                  is_erspan -
 *     1 - port is used for ERSPAN. 0 - is not.
 * REMARKS:
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_port_erspan_get(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPD_PORT                           local_port_ndx,
    SOC_SAND_OUT uint8                                  *is_erspan
  );

/*********************************************************************
* NAME:
 *   arad_pp_eg_encap_ll_entry_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add LL encapsulation entry.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  ll_eep_ndx -
 *     Points to LL table.
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_LL_INFO                    *ll_encap_info -
 *     LL encapsulation information, including DA-MAC.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_ll_entry_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                ll_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_LL_INFO                    *ll_encap_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_entry_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove entry from the encapsulation Table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_EEP_TYPE                   eep_type_ndx -
 *     Encapsulation Table to select (LIFs/Tunnels/LL).
 *   SOC_SAND_IN  uint32                                eep_ndx -
 *     Points into Editing Table according to 'eep_type_ndx'.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_entry_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                eep_ndx
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_entry_type_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get entry type from the Egress encapsulation
 *   tables.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  eep_ndx -
 *     Points into EEDB Table.
 *   SOC_SAND_OUT  SOC_PPD_EG_ENCAP_ENTRY_TYPE            entry_type -
 *     Type of the encapsulation entry. 
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_entry_type_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 eep_ndx,
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_ENTRY_TYPE                *entry_type
  );


/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_entry_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get entry information from the Egress encapsulation
 *   tables.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_EEP_TYPE                   eep_type_ndx -
 *     Encapsulation Table to select (LIFs/Tunnels/LL).
 *   SOC_SAND_IN  uint32                                eep_ndx -
 *     Points into Editing Table according to 'eep_type_ndx'.
 *   SOC_SAND_IN  uint32                                depth -
 *     Number of encapsulation entries to get, see example of
 *     use in remarks.
 *   SOC_SAND_OUT SOC_PPD_EG_ENCAP_ENTRY_INFO                 encap_entry_info -
 *     SOC_PPD_NOF_EG_ENCAP_EEP_TYPES]- To include egress
 *     encapsulation entries.
 *   SOC_SAND_OUT uint32                                next_eep -
 *     SOC_PPD_NOF_EG_ENCAP_EEP_TYPES]- To include next Egress
 *     encapsulation pointer.
 *   SOC_SAND_OUT uint32                                *nof_entries -
 *     Number of valid entries in next_eep and
 *     encap_entry_info.
 * REMARKS:
 *   - To get all egress encapsulations performed for a given
 *   out-lif, set eep_type_ndx =
 *   SOC_PPD_EG_ENCAP_EEP_TYPE_LIF_EEP, eep_ndx = out-lif, depth
 *   = SOC_PPD_NOF_EG_ENCAP_EEP_TYPES.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_entry_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                eep_ndx,
    SOC_SAND_IN  uint32                                depth,
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_ENTRY_INFO                 encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES],
    SOC_SAND_OUT uint32                                next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES],
    SOC_SAND_OUT uint32                                *nof_entries
  );


/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_entry_data_info_to_overlay_arp_encap_info
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Parse data info to get overlay arp encap info.
 *   
 *   tables.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PPD_EG_ENCAP_ENTRY_INFO                 encap_entry_info -
 *     - To include egress encapsulation entries.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
soc_ppd_eg_encap_entry_data_info_to_overlay_arp_encap_info(
   SOC_SAND_IN  int                               unit,
   SOC_SAND_OUT SOC_PPD_EG_ENCAP_ENTRY_INFO                 *encap_entry_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_push_profile_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Setting the push profile info - specifying how to build
 *   the label header.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                profile_ndx -
 *     Profile ID. Range: 0 - 7.
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info -
 *     Push profile information including TTL, EXP, has-CW, and
 *     if it is PIPE mode.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_push_profile_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_eg_encap_push_profile_info_set" API.
 *     Refer to "soc_ppd_eg_encap_push_profile_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_eg_encap_push_profile_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_push_exp_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the EXP value of the pushed label as mapping of the
 *   TC and DP.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY               *exp_key -
 *     The key (set of parameters) that is used to set the EXP.
 *   SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                        exp -
 *     EXP to put in the pushed label.
 * REMARKS:
 *   - T20E only. Error is returned if called for Soc_petra-B.-
 *   Relevant when exp_mark_mode in push profile entry set to
 *   map EXP value from TC and DP
 *   (SOC_PPD_EG_ENCAP_EXP_MARK_MODE_FROM_PUSH_PROFILE)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_push_exp_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                        exp
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_eg_encap_push_exp_info_set" API.
 *     Refer to "soc_ppd_eg_encap_push_exp_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_eg_encap_push_exp_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_EXP                        *exp
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_pwe_glbl_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Global information for PWE Encapsulation.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO              *glbl_info -
 *     Global in
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_pwe_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_eg_encap_pwe_glbl_info_set" API.
 *     Refer to "soc_ppd_eg_encap_pwe_glbl_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_eg_encap_pwe_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_glbl_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Global information for Encapsulation.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_EG_ENCAP_GLBL_INFO              *glbl_info -
 *     Global in
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_GLBL_INFO              *glbl_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_eg_encap_glbl_info_set" API.
 *     Refer to "soc_ppd_eg_encap_glbl_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_eg_encap_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_GLBL_INFO              *glbl_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set source IP address for IPv4 Tunneling.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                entry_ndx -
 *     Entry pointed by IPv4 Tunnel - see
 *     soc_ppd_eg_encap_ipv4_encap_entry_add(). Range: 0 - 15.
 *   SOC_SAND_IN  uint32                                src_ip -
 *     Source IP address.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  uint32                                src_ip
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set" API.
 *     Refer to "soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set" API
 *     for details.
*********************************************************************/
uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_OUT uint32                                *src_ip
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set TTL for IPv4 Tunneling.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                entry_ndx -
 *     Entry pointed by IPv4 Tunnel. See
 *     soc_ppd_eg_encap_ipv4_encap_entry_add(). Range: 0 - 3.
 *   SOC_SAND_IN  SOC_SAND_PP_IP_TTL                          ttl -
 *     TTL value to set in the IPV4 tunnel header. Range: 0 -
 *     63.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                          ttl
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set" API.
 *     Refer to "soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                          *ttl
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set TOS for IPv4 Tunneling.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                entry_ndx -
 *     Entry pointed by IPv4 Tunnel. See
 *     soc_ppd_eg_encap_ipv4_encap_entry_add(). Range: 0 - 15.
 *   SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                        tos -
 *     TOS value to set in the IPV4 tunnel header. Range: 0 -
 *     255.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                        tos
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set" API.
 *     Refer to "soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_TOS                        *tos
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set MPLS PIPE mode to do copy EXP (1) or set EXP (0).
 * INPUT:
 *   SOC_SAND_IN  int                                  unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                                is_exp_copy -
 *     Pipe mode - do copy EXP (1) or set EXP (0).
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_set(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint8                                is_exp_copy
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_set" API.
 *     Refer to "soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_set" API for
 *     details.
*********************************************************************/
uint32
  soc_ppd_eg_encap_mpls_pipe_mode_is_exp_copy_get(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_OUT uint8                                *is_exp_copy
  );

/*********************************************************************
* NAME:
 *   soc_ppd_eg_encap_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_ppd_api_eg_encap module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_eg_encap_get_procs_ptr(void);
void
  SOC_PPD_EG_ENCAP_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_RANGE_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_SWAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_SWAP_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_PWE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_PWE_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_POP_INTO_ETH_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_POP_INTO_ETH_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_POP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_POP_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_VSI_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_VSI_ENCAP_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_MPLS_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_MPLS_TUNNEL_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_IPV4_TUNNEL_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_IPV6_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_IPV6_TUNNEL_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_MIRROR_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_MIRROR_TUNNEL_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_LL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_LL_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_PUSH_EXP_KEY *info
  );

void
  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_PWE_GLBL_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_GLBL_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_ENTRY_VALUE_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_ENTRY_VALUE *info
  );

void
  SOC_PPD_EG_ENCAP_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_ENTRY_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_DATA_INFO_clear(
    SOC_SAND_OUT SOC_PPD_EG_ENCAP_DATA_INFO *info
  );


#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_EG_ENCAP_EEP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_EEP_TYPE enum_val
  );

const char*
  SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_to_string(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE enum_val
  );

const char*
  SOC_PPD_EG_ENCAP_EXP_MARK_MODE_to_string(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_EXP_MARK_MODE enum_val
  );

const char*
  SOC_PPD_EG_ENCAP_ENTRY_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_ENTRY_TYPE enum_val
  );

void
  SOC_PPD_EG_ENCAP_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_RANGE_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_SWAP_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_SWAP_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_PWE_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PWE_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_POP_INTO_ETH_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_POP_INTO_ETH_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_POP_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_POP_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_MPLS_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_MPLS_TUNNEL_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV4_TUNNEL_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_IPV6_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV6_TUNNEL_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_MIRROR_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_MIRROR_TUNNEL_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_LL_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_LL_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY *info
  );

void
  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_GLBL_INFO *info
  );

void
  SOC_PPD_EG_ENCAP_ENTRY_VALUE_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_ENTRY_VALUE *info
  );

void
  SOC_PPD_EG_ENCAP_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_ENTRY_INFO *info
  );

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_eg_encap_range_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_RANGE_INFO                 *range_info
  );

void
  soc_ppd_eg_encap_range_info_get_print(
    SOC_SAND_IN  int                               unit
  );

void
  soc_ppd_eg_encap_null_lif_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  uint32                                next_eep
  );

void
  soc_ppd_eg_encap_ac_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_AC_ID                               ac_id
  );

void
  soc_ppd_eg_encap_swap_command_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_SWAP_INFO                  *swap_info,
    SOC_SAND_IN  uint32                                next_eep
  );

void
  soc_ppd_eg_encap_pwe_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PWE_INFO                   *pwe_info,
    SOC_SAND_IN  uint32                                next_eep
  );

void
  soc_ppd_eg_encap_pop_command_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                lif_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_POP_INFO                   *pop_info,
    SOC_SAND_IN  uint32                                next_eep
  );

void
  soc_ppd_eg_encap_vsi_entry_add_print(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN SOC_PPD_EG_ENCAP_VSI_ENCAP_INFO             *vsi_info, 
    SOC_SAND_IN uint8                                  next_eep_valid, 
    SOC_SAND_IN  uint32                                  next_eep
  );

void
  soc_ppd_eg_encap_mpls_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_MPLS_ENCAP_INFO            *mpls_encap_info,
    SOC_SAND_IN  uint32                                ll_eep
  );

void
  soc_ppd_eg_encap_ipv4_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV4_ENCAP_INFO            *ipv4_encap_info,
    SOC_SAND_IN  uint32                                ll_eep
  );

void
  soc_ppd_eg_encap_ipv6_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_IPV6_ENCAP_INFO            *ipv6_encap_info,
    SOC_SAND_IN  uint32                                ll_eep
  );

void
  soc_ppd_eg_encap_mirror_entry_set_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO *mirror_encap_info
  );

void
  soc_ppd_eg_encap_ll_entry_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                ll_eep_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_LL_INFO                    *ll_encap_info,
  );

void
  soc_ppd_eg_encap_entry_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                eep_ndx
  );

void
  soc_ppd_eg_encap_entry_type_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                eep_ndx
  );

void
  soc_ppd_eg_encap_entry_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                eep_ndx,
    SOC_SAND_IN  uint32                                depth
  );

void
  soc_ppd_eg_encap_push_profile_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info
  );

void
  soc_ppd_eg_encap_push_profile_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                profile_ndx
  );

void
  soc_ppd_eg_encap_push_exp_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                        exp
  );

void
  soc_ppd_eg_encap_push_exp_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PUSH_EXP_KEY               *exp_key
  );

void
  soc_ppd_eg_encap_pwe_glbl_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  );

void
  soc_ppd_eg_encap_pwe_glbl_info_get_print(
    SOC_SAND_IN  int                               unit
  );

void
  soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  uint32                                src_ip
  );

void
  soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx
  );

void
  soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                          ttl
  );

void
  soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx
  );

void
  soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                        tos
  );

void
  soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                entry_ndx
  );

#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPD_API_EG_ENCAP_INCLUDED__*/
#endif

