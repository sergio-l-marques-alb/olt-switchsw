/* $Id$
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_trap_mgmt.h
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

#ifndef __SOC_PB_PP_TRAP_MGMT_INCLUDED__
/* { */
#define __SOC_PB_PP_TRAP_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_trap_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

#include <soc/dpp/Petra/petra_chip_defines.h>
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

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_SET = SOC_PB_PP_PROC_DESC_BASE_TRAP_MGMT_FIRST,
  SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_SET_PRINT,
  SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_SET_UNSAFE,
  SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_SET_VERIFY,
  SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_GET,
  SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_GET_PRINT,
  SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_GET_VERIFY,
  SOC_PB_PP_TRAP_FRWRD_PROFILE_INFO_GET_UNSAFE,
  SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_SET,
  SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_SET_PRINT,
  SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_SET_UNSAFE,
  SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_SET_VERIFY,
  SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_GET,
  SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_GET_PRINT,
  SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_GET_VERIFY,
  SOC_PB_PP_TRAP_SNOOP_PROFILE_INFO_GET_UNSAFE,
  SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_SET,
  SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_SET_PRINT,
  SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_SET_UNSAFE,
  SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_SET_VERIFY,
  SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_GET,
  SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_GET_PRINT,
  SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_GET_VERIFY,
  SOC_PB_PP_TRAP_TO_EG_ACTION_MAP_GET_UNSAFE,
  SOC_PB_PP_TRAP_EG_PROFILE_INFO_SET,
  SOC_PB_PP_TRAP_EG_PROFILE_INFO_SET_PRINT,
  SOC_PB_PP_TRAP_EG_PROFILE_INFO_SET_UNSAFE,
  SOC_PB_PP_TRAP_EG_PROFILE_INFO_SET_VERIFY,
  SOC_PB_PP_TRAP_EG_PROFILE_INFO_GET,
  SOC_PB_PP_TRAP_EG_PROFILE_INFO_GET_PRINT,
  SOC_PB_PP_TRAP_EG_PROFILE_INFO_GET_VERIFY,
  SOC_PB_PP_TRAP_EG_PROFILE_INFO_GET_UNSAFE,
  SOC_PB_PP_TRAP_MACT_EVENT_GET,
  SOC_PB_PP_TRAP_MACT_EVENT_GET_PRINT,
  SOC_PB_PP_TRAP_MACT_EVENT_GET_UNSAFE,
  SOC_PB_PP_TRAP_MACT_EVENT_GET_VERIFY,
  SOC_PB_PP_TRAP_MACT_EVENT_PARSE,
  SOC_PB_PP_TRAP_MACT_EVENT_PARSE_PRINT,
  SOC_PB_PP_TRAP_MACT_EVENT_PARSE_UNSAFE,
  SOC_PB_PP_TRAP_MACT_EVENT_PARSE_VERIFY,
  SOC_PB_PP_TRAP_MGMT_GET_PROCS_PTR,
  SOC_PB_PP_TRAP_MGMT_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_TRAP_MGMT_TRAP_CODE_TO_INTERNAL,
  SOC_PB_PP_TRAP_CPU_TRAP_CODE_FROM_INTERNAL_UNSAFE,
  SOC_PB_PP_TRAP_PACKET_PARSE_INFO_GET_UNSAFE,
  SOC_PB_PP_TRAP_SB_TO_TRAP_CODE_MAP_GET,
  SOC_PB_PP_TRAP_SB_VTT_TO_INTERNAL_TRAP_CODE_MAP_GET,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_TRAP_MGMT_PROCEDURE_DESC_LAST
} SOC_PB_PP_TRAP_MGMT_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NDX_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_TRAP_MGMT_FIRST,
  SOC_PB_PP_TRAP_MGMT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_EG_ACTION_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_PROFILE_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_BUFF_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_BUFF_LEN_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_VSI_SHIFT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_COUNTER_SELECT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_COUNTER_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_METER_SELECT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_METER_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_METER_COMMAND_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_ETHERNET_POLICE_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_FRWRD_OFFSET_INDEX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_STRENGTH_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_BITMAP_MASK_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_SIZE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_SAMPLING_PROBABILITY_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_SNOOP_CMND_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_CUD_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_CPU_TRAP_CODE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_CPU_TRAP_QUALIFIER_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_SRC_SYS_PORT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_LL_HEADER_PTR_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_TRAP_MGMT_TRAP_CODE_NOT_SUPPORTED_ERR,
  SOC_PB_PP_TRAP_MGMT_SNP_STRENGTH_OUT_OF_RANGE_ERR,
  SOC_PB_PP_TRAP_MGMT_MORE_THAN_ONE_BIT_ENABLED_ERR,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_TRAP_MGMT_ERR_LAST
} SOC_PB_PP_TRAP_MGMT_ERR;

/*  Prefixes */
#define SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX  (0x0<<4)
#define SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX  (0x1<<4)
#define SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX  (0x2<<4)
#define SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX  (0x3<<4)
#define SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX  (0x4<<4)
#define SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX  (0x5<<4)
#define SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX  (0x6<<4)
#define SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX  (0x7<<4)
#define SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX  (0x8<<4)
#define SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX  (0x9<<4)
#define SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX  (0xa<<4)
#define SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_PMF_PREFIX   (0xb<<4)
#define SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX   (0xc<<4)
#define SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_TIMNA_PREFIX (0xd<<4)
#define SOC_PB_PP_TRAP_CODE_INTERNAL_USER_PREFIX      (0xf<<4)

typedef enum
{
  /*  Link-Layer Cpu-Trap-Codes */
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP0                         = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x0,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP1                         = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x1,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP2                         = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x2,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_SA_DROP3                         = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x3,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_TE_TRANSPLANT                    = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x4,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_TE_UNKNOWN_TUNNEL                = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x5,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_TRANSPLANT                       = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x6,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PBP_LEARN_SNOOP                      = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x7,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_SA_AUTHENTICATION_FAILED             = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x8,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_PORT_NOT_PERMITTED                   = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0x9,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_UNEXPECTED_VID                       = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0xa,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_SA_MULTICAST                         = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0xb,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_SA_EQUALS_DA                         = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0xc,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_8021X                                = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0xd,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE0               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0xe,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE1               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR0_PREFIX + 0xf,

  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE2               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x0,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE3               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x1,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_MY_ARP                               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x2,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ARP                                  = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x3,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_IGMP_MEMBERSHIP_QUERY                = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x4,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_IGMP_REPORT_LEAVE_MSG                = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x5,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_IGMP_UNDEFINED                       = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x6,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_MC_LISTENER_QUERY         = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x7,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_RESERVED_MC_PREFIX                   = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR1_PREFIX + 0x8, /*  prefix */

  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_REPORT_DONE_MSG           = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x0,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_ICMPV6_MLD_UNDEFINED                 = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x1,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_DHCP_SERVER                          = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x2,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_DHCP_CLIENT                          = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x3,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_DHCPV6_SERVER                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x4,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_DHCPV6_CLIENT                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x5,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP0                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x6,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP1                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x7,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP2                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x8,
  SOC_PB_PP_TRAP_CODE_INTERNAL_LLR_GENERAL_TRAP3                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0x9,

  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_0_TO_5               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_LLR2_PREFIX + 0xa, /* SW prefix */

  /*  VTT Cpu-Trap-Codes */
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_PORT_NOT_VLAN_MEMBER                 = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x0,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR                      = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x1,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_HEADER_SIZE_ERR_O_MPLS               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x2,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_B_MAC_AND_LEARN_NULL              = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x3,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_B_DA_UNKNOWN_I_SID                = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x4,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_IP_DISABLE                = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x5,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_MPLS_DISABLE              = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x6,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_ARP                       = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x7,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MY_MAC_AND_UNKNOWN_L3                = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x8,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IP_COMP_MC_INVALID_IP                = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0x9,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_VERSION                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0xa,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_INVALID_TTL                    = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0xb,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_CHBH                           = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0xc,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_NO_REVERSE_FEC                 = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0xd,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_CITE                           = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0xe,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_TRILL_ILLEGAL_INNER_MC               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT0_PREFIX + 0xf,

  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE0                    = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x0,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE1                    = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x1,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE2                    = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x2,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LABEL_VALUE3                    = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x3,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_NO_RESOURCES                    = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x4,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LABEL_IN_RANGE          = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x5,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_INVALID_LABEL_IN_SEM            = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x6,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_LSP_BOS                         = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x7,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_PWE_NO_BOS_LABEL_14             = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x8,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_PWE_NO_BOS                      = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0x9,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_VRF_NO_BOS                      = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0xa,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_TTL_0                           = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0xb,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_TRAP               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0xc,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_MPLS_CONTROL_WORD_DROP               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0xd,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_STP_STATE_BLOCK                      = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0xe,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_STP_STATE_LEARN                      = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT1_PREFIX + 0xf,

  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_VERSION_ERROR                      = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x0,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_CHECKSUM_ERROR                     = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x1,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_HEADER_LENGTH_ERROR                = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x2,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TOTAL_LENGTH_ERROR                 = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x3,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TTL0                               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x4,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_HAS_OPTIONS                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x5,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TTL1                               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x6,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_SIP_EQUAL_DIP                      = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x7,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_DIP_ZERO                           = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x8,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_SIP_IS_MC                          = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0x9,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED  = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0xa,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_CFM_ACCELERATED_INGRESS                 = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0xb,
  SOC_PB_PP_TRAP_CODE_INTERNAL_VTT_ILLEGEL_PFC                             = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0xc,

  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_6_TO_7               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_VTT2_PREFIX + 0xd, /* SW prefix */

  /*  Forwarding-Lookup Cpu-Trap-Codes */
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_L2CP_PEER                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x0,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_L2CP_DROP                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x1,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_MEMBERSHIP_QUERY         = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x2,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_REPORT_LEAVE_MSG         = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x3,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_IGMP_UNDEFINED                = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x4,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY  = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x5,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_REPORT_DONE        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x6,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ETH_FL_ICMPV6_MLD_UNDEFINED          = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x7,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_VERSION_ERROR                   = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x8,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_CHECKSUM_ERROR                  = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0x9,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_HEADER_LENGTH_ERROR             = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0xa,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TOTAL_LENGTH_ERROR              = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0xb,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL0                            = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0xc,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_HAS_OPTIONS                     = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0xd,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_TTL1                            = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0xe,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_EQUAL_DIP                   = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP0_PREFIX + 0xf,

  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_DIP_ZERO                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x0,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV4_SIP_IS_MC                       = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x1,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_VERSION_ERROR                   = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x2,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT0                      = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x3,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_HOP_COUNT1                      = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x4,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_DESTINATION         = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x5,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOOPBACK_ADDRESS                = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x6,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_SOURCE                = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x7,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_NEXT_HEADER_NULL                = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x8,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_UNSPECIFIED_SOURCE              = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0x9,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_DESTINATION          = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0xa,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_DESTINATION          = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0xb,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_LINK_SOURCE               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0xc,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_LOCAL_SITE_SOURCE               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0xd,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_COMPATIBLE_DESTINATION     = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0xe,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_IPV4_MAPPED_DESTINATION         = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP1_PREFIX + 0xf,

  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_IPV6_MULTICAST_DESTINATION           = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x0,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_TTL0                            = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x1,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_TTL1                            = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x2,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_SN_FLAGS_ZERO                    = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x3,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_SN_ZERO_FLAGS_SET                = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x4,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_SYN_FIN                          = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x5,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_EQUAL_PORTS                      = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x6,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_FRAGMENT_INCOMPLETE_HEADER       = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x7,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TCP_FRAGMENT_OFFSET_LT8              = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x8,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_UDP_EQUAL_PORTS                      = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0x9,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ICMP_DATA_GT_576                     = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0xa,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ICMP_FRAGMENTED                      = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0xb,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP0                             = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0xc,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP1                             = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0xd,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP2                             = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0xe,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_DROP3                             = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP2_PREFIX + 0xf,

  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND0                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x0,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND1                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x1,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND2                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x2,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SA_NOT_FOUND3                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x3,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND0                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x4,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND1                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x5,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND2                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x6,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DA_NOT_FOUND3                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x7,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_UNKNOWN_DA_PREFIX                    = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0x8, /*  prefix */

  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_8_TO_10              = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP3_PREFIX + 0xc, /* SW prefix */

  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_ELK_ERROR                            = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x0,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_P2P_MISCONFIGURATION                 = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x1,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_SAME_INTERFACE                       = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x2,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_UC                     = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x3,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_TRILL_UNKNOWN_MC                     = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x4,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_UC_LOOSE_RPF_FAIL                    = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x5,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DEFAULT_UCV6                         = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x6,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_DEFAULT_MCV6                         = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x7,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_P2P_NO_BOS                      = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x8,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_CONTROL_WORD_TRAP               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0x9,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_CONTROL_WORD_DROP               = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0xa,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_UNKNOWN_LABEL                   = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0xb,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FLP_MPLS_P2P_MPLSX4                      = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0xc,

  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_11_TO_12             = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FLP4_PREFIX + 0xd, /* SW prefix */

  /*  PMF Cpu-Trap-Codes */
  SOC_PB_PP_TRAP_CODE_INTERNAL_PMF_GENERAL                              = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_PMF_PREFIX + 0x0,

  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_13_TO_27             = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_PMF_PREFIX + 0x1, /* SW prefix */

  /*  Fec-Resolution Cpu-Trap-Codes */
  SOC_PB_PP_TRAP_CODE_INTERNAL_FER_FACILITY_INVALID                     = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x0,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FER_FEC_ENTRY_ACCESSED                   = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x1,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FER_UC_STRICT_RPF_FAIL                   = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x2,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FER_MC_EXPLICIT_RPF_FAIL                 = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x3,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_AS_IS_RPF_FAIL            = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x4,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_RPF_FAIL                  = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x5,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FER_MC_USE_SIP_ECMP                      = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x6,
  SOC_PB_PP_TRAP_CODE_INTERNAL_FER_ICMP_REDIRECT                        = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x7,

  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_28_TO_35             = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_FER_PREFIX  + 0x8, /* SW prefix */

  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_36_TO_51             = SOC_PB_PP_TRAP_CODE_INTERNAL_IHP_TIMNA_PREFIX + 0x0, /* SW prefix */
  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_DEFINED_PREFIX_52_TO_60             = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_PREFIX + 0x7, /* SW prefix */


  /*  External device */
  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_OAMP                                = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_PREFIX     + 0x0,
  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_ETHERNET_OAM_ACCELERATED            = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_PREFIX     + 0x1,
  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_MPLS_OAM_ACCELERATED                = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_PREFIX     + 0x2,
  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_BFD_IP_OAM_TUNNEL_ACCELERATED       = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_PREFIX     + 0x3,
  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_BFD_PWE_OAM_ACCELERATED             = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_PREFIX     + 0x4,
  SOC_PB_PP_TRAP_CODE_INTERNAL_USER_ETHERNET_OAM_UP_ACCELERATED         = SOC_PB_PP_TRAP_CODE_INTERNAL_USER_PREFIX     + 0x5

} SOC_PB_PP_TRAP_CODE_INTERNAL;


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
  
uint32
  soc_pb_pp_trap_mgmt_trap_code_to_internal(
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                           trap_code_ndx,
    SOC_SAND_OUT SOC_PB_PP_TRAP_CODE_INTERNAL                  *trap_code_internal,
    SOC_SAND_OUT SOC_PETRA_REG_FIELD                           **strength_fld_fwd,
    SOC_SAND_OUT SOC_PETRA_REG_FIELD                           **strength_fld_snp
  );


uint32
  soc_pb_pp_trap_mgmt_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_trap_frwrd_profile_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set forwarding action profile information.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                           trap_code_ndx -
 *     Trap code. Soc_petraB range: 0-255. T20E range: 0-255, only
 *     enumarators mentioned as supported in T20e.user can use
 *     values not in the enum for used defined values.
 *   SOC_SAND_IN  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO      *profile_info -
 *     Information to set to the forwarding profile.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_trap_frwrd_profile_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_IN  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO          *profile_info
  );

uint32
  soc_pb_pp_trap_frwrd_profile_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_IN  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO          *profile_info
  );

uint32
  soc_pb_pp_trap_frwrd_profile_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                               trap_code_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_trap_frwrd_profile_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_trap_frwrd_profile_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_trap_frwrd_profile_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_OUT SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO          *profile_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_trap_snoop_profile_info_set_unsafe
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
  soc_pb_pp_trap_snoop_profile_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_IN  SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO          *profile_info
  );

uint32
  soc_pb_pp_trap_snoop_profile_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_IN  SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO          *profile_info
  );

uint32
  soc_pb_pp_trap_snoop_profile_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                               trap_code_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_trap_snoop_profile_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_trap_snoop_profile_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_trap_snoop_profile_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_OUT SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO          *profile_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_trap_to_eg_action_map_set_unsafe
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
  soc_pb_pp_trap_to_eg_action_map_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      trap_type_bitmap_ndx,
    SOC_SAND_IN  uint32                                      eg_action_profile
  );

uint32
  soc_pb_pp_trap_to_eg_action_map_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      trap_type_bitmap_ndx,
    SOC_SAND_IN  uint32                                      eg_action_profile
  );

uint32
  soc_pb_pp_trap_to_eg_action_map_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      trap_type_bitmap_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_trap_to_eg_action_map_set_unsafe" API.
 *     Refer to "soc_pb_pp_trap_to_eg_action_map_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_trap_to_eg_action_map_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      trap_type_bitmap_ndx,
    SOC_SAND_OUT uint32                                      *eg_action_profile
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_trap_eg_profile_info_set_unsafe
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
  soc_pb_pp_trap_eg_profile_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO             *profile_info
  );

uint32
  soc_pb_pp_trap_eg_profile_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO             *profile_info
  );

uint32
  soc_pb_pp_trap_eg_profile_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_trap_eg_profile_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_trap_eg_profile_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_trap_eg_profile_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO             *profile_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_trap_mact_event_get_unsafe
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
  soc_pb_pp_trap_mact_event_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT uint32                                      buff[SOC_PB_PP_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_OUT uint32                                      *buff_len
  );

uint32
  soc_pb_pp_trap_mact_event_get_verify(
    SOC_SAND_IN  int                                     unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_trap_mact_event_parse_unsafe
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
  soc_pb_pp_trap_mact_event_parse_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      buff[SOC_PB_PP_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_IN  uint32                                      buff_len,
    SOC_SAND_OUT SOC_PB_PP_TRAP_MACT_EVENT_INFO                    *mact_event
  );

uint32
  soc_pb_pp_trap_mact_event_parse_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      buff[SOC_PB_PP_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_IN  uint32                                      buff_len
  );

/*********************************************************************
* NAME:
*   soc_pb_pp_trap_packet_parse_info_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Fill information required to parse packet arriving to CPU.
* INPUT:
*   SOC_SAND_IN  int                                unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_PPC_TRAP_MGMT_PACKET_PARSE_INFO          *packet_parse_info-
*     Information required to parse packet arriving to CPU.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_trap_packet_parse_info_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT SOC_PPC_TRAP_MGMT_PACKET_PARSE_INFO  *packet_parse_info
    );

/*********************************************************************
* NAME:
*   soc_pb_pp_trap_cpu_trap_code_from_internal_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Convert CPU trap code as received on packet to API enumerator.
* INPUT:
*   SOC_SAND_IN  int                                unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint32                                internal_trap_code -
*     CPU trap code as received on packet.
*   SOC_SAND_IN  T20E_TRAP_CODE                           *trap_code-
*     Trap code enumerator.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_trap_cpu_trap_code_from_internal_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int32                                 internal_trap_code,
    SOC_SAND_OUT SOC_PB_PP_TRAP_CODE                          *trap_code
  );

/*********************************************************************
* NAME:
*   soc_pb_pp_trap_sb_to_trap_code_map_get
* TYPE:
*   PROC
* FUNCTION:
*   Convert CPU trap code Sticky bit packet to API enumerator.
* INPUT:
*   SOC_SAND_IN  int                                unit -
*     Identifier of the device to access.
*   uint32                   rng_num    -
*     which range of the sticky bit.
*   uint32                   sb_trap_indx    -
*     sticky bit trap index
*   SOC_SAND_IN  T20E_TRAP_CODE                           *trap_code-
*     Trap code enumerator.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_trap_sb_to_trap_code_map_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN   uint32                   rng_num,
    SOC_SAND_IN   uint32                   sb_trap_indx,
    SOC_SAND_OUT  SOC_PB_PP_TRAP_CODE           *trap_code
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_trap_mgmt_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_trap_mgmt module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_trap_mgmt_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_trap_mgmt_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_trap_mgmt module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_trap_mgmt_get_errs_ptr(void);

uint32
  SOC_PB_PP_TRAP_ACTION_PROFILE_DEST_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_DEST_INFO *info
  );

uint32
  SOC_PB_PP_TRAP_ACTION_PROFILE_COUNT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_COUNT_INFO *info
  );

uint32
  SOC_PB_PP_TRAP_ACTION_PROFILE_METER_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_METER_INFO *info
  );

uint32
  SOC_PB_PP_TRAP_ACTION_PROFILE_POLICE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_POLICE_INFO *info
  );

uint32
  SOC_PB_PP_TRAP_ACTION_PROFILE_COS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_COS_INFO *info
  );

uint32
  SOC_PB_PP_TRAP_ACTION_PROFILE_PROCESS_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_ACTION_PROFILE_PROCESS_INFO *info
  );

uint32
  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_FRWRD_ACTION_PROFILE_INFO *info
  );

uint32
  SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_SNOOP_ACTION_PROFILE_INFO *info
  );

uint32
  SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_EG_ACTION_PROFILE_INFO *info
  );

uint32
  SOC_PB_PP_TRAP_MACT_EVENT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_MACT_EVENT_INFO *info
  );

uint32
  SOC_PB_PP_TRAP_PACKET_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_TRAP_PACKET_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_TRAP_MGMT_INCLUDED__*/
#endif

