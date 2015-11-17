/* $Id: chip_sim_PB_PP.h,v 1.6 Broadcom SDK $
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
* FILENAME:       chip_sim_PB_PP_PP.h
*
* MODULE PREFIX:  chip_sim
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


#ifndef __CHIP_SIM_PB_PP_H_INCLUDED__
/* { */ 
#define __CHIP_SIM_PB_PP_H_INCLUDED__


/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h> 

#include "chip_sim.h"
#include "chip_sim_counter.h"
#include "chip_sim_interrupts.h"
#include "chip_sim_indirect.h"
#include "chip_sim_PB_TM.h"

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ID                                                       (SOC_PB_TBL_ID_LAST +   0)
#define SOC_PB_PP_IHP_PINFO_LLR_TBL_ID                                                                           (SOC_PB_TBL_ID_LAST +   1)
#define SOC_PB_PP_IHP_LLR_LLVP_TBL_ID                                                                            (SOC_PB_TBL_ID_LAST +   2)
#define SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ID                                                                   (SOC_PB_TBL_ID_LAST +   3)
#define SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ID                                                                     (SOC_PB_TBL_ID_LAST +   4)
#define SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ID                                                                       (SOC_PB_TBL_ID_LAST +   5)
#define SOC_PB_PP_IHP_TOS_2_COS_TBL_ID                                                                           (SOC_PB_TBL_ID_LAST +   6)
#define SOC_PB_PP_IHP_RESERVED_MC_TBL_ID                                                                         (SOC_PB_TBL_ID_LAST +   7)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ID                                                               (SOC_PB_TBL_ID_LAST +   8)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ID                                                      (SOC_PB_TBL_ID_LAST +   9)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ID                                                       (SOC_PB_TBL_ID_LAST +  10)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ID                                                       (SOC_PB_TBL_ID_LAST +  11)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ID                                                      (SOC_PB_TBL_ID_LAST +  12)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ID                                                  (SOC_PB_TBL_ID_LAST +  13)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ID                                                               (SOC_PB_TBL_ID_LAST +  14)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ID                                                         (SOC_PB_TBL_ID_LAST +  15)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ID                                                          (SOC_PB_TBL_ID_LAST +  16)
#define SOC_PB_PP_IHP_FLUSH_DB_TBL_ID                                                                            (SOC_PB_TBL_ID_LAST +  17)
#define SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ID                                                             (SOC_PB_TBL_ID_LAST +  18)
#define SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ID                                                     (SOC_PB_TBL_ID_LAST +  19)
#define SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ID                                                  (SOC_PB_TBL_ID_LAST +  20)
#define SOC_PB_PP_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ID                                                              (SOC_PB_TBL_ID_LAST +  21)
#define SOC_PB_PP_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ID                                                             (SOC_PB_TBL_ID_LAST +  22)
#define SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ID                                                           (SOC_PB_TBL_ID_LAST +  23)
#define SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ID                                                                        (SOC_PB_TBL_ID_LAST +  24)
#define SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ID                                                                      (SOC_PB_TBL_ID_LAST +  25)
#define SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ID                                                        (SOC_PB_TBL_ID_LAST +  26)
#define SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ID                                                         (SOC_PB_TBL_ID_LAST +  27)
#define SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ID                                                        (SOC_PB_TBL_ID_LAST +  28)
#define SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ID                                                (SOC_PB_TBL_ID_LAST +  29)
#define SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ID                                                 (SOC_PB_TBL_ID_LAST +  30)
#define SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ID                                                (SOC_PB_TBL_ID_LAST +  31)
#define SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ID                                                     (SOC_PB_TBL_ID_LAST +  32)
#define SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ID                                                      (SOC_PB_TBL_ID_LAST +  33)
#define SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ID                                                     (SOC_PB_TBL_ID_LAST +  34)
#define SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ID                                                                     (SOC_PB_TBL_ID_LAST +  35)
#define SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ID                                                                     (SOC_PB_TBL_ID_LAST +  36)
#define SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ID                                                                 (SOC_PB_TBL_ID_LAST +  37)
#define SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ID                                                      (SOC_PB_TBL_ID_LAST +  38)
#define SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ID                                                                (SOC_PB_TBL_ID_LAST +  39)
#define SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ID                                                                 (SOC_PB_TBL_ID_LAST +  40)
#define SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ID                                                       (SOC_PB_TBL_ID_LAST +  41)
#define SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ID                                                             (SOC_PB_TBL_ID_LAST +  42)
#define SOC_PB_PP_IHP_VSI_ISID_TBL_ID                                                                            (SOC_PB_TBL_ID_LAST +  43)
#define SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ID                                                                          (SOC_PB_TBL_ID_LAST +  44)
#define SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ID                                                                     (SOC_PB_TBL_ID_LAST +  45)
#define SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ID                                                                       (SOC_PB_TBL_ID_LAST +  46)
#define SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ID                                                                    (SOC_PB_TBL_ID_LAST +  47)
#define SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ID                                                                     (SOC_PB_TBL_ID_LAST +  48)
#define SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ID                                                                       (SOC_PB_TBL_ID_LAST +  49)
#define SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ID                                                                    (SOC_PB_TBL_ID_LAST +  50)
#define SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ID                                                                     (SOC_PB_TBL_ID_LAST +  51)
#define SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ID                                                        (SOC_PB_TBL_ID_LAST +  52)
#define SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ID                                                          (SOC_PB_TBL_ID_LAST +  53)
#define SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ID                                                               (SOC_PB_TBL_ID_LAST +  54)
#define SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ID                                                                 (SOC_PB_TBL_ID_LAST +  55)
#define SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ID                                                               (SOC_PB_TBL_ID_LAST +  56)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ID                                                                    (SOC_PB_TBL_ID_LAST +  57)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ID                                                       (SOC_PB_TBL_ID_LAST +  58)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ID                                                      (SOC_PB_TBL_ID_LAST +  59)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ID                                                      (SOC_PB_TBL_ID_LAST +  60)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ID                                                              (SOC_PB_TBL_ID_LAST +  61)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ID                                                           (SOC_PB_TBL_ID_LAST +  62)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ID                                                            (SOC_PB_TBL_ID_LAST +  63)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ID                                                              (SOC_PB_TBL_ID_LAST +  64)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ID                                                              (SOC_PB_TBL_ID_LAST +  65)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ID                                                      (SOC_PB_TBL_ID_LAST +  66)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ID                                                       (SOC_PB_TBL_ID_LAST +  67)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ID                                                          (SOC_PB_TBL_ID_LAST +  68)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ID                                                          (SOC_PB_TBL_ID_LAST +  69)
#define SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ID                                                               (SOC_PB_TBL_ID_LAST +  70)
#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ID                                                            (SOC_PB_TBL_ID_LAST +  71)
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ID                                                          (SOC_PB_TBL_ID_LAST +  72)
#define SOC_PB_PP_IHP_STP_TABLE_TBL_ID                                                                           (SOC_PB_TBL_ID_LAST +  73)
#define SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ID                                                                     (SOC_PB_TBL_ID_LAST +  74)
#define SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ID                                                        (SOC_PB_TBL_ID_LAST +  75)
#define SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ID                                                       (SOC_PB_TBL_ID_LAST +  76)
#define SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ID                                                            (SOC_PB_TBL_ID_LAST +  77)
#define SOC_PB_PP_IHP_VTT_LLVP_TBL_ID                                                                            (SOC_PB_TBL_ID_LAST +  78)
#define SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ID                                                                       (SOC_PB_TBL_ID_LAST +  79)
#define SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ID                                                            (SOC_PB_TBL_ID_LAST +  80)
#define SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ID                                                            (SOC_PB_TBL_ID_LAST +  81)
#define SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ID                                                             (SOC_PB_TBL_ID_LAST +  82)
#define SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ID                                                     (SOC_PB_TBL_ID_LAST +  83)
#define SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ID                                                               (SOC_PB_TBL_ID_LAST +  84)
#define SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ID                                                                       (SOC_PB_TBL_ID_LAST +  85)
#define SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ID                                                                 (SOC_PB_TBL_ID_LAST +  86)
#define SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ID                                                                 (SOC_PB_TBL_ID_LAST +  87)
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ID                                                                     (SOC_PB_TBL_ID_LAST +  88)
#define SOC_PB_PP_IHB_PINFO_FER_TBL_ID                                                                           (SOC_PB_TBL_ID_LAST +  89)
#define SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ID                                                                     (SOC_PB_TBL_ID_LAST +  90)
#define SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ID                                                                      (SOC_PB_TBL_ID_LAST +  91)
#define SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ID                                                               (SOC_PB_TBL_ID_LAST +  92)
#define SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ID                                                                     (SOC_PB_TBL_ID_LAST +  93)
#define SOC_PB_PP_IHB_FEC_ENTRY_TBL_ID                                                                           (SOC_PB_TBL_ID_LAST +  94)
#define SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ID                                                                   (SOC_PB_TBL_ID_LAST +  95)
#define SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ID                                                              (SOC_PB_TBL_ID_LAST +  96)
#define SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ID                                                                        (SOC_PB_TBL_ID_LAST +  97)
#define SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ID                                                                   (SOC_PB_TBL_ID_LAST +  98)
#define SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ID                                                                  (SOC_PB_TBL_ID_LAST +  99)
#define SOC_PB_PP_IHB_PATH_SELECT_TBL_ID                                                                         (SOC_PB_TBL_ID_LAST + 100)
#define SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ID                                                                  (SOC_PB_TBL_ID_LAST + 101)
#define SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ID                                                                     (SOC_PB_TBL_ID_LAST + 102)
#define SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ID                                                                        (SOC_PB_TBL_ID_LAST + 103)
#define SOC_PB_PP_IHB_PINFO_FLP_TBL_ID                                                                           (SOC_PB_TBL_ID_LAST + 104)
#define SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ID                                                             (SOC_PB_TBL_ID_LAST + 105)
#define SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ID                                                                 (SOC_PB_TBL_ID_LAST + 106)
#define SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ID                                                          (SOC_PB_TBL_ID_LAST + 107)
#define SOC_PB_PP_IHB_VRF_CONFIG_TBL_ID                                                                          (SOC_PB_TBL_ID_LAST + 108)
#define SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ID                                                                      (SOC_PB_TBL_ID_LAST + 109)
#define SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ID                                                                     (SOC_PB_TBL_ID_LAST + 110)
#define SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ID                                                                     (SOC_PB_TBL_ID_LAST + 111)
#define SOC_PB_PP_IHB_LPM1_TBL_ID                                                                                (SOC_PB_TBL_ID_LAST + 112)
#define SOC_PB_PP_IHB_LPM2_TBL_ID                                                                                (SOC_PB_TBL_ID_LAST + 113)
#define SOC_PB_PP_IHB_LPM3_TBL_ID                                                                                (SOC_PB_TBL_ID_LAST + 114)
#define SOC_PB_PP_IHB_LPM4_TBL_ID                                                                                (SOC_PB_TBL_ID_LAST + 115)
#define SOC_PB_PP_IHB_LPM5_TBL_ID                                                                                (SOC_PB_TBL_ID_LAST + 116)
#define SOC_PB_PP_IHB_LPM6_TBL_ID                                                                                (SOC_PB_TBL_ID_LAST + 117)
#define SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ID                                                               (SOC_PB_TBL_ID_LAST + 118)
#define SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ID                                                              (SOC_PB_TBL_ID_LAST + 119)
#define SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ID                                                              (SOC_PB_TBL_ID_LAST + 120)
#define SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ID                                                              (SOC_PB_TBL_ID_LAST + 121)
#define SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ID                                                              (SOC_PB_TBL_ID_LAST + 122)
#define SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ID                                                              (SOC_PB_TBL_ID_LAST + 123)
#define SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ID                                                              (SOC_PB_TBL_ID_LAST + 124)
#define SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ID                                                              (SOC_PB_TBL_ID_LAST + 125)
#define SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ID                                                              (SOC_PB_TBL_ID_LAST + 126)
#define SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ID                                                                  (SOC_PB_TBL_ID_LAST + 127)
#define SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID                                                     (SOC_PB_TBL_ID_LAST + 128)
#define SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1                                                   (SOC_PB_TBL_ID_LAST + 129)
#define SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2                                                   (SOC_PB_TBL_ID_LAST + 130)
#define SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3                                                   (SOC_PB_TBL_ID_LAST + 131)
#define SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID                                                     (SOC_PB_TBL_ID_LAST + 132)
#define SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_1                                                   (SOC_PB_TBL_ID_LAST + 133)
#define SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_2                                                   (SOC_PB_TBL_ID_LAST + 134)
#define SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ID_3                                                   (SOC_PB_TBL_ID_LAST + 135)
#define SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID                                             (SOC_PB_TBL_ID_LAST + 136)
#define SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ID                                             (SOC_PB_TBL_ID_LAST + 137)
#define SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ID                                                                 (SOC_PB_TBL_ID_LAST + 138)
#define SOC_PB_PP_IHB_TCAM_BANK_TBL_ID                                                                           (SOC_PB_TBL_ID_LAST + 139)
#define SOC_PB_PP_IHB_TCAM_ACTION_TBL_ID                                                                         (SOC_PB_TBL_ID_LAST + 140)
#define SOC_PB_PP_EGQ_PPCT_TBL_ID                                                                                (SOC_PB_TBL_ID_LAST + 141)
#define SOC_PB_PP_EGQ_PP_PPCT_TBL_ID                                                                             (SOC_PB_TBL_ID_LAST + 142)
#define SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID                                                       (SOC_PB_TBL_ID_LAST + 143)
#define SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ID                                                                      (SOC_PB_TBL_ID_LAST + 144)
#define SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ID                                                                           (SOC_PB_TBL_ID_LAST + 145)
#define SOC_PB_PP_EGQ_AUX_TABLE_TBL_ID                                                                           (SOC_PB_TBL_ID_LAST + 146)
#define SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ID                                                                     (SOC_PB_TBL_ID_LAST + 147)
#define SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ID                                                                (SOC_PB_TBL_ID_LAST + 148)
#define SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK1_TBL_ID                                                              (SOC_PB_TBL_ID_LAST + 149)
#define SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ID                                       (SOC_PB_TBL_ID_LAST + 150)
#define SOC_PB_PP_EPN_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ID                                         (SOC_PB_TBL_ID_LAST + 151)
#define SOC_PB_PP_EPN_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ID                                        (SOC_PB_TBL_ID_LAST + 152)
#define SOC_PB_PP_EPN_ENCAPSULATION_DB_BANK2_TBL_ID                                                              (SOC_PB_TBL_ID_LAST + 153)
#define SOC_PB_PP_EPN_TX_TAG_TABLE_TBL_ID                                                                        (SOC_PB_TBL_ID_LAST + 154)
#define SOC_PB_PP_EPN_STP_TBL_ID                                                                                 (SOC_PB_TBL_ID_LAST + 155)
#define SOC_PB_PP_EPN_SMALL_EM_RESULT_MEMORY_TBL_ID                                                              (SOC_PB_TBL_ID_LAST + 156)
#define SOC_PB_PP_EPN_PCP_DEI_TABLE_TBL_ID                                                                       (SOC_PB_TBL_ID_LAST + 157)
#define SOC_PB_PP_EPN_PP_PCT_TBL_ID                                                                              (SOC_PB_TBL_ID_LAST + 158)
#define SOC_PB_PP_EPN_LLVP_TABLE_TBL_ID                                                                          (SOC_PB_TBL_ID_LAST + 159)
#define SOC_PB_PP_EPN_EGRESS_EDIT_CMD_TBL_ID                                                                     (SOC_PB_TBL_ID_LAST + 160)
#define SOC_PB_PP_EPN_PROGRAM_VARS_TBL_ID                                                                        (SOC_PB_TBL_ID_LAST + 161)
#define SOC_PB_PP_EPN_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID                                                       (SOC_PB_TBL_ID_LAST + 162)
#define SOC_PB_PP_EPN_IP_EXP_MAP_TBL_ID                                                                          (SOC_PB_TBL_ID_LAST + 163)
#define SOC_PB_PP_EPN_ETPP_DEBUG_TBL_ID                                                                          (SOC_PB_TBL_ID_LAST + 164)
/* as there are to tables, odd and even, don't edit */
#define SOC_PB_PP_IHB_FEC_ENTRY_TBL2_ID                                                                          (SOC_PB_TBL_ID_LAST + 164)
#define SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL2_ID                                                                  (SOC_PB_TBL_ID_LAST + 165)
#define SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL2_ID                                                             (SOC_PB_TBL_ID_LAST + 166)
#define SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL2_ID                                                                       (SOC_PB_TBL_ID_LAST + 167)
#define SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL2_ID                                                                  (SOC_PB_TBL_ID_LAST + 168)
#define SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL2_ID                                                                 (SOC_PB_TBL_ID_LAST + 169)
#define SOC_PB_PP_EPN_ESEM_MGMT_REQUEST_ID                                                                       (SOC_PB_TBL_ID_LAST + 170)
#define SOC_PB_PP_TBL_ID_LAST                                                                                    (SOC_PB_TBL_ID_LAST + 171)

/* table index definitions } */
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
/*****************************************************
*NAME
* soc_pb_pp_indirect_blocks_init
*TYPE:
*  PROC
*FUNCTION:
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_pb_pp_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
void
  soc_pb_pp_indirect_blocks_init(
  );
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __CHIP_SIM_PB_PP_INCLUDED__*/
#endif

       
