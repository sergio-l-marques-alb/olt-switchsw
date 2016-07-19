/* $Id: pb_pp_tbl_access.c,v 1.9 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
                                                                    
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_tbls.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_bitstream.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PB_PP_TCAM_NOF_BANKS  4
#define SOC_PB_PP_TCAM_NOF_LINES  512
#define SOC_PB_PP_TCAM_LOW_WORD_MASK  0x3
#define SOC_PB_PP_TCAM_HIGH_WORD_MASK  0xc
#define SOC_PB_PP_TCAM_ALL_WORD_MASK  0xf

#define SOC_PB_PP_IHP_PINFO_LLR_TBL_ENTRY_SIZE                                                   ( 3)
#define SOC_PB_PP_IHP_LLR_LLVP_TBL_ENTRY_SIZE                                                    ( 1)
#define SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ENTRY_SIZE                                           ( 1)
#define SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ENTRY_SIZE                                             ( 2)
#define SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ENTRY_SIZE                                               ( 1)
#define SOC_PB_PP_IHP_TOS_2_COS_TBL_ENTRY_SIZE                                                   ( 1)
#define SOC_PB_PP_IHP_RESERVED_MC_TBL_ENTRY_SIZE                                                 ( 1)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ENTRY_SIZE                                       ( 2)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ENTRY_SIZE                              ( 2)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ENTRY_SIZE                               ( 2)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ENTRY_SIZE                               ( 2)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ENTRY_SIZE                              ( 2)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ENTRY_SIZE                          ( 2)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ENTRY_SIZE                                       ( 2)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ENTRY_SIZE                                 ( 2)
#define SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ENTRY_SIZE                                  ( 2)
#define SOC_PB_PP_IHP_FLUSH_DB_TBL_ENTRY_SIZE                                                    ( 7)
#define SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ENTRY_SIZE                                     ( 1)
#define SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE                             ( 1)
#define SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ENTRY_SIZE                          ( 1)
#define SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ENTRY_SIZE                                   ( 1)
#define SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ENTRY_SIZE                                                ( 1)
#define SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ENTRY_SIZE                                              ( 1)
#define SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE                                ( 1)
#define SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                                 ( 1)
#define SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE                                ( 2)
#define SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE                        ( 1)
#define SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                         ( 1)
#define SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE                       (4)
#define SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE                             ( 1)
#define SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                              ( 1)
#define SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE                             ( 2)
#define SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ENTRY_SIZE                                             ( 1)
#define SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ENTRY_SIZE                                             ( 1)
#define SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ENTRY_SIZE                                         ( 1)
#define SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ENTRY_SIZE                             (3)
#define SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ENTRY_SIZE                                        ( 1)
#define SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ENTRY_SIZE                                         ( 1)
#define SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ENTRY_SIZE                               ( 1)
#define SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ENTRY_SIZE                                     ( 3)
#define SOC_PB_PP_IHP_VSI_ISID_TBL_ENTRY_SIZE                                                    ( 1)
#define SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ENTRY_SIZE                                                  ( 1)
#define SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ENTRY_SIZE                                             ( 1)
#define SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ENTRY_SIZE                                               ( 1)
#define SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ENTRY_SIZE                                            ( 1)
#define SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ENTRY_SIZE                                             ( 1)
#define SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ENTRY_SIZE                                               ( 1)
#define SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ENTRY_SIZE                                            ( 1)
#define SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ENTRY_SIZE                                             ( 1)
#define SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ENTRY_SIZE                                ( 1)
#define SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ENTRY_SIZE                                  ( 2)
#define SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ENTRY_SIZE                                       ( 1)
#define SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ENTRY_SIZE                                         ( 2)
#define SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ENTRY_SIZE                                      (2)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ENTRY_SIZE                                            ( 2)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ENTRY_SIZE                               ( 2)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ENTRY_SIZE                              ( 2)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ENTRY_SIZE                              ( 2)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ENTRY_SIZE                                      ( 2)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ENTRY_SIZE                                   ( 2)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ENTRY_SIZE                                    ( 2)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ENTRY_SIZE                                      ( 2)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ENTRY_SIZE                                      ( 2)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ENTRY_SIZE                              ( 2)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ENTRY_SIZE                               ( 2)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ENTRY_SIZE                                  ( 2)
#define SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ENTRY_SIZE                                  ( 2)
#define SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ENTRY_SIZE                                       ( 1)
#define SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ENTRY_SIZE                                    ( 1)
#define SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ENTRY_SIZE                                  ( 1)
#define SOC_PB_PP_IHP_STP_TABLE_TBL_ENTRY_SIZE                                                   ( 4)
#define SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ENTRY_SIZE                                             ( 1)
#define SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ENTRY_SIZE                                ( 1)
#define SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ENTRY_SIZE                               ( 1)
#define SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ENTRY_SIZE                                    ( 1)
#define SOC_PB_PP_IHP_VTT_LLVP_TBL_ENTRY_SIZE                                                    ( 1)
#define SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ENTRY_SIZE                                               ( 1)
#define SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE                                    ( 2)
#define SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE                                    ( 2)
#define SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ENTRY_SIZE                                     ( 2)
#define SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ENTRY_SIZE                             ( 1)
#define SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ENTRY_SIZE                                       ( 1)
#define SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ENTRY_SIZE                                               ( 1)
#define SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ENTRY_SIZE                                         ( 1)
#define SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ENTRY_SIZE                                         ( 1)
#define SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ENTRY_SIZE                                             ( 1)
#define SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ENTRY_SIZE                               ( 1)
#define SOC_PB_PP_IHB_PINFO_FER_TBL_ENTRY_SIZE                                                   ( 1)
#define SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ENTRY_SIZE                                             ( 1)
#define SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ENTRY_SIZE                                              ( 1)
#define SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ENTRY_SIZE                                       ( 2)
#define SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE                                             ( 1)
#define SOC_PB_PP_IHB_FEC_ENTRY_TBL_ENTRY_SIZE                                                   ( 2)
#define SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ENTRY_SIZE                                           ( 2)
#define SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ENTRY_SIZE                                      ( 2)
#define SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ENTRY_SIZE                                                ( 2)
#define SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ENTRY_SIZE                                           ( 2)
#define SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ENTRY_SIZE                                          ( 1)
#define SOC_PB_PP_IHB_PATH_SELECT_TBL_ENTRY_SIZE                                                 ( 1)
#define SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ENTRY_SIZE                                          ( 1)
#define SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ENTRY_SIZE                                            (4)
#define SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ENTRY_SIZE                                                ( 1)
#define SOC_PB_PP_IHB_PINFO_FLP_TBL_ENTRY_SIZE                                                   ( 1)
#define SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ENTRY_SIZE                                     ( 1)
#define SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ENTRY_SIZE                                         ( 5)
#define SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ENTRY_SIZE                                  ( 1)
#define SOC_PB_PP_IHB_VRF_CONFIG_TBL_ENTRY_SIZE                                                  ( 1)
#define SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ENTRY_SIZE                                              ( 1)
#define SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ENTRY_SIZE                                             ( 1)
#define SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ENTRY_SIZE                                             ( 1)
#define SOC_PB_PP_IHB_LPM_TBL_ENTRY_SIZE                                                         ( 1)
#define SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ENTRY_SIZE                                       ( 1)
#define SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ENTRY_SIZE                                      ( 1)
#define SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ENTRY_SIZE                                      ( 1)
#define SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ENTRY_SIZE                                      ( 1)
#define SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ENTRY_SIZE                                      ( 1)
#define SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ENTRY_SIZE                                      ( 1)
#define SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ENTRY_SIZE                                      ( 1)
#define SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ENTRY_SIZE                                      ( 1)
#define SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ENTRY_SIZE                                      ( 1)
#define SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ENTRY_SIZE                                         (2)
#define SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ENTRY_SIZE                             ( 2)
#define SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ENTRY_SIZE                             ( 2)
#define SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                     ( 2)
#define SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                     ( 2)
#define SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ENTRY_SIZE                                         ( 1)
#define SOC_PB_PP_IHB_TCAM_BANK_TBL_ENTRY_SIZE                                                   (10)
#define SOC_PB_PP_IHB_TCAM_ACTION_TBL_ENTRY_SIZE                                                 ( 2)
#define SOC_PB_PP_EGQ_PPCT_TBL_ENTRY_SIZE                                                        ( 3)
#define SOC_PB_PP_EGQ_PP_PPCT_TBL_ENTRY_SIZE                                                     ( 1)
#define SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE                               ( 1)
#define SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ENTRY_SIZE                                              ( 2)
#define SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ENTRY_SIZE                                                   ( 1)
#define SOC_PB_PP_EGQ_AUX_TABLE_TBL_ENTRY_SIZE                                                   ( 1)
#define SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ENTRY_SIZE                                             ( 1)
#define SOC_PB_PP_EGQ_CFM_TRAP_TBL_ENTRY_SIZE                                                    ( 1)
#define SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ENTRY_SIZE                                        ( 2)
#define SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ENTRY_SIZE              ( 3)
#define SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ENTRY_SIZE                ( 3)
#define SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ENTRY_SIZE               ( 3)
#define SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_ENTRY_SIZE                                               ( 2)
#define SOC_PB_PP_EPNI_STP_TBL_ENTRY_SIZE                                                        ( 2)
#define SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_ENTRY_SIZE                                     ( 1)
#define SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_ENTRY_SIZE                                              ( 1)
#define SOC_PB_PP_EPNI_PP_PCT_TBL_ENTRY_SIZE                                                     ( 2)
#define SOC_PB_PP_EPNI_LLVP_TABLE_TBL_ENTRY_SIZE                                                 ( 1)
#define SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_ENTRY_SIZE                                            ( 1)
#define SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_ENTRY_SIZE                                               ( 1)
#define SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE                              ( 1)
#define SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_ENTRY_SIZE                                                 ( 1)
#define SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_ENTRY_SIZE                                                 ( 1)
#define SOC_PB_PP_IHB_PGM_INSTR_NOF_CES_PER_TBL                                                  ( 2)

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* for odd entry jump to odd table*/
#define SOC_PB_PP_TBL_FEC_ODD_ENTRY_OFFSET(entry, tbl_act_addr)   \
  (tbl_act_addr.base + (entry)/2)

/* for odd entry jump to odd table*/
#define SOC_PB_PP_TBL_FEC_ODD_ENTRY_ADDR_SET(entry_offset, tbl_addr, tbl_act_addr)   \
  SOC_PETRA_COPY(tbl_act_addr,tbl_addr,SOC_PETRA_TBL_ADDR,1); \
  (tbl_act_addr)->base = (entry_offset%2 == 1)?((tbl_addr)->base+0x2000):(tbl_addr)->base;

/* for odd entry jump to odd table*/
#define SOC_PB_PP_TBL_FEC_ACCESSED_ODD_ENTRY_OFFSET(entry, tbl_act_addr)   \
  (tbl_act_addr.base + (entry)/2)

/* for odd entry jump to odd table*/
#define SOC_PB_PP_TBL_FEC_ACCESSED_ODD_ENTRY_ADDR_SET(entry_offset, tbl_addr, tbl_act_addr)   \
  SOC_PETRA_COPY(tbl_act_addr,tbl_addr,SOC_PETRA_TBL_ADDR,1); \
  (tbl_act_addr)->base = (entry_offset%2 == 1)?((tbl_addr)->base+0x400):(tbl_addr)->base;

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

static uint32
  Soc_pb_pp_nof_tbl_reps[SOC_SAND_MAX_DEVICE];

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_pb_pp_tbl_access_init_unsafe(
    SOC_SAND_IN  int             unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  Soc_pb_pp_nof_tbl_reps[unit] = 0;
  goto exit;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_tbl_access_init_unsafe()",0,0);
}

uint32
  soc_pb_pp_reps_for_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_REPS_FOR_TBL_SET_UNSAFE);

  Soc_pb_pp_nof_tbl_reps[unit] = nof_reps;
  goto exit;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_reps_for_tbl_set_unsafe()",0,0);
}

uint32
  soc_pb_pp_tbl_write_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32               *data_ptr,
    SOC_SAND_IN  uint32               offset,
    SOC_SAND_IN  SOC_PETRA_TBL_ADDR         *tbl_addr,
    SOC_SAND_IN  uint32              size,
    SOC_SAND_IN  uint32              module_id,
    SOC_SAND_IN  uint32              ind_options
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_REGS
    *regs = soc_pb_regs();
  uint32
    nof_lines,
    nof_lines_total,
    lines_max,
    first_entry;
  SOC_PETRA_REG_FIELD
    *command_count_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TBL_WRITE_UNSAFE);

  if (tbl_addr->base != SOC_PB_PP_CHIP_TBLS_LOGICAL_TBL_BASE_ADDR)
  {
    /* If a logical table, reps count and offset is handled in the calling function */
    if (offset >= tbl_addr->base + tbl_addr->size)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_TBL_RANGE_OUT_OF_LIMIT, 1001, exit);
    }
    if (Soc_pb_pp_nof_tbl_reps[unit] > tbl_addr->base + tbl_addr->size - offset + 1)
    {
      Soc_pb_pp_nof_tbl_reps[unit] = tbl_addr->base + tbl_addr->size - offset + 1;
    }
  }

  switch(module_id)
  {
  case SOC_PB_PP_IHP_ID:
    command_count_fld = &(regs->ihp.indirect_command_reg.indirect_command_count);
  	break;
  case SOC_PB_PP_IHB_ID:
    command_count_fld = &(regs->ihb.indirect_command_reg.indirect_command_count);
  	break;
  case SOC_PB_PP_EGQ_ID:
    command_count_fld = &(regs->egq.indirect_command_reg.indirect_command_count);
  	break;
  case SOC_PB_PP_EPNI_ID:
    command_count_fld = &(regs->epni.indirect_command_reg.indirect_command_count);
  	break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_TBL_ACCESS_UNKNOWN_MODULE_ID_ERR, 10, exit);
    break;
  }

  first_entry = 0;
  nof_lines_total = Soc_pb_pp_nof_tbl_reps[unit];
  lines_max = SOC_SAND_BITS_MASK(command_count_fld->msb - command_count_fld->lsb, 0);

  do
  {
    nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
    nof_lines_total -= nof_lines;

    res = soc_petra_write_fld_unsafe(unit, command_count_fld, SOC_PETRA_DEFAULT_INSTANCE, nof_lines);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_sand_tbl_write_unsafe(
            unit,
            data_ptr,
            offset + first_entry,
            size,
            module_id,
            ind_options
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    first_entry += nof_lines;
  } while(nof_lines_total > 0);

  do
  {
    res = soc_petra_read_fld_unsafe(unit, command_count_fld, SOC_PETRA_DEFAULT_INSTANCE, &nof_lines);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  } while ((nof_lines != 0) && (nof_lines != 1));

  /*
   *	Reset the number of replications to 0
   */
  res = soc_pb_pp_tbl_access_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_tbl_write_unsafe()",0,0);
}

uint32
  soc_pb_pp_tbl_read_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_OUT uint32               *result_ptr,
    SOC_SAND_IN  uint32               offset,
    SOC_SAND_IN  SOC_PETRA_TBL_ADDR         *tbl_addr,
    SOC_SAND_IN  uint32              size,
    SOC_SAND_IN  uint32              module_id,
    SOC_SAND_IN  uint32              ind_options
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_TBL_READ_UNSAFE);

  if (tbl_addr->base != 0xffffffff)
  {
    if (offset >= tbl_addr->base + tbl_addr->size)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_TBL_RANGE_OUT_OF_LIMIT, 1001, exit);
    }
    if (Soc_pb_pp_nof_tbl_reps[unit] > tbl_addr->base + tbl_addr->size - offset - 1)
    {
      Soc_pb_pp_nof_tbl_reps[unit] = tbl_addr->base + tbl_addr->size - offset - 1;
    }
  }

  res = soc_sand_tbl_read_unsafe(
          unit,
          result_ptr,
          offset,
          size,
          module_id,
          ind_options
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_tbl_read_unsafe()",0,0);
}

uint32
  soc_pb_pp_ihp_port_mine_table_physical_port_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.port_mine_table_physical_port_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->physical_port_mine0),
          &(tbl_data->physical_port_mine0)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->physical_port_mine1),
          &(tbl_data->physical_port_mine1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->physical_port_mine2),
          &(tbl_data->physical_port_mine2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->physical_port_mine3),
          &(tbl_data->physical_port_mine3)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_port_mine_table_physical_port_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_port_mine_table_physical_port_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.port_mine_table_physical_port_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->physical_port_mine0),
          &(tbl->physical_port_mine0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->physical_port_mine1),
          &(tbl->physical_port_mine1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->physical_port_mine2),
          &(tbl->physical_port_mine2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->physical_port_mine3),
          &(tbl->physical_port_mine3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);
  
  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_port_mine_table_physical_port_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pinfo_llr_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PINFO_LLR_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PINFO_LLR_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PINFO_LLR_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PINFO_LLR_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pinfo_llr_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PINFO_LLR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_classification_profile),
          &(tbl_data->vlan_classification_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pcp_profile),
          &(tbl_data->pcp_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ignore_incoming_pcp),
          &(tbl_data->ignore_incoming_pcp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tc_dp_tos_enable),
          &(tbl_data->tc_dp_tos_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tc_dp_tos_index),
          &(tbl_data->tc_dp_tos_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tc_l4_protocol_enable),
          &(tbl_data->tc_l4_protocol_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tc_subnet_enable),
          &(tbl_data->tc_subnet_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tc_port_protocol_enable),
          &(tbl_data->tc_port_protocol_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->default_traffic_class),
          &(tbl_data->default_traffic_class)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ignore_incoming_up),
          &(tbl_data->ignore_incoming_up)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->incoming_up_map_profile),
          &(tbl_data->incoming_up_map_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->use_dei),
          &(tbl_data->use_dei)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->drop_precedence_profile),
          &(tbl_data->drop_precedence_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sa_lookup_enable),
          &(tbl_data->sa_lookup_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_profile_sa_drop_index),
          &(tbl_data->action_profile_sa_drop_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->enable_sa_authentication),
          &(tbl_data->enable_sa_authentication)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ignore_incoming_vid),
          &(tbl_data->ignore_incoming_vid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vid_ipv4_subnet_enable),
          &(tbl_data->vid_ipv4_subnet_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vid_port_protocol_enable),
          &(tbl_data->vid_port_protocol_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->protocol_profile),
          &(tbl_data->protocol_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 59, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->default_initial_vid),
          &(tbl_data->default_initial_vid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->non_authorized_mode_8021x),
          &(tbl_data->non_authorized_mode_8021x)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 61, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->enable_arp_trap),
          &(tbl_data->enable_arp_trap)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->enable_igmp_trap),
          &(tbl_data->enable_igmp_trap)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 63, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->enable_mld_trap),
          &(tbl_data->enable_mld_trap)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 64, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->enable_dhcp_trap),
          &(tbl_data->enable_dhcp_trap)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->general_trap_enable),
          &(tbl_data->general_trap_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 66, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->default_cpu_trap_code),
          &(tbl_data->default_cpu_trap_code)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 67, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->default_action_profile_fwd),
          &(tbl_data->default_action_profile_fwd)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 68, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->reserved_mc_trap_profile),
          &(tbl_data->reserved_mc_trap_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 68, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->default_action_profile_snp),
          &(tbl_data->default_action_profile_snp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 69, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pinfo_llr_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pinfo_llr_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PINFO_LLR_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PINFO_LLR_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PINFO_LLR_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PINFO_LLR_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pinfo_llr_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->vlan_classification_profile),
          &(tbl->vlan_classification_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pcp_profile),
          &(tbl->pcp_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ignore_incoming_pcp),
          &(tbl->ignore_incoming_pcp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tc_dp_tos_enable),
          &(tbl->tc_dp_tos_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tc_dp_tos_index),
          &(tbl->tc_dp_tos_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tc_l4_protocol_enable),
          &(tbl->tc_l4_protocol_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tc_subnet_enable),
          &(tbl->tc_subnet_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tc_port_protocol_enable),
          &(tbl->tc_port_protocol_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->default_traffic_class),
          &(tbl->default_traffic_class),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ignore_incoming_up),
          &(tbl->ignore_incoming_up),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->incoming_up_map_profile),
          &(tbl->incoming_up_map_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->use_dei),
          &(tbl->use_dei),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->drop_precedence_profile),
          &(tbl->drop_precedence_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->sa_lookup_enable),
          &(tbl->sa_lookup_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_profile_sa_drop_index),
          &(tbl->action_profile_sa_drop_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->reserved_mc_trap_profile),
          &(tbl->reserved_mc_trap_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->enable_sa_authentication),
          &(tbl->enable_sa_authentication),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ignore_incoming_vid),
          &(tbl->ignore_incoming_vid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vid_ipv4_subnet_enable),
          &(tbl->vid_ipv4_subnet_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vid_port_protocol_enable),
          &(tbl->vid_port_protocol_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->protocol_profile),
          &(tbl->protocol_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->default_initial_vid),
          &(tbl->default_initial_vid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->non_authorized_mode_8021x),
          &(tbl->non_authorized_mode_8021x),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->enable_arp_trap),
          &(tbl->enable_arp_trap),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->enable_igmp_trap),
          &(tbl->enable_igmp_trap),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->enable_mld_trap),
          &(tbl->enable_mld_trap),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->enable_dhcp_trap),
          &(tbl->enable_dhcp_trap),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->general_trap_enable),
          &(tbl->general_trap_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->default_cpu_trap_code),
          &(tbl->default_cpu_trap_code),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->default_action_profile_fwd),
          &(tbl->default_action_profile_fwd),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->default_action_profile_snp),
          &(tbl->default_action_profile_snp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 59, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PINFO_LLR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pinfo_llr_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_llr_llvp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LLR_LLVP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LLR_LLVP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LLR_LLVP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LLR_LLVP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LLR_LLVP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_LLR_LLVP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.llr_llvp_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LLR_LLVP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->incoming_vid_exist),
          &(tbl_data->incoming_vid_exist)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->incoming_tag_exist),
          &(tbl_data->incoming_tag_exist)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->incoming_stag_exist),
          &(tbl_data->incoming_stag_exist)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->acceptable_frame_type_action),
          &(tbl_data->acceptable_frame_type_action)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->outer_comp_index),
          &(tbl_data->outer_comp_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inner_comp_index),
          &(tbl_data->inner_comp_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_llr_llvp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_llr_llvp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LLR_LLVP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LLR_LLVP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LLR_LLVP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LLR_LLVP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LLR_LLVP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.llr_llvp_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->incoming_vid_exist),
          &(tbl->incoming_vid_exist),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->incoming_tag_exist),
          &(tbl->incoming_tag_exist),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->incoming_stag_exist),
          &(tbl->incoming_stag_exist),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->acceptable_frame_type_action),
          &(tbl->acceptable_frame_type_action),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->outer_comp_index),
          &(tbl->outer_comp_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inner_comp_index),
          &(tbl->inner_comp_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LLR_LLVP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_llr_llvp_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_ll_mirror_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.ll_mirror_profile_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ll_mirror_profile),
          &(tbl_data->ll_mirror_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_ll_mirror_profile_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.ll_mirror_profile_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ll_mirror_profile),
          &(tbl->ll_mirror_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_ll_mirror_profile_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_subnet_classify_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.subnet_classify_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipv4_subnet_valid),
          &(tbl_data->ipv4_subnet_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipv4_subnet_tc_valid),
          &(tbl_data->ipv4_subnet_tc_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipv4_subnet_tc),
          &(tbl_data->ipv4_subnet_tc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipv4_subnet_vid),
          &(tbl_data->ipv4_subnet_vid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipv4_subnet_mask),
          &(tbl_data->ipv4_subnet_mask)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipv4_subnet),
          &(tbl_data->ipv4_subnet)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_subnet_classify_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_subnet_classify_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.subnet_classify_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ipv4_subnet_valid),
          &(tbl->ipv4_subnet_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ipv4_subnet_tc_valid),
          &(tbl->ipv4_subnet_tc_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ipv4_subnet_tc),
          &(tbl->ipv4_subnet_tc),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ipv4_subnet_vid),
          &(tbl->ipv4_subnet_vid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ipv4_subnet_mask),
          &(tbl->ipv4_subnet_mask),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ipv4_subnet),
          &(tbl->ipv4_subnet),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_subnet_classify_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_port_protocol_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PORT_PROTOCOL_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.port_protocol_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->traffic_class_valid),
          &(tbl_data->traffic_class_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->traffic_class),
          &(tbl_data->traffic_class)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vid_valid),
          &(tbl_data->vid_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vid),
          &(tbl_data->vid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_port_protocol_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_port_protocol_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PORT_PROTOCOL_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.port_protocol_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->traffic_class_valid),
          &(tbl->traffic_class_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->traffic_class),
          &(tbl->traffic_class),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vid_valid),
          &(tbl->vid_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vid),
          &(tbl->vid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_port_protocol_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_tos_2_cos_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_TOS_2_COS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_TOS_2_COS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_TOS_2_COS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_TOS_2_COS_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_TOS_2_COS_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_TOS_2_COS_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.tos_2_cos_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_TOS_2_COS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dp),
          &(tbl_data->dp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tc),
          &(tbl_data->tc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->valid),
          &(tbl_data->valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_tos_2_cos_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_tos_2_cos_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_TOS_2_COS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_TOS_2_COS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_TOS_2_COS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_TOS_2_COS_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_TOS_2_COS_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.tos_2_cos_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->dp),
          &(tbl->dp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tc),
          &(tbl->tc),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->valid),
          &(tbl->valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_TOS_2_COS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_tos_2_cos_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_reserved_mc_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_RESERVED_MC_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_RESERVED_MC_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_RESERVED_MC_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_RESERVED_MC_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_RESERVED_MC_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_RESERVED_MC_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.reserved_mc_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_RESERVED_MC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cpu_trap_code),
          &(tbl_data->cpu_trap_code)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->snp),
          &(tbl_data->snp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd),
          &(tbl_data->fwd)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_reserved_mc_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_reserved_mc_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_RESERVED_MC_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_RESERVED_MC_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_RESERVED_MC_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_RESERVED_MC_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_RESERVED_MC_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.reserved_mc_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->cpu_trap_code),
          &(tbl->cpu_trap_code),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->snp),
          &(tbl->snp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd),
          &(tbl->fwd),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_RESERVED_MC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_reserved_mc_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_pldt_format1_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->format_specific_data),
          &(tbl_data->format_specific_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_pldt_format1_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_tp2p_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_pldt_format1_asd_tp2p_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif),
          &(tbl_data->out_lif)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif_valid),
          &(tbl_data->out_lif_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->identifier),
          &(tbl_data->identifier)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->p2p_service),
          &(tbl_data->p2p_service)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_pldt_format1_asd_tp2p_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_tp2p_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_pldt_format1_asd_tp2p_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_lif),
          &(tbl->out_lif),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_lif_valid),
          &(tbl->out_lif_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->identifier),
          &(tbl->identifier),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->p2p_service),
          &(tbl->p2p_service),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_pldt_format1_asd_tp2p_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_pbp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_pldt_format1_asd_pbp_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->learn_destination),
          &(tbl_data->learn_destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->isid_domain),
          &(tbl_data->isid_domain)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->learn_type),
          &(tbl_data->learn_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sa_drop),
          &(tbl_data->sa_drop)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->is_dynamic),
          &(tbl_data->is_dynamic)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_pldt_format1_asd_pbp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_pbp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_pldt_format1_asd_pbp_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->learn_destination),
          &(tbl->learn_destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->isid_domain),
          &(tbl->isid_domain),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->learn_type),
          &(tbl->learn_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->sa_drop),
          &(tbl->sa_drop),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->is_dynamic),
          &(tbl->is_dynamic),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_pldt_format1_asd_pbp_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_ilm_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_pldt_format1_asd_ilm_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mpls_label),
          &(tbl_data->mpls_label)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mpls_command),
          &(tbl_data->mpls_command)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->p2p_service),
          &(tbl_data->p2p_service)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_pldt_format1_asd_ilm_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_ilm_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_pldt_format1_asd_ilm_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->mpls_label),
          &(tbl->mpls_label),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->mpls_command),
          &(tbl->mpls_command),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->p2p_service),
          &(tbl->p2p_service),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_pldt_format1_asd_ilm_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_sp2p_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_pldt_format1_asd_sp2p_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif),
          &(tbl_data->out_lif)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif_valid),
          &(tbl_data->out_lif_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tpid_profile),
          &(tbl_data->tpid_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->has_cw),
          &(tbl_data->has_cw)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->p2p_service),
          &(tbl_data->p2p_service)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_pldt_format1_asd_sp2p_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_sp2p_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_pldt_format1_asd_sp2p_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_lif),
          &(tbl->out_lif),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_lif_valid),
          &(tbl->out_lif_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tpid_profile),
          &(tbl->tpid_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->has_cw),
          &(tbl->has_cw),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->p2p_service),
          &(tbl->p2p_service),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_pldt_format1_asd_sp2p_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_ethernet_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_pldt_format1_asd_ethernet_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->asd),
          &(tbl_data->asd)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->is_dynamic),
          &(tbl_data->is_dynamic)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_pldt_format1_asd_ethernet_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_pldt_format1_asd_ethernet_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_pldt_format1_asd_ethernet_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->asd),
          &(tbl->asd),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->is_dynamic),
          &(tbl->is_dynamic),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_pldt_format1_asd_ethernet_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_pldt_format2_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_pldt_format2_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->permitted_system_port),
          &(tbl_data->permitted_system_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->format_specific_data),
          &(tbl_data->format_specific_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_pldt_format2_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_pldt_format2_trill_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_pldt_format2_trill_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->permitted_system_port),
          &(tbl_data->permitted_system_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->adjacent),
          &(tbl_data->adjacent)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->drop_if_vid_differ),
          &(tbl_data->drop_if_vid_differ)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->permit_all_ports),
          &(tbl_data->permit_all_ports)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->override_vid_in_tagged),
          &(tbl_data->override_vid_in_tagged)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->use_vid_in_untagged),
          &(tbl_data->use_vid_in_untagged)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->is_dynamic),
          &(tbl_data->is_dynamic)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_pldt_format2_trill_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_pldt_format2_trill_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_pldt_format2_trill_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->permitted_system_port),
          &(tbl->permitted_system_port),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->adjacent),
          &(tbl->adjacent),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->drop_if_vid_differ),
          &(tbl->drop_if_vid_differ),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->permit_all_ports),
          &(tbl->permit_all_ports),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->override_vid_in_tagged),
          &(tbl->override_vid_in_tagged),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->use_vid_in_untagged),
          &(tbl->use_vid_in_untagged),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->is_dynamic),
          &(tbl->is_dynamic),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_pldt_format2_trill_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_pldt_format2_auth_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_pldt_format2_auth_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->permitted_system_port),
          &(tbl_data->permitted_system_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vid),
          &(tbl_data->vid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->accept_untagged),
          &(tbl_data->accept_untagged)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->drop_if_vid_differ),
          &(tbl_data->drop_if_vid_differ)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->permit_all_ports),
          &(tbl_data->permit_all_ports)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->override_vid_in_tagged),
          &(tbl_data->override_vid_in_tagged)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->use_vid_in_untagged),
          &(tbl_data->use_vid_in_untagged)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->is_dynamic),
          &(tbl_data->is_dynamic)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_pldt_format2_auth_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_pldt_format2_auth_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_pldt_format2_auth_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->permitted_system_port),
          &(tbl->permitted_system_port),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vid),
          &(tbl->vid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->accept_untagged),
          &(tbl->accept_untagged),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->drop_if_vid_differ),
          &(tbl->drop_if_vid_differ),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->permit_all_ports),
          &(tbl->permit_all_ports),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->override_vid_in_tagged),
          &(tbl->override_vid_in_tagged),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->use_vid_in_untagged),
          &(tbl->use_vid_in_untagged),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->is_dynamic),
          &(tbl->is_dynamic),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_pldt_format2_auth_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_flush_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_FLUSH_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_FLUSH_DB_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_FLUSH_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_FLUSH_DB_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_FLUSH_DB_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_FLUSH_DB_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.flush_db_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_FLUSH_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->compare_valid),
          &(tbl_data->compare_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->compare_key_16_msbs_data),
          &(tbl_data->compare_key_16_msbs_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->compare_key_16_msbs_mask),
          &(tbl_data->compare_key_16_msbs_mask)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->compare_payload_data),
          (tbl_data->compare_payload_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->compare_payload_mask),
          (tbl_data->compare_payload_mask)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_drop),
          &(tbl_data->action_drop)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_transplant_payload_data),
          (tbl_data->action_transplant_payload_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_transplant_payload_mask),
          (tbl_data->action_transplant_payload_mask)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_flush_db_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_flush_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_FLUSH_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_FLUSH_DB_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_FLUSH_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_FLUSH_DB_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_FLUSH_DB_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.flush_db_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->compare_valid),
          &(tbl->compare_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->compare_key_16_msbs_data),
          &(tbl->compare_key_16_msbs_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->compare_key_16_msbs_mask),
          &(tbl->compare_key_16_msbs_mask),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          (tbl_data->compare_payload_data),
          &(tbl->compare_payload_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          (tbl_data->compare_payload_mask),
          &(tbl->compare_payload_mask),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_drop),
          &(tbl->action_drop),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          (tbl_data->action_transplant_payload_data),
          &(tbl->action_transplant_payload_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          (tbl_data->action_transplant_payload_mask),
          &(tbl->action_transplant_payload_mask),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_FLUSH_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_flush_db_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_fid_counter_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_fid_counter_db_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->entry_count),
          &(tbl_data->entry_count)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->profile_pointer),
          &(tbl_data->profile_pointer)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_fid_counter_db_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_fid_counter_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_fid_counter_db_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->entry_count),
          &(tbl->entry_count),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->profile_pointer),
          &(tbl->profile_pointer),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_fid_counter_db_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_fid_counter_profile_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_fid_counter_profile_db_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->limit),
          &(tbl_data->limit)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->interrupt_en),
          &(tbl_data->interrupt_en)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->message_en),
          &(tbl_data->message_en)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->is_link_layer_fid),
          &(tbl_data->is_link_layer_fid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_fid_counter_profile_db_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_fid_counter_profile_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_fid_counter_profile_db_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->limit),
          &(tbl->limit),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->interrupt_en),
          &(tbl->interrupt_en),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->message_en),
          &(tbl->message_en),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->is_link_layer_fid),
          &(tbl->is_link_layer_fid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_fid_counter_profile_db_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_aging_configuration_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_aging_configuration_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->delete_entry[FALSE]),
          &(tbl_data->delete_entry[FALSE])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->create_aged_out_event[FALSE]),
          &(tbl_data->create_aged_out_event[FALSE])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->create_refresh_event[FALSE]),
          &(tbl_data->create_refresh_event[FALSE])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->delete_entry[TRUE]),
          &(tbl_data->delete_entry[TRUE])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->create_aged_out_event[TRUE]),
          &(tbl_data->create_aged_out_event[TRUE])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->create_refresh_event[TRUE]),
          &(tbl_data->create_refresh_event[TRUE])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_aging_configuration_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_large_em_aging_configuration_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.large_em_aging_configuration_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->delete_entry[FALSE]),
          &(tbl->delete_entry[FALSE]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->create_aged_out_event[FALSE]),
          &(tbl->create_aged_out_event[FALSE]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->create_refresh_event[FALSE]),
          &(tbl->create_refresh_event[FALSE]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->delete_entry[TRUE]),
          &(tbl->delete_entry[TRUE]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->create_aged_out_event[TRUE]),
          &(tbl->create_aged_out_event[TRUE]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->create_refresh_event[TRUE]),
          &(tbl->create_refresh_event[TRUE]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_large_em_aging_configuration_table_tbl_set_unsafe()", entry_offset, 0);
}


uint32
  soc_pb_pp_ihp_tm_port_pp_context_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.tm_port_pp_context_config_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_context_offset1),
          &(tbl_data->pp_context_offset1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_context_offset2),
          &(tbl_data->pp_context_offset2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_context_profile),
          &(tbl_data->pp_context_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_context_value_to_use),
          &(tbl_data->pp_context_value_to_use)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_tm_port_pp_context_config_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_tm_port_pp_context_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.tm_port_pp_context_config_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_context_offset1),
          &(tbl->pp_context_offset1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_context_offset2),
          &(tbl->pp_context_offset2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_context_profile),
          &(tbl->pp_context_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_context_value_to_use),
          &(tbl->pp_context_value_to_use),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_tm_port_pp_context_config_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pp_port_info_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PP_PORT_INFO_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PP_PORT_INFO_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PP_PORT_INFO_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PP_PORT_INFO_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_port_info_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->outer_header_start),
          &(tbl_data->outer_header_start)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tpid_profile),
          &(tbl_data->tpid_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pp_port_info_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pp_port_info_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PP_PORT_INFO_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PP_PORT_INFO_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PP_PORT_INFO_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_port_info_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->outer_header_start),
          &(tbl->outer_header_start),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tpid_profile),
          &(tbl->tpid_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PP_PORT_INFO_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pp_port_info_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pp_port_values_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PP_PORT_VALUES_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_port_values_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_port_packet_format_qualifier0_value),
          &(tbl_data->pp_port_packet_format_qualifier0_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_port_pp_context_value),
          &(tbl_data->pp_port_pp_context_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pp_port_values_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pp_port_values_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PP_PORT_VALUES_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_port_values_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_port_packet_format_qualifier0_value),
          &(tbl->pp_port_packet_format_qualifier0_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_port_pp_context_value),
          &(tbl->pp_port_pp_context_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pp_port_values_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pp_port_fem_bit_select_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_port_fem_bit_select_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_port_fem_bit_select),
          &(tbl_data->pp_port_fem_bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pp_port_fem_bit_select_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pp_port_fem_bit_select_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_port_fem_bit_select_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_port_fem_bit_select),
          &(tbl->pp_port_fem_bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pp_port_fem_bit_select_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pp_port_fem_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
   SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_port_fem_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset + (pgm_ndx << SOC_PB_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS);

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_port_fem_map_index),
          &(tbl_data->pp_port_fem_map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_port_fem_map_data),
          &(tbl_data->pp_port_fem_map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pp_port_fem_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pp_port_fem_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_port_fem_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset + (pgm_ndx << SOC_PB_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_port_fem_map_index),
          &(tbl->pp_port_fem_map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_port_fem_map_data),
          &(tbl->pp_port_fem_map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pp_port_fem_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pp_port_fem_field_select_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL
    *tbl;
  uint32
    field_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_port_fem_field_select_map_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (field_ndx = 0; field_ndx < SOC_PB_PORTS_FEM_PP_PORT_SIZE; ++field_ndx)
  {
    res = soc_petra_field_in_place_get(
            data,
            &(tbl->pp_port_fem_field_select[field_ndx]),
            &(tbl_data->pp_port_fem_field_select[field_ndx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40 + field_ndx, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pp_port_fem_field_select_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pp_port_fem_field_select_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL
    *tbl;
  uint32
    field_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_port_fem_field_select_map_tbl);

  offset =  tbl->addr.base + entry_offset;

  for (field_ndx = 0; field_ndx < SOC_PB_PORTS_FEM_PP_PORT_SIZE; ++field_ndx)
  {
    res = soc_petra_field_in_place_set(
            &(tbl_data->pp_port_fem_field_select[field_ndx]),
            &(tbl->pp_port_fem_field_select[field_ndx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30 + field_ndx, exit);
  }

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pp_port_fem_field_select_map_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_src_system_port_fem_bit_select_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.src_system_port_fem_bit_select_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->src_system_port_fem_bit_select),
          &(tbl_data->src_system_port_fem_bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_src_system_port_fem_bit_select_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_src_system_port_fem_bit_select_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.src_system_port_fem_bit_select_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->src_system_port_fem_bit_select),
          &(tbl->src_system_port_fem_bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_src_system_port_fem_bit_select_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_src_system_port_fem_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.src_system_port_fem_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset + (pgm_ndx << SOC_PB_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS);

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->src_system_port_fem_map_index),
          &(tbl_data->src_system_port_fem_map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->src_system_port_fem_map_data),
          &(tbl_data->src_system_port_fem_map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_src_system_port_fem_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_src_system_port_fem_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.src_system_port_fem_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset + (pgm_ndx << SOC_PB_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS);

  res = soc_petra_field_in_place_set(
          &(tbl_data->src_system_port_fem_map_index),
          &(tbl->src_system_port_fem_map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->src_system_port_fem_map_data),
          &(tbl->src_system_port_fem_map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_src_system_port_fem_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_src_system_port_fem_field_select_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL
    *tbl;
  uint32
    field_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.src_system_port_fem_field_select_map_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (field_ndx = 0; field_ndx < SOC_PB_PORTS_FEM_SYSTEM_PORT_SIZE; ++field_ndx)
  {
    res = soc_petra_field_in_place_get(
            data,
            &(tbl->src_system_port_fem_field_select[field_ndx]),
            &(tbl_data->src_system_port_fem_field_select[field_ndx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40 + field_ndx, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_src_system_port_fem_field_select_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_src_system_port_fem_field_select_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL
    *tbl;
  uint32
    field_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.src_system_port_fem_field_select_map_tbl);

  offset =  tbl->addr.base + entry_offset;

  for (field_ndx = 0; field_ndx < SOC_PB_PORTS_FEM_SYSTEM_PORT_SIZE; ++field_ndx)
  {
    res = soc_petra_field_in_place_set(
            &(tbl_data->src_system_port_fem_field_select[field_ndx]),
            &(tbl->src_system_port_fem_field_select[field_ndx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30 + field_ndx, exit);
  }


  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_src_system_port_fem_field_select_map_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pp_context_fem_bit_select_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_context_fem_bit_select_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_context_fem_bit_select),
          &(tbl_data->pp_context_fem_bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pp_context_fem_bit_select_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pp_context_fem_bit_select_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_context_fem_bit_select_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_context_fem_bit_select),
          &(tbl->pp_context_fem_bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pp_context_fem_bit_select_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pp_context_fem_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_context_fem_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset + (pgm_ndx << SOC_PB_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS);

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_context_fem_map_index),
          &(tbl_data->pp_context_fem_map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_context_fem_map_data),
          &(tbl_data->pp_context_fem_map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pp_context_fem_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pp_context_fem_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_context_fem_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset + (pgm_ndx << SOC_PB_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_context_fem_map_index),
          &(tbl->pp_context_fem_map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_context_fem_map_data),
          &(tbl->pp_context_fem_map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pp_context_fem_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pp_context_fem_field_select_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_context_fem_field_select_map_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_context_fem_field_select[0]),
          &(tbl_data->pp_context_fem_field_select[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_context_fem_field_select[1]),
          &(tbl_data->pp_context_fem_field_select[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_context_fem_field_select[2]),
          &(tbl_data->pp_context_fem_field_select[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_context_fem_field_select[3]),
          &(tbl_data->pp_context_fem_field_select[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_context_fem_field_select[4]),
          &(tbl_data->pp_context_fem_field_select[4])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_context_fem_field_select[5]),
          &(tbl_data->pp_context_fem_field_select[5])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pp_context_fem_field_select_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pp_context_fem_field_select_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_context_fem_field_select_map_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_context_fem_field_select[0]),
          &(tbl->pp_context_fem_field_select[0]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_context_fem_field_select[1]),
          &(tbl->pp_context_fem_field_select[1]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_context_fem_field_select[2]),
          &(tbl->pp_context_fem_field_select[2]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_context_fem_field_select[3]),
          &(tbl->pp_context_fem_field_select[3]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_context_fem_field_select[4]),
          &(tbl->pp_context_fem_field_select[4]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_context_fem_field_select[5]),
          &(tbl->pp_context_fem_field_select[5]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pp_context_fem_field_select_map_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_parser_program1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.parser_program1_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->next_addr_base),
          &(tbl_data->next_addr_base)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->macro_sel),
          &(tbl_data->macro_sel)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_parser_program1_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_parser_program1_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.parser_program1_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->next_addr_base),
          &(tbl->next_addr_base),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->macro_sel),
          &(tbl->macro_sel),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_parser_program1_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_parser_program2_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.parser_program2_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->next_addr_base),
          &(tbl_data->next_addr_base)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->macro_sel),
          &(tbl_data->macro_sel)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_parser_program2_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_parser_program2_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.parser_program2_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->next_addr_base),
          &(tbl->next_addr_base),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->macro_sel),
          &(tbl->macro_sel),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_parser_program2_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_packet_format_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.packet_format_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parser_pmf_profile),
          &(tbl_data->parser_pmf_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_program_profile),
          &(tbl_data->key_program_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->packet_format_code),
          &(tbl_data->packet_format_code)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_packet_format_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_packet_format_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.packet_format_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->parser_pmf_profile),
          &(tbl->parser_pmf_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_program_profile),
          &(tbl->key_program_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->packet_format_code),
          &(tbl->packet_format_code),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_packet_format_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_parser_custom_macro_parameters_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.parser_custom_macro_parameters_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cstm_word_select),
          &(tbl_data->cstm_word_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cstm_mask_left),
          &(tbl_data->cstm_mask_left)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cstm_mask_right),
          &(tbl_data->cstm_mask_right)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cstm_condition_select),
          &(tbl_data->cstm_condition_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cstm_comparator_mode),
          &(tbl_data->cstm_comparator_mode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cstm_comparator_mask),
          &(tbl_data->cstm_comparator_mask)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cstm_shift_sel),
          &(tbl_data->cstm_shift_sel)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cstm_shift_a),
          &(tbl_data->cstm_shift_a)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cstm_shift_b),
          &(tbl_data->cstm_shift_b)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_parser_custom_macro_parameters_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.parser_custom_macro_parameters_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->cstm_word_select),
          &(tbl->cstm_word_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cstm_mask_left),
          &(tbl->cstm_mask_left),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cstm_mask_right),
          &(tbl->cstm_mask_right),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cstm_condition_select),
          &(tbl->cstm_condition_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cstm_comparator_mode),
          &(tbl->cstm_comparator_mode),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cstm_comparator_mask),
          &(tbl->cstm_comparator_mask),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cstm_shift_sel),
          &(tbl->cstm_shift_sel),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cstm_shift_a),
          &(tbl->cstm_shift_a),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cstm_shift_b),
          &(tbl->cstm_shift_b),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_parser_eth_protocols_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.parser_eth_protocols_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->eth_type_protocol),
          &(tbl_data->eth_type_protocol)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->eth_sap_protocol),
          &(tbl_data->eth_sap_protocol)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_parser_eth_protocols_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_parser_eth_protocols_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.parser_eth_protocols_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->eth_type_protocol),
          &(tbl->eth_type_protocol),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->eth_sap_protocol),
          &(tbl->eth_sap_protocol),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_parser_eth_protocols_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_parser_ip_protocols_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.parser_ip_protocols_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ip_protocol),
          &(tbl_data->ip_protocol)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_parser_ip_protocols_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_parser_ip_protocols_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.parser_ip_protocols_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ip_protocol),
          &(tbl->ip_protocol),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_parser_ip_protocols_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_parser_custom_macro_protocols_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.parser_custom_macro_protocols_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cstm_protocol),
          &(tbl_data->cstm_protocol)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_parser_custom_macro_protocols_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.parser_custom_macro_protocols_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->cstm_protocol),
          &(tbl->cstm_protocol),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_isem_management_request_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.isem_management_request_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->isem_type),
          &(tbl_data->isem_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->isem_stamp),
          &(tbl_data->isem_stamp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->isem_key),
          (tbl_data->isem_key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->isem_payload),
          &(tbl_data->isem_payload)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->isem_age_status),
          &(tbl_data->isem_age_status)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->isem_self),
          &(tbl_data->isem_self)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->isem_refreshed_by_dsp),
          &(tbl_data->isem_refreshed_by_dsp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_isem_management_request_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_isem_management_request_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.isem_management_request_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->isem_type),
          &(tbl->isem_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->isem_stamp),
          &(tbl->isem_stamp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          (tbl_data->isem_key),
          &(tbl->isem_key),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->isem_payload),
          &(tbl->isem_payload),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->isem_age_status),
          &(tbl->isem_age_status),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->isem_self),
          &(tbl->isem_self),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->isem_refreshed_by_dsp),
          &(tbl->isem_refreshed_by_dsp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_isem_management_request_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vsi_isid_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VSI_ISID_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VSI_ISID_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VSI_ISID_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VSI_ISID_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VSI_ISID_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_VSI_ISID_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vsi_isid_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VSI_ISID_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vsi_or_isid),
          &(tbl_data->vsi_or_isid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vsi_isid_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vsi_isid_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VSI_ISID_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VSI_ISID_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VSI_ISID_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VSI_ISID_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VSI_ISID_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vsi_isid_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->vsi_or_isid),
          &(tbl->vsi_or_isid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VSI_ISID_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vsi_isid_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vsi_my_mac_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VSI_MY_MAC_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VSI_MY_MAC_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VSI_MY_MAC_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_VSI_MY_MAC_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vsi_my_mac_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->my_mac_lsb),
          &(tbl_data->my_mac_lsb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->enable_my_mac),
          &(tbl_data->enable_my_mac)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vsi_my_mac_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vsi_my_mac_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VSI_MY_MAC_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VSI_MY_MAC_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VSI_MY_MAC_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vsi_my_mac_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->my_mac_lsb),
          &(tbl->my_mac_lsb),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->enable_my_mac),
          &(tbl->enable_my_mac),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VSI_MY_MAC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vsi_my_mac_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vsi_topology_id_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vsi_topology_id_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vsi_topology_id),
          &(tbl_data->vsi_topology_id)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vsi_topology_id_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vsi_topology_id_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vsi_topology_id_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->vsi_topology_id),
          &(tbl->vsi_topology_id),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vsi_topology_id_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vsi_fid_class_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VSI_FID_CLASS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vsi_fid_class_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fid_class[0]),
          &(tbl_data->fid_class[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fid_class[1]),
          &(tbl_data->fid_class[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fid_class[2]),
          &(tbl_data->fid_class[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fid_class[3]),
          &(tbl_data->fid_class[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vsi_fid_class_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vsi_fid_class_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VSI_FID_CLASS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vsi_fid_class_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->fid_class[0]),
          &(tbl->fid_class[0]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fid_class[1]),
          &(tbl->fid_class[1]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fid_class[2]),
          &(tbl->fid_class[2]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fid_class[3]),
          &(tbl->fid_class[3]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vsi_fid_class_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vsi_da_not_found_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vsi_da_not_found_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->profile_index[0]),
          &(tbl_data->profile_index[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->profile_index[1]),
          &(tbl_data->profile_index[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->profile_index[2]),
          &(tbl_data->profile_index[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->profile_index[3]),
          &(tbl_data->profile_index[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vsi_da_not_found_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vsi_da_not_found_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vsi_da_not_found_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->profile_index[0]),
          &(tbl->profile_index[0]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->profile_index[1]),
          &(tbl->profile_index[1]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->profile_index[2]),
          &(tbl->profile_index[2]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->profile_index[3]),
          &(tbl->profile_index[3]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vsi_da_not_found_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_bvd_topology_id_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.bvd_topology_id_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bvd_topology_id),
          &(tbl_data->bvd_topology_id)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_bvd_topology_id_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_bvd_topology_id_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.bvd_topology_id_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bvd_topology_id),
          &(tbl->bvd_topology_id),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_bvd_topology_id_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_bvd_fid_class_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_BVD_FID_CLASS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.bvd_fid_class_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fid_class[0]),
          &(tbl_data->fid_class[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fid_class[1]),
          &(tbl_data->fid_class[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fid_class[2]),
          &(tbl_data->fid_class[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fid_class[3]),
          &(tbl_data->fid_class[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_bvd_fid_class_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_bvd_fid_class_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_BVD_FID_CLASS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.bvd_fid_class_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->fid_class[0]),
          &(tbl->fid_class[0]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fid_class[1]),
          &(tbl->fid_class[1]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fid_class[2]),
          &(tbl->fid_class[2]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fid_class[3]),
          &(tbl->fid_class[3]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_bvd_fid_class_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_bvd_da_not_found_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.bvd_da_not_found_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->profile_index[0]),
          &(tbl_data->profile_index[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->profile_index[1]),
          &(tbl_data->profile_index[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->profile_index[2]),
          &(tbl_data->profile_index[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->profile_index[3]),
          &(tbl_data->profile_index[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_bvd_da_not_found_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_bvd_da_not_found_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.bvd_da_not_found_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->profile_index[0]),
          &(tbl->profile_index[0]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->profile_index[1]),
          &(tbl->profile_index[1]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->profile_index[2]),
          &(tbl->profile_index[2]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->profile_index[3]),
          &(tbl->profile_index[3]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_bvd_da_not_found_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_fid_class_2_fid_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.fid_class_2_fid_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fid),
          &(tbl_data->fid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_fid_class_2_fid_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_fid_class_2_fid_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.fid_class_2_fid_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->fid),
          &(tbl->fid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_fid_class_2_fid_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vlan_range_compression_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vlan_range_compression_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_range_upper_limit),
          &(tbl_data->vlan_range_upper_limit)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_range_lower_limit),
          &(tbl_data->vlan_range_lower_limit)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vlan_range_compression_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vlan_range_compression_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vlan_range_compression_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->vlan_range_upper_limit),
          &(tbl->vlan_range_upper_limit),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vlan_range_lower_limit),
          &(tbl->vlan_range_lower_limit),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vlan_range_compression_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vtt_in_pp_port_vlan_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vtt_in_pp_port_vlan_config_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->range_valid0),
          &(tbl_data->range_valid0)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->range_valid1),
          &(tbl_data->range_valid1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vtt_in_pp_port_vlan_config_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vtt_in_pp_port_vlan_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vtt_in_pp_port_vlan_config_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->range_valid0),
          &(tbl->range_valid0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->range_valid1),
          &(tbl->range_valid1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vtt_in_pp_port_vlan_config_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_designated_vlan_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.designated_vlan_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->designated_vlan),
          &(tbl_data->designated_vlan)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_designated_vlan_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_designated_vlan_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.designated_vlan_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->designated_vlan),
          &(tbl->designated_vlan),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_designated_vlan_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vsi_port_membership_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vsi_port_membership_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_port_member_line),
          (tbl_data->vlan_port_member_line)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vsi_port_membership_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vsi_port_membership_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vsi_port_membership_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          (tbl_data->vlan_port_member_line),
          &(tbl->vlan_port_member_line),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vsi_port_membership_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vtt_in_pp_port_config_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->designated_vlan_index),
          &(tbl_data->designated_vlan_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_domain),
          &(tbl_data->vlan_domain)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->default_sem_base),
          &(tbl_data->default_sem_base)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->default_sem_opcode),
          &(tbl_data->default_sem_opcode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->port_is_pbp),
          &(tbl_data->port_is_pbp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->small_em_key_profile),
          &(tbl_data->small_em_key_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->learn_ac),
          &(tbl_data->learn_ac)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vtt_in_pp_port_config_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->designated_vlan_index),
          &(tbl->designated_vlan_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vlan_domain),
          &(tbl->vlan_domain),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->default_sem_base),
          &(tbl->default_sem_base),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->default_sem_opcode),
          &(tbl->default_sem_opcode),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->port_is_pbp),
          &(tbl->port_is_pbp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->small_em_key_profile),
          &(tbl->small_em_key_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->learn_ac),
          &(tbl->learn_ac),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sem_result_table),
          (tbl_data->sem_result_table)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          (tbl_data->sem_result_table),
          &(tbl->sem_result_table),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_ac_p2p_to_ac_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_ac_p2p_to_ac_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif),
          &(tbl_data->out_lif)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_edit_vid),
          &(tbl_data->vlan_edit_vid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_edit_pcp_dei_profile),
          &(tbl_data->vlan_edit_pcp_dei_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_edit_profile),
          &(tbl_data->vlan_edit_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mef_l2_cp_profile),
          &(tbl_data->mef_l2_cp_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cfm_max_level),
          &(tbl_data->cfm_max_level)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cfm_trap_valid),
          &(tbl_data->cfm_trap_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cos_profile),
          &(tbl_data->cos_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->type),
          &(tbl_data->type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_ac_p2p_to_ac_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_ac_p2p_to_ac_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_ac_p2p_to_ac_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_lif),
          &(tbl->out_lif),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vlan_edit_vid),
          &(tbl->vlan_edit_vid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vlan_edit_pcp_dei_profile),
          &(tbl->vlan_edit_pcp_dei_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vlan_edit_profile),
          &(tbl->vlan_edit_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->mef_l2_cp_profile),
          &(tbl->mef_l2_cp_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cfm_max_level),
          &(tbl->cfm_max_level),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cfm_trap_valid),
          &(tbl->cfm_trap_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cos_profile),
          &(tbl->cos_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->type),
          &(tbl->type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_AC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_ac_p2p_to_ac_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pwe_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_ac_p2p_to_pwe_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif_or_vc_label),
          &(tbl_data->out_lif_or_vc_label)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_edit_pcp_dei_profile),
          &(tbl_data->vlan_edit_pcp_dei_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_edit_profile),
          &(tbl_data->vlan_edit_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mef_l2_cp_profile),
          &(tbl_data->mef_l2_cp_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cfm_max_level),
          &(tbl_data->cfm_max_level)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cfm_trap_valid),
          &(tbl_data->cfm_trap_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cos_profile),
          &(tbl_data->cos_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->type),
          &(tbl_data->type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pwe_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pwe_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_ac_p2p_to_pwe_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_lif_or_vc_label),
          &(tbl->out_lif_or_vc_label),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vlan_edit_pcp_dei_profile),
          &(tbl->vlan_edit_pcp_dei_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vlan_edit_profile),
          &(tbl->vlan_edit_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->mef_l2_cp_profile),
          &(tbl->mef_l2_cp_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cfm_max_level),
          &(tbl->cfm_max_level),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cfm_trap_valid),
          &(tbl->cfm_trap_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cos_profile),
          &(tbl->cos_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->type),
          &(tbl->type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PWE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pwe_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pbb_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_ac_p2p_to_pbb_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->isid),
          &(tbl_data->isid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_edit_pcp_dei_profile),
          &(tbl_data->vlan_edit_pcp_dei_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_edit_profile),
          &(tbl_data->vlan_edit_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mef_l2_cp_profile),
          &(tbl_data->mef_l2_cp_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cfm_max_level),
          &(tbl_data->cfm_max_level)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cfm_trap_valid),
          &(tbl_data->cfm_trap_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cos_profile),
          &(tbl_data->cos_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->type),
          &(tbl_data->type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pbb_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pbb_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_ac_p2p_to_pbb_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->isid),
          &(tbl->isid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vlan_edit_pcp_dei_profile),
          &(tbl->vlan_edit_pcp_dei_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vlan_edit_profile),
          &(tbl->vlan_edit_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->mef_l2_cp_profile),
          &(tbl->mef_l2_cp_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cfm_max_level),
          &(tbl->cfm_max_level),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cfm_trap_valid),
          &(tbl->cfm_trap_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cos_profile),
          &(tbl->cos_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->type),
          &(tbl->type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_P2P_TO_PBB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_ac_p2p_to_pbb_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_ac_mp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_ac_mp_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vsi),
          &(tbl_data->vsi)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tt_learn_enable),
          &(tbl_data->tt_learn_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->orientation_is_hub),
          &(tbl_data->orientation_is_hub)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_edit_vid),
          &(tbl_data->vlan_edit_vid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_edit_pcp_dei_profile),
          &(tbl_data->vlan_edit_pcp_dei_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_edit_profile),
          &(tbl_data->vlan_edit_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mef_l2_cp_profile),
          &(tbl_data->mef_l2_cp_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cfm_max_level),
          &(tbl_data->cfm_max_level)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cfm_trap_valid),
          &(tbl_data->cfm_trap_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cos_profile),
          &(tbl_data->cos_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->learn_destination),
          &(tbl_data->learn_destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->type),
          &(tbl_data->type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_ac_mp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_ac_mp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_ac_mp_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->vsi),
          &(tbl->vsi),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tt_learn_enable),
          &(tbl->tt_learn_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->orientation_is_hub),
          &(tbl->orientation_is_hub),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vlan_edit_vid),
          &(tbl->vlan_edit_vid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vlan_edit_pcp_dei_profile),
          &(tbl->vlan_edit_pcp_dei_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vlan_edit_profile),
          &(tbl->vlan_edit_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->mef_l2_cp_profile),
          &(tbl->mef_l2_cp_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cfm_max_level),
          &(tbl->cfm_max_level),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cfm_trap_valid),
          &(tbl->cfm_trap_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cos_profile),
          &(tbl->cos_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->learn_destination),
          &(tbl->learn_destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->type),
          &(tbl->type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_AC_MP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_ac_mp_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_isid_p2p_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_isid_p2p_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vsi),
          &(tbl_data->vsi)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->isidor_out_lifor_vc_label),
          &(tbl_data->isidor_out_lifor_vc_label)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tpid_profile_index),
          &(tbl_data->tpid_profile_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cos_profile),
          &(tbl_data->cos_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->service_type_lsb),
          &(tbl_data->service_type_lsb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->type),
          &(tbl_data->type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_isid_p2p_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_isid_p2p_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_isid_p2p_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->vsi),
          &(tbl->vsi),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->isidor_out_lifor_vc_label),
          &(tbl->isidor_out_lifor_vc_label),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tpid_profile_index),
          &(tbl->tpid_profile_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cos_profile),
          &(tbl->cos_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->service_type_lsb),
          &(tbl->service_type_lsb),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->type),
          &(tbl->type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_P2P_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_isid_p2p_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_isid_mp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_isid_mp_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vsi),
          &(tbl_data->vsi)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination_valid),
          &(tbl_data->destination_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->orientation_is_hub),
          &(tbl_data->orientation_is_hub)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tt_learn_enable),
          &(tbl_data->tt_learn_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tpid_profile),
          &(tbl_data->tpid_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cos_profile),
          &(tbl_data->cos_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->service_type_lsb),
          &(tbl_data->service_type_lsb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->type),
          &(tbl_data->type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_isid_mp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_isid_mp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_isid_mp_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->vsi),
          &(tbl->vsi),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination_valid),
          &(tbl->destination_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->orientation_is_hub),
          &(tbl->orientation_is_hub),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tt_learn_enable),
          &(tbl->tt_learn_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tpid_profile),
          &(tbl->tpid_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cos_profile),
          &(tbl->cos_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->service_type_lsb),
          &(tbl->service_type_lsb),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->type),
          &(tbl->type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_ISID_MP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_isid_mp_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_trill_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_trill_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vsi),
          &(tbl_data->vsi)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tt_learn_enable),
          &(tbl_data->tt_learn_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tpid_profile),
          &(tbl_data->tpid_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->service_type),
          &(tbl_data->service_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->learn_destination),
          &(tbl_data->learn_destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_trill_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_trill_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_trill_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->vsi),
          &(tbl->vsi),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tt_learn_enable),
          &(tbl->tt_learn_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tpid_profile),
          &(tbl->tpid_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->service_type),
          &(tbl->service_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->learn_destination),
          &(tbl->learn_destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_TRILL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_trill_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_ip_tt_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_ip_tt_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->in_rif),
          &(tbl_data->in_rif)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->in_rif_valid),
          &(tbl_data->in_rif_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cos_profile),
          &(tbl_data->cos_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->service_type),
          &(tbl_data->service_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_ip_tt_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_ip_tt_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_ip_tt_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->in_rif),
          &(tbl->in_rif),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->in_rif_valid),
          &(tbl->in_rif_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cos_profile),
          &(tbl->cos_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->service_type),
          &(tbl->service_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_IP_TT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_ip_tt_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_label_pwe_p2p_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_label_pwe_p2p_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vsi),
          &(tbl_data->vsi)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->model_is_pipe),
          &(tbl_data->model_is_pipe)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination_valid),
          &(tbl_data->destination_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tpid_profile),
          &(tbl_data->tpid_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->has_cw),
          &(tbl_data->has_cw)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->valid),
          &(tbl_data->valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->isidor_out_lifor_vc_label),
          &(tbl_data->isidor_out_lifor_vc_label)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->type),
          &(tbl_data->type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_label_pwe_p2p_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_label_pwe_p2p_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_label_pwe_p2p_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->vsi),
          &(tbl->vsi),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->model_is_pipe),
          &(tbl->model_is_pipe),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination_valid),
          &(tbl->destination_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tpid_profile),
          &(tbl->tpid_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->has_cw),
          &(tbl->has_cw),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->valid),
          &(tbl->valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->isidor_out_lifor_vc_label),
          &(tbl->isidor_out_lifor_vc_label),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->type),
          &(tbl->type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_P2P_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_label_pwe_p2p_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_label_pwe_mp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_label_pwe_mp_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vsi),
          &(tbl_data->vsi)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->orientation_is_hub),
          &(tbl_data->orientation_is_hub)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tt_learn_enable),
          &(tbl_data->tt_learn_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tpid_profile),
          &(tbl_data->tpid_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->has_cw),
          &(tbl_data->has_cw)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->valid),
          &(tbl_data->valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->learn_asd),
          &(tbl_data->learn_asd)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->learn_destination),
          &(tbl_data->learn_destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->type),
          &(tbl_data->type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_label_pwe_mp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_label_pwe_mp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_label_pwe_mp_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->vsi),
          &(tbl->vsi),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->orientation_is_hub),
          &(tbl->orientation_is_hub),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tt_learn_enable),
          &(tbl->tt_learn_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tpid_profile),
          &(tbl->tpid_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->has_cw),
          &(tbl->has_cw),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->valid),
          &(tbl->valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->learn_asd),
          &(tbl->learn_asd),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->learn_destination),
          &(tbl->learn_destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->type),
          &(tbl->type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_PWE_MP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_label_pwe_mp_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_label_vrl_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_label_vrl_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->in_rif),
          &(tbl_data->in_rif)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->in_rif_valid),
          &(tbl_data->in_rif_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->model_is_pipe),
          &(tbl_data->model_is_pipe)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->valid),
          &(tbl_data->valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cos_profile),
          &(tbl_data->cos_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->service_type),
          &(tbl_data->service_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->type),
          &(tbl_data->type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_label_vrl_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_label_vrl_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_label_vrl_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->in_rif),
          &(tbl->in_rif),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->in_rif_valid),
          &(tbl->in_rif_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->model_is_pipe),
          &(tbl->model_is_pipe),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->valid),
          &(tbl->valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cos_profile),
          &(tbl->cos_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->service_type),
          &(tbl->service_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->type),
          &(tbl->type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_VRL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_label_vrl_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_label_lsp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_label_lsp_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->in_rif),
          &(tbl_data->in_rif)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->in_rif_valid),
          &(tbl_data->in_rif_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->model_is_pipe),
          &(tbl_data->model_is_pipe)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->valid),
          &(tbl_data->valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cos_profile),
          &(tbl_data->cos_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->service_type),
          &(tbl_data->service_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->type),
          &(tbl_data->type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_label_lsp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_table_label_lsp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_table_label_lsp_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->in_rif),
          &(tbl->in_rif),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->in_rif_valid),
          &(tbl->in_rif_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->model_is_pipe),
          &(tbl->model_is_pipe),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->valid),
          &(tbl->valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cos_profile),
          &(tbl->cos_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->service_type),
          &(tbl->service_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->type),
          &(tbl->type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_TABLE_LABEL_LSP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_table_label_lsp_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_opcode_ip_offsets_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_opcode_ip_offsets_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipv6_sem_offset),
          &(tbl_data->ipv6_sem_offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipv6_add_offset_to_base),
          &(tbl_data->ipv6_add_offset_to_base)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipv6_opcode_valid),
          &(tbl_data->ipv6_opcode_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipv4_sem_offset),
          &(tbl_data->ipv4_sem_offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipv4_add_offset_to_base),
          &(tbl_data->ipv4_add_offset_to_base)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipv4_opcode_valid),
          &(tbl_data->ipv4_opcode_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_opcode_ip_offsets_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_opcode_ip_offsets_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_opcode_ip_offsets_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ipv6_sem_offset),
          &(tbl->ipv6_sem_offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ipv6_add_offset_to_base),
          &(tbl->ipv6_add_offset_to_base),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ipv6_opcode_valid),
          &(tbl->ipv6_opcode_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ipv4_sem_offset),
          &(tbl->ipv4_sem_offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ipv4_add_offset_to_base),
          &(tbl->ipv4_add_offset_to_base),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ipv4_opcode_valid),
          &(tbl->ipv4_opcode_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_opcode_ip_offsets_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_opcode_tc_dp_offsets_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_opcode_tc_dp_offsets_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tc_dp_sem_offset),
          &(tbl_data->tc_dp_sem_offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tc_dp_add_offset_to_base),
          &(tbl_data->tc_dp_add_offset_to_base)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tc_dp_opcode_valid),
          &(tbl_data->tc_dp_opcode_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_opcode_tc_dp_offsets_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_opcode_tc_dp_offsets_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_opcode_tc_dp_offsets_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->tc_dp_sem_offset),
          &(tbl->tc_dp_sem_offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tc_dp_add_offset_to_base),
          &(tbl->tc_dp_add_offset_to_base),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tc_dp_opcode_valid),
          &(tbl->tc_dp_opcode_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_opcode_tc_dp_offsets_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_opcode_pcp_dei_offsets_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_opcode_pcp_dei_offsets_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pcp_dei_sem_offset),
          &(tbl_data->pcp_dei_sem_offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pcp_dei_add_offset_to_base),
          &(tbl_data->pcp_dei_add_offset_to_base)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pcp_dei_opcode_valid),
          &(tbl_data->pcp_dei_opcode_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_opcode_pcp_dei_offsets_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_opcode_pcp_dei_offsets_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_opcode_pcp_dei_offsets_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->pcp_dei_sem_offset),
          &(tbl->pcp_dei_sem_offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pcp_dei_add_offset_to_base),
          &(tbl->pcp_dei_add_offset_to_base),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pcp_dei_opcode_valid),
          &(tbl->pcp_dei_opcode_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_opcode_pcp_dei_offsets_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_stp_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_STP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_STP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_STP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_STP_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_STP_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_STP_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.stp_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_STP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->per_port_stp_state),
          (tbl_data->per_port_stp_state)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_stp_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_stp_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_STP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_STP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_STP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_STP_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_STP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.stp_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          (tbl_data->per_port_stp_state),
          &(tbl->per_port_stp_state),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_STP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_stp_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vrid_my_mac_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vrid_my_mac_map_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vrid_my_mac_map),
          &(tbl_data->vrid_my_mac_map)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vrid_my_mac_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vrid_my_mac_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vrid_my_mac_map_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->vrid_my_mac_map),
          &(tbl->vrid_my_mac_map),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vrid_my_mac_map_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_mpls_label_range_encountered_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.mpls_label_range_encountered_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mpls_label_range_encountered),
          &(tbl_data->mpls_label_range_encountered)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_mpls_label_range_encountered_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_mpls_label_range_encountered_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.mpls_label_range_encountered_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->mpls_label_range_encountered),
          &(tbl->mpls_label_range_encountered),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_mpls_label_range_encountered_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_mpls_tunnel_termination_valid_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.mpls_tunnel_termination_valid_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mpls_tunnel_termination_valid),
          &(tbl_data->mpls_tunnel_termination_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_mpls_tunnel_termination_valid_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_mpls_tunnel_termination_valid_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.mpls_tunnel_termination_valid_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->mpls_tunnel_termination_valid),
          &(tbl->mpls_tunnel_termination_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_mpls_tunnel_termination_valid_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_ip_over_mpls_exp_mapping_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.ip_over_mpls_exp_mapping_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ip_over_mpls_exp_mapping),
          &(tbl_data->ip_over_mpls_exp_mapping)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_ip_over_mpls_exp_mapping_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_ip_over_mpls_exp_mapping_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.ip_over_mpls_exp_mapping_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ip_over_mpls_exp_mapping),
          &(tbl->ip_over_mpls_exp_mapping),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_ip_over_mpls_exp_mapping_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vtt_llvp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VTT_LLVP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VTT_LLVP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VTT_LLVP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VTT_LLVP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VTT_LLVP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_VTT_LLVP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vtt_llvp_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VTT_LLVP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->incoming_tag_structure),
          &(tbl_data->incoming_tag_structure)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pcp_dei_profile),
          &(tbl_data->pcp_dei_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ivec),
          &(tbl_data->ivec)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vtt_llvp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vtt_llvp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VTT_LLVP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VTT_LLVP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VTT_LLVP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VTT_LLVP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VTT_LLVP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vtt_llvp_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->incoming_tag_structure),
          &(tbl->incoming_tag_structure),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pcp_dei_profile),
          &(tbl->pcp_dei_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ivec),
          &(tbl->ivec),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VTT_LLVP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vtt_llvp_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_llvp_prog_sel_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.llvp_prog_sel_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->llvp_prog_sel),
          &(tbl_data->llvp_prog_sel)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_llvp_prog_sel_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_llvp_prog_sel_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.llvp_prog_sel_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->llvp_prog_sel),
          &(tbl->llvp_prog_sel),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_llvp_prog_sel_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vtt1st_key_construction0_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vtt1st_key_construction0_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_inst0),
          &(tbl_data->key_inst0)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_inst1),
          &(tbl_data->key_inst1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vtt1st_key_construction0_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vtt1st_key_construction0_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vtt1st_key_construction0_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_inst0),
          &(tbl->key_inst0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_inst1),
          &(tbl->key_inst1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vtt1st_key_construction0_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vtt1st_key_construction1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vtt1st_key_construction1_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_inst2),
          &(tbl_data->key_inst2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_inst3),
          &(tbl_data->key_inst3)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->and_mask),
          &(tbl_data->and_mask)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->or_mask),
          &(tbl_data->or_mask)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sem_lookup_enable),
          &(tbl_data->sem_lookup_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vtt1st_key_construction1_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vtt1st_key_construction1_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vtt1st_key_construction1_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_inst2),
          &(tbl->key_inst2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_inst3),
          &(tbl->key_inst3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->and_mask),
          &(tbl->and_mask),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->or_mask),
          &(tbl->or_mask),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->sem_lookup_enable),
          &(tbl->sem_lookup_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vtt1st_key_construction1_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vtt1st_key_construction_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_DATA
    data0;
  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_DATA
    data1;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA, 1);

  res = soc_pb_pp_ihp_vtt1st_key_construction0_tbl_get_unsafe(
          unit,
          entry_offset,
          &data0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_ihp_vtt1st_key_construction1_tbl_get_unsafe(
          unit,
          entry_offset,
          &data1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data->key_inst0 = data0.key_inst0;
  tbl_data->key_inst1 = data0.key_inst1;
  tbl_data->key_inst2 = data1.key_inst2;
  tbl_data->key_inst3 = data1.key_inst3;
  tbl_data->and_mask = data1.and_mask;
  tbl_data->or_mask = data1.or_mask;
  tbl_data->sem_lookup_enable = data1.sem_lookup_enable;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vtt1st_key_construction_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vtt1st_key_construction_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_DATA
    data0;
  SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_DATA
    data1;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_SET_UNSAFE);

  data0.key_inst0 = tbl_data->key_inst0;
  data0.key_inst1 = tbl_data->key_inst1;
  data1.key_inst2 = tbl_data->key_inst2;
  data1.key_inst3 = tbl_data->key_inst3;
  data1.and_mask = tbl_data->and_mask;
  data1.or_mask = tbl_data->or_mask;
  data1.sem_lookup_enable = tbl_data->sem_lookup_enable;

  res = soc_pb_pp_ihp_vtt1st_key_construction0_tbl_set_unsafe(
          unit,
          entry_offset,
          &data0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_ihp_vtt1st_key_construction1_tbl_set_unsafe(
          unit,
          entry_offset,
          &data1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vtt1st_key_construction_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vtt2nd_key_construction_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vtt2nd_key_construction_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_inst0),
          &(tbl_data->key_inst0)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_inst1),
          &(tbl_data->key_inst1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->and_mask),
          &(tbl_data->and_mask)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->or_mask),
          &(tbl_data->or_mask)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vtt2nd_key_construction_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vtt2nd_key_construction_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vtt2nd_key_construction_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_inst0),
          &(tbl->key_inst0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_inst1),
          &(tbl->key_inst1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->and_mask),
          &(tbl->and_mask),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->or_mask),
          &(tbl->or_mask),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vtt2nd_key_construction_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_ingress_vlan_edit_command_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.ingress_vlan_edit_command_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ivec),
          &(tbl_data->ivec)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_ingress_vlan_edit_command_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_ingress_vlan_edit_command_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.ingress_vlan_edit_command_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ivec),
          &(tbl->ivec),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_ingress_vlan_edit_command_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vlan_edit_pcp_dei_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vlan_edit_pcp_dei_map_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pcp_dei),
          &(tbl_data->pcp_dei)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vlan_edit_pcp_dei_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_vlan_edit_pcp_dei_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.vlan_edit_pcp_dei_map_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->pcp_dei),
          &(tbl->pcp_dei),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vlan_edit_pcp_dei_map_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pbb_cfm_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pbb_cfm_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pbb_cfm_max_level),
          &(tbl_data->pbb_cfm_max_level)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pbb_cfm_trap_valid),
          &(tbl_data->pbb_cfm_trap_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pbb_cfm_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_pbb_cfm_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pbb_cfm_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->pbb_cfm_max_level),
          &(tbl->pbb_cfm_max_level),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pbb_cfm_trap_valid),
          &(tbl->pbb_cfm_trap_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_pbb_cfm_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_accessed_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_accessed_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sem_result_accessed),
          &(tbl_data->sem_result_accessed)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_accessed_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_sem_result_accessed_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.sem_result_accessed_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->sem_result_accessed),
          &(tbl->sem_result_accessed),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_sem_result_accessed_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_in_rif_config_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.in_rif_config_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vrf),
          &(tbl_data->vrf)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->uc_rpf_enable),
          &(tbl_data->uc_rpf_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->enable_routing_mc),
          &(tbl_data->enable_routing_mc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->enable_routing_uc),
          &(tbl_data->enable_routing_uc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->enable_routing_mpls),
          &(tbl_data->enable_routing_mpls)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->in_rif_cos_profile),
          &(tbl_data->in_rif_cos_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_in_rif_config_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_in_rif_config_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.in_rif_config_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->vrf),
          &(tbl->vrf),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->uc_rpf_enable),
          &(tbl->uc_rpf_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->enable_routing_mc),
          &(tbl->enable_routing_mc),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->enable_routing_uc),
          &(tbl->enable_routing_uc),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->enable_routing_mpls),
          &(tbl->enable_routing_mpls),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->in_rif_cos_profile),
          &(tbl->in_rif_cos_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_in_rif_config_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_tc_dp_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.tc_dp_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->traffic_class),
          &(tbl_data->traffic_class)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->drop_precedence),
          &(tbl_data->drop_precedence)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_tc_dp_map_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihp_tc_dp_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.tc_dp_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->traffic_class),
          &(tbl->traffic_class),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->drop_precedence),
          &(tbl->drop_precedence),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHP_ID,
          SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_tc_dp_map_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_pinfo_fer_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PINFO_FER_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PINFO_FER_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PINFO_FER_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PINFO_FER_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PINFO_FER_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_PINFO_FER_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.pinfo_fer_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PINFO_FER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lag_lb_key_count),
          &(tbl_data->lag_lb_key_count)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lag_lb_key_start),
          &(tbl_data->lag_lb_key_start)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lb_profile),
          &(tbl_data->lb_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ecmp_lb_key_count),
          &(tbl_data->ecmp_lb_key_count)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lb_bos_search),
          &(tbl_data->lb_bos_search)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lb_include_bos_hdr),
          &(tbl_data->lb_include_bos_hdr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_pinfo_fer_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_pinfo_fer_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PINFO_FER_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PINFO_FER_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PINFO_FER_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PINFO_FER_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PINFO_FER_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.pinfo_fer_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->lag_lb_key_count),
          &(tbl->lag_lb_key_count),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lag_lb_key_start),
          &(tbl->lag_lb_key_start),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lb_profile),
          &(tbl->lb_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ecmp_lb_key_count),
          &(tbl->ecmp_lb_key_count),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lb_bos_search),
          &(tbl->lb_bos_search),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lb_include_bos_hdr),
          &(tbl->lb_include_bos_hdr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PINFO_FER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_pinfo_fer_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_ecmp_group_size_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.ecmp_group_size_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ecmp_group_size),
          &(tbl_data->ecmp_group_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_ecmp_group_size_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_ecmp_group_size_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.ecmp_group_size_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ecmp_group_size),
          &(tbl->ecmp_group_size),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_ecmp_group_size_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_lb_pfc_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.lb_pfc_profile_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lb_vector_index3),
          &(tbl_data->lb_vector_index3)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lb_vector_index1),
          &(tbl_data->lb_vector_index1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lb_vector_index5),
          &(tbl_data->lb_vector_index5)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lb_vector_index4),
          &(tbl_data->lb_vector_index4)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lb_vector_index2),
          &(tbl_data->lb_vector_index2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_lb_pfc_profile_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_lb_pfc_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.lb_pfc_profile_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->lb_vector_index3),
          &(tbl->lb_vector_index3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lb_vector_index1),
          &(tbl->lb_vector_index1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lb_vector_index5),
          &(tbl->lb_vector_index5),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lb_vector_index4),
          &(tbl->lb_vector_index4),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lb_vector_index2),
          &(tbl->lb_vector_index2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_lb_pfc_profile_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_lb_vector_program_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.lb_vector_program_map_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->chunk_size),
          &(tbl_data->chunk_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->chunk_bitmap),
          (tbl_data->chunk_bitmap)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_lb_vector_program_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_lb_vector_program_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.lb_vector_program_map_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->chunk_size),
          &(tbl->chunk_size),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          (tbl_data->chunk_bitmap),
          &(tbl->chunk_bitmap),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_lb_vector_program_map_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_fec_super_entry_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fec_super_entry_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->protection_pointer),
          &(tbl_data->protection_pointer)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ecmp_group_size_index),
          &(tbl_data->ecmp_group_size_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_fec_super_entry_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_fec_super_entry_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fec_super_entry_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->protection_pointer),
          &(tbl->protection_pointer),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ecmp_group_size_index),
          &(tbl->ecmp_group_size_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_fec_super_entry_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_fec_entry_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FEC_ENTRY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FEC_ENTRY_TBL
    *tbl;
  SOC_PETRA_TBL_ADDR
    act_addr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FEC_ENTRY_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FEC_ENTRY_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_FEC_ENTRY_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fec_entry_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_PP_TBL_FEC_ODD_ENTRY_ADDR_SET(entry_offset, &(tbl->addr), &act_addr);
  offset = SOC_PB_PP_TBL_FEC_ODD_ENTRY_OFFSET(entry_offset,act_addr);

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(act_addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FEC_ENTRY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fec_entry),
          (tbl_data->fec_entry)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_fec_entry_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_fec_entry_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FEC_ENTRY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FEC_ENTRY_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FEC_ENTRY_TBL
    *tbl;
  SOC_PETRA_TBL_ADDR
    act_addr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FEC_ENTRY_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FEC_ENTRY_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fec_entry_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_PP_TBL_FEC_ODD_ENTRY_ADDR_SET(entry_offset, &(tbl->addr),&act_addr);
  offset = SOC_PB_PP_TBL_FEC_ODD_ENTRY_OFFSET(entry_offset,act_addr);
  
  res = soc_petra_field_in_place_set(
          (tbl_data->fec_entry),
          &(tbl->fec_entry),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(act_addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FEC_ENTRY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_fec_entry_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_fec_entry_general_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL
    *tbl;
  SOC_PETRA_TBL_ADDR
    act_addr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fec_entry_general_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_PP_TBL_FEC_ODD_ENTRY_ADDR_SET(entry_offset, &(tbl->addr),&act_addr);
  offset = SOC_PB_PP_TBL_FEC_ODD_ENTRY_OFFSET(entry_offset,act_addr);
  
  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(act_addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_fec_entry_general_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_fec_entry_general_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL
    *tbl;
  SOC_PETRA_TBL_ADDR
    act_addr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fec_entry_general_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_PP_TBL_FEC_ODD_ENTRY_ADDR_SET(entry_offset, &(tbl->addr),&act_addr);
  offset = SOC_PB_PP_TBL_FEC_ODD_ENTRY_OFFSET(entry_offset,act_addr);
  
  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(act_addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FEC_ENTRY_GENERAL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_fec_entry_general_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_fec_entry_eth_or_trill_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL
    *tbl;
  SOC_PETRA_TBL_ADDR
    act_addr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fec_entry_eth_or_trill_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_PP_TBL_FEC_ODD_ENTRY_ADDR_SET(entry_offset, &(tbl->addr),&act_addr);
  offset = SOC_PB_PP_TBL_FEC_ODD_ENTRY_OFFSET(entry_offset,act_addr);
  
  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(act_addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif_or_dist_tree_nick),
          &(tbl_data->out_lif_or_dist_tree_nick)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->is_out_lif),
          &(tbl_data->is_out_lif)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_fec_entry_eth_or_trill_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_fec_entry_eth_or_trill_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL
    *tbl;
  SOC_PETRA_TBL_ADDR
    act_addr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fec_entry_eth_or_trill_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_PP_TBL_FEC_ODD_ENTRY_ADDR_SET(entry_offset, &(tbl->addr),&act_addr);
  offset = SOC_PB_PP_TBL_FEC_ODD_ENTRY_OFFSET(entry_offset,act_addr);
  
  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_lif_or_dist_tree_nick),
          &(tbl->out_lif_or_dist_tree_nick),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->is_out_lif),
          &(tbl->is_out_lif),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(act_addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FEC_ENTRY_ETH_OR_TRILL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_fec_entry_eth_or_trill_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_fec_entry_ip_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL
    *tbl;
  SOC_PETRA_TBL_ADDR
    act_addr;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fec_entry_ip_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_PP_TBL_FEC_ODD_ENTRY_ADDR_SET(entry_offset, &(tbl->addr),&act_addr);
  offset = SOC_PB_PP_TBL_FEC_ODD_ENTRY_OFFSET(entry_offset,act_addr);

  
  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(act_addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif_lsb),
          &(tbl_data->out_lif_lsb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->uc_rpf_mode),
          &(tbl_data->uc_rpf_mode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mc_rpf_mode),
          &(tbl_data->mc_rpf_mode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->rif),
          &(tbl_data->rif)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif_type),
          &(tbl_data->out_lif_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_fec_entry_ip_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_fec_entry_ip_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL
    *tbl;
  SOC_PETRA_TBL_ADDR
    act_addr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fec_entry_ip_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_PP_TBL_FEC_ODD_ENTRY_ADDR_SET(entry_offset, &(tbl->addr),&act_addr);
  offset = SOC_PB_PP_TBL_FEC_ODD_ENTRY_OFFSET(entry_offset,act_addr);
  
  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_lif_lsb),
          &(tbl->out_lif_lsb),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->uc_rpf_mode),
          &(tbl->uc_rpf_mode),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->mc_rpf_mode),
          &(tbl->mc_rpf_mode),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->rif),
          &(tbl->rif),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_lif_type),
          &(tbl->out_lif_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(act_addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FEC_ENTRY_IP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_fec_entry_ip_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_fec_entry_default_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL
    *tbl;
  SOC_PETRA_TBL_ADDR
    act_addr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fec_entry_default_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_PP_TBL_FEC_ODD_ENTRY_ADDR_SET(entry_offset, &(tbl->addr),&act_addr);
  offset = SOC_PB_PP_TBL_FEC_ODD_ENTRY_OFFSET(entry_offset,act_addr);
  
  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(act_addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif),
          &(tbl_data->out_lif)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_vsi_lsb),
          &(tbl_data->out_vsi_lsb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_fec_entry_default_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_fec_entry_default_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL
    *tbl;
  SOC_PETRA_TBL_ADDR
    act_addr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fec_entry_default_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_PP_TBL_FEC_ODD_ENTRY_ADDR_SET(entry_offset, &(tbl->addr),&act_addr);
  offset = SOC_PB_PP_TBL_FEC_ODD_ENTRY_OFFSET(entry_offset,act_addr);
  
  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_lif),
          &(tbl->out_lif),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_vsi_lsb),
          &(tbl->out_vsi_lsb),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(act_addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FEC_ENTRY_DEFAULT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_fec_entry_default_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_fec_entry_accessed_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL
    *tbl;
  SOC_PETRA_TBL_ADDR
    act_addr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fec_entry_accessed_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_PP_TBL_FEC_ACCESSED_ODD_ENTRY_ADDR_SET(entry_offset, &(tbl->addr),&act_addr);
  offset = SOC_PB_PP_TBL_FEC_ACCESSED_ODD_ENTRY_OFFSET(entry_offset,act_addr);
  
  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(act_addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fec_entry_accessed),
          &(tbl_data->fec_entry_accessed)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_fec_entry_accessed_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_fec_entry_accessed_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL
    *tbl;
  SOC_PETRA_TBL_ADDR
    act_addr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fec_entry_accessed_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_PP_TBL_FEC_ACCESSED_ODD_ENTRY_ADDR_SET(entry_offset, &(tbl->addr),&act_addr);
  offset = SOC_PB_PP_TBL_FEC_ACCESSED_ODD_ENTRY_OFFSET(entry_offset,act_addr);
  
  res = soc_petra_field_in_place_set(
          &(tbl_data->fec_entry_accessed),
          &(tbl->fec_entry_accessed),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(act_addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_fec_entry_accessed_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_path_select_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PATH_SELECT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PATH_SELECT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PATH_SELECT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PATH_SELECT_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PATH_SELECT_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_PATH_SELECT_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.path_select_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PATH_SELECT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->path_select0),
          &(tbl_data->path_select[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->path_select1),
          &(tbl_data->path_select[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->path_select2),
          &(tbl_data->path_select[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->path_select3),
          &(tbl_data->path_select[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->path_select4),
          &(tbl_data->path_select[4])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->path_select5),
          &(tbl_data->path_select[5])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->path_select6),
          &(tbl_data->path_select[6])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->path_select7),
          &(tbl_data->path_select[7])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_path_select_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_path_select_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PATH_SELECT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PATH_SELECT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PATH_SELECT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PATH_SELECT_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PATH_SELECT_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.path_select_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->path_select[0]),
          &(tbl->path_select0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->path_select[1]),
          &(tbl->path_select1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->path_select[2]),
          &(tbl->path_select2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->path_select[3]),
          &(tbl->path_select3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->path_select[4]),
          &(tbl->path_select4),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->path_select[5]),
          &(tbl->path_select5),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->path_select[6]),
          &(tbl->path_select6),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->path_select[7]),
          &(tbl->path_select7),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PATH_SELECT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_path_select_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_destination_status_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_DESTINATION_STATUS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.destination_status_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination_valid),
          &(tbl_data->destination_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_destination_status_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_destination_status_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_DESTINATION_STATUS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.destination_status_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination_valid),
          &(tbl->destination_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_destination_status_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_fwd_act_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fwd_act_profile_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_trap),
          &(tbl_data->fwd_act_trap)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_control),
          &(tbl_data->fwd_act_control)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_destination),
          &(tbl_data->fwd_act_destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_destination_valid),
          &(tbl_data->fwd_act_destination_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_destination_add_vsi),
          &(tbl_data->fwd_act_destination_add_vsi)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_destination_add_vsi_shift),
          &(tbl_data->fwd_act_destination_add_vsi_shift)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_traffic_class),
          &(tbl_data->fwd_act_traffic_class)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_traffic_class_valid),
          &(tbl_data->fwd_act_traffic_class_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_drop_precedence),
          &(tbl_data->fwd_act_drop_precedence)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_drop_precedence_valid),
          &(tbl_data->fwd_act_drop_precedence_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_meter_pointer),
          &(tbl_data->fwd_act_meter_pointer)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_meter_pointer_selector),
          &(tbl_data->fwd_act_meter_pointer_selector)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_counter_pointer),
          &(tbl_data->fwd_act_counter_pointer)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_counter_pointer_selector),
          &(tbl_data->fwd_act_counter_pointer_selector)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_drop_precedence_meter_command),
          &(tbl_data->fwd_act_drop_precedence_meter_command)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_drop_precedence_meter_command_valid),
          &(tbl_data->fwd_act_drop_precedence_meter_command_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_fwd_offset_index),
          &(tbl_data->fwd_act_fwd_offset_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_fwd_offset_index_valid),
          &(tbl_data->fwd_act_fwd_offset_index_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_ethernet_policer_pointer),
          &(tbl_data->fwd_act_ethernet_policer_pointer)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_ethernet_policer_pointer_valid),
          &(tbl_data->fwd_act_ethernet_policer_pointer_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 59, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd_act_learn_disable),
          &(tbl_data->fwd_act_learn_disable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_fwd_act_profile_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_fwd_act_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fwd_act_profile_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_trap),
          &(tbl->fwd_act_trap),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_control),
          &(tbl->fwd_act_control),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_destination),
          &(tbl->fwd_act_destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_destination_valid),
          &(tbl->fwd_act_destination_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_destination_add_vsi),
          &(tbl->fwd_act_destination_add_vsi),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_destination_add_vsi_shift),
          &(tbl->fwd_act_destination_add_vsi_shift),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_traffic_class),
          &(tbl->fwd_act_traffic_class),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_traffic_class_valid),
          &(tbl->fwd_act_traffic_class_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_drop_precedence),
          &(tbl->fwd_act_drop_precedence),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_drop_precedence_valid),
          &(tbl->fwd_act_drop_precedence_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_meter_pointer),
          &(tbl->fwd_act_meter_pointer),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_meter_pointer_selector),
          &(tbl->fwd_act_meter_pointer_selector),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_counter_pointer),
          &(tbl->fwd_act_counter_pointer),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_counter_pointer_selector),
          &(tbl->fwd_act_counter_pointer_selector),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_drop_precedence_meter_command),
          &(tbl->fwd_act_drop_precedence_meter_command),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_drop_precedence_meter_command_valid),
          &(tbl->fwd_act_drop_precedence_meter_command_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_fwd_offset_index),
          &(tbl->fwd_act_fwd_offset_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_fwd_offset_index_valid),
          &(tbl->fwd_act_fwd_offset_index_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_ethernet_policer_pointer),
          &(tbl->fwd_act_ethernet_policer_pointer),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_ethernet_policer_pointer_valid),
          &(tbl->fwd_act_ethernet_policer_pointer_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd_act_learn_disable),
          &(tbl->fwd_act_learn_disable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_fwd_act_profile_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_snoop_action_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_SNOOP_ACTION_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_SNOOP_ACTION_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.snoop_action_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->snoop_action),
          &(tbl_data->snoop_action)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_snoop_action_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_snoop_action_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_SNOOP_ACTION_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_SNOOP_ACTION_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.snoop_action_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->snoop_action),
          &(tbl->snoop_action),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_SNOOP_ACTION_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_snoop_action_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_pinfo_flp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PINFO_FLP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PINFO_FLP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PINFO_FLP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PINFO_FLP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PINFO_FLP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_PINFO_FLP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.pinfo_flp_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PINFO_FLP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_profile_sa_not_found_index),
          &(tbl_data->action_profile_sa_not_found_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->learn_enable),
          &(tbl_data->learn_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_profile_sa_drop_index),
          &(tbl_data->action_profile_sa_drop_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->enable_unicast_same_interface_filter),
          &(tbl_data->enable_unicast_same_interface_filter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program_translation_profile),
          &(tbl_data->program_translation_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sa_lookup_enable),
          &(tbl_data->sa_lookup_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_profile_da_not_found_index),
          &(tbl_data->action_profile_da_not_found_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->transparent_p2p_service_enable),
          &(tbl_data->transparent_p2p_service_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_pinfo_flp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_pinfo_flp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PINFO_FLP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PINFO_FLP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PINFO_FLP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PINFO_FLP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PINFO_FLP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.pinfo_flp_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_profile_sa_not_found_index),
          &(tbl->action_profile_sa_not_found_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->learn_enable),
          &(tbl->learn_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_profile_sa_drop_index),
          &(tbl->action_profile_sa_drop_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->enable_unicast_same_interface_filter),
          &(tbl->enable_unicast_same_interface_filter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->program_translation_profile),
          &(tbl->program_translation_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->sa_lookup_enable),
          &(tbl->sa_lookup_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_profile_da_not_found_index),
          &(tbl->action_profile_da_not_found_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->transparent_p2p_service_enable),
          &(tbl->transparent_p2p_service_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PINFO_FLP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_pinfo_flp_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_translation_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_translation_map_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program11),
          &(tbl_data->program11)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program10),
          &(tbl_data->program10)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program01),
          &(tbl_data->program01)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program00),
          &(tbl_data->program00)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_translation_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_translation_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_translation_map_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program11),
          &(tbl->program11),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->program10),
          &(tbl->program10),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->program01),
          &(tbl->program01),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->program00),
          &(tbl->program00),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_translation_map_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_flp_key_program_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.flp_key_program_map_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_a_instruction0),
          &(tbl_data->key_a_instruction0)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_a_instruction1),
          &(tbl_data->key_a_instruction1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_b_instruction0),
          &(tbl_data->key_b_instruction0)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_b_instruction1),
          &(tbl_data->key_b_instruction1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_b_instruction2),
          &(tbl_data->key_b_instruction2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_b_instruction3),
          &(tbl_data->key_b_instruction3)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->elk_lkp_valid),
          &(tbl_data->elk_lkp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lem_1st_lkp_valid),
          &(tbl_data->lem_1st_lkp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lem_1st_lkp_key_select),
          &(tbl_data->lem_1st_lkp_key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lem_1st_lkp_key_type),
          &(tbl_data->lem_1st_lkp_key_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lem_1st_lkp_and_value),
          &(tbl_data->lem_1st_lkp_and_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lem_1st_lkp_or_value),
          &(tbl_data->lem_1st_lkp_or_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lem_2nd_lkp_valid),
          &(tbl_data->lem_2nd_lkp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lem_2nd_lkp_key_select),
          &(tbl_data->lem_2nd_lkp_key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lem_2nd_lkp_and_value),
          &(tbl_data->lem_2nd_lkp_and_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lem_2nd_lkp_or_value),
          &(tbl_data->lem_2nd_lkp_or_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lpm_1st_lkp_valid),
          &(tbl_data->lpm_1st_lkp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lpm_1st_lkp_and_value),
          &(tbl_data->lpm_1st_lkp_and_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lpm_1st_lkp_or_value),
          &(tbl_data->lpm_1st_lkp_or_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lpm_2nd_lkp_valid),
          &(tbl_data->lpm_2nd_lkp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 59, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lpm_2nd_lkp_and_value),
          &(tbl_data->lpm_2nd_lkp_and_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lpm_2nd_lkp_or_value),
          &(tbl_data->lpm_2nd_lkp_or_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 61, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_key_size),
          &(tbl_data->tcam_key_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_bank_valid),
          &(tbl_data->tcam_bank_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 63, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_and_value),
          &(tbl_data->tcam_and_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 64, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_or_value),
          &(tbl_data->tcam_or_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->data_processing_profile),
          &(tbl_data->data_processing_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 66, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_flp_key_program_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_flp_key_program_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.flp_key_program_map_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_a_instruction0),
          &(tbl->key_a_instruction0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_a_instruction1),
          &(tbl->key_a_instruction1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_b_instruction0),
          &(tbl->key_b_instruction0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_b_instruction1),
          &(tbl->key_b_instruction1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_b_instruction2),
          &(tbl->key_b_instruction2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_b_instruction3),
          &(tbl->key_b_instruction3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->elk_lkp_valid),
          &(tbl->elk_lkp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lem_1st_lkp_valid),
          &(tbl->lem_1st_lkp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lem_1st_lkp_key_select),
          &(tbl->lem_1st_lkp_key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lem_1st_lkp_key_type),
          &(tbl->lem_1st_lkp_key_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lem_1st_lkp_and_value),
          &(tbl->lem_1st_lkp_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lem_1st_lkp_or_value),
          &(tbl->lem_1st_lkp_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lem_2nd_lkp_valid),
          &(tbl->lem_2nd_lkp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lem_2nd_lkp_key_select),
          &(tbl->lem_2nd_lkp_key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lem_2nd_lkp_and_value),
          &(tbl->lem_2nd_lkp_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lem_2nd_lkp_or_value),
          &(tbl->lem_2nd_lkp_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lpm_1st_lkp_valid),
          &(tbl->lpm_1st_lkp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lpm_1st_lkp_and_value),
          &(tbl->lpm_1st_lkp_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lpm_1st_lkp_or_value),
          &(tbl->lpm_1st_lkp_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lpm_2nd_lkp_valid),
          &(tbl->lpm_2nd_lkp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lpm_2nd_lkp_and_value),
          &(tbl->lpm_2nd_lkp_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lpm_2nd_lkp_or_value),
          &(tbl->lpm_2nd_lkp_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tcam_key_size),
          &(tbl->tcam_key_size),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tcam_bank_valid),
          &(tbl->tcam_bank_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tcam_and_value),
          &(tbl->tcam_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tcam_or_value),
          &(tbl->tcam_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data_processing_profile),
          &(tbl->data_processing_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_flp_key_program_map_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_unknown_da_action_profiles_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.unknown_da_action_profiles_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cpu_trap_code_lsb),
          &(tbl_data->cpu_trap_code_lsb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->snp),
          &(tbl_data->snp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fwd),
          &(tbl_data->fwd)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_unknown_da_action_profiles_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_unknown_da_action_profiles_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.unknown_da_action_profiles_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->cpu_trap_code_lsb),
          &(tbl->cpu_trap_code_lsb),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->snp),
          &(tbl->snp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fwd),
          &(tbl->fwd),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_unknown_da_action_profiles_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_vrf_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_VRF_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_VRF_CONFIG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_VRF_CONFIG_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_VRF_CONFIG_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_VRF_CONFIG_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.vrf_config_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_VRF_CONFIG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination),
          &(tbl_data->destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->l3vpn_default_routing),
          &(tbl_data->l3vpn_default_routing)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_vrf_config_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_vrf_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_VRF_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_VRF_CONFIG_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_VRF_CONFIG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_VRF_CONFIG_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_VRF_CONFIG_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.vrf_config_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->l3vpn_default_routing),
          &(tbl->l3vpn_default_routing),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_VRF_CONFIG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_vrf_config_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_header_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_HEADER_PROFILE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_HEADER_PROFILE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.header_profile_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->header_profile_learn_disable),
          &(tbl_data->header_profile_learn_disable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->header_profile_always_add_pph_learn_ext),
          &(tbl_data->header_profile_always_add_pph_learn_ext)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->header_profile_st_vsq_ptr_tc_mode),
          &(tbl_data->header_profile_st_vsq_ptr_tc_mode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->header_profile_build_pph),
          &(tbl_data->header_profile_build_pph)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->header_profile_ftmh_pph_present),
          &(tbl_data->header_profile_ftmh_pph_present)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->header_profile_build_ftmh),
          &(tbl_data->header_profile_build_ftmh)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_header_profile_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_header_profile_tbl2stream_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA  *tbl_data,
    SOC_SAND_IN  SOC_PB_PP_IHB_HEADER_PROFILE_TBL  *tbl,
    SOC_SAND_OUT uint32  *data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_HEADER_PROFILE_TBL2STREAM_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, 1);

  res = soc_petra_field_in_place_set(
          &(tbl_data->header_profile_learn_disable),
          &(tbl->header_profile_learn_disable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->header_profile_always_add_pph_learn_ext),
          &(tbl->header_profile_always_add_pph_learn_ext),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->header_profile_st_vsq_ptr_tc_mode),
          &(tbl->header_profile_st_vsq_ptr_tc_mode),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->header_profile_build_pph),
          &(tbl->header_profile_build_pph),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->header_profile_ftmh_pph_present),
          &(tbl->header_profile_ftmh_pph_present),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

   res = soc_petra_field_in_place_set(
          &(tbl_data->header_profile_build_ftmh),
          &(tbl->header_profile_build_ftmh),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_header_profile_tbl2stream_set_unsafe()", 0, 0);
}


uint32
  soc_pb_pp_ihb_header_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_HEADER_PROFILE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_HEADER_PROFILE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.header_profile_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_pb_pp_ihb_header_profile_tbl2stream_set_unsafe(
          unit,
          tbl_data,
          tbl,
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_HEADER_PROFILE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_header_profile_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_snp_act_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.snp_act_profile_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->snp_act_snp_sampling_probability),
          &(tbl_data->snp_act_snp_sampling_probability)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_snp_act_profile_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_snp_act_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.snp_act_profile_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->snp_act_snp_sampling_probability),
          &(tbl->snp_act_snp_sampling_probability),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_snp_act_profile_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_mrr_act_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.mrr_act_profile_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mrr_act_mrr_sampling_probability),
          &(tbl_data->mrr_act_mrr_sampling_probability)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_mrr_act_profile_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_mrr_act_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.mrr_act_profile_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->mrr_act_mrr_sampling_probability),
          &(tbl->mrr_act_mrr_sampling_probability),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_mrr_act_profile_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_lpm_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_LPM_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_LPM_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_LPM1_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_LPM1_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_LPM_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_LPM_TBL_DATA, 1);

  if (bank_ndx == 0 || bank_ndx > 6)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_TBL_RANGE_OUT_OF_LIMIT, 20, exit);
  }

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  tbl = &(tables->ihb.lpm_tbl[(bank_ndx-1)]);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_LPM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lpm),
          &(tbl_data->lpm)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_lpm1_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_lpm_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_LPM_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_LPM_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_LPM1_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_LPM1_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_LPM_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (bank_ndx == 0 || bank_ndx > 6)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_TBL_RANGE_OUT_OF_LIMIT, 20, exit);
  }

  tbl = &(tables->ihb.lpm_tbl[(bank_ndx-1)]);
  offset = tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->lpm),
          &(tbl->lpm),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_LPM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_lpm1_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_pinfo_pmf_key_gen_var_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.pinfo_pmf_key_gen_var_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pinfo_pmf_key_gen_var),
          &(tbl_data->pinfo_pmf_key_gen_var)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_pinfo_pmf_key_gen_var_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_pinfo_pmf_key_gen_var_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.pinfo_pmf_key_gen_var_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->pinfo_pmf_key_gen_var),
          &(tbl->pinfo_pmf_key_gen_var),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_pinfo_pmf_key_gen_var_tbl_set_unsafe()", entry_offset, 0);
}

/* $Id: pb_pp_tbl_access.c,v 1.9 Broadcom SDK $
 *	Generic access functions
 */

uint32
  soc_pb_pp_ihb_program_selection_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              map_tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_GET_UNSAFE);

  switch(map_tbl_ndx)
  {
  case 0:
    res = soc_pb_pp_ihb_program_selection_map0_tbl_get_unsafe(unit,entry_offset,tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    break;
  case 1:
    res = soc_pb_pp_ihb_program_selection_map1_tbl_get_unsafe(unit,entry_offset,tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
    break;
  case 2:
    res = soc_pb_pp_ihb_program_selection_map2_tbl_get_unsafe(unit,entry_offset,tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    break;
  case 3:
    res = soc_pb_pp_ihb_program_selection_map3_tbl_get_unsafe(unit,entry_offset,tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
    break;
  case 4:
    res = soc_pb_pp_ihb_program_selection_map4_tbl_get_unsafe(unit,entry_offset,tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
    break;
  case 5:
    res = soc_pb_pp_ihb_program_selection_map5_tbl_get_unsafe(unit,entry_offset,tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    break;
  case 6:
    res = soc_pb_pp_ihb_program_selection_map6_tbl_get_unsafe(unit,entry_offset,tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
    break;
  case 7:
    res = soc_pb_pp_ihb_program_selection_map7_tbl_get_unsafe(unit,entry_offset,tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map_tbl_get_unsafe()", entry_offset, map_tbl_ndx);
}

uint32
  soc_pb_pp_ihb_program_selection_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              map_tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_SET_UNSAFE);

  switch(map_tbl_ndx)
  {
  case 0:
    res = soc_pb_pp_ihb_program_selection_map0_tbl_set_unsafe(unit,entry_offset,tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    break;
  case 1:
    res = soc_pb_pp_ihb_program_selection_map1_tbl_set_unsafe(unit,entry_offset,tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
    break;
  case 2:
    res = soc_pb_pp_ihb_program_selection_map2_tbl_set_unsafe(unit,entry_offset,tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    break;
  case 3:
    res = soc_pb_pp_ihb_program_selection_map3_tbl_set_unsafe(unit,entry_offset,tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);
    break;
  case 4:
    res = soc_pb_pp_ihb_program_selection_map4_tbl_set_unsafe(unit,entry_offset,tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
    break;
  case 5:
    res = soc_pb_pp_ihb_program_selection_map5_tbl_set_unsafe(unit,entry_offset,tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    break;
  case 6:
    res = soc_pb_pp_ihb_program_selection_map6_tbl_set_unsafe(unit,entry_offset,tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
    break;
  case 7:
    res = soc_pb_pp_ihb_program_selection_map7_tbl_set_unsafe(unit,entry_offset,tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MAP_TBL_NDX_OUT_OF_RANGE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map_tbl_set_unsafe()", entry_offset, map_tbl_ndx);
}

/*
 *	End of generic functions
 */

uint32
  soc_pb_pp_ihb_program_selection_map0_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_selection_map0_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program_selection_map0),
          &(tbl_data->program_selection_map)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map0_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_selection_map0_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_selection_map0_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program_selection_map),
          &(tbl->program_selection_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP0_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map0_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_selection_map1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_selection_map1_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program_selection_map1),
          &(tbl_data->program_selection_map)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map1_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_selection_map1_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_selection_map1_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program_selection_map),
          &(tbl->program_selection_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map1_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_selection_map2_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_selection_map2_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program_selection_map2),
          &(tbl_data->program_selection_map)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map2_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_selection_map2_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_selection_map2_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program_selection_map),
          &(tbl->program_selection_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP2_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map2_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_selection_map3_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_selection_map3_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program_selection_map3),
          &(tbl_data->program_selection_map)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map3_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_selection_map3_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_selection_map3_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program_selection_map),
          &(tbl->program_selection_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP3_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map3_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_selection_map4_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_selection_map4_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program_selection_map4),
          &(tbl_data->program_selection_map)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map4_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_selection_map4_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_selection_map4_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program_selection_map),
          &(tbl->program_selection_map4),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP4_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map4_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_selection_map5_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_selection_map5_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program_selection_map5),
          &(tbl_data->program_selection_map)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map5_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_selection_map5_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_selection_map5_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program_selection_map),
          &(tbl->program_selection_map5),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP5_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map5_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_selection_map6_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_selection_map6_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program_selection_map6),
          &(tbl_data->program_selection_map)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map6_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_selection_map6_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_selection_map6_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program_selection_map),
          &(tbl->program_selection_map6),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP6_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map6_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_selection_map7_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_selection_map7_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program_selection_map7),
          &(tbl_data->program_selection_map)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map7_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_selection_map7_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_selection_map7_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program_selection_map),
          &(tbl->program_selection_map7),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP7_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_selection_map7_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_vars_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_vars_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->first_pass_key_profile_index),
          &(tbl_data->first_pass_key_profile_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->second_pass_key_profile_index),
          &(tbl_data->second_pass_key_profile_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tag_selection_profile_index),
          &(tbl_data->tag_selection_profile_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bytes_to_remove_header),
          &(tbl_data->bytes_to_remove_header)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bytes_to_remove_offset),
          &(tbl_data->bytes_to_remove_offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->system_header_profile),
          &(tbl_data->system_header_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->copy_program_variable),
          &(tbl_data->copy_program_variable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_vars_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_program_vars_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.program_vars_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->first_pass_key_profile_index),
          &(tbl->first_pass_key_profile_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->second_pass_key_profile_index),
          &(tbl->second_pass_key_profile_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tag_selection_profile_index),
          &(tbl->tag_selection_profile_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bytes_to_remove_header),
          &(tbl->bytes_to_remove_header),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bytes_to_remove_offset),
          &(tbl->bytes_to_remove_offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->system_header_profile),
          &(tbl->system_header_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->copy_program_variable),
          &(tbl->copy_program_variable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_vars_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 *	Generic access function
 */

uint32
  soc_pb_pp_ihb_program_instruction_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pmf_key_ndx,
    SOC_SAND_IN  uint32              pmf_pgm_ndx,
    SOC_SAND_IN  uint32              ce_ndx,
    SOC_SAND_OUT SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    tbl_ndx,
    entry_offset,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_KEY_AB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA
    data;
  uint8
    is_even;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_GET_UNSAFE);

  /*
   *	Set the right entry
   */
  entry_offset = pmf_pgm_ndx;
  tbl_ndx = ce_ndx / SOC_PB_PP_IHB_PGM_INSTR_NOF_CES_PER_TBL;
  if (ce_ndx % SOC_PB_PP_IHB_PGM_INSTR_NOF_CES_PER_TBL == 0)
  {
    is_even = TRUE;
  }
  else
  {
    is_even = FALSE;
  }

  /*
   *	Get the entry data
   */
  if (pmf_key_ndx == 0)
  {
    res = soc_pb_pp_ihb_key_a_program_instruction_table_tbl_get_unsafe(
            unit,
            tbl_ndx,
            entry_offset,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
    res = soc_pb_pp_ihb_key_b_program_instruction_table_tbl_get_unsafe(
            unit,
            tbl_ndx,
            entry_offset,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  /*
   *	Filter the right data
   */
  if (is_even == TRUE)
  {
    tbl_data->inst_bit_count = data.inst0_bit_count;
    tbl_data->inst_header_offset_select = data.inst0_header_offset_select;
    tbl_data->inst_niblle_field_offset = data.inst0_niblle_field_offset;
    tbl_data->inst_source_select = data.inst0_source_select;
    tbl_data->inst_valid = data.inst0_valid;
  }
  else
  {
    tbl_data->inst_bit_count = data.inst1_bit_count;
    tbl_data->inst_header_offset_select = data.inst1_header_offset_select;
    tbl_data->inst_niblle_field_offset = data.inst1_niblle_field_offset;
    tbl_data->inst_source_select = data.inst1_source_select;
    tbl_data->inst_valid = data.inst1_valid;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_instruction_table_tbl_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_ihb_program_instruction_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pmf_key_ndx,
    SOC_SAND_IN  uint32              pmf_pgm_ndx,
    SOC_SAND_IN  uint32              ce_ndx,
    SOC_SAND_IN  SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    tbl_ndx,
    entry_offset,
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_KEY_AB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA
    data;
  uint8
    is_even;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_SET_UNSAFE);

  /*
   *	Set the right entry
   */
  entry_offset = pmf_pgm_ndx;
  tbl_ndx = ce_ndx / SOC_PB_PP_IHB_PGM_INSTR_NOF_CES_PER_TBL;
  if (ce_ndx % SOC_PB_PP_IHB_PGM_INSTR_NOF_CES_PER_TBL == 0)
  {
    is_even = TRUE;
  }
  else
  {
    is_even = FALSE;
  }

  /*
   *	Get the entry data
   */
  if (pmf_key_ndx == 0)
  {
    res = soc_pb_pp_ihb_key_a_program_instruction_table_tbl_get_unsafe(
            unit,
            tbl_ndx,
            entry_offset,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
    res = soc_pb_pp_ihb_key_b_program_instruction_table_tbl_get_unsafe(
            unit,
            tbl_ndx,
            entry_offset,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  /*
   *	Filter the right data
   */
  if (is_even == TRUE)
  {
    data.inst0_bit_count = tbl_data->inst_bit_count;
    data.inst0_header_offset_select = tbl_data->inst_header_offset_select;
    data.inst0_niblle_field_offset = tbl_data->inst_niblle_field_offset;
    data.inst0_source_select = tbl_data->inst_source_select;
    data.inst0_valid = tbl_data->inst_valid;
  }
  else
  {
    data.inst1_bit_count = tbl_data->inst_bit_count;
    data.inst1_header_offset_select = tbl_data->inst_header_offset_select;
    data.inst1_niblle_field_offset = tbl_data->inst_niblle_field_offset;
    data.inst1_source_select = tbl_data->inst_source_select;
    data.inst1_valid = tbl_data->inst_valid;
  }

  /*
   *	Set the entry data
   */
  if (pmf_key_ndx == 0)
  {
    res = soc_pb_pp_ihb_key_a_program_instruction_table_tbl_set_unsafe(
            unit,
            tbl_ndx,
            entry_offset,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else
  {
    res = soc_pb_pp_ihb_key_b_program_instruction_table_tbl_set_unsafe(
            unit,
            tbl_ndx,
            entry_offset,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_program_instruction_table_tbl_set_unsafe()", 0, 0);
}

/*
 *	End of generic access functions
 */

uint32
  soc_pb_pp_ihb_key_a_program_instruction_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_KEY_AB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_KEY_AB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.key_a_program_instruction_table_tbl[tbl_ndx]);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst0_valid),
          &(tbl_data->inst0_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst0_source_select),
          &(tbl_data->inst0_source_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst0_header_offset_select),
          &(tbl_data->inst0_header_offset_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst0_niblle_field_offset),
          &(tbl_data->inst0_niblle_field_offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst0_bit_count),
          &(tbl_data->inst0_bit_count)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst1_valid),
          &(tbl_data->inst1_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst1_source_select),
          &(tbl_data->inst1_source_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst1_header_offset_select),
          &(tbl_data->inst1_header_offset_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst1_niblle_field_offset),
          &(tbl_data->inst1_niblle_field_offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst1_bit_count),
          &(tbl_data->inst1_bit_count)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_key_a_program_instruction_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_key_a_program_instruction_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_KEY_AB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.key_a_program_instruction_table_tbl[tbl_ndx]);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst0_valid),
          &(tbl->inst0_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst0_source_select),
          &(tbl->inst0_source_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst0_header_offset_select),
          &(tbl->inst0_header_offset_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst0_niblle_field_offset),
          &(tbl->inst0_niblle_field_offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst0_bit_count),
          &(tbl->inst0_bit_count),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst1_valid),
          &(tbl->inst1_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst1_source_select),
          &(tbl->inst1_source_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst1_header_offset_select),
          &(tbl->inst1_header_offset_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst1_niblle_field_offset),
          &(tbl->inst1_niblle_field_offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst1_bit_count),
          &(tbl->inst1_bit_count),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_KEY_A_PROGRAM_INSTRUCTION_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_key_a_program_instruction_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_key_b_program_instruction_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_KEY_AB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_KEY_AB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.key_b_program_instruction_table_tbl[tbl_ndx]);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst0_valid),
          &(tbl_data->inst0_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst0_source_select),
          &(tbl_data->inst0_source_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst0_header_offset_select),
          &(tbl_data->inst0_header_offset_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst0_niblle_field_offset),
          &(tbl_data->inst0_niblle_field_offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst0_bit_count),
          &(tbl_data->inst0_bit_count)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst1_valid),
          &(tbl_data->inst1_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst1_source_select),
          &(tbl_data->inst1_source_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst1_header_offset_select),
          &(tbl_data->inst1_header_offset_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst1_niblle_field_offset),
          &(tbl_data->inst1_niblle_field_offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inst1_bit_count),
          &(tbl_data->inst1_bit_count)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_key_b_program_instruction_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_key_b_program_instruction_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_KEY_AB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.key_b_program_instruction_table_tbl[tbl_ndx]);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst0_valid),
          &(tbl->inst0_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst0_source_select),
          &(tbl->inst0_source_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst0_header_offset_select),
          &(tbl->inst0_header_offset_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst0_niblle_field_offset),
          &(tbl->inst0_niblle_field_offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst0_bit_count),
          &(tbl->inst0_bit_count),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst1_valid),
          &(tbl->inst1_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst1_source_select),
          &(tbl->inst1_source_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst1_header_offset_select),
          &(tbl->inst1_header_offset_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst1_niblle_field_offset),
          &(tbl->inst1_niblle_field_offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inst1_bit_count),
          &(tbl->inst1_bit_count),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_KEY_B_PROGRAM_INSTRUCTION_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_key_b_program_instruction_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 *	Generic function to access the TCAM PMF table
 */
uint32
  soc_pb_pp_ihb_tcam_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              db_profile_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_OUT SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
    )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  if (cycle_ndx == 0)
  {
    res = soc_pb_pp_ihb_tcam_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
            unit,
            db_profile_ndx,
            tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
    res = soc_pb_pp_ihb_tcam_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
            unit,
            db_profile_ndx,
            tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_tcam_key_profile_resolved_data_tbl_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_ihb_tcam_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              db_profile_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  if (cycle_ndx == 0)
  {
    res = soc_pb_pp_ihb_tcam_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
            unit,
            db_profile_ndx,
            tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
    res = soc_pb_pp_ihb_tcam_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
            unit,
            db_profile_ndx,
            tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_tcam_key_profile_resolved_data_tbl_set_unsafe()", 0, 0);
}

/*
 *	End of generic function
 */

uint32
  soc_pb_pp_ihb_tcam_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.tcam_1st_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_a_key_select),
          &(tbl_data->bank_key_select[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_a_db_id_and_value),
          &(tbl_data->bank_db_id_and_value[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_a_db_id_or_value),
          &(tbl_data->bank_db_id_or_value[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_b_key_select),
          &(tbl_data->bank_key_select[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_b_db_id_and_value),
          &(tbl_data->bank_db_id_and_value[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_b_db_id_or_value),
          &(tbl_data->bank_db_id_or_value[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_c_key_select),
          &(tbl_data->bank_key_select[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_c_db_id_and_value),
          &(tbl_data->bank_db_id_and_value[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_c_db_id_or_value),
          &(tbl_data->bank_db_id_or_value[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_d_key_select),
          &(tbl_data->bank_key_select[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_d_db_id_and_value),
          &(tbl_data->bank_db_id_and_value[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_d_db_id_or_value),
          &(tbl_data->bank_db_id_or_value[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_pd1_members),
          &(tbl_data->tcam_pd1_members)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_pd2_members),
          &(tbl_data->tcam_pd2_members)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_sel3_member),
          &(tbl_data->tcam_sel3_member)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_sel4_member),
          &(tbl_data->tcam_sel4_member)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_tcam_1st_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_tcam_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.tcam_1st_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_key_select[0]),
          &(tbl->bank_a_key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_db_id_and_value[0]),
          &(tbl->bank_a_db_id_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_db_id_or_value[0]),
          &(tbl->bank_a_db_id_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_key_select[1]),
          &(tbl->bank_b_key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_db_id_and_value[1]),
          &(tbl->bank_b_db_id_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_db_id_or_value[1]),
          &(tbl->bank_b_db_id_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_key_select[2]),
          &(tbl->bank_c_key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_db_id_and_value[2]),
          &(tbl->bank_c_db_id_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_db_id_or_value[2]),
          &(tbl->bank_c_db_id_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_key_select[3]),
          &(tbl->bank_d_key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_db_id_and_value[3]),
          &(tbl->bank_d_db_id_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_db_id_or_value[3]),
          &(tbl->bank_d_db_id_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tcam_pd1_members),
          &(tbl->tcam_pd1_members),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tcam_pd2_members),
          &(tbl->tcam_pd2_members),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tcam_sel3_member),
          &(tbl->tcam_sel3_member),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tcam_sel4_member),
          &(tbl->tcam_sel4_member),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_TCAM_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_tcam_1st_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_tcam_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.tcam_2nd_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_a_key_select),
          &(tbl_data->bank_key_select[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_a_db_id_and_value),
          &(tbl_data->bank_db_id_and_value[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_a_db_id_or_value),
          &(tbl_data->bank_db_id_or_value[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_b_key_select),
          &(tbl_data->bank_key_select[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_b_db_id_and_value),
          &(tbl_data->bank_db_id_and_value[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_b_db_id_or_value),
          &(tbl_data->bank_db_id_or_value[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_c_key_select),
          &(tbl_data->bank_key_select[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_c_db_id_and_value),
          &(tbl_data->bank_db_id_and_value[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_c_db_id_or_value),
          &(tbl_data->bank_db_id_or_value[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_d_key_select),
          &(tbl_data->bank_key_select[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_d_db_id_and_value),
          &(tbl_data->bank_db_id_and_value[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bank_d_db_id_or_value),
          &(tbl_data->bank_db_id_or_value[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_pd1_members),
          &(tbl_data->tcam_pd1_members)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_pd2_members),
          &(tbl_data->tcam_pd2_members)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_sel3_member),
          &(tbl_data->tcam_sel3_member)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_sel4_member),
          &(tbl_data->tcam_sel4_member)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_tcam_2nd_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_tcam_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.tcam_2nd_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_key_select[0]),
          &(tbl->bank_a_key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_db_id_and_value[0]),
          &(tbl->bank_a_db_id_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_db_id_or_value[0]),
          &(tbl->bank_a_db_id_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_key_select[1]),
          &(tbl->bank_b_key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_db_id_and_value[1]),
          &(tbl->bank_b_db_id_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_db_id_or_value[1]),
          &(tbl->bank_b_db_id_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_key_select[2]),
          &(tbl->bank_c_key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_db_id_and_value[2]),
          &(tbl->bank_c_db_id_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_db_id_or_value[2]),
          &(tbl->bank_c_db_id_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_key_select[3]),
          &(tbl->bank_d_key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_db_id_and_value[3]),
          &(tbl->bank_d_db_id_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->bank_db_id_or_value[3]),
          &(tbl->bank_d_db_id_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tcam_pd1_members),
          &(tbl->tcam_pd1_members),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tcam_pd2_members),
          &(tbl->tcam_pd2_members),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tcam_sel3_member),
          &(tbl->tcam_sel3_member),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tcam_sel4_member),
          &(tbl->tcam_sel4_member),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_TCAM_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_tcam_2nd_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_direct_action_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.direct_action_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->direct_action_table),
          &(tbl_data->direct_action_table)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_direct_action_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_direct_action_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.direct_action_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->direct_action_table),
          &(tbl->direct_action_table),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_direct_action_table_tbl_set_unsafe()", entry_offset, 0);
}

/* calculate offset of entry in tcam table based on tcam bank id
 *   and entry offset within a bank
 * 2 bits - bank id (4 banks), 9 bits - offset (512 entries)
 */

STATIC
  uint32
    soc_pb_pp_ihb_tcam_bank_tbl_bank_and_offset_to_offset(
      SOC_SAND_IN uint32 bank_id,
      SOC_SAND_IN uint32 entry_offset
    )
{
  uint32
    res;

  /* 2 bits - bank id (4 banks), 9 bits - offset (512 entries)*/
  res = ((bank_id & 0x3) << 9 ) | (entry_offset & 0x1ff);

  return res;
}

#ifdef SAND_LOW_LEVEL_SIMULATION
STATIC
  uint32
    soc_pb_pp_ihb_tcam_bank_tbl_chip_sim_get(
      SOC_SAND_IN    uint32          offset,
      SOC_SAND_INOUT uint32          data[SOC_PB_PP_IHB_TCAM_BANK_TBL_ENTRY_SIZE]
    )
{
  uint32
    buffer[5],
    input[SOC_PB_PP_IHB_TCAM_BANK_TBL_ENTRY_SIZE],
    res = SOC_SAND_OK;
  uint32
    ndx;
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_TCAM_BANK_IN_TBL
    *tbl_in;
  SOC_PB_PP_IHB_TCAM_BANK_OUT_TBL
    *tbl_out;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_in = &(tables->ihb.tcam_bank_in_tbl);
  tbl_out = &(tables->ihb.tcam_bank_out_tbl);

  for (ndx = 0; ndx < SOC_PB_PP_IHB_TCAM_BANK_TBL_ENTRY_SIZE; ++ndx)
  {
    input[ndx] = data[ndx];
    data[ndx] = 0;
  }

  for (ndx = 0; ndx < 5; ++ndx)
  {
    buffer[ndx] = 0;
  }
  res = soc_petra_field_in_place_get(
          input,
          &(tbl_in->vbi),
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_field_in_place_set(
          buffer,
          &(tbl_out->vbo),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (ndx = 0; ndx < 5; ++ndx)
  {
    buffer[ndx] = 0;
  }
  res = soc_petra_field_in_place_get(
          input,
          &(tbl_in->di),
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          buffer,
          &(tbl_out->dataout),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_set(
          &offset,
          &(tbl_out->hadr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  for (ndx = 0; ndx < 5; ++ndx)
  {
    buffer[ndx] = 0;
  }
  res = soc_petra_field_in_place_get(
          input,
          &(tbl_in->mdi),
          buffer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_field_in_place_set(
          buffer,
          &(tbl_out->mdo),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_tcam_bank_tbl_chip_sim_get()", 0, 0);
}
#endif

/*
 * Read indirect table tcam_bank_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_pb_pp_ihb_tcam_bank_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_TCAM_TBL_GET_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_TCAM_BANK_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_TCAM_BANK_OUT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_TCAM_TBL_GET_UNSAFE);

  if (bank_ndx > SOC_PB_PP_TCAM_NOF_BANKS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBL_RANGE_OUT_OF_LIMIT_ERR, 7, exit);
  }

  res = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = SOC_SAND_OK; sal_memset(
          tbl_data,
          0x0,
          sizeof(SOC_PB_PP_IHB_TCAM_TBL_GET_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.tcam_bank_out_tbl);

  offset = tbl->addr.base + soc_pb_pp_ihb_tcam_bank_tbl_bank_and_offset_to_offset(bank_ndx, entry_offset);


 /*
  * the result already in the indirect read registers
  * read it.
*/
  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_PP_IHB_TCAM_BANK_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


#ifdef SAND_LOW_LEVEL_SIMULATION
  res = soc_pb_pp_ihb_tcam_bank_tbl_chip_sim_get(
          offset - tbl->addr.base,
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
#endif

   res = soc_petra_field_in_place_get(
          data,
          &(tbl->vbo),
          &(tbl_data->vbo)
        );
   SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dataout),
          tbl_data->dataout
        );
   SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->hadr),
          &(tbl_data->hadr)
        );
   SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->hit),
          &(tbl_data->hit)
        );
   SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mhit),
          &(tbl_data->mhit)
        );
   SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mdo),
          tbl_data->mask
        );
   SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_ipv4_stat_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table tcam_bank_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_pb_pp_ihb_tcam_bank_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_TCAM_TBL_SET_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_TCAM_BANK_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_TCAM_BANK_IN_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_TCAM_TBL_SET_UNSAFE);

  if (bank_ndx > SOC_PB_PP_TCAM_NOF_BANKS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBL_RANGE_OUT_OF_LIMIT_ERR, 7, exit);
  }

  res = SOC_SAND_OK; sal_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.tcam_bank_in_tbl);

  offset = tbl->addr.base + soc_pb_pp_ihb_tcam_bank_tbl_bank_and_offset_to_offset(bank_ndx, entry_offset);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vbe),
          &(tbl->vbe),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->mskdatae),
          &(tbl->mskdatae),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->wr),
          &(tbl->wr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->rd),
          &(tbl->rd),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cmp),
          &(tbl->cmp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->flush),
          &(tbl->flush),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_petra_field_in_place_set(
          tbl_data->val,
          &(tbl->di),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vbi),
          &(tbl->vbi),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cfg),
          &(tbl->cfg),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->hqsel),
          &(tbl->hqsel),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

  res = soc_petra_field_in_place_set(
          tbl_data->mask,
          &(tbl->mdi),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);


  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_TCAM_BANK_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_tcam_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table tcam_action_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_pb_pp_ihb_tcam_action_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_IHB_TCAM_ACTION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_TCAM_ACTION_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_TCAM_ACTION_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_TCAM_ACTION_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_IHB_TCAM_ACTION_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.tcam_action_tbl[bank_id]);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_TCAM_ACTION_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->low),
          &(tbl_data->low)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->high),
          &(tbl_data->high)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_tcam_action_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_tcam_action_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_IHB_TCAM_ACTION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_IHB_TCAM_ACTION_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_IHB_TCAM_ACTION_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_TCAM_ACTION_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_IHB_TCAM_ACTION_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.tcam_action_tbl[bank_id]);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->low),
          &(tbl->low),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->high),
          &(tbl->high),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_IHB_ID,
          SOC_PB_PP_IHB_TCAM_ACTION_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_tcam_action_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table ppct_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_pb_pp_egq_ppct_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_PPCT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_PPCT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_PPCT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_PPCT_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_PPCT_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EGQ_PPCT_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.ppct_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_PPCT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_pp_port),
          &(tbl_data->out_pp_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mtu),
          &(tbl_data->mtu)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dst_system_port_id),
          &(tbl_data->dst_system_port_id)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lb_key_max),
          &(tbl_data->lb_key_max)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lb_key_min),
          &(tbl_data->lb_key_min)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->is_stacking_port),
          &(tbl_data->is_stacking_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->peer_tm_domain_id),
          &(tbl_data->peer_tm_domain_id)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->port_type),
          &(tbl_data->port_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cnm_intrcpt_fc_vec_index_5_0),
          &(tbl_data->cnm_intrcpt_fc_vec_index_5_0)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cnm_intrcpt_fc_vec_index_12_6),
          &(tbl_data->cnm_intrcpt_fc_vec_index_12_6)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cnm_intrcpt_fc_vec_index_13_13),
          &(tbl_data->cnm_intrcpt_fc_vec_index_13_13)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cnm_intrcpt_en),
          &(tbl_data->cnm_intrcpt_en)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ad_count_out_port_flag),
          &(tbl_data->ad_count_out_port_flag)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_ppct_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table ppct_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  soc_pb_pp_egq_ppct_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_PPCT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_PPCT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_PPCT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_PPCT_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_PPCT_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.ppct_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_pp_port),
          &(tbl->out_pp_port),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->mtu),
          &(tbl->mtu),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dst_system_port_id),
          &(tbl->dst_system_port_id),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lb_key_max),
          &(tbl->lb_key_max),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lb_key_min),
          &(tbl->lb_key_min),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->is_stacking_port),
          &(tbl->is_stacking_port),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->peer_tm_domain_id),
          &(tbl->peer_tm_domain_id),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->port_type),
          &(tbl->port_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cnm_intrcpt_fc_vec_index_5_0),
          &(tbl->cnm_intrcpt_fc_vec_index_5_0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cnm_intrcpt_fc_vec_index_12_6),
          &(tbl->cnm_intrcpt_fc_vec_index_12_6),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cnm_intrcpt_fc_vec_index_13_13),
          &(tbl->cnm_intrcpt_fc_vec_index_13_13),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cnm_intrcpt_en),
          &(tbl->cnm_intrcpt_en),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ad_count_out_port_flag),
          &(tbl->ad_count_out_port_flag),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_PPCT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_ppct_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_egq_pp_ppct_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_PP_PPCT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_PP_PPCT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_PP_PPCT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_PP_PPCT_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_PP_PPCT_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EGQ_PP_PPCT_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.pp_ppct_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_PP_PPCT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pvlan_port_type),
          &(tbl_data->pvlan_port_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->orientation_is_hub),
          &(tbl_data->orientation_is_hub)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->eei_type),
          &(tbl_data->eei_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->unknown_bc_da_action_filter),
          &(tbl_data->unknown_bc_da_action_filter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->unknown_mc_da_action_filter),
          &(tbl_data->unknown_mc_da_action_filter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->unknown_uc_da_action_filter),
          &(tbl_data->unknown_uc_da_action_filter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->enable_src_equal_dst_filter),
          &(tbl_data->enable_src_equal_dst_filter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->acceptable_frame_type_profile),
          &(tbl_data->acceptable_frame_type_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->egress_vsi_filtering_enable),
          &(tbl_data->egress_vsi_filtering_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->disable_filtering),
          &(tbl_data->disable_filtering)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->acl_profile),
          &(tbl_data->acl_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->acl_data),
          &(tbl_data->acl_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->disable_learning),
          &(tbl_data->disable_learning)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->port_is_pbp),
          &(tbl_data->port_is_pbp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_pp_ppct_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_egq_pp_ppct_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_PP_PPCT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_PP_PPCT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_PP_PPCT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_PP_PPCT_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_PP_PPCT_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.pp_ppct_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->pvlan_port_type),
          &(tbl->pvlan_port_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->orientation_is_hub),
          &(tbl->orientation_is_hub),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->eei_type),
          &(tbl->eei_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->unknown_bc_da_action_filter),
          &(tbl->unknown_bc_da_action_filter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->unknown_mc_da_action_filter),
          &(tbl->unknown_mc_da_action_filter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->unknown_uc_da_action_filter),
          &(tbl->unknown_uc_da_action_filter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->enable_src_equal_dst_filter),
          &(tbl->enable_src_equal_dst_filter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->acceptable_frame_type_profile),
          &(tbl->acceptable_frame_type_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->egress_vsi_filtering_enable),
          &(tbl->egress_vsi_filtering_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->disable_filtering),
          &(tbl->disable_filtering),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->acl_profile),
          &(tbl->acl_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->acl_data),
          &(tbl->acl_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->disable_learning),
          &(tbl->disable_learning),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->port_is_pbp),
          &(tbl->port_is_pbp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_PP_PPCT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_pp_ppct_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_egq_ingress_vlan_edit_command_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.ingress_vlan_edit_command_map_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tpid_profile_link),
          &(tbl_data->tpid_profile_link)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->edit_command_outer_vid_source),
          &(tbl_data->edit_command_outer_vid_source)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->edit_command_outer_pcp_dei_source),
          &(tbl_data->edit_command_outer_pcp_dei_source)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->edit_command_inner_vid_source),
          &(tbl_data->edit_command_inner_vid_source)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->edit_command_inner_pcp_dei_source),
          &(tbl_data->edit_command_inner_pcp_dei_source)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->edit_command_bytes_to_remove),
          &(tbl_data->edit_command_bytes_to_remove)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->outer_tpid_ndx),
          &(tbl_data->outer_tpid_ndx)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inner_tpid_ndx),
          &(tbl_data->inner_tpid_ndx)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_ingress_vlan_edit_command_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_egq_ingress_vlan_edit_command_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.ingress_vlan_edit_command_map_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->tpid_profile_link),
          &(tbl->tpid_profile_link),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->edit_command_outer_vid_source),
          &(tbl->edit_command_outer_vid_source),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->edit_command_outer_pcp_dei_source),
          &(tbl->edit_command_outer_pcp_dei_source),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->edit_command_inner_vid_source),
          &(tbl->edit_command_inner_vid_source),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->edit_command_inner_pcp_dei_source),
          &(tbl->edit_command_inner_pcp_dei_source),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->edit_command_bytes_to_remove),
          &(tbl->edit_command_bytes_to_remove),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->outer_tpid_ndx),
          &(tbl->outer_tpid_ndx),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inner_tpid_ndx),
          &(tbl->inner_tpid_ndx),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_ingress_vlan_edit_command_map_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_egq_vsi_membership_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.vsi_membership_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vsi_membership),
          (tbl_data->vsi_membership)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_vsi_membership_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_egq_vsi_membership_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.vsi_membership_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          (tbl_data->vsi_membership),
          &(tbl->vsi_membership),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_vsi_membership_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_egq_ttl_scope_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_TTL_SCOPE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_TTL_SCOPE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_TTL_SCOPE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EGQ_TTL_SCOPE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.ttl_scope_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ttl_scope),
          &(tbl_data->ttl_scope)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_ttl_scope_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_egq_ttl_scope_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_TTL_SCOPE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_TTL_SCOPE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_TTL_SCOPE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.ttl_scope_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ttl_scope),
          &(tbl->ttl_scope),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_TTL_SCOPE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_ttl_scope_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_egq_aux_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_AUX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_AUX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_AUX_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_AUX_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EGQ_AUX_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.aux_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_AUX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->auxtable),
          &(tbl_data->auxtable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_aux_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_egq_aux_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_AUX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_AUX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_AUX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_AUX_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_AUX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.aux_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->auxtable),
          &(tbl->auxtable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_AUX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_aux_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_egq_eep_orientation_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.eep_orientation_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->eep_orientation),
          &(tbl_data->eep_orientation)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_eep_orientation_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_egq_eep_orientation_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.eep_orientation_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->eep_orientation),
          &(tbl->eep_orientation),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_eep_orientation_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_egq_cfm_trap_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_CFM_TRAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_CFM_TRAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_CFM_TRAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_CFM_TRAP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_CFM_TRAP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EGQ_CFM_TRAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.cfm_trap_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_CFM_TRAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cfm_max_level),
          &(tbl_data->cfm_max_level)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cfm_trap_valid),
          &(tbl_data->cfm_trap_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_cfm_trap_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_egq_cfm_trap_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_CFM_TRAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_CFM_TRAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_CFM_TRAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_CFM_TRAP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_CFM_TRAP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.cfm_trap_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->cfm_max_level),
          &(tbl->cfm_max_level),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cfm_trap_valid),
          &(tbl->cfm_trap_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_CFM_TRAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_cfm_trap_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_egq_action_profile_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.action_profile_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* TBL DEF CHANGE: Replaced single 33b field with actual fields */

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->otm_valid),
          &(tbl_data->otm_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->otm),
          &(tbl_data->otm)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->discard),
          &(tbl_data->discard)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tc),
          &(tbl_data->tc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tc_valid),
          &(tbl_data->tc_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dp),
          &(tbl_data->dp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dp_valid),
          &(tbl_data->dp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cud),
          &(tbl_data->cud)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cud_valid),
          &(tbl_data->cud_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_action_profile_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_egq_action_profile_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.action_profile_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  /* TBL DEF CHANGE: Replaced single 33b field with actual fields */

  res = soc_petra_field_in_place_set(
          &(tbl_data->otm_valid),
          &(tbl->otm_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->otm),
          &(tbl->otm),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->discard),
          &(tbl->discard),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tc),
          &(tbl->tc),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tc_valid),
          &(tbl->tc_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dp),
          &(tbl->dp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dp_valid),
          &(tbl->dp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cud),
          &(tbl->cud),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cud_valid),
          &(tbl->cud_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EGQ_ID,
          SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_egq_action_profile_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.egress_encapsulation_entry_mpls_tunnel_format_tbl);

  if (entry_offset < tables->epni.egress_encapsulation_bank1_tbl.addr.size)
  { /* Entry is in bank1 */
    offset = tables->epni.egress_encapsulation_bank1_tbl.addr.base + entry_offset;
  }
  else if (entry_offset < (tables->epni.egress_encapsulation_bank1_tbl.addr.size + tables->epni.egress_encapsulation_bank2_tbl.addr.size))
  { /* Entry is in bank2 */
    offset = tables->epni.egress_encapsulation_bank2_tbl.addr.base + entry_offset - tables->epni.egress_encapsulation_bank1_tbl.addr.size;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->next_eep),
          &(tbl_data->next_eep)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->next_vsi_lsb),
          &(tbl_data->next_vsi_lsb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mpls2_label),
          &(tbl_data->mpls2_label)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mpls2_command),
          &(tbl_data->mpls2_command)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->has_cw),
          &(tbl_data->has_cw)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tpid_profile),
          &(tbl_data->tpid_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mpls1_label),
          &(tbl_data->mpls1_label)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mpls1_command),
          &(tbl_data->mpls1_command)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ENTRY_SIZE],
    tbl_reps_for_bank2;
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.egress_encapsulation_entry_mpls_tunnel_format_tbl);

  res = soc_petra_field_in_place_set(
          &(tbl_data->next_eep),
          &(tbl->next_eep),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->next_vsi_lsb),
          &(tbl->next_vsi_lsb),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  if (tbl_data->mpls1_command == SOC_PB_MPLS_COMMAND_TYPE_POP_INTO_ETHERNET)
  {
    res = soc_petra_field_in_place_set(
            &(tbl_data->has_cw),
            &(tbl->has_cw),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

    res = soc_petra_field_in_place_set(
            &(tbl_data->tpid_profile),
            &(tbl->tpid_profile),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);
  }
  else if (tbl_data->mpls1_command < SOC_PB_MPLS_COMMAND_TYPE_POP_INTO_MPLS_PIPE)
  {
    /* SOC_PB_PP_MPLS_COMMAND_TYPE_PUSH_0 to 7 */
    res = soc_petra_field_in_place_set(
            &(tbl_data->mpls2_label),
            &(tbl->mpls2_label),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

    res = soc_petra_field_in_place_set(
            &(tbl_data->mpls2_command),
            &(tbl->mpls2_command),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
  }

  res = soc_petra_field_in_place_set(
          &(tbl_data->mpls1_label),
          &(tbl->mpls1_label),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->mpls1_command),
          &(tbl->mpls1_command),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  tbl_reps_for_bank2 = 0;
  if (entry_offset < tables->epni.egress_encapsulation_bank1_tbl.addr.size)
  { /* Entry is in bank1 */
    offset = tables->epni.egress_encapsulation_bank1_tbl.addr.base + entry_offset;

    if (Soc_pb_pp_nof_tbl_reps[unit] >
        (tables->epni.egress_encapsulation_bank1_tbl.addr.base +
         tables->epni.egress_encapsulation_bank1_tbl.addr.size - offset - 1))
    {
      /* Multiple reps, which span accros both banks. Set reps for bank2 as
         the minimun between bank2 size, and the remaing count after writing to
         bank1 */
      tbl_reps_for_bank2 = Soc_pb_pp_nof_tbl_reps[unit];

      Soc_pb_pp_nof_tbl_reps[unit] =
        (tables->epni.egress_encapsulation_bank1_tbl.addr.base +
         tables->epni.egress_encapsulation_bank1_tbl.addr.size - offset - 1);

      tbl_reps_for_bank2 -= Soc_pb_pp_nof_tbl_reps[unit];

      tbl_reps_for_bank2 =
        SOC_SAND_MIN(tbl_reps_for_bank2, tables->epni.egress_encapsulation_bank2_tbl.addr.size);
    }
  }
  else if (entry_offset < (tables->epni.egress_encapsulation_bank1_tbl.addr.size +  tables->epni.egress_encapsulation_bank2_tbl.addr.size))
  { /* Entry is in bank2 */
    offset = tables->epni.egress_encapsulation_bank2_tbl.addr.base + entry_offset - tables->epni.egress_encapsulation_bank1_tbl.addr.size;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }
  
  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  if (tbl_reps_for_bank2 > 0)
  {
    offset = tables->epni.egress_encapsulation_bank2_tbl.addr.base;
    Soc_pb_pp_nof_tbl_reps[unit] = tbl_reps_for_bank2;

    res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_egress_encapsulation_entry_ip_tunnel_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.egress_encapsulation_entry_ip_tunnel_format_tbl);

  if (entry_offset < tables->epni.egress_encapsulation_bank1_tbl.addr.size)
  { /* Entry is in bank1 */
    offset = tables->epni.egress_encapsulation_bank1_tbl.addr.base + entry_offset;
  }
  else if (entry_offset < (tables->epni.egress_encapsulation_bank1_tbl.addr.size + tables->epni.egress_encapsulation_bank2_tbl.addr.size))
  { /* Entry is in bank2 */
    offset = tables->epni.egress_encapsulation_bank2_tbl.addr.base + entry_offset - tables->epni.egress_encapsulation_bank1_tbl.addr.size;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->next_eep),
          &(tbl_data->next_eep)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->next_vsi_lsb),
          &(tbl_data->next_vsi_lsb)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->gre_enable),
          &(tbl_data->gre_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipv4_tos_index),
          &(tbl_data->ipv4_tos_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipv4_ttl_index),
          &(tbl_data->ipv4_ttl_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipv4_src_index),
          &(tbl_data->ipv4_src_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipv4_dst),
          &(tbl_data->ipv4_dst)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_egress_encapsulation_entry_ip_tunnel_format_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_egress_encapsulation_entry_ip_tunnel_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ENTRY_SIZE],
    tbl_reps_for_bank2;
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.egress_encapsulation_entry_ip_tunnel_format_tbl);

  res = soc_petra_field_in_place_set(
          &(tbl_data->next_eep),
          &(tbl->next_eep),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->next_vsi_lsb),
          &(tbl->next_vsi_lsb),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->gre_enable),
          &(tbl->gre_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ipv4_tos_index),
          &(tbl->ipv4_tos_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ipv4_ttl_index),
          &(tbl->ipv4_ttl_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ipv4_src_index),
          &(tbl->ipv4_src_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ipv4_dst),
          &(tbl->ipv4_dst),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  tbl_reps_for_bank2 = 0;
  if (entry_offset < tables->epni.egress_encapsulation_bank1_tbl.addr.size)
  { /* Entry is in bank1 */
    offset = tables->epni.egress_encapsulation_bank1_tbl.addr.base + entry_offset;

    if (Soc_pb_pp_nof_tbl_reps[unit] >
        (tables->epni.egress_encapsulation_bank1_tbl.addr.base +
         tables->epni.egress_encapsulation_bank1_tbl.addr.size - offset - 1))
    {
      /* Multiple reps, which span accros both banks. Set reps for bank2 as
         the minimun between bank2 size, and the remaing count after writing to
         bank1 */
      tbl_reps_for_bank2 = Soc_pb_pp_nof_tbl_reps[unit];

      Soc_pb_pp_nof_tbl_reps[unit] =
        (tables->epni.egress_encapsulation_bank1_tbl.addr.base +
         tables->epni.egress_encapsulation_bank1_tbl.addr.size - offset - 1);

      tbl_reps_for_bank2 -= Soc_pb_pp_nof_tbl_reps[unit];

      tbl_reps_for_bank2 =
        SOC_SAND_MIN(tbl_reps_for_bank2, tables->epni.egress_encapsulation_bank2_tbl.addr.size);
    }
  }
  else if (entry_offset < (tables->epni.egress_encapsulation_bank1_tbl.addr.size +  tables->epni.egress_encapsulation_bank2_tbl.addr.size))
  { /* Entry is in bank2 */
    offset = tables->epni.egress_encapsulation_bank2_tbl.addr.base + entry_offset - tables->epni.egress_encapsulation_bank1_tbl.addr.size;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_TBL_RANGE_OUT_OF_LIMIT, 10, exit);
  }
  
  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  if (tbl_reps_for_bank2 > 0)
  {
    offset = tables->epni.egress_encapsulation_bank2_tbl.addr.base;
    Soc_pb_pp_nof_tbl_reps[unit] = tbl_reps_for_bank2;

    res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_egress_encapsulation_entry_ip_tunnel_format_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_encapsulation_memory_link_layer_entry_format_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL
    *tbl;
  SOC_PETRA_TBL_ADDR 
    table_addr;
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_BANK_TBL
    *db2_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.encapsulation_memory_link_layer_entry_format_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PETRA_COPY(&table_addr,&(tbl->addr),SOC_PETRA_TBL_ADDR,1);
  if (offset >= tbl->addr.base + tbl->addr.size)
  {
    db2_tbl = &(tables->epni.egress_encapsulation_bank2_tbl);

    SOC_PETRA_COPY(&table_addr,&(db2_tbl->addr),SOC_PETRA_TBL_ADDR,1);
    offset = db2_tbl->addr.base + (entry_offset - tbl->addr.size);
  }

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(table_addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vid),
          &(tbl_data->vid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vid_valid),
          &(tbl_data->vid_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dest_mac),
          (tbl_data->dest_mac)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_encapsulation_memory_link_layer_entry_format_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_encapsulation_memory_link_layer_entry_format_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL
    *tbl;
  SOC_PETRA_TBL_ADDR 
    table_addr;
  SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_BANK_TBL
    *db2_tbl;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.encapsulation_memory_link_layer_entry_format_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PETRA_COPY(&table_addr,&(tbl->addr),SOC_PETRA_TBL_ADDR,1);
  if (offset >= tbl->addr.base + tbl->addr.size)
  {
    db2_tbl = &(tables->epni.egress_encapsulation_bank2_tbl);

    SOC_PETRA_COPY(&table_addr,&(db2_tbl->addr),SOC_PETRA_TBL_ADDR,1);
    offset = db2_tbl->addr.base + (entry_offset - tbl->addr.size);
  }

  res = soc_petra_field_in_place_set(
          &(tbl_data->vid),
          &(tbl->vid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vid_valid),
          &(tbl->vid_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          (tbl_data->dest_mac),
          &(tbl->dest_mac),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(table_addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_encapsulation_memory_link_layer_entry_format_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_tx_tag_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.tx_tag_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->entry),
          (tbl_data->entry)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_tx_tag_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_tx_tag_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.tx_tag_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          (tbl_data->entry),
          &(tbl->entry),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_tx_tag_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_stp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_STP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_STP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_STP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_STP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_STP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EPNI_STP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.stp_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_STP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->egress_stp_state),
          (tbl_data->egress_stp_state)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_stp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_stp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_STP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_STP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_STP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_STP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_STP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.stp_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          (tbl_data->egress_stp_state),
          &(tbl->egress_stp_state),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_STP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_stp_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_small_em_result_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.small_em_result_memory_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pcp_dei_profile),
          &(tbl_data->pcp_dei_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /* VID-2 */
  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vid[1]),
          &(tbl_data->vid[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  /* VID-1 */
  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vid[0]),
          &(tbl_data->vid[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_edit_profile),
          &(tbl_data->vlan_edit_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_small_em_result_memory_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_small_em_result_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.small_em_result_memory_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->pcp_dei_profile),
          &(tbl->pcp_dei_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* VID-2 */
  res = soc_petra_field_in_place_set(
          &(tbl_data->vid[1]),
          &(tbl->vid[1]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  /* VID-1 */
  res = soc_petra_field_in_place_set(
          &(tbl_data->vid[0]),
          &(tbl->vid[0]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vlan_edit_profile),
          &(tbl->vlan_edit_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_small_em_result_memory_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_pcp_dei_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.pcp_dei_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pcp_enc_table),
          &(tbl_data->pcp_enc_table)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_pcp_dei_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_pcp_dei_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.pcp_dei_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->pcp_enc_table),
          &(tbl->pcp_enc_table),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_pcp_dei_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_pp_pct_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_PP_PCT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_PP_PCT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_PP_PCT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_PP_PCT_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_PP_PCT_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EPNI_PP_PCT_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.pp_pct_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_PP_PCT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->acceptable_frame_type_profile),
          &(tbl_data->acceptable_frame_type_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->port_is_pbp),
          &(tbl_data->port_is_pbp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->egress_stp_filter_enable),
          &(tbl_data->egress_stp_filter_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->egress_vsi_filter_enable),
          &(tbl_data->egress_vsi_filter_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->disable_filter),
          &(tbl_data->disable_filter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->default_sem_result_0),
          &(tbl_data->default_sem_result_0)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->default_sem_result_14_1),
          &(tbl_data->default_sem_result_14_1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->exp_map_profile),
          &(tbl_data->exp_map_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->c_tpid_index),
          &(tbl_data->c_tpid_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->s_tpid_index),
          &(tbl_data->s_tpid_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tag_profile),
          &(tbl_data->tag_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->vlan_domain),
          &(tbl_data->vlan_domain)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->eei_type),
          &(tbl_data->eei_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cep_c_vlan_edit),
          &(tbl_data->cep_c_vlan_edit)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->llvp_profile),
          &(tbl_data->llvp_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mpls_ethertype_select),
          &(tbl_data->mpls_ethertype_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->count_mode),
          &(tbl_data->count_mode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->count_enable),
          &(tbl_data->count_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->counter_compensation),
          &(tbl_data->counter_compensation)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_pp_pct_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_pp_pct_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_PP_PCT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_PP_PCT_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_PP_PCT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_PP_PCT_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_PP_PCT_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.pp_pct_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->acceptable_frame_type_profile),
          &(tbl->acceptable_frame_type_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->port_is_pbp),
          &(tbl->port_is_pbp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->egress_stp_filter_enable),
          &(tbl->egress_stp_filter_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->egress_vsi_filter_enable),
          &(tbl->egress_vsi_filter_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->disable_filter),
          &(tbl->disable_filter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->default_sem_result_0),
          &(tbl->default_sem_result_0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->default_sem_result_14_1),
          &(tbl->default_sem_result_14_1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->exp_map_profile),
          &(tbl->exp_map_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->c_tpid_index),
          &(tbl->c_tpid_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->s_tpid_index),
          &(tbl->s_tpid_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tag_profile),
          &(tbl->tag_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->vlan_domain),
          &(tbl->vlan_domain),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->eei_type),
          &(tbl->eei_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cep_c_vlan_edit),
          &(tbl->cep_c_vlan_edit),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->llvp_profile),
          &(tbl->llvp_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->mpls_ethertype_select),
          &(tbl->mpls_ethertype_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->count_mode),
          &(tbl->count_mode),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->count_enable),
          &(tbl->count_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->counter_compensation),
          &(tbl->counter_compensation),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_PP_PCT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_pp_pct_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_llvp_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_LLVP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_LLVP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_LLVP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_LLVP_TABLE_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_LLVP_TABLE_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EPNI_LLVP_TABLE_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.llvp_table_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_LLVP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->llvp_c_tag_offset),
          &(tbl_data->llvp_c_tag_offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->llvp_packet_has_c_tag),
          &(tbl_data->llvp_packet_has_c_tag)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->llvp_packet_has_up),
          &(tbl_data->llvp_packet_has_up)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->llvp_packet_has_pcp_dei),
          &(tbl_data->llvp_packet_has_pcp_dei)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->llvp_incoming_tag_format),
          &(tbl_data->llvp_incoming_tag_format)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_llvp_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_llvp_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_LLVP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_LLVP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_LLVP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_LLVP_TABLE_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_LLVP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.llvp_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->llvp_c_tag_offset),
          &(tbl->llvp_c_tag_offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->llvp_packet_has_c_tag),
          &(tbl->llvp_packet_has_c_tag),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->llvp_packet_has_up),
          &(tbl->llvp_packet_has_up),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->llvp_packet_has_pcp_dei),
          &(tbl->llvp_packet_has_pcp_dei),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->llvp_incoming_tag_format),
          &(tbl->llvp_incoming_tag_format),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_LLVP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_llvp_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_egress_edit_cmd_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.egress_edit_cmd_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->outer_tpid_index),
          &(tbl_data->outer_tpid_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inner_tpid_index),
          &(tbl_data->inner_tpid_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->outer_vid_source),
          &(tbl_data->outer_vid_source)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inner_vid_source),
          &(tbl_data->inner_vid_source)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->outer_pcp_dei_source),
          &(tbl_data->outer_pcp_dei_source)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inner_pcp_dei_source),
          &(tbl_data->inner_pcp_dei_source)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tags_to_remove),
          &(tbl_data->tags_to_remove)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_egress_edit_cmd_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_egress_edit_cmd_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.egress_edit_cmd_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->outer_tpid_index),
          &(tbl->outer_tpid_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inner_tpid_index),
          &(tbl->inner_tpid_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->outer_vid_source),
          &(tbl->outer_vid_source),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inner_vid_source),
          &(tbl->inner_vid_source),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->outer_pcp_dei_source),
          &(tbl->outer_pcp_dei_source),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inner_pcp_dei_source),
          &(tbl->inner_pcp_dei_source),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tags_to_remove),
          &(tbl->tags_to_remove),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_egress_edit_cmd_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_program_vars_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_PROGRAM_VARS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.program_vars_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program_index),
          &(tbl_data->program_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->new_header_size),
          &(tbl_data->new_header_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->add_network_header),
          &(tbl_data->add_network_header)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->remove_network_header),
          &(tbl_data->remove_network_header)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->system_header_size),
          &(tbl_data->system_header_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_program_vars_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_program_vars_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_PROGRAM_VARS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.program_vars_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program_index),
          &(tbl->program_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->new_header_size),
          &(tbl->new_header_size),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->add_network_header),
          &(tbl->add_network_header),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->remove_network_header),
          &(tbl->remove_network_header),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->system_header_size),
          &(tbl->system_header_size),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_program_vars_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_ingress_vlan_edit_command_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.ingress_vlan_edit_command_map_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_EPNI_ID,
          SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tpid_profile_link),
          &(tbl_data->tpid_profile_link)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->edit_command_outer_vid_source),
          &(tbl_data->edit_command_outer_vid_source)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->edit_command_outer_pcp_dei_source),
          &(tbl_data->edit_command_outer_pcp_dei_source)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->edit_command_inner_vid_source),
          &(tbl_data->edit_command_inner_vid_source)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->edit_command_inner_pcp_dei_source),
          &(tbl_data->edit_command_inner_pcp_dei_source)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->edit_command_bytes_to_remove),
          &(tbl_data->edit_command_bytes_to_remove)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->outer_tpid_ndx),
          &(tbl_data->outer_tpid_ndx)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->inner_tpid_ndx),
          &(tbl_data->inner_tpid_ndx)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_ingress_vlan_edit_command_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_ingress_vlan_edit_command_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.ingress_vlan_edit_command_map_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->tpid_profile_link),
          &(tbl->tpid_profile_link),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->edit_command_outer_vid_source),
          &(tbl->edit_command_outer_vid_source),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->edit_command_outer_pcp_dei_source),
          &(tbl->edit_command_outer_pcp_dei_source),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->edit_command_inner_vid_source),
          &(tbl->edit_command_inner_vid_source),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->edit_command_inner_pcp_dei_source),
          &(tbl->edit_command_inner_pcp_dei_source),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->edit_command_bytes_to_remove),
          &(tbl->edit_command_bytes_to_remove),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->outer_tpid_ndx),
          &(tbl->outer_tpid_ndx),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->inner_tpid_ndx),
          &(tbl->inner_tpid_ndx),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_EPNI_ID,
          SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_ingress_vlan_edit_command_map_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_ip_exp_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_IP_EXP_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.ip_exp_map_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ip_exp_map),
          &(tbl_data->ip_exp_map)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_ip_exp_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_ip_exp_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_IP_EXP_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.ip_exp_map_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ip_exp_map),
          &(tbl->ip_exp_map),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_ip_exp_map_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_etpp_debug_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_ETPP_DEBUG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_GET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_ENTRY_SIZE);
  SOC_PETRA_CLEAR(tbl_data, SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_DATA, 1);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.etpp_debug_tbl);

  offset = tbl->addr.base + entry_offset;

  res = soc_pb_pp_tbl_read_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->etpp_debug),
          &(tbl_data->etpp_debug)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_etpp_debug_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_epni_etpp_debug_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset,
    data[SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_ENTRY_SIZE];
  SOC_PB_PP_TBLS
    *tables = NULL;
  SOC_PB_PP_EPNI_ETPP_DEBUG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_SET_UNSAFE);

  SOC_PETRA_CLEAR(data, uint32, SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_ENTRY_SIZE);

  res = soc_pb_pp_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.etpp_debug_tbl);

  offset =  tbl->addr.base + entry_offset;

  res = soc_petra_field_in_place_set(
          &(tbl_data->etpp_debug),
          &(tbl->etpp_debug),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_tbl_write_unsafe(
          unit,
          data,
          offset,
          &(tbl->addr),
          sizeof(data),
          SOC_PB_PP_EPNI_ID,
          SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_epni_etpp_debug_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_pp_ihb_tcam_tbl_read_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                           entry_offset,
    SOC_SAND_OUT   SOC_PB_PP_IHB_TCAM_BANK_TBL_DATA    *tcam_data
  )
{
  uint32
    res = SOC_SAND_OK,
    data_indx,
    nof_words;
  uint32
    hw_offset;
  SOC_PB_PP_IHB_TCAM_TBL_SET_DATA
    tcam_in_data;
  SOC_PB_PP_IHB_TCAM_TBL_GET_DATA
    tcam_out_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHP_TCAM_TBL_READ_UNSAFE);

  res = SOC_SAND_OK; sal_memset(
          tcam_data,
          0x0,
          sizeof(SOC_PB_PP_IHB_TCAM_BANK_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = SOC_SAND_OK; sal_memset(
          &tcam_in_data,
          0x0,
          sizeof(SOC_PB_PP_IHB_TCAM_TBL_SET_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

 /*
  * we are reading
  */
  tcam_in_data.rd = 1;
 /*
  * Reading data and valid bit
  */
  tcam_in_data.vbe = 1;
  tcam_in_data.mskdatae = 1;
 /*
  * the type of the bank width
  */

 /*
  * in soc_petra the only use of the TACM for 144 bits word (one word per line) 77 bits words (2 words per line)
  */
  if (nof_entries_in_line == 0 || nof_entries_in_line > 2)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBL_RANGE_OUT_OF_LIMIT_ERR, 17, exit);
  }
  tcam_in_data.cfg = nof_entries_in_line;
 /*
  * in case there is two words per line which word to use.
  */
  if (entry_offset >= nof_entries_in_line * SOC_PB_PP_TCAM_NOF_LINES)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBL_RANGE_OUT_OF_LIMIT_ERR, 27, exit);
  }
 /*
  * low word or high word
  */
  if (nof_entries_in_line == 1)
  {
    tcam_in_data.hqsel = SOC_PB_PP_TCAM_ALL_WORD_MASK;
    hw_offset          = entry_offset;
  }
  else
  {
    if (entry_offset % 2 == 0)
    {
      tcam_in_data.hqsel  = SOC_PB_PP_TCAM_LOW_WORD_MASK;
    }
    else
    {
      tcam_in_data.hqsel  = SOC_PB_PP_TCAM_HIGH_WORD_MASK;
    }
    hw_offset = entry_offset >> 1;
  }

#ifdef SAND_LOW_LEVEL_SIMULATION
#else
 /*
  * perform the read command
  */
  res = soc_pb_pp_ihb_tcam_bank_tbl_set_unsafe(
          unit,
          bank_ndx,
          hw_offset,
          &tcam_in_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
#endif
 /*
  * get the result
  */
  res = soc_pb_pp_ihb_tcam_bank_tbl_get_unsafe(
          unit,
          bank_ndx,
          hw_offset,
          &tcam_out_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

 /*
  * now set the value and the mask.
  */
 /*
  * DI = val & mask
  * MDI = DI XOR mask.
  */
  nof_words = (SOC_PB_PP_IHB_TCAM_DATA_WIDTH + (nof_entries_in_line -  1)) / nof_entries_in_line;

  tcam_data->valid = tcam_out_data.vbo;

  for (data_indx = 0; data_indx < nof_words; ++data_indx)
  {
    tcam_data->value[data_indx] = tcam_out_data.dataout[data_indx];
    tcam_data->mask[data_indx] = tcam_out_data.dataout[data_indx] ^ tcam_out_data.mask[data_indx];
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_tcam_tbl_read_unsafe()",0,0);
}

/*
 * Write to Tcam Banks from block IHB,
 * takes semaphore
 */
uint32
  soc_pb_pp_ihb_tcam_tbl_write_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                            entry_offset,
    SOC_SAND_IN   SOC_PB_PP_IHB_TCAM_BANK_TBL_DATA        *tcam_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data_indx,
    try_indx,
    nof_words,
    hw_offset;
  SOC_PB_PP_IHB_TCAM_TBL_SET_DATA
    tcam_in_data;
  uint8
    written;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_TCAM_TBL_WRITE_UNSAFE);

  res = SOC_SAND_OK; sal_memset(
          &tcam_in_data,
          0x0,
          sizeof(SOC_PB_PP_IHB_TCAM_TBL_SET_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 /*
  * we are writing
  */
  tcam_in_data.wr = 1;
 /*
  * writing data and valid bit
  */
  tcam_in_data.vbe = 1;
  tcam_in_data.mskdatae = 1;
 /*
  * write valid entry
  */
  tcam_in_data.vbi = tcam_data->valid;
 /*
  * the type of the bank width
  */

 /*
  * in soc_petra the only use of the TACM for 144 bits word (one word per line) 77 bits words (2 words per line)
  */
  if (nof_entries_in_line == 0 || nof_entries_in_line > 2)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBL_RANGE_OUT_OF_LIMIT_ERR, 17, exit);
  }
  tcam_in_data.cfg = nof_entries_in_line;
 /*
  * in case there is two words per line which word to use.
  */
  if (entry_offset >= nof_entries_in_line * SOC_PB_PP_TCAM_NOF_LINES)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBL_RANGE_OUT_OF_LIMIT_ERR, 27, exit);
  }
 /*
  * low word or high word
  */
  if (nof_entries_in_line == 1)
  {
    tcam_in_data.hqsel  = SOC_PB_PP_TCAM_ALL_WORD_MASK;
    hw_offset = entry_offset;
  }
  else
  {
    if (entry_offset % 2 == 0)
    {
      tcam_in_data.hqsel  = SOC_PB_PP_TCAM_LOW_WORD_MASK;
    }
    else
    {
      tcam_in_data.hqsel  = SOC_PB_PP_TCAM_HIGH_WORD_MASK;
    }
    hw_offset = entry_offset >> 1;
  }

 /*
  * now set the value and the mask.
  */
 /*
  * DI = val & mask
  * MDI = DI XOR mask.
  */
  nof_words = (SOC_PB_PP_IHB_TCAM_DATA_WIDTH + (nof_entries_in_line -  1)) / nof_entries_in_line;

  for (data_indx = 0; data_indx < nof_words; ++data_indx)
  {
    tcam_in_data.val[data_indx] = tcam_data->value[data_indx] & tcam_data->mask[data_indx];
    tcam_in_data.mask[data_indx] = tcam_in_data.val[data_indx] ^ tcam_data->mask[data_indx];
  }

  written = FALSE;
  try_indx = 0;

  while (try_indx< 10 && !written)
  {

    res = soc_pb_pp_ihb_tcam_bank_tbl_set_unsafe(
            unit,
            bank_ndx,
            hw_offset,
            &tcam_in_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_tcam_tbl_write_unsafe()",0,0);
}

/*
 * Set the valid bit in the TCAM
 */
uint32
  soc_pb_pp_ihb_tcam_valid_bit_tbl_set_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                            entry_offset,
    SOC_SAND_IN  uint8                            valid
  )
{
  uint32
    res = SOC_SAND_OK,
    hw_offset;
  SOC_PB_PP_IHB_TCAM_TBL_SET_DATA
    tcam_in_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_TCAM_VALID_BIT_TBL_SET_UNSAFE);

  res = SOC_SAND_OK; sal_memset(
          &tcam_in_data,
          0x0,
          sizeof(SOC_PB_PP_IHB_TCAM_TBL_SET_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 /*
  * we are writing
  */
  tcam_in_data.wr = 1;
 /*
  * writing only the valid bit
  */
  tcam_in_data.vbe = 1;
 /*
  * write valid entry
  */
  tcam_in_data.vbi = valid;
 /*
  * the type of the bank width
  */

 /*
  * in soc_petra the only use of the TACM for 144 bits word (one word per line) 77 bits words (2 words per line)
  */
  if (nof_entries_in_line == 0 || nof_entries_in_line > 2)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBL_RANGE_OUT_OF_LIMIT_ERR, 17, exit);
  }
  tcam_in_data.cfg = nof_entries_in_line;
 /*
  * in case there is two words per line which word to use.
  */
  if (entry_offset >= nof_entries_in_line * SOC_PB_PP_TCAM_NOF_LINES)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBL_RANGE_OUT_OF_LIMIT_ERR, 27, exit);
  }
 /*
  * low word or high word
  */
  if (nof_entries_in_line == 1)
  {
    tcam_in_data.hqsel  = SOC_PB_PP_TCAM_ALL_WORD_MASK;
    hw_offset           = entry_offset;
  }
  else
  {
    if (entry_offset % 2 == 0)
    {
      tcam_in_data.hqsel  = SOC_PB_PP_TCAM_LOW_WORD_MASK;
    }
    else
    {
      tcam_in_data.hqsel  = SOC_PB_PP_TCAM_HIGH_WORD_MASK;
    }
    hw_offset = entry_offset >> 1;
  }

 /*
  * now set the value and the mask.
  */

  res = soc_pb_pp_ihb_tcam_bank_tbl_set_unsafe(
          unit,
          bank_ndx,
          hw_offset,
          &tcam_in_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_tcam_valid_bit_tbl_set_unsafe()",0,0);
}

uint32
  soc_pb_pp_ihb_tcam_valid_bit_tbl_get_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                            entry_offset,
    SOC_SAND_OUT  uint8                           *valid
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_TCAM_TBL_SET_DATA
    tcam_in_data;
  SOC_PB_PP_IHB_TCAM_TBL_GET_DATA
    tcam_out_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_TCAM_VALID_BIT_TBL_GET_UNSAFE);

  res = SOC_SAND_OK; sal_memset(
          &tcam_in_data,
          0x0,
          sizeof(SOC_PB_PP_IHB_TCAM_TBL_SET_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 /*
  * we are reading
  */
  tcam_in_data.rd = 1;
 /*
  * Reading valid bit
  */
  tcam_in_data.vbe = 1;
 /*
  * the type of the bank width
  */

 /*
  * in soc_petra the only use of the TACM for 144 bits word (one word per line) 77 bits words (2 words per line)
  */
  if (nof_entries_in_line == 0 || nof_entries_in_line > 2)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBL_RANGE_OUT_OF_LIMIT_ERR, 27, exit);
  }
  tcam_in_data.cfg = nof_entries_in_line;
 /*
  * in case there is two words per line which word to use.
  */
  if (entry_offset >= nof_entries_in_line * SOC_PB_PP_TCAM_NOF_LINES)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBL_RANGE_OUT_OF_LIMIT_ERR, 37, exit);
  }
 /*
  * low word or high word
  */
  if (nof_entries_in_line == 1)
  {
    tcam_in_data.hqsel  = SOC_PB_PP_TCAM_ALL_WORD_MASK;
  }
  else
  {
    if (entry_offset % 2 == 0)
    {
      tcam_in_data.hqsel  = SOC_PB_PP_TCAM_LOW_WORD_MASK;
    }
    else
    {
      tcam_in_data.hqsel  = SOC_PB_PP_TCAM_HIGH_WORD_MASK;
    }
  }

 /*
  * perform the read command
  */
  res = soc_pb_pp_ihb_tcam_bank_tbl_set_unsafe(
          unit,
          bank_ndx,
          entry_offset,
          &tcam_in_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
 /*
  * get the result
  */
  res = soc_pb_pp_ihb_tcam_bank_tbl_get_unsafe(
          unit,
          bank_ndx,
          entry_offset,
          &tcam_out_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

 /*
  * now set the value and the mask.
  */

  *valid  = SOC_SAND_NUM2BOOL(tcam_out_data.vbo);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_tcam_valid_bit_tbl_get_unsafe()",0,0);
}

/*
 * Get indirect table Tcam Bank A from block IHB,
 * takes semaphore
 */
uint32
  soc_pb_pp_ihb_tcam_tbl_flush_unsafe(
    SOC_SAND_IN   int          unit,
    SOC_SAND_IN   uint32          bank_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_PP_IHB_TCAM_TBL_SET_DATA
    tcam_in_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_TCAM_TBL_FLUSH_UNSAFE);

  res = SOC_SAND_OK; sal_memset(
          &tcam_in_data,
          0x0,
          sizeof(SOC_PB_PP_IHB_TCAM_TBL_SET_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
 /*
  * command is FLUSH
  */
  tcam_in_data.flush = 1;

  res = soc_pb_pp_ihb_tcam_bank_tbl_set_unsafe(
          unit,
          bank_ndx,
          0,
          &tcam_in_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_tcam_tbl_flush_unsafe()",0,0);
}

/*
 *  Compare operation for a single TCAM bank. Note that only 72b and 144b are supported by this
 *  operation.
 */
uint32
  soc_pb_pp_ihb_tcam_tbl_compare_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   SOC_PB_PP_IHB_TCAM_BANK_TBL_DATA     *compare_data,
    SOC_SAND_OUT  SOC_PB_PP_IHB_TCAM_COMPARE_DATA      *found_data
  )
{
  uint32
    res,
    nof_words,
    data_indx;
  SOC_PB_PP_IHB_TCAM_TBL_SET_DATA
    tcam_in_data;
  SOC_PB_PP_IHB_TCAM_TBL_GET_DATA
    tcam_out_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_IHB_TCAM_TBL_COMPARE_UNSAFE);

  res = SOC_SAND_OK; sal_memset(
          &tcam_in_data,
          0x0,
          sizeof(SOC_PB_PP_IHB_TCAM_TBL_SET_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 /*
  * we are comparing
  */
  tcam_in_data.cmp = 1;

 /*
  * in soc_petra the only use of the TACM for 144 bits word (one word per line) 77 bits words (2 words per line)
  */
  if (nof_entries_in_line == 0 || nof_entries_in_line > 2)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBL_RANGE_OUT_OF_LIMIT_ERR, 17, exit);
  }
  tcam_in_data.cfg = nof_entries_in_line;

 /*
  * DI = val
  * MDI = mask.
  */
  nof_words = (SOC_PB_PP_IHB_TCAM_DATA_WIDTH + (nof_entries_in_line -  1)) / nof_entries_in_line;

  for (data_indx = 0; data_indx < nof_words; ++data_indx)
  {
    tcam_in_data.val[data_indx] = compare_data->value[data_indx] & compare_data->mask[data_indx];
    tcam_in_data.mask[data_indx] = compare_data->mask[data_indx];
  }

 /*
  * perform the compare command
  */
  res = soc_pb_pp_ihb_tcam_bank_tbl_set_unsafe(
          unit,
          bank_ndx,
          0,
          &tcam_in_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
 /*
  * get the result
  */
  res = soc_pb_pp_ihb_tcam_bank_tbl_get_unsafe(
          unit,
          bank_ndx,
          0,
          &tcam_out_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

 /*
  * now set the value and the mask.
  */

  found_data->found = tcam_out_data.hit;
  found_data->address = tcam_out_data.hadr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihb_tcam_tbl_compare_unsafe()",0,0);
}

/* } */

                                                                    
#include <soc/dpp/SAND/Utils/sand_footer.h>
                                                                    
